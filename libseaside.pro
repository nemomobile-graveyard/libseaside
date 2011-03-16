VERSION = 0.0.38

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
CONFIG += meegotouch mobility qt debug link_pkgconfig
PKGCONFIG += meegotouch QtContacts
MOBILITY += contacts
SUBDIRS += tests

MODEL_HEADERS += \
    src/seasidepersonmodel.h
STYLE_HEADERS += \
    src/seasidelabelstyle.h \
    src/seasideimagelabelstyle.h
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
    src/seasidesyncmodel.h  \
    src/seasidedetailview.h \
    src/seasidecommspage.h \
    src/seasideeditview.h \
    src/seasideperson.h \
    src/seasideimagelabel.h \
    src/seasidecellcreators.h \
    src/seasidelistitemsmall.h \
    src/seasidelistitemlarge.h
SOURCES += \
    src/seaside.cpp \
    src/seasidedetail.cpp \
    src/seasidelabel.cpp \
    src/seasidelist.cpp \
    src/seasidelistitem.cpp \
    src/seasidelistmodel.cpp \
    src/seasidepersonmodel.cpp \
    src/seasideproxymodel.cpp \
    src/seasidesyncmodel.cpp \
    src/seasidedetailview.cpp \
    src/seasidecommspage.cpp \
    src/seasideeditview.cpp \
    src/seasideperson.cpp \
    src/seasideimagelabel.cpp \
    src/seasidecellcreators.cpp \
    src/seasidelistitemsmall.cpp \
    src/seasidelistitemlarge.cpp
M_THEME_DIR = $$PREFIX/share/themes/base/meegotouch
SEASIDE_THEME_DIR = $$M_THEME_DIR/lib$$TARGET
DEFINES += IMAGES_DIR=\"\\\"$$SEASIDE_THEME_DIR/images/\\\"\" 

target.path=$$LIBDIR

headers.files = $$HEADERS
headers.path = $$INCLUDEDIR

theme.files = theme/*
theme.path = $$SEASIDE_THEME_DIR
theme.CONFIG += no_check_exist

# private headers, that shouldn't be installed
HEADERS += src/seasidelistitem_p.h

INSTALLS += \
    target \
    headers \
    theme

OTHER_FILES += \
    theme/styles/libseaside.css \
    theme/libseaside.conf \
    README


