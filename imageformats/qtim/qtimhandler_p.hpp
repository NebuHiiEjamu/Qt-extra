#ifndef _TIMHANDLER_H
#define _TIMHANDLER_H

#include <QImageIOHandler>
#include <QSize>

class QTimHandler final: public QImageIOHandler
{
public:
	bool canRead() const;
	static bool canRead(QIODevice *dev);
	bool read(QImage *image);
	//bool write(const QImage &img) override;
	QVariant option(ImageOption op) const override;
	//void setOption(ImageOption op, const QVariant &val) override;
	bool supportsOption(ImageOption op) const override;
private:
	QSize m_size;
};

#endif // _TIMHANDLER_H
