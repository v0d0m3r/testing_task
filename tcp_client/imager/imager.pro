QT += widgets network
requires(qtConfig(filedialog))

CONFIG += c++14

QMAKE_CXXFLAGS += --pedantic

DESTDIR = ../bin

HEADERS += \
    imager_lib/Resize_image_dialog.hpp \
    imager_lib/Resize_image_widget.hpp \
    imager_lib/Transmit_image_dialog.hpp \
    ../../common_lib/Common_facilities.hpp \
    ../../common_lib/Protocol.hpp


SOURCES += \    
    main.cpp \
    imager_lib/Resize_image_dialog.cpp \
    imager_lib/Resize_image_widget.cpp \
    imager_lib/Transmit_image_dialog.cpp \
    ../../common_lib/Common_facilities.cpp \
    ../../common_lib/Protocol.cpp

