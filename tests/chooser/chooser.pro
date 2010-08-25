TEMPLATE = app
TARGET = chooser
MOC_DIR = .moc
OBJECTS_DIR = .obj
MGEN_OUTDIR = .gen
LIBS += -L$$PREFIX/lib
INCLUDEPATH += $$PREFIX/include
CONFIG += meegotouch mobility qt debug link_pkgconfig
PKGCONFIG += meegotouch QtContacts
LIBS += -lseaside
HEADERS += \
    chooser.h
SOURCES = \
    chooser.cpp

M_THEME_DIR = $$PREFIX/share/themes/base/meegotouch
CHOOSER_THEME_DIR = $$M_THEME_DIR/$$TARGET

theme.files = theme/*
theme.path = $$CHOOSER_THEME_DIR
theme.CONFIG += no_check_exist

INSTALLS += \
    theme

OTHER_FILES += \
    theme/styles/chooser.css \
    theme/chooser.conf \

