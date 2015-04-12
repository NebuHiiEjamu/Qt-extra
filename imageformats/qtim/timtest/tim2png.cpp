#include <QCoreApplication>
#include <QImage>
#include <QImageReader>
#include <QString>
#include <QtDebug>
#include <QTextStream>
#include <QByteArray>

int main()
{
	QCoreApplication::addLibraryPath(".");
	qDebug() << QImageReader::supportedImageFormats();
	QImage img;
	img.load("0025.tim");
	QTextStream ts(stdout);
	
	for(auto c: img.colorTable())
		ts << QString::number(c, 16).remove("\"") << " ";
	ts << '\n';
	
	qDebug() << "Size: " << img.colorTable().size();
	qDebug() << "Saved: " << img.save("0025.png");
	
	return 0; //app.exec();
}