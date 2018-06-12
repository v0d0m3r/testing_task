QT += widgets network
requires(qtConfig(filedialog))

CONFIG += c++14

QMAKE_CXXFLAGS += --pedantic

DESTDIR = ../bin

HEADERS += \
    lib/Resize_image_dialog.hpp \
    lib/Resize_image_widget.hpp \
    lib/Transmit_image_dialog.hpp \
    lib/Common_facilities.hpp

SOURCES += \
    lib/Resize_image_dialog.cpp \
    lib/Resize_image_widget.cpp \
    lib/Transmit_image_dialog.cpp \
    lib/Common_facilities.cpp
