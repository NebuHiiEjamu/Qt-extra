#ifndef _TIM_H
#define _TIM_H

#include <QImageIOHandler>
#include <QSize>

class TIMHandler final: public QImageIOHandler
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

class TIMPlugin final: public QImageIOPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "tim.json")
public:
  Capabilities capabilities(QIODevice *dev, const QByteArray &fmt) const;
  QImageIOHandler* create(QIODevice *dev, const QByteArray &fmt = QByteArray()) const;
};

#endif // _TIM_H
