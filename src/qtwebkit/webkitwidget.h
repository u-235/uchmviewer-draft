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

#ifndef WEBKITWIDGET_H
#define WEBKITWIDGET_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QWebView>

class QContextMenuEvent;
class QMouseEvent;
class QPoint;
class QWidget;

#include <ubrowser/types.hpp>


class WebKitWidget : public QWebView
{
		Q_OBJECT

	public:
		WebKitWidget( QWidget* parent );
		virtual ~WebKitWidget();

	signals:
		void linkClicked( const QUrl& link, UBrowser::OpenMode mode );
		void contextMenuRequested( const QPoint& globalPos, const QUrl& url );

	protected:
		QUrl anchorAt( const QPoint& pos );
		void contextMenuEvent( QContextMenuEvent* event );
		void mouseReleaseEvent( QMouseEvent* event );

	protected slots:
		void onLinkClicked( const QUrl& link );
};

#endif // WEBKITWIDGET_H
