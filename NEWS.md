## 2021-10-07 Release 8.1

This release is dedicated to the translation of the UI. Initially, translation worked only in an application which was built with KDE support.

* Added KDE-independent UI translation support using GNU Gettext. QT and KDE dialogs and messages are also translated if appropriate files are available.
* Updated translation into Russian.
* The build script has been updated to better support application installation on Windows and macOS.
* Fixed a bug in showing pages of some EPUB files. EPUB files contain XML, but the app treated them as HTML. And if XML contains self-closing `title` tag, then browser was considered it as invalid HTML and did not show content.
* Fixed a bug in a file filter for the Open File dialog box in KDE. Due to an error in the filter description, the Open File dialog shows only CHM files. Now, the dialog has three display options:
  - All supported books
  - CHM only
  - EPUB only
* Fixed a bug of parsing command line arguments with non-Latin characters, occurring in KDE.


## 2021-09-20 Release 8.0

This release has two goals: the option to use QtWebEngine and the migration to cmake as the primary build system.

* Full support for QtWebEngine since Qt 5.9, including the ability to change page encoding.
* When building an application, it is possible to choose HTML engine: QtWebKit or QtWebEngine.
* Updated Cmake scripts to build applications without depending on KDE. This allows Cmake to build applications for Windows, macOS and Linux using only Qt.
* Added initial support for Cpack.