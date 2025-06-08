/*
 *  Kchmviewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2004-2016 George Yunaev, gyunaev@ulduzsoft.com
 *  Copyright (C) 2021-2025 Nick Egorrov, nicegorov@yandex.ru
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

#include <QBuffer>
#include <QByteArray>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QWebEngineUrlRequestJob>
#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
	#include <QWebEngineUrlScheme>
#endif

#include <ubrowser/contentprovider.hpp>

#include "dataprovider.h"


#if defined PRINT_DEBUG_ALL || defined PRINT_DEBUG_WEBENGINE || defined PRINT_DEBUG_WEBENGINEDATAPROVIDER
	#define PRINT_DEBUG 1
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
static struct RegistrationHelper
{
	RegistrationHelper()
	{
		QWebEngineUrlScheme scheme( EBOOK_URL_SCHEME );
		scheme.setSyntax( QWebEngineUrlScheme::Syntax::HostAndPort );
		scheme.setDefaultPort( 443 );
		scheme.setFlags( QWebEngineUrlScheme::SecureScheme );
		QWebEngineUrlScheme::registerScheme( scheme );
	}
} helper;

#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))

DataProvider::DataProvider( UBrowser::ContentProvider::Ptr contentProvider, QObject* parent )
	: QWebEngineUrlSchemeHandler( parent ),
	  m_contentProvider( contentProvider )
{
}

void DataProvider::requestStarted( QWebEngineUrlRequestJob* request )
{
	QUrl url = request->requestUrl();
#if PRINT_DEBUG
	qDebug() << "[DEBUG] DataProvider::requestStarted";
	qDebug() << "  url = " << url.toString();
#endif

	// Retrieve the data from ebook file
	UBrowser::ContentData data;

	if ( !m_contentProvider->content( data, url ) )
	{
		qWarning( "Could not resolve file %s\n", qPrintable( url.toString() ) );
		request->fail( QWebEngineUrlRequestJob::UrlNotFound );
		return;
	}

	// We must specify the proper MIME type for the page to display correctly.
	// The HTML and XML files correspond to "text/html";
	// for other types "application/octet-stream" is sufficient.
	// In addition, for "text/html", a "meta" tag is added specifying the text encoding.
	// This is the easiest and most stable way to set the encoding.
	if ( data.mime == "text/html" )
		data.asUtf8.prepend( QString( "<META http-equiv='Content-Type' content='text/html; charset=utf-8'>" ).toUtf8() );

	// We will use the buffer because reply() requires the QIODevice.
	// This buffer must be valid until the request is deleted.
	QBuffer* outbuf = new QBuffer;

	if ( data.mime.startsWith( "text" ) )
		outbuf->setData( data.asUtf8 );
	else
		outbuf->setData( data.buffer );

	outbuf->close();

	// Only delete the buffer when the request is deleted too
	connect( request, SIGNAL( destroyed() ), outbuf, SLOT( deleteLater() ) );

	// We're good to go
	request->reply( data.mime, outbuf );
}
