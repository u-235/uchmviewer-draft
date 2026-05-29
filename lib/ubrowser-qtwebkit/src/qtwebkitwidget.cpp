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
#include <QFlags>
#include <QMouseEvent>
#include <QUrl>
#include <QWebFrame>
#include <QWebPage>
#include <QWebView>
#include <Qt>

class QWidget;

#include <ubrowser/browser.hpp>

#include "qtwebkitwidget.h"


QtWebKit::Widget::Widget( QWidget* parent )
	: QWebView( parent )
{
}

QtWebKit::Widget::~Widget()
{
}

QUrl QtWebKit::Widget::anchorAt( const QPoint& pos )
{
	QWebHitTestResult res = page()->currentFrame()->hitTestContent( pos );

	if ( !res.linkUrl().isValid() )
		return QUrl();

	return res.linkUrl();
}

void QtWebKit::Widget::contextMenuEvent( QContextMenuEvent* event )
{
	QUrl link = anchorAt( event->pos() );
	emit contextMenuRequested( event->globalPos(), link );
}

void QtWebKit::Widget::mouseReleaseEvent( QMouseEvent* event )
{
	if ( event->button() == Qt::MidButton )
	{
		QUrl link = anchorAt( event->pos() );

		if ( !link.isEmpty() )
		{
			emit linkClicked( link, UBrowser::OpenMode::open_in_background );
			return;
		}
	}

	QWebView::mouseReleaseEvent( event );
}
