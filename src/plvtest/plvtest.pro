TEMPLATE = lib
TARGET = plv_test_plugin
CONFIG += plugin
QT += core
QT -= gui
DESTDIR = ../../libs/plugins
INCLUDEPATH += ../../include/
LIBS += -lplvcore
QMAKE_LIBDIR += ../../libs/plugins

include(../../common.pri)

DEFINES += PLV_TEST_PLUGIN_LIBRARY

SOURCES += plvtest_plugin.cpp \
            TestProducer.cpp \
    BlobProducer.cpp

HEADERS +=  plvtest_plugin.h \
            plvtest_global.h \
            TestProducer.h \
    BlobProducer.h

OTHER_FILES += \
    test.json
