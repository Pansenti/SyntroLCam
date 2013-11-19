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

#ifndef CAMCLIENT_H
#define CAMCLIENT_H

#include "SyntroLib.h"


//		Settings keys

#define	SYNTRO_CAMERA_CAMERA		"device"
#define	SYNTRO_CAMERA_WIDTH			"width"
#define	SYNTRO_CAMERA_HEIGHT		"height"
#define	SYNTRO_CAMERA_FRAMERATE		"frameRate"
#define SYNTRO_CAMERA_FORMAT        "format"

class CamClient : public Endpoint
{
	Q_OBJECT

public:
    CamClient(QObject *parent);
	virtual ~CamClient() {}

public slots:
	void pixelFormat(quint32 format);
    void videoFormat(int width, int height, int frameRate);
	void newJPEG(QByteArray);
	void newImage(QImage);

protected:
	void appClientInit();
	void appClientExit();
	void appClientConnected();
	void appClientBackground();

private:
	void sendFrame();
	QByteArray getFrame();

	int m_cameraPort;

	QMutex m_frameQMutex;
	QQueue <QByteArray> m_jpegFrameQ;
	QQueue <QImage> m_imgFrameQ;

	quint32 m_pixelFormat;

    int m_recordIndex;

    SYNTRO_AVPARAMS m_avParams;
};

#endif // CAMCLIENT_H

