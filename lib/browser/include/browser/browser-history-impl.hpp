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

#ifndef BROWSER_HISTORY_IMPL_HPP
#define BROWSER_HISTORY_IMPL_HPP

#include <QObject>
#include <QList>

#include <browser/browser-history.hpp>


/**
 * @brief The BrowserHistoryImpl class implements the BrowserHistory interface.
 *
 * To control how pages are loaded from the history URL, the BrowserHistoryImpl
 * class has a restoreRequest() signal and a restoreResponse() slot. If the
 * history contains the desired item, calling the back(), forward() or
 * gotoIndex() methods sends the restoreRequest() signal with the history item
 * index. In response, the BrowserPage class must try to load the page at the
 * URL from the specified history element and, if successful, call
 * restoreResponse() with the same index.
 */
class BrowserHistoryImpl : public BrowserHistory
{
		Q_OBJECT

	public:
		BrowserHistoryImpl(QObject* parent);

		~BrowserHistoryImpl();

		void add(BrowserHistoryItem item) override;

		bool canGoBack() const override;

		bool canGoForward() const override;

		/**
		 * @copydoc BrowserHistory::back()
		 * @note If the history contains the desired item, this method triggers
		 * the restoreRequest() signal.
		 */
		void back() override;

		/**
		 * @copydoc BrowserHistory::forward()
		 * @note If the history contains the desired item, this method triggers
		 * the restoreRequest() signal.
		 */
		void forward() override;

		/**
		 * @copydoc BrowserHistory::gotoIndex()
		 * @note If the history contains the desired item, this method triggers
		 * the restoreRequest() signal.
		 */
		void gotoIndex(int index) override;

		virtual bool isEmpty() override;

		int size() override;

		/**
		 * @copydoc BrowserHistory::item()
		 *
		 * If @p index falls outside the history item list, the method returns
		 * a fake BrowserHistoryItem object. See #m_fakeItem
		 */
		BrowserHistoryItem& item(int index) override;

		int currentItemIndex() override;

		/**
		 * @copydoc BrowserHistory::currentItem()
		 *
		 * If the history is empty, the method returns a fake BrowserHistoryItem
		 * object. See #m_fakeItem
		 */
		BrowserHistoryItem& currentItem() override;

		void clear() override;

		const QList<BrowserHistoryItem>& items() const override;

		void setItems(const QList<BrowserHistoryItem>& items, int index = 0) override;

	public slots:
		/**
		 * @brief Response on request to load from history.
		 *
		 * @param index The new index of the current history item.
		 * @see restoreRequest()
		 */
		void restoreResponse(int index);

	signals:
		/**
		 * @brief Requests to load the page by URL from the history.
		 *
		 * This signal is emitted when back(), forward() and gotoIndex() methods
		 * are activated.
		 *
		 * @param index Index of the history item to restore the page.
		 * @see restoreResponse()
		 */
		void restoreRequest(int index);

	protected:
		int m_currentItemIndex;
		QList<BrowserHistoryItem> m_items;
		/// History item returned for an invalid index.
		BrowserHistoryItem m_fakeItem;
};

#endif // BROWSER_HISTORY_IMPL_HPP
