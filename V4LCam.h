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

#ifndef V4LCAM_H
#define V4LCAM_H

#include <QThread>
#include <QSize>
#include <QSettings>
#include <qimage.h>

#include <linux/videodev2.h>

#define HUFFMAN_TABLE_SIZE 420
#define AVI_HEADER_SIZE 37

class V4LCam : public QThread
{
	Q_OBJECT

public:
	V4LCam(QSettings *settings = NULL);
	virtual ~V4LCam();

	bool deviceExists();
	bool isDeviceOpen();
	void startCapture();
	void stopCapture();

	QSize getImageSize();

signals:
	void pixelFormat(quint32 format);
	void frameSize(int width, int height);
	void newJPEG(QByteArray);
	void newImage(QImage);
	void newFrame();
	void cameraState(QString state);

protected:
	void run();

private:
	bool openDevice();
	void closeDevice();
	bool readFrame();

	void queryAvailableFormats();
	void queryAvailableSizes();
	void queryAvailableRates();

	bool choosePixelFormat();
	bool chooseFrameSize();
	bool chooseFrameRate();

	bool setImageFormat();    
	bool setFrameRate();
	bool allocMmapBuffers();
	void freeMmapBuffers();
	bool queueV4LBuffer(quint32 index);
	bool streamOn();
	void streamOff();
	bool handleFrame();
	bool handleJpeg(quint32 index, quint32 size);
	bool handleYUYV(quint32 index, quint32 size);
	QImage YUYV2RGB(quint32 index);
	int xioctl(int request, void *arg);
	void dumpRaw(QByteArray frame);

	static bool frameSizeLessThan(const QSize &a, const QSize &b);
	static bool frameRateLessThan(const QSize &a, const QSize &b);

	bool m_stopTime;
	quint32 m_preferredFormat;
	int m_preferredWidth;
	int m_preferredHeight;
	qreal m_preferredFrameRate;

	quint32 m_format;
	int m_width;
	int m_height;
	qreal m_frameRate;
	int m_frameRateIndex;

	quint32 m_pixelFormat;
	int m_fd;
	int m_cameraNum;
	quint32 m_consecutiveBadFrames;
	quint32 m_mmBuffLen;
	QList<char *> m_mmBuff;
	uchar *m_rgbBuff;
	quint32 m_jpegSize;
	static const unsigned char huffmanTable[HUFFMAN_TABLE_SIZE];
	static const unsigned char jpegAviHeader[AVI_HEADER_SIZE];
	int m_frameCount;
	QList<quint32> m_formatList;
	QList<QSize> m_sizeList;
	QList<QSize> m_rateList;
};

#endif // V4LCAM_H

