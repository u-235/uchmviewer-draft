/*
 *  Kchmviewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2004-2016 George Yunaev, gyunaev@ulduzsoft.com
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

#ifndef QTWEBENGINEWIDGET_H
#define QTWEBENGINEWIDGET_H

#include <QObject>
#include <QString>
#include <QWebEngineView>

class QContextMenuEvent;
class QPoint;
class QUrl;
class QWidget;


namespace QtWebEngine
{

class Widget : public QWebEngineView
{
		Q_OBJECT

	public:
		Widget( QWidget* parent );
		virtual ~Widget();

	signals:
		void contextMenuRequested( const QPoint& globalPos, const QUrl& url );

	protected:
		// Overloaded to provide custom context menu
		void contextMenuEvent( QContextMenuEvent* event );
};

} // QtWebEngine

#endif // QTWEBENGINEWIDGET_H
