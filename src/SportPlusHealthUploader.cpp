/*
 * Copyright (c) 2015 Luca Rasina <luca.rasina@sphtechnology.ch>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "SportPlusHealthUploader.h"
#include "ShareDialog.h"
#include "Athlete.h"
#include "Settings.h"
#include "TimeUtils.h"
#include "Units.h"

#include <QMessageLogger>
#include <stdio.h>

// access to metrics
#include "PwxRideFile.h"

const QString SPH_URL( "http://dev.sportplushealth.com/sport/en/api/1" );

SportPlusHealthUploader::SportPlusHealthUploader(Context *context, RideItem *ride, ShareDialog *parent ) :
    ShareDialogUploader( tr("SportPlusHealth"), context, ride, parent)
{
    exerciseId = ride->ride()->getTag("SphExercise", "");

    connect(&networkMgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(dispatchReply(QNetworkReply*)));
}

bool
SportPlusHealthUploader::canUpload( QString &err )
{
    QString username = appsettings->cvalue(context->athlete->cyclist, GC_SPORTPLUSHEALTHUSER).toString();

    if( username.length() > 0 ){
        return true;
    }

    err = tr("Cannot upload to SportPlusHealth without credentials. Check Settings");
    return false;
}

bool
SportPlusHealthUploader::wasUploaded()
{
    return exerciseId.length() > 0;
}

void
SportPlusHealthUploader::upload()
{
    requestUpload();
}

void
SportPlusHealthUploader::requestUpload()
{
    QMessageLogger().info() << "---> Starting...";
    parent->progressLabel->setText(tr("sending to SportPlusHealth..."));

    //Retrieve user credentials
    QString username = appsettings->cvalue(context->athlete->cyclist, GC_SPORTPLUSHEALTHUSER).toString();
    QString password = appsettings->cvalue(context->athlete->cyclist, GC_SPORTPLUSHEALTHPASS).toString();

    //Building URL of the API, including credentials for authentication
    QUrl url(SPH_URL + "/" + username + "/importGC");
    QMessageLogger().info() << "---> URL: " << url;

    //Building the message content
    QHttpMultiPart *body = new QHttpMultiPart( QHttpMultiPart::FormDataType );

    //Including the optional session name
    QHttpPart textPart;
    textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"session_name\""));
    textPart.setBody(QByteArray(insertedName.toLatin1()));
    QMessageLogger().info() << "---> NAME: " << insertedName;

    body->append(textPart);

    //Including file in the request
    QString fname = context->athlete->home->temp().absoluteFilePath(".sph-upload.pwx" );
    QFile *uploadFile = new QFile( fname );
    uploadFile->setParent(body);

    PwxFileReader reader;
    reader.writeRideFile(context, ride->ride(), *uploadFile );
    parent->progressBar->setValue(parent->progressBar->value()+20/parent->shareSiteCount);

    int limit = 16777216; // 16MB
    if( uploadFile->size() >= limit ){
        parent->progressLabel->setText(tr("error uploading to SportPlusHealth"));
        parent->errorLabel->setText(tr("temporary file too large for upload: %1 > %1 bytes")
                                    .arg(uploadFile->size())
                                    .arg(limit) );

        eventLoop.quit();
        return;
    }

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"datafile\"; filename=\"gc-upload-sph.pwx\""));
    uploadFile->open(QIODevice::ReadOnly);
    filePart.setBodyDevice(uploadFile);
    body->append( filePart );

    //Sending the authenticated post request to the API
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Authorization", "Basic " + QByteArray(QString("%1:%2").arg(username).arg(password).toLatin1()).toBase64());
    networkMgr.post(request, body);
}

/*////////////////////
/// QXML HANDLER
////////////////////*/
class SphParser : public QXmlDefaultHandler
{
public:
    friend class SportPlusHealthUploader;

    bool startElement( const QString&, const QString&, const QString&, const QXmlAttributes& )
    {
        cdata = "";
        return true;
    }

    bool endElement( const QString&, const QString&, const QString& qName )
    {
        if( qName == "error" ){
            error = cdata;
            return true;
        }
        return true;
    }

    bool characters( const QString& str)
    {
        cdata += str;
        return true;
    }

protected:
    QString cdata;
    QString error;
};

/*////////////////////
/// RESULTS HANDLER
////////////////////*/
void
SportPlusHealthUploader::dispatchReply( QNetworkReply *reply )
{
    if( reply->error() != QNetworkReply::NoError ){
        parent->progressLabel->setText(tr("error uploading to SportPlusHealth"));
        parent->errorLabel->setText( tr("request failed: ") + reply->errorString() );

        eventLoop.quit();
        return;
    }

    QVariant status = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    if( ! status.isValid() || status.toInt() != 200 ){
        QVariant msg( reply->attribute( QNetworkRequest::HttpReasonPhraseAttribute ) );

        parent->progressLabel->setText(tr("error uploading to SportPlusHealth"));
        parent->errorLabel->setText( tr("request failed, Server response: %1 %2")
            .arg(status.toInt())
            .arg(msg.toString()) );

        eventLoop.quit();
        return;
    }

    finishUpload( reply );
}

class SphUploadParser : public SphParser
{
public:
    friend class SportPlusHealthUploader;

    bool endElement( const QString& a, const QString&b, const QString& qName )
    {
        if( qName == "id" ){
            id = cdata;
            return true;

        }

        return SphParser::endElement( a, b, qName );
    };

protected:
    QString id;

};

void
SportPlusHealthUploader::finishUpload(QNetworkReply *reply)
{
    parent->progressBar->setValue(parent->progressBar->value()+60/parent->shareSiteCount);

    //Parsing JSON server reply
    QString strReply = (QString)reply->readAll();
    qDebug() << "Response:" << strReply;
    QJsonObject jsonObj = QJsonDocument::fromJson(strReply.toUtf8()).object();
    jsonResponseSuccess = jsonObj["success"].toBool();
    jsonResponseErrorCode = jsonObj["error_code"].toInt();
    reply->deleteLater();

    if(!jsonResponseSuccess) {
       parent->progressLabel->setText(tr("error uploading to SportPlusHealth"));
       parent->errorLabel->setText(tr("failed to upload file (cod. %1)").arg(jsonResponseErrorCode));
       ride->ride()->setTag("SphExercise", exerciseId );
       ride->setDirty(true);

       eventLoop.quit();
       return;
    }

    parent->progressLabel->setText(tr("successfully uploaded to SportPlusHealth"));
    eventLoop.quit();
}
