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

#include <QObject>
#include <QtGlobal>     // qreal

#include <browser/browser-controller.hpp>


BrowserController::BrowserController(
    ContentProviderPtr contentProvider,
    QObject* parent)
	:
	QObject(parent),
	m_contentProvider(contentProvider) {}

BrowserController::~BrowserController() {}

void BrowserController::zoomIncrease() {
	setZoomFactor(zoomFactor() + ZOOM_STEP);
}

void BrowserController::zoomDecrease() {
	setZoomFactor(zoomFactor() - ZOOM_STEP);
}
