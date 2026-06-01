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

#include <functional>

#include <QApplication>
#include <QAtomicInt>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QMetaObject>
#include <QPalette>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QUrl>
#include <QVariant>
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineScript>
#include <QWebEngineView>
#include <Qt>
#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
	#include <QWebEngineFindTextResult>
#endif

class QPoint;

#include <ubrowser/abstractbrowser.hpp>
#include <ubrowser/browser.hpp>
#include <ubrowser/contentprovider.hpp>

#include "qtwebenginepage.h"
#include "qtwebenginewidget.h"
#include "qtwebengineurlschemehandler.h"

#include "qtwebenginebrowser.h"


#define BROWSER_SCRIPT_WORLD QWebEngineScript::UserWorld


QtWebEngine::Browser::Browser( UBrowser::ContentProvider::Ptr content, QWidget* parentWidget, QObject* parent )
	: UBrowser::AbstractBrowser( content, parent )
{
	m_page = new QtWebEngine::Page( content, this );
	QWebChannel* channel = new QWebChannel( m_page );
	channel->registerObject( "WebEngineBrowser", this );
	m_page->setWebChannel( channel, BROWSER_SCRIPT_WORLD );
	connect( m_page, &QtWebEngine::Page::loadStarted,
	         this, &QtWebEngine::Browser::onLoadStarted, Qt::QueuedConnection );
	connect( m_page, &QtWebEngine::Page::loadFinished,
	         this, &QtWebEngine::Browser::onLoadFinished, Qt::QueuedConnection );
	connect( m_page, &QtWebEngine::Page::urlChanged, m_page, [this]( const QUrl & url )
	         {
	             onUrlChanged( convertUrlForEbook( url ) );
	         }, Qt::QueuedConnection );
	connect( m_page, &QtWebEngine::Page::linkClicked, m_page, [this]( const QUrl & link, UBrowser::OpenMode mode )
	         {
	             onLinkClicked( convertUrlForEbook( link ), mode );
	         }, Qt::QueuedConnection );

	m_widget = new QtWebEngine::Widget( parentWidget );
	m_widget->setPage( m_page );
	connect( m_widget, &QtWebEngine::Widget::contextMenuRequested, m_widget, [this]( const QPoint & globalPos, const QUrl & link )
	         {
	             onContextMenuRequested( globalPos, convertUrlForEbook( link ) );
	         } );

	// Search results highlighter
	QPalette pal = m_widget->palette();
	pal.setColor( QPalette::Inactive, QPalette::Highlight, pal.color( QPalette::Active, QPalette::Highlight ) );
	pal.setColor( QPalette::Inactive, QPalette::HighlightedText, pal.color( QPalette::Active, QPalette::HighlightedText ) );
	m_widget->setPalette( pal );
}

QtWebEngine::Browser::~Browser()
{
}

QString QtWebEngine::Browser::kind() const
{
	return UBROWSER_KIND_HTML;
}

QWidget* QtWebEngine::Browser::view()
{
	return m_widget;
}

bool QtWebEngine::Browser::testOption( UBrowser::Option option )
{
	switch ( option )
	{
	case UBrowser::Option::high_light_search_result:
	case UBrowser::Option::images:
	case UBrowser::Option::javascript:
	case UBrowser::Option::java:
		return true;

	default:
		return false;
	}
}

bool QtWebEngine::Browser::testFeature( UBrowser::Feature feature )
{
	switch ( feature )
	{
	case UBrowser::Feature::copy:
	case UBrowser::Feature::print:
	case UBrowser::Feature::scroll:
	case UBrowser::Feature::search:
	case UBrowser::Feature::zoom:
		return true;

	default:
		return false;
	}
}

bool QtWebEngine::Browser::hasOption( UBrowser::Option option )
{
	return testOption( option );
}

bool QtWebEngine::Browser::hasFeature( UBrowser::Feature feature ) const
{
	return testFeature( feature );
}

void QtWebEngine::Browser::print( QPrinter* printer, std::function<void ( bool success )> result )
{
#if QT_VERSION < QT_VERSION_CHECK(6, 2, 0)
	m_page->print( printer,
	               [&result]( bool success ) { result( success ); } );
#else
	connect( m_widget, &QWebEngineView::printFinished,
	         [result]( bool success ) { result( success ); } );
	m_widget->print( printer );
#endif
}

qreal QtWebEngine::Browser::zoomFactor() const
{
	return  m_page->zoomFactor();
}

void QtWebEngine::Browser::realSetZoomFactor( qreal zoom )
{
	m_page->setZoomFactor( zoom );
}

int QtWebEngine::Browser::scrollTop()
{
	QAtomicInt value = -1;

	m_page->runJavaScript( "document.body.scrollTop",
	                       BROWSER_SCRIPT_WORLD,
	                       [&value]( const QVariant & v ) { value = v.toInt(); } );

	while ( value == -1 )
		QApplication::processEvents();

	return value;
}

void QtWebEngine::Browser::setScrollTop( int pos )
{
	m_page->runJavaScript( QString( "window.scrollTo(0, %1)" ).arg( pos )
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
	m_page->findText( text, webkitflags,
	                  [ = ]( bool found ) { result( found, false ); } );
#else
	m_page->findText( text, webkitflags,
	                  [ = ]( const QWebEngineFindTextResult & found )
	                  { result( found.numberOfMatches() > 0, false ); } );
#endif
}

void QtWebEngine::Browser::selectAll()
{
	m_page->triggerAction( QWebEnginePage::SelectAll );
}

QString QtWebEngine::Browser::selectedText() const
{
	return m_page->selectedText();
}

QString QtWebEngine::Browser::title() const
{
	QString title = m_page->title();

	// If no title is found, use the path (without the first /)
	if ( title.isEmpty() )
		title = url().path().mid( 1 );

	return title;
}

void QtWebEngine::Browser::heuristicPageRevealJS()
{
	QMetaObject::invokeMethod( this, "onPageReveal", Qt::QueuedConnection );
}

void QtWebEngine::Browser::scrollEventJS( int scrollY )
{
	QMetaObject::invokeMethod( this, "onScrollChanged", Qt::QueuedConnection, Q_ARG( int, scrollY ) );
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

	m_page->runJavaScript( js, BROWSER_SCRIPT_WORLD );
}

void QtWebEngine::Browser::loadPage( const QUrl& url )
{
	m_page->load( convertUrlForBrowser( url ) );
}

QUrl QtWebEngine::Browser::convertUrlForEbook( const QUrl& browserUrl )
{
	QUrl result{browserUrl};

	if ( result.scheme() == EBOOK_URL_SCHEME && contentProvider() )
		result.setScheme( contentProvider()->urlScheme() );

	return result;
}

QUrl QtWebEngine::Browser::convertUrlForBrowser( const QUrl& ebookUrl )
{
	QUrl result{ebookUrl};
	result.setScheme( EBOOK_URL_SCHEME );
	return result;
}
