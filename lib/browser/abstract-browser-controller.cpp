/*
 *  uChmViewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2022  Nick Egorrov, nicegorov@yandex.ru
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

#include <QClipboard>
#include <QMetaObject>      // QMetaObject::invokeMethod()
#include <QString>
#include <Qt>               // Qt::ShiftModifier, Qt::ControlModifier
#include <QtGlobal>         // Q_UNUSED
#include <QGuiApplication>  // QGuiApplication::keyboardModifiers()
#include <QUrl>

#include <browser/abstract-browser-controller.hpp>
#include <browser/browser-history-impl.hpp>


AbstractBrowserController::AbstractBrowserController(ContentProviderPtr contentProvider,
                                                     QObject* parent) :
	BrowserController(contentProvider, parent)
{
	m_history = new BrowserHistoryImpl(this);
	connect(m_history,
	        &BrowserHistoryImpl::restoreRequest,
	        this,
	        &AbstractBrowserController::loadFromHistory);
	m_applyAutoScroll = false;
	m_recoveryIndex = -1;
	m_recoveryMode = false;
	m_requestUrl.clear();
	m_currentUrl.clear();
	m_hoveredUrl.clear();
}

AbstractBrowserController::~AbstractBrowserController()
{
}

QUrl AbstractBrowserController::url() const
{
	return m_currentUrl;
}

void AbstractBrowserController::load(const QUrl& url)
{
	if (!m_history->isEmpty()) {
		m_history->currentItem().scroll = scrollTop();
	}

	m_recoveryMode = false;
	m_requestUrl = url;
	loadImpl(url);
}

void AbstractBrowserController::reload()
{
	if (!m_requestUrl.isEmpty()) {
		load(m_requestUrl);
	} else if (!m_history->isEmpty()) {
		m_history->currentItem().scroll = scrollTop();
		loadFromHistory(m_history->currentItemIndex());
	}
}

void AbstractBrowserController::loadFromHistory(int index)
{
	m_recoveryIndex = index;
	m_recoveryMode = true;
	m_autoScroll = m_history->item(m_recoveryIndex).scroll;
	m_applyAutoScroll = true;
	m_requestUrl = m_history->item(index).url;
	loadImpl(m_requestUrl);
}

void AbstractBrowserController::onLoadFinished(bool ok)
{
	if (ok) {
		m_currentUrl = m_requestUrl;
		m_requestUrl.clear();

		if (m_applyAutoScroll) {
			setScrollTop(m_autoScroll);
			m_applyAutoScroll = false;
		}

		if (m_recoveryMode) {
			m_history->restoreResponse(m_recoveryIndex);
		} else {
			m_history->add({m_currentUrl, scrollTop(), title()});
		}
	}

	m_recoveryMode = false;
	emit loadFinished(this, ok);
}

BrowserHistory* AbstractBrowserController::history() const
{
	return m_history;
}

bool AbstractBrowserController::canGoBack() const
{
	return m_history->canGoBack();
}

bool AbstractBrowserController::canGoForward() const
{
	return m_history->canGoForward();
}

void AbstractBrowserController::back()
{
	m_history->back();
}

void AbstractBrowserController::forward()
{
	m_history->forward();
}

qreal AbstractBrowserController::zoomFactor() const
{
	return 1.0;
}

void AbstractBrowserController::setZoomFactor(qreal zoom)
{
	Q_UNUSED(zoom);
}

int AbstractBrowserController::scrollTop()
{
	return 0;
}

void AbstractBrowserController::setScrollTop(int pos)
{
	Q_UNUSED(pos);
}

void AbstractBrowserController::setAutoScroll(int pos)
{
	if (pos >= 0) {
		m_autoScroll = pos;
		m_applyAutoScroll = true;
	}
}

void AbstractBrowserController::selectAll()
{
	;
}

QString AbstractBrowserController::selectedText() const
{
	return{};
}

void AbstractBrowserController::selectedCopy() const
{
	QString text = selectedText();

	if (!text.isEmpty()) {
		QClipboard* clip = QGuiApplication::clipboard();
		clip->setText(text);
	}
}

void AbstractBrowserController::findText(
    const QString& text,
    const BrowserController::FindFlags& flags)
{
	Q_UNUSED(text);
	Q_UNUSED(flags);

	QMetaObject::invokeMethod(
	    this,
	    "onFindTextFinished",
	    Qt::QueuedConnection,
	    Q_ARG(bool, false));
}

void AbstractBrowserController::print(QPrinter* printer)
{
	Q_UNUSED(printer);

	QMetaObject::invokeMethod(
	    this,
	    "onPrintFinished",
	    Qt::QueuedConnection,
	    Q_ARG(bool, false));
}

void AbstractBrowserController::onUrlChanged(const QUrl& newUrl)
{
	m_currentUrl = newUrl;
	emit urlChanged(this, m_currentUrl);
}

void AbstractBrowserController::onLinkHovered(const QUrl link)
{
	m_hoveredUrl = link;
}

void AbstractBrowserController::onContextMenuRequested(const QPoint& pos)
{
	emit contextMenuRequested(this, pos, m_hoveredUrl);
}

void AbstractBrowserController::onLinkClick(QUrl link)
{
	if (link.isEmpty()) {
		return;
	}

	Qt::KeyboardModifiers mods = QGuiApplication::keyboardModifiers();
	Browser::OpenFlag flag = Browser::OPEN_IN_CURRENT;

	if (mods & Qt::ShiftModifier) {
		flag = Browser::OPEN_IN_NEW;
	} else if (mods & Qt::ControlModifier) {
		flag = Browser::OPEN_IN_BACKGROUND;
	}

	emit linkClicked(this, link, flag);
}

void AbstractBrowserController::onLinkClick()
{
	onLinkClick(m_hoveredUrl);
}

void AbstractBrowserController::onFindTextFinished(bool hit, int count, int pos)
{
	emit findTextFinished(this, hit, count, pos);
}

void AbstractBrowserController::onFindTextFinished(bool hit)
{
	onFindTextFinished(hit, -1, -1);
}

void AbstractBrowserController::onPrintFinished(bool ok)
{
	emit printFinished(this, ok);
}
