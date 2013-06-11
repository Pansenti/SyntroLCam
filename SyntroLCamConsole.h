//
//  Copyright (c) 2012 Pansenti, LLC.
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

#ifndef SYNTROLCAMCONSOLE_H
#define SYNTROLCAMCONSOLE_H

#include <QThread>
#include "SyntroLCam.h"


class SyntroLCamConsole : public QThread
{
	Q_OBJECT

public:
    SyntroLCamConsole(QSettings *settings, bool daemonMode, QObject *parent);

public slots:
	void cameraState(QString);
	void newFrame();
	void aboutToQuit();

protected:
	void run();
	void timerEvent(QTimerEvent *event);

private:
	bool createCamera();
	bool startVideo();
	void stopVideo();
	void showHelp();
	void showStatus();

	void runConsole();
	void runDaemon();

	void registerSigHandler();
	static void sigHandler(int sig);

	QSettings *m_settings;

	CamClient *m_client;
	V4LCam *m_camera;

	QString m_cameraState;
	int m_frameCount;
	int m_frameRateTimer;
	double m_frameRate;
	bool m_daemonMode;
	static volatile bool sigIntReceived;
};

#endif // SYNTROLCAMCONSOLE_H

