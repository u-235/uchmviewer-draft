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

#include <QClipboard>       // for QClipboard
#include "QGuiApplication"  // for QGuiApplication
#include <QMetaObject>      // for QMetaObject
#include <QString>          // for QString
#include <QUrl>             // for QUrl
#include <Qt>               // for QueuedConnection, ControlModifier, KeyboardModifiers, ShiftModifier
#include <QtGlobal>         // for QFlags, Q_UNUSED, qreal

class QPoint;

#include <browser/abstract-controller.hpp>  // IWYU pragma: associated
#include <browser/content-provider.hpp>     // for ContentProvider, ContentProvider::Ptr
#include <browser/controller.hpp>           // for Controller
#include <browser/history-impl.hpp>         // for HistoryImpl
#include <browser/history.hpp>              // for HistoryItem, History (ptr only)
#include <browser/types.hpp>                // for OPEN_IN_BACKGROUND, OPEN_IN_CURRENT, OPEN_IN_NEW, OpenFlag


Browser::AbstractController::AbstractController(ContentProvider::Ptr contentProvider,
                                                QObject* parent) :
	Browser::Controller(contentProvider, parent)
{
	m_history = new Browser::HistoryImpl(this);
	connect(m_history,
	        &Browser::HistoryImpl::restoreRequest,
	        this,
	        &Browser::AbstractController::loadFromHistory);
	m_applyAutoScroll = false;
	m_recoveryIndex = -1;
	m_recoveryMode = false;
	m_requestUrl.clear();
	m_currentUrl.clear();
	m_hoveredUrl.clear();
}

Browser::AbstractController::~AbstractController()
{
}

QUrl Browser::AbstractController::url() const
{
	return m_currentUrl;
}

void Browser::AbstractController::load(const QUrl& address)
{
	if (!m_history->isEmpty()) {
		m_history->currentItem().scroll = scrollTop();
	}

	m_recoveryMode = false;
	m_requestUrl = address;
	loadImpl(address);
}

void Browser::AbstractController::reload()
{
	if (!m_requestUrl.isEmpty()) {
		Browser::Controller::load(m_requestUrl);
	} else if (!m_history->isEmpty()) {
		m_history->currentItem().scroll = scrollTop();
		loadFromHistory(m_history->currentItemIndex());
	}
}

void Browser::AbstractController::loadFromHistory(int index)
{
	m_recoveryIndex = index;
	m_recoveryMode = true;
	m_autoScroll = m_history->item(m_recoveryIndex).scroll;
	m_applyAutoScroll = true;
	m_requestUrl = m_history->item(index).url;
	loadImpl(m_requestUrl);
}

void Browser::AbstractController::onLoadFinished(bool ok)
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
	emit loadFinished(ok);
}

Browser::History* Browser::AbstractController::history() const
{
	return m_history;
}

bool Browser::AbstractController::canGoBack() const
{
	return m_history->canGoBack();
}

bool Browser::AbstractController::canGoForward() const
{
	return m_history->canGoForward();
}

void Browser::AbstractController::back()
{
	m_history->back();
}

void Browser::AbstractController::forward()
{
	m_history->forward();
}

qreal Browser::AbstractController::zoomFactor() const
{
	return 1.0;
}

void Browser::AbstractController::setZoomFactor(qreal zoom)
{
	Q_UNUSED(zoom);
}

int Browser::AbstractController::scrollTop()
{
	return 0;
}

void Browser::AbstractController::setScrollTop(int pos)
{
	Q_UNUSED(pos);
}

void Browser::AbstractController::setAutoScroll(int pos)
{
	if (pos >= 0) {
		m_autoScroll = pos;
		m_applyAutoScroll = true;
	}
}

void Browser::AbstractController::selectAll()
{
	;
}

QString Browser::AbstractController::selectedText() const
{
	return{};
}

void Browser::AbstractController::selectedCopy()
{
	QString text = selectedText();

	if (!text.isEmpty()) {
		QClipboard* clip = QGuiApplication::clipboard();
		clip->setText(text);
	}
}

void Browser::AbstractController::findText(const QString& text,
                                           bool backward,
                                           bool caseSensitively,
                                           bool highlightSearchResults,
                                           std::function<void (bool found, bool wrapped)> result)
{
	Q_UNUSED(text);

	QMetaObject::invokeMethod(
	    this,
	    "onFindTextFinished",
	    Qt::QueuedConnection,
	    Q_ARG(bool, false));
}

void Browser::AbstractController::print(QPrinter* printer, std::function<void (bool)> result)
{
	Q_UNUSED(printer);

	QMetaObject::invokeMethod(
	    this,
	    "onPrintFinished",
	    Qt::QueuedConnection,
	    Q_ARG(bool, false));
}

void Browser::AbstractController::onUrlChanged(const QUrl& newUrl)
{
	m_currentUrl = newUrl;
	emit urlChanged(m_currentUrl);
}

void Browser::AbstractController::onLinkHovered(const QUrl link)
{
	m_hoveredUrl = link;
}

void Browser::AbstractController::onContextMenuRequested(const QPoint& pos)
{
	emit contextMenuRequested(pos, m_hoveredUrl);
}

void Browser::AbstractController::onLinkClick(QUrl link)
{
	if (link.isEmpty()) {
		return;
	}

	Qt::KeyboardModifiers mods = QGuiApplication::keyboardModifiers();
	Browser::OpenMode mode = Browser::OPEN_IN_CURRENT;

	if (mods & Qt::ShiftModifier) {
		mode = Browser::OPEN_IN_NEW;
	} else if (mods & Qt::ControlModifier) {
		mode = Browser::OPEN_IN_BACKGROUND;
	}

	emit linkClicked(link, mode);
}

void Browser::AbstractController::onLinkClick()
{
	onLinkClick(m_hoveredUrl);
}

void Browser::AbstractController::onFindTextFinished(bool hit, int count, int pos)
{
	emit findTextFinished(hit, count, pos);
}

void Browser::AbstractController::onFindTextFinished(bool hit)
{
	onFindTextFinished(hit, -1, -1);
}

void Browser::AbstractController::onPrintFinished(bool ok)
{
	emit printFinished(ok);
}
