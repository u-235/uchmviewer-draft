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

#ifndef QTWEBENGINEURLSCHEMEHANDLER_H
#define QTWEBENGINEURLSCHEMEHANDLER_H

#include <QWebEngineUrlSchemeHandler>

class QObject;
class QWebEngineUrlRequestJob;

#include <ubrowser/contentprovider.hpp>


namespace QtWebEngine
{

class UrlSchemeHandler : public QWebEngineUrlSchemeHandler
{
	public:
		UrlSchemeHandler( UBrowser::ContentProvider::Ptr contentProvider, QObject* parent );

		void requestStarted( QWebEngineUrlRequestJob* request );

	private:
		UBrowser::ContentProvider::Ptr m_contentProvider;
};

} // namespace QtWebEngine

#endif // QTWEBENGINEURLSCHEMEHANDLER_H
