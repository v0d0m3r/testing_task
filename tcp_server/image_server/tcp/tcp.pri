QT += network
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/Tcp_server.hpp \
    $$PWD/Tcp_connection.hpp \
    $$PWD/Tcp_connections.hpp

SOURCES += \
    $$PWD/Tcp_server.cpp \
    $$PWD/Tcp_connection.cpp \
    $$PWD/Tcp_connections.cpp

