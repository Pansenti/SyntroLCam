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

#ifndef SYNTROLCAM_H
#define SYNTROLCAM_H

#include <QtGui>
#include "ui_SyntroLCam.h"

#include "V4LCam.h"
#include "CamClient.h"

#define PRODUCT_TYPE "SyntroLCam"


class SyntroLCam : public QMainWindow
{
	Q_OBJECT

public:
    SyntroLCam();
	~SyntroLCam();

public slots:
	void onStart();
	void onStop();
	void cameraState(QString state);
	void pixelFormat(quint32 format);
    void videoFormat(int width, int height, int frameRate);
	void newJPEG(QByteArray);
	void newImage(QImage);

signals:
    void startCapture();
    void stopCapture();

protected:
	void timerEvent(QTimerEvent *event);
	void closeEvent(QCloseEvent *event);

private:
	void startVideo();
	void stopVideo();
	void processFrameQueue();
	void showJPEG(QByteArray frame);
	void showImage(QImage img);
	bool createCamera();
	void clearQueue();
	void layoutStatusBar();
	void saveWindowState();
	void restoreWindowState();

	Ui::SyntroLCamClass ui;

	QLabel *m_rawImageFormat;
	QLabel *m_frameRateStatus;
	QLabel *m_controlStatus;

	CamClient *m_client;
	V4LCam *m_camera;
	QString m_cameraState;
	QMutex m_frameQMutex;
	QQueue <QByteArray> m_jpegFrameQ;
	QQueue <QImage> m_imgFrameQ;

	int m_frameRateTimer;
	int m_frameRefreshTimer;
	int m_frameCount;
	QSize m_imgSize;
	quint32 m_pixelFormat;
};

#endif // SYNTROLCAM_H

