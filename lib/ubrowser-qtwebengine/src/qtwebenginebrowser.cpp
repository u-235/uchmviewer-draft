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

#include "qtwebengineurlschemehandler.h"
#include "qtwebenginepage.h"
#include "qtwebenginewidget.h"

#include "qtwebenginebrowser.h"


#define BROWSER_SCRIPT_WORLD QWebEngineScript::UserWorld


QtWebEngine::Browser::Browser( UBrowser::ContentProvider::Ptr content, QObject* parent )
	: UBrowser::AbstractBrowser( content, parent )
{
	m_fireRevealIfShow = true;
	m_widget = new QtWebEngine::Widget( nullptr );

	QWebEngineProfile* pf = new QWebEngineProfile( this );

	if ( pf->urlSchemeHandler( EBOOK_URL_SCHEME ) == nullptr )
		pf->installUrlSchemeHandler( EBOOK_URL_SCHEME, new QtWebEngine::UrlSchemeHandler( content, m_widget ) );

	QtWebEngine::Page* page = new QtWebEngine::Page( pf, m_widget );
	QWebChannel* channel = new QWebChannel( this );
	channel->registerObject( "WebEngineBrowser", this );
	page->setWebChannel( channel, BROWSER_SCRIPT_WORLD );
	m_widget->setPage( page );

	connect( m_widget, &QtWebEngine::Widget::loadStarted,
	         this, &QtWebEngine::Browser::onLoadStarted );
	connect( m_widget, &QtWebEngine::Widget::loadFinished,
	         this, &QtWebEngine::Browser::onLoadFinished );
	connect( m_widget, &QtWebEngine::Widget::urlChanged,
	         this, &QtWebEngine::Browser::onUrlChanged );
	connect( page, &QtWebEngine::Page::linkClicked,
	         this, &QtWebEngine::Browser::onLinkClicked );
	connect( m_widget, &QtWebEngine::Widget::contextMenuRequested,
	         this, &QtWebEngine::Browser::onContextMenuRequested );

	// Search results highlighter
	QPalette pal = m_widget->palette();
	pal.setColor( QPalette::Inactive, QPalette::Highlight, pal.color( QPalette::Active, QPalette::Highlight ) );
	pal.setColor( QPalette::Inactive, QPalette::HighlightedText, pal.color( QPalette::Active, QPalette::HighlightedText ) );
	m_widget->setPalette( pal );
}

QtWebEngine::Browser::~Browser()
{
}

void QtWebEngine::Browser::applySettings( UBrowser::Settings& settings )
{
	QWebEngineSettings* setup = QWebEngineProfile::defaultProfile()->settings();

	setup->setAttribute( QWebEngineSettings::AutoLoadImages, settings.enableImages );
	setup->setAttribute( QWebEngineSettings::JavascriptEnabled, settings.enableJS );
	setup->setAttribute( QWebEngineSettings::PluginsEnabled, settings.enablePlugins );
	setup->setAttribute( QWebEngineSettings::LocalStorageEnabled, settings.enableLocalStorage );
}

QWidget* QtWebEngine::Browser::view()
{
	return m_widget;
}

bool QtWebEngine::Browser::hasOption( UBrowser::Option option )
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

bool QtWebEngine::Browser::hasFeature( UBrowser::Feature feature ) const
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

void QtWebEngine::Browser::print( QPrinter* printer, std::function<void ( bool success )> result )
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

qreal QtWebEngine::Browser::zoomFactor() const
{
	return  m_widget->zoomFactor();
}

void QtWebEngine::Browser::setZoomFactorImpl( qreal zoom )
{
	m_widget->setZoomFactor( zoom );
}

void QtWebEngine::Browser::setScrollTop( int pos )
{
	m_widget->page()->runJavaScript( QString( "window.scrollTo(0, %1)" ).arg( pos )
	                                 , BROWSER_SCRIPT_WORLD );
}

void QtWebEngine::Browser::findText( const QString& text,
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

void QtWebEngine::Browser::selectAll()
{
	m_widget->triggerPageAction( QWebEnginePage::SelectAll );
}

QString QtWebEngine::Browser::selectedText() const
{
	return m_widget->selectedText();
}

QString QtWebEngine::Browser::title() const
{
	QString title = contentProvider()->topicTitle( url() );

	if ( title.isEmpty() )
		title = m_widget->page()->title();

	// If no title is found, use the path (without the first /)
	if ( title.isEmpty() )
		title = url().path().mid( 1 );

	return title;
}

void QtWebEngine::Browser::injectJS()
{
	QString js;
	const QStringList& jsfiles = QStringList
	{
		{":/qtwebchannel/qwebchannel.js"},
		{":/ubrowser/qtwebenginebrowser.js"}
	};

	for ( const QString& f : jsfiles )
	{
		QFile* io = new QFile{f};
		QTextStream ts{io};

		if ( io->open( QIODevice::ReadOnly ) )
			js.append( ts.readAll() );
		else
			qWarning() << "ERROR in WebEngineBrowser::injectJS()\n"
			           << "  Unable to load the " << f << " file.";

		io->close();
		io->deleteLater();
	}

	m_widget->page()->runJavaScript( js, BROWSER_SCRIPT_WORLD );
}

void QtWebEngine::Browser::loadImpl( const QUrl& url )
{
	m_widget->load( url );
}
