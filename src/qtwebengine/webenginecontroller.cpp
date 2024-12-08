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
#include <QPalette>             // for QPalette, QPalette::Active, QPalette::Highlight, QPalette::HighlightedText, QPalette::Inactive
#include <QString>              // for QString
#include <QUrl>                 // for QUrl
#include <QVariant>             // for QVariant
#include <QWebEngineHistory>    // for QWebEngineHistory
#include <QWebEnginePage>       // for QWebEnginePage, QWebEnginePage::Copy, QWebEnginePage::SelectAll
#include <QWebEngineProfile>    // for QWebEngineProfile
#include <QWebEngineScript>     // for QWebEngineScript, QWebEngineScript::UserWorld
#include <QWebEngineSettings>   // for QWebEngineSettings, QWebEngineSettings::AutoLoadImages, QWebEngineSettings::JavascriptEnabled, QWebEngineSettin...
#include <QtGlobal>             // for QAtomicInt, QT_VERSION, QT_VERSION_CHECK, qreal, Q_UNUSED


#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
	#include <QWebEngineContextMenuRequest>
	#include <QWebEngineFindTextResult>
#else
	#include <QWebEngineContextMenuData>
#endif

class QPrinter;

#include <browser/settings.hpp> // for BrowserSettings
#include <browser/types.hpp>    // for OpenMode
#include <ebook.h>              // for EBook

#include "../mainwindow.h"          // for MainWindow, mainWindow
#include "../viewwindowmgr.h"       // for ViewWindowMgr
#include "webenginepage.h"          // for WebEnginePage
#include "webenginecontroller.h"
#include "webenginewidget.h"


WebEngineController::WebEngineController( QWidget* parent )
	: Browser::Controller(nullptr, parent)
{
	m_widget = new WebEngineWidget(parent);
	invalidate();
	m_storedScrollbarPosition = 0;

	WebEnginePage* page = new WebEnginePage( this );
	connect( page, SIGNAL( linkClicked ( const QUrl&, Browser::OpenMode ) ), this, SLOT( onLinkClicked( const QUrl&, Browser::OpenMode ) ) );
	m_widget->setPage( page );

	connect( m_widget, SIGNAL( loadFinished(bool)), this, SLOT( onLoadFinished(bool)) );
	connect(m_widget, &WebEngineWidget::contextMenuRequested,
	        [this](const QPoint & globalPos, const QUrl & link)
	{
		emit contextMenuRequested(globalPos, link);
	});
	connect(m_widget, &WebEngineWidget::urlChanged,
	        [this](const QUrl & link)
	{
		emit urlChanged(link);
	});

	// Search results highlighter
	QPalette pal = m_widget->palette();
	pal.setColor( QPalette::Inactive, QPalette::Highlight, pal.color(QPalette::Active, QPalette::Highlight) );
	pal.setColor( QPalette::Inactive, QPalette::HighlightedText, pal.color(QPalette::Active, QPalette::HighlightedText) );
	m_widget->setPalette( pal );
}

WebEngineController::~WebEngineController()
{
}

void WebEngineController::invalidate( )
{
	m_storedScrollbarPosition = 0;
	reload();
}

void WebEngineController::load ( const QUrl& url )
{
	// Do not use setContent() here, it resets QWebHistory
	m_widget->load( url );
}

QString WebEngineController::title() const
{
	QString title = ::mainWindow->chmFile()->getTopicByUrl( url() );

	// If no title is found, use the path (without the first /)
	if ( title.isEmpty() )
		title = url().path().mid( 1 );

	return title;
}

bool WebEngineController::canGoBack() const
{
	return  m_widget->history()->canGoBack();
}

bool WebEngineController::canGoForward() const
{
	return  m_widget->history()->canGoForward();
}

void WebEngineController::print( QPrinter* printer, std::function<void (bool success)> result )
{
#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
	m_widget->page()->print( printer, [&result](bool success) {
		result( success );
	});
#else
	connect( m_widget, &QWebEngineView::printFinished, [result](bool success) {
		result( success );
	});
	m_widget->print( printer );
#endif
}

void WebEngineController::setZoomFactor(qreal zoom)
{
	m_widget->setZoomFactor( zoom );
}

qreal WebEngineController::zoomFactor() const
{
	return  m_widget->zoomFactor();
}

int WebEngineController::scrollTop()
{
	QAtomicInt value = -1;

	m_widget->page()->runJavaScript("document.body.scrollTop",
	                                QWebEngineScript::UserWorld,
	[&value](const QVariant & v) { value = v.toInt(); });

	while ( value == -1 )
	{
		QApplication::processEvents();
	}

	return value;
}

void WebEngineController::setScrollTop(int pos)
{
	m_widget->page()->runJavaScript( QString( "document.body.scrollTop=%1" ).arg( pos )
	                                 , QWebEngineScript::UserWorld );
}

void WebEngineController::setAutoScroll(int pos)
{
	m_storedScrollbarPosition = pos;
}

void WebEngineController::findText(const QString& text,
                                   bool backward,
                                   bool caseSensitively,
                                   bool highlightSearchResults,
                                   std::function<void (bool found, bool wrapped)> result)
{
	Q_UNUSED(highlightSearchResults);
	QWebEnginePage::FindFlags webkitflags;

	if ( caseSensitively )
		webkitflags |= QWebEnginePage::FindCaseSensitively;

	if ( backward )
		webkitflags |= QWebEnginePage::FindBackward;

#if QT_VERSION <= QT_VERSION_CHECK(6, 2, 0)
	m_widget->findText( text, webkitflags,
	                    [ = ](bool found)
	{
		result(found, false);
	});
#else
	m_widget->findText( text, webkitflags,
	                    [ = ](const QWebEngineFindTextResult & found)
	{
		result(found.numberOfMatches() > 0, false);
	});
#endif
}

void WebEngineController::selectAll()
{
	m_widget->triggerPageAction( QWebEnginePage::SelectAll );
}

void WebEngineController::selectedCopy()
{
	m_widget->triggerPageAction( QWebEnginePage::Copy );
}

void WebEngineController::onLoadFinished ( bool success )
{
	if ( m_storedScrollbarPosition > 0 )
	{
		m_widget->page()->runJavaScript( QString( "document.body.scrollTop=%1" ).arg( m_storedScrollbarPosition )
		                                 , QWebEngineScript::UserWorld );
		m_storedScrollbarPosition = 0;
	}

	emit urlChanged( url() );
	emit loadFinished( success );
}

void WebEngineController::onLinkClicked(const QUrl& url, Browser::OpenMode mode)
{
	emit linkClicked( url, mode );
}

void WebEngineController::applySettings(Browser::Settings& settings)
{
	QWebEngineSettings* setup = QWebEngineProfile::defaultProfile()->settings();

	setup->setAttribute( QWebEngineSettings::AutoLoadImages, settings.enableImages );
	setup->setAttribute( QWebEngineSettings::JavascriptEnabled, settings.enableJS );
	setup->setAttribute( QWebEngineSettings::PluginsEnabled, settings.enablePlugins );
	setup->setAttribute( QWebEngineSettings::LocalStorageEnabled, settings.enableLocalStorage );
}

QWidget* WebEngineController::view()
{
	return m_widget;
}

bool WebEngineController::hasOption(Browser::Option option)
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

QUrl WebEngineController::url() const
{
	return m_widget->url();
}

void WebEngineController::back()
{
	m_widget->back();
}

void WebEngineController::forward()
{
	m_widget->forward();
}
