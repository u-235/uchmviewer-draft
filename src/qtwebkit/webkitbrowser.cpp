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

#include <QPalette>
#include <QString>
#include <QUrl>
#include <QWebFrame>
#include <QWebPage>
#include <QWebSettings>
#include <Qt>
#include <QtGlobal>

class QPrinter;

#include <ubrowser/abstractbrowser.hpp>
#include <ubrowser/contentprovider.hpp>
#include <ubrowser/settings.hpp>
#include <ubrowser/types.hpp>

#include "dataprovider.h"
#include "webkitwidget.h"

#include "webkitbrowser.h"


WebKitBrowser::WebKitBrowser( UBrowser::ContentProvider::Ptr provider, QObject* parent )
	: UBrowser::AbstractBrowser( provider, parent )
{
	m_widget = new WebKitWidget( nullptr );

	// Use our network emulation layer
	m_widget->page()->setNetworkAccessManager( new KCHMNetworkAccessManager( contentProvider(), this ) );

	// All links are going through us
	m_widget->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );

	connect( m_widget, &WebKitWidget::loadStarted,
	         this, &WebKitBrowser::onLoadStarted );
	connect( m_widget, &WebKitWidget::loadFinished,
	         this, &WebKitBrowser::onLoadFinished ); //, Qt::QueuedConnection );
	connect( m_widget->page()->mainFrame(), &QWebFrame::initialLayoutCompleted,
	         this, &WebKitBrowser::onPageReveal );
	connect( m_widget, &WebKitWidget::urlChanged,
	         this, &WebKitBrowser::onUrlChanged );
	connect( m_widget, &WebKitWidget::linkClicked,
	         this, &WebKitBrowser::onLinkClicked );
	connect( m_widget, &WebKitWidget::contextMenuRequested,
	         this, &WebKitBrowser::onContextMenuRequested );

	// Search results highlighter
	QPalette pal = m_widget->palette();
	pal.setColor( QPalette::Inactive, QPalette::Highlight, pal.color( QPalette::Active, QPalette::Highlight ) );
	pal.setColor( QPalette::Inactive, QPalette::HighlightedText, pal.color( QPalette::Active, QPalette::HighlightedText ) );
	m_widget->setPalette( pal );
}

WebKitBrowser::~WebKitBrowser()
{
}

void WebKitBrowser::applySettings( UBrowser::Settings& settings )
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

QWidget* WebKitBrowser::view()
{
	return m_widget;
}

bool WebKitBrowser::hasOption( UBrowser::Option option )
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

bool WebKitBrowser::hasFeature( UBrowser::Feature feature ) const
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

void WebKitBrowser::print( QPrinter* printer, std::function<void ( bool success )> result )
{
	m_widget->print( printer );
	result( true );
}

qreal WebKitBrowser::zoomFactor() const
{
	return m_widget->zoomFactor();
}

void WebKitBrowser::setZoomFactorImpl( qreal zoom )
{
	m_widget->setZoomFactor( zoom );
}

void WebKitBrowser::setScrollTop( int pos )
{
	m_widget->page()->currentFrame()->setScrollBarValue( Qt::Vertical, pos );
}

void WebKitBrowser::findText( const QString& text,
                              bool backward,
                              bool caseSensitively,
                              bool highlightSearchResults,
                              std::function<void ( bool found, bool wrapped )> result )
{
	QWebPage::FindFlags webkitflags;

	if ( caseSensitively )
	{
		webkitflags |= QWebPage::FindCaseSensitively;
	}

	if ( backward )
	{
		webkitflags |= QWebPage::FindBackward;
	}

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
		{
			m_widget->findText( "", webkitflags | QWebPage::HighlightAllOccurrences );
		}

		m_lastSearchedWord = text;

		// Now we call search with highlighting enabled, while the main search below will have
		// it disabled. This leads in both having the highlighting results AND working forward/
		// backward buttons.
		m_widget->findText( text, webkitflags | QWebPage::HighlightAllOccurrences );
	}

	bool found = m_widget->findText( text, webkitflags );
	bool wrapped = false;

	// If we didn't find anything, enable the wrap and try again
	if ( !found )
	{
		found = m_widget->findText( text, webkitflags | QWebPage::FindWrapsAroundDocument );
		wrapped = found;
	}

	result( found, wrapped );
}

void WebKitBrowser::selectAll()
{
	m_widget->triggerPageAction( QWebPage::SelectAll );
}

QString WebKitBrowser::selectedText() const
{
	return m_widget->selectedText();
}

QString WebKitBrowser::title() const
{
	QString title = contentProvider()->topicTitle( url() );

	if ( title.isEmpty() )
		title = m_widget->page()->mainFrame()->title();

	// If no title is found, use the path (without the first /)
	if ( title.isEmpty() )
		title = url().path().mid( 1 );

	return title;
}

void WebKitBrowser::loadImpl( const QUrl& url )
{
	// Do not use setContent() here, it resets QWebHistory
	m_widget->load( url );
}
