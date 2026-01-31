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

#ifndef QWEBKITNETWORK_H
#define QWEBKITNETWORK_H

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtGlobal>

class QIODevice;
class QNetworkRequest;
class QObject;
class QUrl;

#include <ubrowser/contentprovider.hpp>


namespace QtWebKit
{

//
// A network reply to emulate data transfer from CHM file
//
class NetworkReply : public QNetworkReply
{
	public:
		NetworkReply( UBrowser::ContentProvider::Ptr contentProvider,
		              const QNetworkRequest& request,
		              const QUrl& url );
		qint64 bytesAvailable() const override;
		void abort() override;

	protected:
		qint64 readData( char* buffer, qint64 maxlen ) override;
		QByteArray loadResource( const QUrl& url );

	private:
		QByteArray  m_data;
		qint64      m_length;
		UBrowser::ContentProvider::Ptr m_contentProvider;
};


//
// A network manager to emulate data transfer from CHM file
//
class NetworkAccessManager : public QNetworkAccessManager
{
	public:
		NetworkAccessManager( UBrowser::ContentProvider::Ptr contentProvider,
		                      QObject* parent );

	protected:
		QNetworkReply* createRequest( Operation op,
		                              const QNetworkRequest& request,
		                              QIODevice* outgoingData = nullptr ) override;

	private:
		UBrowser::ContentProvider::Ptr m_contentProvider;
};

} // namespace QtWebKit

#endif // QWEBKITNETWORK_H
