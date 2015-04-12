#include <QDataStream>
#include <QImage>
#include <QIODevice>
#include <QtGlobal>
#include <QtMath>
#include <QVariant>
#include <QVector>

#ifndef QT_NO_DEBUG
#include <QFile>
#include <QString>
#include <QtDebug>
#include <QTextStream>
#endif

#include "qtimhandler_p.hpp"

#define SCALE5TO8(c) (((c) << 3) | ((c) >> 2))
#define SCALE8TO5(c) (((c) & 0xf8) >> 3)

bool QTimHandler::canRead() const
{
	return canRead(device());
}

bool QTimHandler::canRead(QIODevice *dev)
{
	return dev->peek(4) == QByteArrayLiteral("\x10\x0\x0\x0");
}

static inline quint32 rgba5551ToArgb32(quint16 c)
{
	quint32 r = SCALE5TO8(c & 0x1f);
	quint32 g = SCALE5TO8((c >> 5) & 0x1f);
	quint32 b = SCALE5TO8((c >> 10) & 0x1f);
	quint32 a = ~SCALE5TO8((c >> 15) & 0x1f);
	
	return ((a << 24) | (r << 16) | (g << 8) | b);
}

bool QTimHandler::read(QImage *img)
{
	QDataStream ds(device());
	ds.setByteOrder(QDataStream::LittleEndian);
	quint32 magic, flags;
	
	ds >> magic >> flags;
#ifndef QT_NO_DEBUG
	qDebug() << "Magic: " << QString::number(magic, 16);
#endif
	if (ds.status() != QDataStream::Ok || magic != 0x10)
			return false;
	bool hasClut = flags & 0x8 ? true : false;
	quint8 pxlMode = flags & 0x7;
	
	quint8 bpp;
	switch (pxlMode)
	{
		case 0: bpp = 4; break;
		case 1: bpp = 8; break;
		case 2: bpp = 16; break;
		case 3: bpp = 24; break;
		default: bpp = 4;
	}
#ifndef QT_NO_DEBUG
	qDebug() << "Has CLUT: " << hasClut;
	qDebug() << "Pixel mode: " << pxlMode;
	qDebug() << "Bits per pixel: " << bpp;
#endif
	
	QVector<QRgb> clut;
	if (hasClut)
	{
		quint16 colors, npals;
		ds.skipRawData(8);
		ds >> colors >> npals;
		
		for (quint16 i = 0; i < (colors*npals); i++)
		{
			quint16 c;
			ds >> c;
			clut.append(rgba5551ToArgb32(c));
		}
		
#ifndef QT_NO_DEBUG
		qDebug() << "Colors: " << colors;
		qDebug() << "Palettes: " << npals;
#endif
	}
	
	ds.skipRawData(8);
	quint16 w, h;
	ds >> w >> h;
	
	switch (bpp)
	{
		case 4: w /= 4; break;
		case 8: w /= 2; break;
		default: ;
	}
#ifndef QT_NO_DEBUG
	qDebug() << "Width: " << w;
	qDebug() << "Height: " << h;
	QFile pxldat("pxlidx");
	pxldat.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream ts(&pxldat);
#endif
	
	QImage result;
	m_size = QSize(w, h);
	
	switch (bpp)
	{
		case 4:
		case 8:
			result = QImage(w, h, QImage::Format_Indexed8);
			result.setColorTable(clut);
			break;
		default:
			result = QImage(w, h, QImage::Format_ARGB32);
	}
	
	for (quint16 y = 0; y < h; y++)
	{
		for (quint16 x = 0; x < w; x++)
		{
			switch (bpp)
			{
				case 4:
				{
					quint8 p;
					ds >> p;
					result.setPixel(x, y, p & 0xf0);
					result.setPixel(x++, y, p & 0xf);
#ifndef QT_NO_DEBUG
					ts << (p & 0xf0) << ", " << (p & 0xf) << ", ";
#endif
				}
					break;
				case 8:
				{
					quint8 p;
					ds >> p;
					result.setPixel(x, y, p);
#ifndef QT_NO_DEBUG
					ts << p << ", ";
#endif
				}
					break;
				case 16:
				{
					quint16 p;
					ds >> p;
					result.setPixel(x, y, rgba5551ToArgb32(p));
				}
					break;
			}
		}
#ifndef QT_NO_DEBUG
		ts << '\n';
#endif
	}
	
	*img = result;
	return ds.status() == QDataStream::Ok;
}

bool QTimHandler::supportsOption(ImageOption op) const
{
	switch (op)
	{
		case Size:
			return true;
		default:
			return false;
	}
}

QVariant QTimHandler::option(ImageOption op) const
{
	switch (op)
	{
		case Size: return m_size;
		default: return QVariant();
	}
}
