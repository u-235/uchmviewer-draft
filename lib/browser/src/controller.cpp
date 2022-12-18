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

#include <QObject>  // for QObject
#include <QUrl>     // for QUrl

#include <browser/content-provider.hpp> // for ContentProvider, ContentProvider::Ptr
#include <browser/controller.hpp>       // IWYU pragma: associated
#include <browser/types.hpp>            // for Feature, Option


Browser::Controller::Controller(ContentProvider::Ptr contentProvider, QObject* parent) :
	QObject(parent),
	m_contentProvider(contentProvider)
{
}

Browser::Controller::~Controller()
{
}

QWidget* Browser::Controller::view()
{
	return nullptr;
}

void Browser::Controller::invalidate()
{
}

bool Browser::Controller::hasOption(Option option) {
	Q_UNUSED(option);
	return false;
}

void Browser::Controller::configure(const Settings& settings)
{
	Q_UNUSED(settings);
}

QUrl Browser::Controller::url() const
{
	return QUrl();
}

void Browser::Controller::load(const QUrl& address)
{
	Q_UNUSED(address);
}

void Browser::Controller::reload()
{
}

QString Browser::Controller::title() const
{
	return QString();
}

bool Browser::Controller::hasFeature(Feature feature) const
{
	Q_UNUSED(feature);
	return false;
}

Browser::History* Browser::Controller::history() const
{
	return nullptr;
}

bool Browser::Controller::canGoBack() const
{
	return false;
}

bool Browser::Controller::canGoForward() const
{
	return false;
}

void Browser::Controller::back()
{
}

void Browser::Controller::forward()
{
}

qreal Browser::Controller::zoomFactor() const
{
	return 0;
}

void Browser::Controller::setZoomFactor(qreal zoom)
{
	Q_UNUSED(zoom);
}

void Browser::Controller::zoomIncrease() {
	setZoomFactor(zoomFactor() + ZOOM_STEP);
}

void Browser::Controller::zoomDecrease() {
	setZoomFactor(zoomFactor() - ZOOM_STEP);
}

int Browser::Controller::scrollTop()
{
	return 0;
}

void Browser::Controller::setScrollTop(int pos)
{
	Q_UNUSED(pos);
}

void Browser::Controller::setAutoScroll(int pos)
{
	Q_UNUSED(pos);
}

void Browser::Controller::selectAll()
{
}

QString Browser::Controller::selectedText() const
{
	return QString();
}

void Browser::Controller::selectedCopy()
{
}

void Browser::Controller::findText(const QString& text,
                                   bool backward,
                                   bool caseSensitively,
                                   bool highlightSearchResults,
                                   std::function<void (bool found, bool wrapped)> result)
{
	Q_UNUSED(text);
	Q_UNUSED(backward);
	Q_UNUSED(caseSensitively);
	Q_UNUSED(highlightSearchResults);
	result(false, false);
}

void Browser::Controller::print(QPrinter* printer, std::function<void (bool)> result)
{
	Q_UNUSED(printer);
	result(false);
}
