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

#include <QPalette>     // for QPalette, QPalette::Active, QPalette::Highlight, QPalette::HighlightedText, QPalette::Inactive
#include <QString>      // for QString, operator!=
#include <QUrl>         // for QUrl
#include <QWebFrame>    // for QWebFrame
#include <QWebHistory>  // for QWebHistory
#include <QWebPage>     // for QWebPage, QWebPage::FindFlags, QWebPage::HighlightAllOccurrences, QWebPage::Copy, QWebPage::DelegateAllLinks
#include <QWebSettings> // for QWebSettings, QWebSettings::AutoLoadImages, QWebSettings::JavaEnabled, QWebSettings::JavascriptEnabled, QWeb...
#include <QWidget>      // for QWidget
#include <Qt>           // for Vertical
#include <QtGlobal>     // for QFlags, qreal

class QPoint;
class QPrinter;

#include <browser/content-provider.hpp> // for ContentProvider::Ptr
#include <browser/controller.hpp>       // for Controller
#include <browser/settings.hpp>         // for Settings
#include <browser/types.hpp>            // for OPTION_HIGH_LIGHT_SEARCH_RESULT, OPTION_IMAGES, OPTION_JAVA, OPTION_JAVA_SCRIPT, OpenFlag, Option
#include <ebook.h>      // for EBook

#include "../mainwindow.h"      // for MainWindow, mainWindow
#include "dataprovider.h"       // for KCHMNetworkAccessManager
#include "webkitcontroller.h"
#include "webkitwidget.h"       // for WebKitWidget


WebKitController::WebKitController(QWidget* parent)
	: Browser::Controller(nullptr, parent)
{
	m_widget = new WebKitWidget(this);
	invalidate();
	m_storedScrollbarPosition = 0;

	// Use our network emulation layer
	m_widget->page()->setNetworkAccessManager(new KCHMNetworkAccessManager(this));

	// All links are going through us
	m_widget->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

	connect(m_widget, &WebKitWidget::loadFinished,
	        [this](bool success)
	{
		if (m_storedScrollbarPosition > 0) {
			m_widget->page()->currentFrame()->setScrollBarValue(Qt::Vertical, m_storedScrollbarPosition);
			m_storedScrollbarPosition = 0;
		}

		emit loadFinished(success);
	});
	connect(m_widget, &WebKitWidget::linkClicked,
	        [this](const QUrl & link, Browser::OpenMode mode)
	{
		emit linkClicked(link, mode);
	});
	connect(m_widget, &WebKitWidget::contextMenuRequested,
	        [this](const QPoint & globalPos, const QUrl & link)
	{
		emit contextMenuRequested(globalPos, link);
	});
	connect(m_widget, &WebKitWidget::urlChanged,
	        [this](const QUrl & link)
	{
		emit urlChanged(link);
	});

	// Search results highlighter
	QPalette pal = m_widget->palette();
	pal.setColor(QPalette::Inactive, QPalette::Highlight, pal.color(QPalette::Active, QPalette::Highlight));
	pal.setColor(QPalette::Inactive, QPalette::HighlightedText, pal.color(QPalette::Active, QPalette::HighlightedText));
	m_widget->setPalette(pal);
}

WebKitController::~WebKitController()
{
}

void WebKitController::invalidate()
{
	m_storedScrollbarPosition = 0;
	m_widget->reload();
}

void WebKitController::load (const QUrl& url)
{
	// Do not use setContent() here, it resets QWebHistory
	m_widget->load( url );
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
	QString title = ::mainWindow->chmFile()->getTopicByUrl(url());

	// If no title is found, use the path (without the first /)
	if (title.isEmpty()) {
		title = url().path().mid(1);
	}

	return title;
}

bool WebKitController::canGoBack() const
{
	return m_widget->history()->canGoBack();
}

bool WebKitController::canGoForward() const
{
	return m_widget->history()->canGoForward();
}

void WebKitController::print(QPrinter* printer, std::function<void (bool succes)> result)
{
	m_widget->print(printer);
	result(true);
}

void WebKitController::setZoomFactor(qreal zoom)
{
	m_widget->setZoomFactor( zoom );
}

qreal WebKitController::zoomFactor() const
{
	return m_widget->zoomFactor();
}

int WebKitController::scrollTop()
{
	return m_widget->page()->currentFrame()->scrollBarValue(Qt::Vertical);
}

void WebKitController::setScrollTop(int pos)
{
	m_widget->page()->currentFrame()->setScrollBarValue(Qt::Vertical, pos);
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

	if (caseSensitively) {
		webkitflags |= QWebPage::FindCaseSensitively;
	}

	if (backward) {
		webkitflags |= QWebPage::FindBackward;
	}

	if (highlightSearchResults) {
		// From the doc:
		// If the HighlightAllOccurrences flag is passed, the
		// function will highlight all occurrences that exist
		// in the page. All subsequent calls will extend the
		// highlight, rather than replace it, with occurrences
		// of the new string.

		// If the search text is different, we run the empty string search
		// to discard old highlighting
		if (m_lastSearchedWord != text) {
			m_widget->findText("", webkitflags | QWebPage::HighlightAllOccurrences);
		}

		m_lastSearchedWord = text;

		// Now we call search with highlighting enabled, while the main search below will have
		// it disabled. This leads in both having the highlighting results AND working forward/
		// backward buttons.
		m_widget->findText(text, webkitflags | QWebPage::HighlightAllOccurrences);
	}

	bool found = m_widget->findText(text, webkitflags);
	bool wrapped = false;

	// If we didn't find anything, enable the wrap and try again
	if (!found) {
		found = m_widget->findText(text, webkitflags | QWebPage::FindWrapsAroundDocument);
		wrapped = found;
	}

	result(found, wrapped);
}

void WebKitController::selectAll()
{
	m_widget->triggerPageAction(QWebPage::SelectAll);
}

void WebKitController::selectedCopy()
{
	m_widget->triggerPageAction(QWebPage::Copy);
}

QWidget* WebKitController::view()
{
	return m_widget;
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
	return m_widget->url();
}

void WebKitController::back()
{
	m_widget->back();
}

void WebKitController::forward()
{
	m_widget->forward();
}
