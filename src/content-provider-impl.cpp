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

#include <memory>                        // for shared_ptr

#include <QByteArray>   // for QByteArray
#include <QString>      // for QString

#include <content-provider-impl.h>  // for ContentData
#include <ebook.h>                  // for EBook::Ptr, EBook

#include "browser/content-provider.hpp"
#include "mimehelper.h"                 // for MimeHelper


ContentProviderImpl::ContentProviderImpl(EBook::Ptr ebook)
{
	m_ebook = ebook;
}

ContentProviderImpl::~ContentProviderImpl()
{
}

EBook::Ptr ContentProviderImpl::ebook()
{
	return m_ebook;
}

QString ContentProviderImpl::urlScheme()
{
	return m_ebook->urlScheme();
}

bool ContentProviderImpl::isSupportedUrl(const QUrl& url)
{
	return m_ebook->isSupportedUrl(url);
}

bool ContentProviderImpl::getContent(Browser::ContentData& data, const QUrl& url) const
{
	if (!m_ebook->getFileContentAsBinary(data.buffer, url)) {
		return false;
	}

	data.encoding = m_ebook->currentEncoding();
	data.mime = MimeHelper::mimeType(url, data.buffer);
	return true;
}
