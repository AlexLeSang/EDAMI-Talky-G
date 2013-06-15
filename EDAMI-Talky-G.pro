#-------------------------------------------------
#
# Project created by QtCreator 2013-05-06T14:54:02
#
#-------------------------------------------------

QT       -= core
QT       -= gui

TARGET = EDAMI-Talky-G
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    Node.cpp \
    Talky-G.cpp

HEADERS += \
    Typedefs.hpp \
    Node.hpp \
    CSet.hpp \
    ResultSaver.hpp \
    DatabaseReader.hpp \
    Item.hpp \
    Itemset.hpp \
    Tidset.hpp \
    Database.hpp \
    Talky-G.hpp

#DEFINES += "TREE_TEST"
#DEFINES += "SIMPLE_TEST"
QMAKE_CXX = g++-4.7
