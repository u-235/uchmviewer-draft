/*
 *  Kchmviewer - a CHM and EPUB file viewer with broad language support
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

#ifndef QTWEBENGINEPAGE_H
#define QTWEBENGINEPAGE_H

#include <QObject>
#include <QString>
#include <QWebEnginePage>

class QUrl;

#include <ubrowser/contentprovider.hpp>

namespace UBrowser { enum class OpenMode; }


namespace QtWebEngine
{

class Page : public QWebEnginePage
{
		Q_OBJECT

	signals:
		// This signal is emitted whenever the user clicks on a link.
		void linkClicked( const QUrl& url, UBrowser::OpenMode mode );

	public:
		Page( UBrowser::ContentProvider::Ptr content, QObject* parent );

		~Page();

		bool acceptNavigationRequest( const QUrl& url, NavigationType type, bool isMainFrame ) override;

		QWebEnginePage* createWindow( QWebEnginePage::WebWindowType type ) override;

	protected slots:
		void onLinkHovered( const QString& url );

	protected:
		QString m_url;
};

} // namespace QtWebEngine

#endif // QTWEBENGINEPAGE_H
