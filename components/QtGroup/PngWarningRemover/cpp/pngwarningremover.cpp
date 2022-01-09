/*
    This file is part of JQTools

    Project introduce: https://github.com/188080501/JQTools

    Copyright: Jason

    Contact email: Jason@JasonServer.com

    GitHub: https://github.com/188080501/
*/

#include "pngwarningremover.h"

// Qt lib import
#include <QImage>
#include <QFileDialog>
#include <QStandardPaths>
#include <QtConcurrent>
#include <QFileInfo>

using namespace PngWarningRemover;

QString Manage::conversationPng()
{
    QEventLoop eventLoop;
    QString reply;

	//const auto&& filePaths = QFileDialog::getOpenFileNames(
	//	nullptr,
 //       QString::fromLocal8Bit("请选择PNG图片（可多选）"),
	//	//QStringLiteral( "\u8BF7\u9009\u62E9PNG\u56FE\u7247\uFF08\u53EF\u591A\u9009\uFF09" ),
	//	QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
	//	"*.png"
	//);

	const auto&& filePath = QFileDialog::getExistingDirectory(
		nullptr,
		QString::fromLocal8Bit("请选择PNG图片（可多选）"),
		QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)
		//"*.png"
	);
	QStringList filePaths;
	FindFile(filePath, filePaths);

    this->lastErrorFileName_.clear();

    QtConcurrent::run( [ this, &eventLoop, &reply, &filePaths ]()
    {
        if ( filePaths.isEmpty() )
        {
            reply = "cancel";
            QMetaObject::invokeMethod( &eventLoop, "quit" );
            return;
        }

        for ( const auto &filePath: filePaths )
        {
            QImage image( filePath, "PNG" );

            if ( image.isNull() || !image.save( filePath ) )
            {
                reply = "openSourceError";
                this->lastErrorFileName_ = QFileInfo( filePath ).fileName();
                QMetaObject::invokeMethod( &eventLoop, "quit" );
                return;
            }

            QImage image2( image.bits(), image.width(), image.height(), QImage::Format_ARGB32 );

            if ( image2.isNull() || !image2.save( filePath ) )
            {
                reply = "saveTargetError";
                this->lastErrorFileName_ = QFileInfo( filePath ).fileName();
                QMetaObject::invokeMethod( &eventLoop, "quit" );
                return;
            }
        }

        reply = "OK";
        QMetaObject::invokeMethod( &eventLoop, "quit" );
    } );

    eventLoop.exec();

    return reply;
}

bool Manage::FindFile(const QString& path, QStringList& files) {

	QDir dir(path);
	if (!dir.exists()) {
		return false;
	}
	dir.setFilter(QDir::Dirs | QDir::Files);
	dir.setSorting(QDir::DirsFirst);
	QFileInfoList list = dir.entryInfoList();
	int i = 0;
	do {

		QFileInfo fileInfo = list.at(i);
		if (fileInfo.fileName() == "." | fileInfo.fileName() == "..") {
			i++;
			continue;
		}
		bool bisDir = fileInfo.isDir();
		if (bisDir) {
			FindFile(fileInfo.filePath(),files);
		}
		else {
			bool ispng = fileInfo.fileName().endsWith(".png");
			if (ispng)
			{
				files.append(fileInfo.filePath());
			}
		}
		i++;
	} while (i < list.size());
	return true;
}
