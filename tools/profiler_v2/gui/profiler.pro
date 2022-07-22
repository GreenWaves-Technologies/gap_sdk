BACKEND_BUILD_DIR = ../backend/build
GAP_SDK_HOME = $$(GAP_SDK_HOME)

QT       += core gui
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -L$$BACKEND_BUILD_DIR -lprofiling
LIBS += -L$$GAP_SDK_HOME/install/workstation/lib 
LIBS += -lpulpvp-debug

PRE_TARGETDEPS += $$BACKEND_BUILD_DIR/libprofiling.a

TARGET = profiler
TEMPLATE = app

DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
UI_DIR = build/ui
RCC_DIR = build/rcc

SOURCES += $$files("src/*.cpp", true) \
    src/tlscrollbar.cpp \
    src/linenumberarea.cpp \
    src/codeeditor.cpp \
    src/codeeditorwrap.cpp \
    src/hotspotsfunctionwidget.cpp \
    src/hotspotsfilewidget.cpp \
    src/hotspotslinewidget.cpp \
    src/pccheckbox.cpp
HEADERS += $$files("include/*.hpp", true) \
    include/profiler_backend_user.hpp \
    src/tlscrollbar.h \
    include/safehotspot.hpp \
    include/codeeditorwrap.hpp \
    include/hotspotsfunctionwidget.hpp \
    include/hotspotsfilewidget.hpp \
    include/safehotspotfileinfo.hpp \
    include/safehotspotfunctioninfo.hpp \
    include/safehotspotlineinfo.hpp \
    include/hotspotslinewidget.hpp \
    include/pccheckbox.hpp
INCLUDEPATH += "../backend/include/"
FORMS += $$files("forms/*.ui", true)

INCLUDEPATH += include
INCLUDEPATH += $$GAP_SDK_HOME/install/workstation/include

CONFIG += debug
CONFIG += c++14 
QMAKE_CXXFLAGS += -g -Wall -Wextra -Werror 


RESOURCES = images/

QT_DEBUG_PLUGINS = 1
QML_IMPORT_TRACE = 1
