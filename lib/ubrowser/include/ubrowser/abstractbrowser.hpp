/*
 *  uChmViewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2022-2025 Nick Egorrov, nicegorov@yandex.ru
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

#ifndef UBROWSER_ABSTRACTBROWSER_HPP
#define UBROWSER_ABSTRACTBROWSER_HPP

#include <QObject>
#include <QString>
#include <QUrl>

class QPoint;

#include <ubrowser/browser.hpp>
#include <ubrowser/contentprovider.hpp>
#include <ubrowser/history.hpp>
#include <ubrowser/types.hpp>

#include "export.hpp"


namespace UBrowser
{

//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

class HistoryImpl;

/**
 * @brief Partial implementation of the UBrowser::Browser interface.
 *
 * The class fully implements the following methods:
 *
 * * url()
 * * load()
 * * reload()
 * * All methods of UBrowser::FEATURE_HISTORY feature:
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
 * * setZoomFactorImpl()
 * * setScrollTop()
 * * selectAll()
 * * selectedText()
 * * findText()
 * * print()
 *
 */
class UBROWSER_API_DECL AbstractBrowser : public UBrowser::Browser
{
		Q_OBJECT

	protected:
		explicit AbstractBrowser(
		    ContentProvider::Ptr contentProvider,
		    QObject* parent );

		~AbstractBrowser() override;

	public:
		QUrl url() const final;
		void load( const QUrl& ebookUrl ) final;
		void reload() final;
		UBrowser::History* history() const final;
		bool canGoBack() const final;
		bool canGoForward() const final;
		void back() final;
		void forward() final;
		void selectedCopy() final;
		int scrollTop() final;

	protected:
		void setUrl( const QUrl& url );

		void loadConverted( const QUrl& ebookUrl );

		virtual void loadImpl( const QUrl& address ) = 0;

		virtual void injectJS();

		virtual void rePageReveal();
		virtual void reUrlChanged();


	public slots:
		void onLoadStarted();

		/**
		 * @brief Receives a signal when the page has finished loading.
		 *
		 * @param[in] successfully @c true if the loading is successful.
		 */
		void onLoadFinished(bool successfully );

		void onPageReveal();

		/**
		 * @brief Receives a signal when the url of the page changes.
		 *
		 * @param url The new url of the page.
		 */
		void onUrlChanged(const QUrl& url );

		void onHistoryChanged();

		void onScrollChanged(int scroll);

		/**
		 * @brief Receives a signal when the link is activated.
		 *
		 * @param[in] link Url of the link.
		 */
		void onLinkClicked( const QUrl& link, UBrowser::OpenMode mode );

		/**
		 * @brief Receives a signal when the context menu should be shown.
		 *
		 * @param[in] pos  Global position of requested menu.
		 */
		void onContextMenuRequested( const QPoint& globalPos, const QUrl& link );

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
		void onFindTextFinished( bool hit, int count, int pos );

		/**
		 * @brief Receives a signal when the text search is finished.
		 *
		 * This slot is used if the browser does not support counting
		 * occurrences of the phrase searched for.
		 *
		 * @param[in] hit @c true if the phrase has been found.
		 */
		void onFindTextFinished( bool hit );

		/**
		 * @brief Receives a signal when the page has finished printing.
		 *
		 * @param[in] ok @c true if the page was printed successfully.
		 */
		void onPrintFinished( bool ok );

	protected:
		bool m_fireRevealOnLoad;
		bool m_fireRevealIfShow;
		int m_fireRevealDelay;

	private:
		bool m_pageLoads;
		bool m_pageReveal;
		bool m_untimelyPageReveal;
		bool m_untimelyUrlChanged;
		UBrowser::HistoryImpl* m_history;
		QUrl m_requestUrl;
		QUrl m_currentUrl;
		QUrl m_changedUrl;
		bool m_fragmentScrollInvalid;
		int m_fragmentScroll;
		int m_scrollTop;
};

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

} // namespace UBrowser

#endif // UBROWSER_ABSTRACTBROWSER_HPP
