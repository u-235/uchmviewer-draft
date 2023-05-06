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

#ifndef ABSTRACT_BROWSER_CONTROLLER_HPP
#define ABSTRACT_BROWSER_CONTROLLER_HPP

#include <QObject>
#include <QUrl>

#include <browser/browser-controller.hpp>
#include <browser/content-provider.hpp>

class QPrinter;
class QString;
class QWidget;

class BrowserSettings;
class BrowserHistory;
class BrowserHistoryImpl;


//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

/**
 * @brief Partial implementation of the BrowserController class.
 *
 * The class fully implements the following methods:
 *
 * * url()
 * * load()
 * * reload()
 * * All methods of Browser::FEATURE_HISTORY feature:
 *   * history()
 *   * canGoBack()
 *   * canGoForward()
 *   * back()
 *   * forward()
 * * setAutoScroll()
 * * selectedCopy()
 *
 * Note that without a complete implementation of the selectedText() method,
 * the selectedCopy() method does not work.
 *
 *
 * <h2>Must be implemented in descendants</h2>
 *
 * * view()
 * * hasOption()
 * * configure()
 * * title()
 * * loadImpl()
 *
 *
 * <h2>Not implemented additional features</h2>
 *
 * * zoomFactor()
 * * setZoomFactor()
 * * scrollTop()
 * * setScrollTop()
 * * selectAll()
 * * selectedText()
 * * findText()
 * * print()
 *
 */
class AbstractBrowserController : public BrowserController
{
		Q_OBJECT

	protected:
		/**
		 * @brief Creates the controller.
		 *
		 * @param[in] contentProvider Content Provider.
		 * @param[in] parent Parent object.
		 */
		explicit AbstractBrowserController(
		    ContentProviderPtr contentProvider,
		    QObject* parent);

		~AbstractBrowserController();

	public:
		QUrl url() const override;
		void load(const QUrl& address) override;
		void reload() override;

		/**
		 * @brief Returns a pointer to the history of navigated web pages.
		 *
		 * @return  Returns a pointer to the BrowserHistory object.
		 */
		BrowserHistory* history() const override;

		/**
		 * @brief Checks the history to see if it can go back.
		 *
		 * @return Returns @c true if history has previous item.
		 */
		bool canGoBack() const override;

		/**
		 * @brief Checks the history to see if it can go forward.
		 *
		 * @return Returns @c true if history has next item.
		 */
		bool canGoForward() const override;

		/**
		 * @brief Tries to go back in history.
		 */
		void back() override;

		/**
		 * @brief Tries to go forward in history.
		 */
		void forward() override;

		void setAutoScroll(int pos) override;
		void selectedCopy() const override;

		// Stubs for additional features.
		qreal zoomFactor() const override;
		void setZoomFactor(qreal zoom) override;
		int scrollTop() override;
		void setScrollTop(int pos) override;
		void selectAll() override;
		QString selectedText() const override;
		void findText(const QString& text, const FindFlags& flags) override;
		void print(QPrinter* printer) override;

	protected:
		BrowserHistoryImpl* m_history;
		bool m_applyAutoScroll;
		int m_autoScroll;
		bool m_recoveryMode;
		int m_recoveryIndex;
		QUrl m_requestUrl;
		QUrl m_currentUrl;
		QUrl m_hoveredUrl;

		/**
		 * @brief Loads the page.
		 *
		 * This method is called from the load() and reload() methods to start
		 * loading the page. The onLoadFinished() slot must be activated when
		 * the page finishes loading.
		 *
		 * @param[in] address The address from which the page is loaded.
		 */
		virtual void loadImpl(const QUrl& address) = 0;

	protected slots:
		/**
		 * @brief Starts page loading by history index.
		 *
		 * This slot connects with BrowserHistoryImpl::restoreRequest() signal.
		 *
		 * @param[in] index Index of the history item to restore the page.
		 */
		void loadFromHistory(int index);

		/**
		 * @brief Receives a signal when the page has finished loading.
		 *
		 * @param[in] ok @c true if the loading is successful.
		 */
		void onLoadFinished(bool ok);

		/**
		 * @brief Receives a signal when the url of the page changes.
		 *
		 * @param newUrl The new url of the page.
		 */
		void onUrlChanged(const QUrl& newUrl);

		/**
		 * @brief Receives a signal when a mouse is moved over a link.
		 *
		 * The slot should activate in two cases: when the mouse enters the link
		 * and when it leaves the link.
		 *
		 * @param link Url of the link. If mouse leaves the link, url must be
		 *     empty.
		 */
		void onLinkHovered(const QUrl link);

		/**
		 * @brief Receives a signal when the context menu should be shown.
		 *
		 * @param[in] pos  Global position of requested menu.
		 */
		void onContextMenuRequested(const QPoint& pos);

		/**
		 * @brief Receives a signal when the link is activated.
		 *
		 * @param[in] link Url of the link.
		 */
		void onLinkClick(QUrl link);

		/**
		 * @brief Receives a signal when the link is activated.
		 */
		void onLinkClick();

		/**
		 * @brief Receives a signal when the text search is finished.
		 *
		 * This slot is used if the browser supports counting the occurrences
		 * of the phrase searched for.
		 *
		 * @param[in] hit @c true if the phrase has been found.
		 * @param[in] count The number of occurrences of the search phrase.
		 * @param[in] pos The index of the current highlighted occurrence of
		 *     the phrase.
		 */
		void onFindTextFinished(bool hit, int count, int pos);

		/**
		 * @brief Receives a signal when the text search is finished.
		 *
		 * This slot is used if the browser does not support counting
		 * occurrences of the phrase searched for.
		 *
		 * @param[in] hit @c true if the phrase has been found.
		 */
		void onFindTextFinished(bool hit);

		/**
		 * @brief Receives a signal when the page has finished printing.
		 *
		 * @param[in] ok @c true if the page was printed successfully.
		 */
		void onPrintFinished(bool ok);
};

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

#endif // ABSTRACT_BROWSER_CONTROLLER_HPP
