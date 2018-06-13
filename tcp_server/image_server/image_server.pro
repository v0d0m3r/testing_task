QT += widgets network

CONFIG += c++14

QMAKE_CXXFLAGS += --pedantic

DESTDIR = ../bin

include("tcp/tcp.pri")
