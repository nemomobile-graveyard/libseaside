VERSION = 0.0.42

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
CONFIG += mobility qt debug link_pkgconfig
MOBILITY += contacts
SUBDIRS += tests

HEADERS += \
    $$MODEL_HEADERS \
    $$STYLE_HEADERS \
    src/seaside.h \
    src/seasidedetail.h \
    src/seasideproxymodel.h \
    src/seasidesyncmodel.h  \
    src/seasidesyncmodel_p.h
SOURCES += \
    src/seaside.cpp \
    src/seasidedetail.cpp \
    src/seasideproxymodel.cpp \
    src/seasidesyncmodel.cpp \
    src/seasidesyncmodel_p.cpp

target.path=$$LIBDIR

headers.files = $$HEADERS
headers.path = $$INCLUDEDIR

INSTALLS += \
    target \
    headers

OTHER_FILES += \
    README


