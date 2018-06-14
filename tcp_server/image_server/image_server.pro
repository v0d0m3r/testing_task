QT += widgets network

CONFIG += c++14

QMAKE_CXXFLAGS += --pedantic

DESTDIR = ../bin

include("tcp/tcp.pri")

HEADERS += \
    ../../common_lib/Protocol.hpp \
    ../../common_lib/Common_facilities.hpp \
    dialog.h


SOURCES += \
    ../../common_lib/Protocol.cpp \
    ../../common_lib/Common_facilities.cpp \
    main.cpp \
    dialog.cpp

FORMS += \
    dialog.ui
