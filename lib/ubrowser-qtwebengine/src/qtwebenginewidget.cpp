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

#include <QContextMenuEvent>
#include <QUrl>
#include <QtGlobal>
#include <QWebEnginePage>


#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
	#include <QWebEngineContextMenuRequest>
#else
	#include <QWebEngineContextMenuData>
#endif

#include "qtwebenginewidget.h"


QtWebEngine::Widget::Widget( QWidget* parent )
	: QWebEngineView( parent )
{
}

QtWebEngine::Widget::~Widget()
{
}

void QtWebEngine::Widget::contextMenuEvent( QContextMenuEvent* event )
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
	QUrl link = lastContextMenuRequest()->linkUrl();
#else
	QUrl link = page()->contextMenuData().linkUrl();
#endif

	emit contextMenuRequested( event->globalPos(), link );
}
