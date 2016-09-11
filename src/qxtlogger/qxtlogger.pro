TARGET = qxtlogger
TEMPLATE = lib
CONFIG += dll
DEFINES         += BUILD_QXT_CORE

DEPENDPATH +=   .\
                ..
macx {

    # Make sure there is no mess in ./
    # but put all output files in build/(debug|release)
#    !debug_and_release|build_pass {
#        CONFIG(debug, debug|release):BUILDDIR = test/build/debug
#        CONFIG(release, debug|release):BUILDDIR = build/release
#    }
#    MOC_DIR = $$BUILDDIR
#    OBJECTS_DIR = $$BUILDDIR
#    RCC_DIR = $$BUILDDIR
#    UI_DIR = $$BUILDDIR
#    DESTDIR = $$BUILDDIR
}

DESTDIR = ../../libs

CONFIG(debug, debug|release):DEFINES += DEBUG

QT += core
QT += xml

#QMAKE_CXXFLAGS_DEBUG += -pedantic \
# -Wunused-parameter \
# -Wunused-variable

INCLUDEPATH += ../../include ../../include/plvcore

DEFINES += QXT PLV_DLL_EXPORTS

SOURCES += \
        qxtabstractfileloggerengine.cpp \
        qxtlogger.cpp \
        qxtabstractiologgerengine.cpp \
        qxtloggerengine.cpp \
        qxtbasicfileloggerengine.cpp  \
        qxtlogstream.cpp \
        qxtbasicstdloggerengine.cpp   \
        qxtxmlfileloggerengine.cpp

HEADERS += \
        qxtglobal.h \
        qxtabstractfileloggerengine.h  \
        qxtlogger_p.h \
        qxtabstractiologgerengine.h \
        qxtloggerengine.h \
        qxtbasicfileloggerengine.h \
        qxtlogstream.h \
        qxtbasicstdloggerengine.h \
        qxtlogstream_p.h \
        qxtlogger.h \
        qxtxmlfileloggerengine.h
