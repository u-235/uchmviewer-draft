/*
 *  Kchmviewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2004-2016 George Yunaev, gyunaev@ulduzsoft.com
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

#ifndef WEBENGINEWIDGET_H
#define WEBENGINEWIDGET_H

#include <QObject>          // for Q_OBJECT, signals
#include <QString>          // for QString
#include <QWebEngineView>   // for QWebEngineView

class QContextMenuEvent;
class QPoint;
class QUrl;
class QWidget;


class WebEngineWidget : public QWebEngineView
{
		Q_OBJECT

	public:
		WebEngineWidget(QWidget* parent);
		virtual ~WebEngineWidget();

	signals:
		void contextMenuRequested(const QPoint& globalPos, const QUrl& url);

	protected:
		// Overloaded to provide custom context menu
		void contextMenuEvent(QContextMenuEvent* event);
};

#endif // WEBENGINEWIDGET_H
