/*
 *  uChmViewer - a CHM and EPUB file viewer with broad language support
 *  Copyright (C) 2025 Nick Egorrov, nicegorov@yandex.ru
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

new QWebChannel(qt.webChannelTransport, function(channel) {
	window.reveal = false;
	window.WebEngineBrowser = channel.objects.WebEngineBrowser;
	window.addEventListener("resize", function(event) {
	    if (!reveal) {
			reveal = true;
			window.WebEngineBrowser.onPageReveal();
		}
	});

    document.addEventListener("scroll", function(event) {
        window.WebEngineBrowser.onScrollChanged(window.scrollY);
    });
});

var resizeEventTimeout = 20;

window.addEventListener("resize", function(event) {
    if (typeof window.WebEngineBrowser == 'undefined' && resizeEventTimeout < 300) {
        setTimeout( function() {
            dispatchEvent(new Event("resize"));
        }, resizeEventTimeout);
		resizeEventTimeout += resizeEventTimeout;
	}
});
