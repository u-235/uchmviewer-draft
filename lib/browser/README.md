Browser API library
===================

The BrowserAPI library describes the interface for the interaction between the
uChmViewer application and the plugin that displays the pages of eBooks. In
addition, the library contains several classes that make it easy to create
plugins.


Documentation
-------------

Use [Doxygen] to generate documentation.


Dependies
---------

- The C++11 compiler.
- [Qt] Core and Widgets modules with version 5.0 or later.
- [Cmake] version 3.11 or later.


Build
-----

    mkdir build
    cd build
    cmake ..
    cmake --build .

The build script uses the `APP_QT_VERSION` cache variable to specify the minimum
version of Qt. By default this variable is set to 5.0.


[Doxygen]: https://www.doxygen.nl
[Qt]: https://doc.qt.io
[Cmake]: https://cmake.org