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

#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "SyntroLCamConsole.h"
#include "V4LCam.h"

#define FRAME_RATE_TIMER_INTERVAL 3

volatile bool SyntroLCamConsole::sigIntReceived = false;

SyntroLCamConsole::SyntroLCamConsole(QSettings *settings, bool daemonMode, QObject *parent)
	: QThread(parent)
{
	m_settings = settings;
	m_daemonMode = daemonMode;

	m_frameRate = 0.0;
	m_frameCount = 0;
	m_frameRateTimer = 0;
	m_camera = NULL;
	m_client = NULL;

	if (m_daemonMode) {
		registerSigHandler();
		
		if (daemon(1, 1)) {
			perror("daemon");
			return;
		}
	}

	connect((QCoreApplication *)parent, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));

    SyntroUtils::syntroAppInit(m_settings);

	m_client = new CamClient(this, m_settings);
	m_client->resumeThread();

	start();
}

void SyntroLCamConsole::aboutToQuit()
{
	for (int i = 0; i < 5; i++) {
		if (wait(1000))
			break;

		if (!m_daemonMode)
			printf("Waiting for console thread to finish...\n");
	}
}

bool SyntroLCamConsole::createCamera()
{
	if (m_camera) {
		delete m_camera;
		m_camera = NULL;
	}

	m_camera = new V4LCam(m_settings);

	if (!m_camera)
		return false;

	return true;
}

bool SyntroLCamConsole::startVideo()
{
	if (!m_camera) {
		if (!createCamera()) {
			logError("Error allocating camera");
			return false;
		}
	}

	if (!m_daemonMode) {
		connect(m_camera, SIGNAL(cameraState(QString)), this, SLOT(cameraState(QString)), Qt::DirectConnection);
		connect(m_camera, SIGNAL(newFrame()), this, SLOT(newFrame()), Qt::DirectConnection);
	}

	connect(m_camera, SIGNAL(pixelFormat(quint32)), m_client, SLOT(pixelFormat(quint32)));
	connect(m_camera, SIGNAL(frameSize(int,int)), m_client, SLOT(frameSize(int,int)));
	connect(m_camera, SIGNAL(newJPEG(QByteArray)), m_client, SLOT(newJPEG(QByteArray)), Qt::DirectConnection);
	connect(m_camera, SIGNAL(newImage(QImage)), m_client, SLOT(newImage(QImage)), Qt::DirectConnection);

	m_camera->startCapture();

	if (!m_daemonMode) {
		m_frameCount = 0;
		m_frameRateTimer = startTimer(FRAME_RATE_TIMER_INTERVAL * 1000);
	}

	return true;
}

void SyntroLCamConsole::stopVideo()
{
	if (m_frameRateTimer) {
		killTimer(m_frameRateTimer);
		m_frameRateTimer = 0;
	}

	if (m_camera) {
		if (!m_daemonMode) {
			disconnect(m_camera, SIGNAL(cameraState(QString)), this, SLOT(cameraState(QString)));
			disconnect(m_camera, SIGNAL(newFrame()), this, SLOT(newFrame()));
		}

		disconnect(m_camera, SIGNAL(pixelFormat(quint32)), m_client, SLOT(pixelFormat(quint32)));
		disconnect(m_camera, SIGNAL(frameSize(int,int)), m_client, SLOT(frameSize(int,int)));
		disconnect(m_camera, SIGNAL(newJPEG(QByteArray)), m_client, SLOT(newJPEG(QByteArray)));
		disconnect(m_camera, SIGNAL(newImage(QImage)), m_client, SLOT(newImage(QImage)));

		m_camera->stopCapture();
		delete m_camera;
		m_camera = NULL;
	}
}

void SyntroLCamConsole::cameraState(QString state)
{
	m_cameraState = state;
}

void SyntroLCamConsole::newFrame()
{
	m_frameCount++;
}

void SyntroLCamConsole::timerEvent(QTimerEvent *)
{
	m_frameRate =  (double)m_frameCount / (double)FRAME_RATE_TIMER_INTERVAL;
	m_frameCount = 0;
}

void SyntroLCamConsole::showHelp()
{
	printf("\nOptions are:\n\n");
	printf("  h - Show help\n");
	printf("  s - Show status\n");
	printf("  x - Exit\n");
}

void SyntroLCamConsole::showStatus()
{    
	printf("\nStatus: %s\n", qPrintable(m_client->getLinkState()));

	if (m_cameraState == "Running")
		printf("Frame rate: %0.1lf fps\n", m_frameRate);
	else
		printf("Camera state: %s\n", qPrintable(m_cameraState));
}

void SyntroLCamConsole::run()
{
	if (m_daemonMode)
		runDaemon();
	else
		runConsole();

	m_client->exitThread();
    SyntroUtils::syntroAppExit();
	QCoreApplication::exit();
}

void SyntroLCamConsole::runConsole()
{
	struct termios	ctty;

	tcgetattr(fileno(stdout), &ctty);
	ctty.c_lflag &= ~(ICANON);
	tcsetattr(fileno(stdout), TCSANOW, &ctty);

	bool grabbing = startVideo();

	while (grabbing) {
		printf("\nEnter option: ");

		switch (tolower(getchar())) {
		case 'h':
			showHelp();
			break;

		case 's':
			showStatus();
			break;

		case 'x':
			printf("\nExiting\n");
			stopVideo();
			grabbing = false;		
			break;

		case '\n':
			continue;
		}
	}
}

void SyntroLCamConsole::runDaemon()
{
	startVideo();

	while (!SyntroLCamConsole::sigIntReceived)
		msleep(100);
}

void SyntroLCamConsole::registerSigHandler()
{
	struct sigaction sia;

	bzero(&sia, sizeof sia);
	sia.sa_handler = SyntroLCamConsole::sigHandler;

	if (sigaction(SIGINT, &sia, NULL) < 0)
		perror("sigaction(SIGINT)");
}

void SyntroLCamConsole::sigHandler(int)
{
	SyntroLCamConsole::sigIntReceived = true;
}

