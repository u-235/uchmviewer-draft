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

#ifndef UBROWSER_BROWSER_HPP
#define UBROWSER_BROWSER_HPP

#include <functional>

#include <QObject>
#include <QString>
#include <QtGlobal>

class QPoint;
class QPrinter;
class QUrl;
class QWidget;

#include <ubrowser/contentprovider.hpp>
#include <ubrowser/types.hpp>

#include "export.hpp"


namespace UBrowser
{

//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

class History;


/**
 * @brief The UBrowser::Browser class provides an interface to display
 * documents.
 *
 * The application uses the UBrowser::Browser class to display eBook pages.
 * Hereinafter the word "browser" is used to mean "the UBrowser::Browser class
 * or its descendant". Depending on the implementation, there may be stubs
 * instead of some methods. For example, some implementation may not have page
 * printing or text selection and copying. The hasFeature() method is used to
 * inform the application of supported features. The browser may not have all
 * settings options via the configure() method, and the hasOption() method
 * should inform the application of supported options.
 *
 * The application passes a pointer to a UBrowser::ContentProvider instance to
 * the browser constructor. The UBrowser::ContentProvider class is the only way
 * to get content to display the page. The browser should not attempt to load
 * content from the network or any other way other than from the
 * UBrowser::ContentProvider. The application starts loading the page by calling
 * the load() or reload() method. The loading can also be triggered by the
 * management methods of the navigation history, discussed
 * @ref browser-feature-history "below". When the load is complete, the browser
 * emits the loadFinished() signal.
 *
 * The linkClicked() signal is emitted when a link is activated by any method
 * (mouse, Enter key, menu item, etc.) except for the link activation to the
 * anchor inside the current page. The browser must not load a page or create
 * a new one itself when activating a link - the application does this in
 * response to the linkClicked() signal. The browser must not create and show
 * a context menu, instead it must emit the contextMenuRequested() signal.
 *
 * When the URL changes, the urlChanged() signal is emitted. The URL can change
 * after the page loads or if the user clicks on a link to an anchor inside the
 * current page. The URL does not change when the page is reloaded.
 *
 * For integration into the GUI, the view() method returns a widget with the
 * page displayed. In the destructor, the browser should call deleteLater() of
 * the widget.
 *
 *
 * <h2>Additional features</h2>
 *
 * The methods and signals that must be fully implemented were listed above. The
 * following are additional features without which the application can work.
 * Note that for a supported feature, the browser must have a complete
 * implementation of all methods of that feature.
 *
 *
 * @anchor browser-feature-history
 * <h3>Navigation and history</h3>
 *
 * A browser that supports the history must meet the following conditions:
 *
 * * The hasFeature() method called with the UBrowser::FEATURE_HISTORY must
 *   return @c true.
 * * The canGoBack(), canGoForward(), back() and forward() methods must be fully
 *   implemented and the history() method must return not @c nullptr.
 * * When history changes, the historyChanged() signal is emitted.
 *
 *
 * @anchor browser-feature-copytext
 * <h3>Text selection and copying</h3>
 *
 * A browser that supports text copying must meet the following conditions:
 *
 * * The hasFeature() method called with the UBrowser::FEATURE_COPY must
 *   return @c true.
 * * The selectAll(), selectedCopy() and selectedText() methods must be fully
 *   implemented.
 *
 *
 * @anchor browser-feature-findtext
 * <h3>Searching in the text</h3>
 *
 * A browser that supports text searching must meet the following conditions:
 *
 * * The hasFeature() method called with the UBrowser::FEATURE_SEARCH must
 *   return @c true.
 * * The findText() method must be fully implemented.
 * * The  findTextFinished() signal must be emitted when the text search is
 *   finished.
 *
 *
 * @anchor browser-feature-print
 * <h3>Page printing</h3>
 *
 * A browser that supports page printing must meet the following conditions:
 *
 * * The hasFeature() method called with the UBrowser::FEATURE_PRINT must
 *   return @c true.
 * * The print() method must be fully implemented.
 * * The printFinished() signal should be emitted when printing of the page is
 *   complete.
 *
 *
 * @anchor browser-feature-zoom
 * <h3>Page zooming</h3>
 *
 * A browser that supports page zooming must meet the following conditions:
 *
 * * The hasFeature() method called with the UBrowser::FEATURE_ZOOM must
 *   return @c true.
 * * The zoomFactor() and setZoomFactorImpl() methods must be fully implemented.
 *
 *
 * @anchor browser-feature-scroll
 * <h3>Page scrolling</h3>
 *
 * A browser that supports page scrolling must meet the following conditions:
 *
 * * The hasFeature() method called with the UBrowser::FEATURE_SCROLL must
 *   return @c true.
 * * The scrollTop() and setScrollTop() methods must be fully implemented.
 *
 */
class UBROWSER_API_DECL Browser : public QObject
{
		Q_OBJECT

	public:
		/**
		 * @brief Creates the browser.
		 *
		 * @param[in] contentProvider Content Provider.
		 * @param[in] parent Parent object.
		 */
		explicit Browser( ContentProvider::Ptr contentProvider, QObject* parent );

		ContentProvider::Ptr contentProvider() const;
		void setContentProvider( ContentProvider::Ptr contentProvider );

		/**
		 * @brief Destroys the browser.
		 */
		~Browser() override;

		/**
		 * @brief Returns a widget that shows the page.
		 */
		virtual QWidget* view() = 0;

		/**
		 * @brief Checks the support for the setting option.
		 *
		 * @param[in] option The option whose support is being checked.
		 * @return Returns @c true if the option is supported.
		 */
		virtual bool hasOption( UBrowser::Option option ) = 0;

		/**
		 * @brief Returns the URL of the displayed page.
		 */
		virtual QUrl url() const = 0;

		/**
		 * @brief Loads the page.
		 *
		 * The page is loaded from the specified address. When the page finishes
		 * loading, the loadFinished() signal is emitted.
		 *
		 * @param[in] ebookUrl The address from which the page is loaded.
		 */
		virtual void load( const QUrl& ebookUrl ) = 0;

		/**
		 * @brief Reloads the page.
		 *
		 * The loadFinished() signal is emitted.
		 */
		virtual void reload() = 0;

		/**
		 * @brief Returns the title of the displayed page.
		 */
		virtual QString title() const = 0;

		/**
		 * @brief Checks the support for a feature.
		 *
		 * @param[in] feature The feature whose support is being checked.
		 * @return Returns @c true if the feature is supported.
		 */
		virtual bool hasFeature( UBrowser::Feature feature ) const = 0;

		/**
		 * @brief Returns a pointer to the history of navigated web pages.
		 *
		 * If browser has not UBrowser::FEATURE_HISTORY feature, the method
		 * return @c nullptr.
		 *
		 * @return Returns a pointer to the UBrowser::History object or
		 *     @c nullptr.
		 */
		virtual UBrowser::History* history() const = 0;

		/**
		 * @brief Checks the history to see if it can go back.
		 *
		 * If browser has not UBrowser::FEATURE_HISTORY feature, the method
		 * return @c false.
		 *
		 * @return Returns @c true if history has previous item.
		 * @see back()
		 */
		virtual bool canGoBack() const = 0;

		/**
		 * @brief Checks the history to see if it can go forward.
		 *
		 * If browser has not UBrowser::FEATURE_HISTORY feature, the method
		 * return @c false.
		 *
		 * @return Returns @c true if history has next item.
		 * @see forward()
		 */
		virtual bool canGoForward() const = 0;

	public slots:
		/**
		 * @brief Tries to go back in history.
		 *
		 * If the history contains a previous item and the canGoBack() function
		 * returns @c true, it goes back through the history. The signals
		 * loadFinished() and historyChanged() are also emitted.
		 *
		 * If browser has not UBrowser::FEATURE_HISTORY feature, the method
		 * does nothing.
		 *
		 * @see forward()
		 */
		virtual void back() = 0;

		/**
		 * @brief Tries to go forward in history.
		 *
		 * If the history contains a next item and the canGoBack() function
		 * returns @c true, it goes forward through the history. The signals
		 * loadFinished() and historyChanged() are also emitted.
		 *
		 * If browser has not UBrowser::FEATURE_HISTORY feature, the method
		 * does nothing.
		 *
		 * @see back()
		 */
		virtual void forward() = 0;

	public:
		/**
		 * @brief Returns the page scaling.
		 *
		 * If browser has not UBrowser::FEATURE_ZOOM feature, the method
		 * must returns 1.0.
		 *
		 * @see setZoomFactor()
		 * @see zoomDecrease()
		 * @see zoomIncrease()
		 */
		virtual qreal zoomFactor() const = 0;

		/**
		 * @brief Sets the page scaling.
		 *
		 * The method must limit the zoom factor to a range from #ZOOM_MIN to
		 * #ZOOM_MAX inclusive.
		 *
		 * If browser has not UBrowser::FEATURE_ZOOM feature, the method
		 * does nothing.
		 *
		 * @param[in] zoom The page zoom factor.
		 * @see zoomFactor()
		 */
		void setZoomFactor( qreal zoom );

	public slots:
		/**
		 * @brief Zooms in on the page.
		 *
		 * The method zooms in by the #ZOOM_STEP value as long as the zoom
		 * is smaller than #ZOOM_MAX.
		 *
		 * If browser has not UBrowser::FEATURE_ZOOM feature, the method
		 * does nothing.
		 * @see zoomDecrease()
		 * @see zoomFactor()
		 */
		void zoomIncrease();

		/**
		 * @brief Zooms out the page.
		 *
		 * The method zooms out by the #ZOOM_STEP value as long as the zoom
		 * is larger than #ZOOM_MIN.
		 *
		 * If browser has not UBrowser::FEATURE_ZOOM feature, the method
		 * does nothing.
		 * @see zoomIncrease()
		 * @see zoomFactor()
		 */
		void zoomDecrease();

	public:
		/**
		 * @brief Returns the position of the scroll bar in the view window.
		 *
		 * If browser has not UBrowser::FEATURE_SCROLL feature, the method
		 * must returns @c 0.
		 *
		 * @return Returns the scroll position.
		 * @see setScrollTop()
		 */
		virtual int scrollTop() = 0;

		/**
		 * @brief Sets the position of the scroll bar in the view window.
		 *
		 * If browser has not UBrowser::FEATURE_SCROLL feature, the method
		 * does nothing.
		 *
		 * @param[in] pos The position of the scroll bar.
		 * @see scrollTop()
		 */
		virtual void setScrollTop( int pos ) = 0;

		/**
		 * @brief Sets the scroll position after page loading.
		 *
		 * For automatic page scrolling after loading, this method must be
		 * called before load() method. Automatic scrolling is triggered once.
		 *
		 * If browser has not UBrowser::FEATURE_SCROLL feature, the method
		 * does nothing.
		 *
		 * @param[in] pos The position of the scroll bar.
		 */
		void setAutoScroll( int pos );

		/**
		 * @brief Selects all text on the page.
		 *
		 * If browser has not UBrowser::FEATURE_COPY feature, the method
		 * does nothing.
		 *
		 * @see selectedText()
		 * @see selectedCopy()
		 */
		virtual void selectAll() = 0;

		/**
		 * @brief Returns the selected text.
		 *
		 * If browser has not UBrowser::FEATURE_COPY feature, the method
		 * must returns an empty string.
		 *
		 * @see selectAll()
		 */
		virtual QString selectedText() const = 0;

		/**
		 * @brief Copies the selected text to the clipboard.
		 *
		 * If browser has not UBrowser::FEATURE_COPY feature, the method
		 * does nothing.
		 *
		 * @see selectAll()
		 */
		virtual void selectedCopy() = 0;

		/**
		 * @brief The FindFlags structure contains flags for the findText()
		 * method.
		 */
		struct FindFlags
		{
			/// Search from the end of the page to the head of the page.
			bool backward;
			/// When searching, take the case into account.
			bool caseSensitively;
			/// Highlight hits with the search pattern.
			bool highlight;
		};

		/**
		 * @brief Finds the text on the page.
		 *
		 * The findTextFinished() signal is emitted when searching is finished.
		 *
		 * If browser has not UBrowser::FEATURE_SEARCH feature, the method
		 * does nothing.
		 *
		 * @param[in] text The text to be found.
		 * @param[in] flags Options for searching and displaying the search
		 *     result.
		 */
		virtual void findText( const QString& text,
		                       bool backward,
		                       bool caseSensitively,
		                       bool highlightSearchResults,
		                       std::function<void ( bool found, bool wrapped )> result ) = 0;

		/**
		 * @brief Prints the page.
		 *
		 * The printFinished() signal is emitted when printing is finished.
		 *
		 * If browser has not UBrowser::FEATURE_PRINT feature, the method
		 * does nothing.
		 *
		 * @param[in] printer Print device.
		 */
		virtual void print( QPrinter* printer, std::function<void ( bool success )> result ) = 0;

	protected:
		int autoScroll();
		void scrollAfterLoad();
		virtual void setZoomFactorImpl(qreal zoom) = 0;

	private:
		ContentProvider::Ptr m_contentProvider;
		/// @brief Minimum scaling factor.
		/// @see UsetZoomFactor()
		qreal m_zoomFactorMin;
		/// @brief Maximum scaling factor.
		/// @see setZoomFactor()
		qreal m_zoomFactorMax;
		/// @brief Zoom factor increase/decrease step.
		/// @see zoomIncrease() @see zoomDecrease()
		qreal m_zoomFactorStep;
		int m_autoScroll;

		signals:
		/**
		 * @brief The signal is emitted before the page loads.
		 *
		 * The load(), reload(), back() and forward() methods can trigger this
		 * signal.
		 */
		void loadStarted();

		/**
		 * @brief The signal is emitted at the end of loading.
		 *
		 * The load(), reload(), back() and forward() methods can trigger this
		 * signal.
		 *
		 * @param[in] ok Indicates if load was successful.
		 */
		void loadFinished( bool ok );

		/**
		 * @brief The signal is emitted when the url of the page is changed.
		 *
		 * The load(), back() and forward() methods can trigger this signal.
		 * Also it can be triggered by clicking a link to an anchor inside
		 * the page.
		 *
		 * @param[in] newUrl The new address of the displayed page.
		 */
		void urlChanged( const QUrl& newUrl );

		/**
		 * @brief The signal is triggered by pressing the right mouse button.
		 *
		 * @param[in] pos Global position of requested menu.
		 *     See QMenu::exec(QPoint&, QAction*) and QWidget::mapToGlobal().
		 * @param[in] link Url of the link that was right-clicked. If the user
		 *     right-clicked not on the link, this parameter should be empty.
		 * @see QWidget::customContextMenuRequested()
		 */
		void contextMenuRequested( const QPoint& pos, const QUrl& link );

		/**
		 * @brief The signal is emitted when the link is activated.
		 *
		 * @param[in] link Url link that has been activated.
		 * @param[in] mode How to open the link: in the current tab or create
		 *     a new one.
		 */
		void linkClicked( const QUrl& link, UBrowser::OpenMode mode );

		/**
		 * @brief The signal is emitted when the history of the page is changed.
		 *
		 * The load(), back() and forward() methods can trigger this signal.
		 * Also it can be triggered by clicking a link to an anchor inside
		 * the page. In fact, this signal duplicates the urlChanged() signal
		 * without the parameter.
		 *
		 * The signal works if the browser supports the
		 * UBrowser::FEATURE_HISTORY feature.
		 */
		void historyChanged();

		/**
		 * @brief The signal is emitted when the search is finished.
		 *
		 * The signal works if the browser supports the
		 * UBrowser::FEATURE_SEARCH feature.
		 *
		 * @param[in] hit Indicates whether text has been found.
		 * @param[in] count The number of matches found.  If the browser does
		 *     not support match counting, it must pass a negative value.
		 * @param[in] pos Index of the current highlighted match.
		 */
		void findTextFinished( bool hit, int count, int pos );

		/**
		 * @brief The signal is emitted when the print is finished.
		 *
		 * The signal works if the browser supports the
		 * UBrowser::FEATURE_PRINT feature.
		 *
		 * @param[in] ok Indicates if print was successful.
		 */
		void printFinished( bool ok );
};

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

} // namespace UBrowser

#endif // UBROWSER_BROWSER_HPP
