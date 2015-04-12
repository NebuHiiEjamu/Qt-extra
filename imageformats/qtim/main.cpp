#include <QByteArray>
#include <QImageIOPlugin>
#include <QIODevice>
#include <QStringList>

#include "qtimhandler_p.hpp"

class QTimPlugin final: public QImageIOPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "tim.json")
public:
	Capabilities capabilities(QIODevice *dev, const QByteArray &fmt) const;
	QImageIOHandler* create(QIODevice *dev, const QByteArray &fmt = QByteArray()) const;
};

QImageIOPlugin::Capabilities QTimPlugin::capabilities(QIODevice *dev, const QByteArray &fmt) const
{
	if (fmt == QByteArrayLiteral("tim")) return Capabilities(CanRead);
	if (!fmt.isEmpty()) return 0;
	if (!dev || !dev->isOpen()) return 0;
	
	Capabilities cap;
	if (dev->isReadable() && QTimHandler::canRead(dev)) cap |= CanRead;
	//if (dev->isWritable()) cap |= CanWrite;
	
	return cap;
}

QImageIOHandler* QTimPlugin::create(QIODevice *dev, const QByteArray &fmt) const
{
	QImageIOHandler *hnd = new QTimHandler;
	hnd->setDevice(dev);
	hnd->setFormat(fmt);
	return hnd;
}

#include "main.moc"
