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
#include <QAtomicInt>
#include <QPalette>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <QWebEngineHistory>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineSettings>
#include <QWebEngineView>
#include <QWidget>
#include <QtGlobal>


#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
	#include <QWebEngineFindTextResult>
#endif

class QPrinter;
class QPoint;

#include <ebook.h>
#include <ubrowser/browser.hpp>
#include <ubrowser/settings.hpp>
#include <ubrowser/types.hpp>

#include "webenginepage.h"
#include "webenginewidget.h"

#include "webenginebrowser.h"


WebEngineBrowser::WebEngineBrowser( UBrowser::ContentProvider::Ptr contentProvider, QObject* parent )
	: UBrowser::Browser( contentProvider, parent )
{
	m_widget = new WebEngineWidget( nullptr );
	m_storedScrollbarPosition = 0;

	QWebEngineProfile* pf = new QWebEngineProfile( this );

	if ( pf->urlSchemeHandler( UBrowser::ContentProvider::URL_SCHEME ) == nullptr )
		pf->installUrlSchemeHandler( UBrowser::ContentProvider::URL_SCHEME, new DataProvider( contentProvider, m_widget ) );

	WebEnginePage* page = new WebEnginePage( pf, m_widget );
	m_widget->setPage( page );

	connect( m_widget, &WebEngineWidget::loadFinished,
	         this, &WebEngineBrowser::onLoadFinished );
	connect( m_widget, &WebEngineWidget::urlChanged,
	         this, &WebEngineBrowser::onUrlChanged );
	connect( page, &WebEnginePage::linkClicked,
	         this, &WebEngineBrowser::onLinkClicked );
	connect( m_widget, &WebEngineWidget::contextMenuRequested,
	         this, &WebEngineBrowser::onContextMenuRequested );

	// Search results highlighter
	QPalette pal = m_widget->palette();
	pal.setColor( QPalette::Inactive, QPalette::Highlight, pal.color( QPalette::Active, QPalette::Highlight ) );
	pal.setColor( QPalette::Inactive, QPalette::HighlightedText, pal.color( QPalette::Active, QPalette::HighlightedText ) );
	m_widget->setPalette( pal );
}

WebEngineBrowser::~WebEngineBrowser()
{
}

void WebEngineBrowser::invalidate( )
{
	m_storedScrollbarPosition = 0;
	reload();
}

void WebEngineBrowser::load( const QUrl& url )
{
	// Do not use setContent() here, it resets QWebHistory
	m_widget->load( m_contentProvider->convertUrlForBrowser( url ) );
}

QUrl WebEngineBrowser::url() const
{
	return m_contentProvider->convertUrlForEbook( m_widget->url() );
}

void WebEngineBrowser::reload()
{
	m_widget->reload();
}

QString WebEngineBrowser::title() const
{
	QString title = m_contentProvider->topicTitle( url() );

	if ( title.isEmpty() )
		title = m_widget->page()->title();

	// If no title is found, use the path (without the first /)
	if ( title.isEmpty() )
		title = url().path().mid( 1 );

	return title;
}

bool WebEngineBrowser::canGoBack() const
{
	return  m_widget->history()->canGoBack();
}

bool WebEngineBrowser::canGoForward() const
{
	return  m_widget->history()->canGoForward();
}

void WebEngineBrowser::back()
{
	m_widget->back();
}

void WebEngineBrowser::forward()
{
	m_widget->forward();
}

void WebEngineBrowser::print( QPrinter* printer, std::function<void ( bool success )> result )
{
#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
	m_widget->page()->print( printer,
	                         [&result]( bool success ) { result( success ); } );
#else
	connect( m_widget, &QWebEngineView::printFinished,
	         [result]( bool success ) { result( success ); } );
	m_widget->print( printer );
#endif
}

qreal WebEngineBrowser::zoomFactor() const
{
	return  m_widget->zoomFactor();
}

void WebEngineBrowser::setZoomFactor( qreal zoom )
{
	m_widget->setZoomFactor( zoom );
}

void WebEngineBrowser::zoomIncrease()
{
	setZoomFactor( zoomFactor() + ZOOM_STEP );
}

void WebEngineBrowser::zoomDecrease()
{
	setZoomFactor( zoomFactor() - ZOOM_STEP );
}

int WebEngineBrowser::scrollTop()
{
	QAtomicInt value = -1;

	m_widget->page()->runJavaScript( "window.scrollY",
	                                 QWebEngineScript::UserWorld,
	                                 [&value]( const QVariant & v ) { value = v.toInt(); } );

	while ( value == -1 )
	{
		QApplication::processEvents();
	}

	return value;
}

void WebEngineBrowser::setScrollTop( int pos )
{
	m_widget->page()->runJavaScript( QString( "window.scrollTo(0, %1)" ).arg( pos )
	                                 , QWebEngineScript::UserWorld );
}

void WebEngineBrowser::setAutoScroll( int pos )
{
	m_storedScrollbarPosition = pos;
}

void WebEngineBrowser::findText( const QString& text,
                                 bool backward,
                                 bool caseSensitively,
                                 bool highlightSearchResults,
                                 std::function<void ( bool found, bool wrapped )> result )
{
	Q_UNUSED( highlightSearchResults );
	QWebEnginePage::FindFlags webkitflags;

	if ( caseSensitively )
		webkitflags |= QWebEnginePage::FindCaseSensitively;

	if ( backward )
		webkitflags |= QWebEnginePage::FindBackward;

#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
	m_widget->findText( text, webkitflags,
	                    [ = ]( bool found ) { result( found, false ); } );
#else
	m_widget->findText( text, webkitflags,
	                    [ = ]( const QWebEngineFindTextResult & found )
	                    { result( found.numberOfMatches() > 0, false ); } );
#endif
}

void WebEngineBrowser::selectAll()
{
	m_widget->triggerPageAction( QWebEnginePage::SelectAll );
}

void WebEngineBrowser::selectedCopy()
{
	m_widget->triggerPageAction( QWebEnginePage::Copy );
}

QString WebEngineBrowser::selectedText() const
{
	return m_widget->selectedText();
}

void WebEngineBrowser::onLoadFinished( bool success )
{
	if ( m_storedScrollbarPosition > 0 )
	{
		setScrollTop( m_storedScrollbarPosition );
		m_storedScrollbarPosition = 0;
	}

	emit loadFinished( success );
}

void WebEngineBrowser::onUrlChanged( const QUrl& url )
{
	emit urlChanged( m_contentProvider->convertUrlForEbook( url ) );
}

void WebEngineBrowser::onLinkClicked( const QUrl& link, UBrowser::OpenMode mode )
{
	emit linkClicked( m_contentProvider->convertUrlForEbook( link ), mode );
}

void WebEngineBrowser::onContextMenuRequested( const QPoint& globalPos, const QUrl& link )
{
	emit contextMenuRequested( globalPos, m_contentProvider->convertUrlForEbook( link ) );
}

void WebEngineBrowser::applySettings( UBrowser::Settings& settings )
{
	QWebEngineSettings* setup = QWebEngineProfile::defaultProfile()->settings();

	setup->setAttribute( QWebEngineSettings::AutoLoadImages, settings.enableImages );
	setup->setAttribute( QWebEngineSettings::JavascriptEnabled, settings.enableJS );
	setup->setAttribute( QWebEngineSettings::PluginsEnabled, settings.enablePlugins );
	setup->setAttribute( QWebEngineSettings::LocalStorageEnabled, settings.enableLocalStorage );
}

QWidget* WebEngineBrowser::view()
{
	return m_widget;
}

bool WebEngineBrowser::hasOption( UBrowser::Option option )
{
	switch ( option )
	{
	case UBrowser::OPTION_HIGH_LIGHT_SEARCH_RESULT:
	case UBrowser::OPTION_IMAGES:
	case UBrowser::OPTION_JAVA_SCRIPT:
	case UBrowser::OPTION_JAVA:
		return true;

	default:
		return false;
	}
}

bool WebEngineBrowser::hasFeature( UBrowser::Feature feature ) const
{
	switch ( feature )
	{
	case UBrowser::FEATURE_COPY:
	case UBrowser::FEATURE_PRINT:
	case UBrowser::FEATURE_SCROLL:
	case UBrowser::FEATURE_SEARCH:
	case UBrowser::FEATURE_ZOOM:
		return true;

	default:
		return false;
	}
}

void WebEngineBrowser::configure( const UBrowser::Settings& settings )
{
	Q_UNUSED( settings );
}

UBrowser::History* WebEngineBrowser::history() const
{
	return nullptr;
}
