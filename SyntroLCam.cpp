//
//  Copyright (c) 2013 Pansenti, LLC.
//
//  This file is part of Syntro
//
//  Syntro is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Syntro is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Syntro.  If not, see <http://www.gnu.org/licenses/>.
//

#include <QMessageBox>
#include "SyntroLCam.h"

#define FRAME_RATE_TIMER_INTERVAL 2

SyntroLCam::SyntroLCam()
    : QMainWindow()
{
	ui.setupUi(this);

	m_frameCount = 0;
	m_frameRateTimer = 0;
	m_frameRefreshTimer = 0;
	m_camera = NULL;
	m_pixelFormat = 0;

	layoutStatusBar();

	ui.cameraView->setText("Camera Image");

	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionStart, SIGNAL(triggered()), this, SLOT(onStart()));
	connect(ui.actionStop, SIGNAL(triggered()), this, SLOT(onStop()));

	ui.actionStop->setEnabled(false);
	ui.actionStart->setEnabled(true);

    SyntroUtils::syntroAppInit();
    m_client = new CamClient(this);
	m_client->resumeThread();
	
	restoreWindowState();

	setWindowTitle(QString("%1 - %2")
                   .arg(SyntroUtils::getAppType())
                   .arg(SyntroUtils::getAppName()));

	m_frameRateTimer = startTimer(FRAME_RATE_TIMER_INTERVAL * 1000);

	startVideo();
}

SyntroLCam::~SyntroLCam()
{
	if (m_camera) {
		delete m_camera;
		m_camera = NULL;
	}

	clearQueue();
}

void SyntroLCam::closeEvent(QCloseEvent *)
{
	stopVideo();

	clearQueue();

	if (m_frameRateTimer) {
		killTimer(m_frameRateTimer);
		m_frameRateTimer = 0;
	}

	if (m_frameRefreshTimer) {
		killTimer(m_frameRefreshTimer);
		m_frameRefreshTimer = 0;
	}

    m_client->exitThread();

	saveWindowState();
    SyntroUtils::syntroAppExit();
}

bool SyntroLCam::createCamera()
{
	if (m_camera) {
		delete m_camera;
		m_camera = NULL;
	}

    m_camera = new V4LCam();

	if (!m_camera)
		return false;

	return true;
}

void SyntroLCam::onStart()
{
	m_imgSize.setWidth(-1);
	m_imgSize.setHeight(-1);

	startVideo();
}

void SyntroLCam::onStop()
{
	stopVideo();
}

void SyntroLCam::startVideo()
{
	if (!m_camera) {
		if (!createCamera()) {
			QMessageBox::warning(this, "SyntroLCam", "Error allocating camera", QMessageBox::Ok);
			return;
		}
	}

	clearQueue();

	connect(m_camera, SIGNAL(cameraState(QString)), this, SLOT(cameraState(QString)), Qt::DirectConnection);
	connect(m_camera, SIGNAL(pixelFormat(quint32)), this, SLOT(pixelFormat(quint32)));
	connect(m_camera, SIGNAL(pixelFormat(quint32)), m_client, SLOT(pixelFormat(quint32)));
	connect(m_camera, SIGNAL(frameSize(int,int)), this, SLOT(frameSize(int,int)));
	connect(m_camera, SIGNAL(frameSize(int,int)), m_client, SLOT(frameSize(int,int)));

	connect(m_camera, SIGNAL(newJPEG(QByteArray)), this, SLOT(newJPEG(QByteArray)), Qt::DirectConnection);
	connect(m_camera, SIGNAL(newJPEG(QByteArray)), m_client, SLOT(newJPEG(QByteArray)), Qt::DirectConnection);
	connect(m_camera, SIGNAL(newImage(QImage)), this, SLOT(newImage(QImage)), Qt::DirectConnection);
	connect(m_camera, SIGNAL(newImage(QImage)), m_client, SLOT(newImage(QImage)), Qt::DirectConnection);

    connect(this, SIGNAL(startCapture()), m_camera, SLOT(startCapture()), Qt::QueuedConnection);
    connect(this, SIGNAL(stopCapture()), m_camera, SLOT(stopCapture()), Qt::QueuedConnection);

    emit startCapture();
	m_frameCount = 0;
	m_frameRefreshTimer = startTimer(10);
	ui.actionStart->setEnabled(false);
	ui.actionStop->setEnabled(true);
}

void SyntroLCam::stopVideo()
{
	if (m_camera) {
		disconnect(m_camera, SIGNAL(newJPEG(QByteArray)), this, SLOT(newJPEG(QByteArray)));
		disconnect(m_camera, SIGNAL(newJPEG(QByteArray)), m_client, SLOT(newJPEG(QByteArray)));
		disconnect(m_camera, SIGNAL(newImage(QImage)), this, SLOT(newImage(QImage)));
		disconnect(m_camera, SIGNAL(newImage(QImage)), m_client, SLOT(newImage(QImage)));

		disconnect(m_camera, SIGNAL(cameraState(QString)), this, SLOT(cameraState(QString)));
		disconnect(m_camera, SIGNAL(pixelFormat(quint32)), this, SLOT(pixelFormat(quint32)));
		disconnect(m_camera, SIGNAL(pixelFormat(quint32)), m_client, SLOT(pixelFormat(quint32)));
		disconnect(m_camera, SIGNAL(frameSize(int,int)), this, SLOT(frameSize(int,int)));
		disconnect(m_camera, SIGNAL(frameSize(int,int)), m_client, SLOT(frameSize(int,int)));

        disconnect(this, SIGNAL(startCapture()), m_camera, SLOT(startCapture()));
        disconnect(this, SIGNAL(stopCapture()), m_camera, SLOT(stopCapture()));

        emit stopCapture();
        m_camera->exitThread();                             // let the old thread die and delete itself
		m_camera = NULL;
	}

	if (m_frameRefreshTimer) {
		killTimer(m_frameRefreshTimer);
		m_frameRefreshTimer = 0;
	}

	ui.actionStop->setEnabled(false);
	ui.actionStart->setEnabled(true);
}

void SyntroLCam::cameraState(QString state)
{
	m_cameraState = state;
}

void SyntroLCam::pixelFormat(quint32 format)
{
	m_pixelFormat = format;
}

void SyntroLCam::frameSize(int width, int height)
{
	m_imgSize.setWidth(width);
	m_imgSize.setHeight(height);
}

void SyntroLCam::newJPEG(QByteArray frame)
{
	m_frameCount++;

	if (m_frameQMutex.tryLock()) {
		if (m_jpegFrameQ.empty())
			m_jpegFrameQ.enqueue(frame);

		m_frameQMutex.unlock();
	}
}

void SyntroLCam::newImage(QImage img)
{
	m_frameCount++;

	if (m_frameQMutex.tryLock()) {
		if (m_imgFrameQ.empty())
			m_imgFrameQ.enqueue(img);

		m_frameQMutex.unlock();
	}
}

void SyntroLCam::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_frameRateTimer) {
		if (m_cameraState == "Running") {
			m_frameRateStatus->setText(QString().sprintf("%0.1lf fps",
				(double)m_frameCount / FRAME_RATE_TIMER_INTERVAL));

			m_frameCount = 0;

			if (m_pixelFormat == V4L2_PIX_FMT_MJPEG)
				m_rawImageFormat->setText("MJPG");
			else
				m_rawImageFormat->setText("YUYV");                            
		}
		else {
			m_frameRateStatus->setText(m_cameraState);
			m_rawImageFormat->setText("");
		}
	}
	else {
		processFrameQueue();
	}
}

void SyntroLCam::processFrameQueue()
{
	QByteArray frame;
	QImage img;

	bool isJpeg = false;

	m_frameQMutex.lock();

	if (!m_jpegFrameQ.empty()) {
		frame = m_jpegFrameQ.dequeue();
		isJpeg = true;
	}
	else if (!m_imgFrameQ.empty()) {
		img = m_imgFrameQ.dequeue();
	}

	m_frameQMutex.unlock();

	if (isMinimized())
		return;

	if (isJpeg)
		showJPEG(frame);
	else
		showImage(img);
}

void SyntroLCam::showJPEG(QByteArray frame)
{  
	if (frame.size() > 0) {
		QImage img;
		img.loadFromData(frame, "JPEG");
		showImage(img);
	}
}

void SyntroLCam::showImage(QImage img)
{
	if (img.isNull())
		return;

	QImage scaledImg = img.scaled(ui.cameraView->size(), Qt::KeepAspectRatio);

	if (!scaledImg.isNull())
		ui.cameraView->setPixmap(QPixmap::fromImage(scaledImg));
}

void SyntroLCam::clearQueue()
{
	m_frameQMutex.lock();
	m_jpegFrameQ.clear();
	m_imgFrameQ.clear();
	m_frameQMutex.unlock();
}

void SyntroLCam::layoutStatusBar()
{
	m_controlStatus = new QLabel(this);
	m_controlStatus->setAlignment(Qt::AlignLeft);
	ui.statusBar->addWidget(m_controlStatus, 1);

	m_rawImageFormat = new QLabel(this);
	m_rawImageFormat->setAlignment(Qt::AlignLeft);
	ui.statusBar->addWidget(m_rawImageFormat);

	m_frameRateStatus = new QLabel(this);
	m_frameRateStatus->setAlignment(Qt::AlignCenter | Qt::AlignLeft);
	ui.statusBar->addWidget(m_frameRateStatus);
}

void SyntroLCam::saveWindowState()
{
    QSettings *settings = SyntroUtils::getSettings();

    settings->beginGroup("Window");
    settings->setValue("Geometry", saveGeometry());
    settings->setValue("State", saveState());
    settings->endGroup();

    delete settings;
}

void SyntroLCam::restoreWindowState()
{
    QSettings *settings = SyntroUtils::getSettings();

    settings->beginGroup("Window");
    restoreGeometry(settings->value("Geometry").toByteArray());
    restoreState(settings->value("State").toByteArray());
    settings->endGroup();

    delete settings;
}

