/*
 *  Kchmviewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2004-2014 George Yunaev, gyunaev@ulduzsoft.com
 *  Copyright (C) 2025 Nick Egorrov, nicegorov@yandex.ru
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstring>

#include <QByteArray>
#include <QIODevice>
#include <QMetaObject>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <Qt>
#include <QtGlobal>

#include <ubrowser/contentprovider.hpp>

#include "qtwebkitnetwork.h"


QtWebKit::NetworkReply::NetworkReply( UBrowser::ContentProvider::Ptr contentProvider,
                                      const QNetworkRequest& request,
                                      const QUrl& url ) :
	m_contentProvider( contentProvider )
{
	setRequest( request );
	setOpenMode( QIODevice::ReadOnly );

	m_data = loadResource( url );
	m_length = m_data.length();

	setHeader( QNetworkRequest::ContentLengthHeader, QByteArray::number( m_data.length() ) );
	QMetaObject::invokeMethod( this, "metaDataChanged", Qt::QueuedConnection );

	if ( m_length )
		QMetaObject::invokeMethod( this, "readyRead", Qt::QueuedConnection );

	QMetaObject::invokeMethod( this, "finished", Qt::QueuedConnection );
}

qint64 QtWebKit::NetworkReply::bytesAvailable() const
{
	return m_data.length() + QNetworkReply::bytesAvailable();
}

void QtWebKit::NetworkReply::abort()
{
}

qint64 QtWebKit::NetworkReply::readData( char* buffer, qint64 maxlen )
{
	qint64 len = qMin( qint64( m_data.length() ), maxlen );

	if ( len )
	{
		memcpy( buffer, m_data.constData(), len );
		m_data.remove( 0, len );
	}

	return len;
}

QByteArray QtWebKit::NetworkReply::loadResource( const QUrl& url )
{
	// Retrieve the data from ebook file
	UBrowser::ContentData data;

	if ( !m_contentProvider->content( data, url ) )
		qWarning( "Could not resolve file %s\n", qPrintable( url.toString() ) );

	if ( data.mime.startsWith( "text" ) )
	{
		QString header = QString( "%1; charset=utf-8" ).arg( QString( data.mime ) );
		setHeader( QNetworkRequest::ContentTypeHeader, header );
		return data.asUtf8;
	}

	return data.buffer;
}

QtWebKit::NetworkAccessManager::NetworkAccessManager( UBrowser::ContentProvider::Ptr contentProvider, QObject* parent )
	: QNetworkAccessManager( parent ),
	  m_contentProvider( contentProvider )
{
}

QNetworkReply* QtWebKit::NetworkAccessManager::createRequest( Operation op, const QNetworkRequest& request, QIODevice* outgoingData )
{
	//qDebug("KCHMNetworkAccessManager::createRequest %s", qPrintable( request.url().toString()) );

	if ( m_contentProvider->isValidUrl( request.url() ) )
		return new NetworkReply( m_contentProvider, request, request.url() );
	// TODO: remove pConfig->m_browserEnableRemoteContent )
	else
		return QNetworkAccessManager::createRequest( QNetworkAccessManager::GetOperation, QNetworkRequest( QUrl() ) );
}
