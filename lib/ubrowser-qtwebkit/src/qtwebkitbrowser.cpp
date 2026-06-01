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

#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QPalette>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QUrl>
#include <QWebFrame>
#include <QWebPage>
#include <Qt>
#include <QtGlobal>


#include <ubrowser/abstractbrowser.hpp>
#include <ubrowser/browser.hpp>
#include <ubrowser/contentprovider.hpp>

#include "qtwebkitnetwork.h"
#include "qtwebkitwidget.h"

#include "qtwebkitbrowser.h"


QtWebKit::Browser::Browser( UBrowser::ContentProvider::Ptr provider, QWidget* parentWidget, QObject* parent )
	: UBrowser::AbstractBrowser( provider, parent )
{
	m_widget = new QtWebKit::Widget( parentWidget );
	// Use our network emulation layer
	m_widget->page()->setNetworkAccessManager( new NetworkAccessManager( contentProvider(), this ) );
	// All links are going through us
	m_widget->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );

	connect( m_widget, &QtWebKit::Widget::loadStarted,
	         this, &QtWebKit::Browser::onLoadStarted, Qt::QueuedConnection );
	connect( m_widget, &QtWebKit::Widget::loadFinished,
	         this, &QtWebKit::Browser::onLoadFinished, Qt::QueuedConnection );
	connect( m_widget->page()->mainFrame(), &QWebFrame::initialLayoutCompleted,
	         this, &QtWebKit::Browser::onPageReveal, Qt::QueuedConnection );
	connect( m_widget, &QtWebKit::Widget::urlChanged,
	         this, &QtWebKit::Browser::onUrlChanged, Qt::QueuedConnection );
	connect( m_widget, &QtWebKit::Widget::linkClicked,
	         this, &QtWebKit::Browser::onLinkClicked );
	connect( m_widget, &QtWebKit::Widget::contextMenuRequested,
	         this, &QtWebKit::Browser::onContextMenuRequested );

	// Search results highlighter
	QPalette pal = m_widget->palette();
	pal.setColor( QPalette::Inactive, QPalette::Highlight, pal.color( QPalette::Active, QPalette::Highlight ) );
	pal.setColor( QPalette::Inactive, QPalette::HighlightedText, pal.color( QPalette::Active, QPalette::HighlightedText ) );
	m_widget->setPalette( pal );
}

QtWebKit::Browser::~Browser()
{
}

QString QtWebKit::Browser::kind() const
{
	return UBROWSER_KIND_HTML;
}

QWidget* QtWebKit::Browser::view()
{
	return m_widget;
}

bool QtWebKit::Browser::testOption( UBrowser::Option option )
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

bool QtWebKit::Browser::testFeature( UBrowser::Feature feature )
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

bool QtWebKit::Browser::hasOption( UBrowser::Option option )
{
	return testOption( option );
}

bool QtWebKit::Browser::hasFeature( UBrowser::Feature feature ) const
{
	return testFeature( feature );
}

void QtWebKit::Browser::print( QPrinter* printer, std::function<void ( bool success )> result )
{
	m_widget->print( printer );
	result( true );
}

qreal QtWebKit::Browser::zoomFactor() const
{
	return m_widget->zoomFactor();
}

int QtWebKit::Browser::scrollTop()
{
	return m_widget->page()->currentFrame()->scrollBarValue( Qt::Vertical );
}

void QtWebKit::Browser::realSetZoomFactor( qreal zoom )
{
	m_widget->setZoomFactor( zoom );
}

void QtWebKit::Browser::setScrollTop( int pos )
{
	m_widget->page()->currentFrame()->setScrollBarValue( Qt::Vertical, pos );
}

void QtWebKit::Browser::findText( const QString& text,
                                  bool backward,
                                  bool caseSensitively,
                                  bool highlightSearchResults,
                                  std::function<void ( bool found, bool wrapped )> result )
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
			m_widget->findText( "", webkitflags | QWebPage::HighlightAllOccurrences );

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

void QtWebKit::Browser::selectAll()
{
	m_widget->triggerPageAction( QWebPage::SelectAll );
}

QString QtWebKit::Browser::selectedText() const
{
	return m_widget->selectedText();
}

QString QtWebKit::Browser::title() const
{
	QString title = m_widget->page()->mainFrame()->title();

	// If no title is found, use the path (without the first /)
	if ( title.isEmpty() )
		title = url().path().mid( 1 );

	return title;
}

void QtWebKit::Browser::injectJS()
{
	QString js;
	const QStringList& jsfiles = QStringList
	{
		{":/ubrowser/qtwebkitbrowser.js"}
	};

	for ( const QString& f : jsfiles )
	{
		QFile* io = new QFile{f};
		QTextStream ts{io};

		if ( io->open( QIODevice::ReadOnly ) )
			js.append( ts.readAll() );
		else
			qWarning() << "ERROR in QtWebKit::Browser::injectJS()\n"
			           << "  Unable to load the '" << f << "' file.";

		io->close();
		io->deleteLater();
	}

	// https://doc.qt.io/archives/qt-5.5/qtwebkit-bridge.html
	// https://doc.qt.io/archives/qt-5.5/qwebframe.html
	m_widget->page()->currentFrame()->addToJavaScriptWindowObject( "QtWebKitBrowser", this );
	m_widget->page()->currentFrame()->evaluateJavaScript( js );
}

void QtWebKit::Browser::loadPage( const QUrl& url )
{
	m_widget->load( url );
}
