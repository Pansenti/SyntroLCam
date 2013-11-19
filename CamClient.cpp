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

#include <qbuffer.h>
#include <SyntroAVDefs.h>
#include "CamClient.h"

#define	CAMERA_IMAGE_INTERVAL	((qint64)SYNTRO_CLOCKS_PER_SEC/60)

CamClient::CamClient(QObject *)
    : Endpoint(CAMERA_IMAGE_INTERVAL, "LCam")
{
}

void CamClient::sendFrame()
{
	QByteArray frame = getFrame();

	if (frame.size() == 0)
		return;

    SYNTRO_EHEAD *multiCast = clientBuildMessage(m_cameraPort, sizeof(SYNTRO_RECORD_AVMUX) + frame.size());

    SYNTRO_RECORD_AVMUX *avmuxHead = (SYNTRO_RECORD_AVMUX *)(multiCast + 1);

    SyntroUtils::avmuxHeaderInit(avmuxHead, &m_avParams, SYNTRO_RECORDHEADER_PARAM_NORMAL,
        m_recordIndex++, 0, frame.size(), 0);

    memcpy((unsigned char *)(avmuxHead + 1), frame.constData(), frame.size());

    int length = sizeof(SYNTRO_RECORD_AVMUX) + frame.size();

	clientSendMessage(m_cameraPort, multiCast, length, SYNTROLINK_LOWPRI);
}

void CamClient::appClientInit()
{
    m_cameraPort = clientAddService(SYNTRO_STREAMNAME_AVMUX, SERVICETYPE_MULTICAST, true);
    m_recordIndex = 0;

    // set avParam defaults

    m_avParams.videoWidth = 640;
    m_avParams.videoHeight = 480;
    m_avParams.videoFramerate = 30;
    m_avParams.videoSubtype = SYNTRO_RECORD_TYPE_VIDEO_MJPEG;
    m_avParams.avmuxSubtype = SYNTRO_RECORD_TYPE_AVMUX_MJPPCM;

}

void CamClient::appClientExit()
{
}

void CamClient::appClientBackground()
{
	if (clientIsServiceActive(m_cameraPort) && clientClearToSend(m_cameraPort))
		sendFrame();
}

void CamClient::appClientConnected()
{
}

QByteArray CamClient::getFrame()
{
	QByteArray frame;

	QMutexLocker lock(&m_frameQMutex);

	if (!m_jpegFrameQ.empty()) {
		frame = m_jpegFrameQ.dequeue();
	}
	else if (!m_imgFrameQ.empty()) {
		QImage img = m_imgFrameQ.dequeue();

		if (!img.isNull()) {
			QBuffer buffer(&frame);
			buffer.open(QIODevice::WriteOnly);
			img.save(&buffer, "JPG");
		}
	}

	return frame;		
}

void CamClient::pixelFormat(quint32 format)
{
	m_pixelFormat = format;
}

void CamClient::videoFormat(int width, int height, int frameRate)
{
    m_avParams.videoWidth = width;
    m_avParams.videoHeight = height;
    m_avParams.videoFramerate = frameRate;
}

void CamClient::newJPEG(QByteArray frame)
{
	if (m_frameQMutex.tryLock()) {
		if (m_jpegFrameQ.empty())
			m_jpegFrameQ.enqueue(frame);

		m_frameQMutex.unlock();
	}
}

void CamClient::newImage(QImage img)
{
	if (m_frameQMutex.tryLock()) {
		if (m_imgFrameQ.empty())
			m_imgFrameQ.enqueue(img);

		m_frameQMutex.unlock();
	}
}

