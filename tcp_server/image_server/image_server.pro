QT += widgets network

CONFIG += c++14

QMAKE_CXXFLAGS += --pedantic

DESTDIR = ../bin

include("tcp/tcp.pri")

HEADERS += \
    ../../common_lib/Protocol.hpp \
    ../../common_lib/Common_facilities.hpp \
    Tcp_server_dialog.hpp


SOURCES += \
    main.cpp \
    Tcp_server_dialog.cpp

FORMS += \
    Tcp_server_dialog.ui
