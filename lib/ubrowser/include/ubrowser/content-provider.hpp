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

#ifndef UBROWSER_CONTENTPROVIDER_HPP
#define UBROWSER_CONTENTPROVIDER_HPP

#include <memory>

#include <QByteArray>
#include <QString>
#include <QUrl>


namespace UBrowser
{

//------------------------------------------------------------------------------
// Start BrowserAPI group.
/// @addtogroup BrowserAPI
/// @{

/**
 * @brief The Content structure holds a data retrieved from an eBook.
 *
 * See ContentProvider::getContent() method.
 */
struct ContentData
{
	QByteArray  buffer;     ///< Raw data
	QByteArray  mime;       ///< Mime type of data
	QByteArray  asUtf8;     ///< Raw data as text in utf-8
};


/**
 * @brief The UBrowser::ContentProvider interface provides access to the contents
 * of an eBook.
 *
 * @details The UBrowser::ContentProvider class performs access to an content by
 * URL. The isSupportedUrl() method allows you to check the validity of an URL.
 * If the URL is supported, the content can be retrieved using the getContent()
 * method.
 *
 * @see ContentData
 * @see ContentProvider::Ptr
 */
class ContentProvider
{
	public:
		/**
		* @brief The UBrowser::ContentProvider::Ptr is a shared pointer to
		* UBrowser::ContentProvider.
		*/
		typedef std::shared_ptr<ContentProvider> Ptr;

		static const char* URL_SCHEME;

		virtual ~ContentProvider();

		/**
		 * @brief Returns the URL scheme used by an eBook implementation.
		 *
		 * @return A non-empty string with a URL scheme.
		 * @see EBook::urlScheme()
		 */
		virtual QString urlScheme() const = 0;

		QUrl convertUrlForEbook( const QUrl& browserUrl ) const;

		QUrl convertUrlForBrowser( const QUrl& ebookUrl ) const;

		/**
		 * @brief Checks if this kind of URL is supported by the given eBook
		 * type.
		 *
		 * @details The scheme of the tested URL is compared with the scheme
		 * inherent in the eBook.
		 *
		 * @param[in] browserUrl The URL to check.
		 * @return Returns @c true if this kind of URL is supported.
		 * @see EBook::isSupportedUrl()
		 */
		bool isValidUrl( const QUrl& browserUrl ) const;

		/**
		 * @brief Fills the UBrowser::ContentData structure with eBook content
		 * by URL.
		 *
		 * @details The method tries to retrieve the requested content from
		 * the eBook. If the content exists, the UBrowser::ContentData::mime and
		 * UBrowser::ContentData::encoding fields are also filled. Note that the
		 * UBrowser::ContentData::encoding field makes sense for the MIME type
		 * "text/".
		 * See MimeHelper.
		 *
		 * @param[out] data A structure where the retreived content should be
		 *     stored.
		 * @param[in]  browserUrl  An URL in eBook to retreive content from.
		 * @return Returns @c true if operation is succes.
		 */
		bool content( ContentData& data, const QUrl& browserUrl );

		virtual QString topicTitle( const QUrl& browserUrl ) = 0;

	protected:
		/**
		 * @brief Checks if this kind of URL is supported by the given eBook
		 * type.
		 *
		 * @details The scheme of the tested URL is compared with the scheme
		 * inherent in the eBook.
		 *
		 * @param[in] url The URL to check.
		 * @return Returns @c true if this kind of URL is supported.
		 * @see EBook::isSupportedUrl()
		 */
		virtual bool ebookIsValidUrl( const QUrl& ebookUrl ) const = 0;

		/**
		 * @brief Fills the UBrowser::ContentData structure with eBook content
		 * by URL.
		 *
		 * @details The method tries to retrieve the requested content from
		 * the eBook. If the content exists, the UBrowser::ContentData::mime and
		 * UBrowser::ContentData::encoding fields are also filled. Note that the
		 * UBrowser::ContentData::encoding field makes sense for the MIME type
		 * "text/".
		 * See MimeHelper.
		 *
		 * @param[out] data A structure where the retreived content should be
		 *     stored.
		 * @param[in]  url  An URL in eBook to retreive content from.
		 * @return Returns @c true if operation is succes.
		 */
		virtual bool ebookContent( ContentData& data, const QUrl& ebookUrl ) = 0;
};

/// @}
// End BrowserAPI group.
//------------------------------------------------------------------------------

} // namespace UBrowser

#endif // UBROWSER_CONTENTPROVIDER_HPP
