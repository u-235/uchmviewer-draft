/*
 *  Kchmviewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2004-2014 George Yunaev, gyunaev@ulduzsoft.com
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

#include <QApplication>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QUrl>
#include <QWebFrame>
#include <QWebPage>
#include <QWebView>
#include <Qt>

#include <ubrowser/types.hpp>

#include "webkitwidget.h"


WebKitWidget::WebKitWidget( QWidget* parent )
	: QWebView( parent )
{
	connect( this, &QWebView::linkClicked,
	         [this]( const QUrl & link ) { onLinkClicked( link ); } );
}

WebKitWidget::~WebKitWidget()
{
}

QUrl WebKitWidget::anchorAt( const QPoint& pos )
{
	QWebHitTestResult res = page()->currentFrame()->hitTestContent( pos );

	if ( !res.linkUrl().isValid() )
	{
		return QUrl();
	}

	return  res.linkUrl();
}

void WebKitWidget::contextMenuEvent( QContextMenuEvent* event )
{
	QUrl link = anchorAt( event->pos() );
	emit contextMenuRequested( event->globalPos(), link );
}

void WebKitWidget::mouseReleaseEvent( QMouseEvent* event )
{
	if ( event->button() == Qt::MidButton )
	{
		QUrl link = anchorAt( event->pos() );

		if ( !link.isEmpty() )
		{
			emit linkClicked( link, UBrowser::OPEN_IN_BACKGROUND );
			return;
		}
	}

	QWebView::mouseReleaseEvent( event );
}

void WebKitWidget::onLinkClicked( const QUrl& link )
{
	if ( ( QApplication::keyboardModifiers() & Qt::ShiftModifier ) != 0 )
		emit linkClicked( link, UBrowser::OPEN_IN_NEW );
	else if ( ( QApplication::keyboardModifiers() & Qt::ControlModifier ) != 0 )
		emit linkClicked( link, UBrowser::OPEN_IN_BACKGROUND );
	else
		emit linkClicked( link, UBrowser::OPEN_IN_CURRENT );
}
