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

#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QPalette>
#include <QString>
#include <QTextStream>
#include <QUrl>
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineSettings>
#include <QWebEngineView>
#include <QtGlobal>


#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
	#include <QWebEngineFindTextResult>
#endif

class QPrinter;

#include <ubrowser/abstractbrowser.hpp>
#include <ubrowser/contentprovider.hpp>
#include <ubrowser/settings.hpp>
#include <ubrowser/types.hpp>

#include "dataprovider.h"
#include "webenginepage.h"
#include "webenginewidget.h"

#include "webenginebrowser.h"


#define BROWSER_SCRIPT_WORLD QWebEngineScript::UserWorld


WebEngineBrowser::WebEngineBrowser( UBrowser::ContentProvider::Ptr content, QObject* parent )
	: UBrowser::AbstractBrowser( content, parent )
{
	m_widget = new WebEngineWidget( nullptr );

	QWebEngineProfile* pf = new QWebEngineProfile( this );

	if ( pf->urlSchemeHandler( EBOOK_URL_SCHEME ) == nullptr )
		pf->installUrlSchemeHandler( EBOOK_URL_SCHEME, new DataProvider( content, m_widget ) );

	WebEnginePage* page = new WebEnginePage( pf, m_widget );
	QWebChannel* channel = new QWebChannel( this );
	channel->registerObject( "WebEngineBrowser", this );
	page->setWebChannel( channel, BROWSER_SCRIPT_WORLD );
	m_widget->setPage( page );

	connect( m_widget, &WebEngineWidget::loadStarted,
	         this, &WebEngineBrowser::onLoadStarted );
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

void WebEngineBrowser::setZoomFactorImpl( qreal zoom )
{
	m_widget->setZoomFactor( zoom );
}

void WebEngineBrowser::setScrollTop( int pos )
{
	m_widget->page()->runJavaScript( QString( "window.scrollTo(0, %1)" ).arg( pos )
	                                 , BROWSER_SCRIPT_WORLD );
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

QString WebEngineBrowser::selectedText() const
{
	return m_widget->selectedText();
}

QString WebEngineBrowser::title() const
{
	QString title = contentProvider()->topicTitle( url() );

	if ( title.isEmpty() )
		title = m_widget->page()->title();

	// If no title is found, use the path (without the first /)
	if ( title.isEmpty() )
		title = url().path().mid( 1 );

	return title;
}

void WebEngineBrowser::injectJS()
{
	QString js;
	QFile webchannelFile {":/qtwebchannel/qwebchannel.js"};
	QTextStream webchannelTS {&webchannelFile};

	if ( webchannelFile.open( QIODevice::ReadOnly ) )
		js.append( webchannelTS.readAll() );
	else
		qWarning() << "ERROR in WebEngineBrowser::injectJS()\n"
		           << "  Unable to load the qwebchannel.js file.";

	webchannelFile.close();
	QFile ubrowserFile {":/ubrowser/webenginebrowser.js"};
	QTextStream ubrowserTS {&ubrowserFile};

	if ( ubrowserFile.open( QIODevice::ReadOnly ) )
		js.append( ubrowserTS.readAll() );
	else
		qWarning() << "ERROR in WebEngineBrowser::injectJS()\n"
		           << "  Unable to load the webenginebrowser.js file.";

	ubrowserFile.close();
	m_widget->page()->runJavaScript( js, BROWSER_SCRIPT_WORLD );
}

void WebEngineBrowser::loadImpl( const QUrl& url )
{
	m_widget->load( url );
}
