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

#include <QContextMenuEvent>    // for QContextMenuEvent
#include <QMouseEvent>          // for QMouseEvent
#include <QPalette>             // for QPalette, QPalette::Active, QPalette::Highlight, QPalette::HighlightedText, QPalette::Inactive
#include <QString>              // for QString, operator!=
#include <QUrl>                 // for QUrl
#include <QWebFrame>            // for QWebFrame, QWebHitTestResult
#include <QWebHistory>          // for QWebHistory
#include <QWebPage>             // for QWebPage, QWebPage::FindFlags, QWebPage::HighlightAllOccurrences, QWebPage::Copy, QWebPage::DelegateAllLinks
#include <QWebSettings>         // for QWebSettings, QWebSettings::AutoLoadImages, QWebSettings::JavaEnabled, QWebSettings::JavascriptEnabled, QWeb...
#include <QWebView>             // for QWebView
#include <QWidget>              // for QWidget
#include <Qt>                   // for Vertical, MidButton
#include <QtGlobal>             // for QFlags, qrealManager

class QPrinter;

#include <browser/content-provider.hpp> // for ContentProvider::Ptr
#include <browser/controller.hpp>       // for Controller
#include <browser/settings.hpp>         // for Settings
#include <browser/types.hpp>            // for OPTION_HIGH_LIGHT_SEARCH_RESULT, OPTION_IMAGES, OPTION_JAVA, OPTION_JAVA_SCRIPT, Option
#include <ebook.h>                      // for EBook

#include "../mainwindow.h"          // for MainWindow, mainWindow
#include "../viewwindowmgr.h"       // for ViewWindowMgr
#include "dataprovider.h"           // for KCHMNetworkAccessManager
#include "webkitcontroller.h"


WebKitController::WebKitController( QWidget* parent )
	: Browser::Controller ( nullptr, parent )
{
	m_webView = new QWebView();
	invalidate();
	m_contextMenu = 0;
	m_contextMenuLink = 0;
	m_storedScrollbarPosition = 0;

	// Use our network emulation layer
	m_webView->page()->setNetworkAccessManager( new KCHMNetworkAccessManager(this) );

	// All links are going through us
	m_webView->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );

	connect( m_webView, SIGNAL( loadFinished(bool)), this, SLOT( onLoadFinished(bool)) );

	// Search results highlighter
	QPalette pal = m_webView->palette();
	pal.setColor( QPalette::Inactive, QPalette::Highlight, pal.color(QPalette::Active, QPalette::Highlight) );
	pal.setColor( QPalette::Inactive, QPalette::HighlightedText, pal.color(QPalette::Active, QPalette::HighlightedText) );
	m_webView->setPalette( pal );
}

WebKitController::~WebKitController()
{
}

void WebKitController::invalidate( )
{
	m_storedScrollbarPosition = 0;
	m_webView->reload();
}

void WebKitController::load ( const QUrl& url )
{
	//qDebug("ViewWindow::openUrl %s", qPrintable(url.toString()));

	// Do not use setContent() here, it resets QWebHistory
	m_webView->load( url );

	mainWindow->viewWindowMgr()->setTabName( this );
}

void WebKitController::applySettings(Browser::Settings& settings)
{
	QWebSettings* setup = QWebSettings::globalSettings();

	setup->setAttribute( QWebSettings::AutoLoadImages, settings.enableImages );
	setup->setAttribute( QWebSettings::JavascriptEnabled, settings.enableJS );
	setup->setAttribute( QWebSettings::JavaEnabled, settings.enableJava );
	setup->setAttribute( QWebSettings::PluginsEnabled, settings.enablePlugins );
	setup->setAttribute( QWebSettings::OfflineStorageDatabaseEnabled, settings.enableOfflineStorage );
	setup->setAttribute( QWebSettings::LocalStorageDatabaseEnabled, settings.enableLocalStorage );
	setup->setAttribute( QWebSettings::LocalStorageEnabled, settings.enableLocalStorage );
}

QString WebKitController::title() const
{
	QString title = ::mainWindow->chmFile()->getTopicByUrl( url() );

	// If no title is found, use the path (without the first /)
	if ( title.isEmpty() )
		title = url().path().mid( 1 );

	return title;
}

bool WebKitController::canGoBack() const
{
	return m_webView->history()->canGoBack();
}

bool WebKitController::canGoForward() const
{
	return m_webView->history()->canGoForward();
}

void WebKitController::print( QPrinter* printer, std::function<void (bool succes)> result )
{
	m_webView->print( printer );
	result(true);
}

void WebKitController::setZoomFactor(qreal zoom)
{
	m_webView->setZoomFactor( zoom );
}

qreal WebKitController::zoomFactor() const
{
	return m_webView->zoomFactor();
}

int WebKitController::scrollTop()
{
	return m_webView->page()->currentFrame()->scrollBarValue( Qt::Vertical );
}

void WebKitController::setScrollTop(int pos)
{
	m_webView->page()->currentFrame()->setScrollBarValue( Qt::Vertical, pos );
}

void WebKitController::setAutoScroll(int pos)
{
	m_storedScrollbarPosition = pos;
}

void WebKitController::findText(const QString& text,
                                bool backward,
                                bool caseSensitively,
                                bool highlightSearchResults,
                                std::function<void (bool found, bool wrapped)> result)
{
	QWebPage::FindFlags webkitflags;

	if ( caseSensitively )
		webkitflags |= QWebPage::FindCaseSensitively;

	if ( backward )
		webkitflags |= QWebPage::FindBackward;

	if ( highlightSearchResults )
	{
		// From the doc:
		// If the HighlightAllOccurrences flag is passed, the
		// function will highlight all occurrences that exist
		// in the page. All subsequent calls will extend the
		// highlight, rather than replace it, with occurrences
		// of the new string.

		// If the search text is different, we run the empty string search
		// to discard old highlighting
		if ( m_lastSearchedWord != text )
			m_webView->findText( "", webkitflags | QWebPage::HighlightAllOccurrences );

		m_lastSearchedWord = text;

		// Now we call search with highlighting enabled, while the main search below will have
		// it disabled. This leads in both having the highlighting results AND working forward/
		// backward buttons.
		m_webView->findText( text, webkitflags | QWebPage::HighlightAllOccurrences );
	}

	bool found = m_webView->findText( text, webkitflags );
	bool wrapped = false;

	// If we didn't find anything, enable the wrap and try again
	if ( !found )
	{
		found = m_webView->findText( text, webkitflags | QWebPage::FindWrapsAroundDocument );
		wrapped = found;
	}

	result(found, wrapped);
}

void WebKitController::selectAll()
{
	m_webView->triggerPageAction( QWebPage::SelectAll );
}

void WebKitController::selectedCopy()
{
	m_webView->triggerPageAction( QWebPage::Copy );
}

QUrl WebKitController::anchorAt(const QPoint& pos)
{
	QWebHitTestResult res = m_webView->page()->currentFrame()->hitTestContent( pos );

	if ( !res.linkUrl().isValid() )
		return QUrl();

	return  res.linkUrl();
}

void WebKitController::mouseReleaseEvent ( QMouseEvent* event )
{
	if ( event->button() == Qt::MidButton )
	{
		QUrl link = anchorAt( event->pos() );

		if ( !link.isEmpty() )
		{
			emit linkClicked( link, true );
			return;
		}
	}

	m_webView->event( event );
}

void WebKitController::contextMenuEvent(QContextMenuEvent* e)
{
	QUrl link = anchorAt( e->pos() );
	emit contextMenuRequest(this, e->globalPos(), link);
}

void WebKitController::onLoadFinished ( bool succes)
{
	if ( m_storedScrollbarPosition > 0 )
	{
		m_webView->page()->currentFrame()->setScrollBarValue( Qt::Vertical, m_storedScrollbarPosition );
		m_storedScrollbarPosition = 0;
	}

	emit loadFinished( this, succes);
}

QWidget* WebKitController::view()
{
	return m_webView;
}

bool WebKitController::hasOption(Browser::Option option)
{
	switch (option) {
	case Browser::OPTION_HIGH_LIGHT_SEARCH_RESULT:
	case Browser::OPTION_IMAGES:
	case Browser::OPTION_JAVA_SCRIPT:
	case Browser::OPTION_JAVA:
		return true;
	default:
		return false;
	}
}

QUrl WebKitController::url() const
{
	return m_webView->url();
}

void WebKitController::back()
{
	m_webView->back();
}

void WebKitController::forward()
{
	m_webView->forward();
}
