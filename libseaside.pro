VERSION = 0.0.26

unix{
    PREFIX=/usr
    LIBDIR= $$PREFIX/lib
    INCLUDEDIR=$$PREFIX/include
}
TEMPLATE = lib subdirs
TARGET = seaside
MOC_DIR = .moc
OBJECTS_DIR = .obj
MGEN_OUTDIR = .gen
DEPENDPATH += .
INCLUDEPATH += ./src
CONFIG += meegotouch
CONFIG += mobility link_pkgconfig
PKGCONFIG += meegotouch QtContacts
MOBILITY += contacts
SUBDIRS += tests

MODEL_HEADERS += \
    src/seasidepersonmodel.h
STYLE_HEADERS += \
    src/seasidelabelstyle.h
HEADERS += \
    $$MODEL_HEADERS \
    $$STYLE_HEADERS \
    src/seaside.h \
    src/seasidedetail.h \
    src/seasidelabel.h \
    src/seasidelist.h \
    src/seasidelistitem.h \
    src/seasidelistmodel.h \
    src/seasideproxymodel.h \
    src/seasidesyncmodel.h
SOURCES += \
    src/seaside.cpp \
    src/seasidedetail.cpp \
    src/seasidelabel.cpp \
    src/seasidelist.cpp \
    src/seasidelistitem.cpp \
    src/seasidelistmodel.cpp \
    src/seasidepersonmodel.cpp \
    src/seasideproxymodel.cpp \
    src/seasidesyncmodel.cpp
M_THEME_DIR = $$PREFIX/share/themes/base/meegotouch
SEASIDE_THEME_DIR = $$M_THEME_DIR/lib$$TARGET
DEFINES += IMAGES_DIR=\"\\\"$$SEASIDE_THEME_DIR/images/\\\"\" 

target.path=$$LIBDIR

headers.files = $$HEADERS
headers.path = $$INCLUDEDIR

theme.files = theme/*
theme.path = $$SEASIDE_THEME_DIR
theme.CONFIG += no_check_exist

INSTALLS += \
    target \
    headers \
    theme

OTHER_FILES += \
    README

CONFIG += qt debug
