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

#include <QApplication>
#include <unistd.h>

#include "SyntroLib.h"
#include "SyntroLCam.h"
#include "SyntroLCamConsole.h"


int runGuiApp(int argc, char **);
int runConsoleApp(int argc, char **);
QSettings *loadSettings(QStringList arglist);


int main(int argc, char *argv[])
{
	if (checkConsoleModeFlag(argc, argv))
		return runConsoleApp(argc, argv);
	else
		return runGuiApp(argc, argv);
}

int runGuiApp(int argc, char **argv)
{
	QApplication a(argc, argv);

	QSettings *settings = loadSettings(a.arguments());

	SyntroLCam *w = new SyntroLCam(settings);

	w->show();

	return a.exec();
}

int runConsoleApp(int argc, char **argv)
{
	QCoreApplication a(argc, argv);

	bool daemonMode = checkDaemonModeFlag(argc, argv);

	QSettings *settings = loadSettings(a.arguments());

	SyntroLCamConsole console(settings, daemonMode, &a);

	return a.exec();
}

QSettings *loadSettings(QStringList arglist)
{
	char host[32];

	QSettings *settings = loadStandardSettings(PRODUCT_TYPE, arglist);

	// app-specific defaults

	settings->setValue(SYNTRO_PARAMS_COMPTYPE, "SyntroLCam");

	settings->beginGroup("Camera");

	if (!settings->contains(SYNTRO_CAMERA_STREAMNAME)
			|| settings->value(SYNTRO_CAMERA_STREAMNAME).toString().length() == 0) {
		memset(host, 0, sizeof(host));

		if (!gethostname(host, sizeof(host) - 1))
			settings->setValue(SYNTRO_CAMERA_STREAMNAME, host);
		else
			settings->setValue(SYNTRO_CAMERA_STREAMNAME, "Video");
	}

	if (!settings->contains(SYNTRO_CAMERA_CAMERA))
		settings->setValue(SYNTRO_CAMERA_CAMERA, 0);

	if (!settings->contains(SYNTRO_CAMERA_WIDTH))
		settings->setValue(SYNTRO_CAMERA_WIDTH, 640);

	if (!settings->contains(SYNTRO_CAMERA_HEIGHT))
		settings->setValue(SYNTRO_CAMERA_HEIGHT, 480);

	if (!settings->contains(SYNTRO_CAMERA_FRAMERATE))
		settings->setValue(SYNTRO_CAMERA_FRAMERATE, 30);

	if (!settings->contains(SYNTRO_CAMERA_FORMAT))
		settings->setValue(SYNTRO_CAMERA_FORMAT, "MJPG");

	settings->endGroup();

	return settings;
}
