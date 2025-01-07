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

#ifndef CONTENTPROVIDERIMPL_H
#define CONTENTPROVIDERIMPL_H

class QString;
class QTextCodec;
class QUrl;

#include <ebook.h>
#include <ubrowser/content-provider.hpp>


class ContentProviderImpl : public UBrowser::ContentProvider
{
	public:
		ContentProviderImpl( EBook::Ptr ebook );
		~ContentProviderImpl();

		EBook::Ptr ebook();

		QString urlScheme() const override;

		QString topicTitle( const QUrl& browserUrl ) override;

		void setTextCodec( QTextCodec* codec );

	protected:
		bool ebookIsValidUrl( const QUrl& ebookUrl ) const override;

		bool ebookContent( UBrowser::ContentData& data, const QUrl& ebookUrl ) override;

		EBook::Ptr m_ebook;
		QTextCodec* m_textCodec;
};

#endif // CONTENTPROVIDERIMPL_H
