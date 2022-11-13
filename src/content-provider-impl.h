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

#include <ebook.h>                      // for EBook::Ptr
#include <browser/content-provider.hpp> // for ContentProvider

class QString;
class QUrl;


class ContentProviderImpl : public Browser::ContentProvider
{
	public:
		ContentProviderImpl(EBook::Ptr ebook);
		~ContentProviderImpl();

		EBook::Ptr ebook();

		QString urlScheme() override;

		bool isSupportedUrl(const QUrl& url) override;

		bool getContent(Browser::ContentData& data, const QUrl& url) const override;

	protected:
		EBook::Ptr m_ebook;
};

#endif // CONTENTPROVIDERIMPL_H
