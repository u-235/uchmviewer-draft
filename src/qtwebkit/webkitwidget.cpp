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

#include <QApplication>         // for QApplication
#include <QContextMenuEvent>    // for QContextMenuEvent
#include <QMouseEvent>          // for QMouseEvent
#include <QUrl>                 // for QUrl
#include <QWebFrame>            // for QWebHitTestResult, QWebFrame
#include <QWebPage>             // for QWebPage
#include <QWebView>             // for QWebView
#include <Qt>                   // for MidButton

#include <browser/types.hpp>    // for OPEN_IN_BACKGROUND, OPEN_IN_CURRENT

#include "webkitwidget.h"


WebKitWidget::WebKitWidget(WebKitController* parent)
	: QWebView()
{
	connect(this, &QWebView::linkClicked,
	        [this](const QUrl & link)
	{
		if ( (QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0 )
			emit linkClicked( link, Browser::OPEN_IN_NEW );
		else if ( (QApplication::keyboardModifiers() & Qt::ControlModifier) != 0 )
			emit linkClicked( link, Browser::OPEN_IN_BACKGROUND );
		else
			emit linkClicked( link, Browser::OPEN_IN_CURRENT );
	});
}

WebKitWidget::~WebKitWidget()
{
}

QUrl WebKitWidget::anchorAt(const QPoint& pos)
{
	QWebHitTestResult res = page()->currentFrame()->hitTestContent(pos);

	if (!res.linkUrl().isValid()) {
		return QUrl();
	}

	return  res.linkUrl();
}

void WebKitWidget::mouseReleaseEvent (QMouseEvent* event)
{
	if (event->button() == Qt::MidButton) {
		QUrl link = anchorAt(event->pos());

		if (!link.isEmpty()) {
			emit linkClicked(link, Browser::OPEN_IN_BACKGROUND);
			return;
		}
	}

	QWebView::mouseReleaseEvent(event);
}

void WebKitWidget::contextMenuEvent(QContextMenuEvent* event)
{
	QUrl link = anchorAt(event->pos());
	emit contextMenuRequested(event->globalPos(), link);
}
