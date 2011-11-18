VERSION = 0.1.0

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
MOBILITY += contacts versit
SUBDIRS += tests

HEADERS += \
    $$MODEL_HEADERS \
    $$STYLE_HEADERS \
    src/seaside.h \
    src/seasidedetail.h \
    src/seasideproxymodel.h \
    src/seasidepeoplemodel.h

SOURCES += \
    src/seaside.cpp \
    src/seasidedetail.cpp \
    src/seasideproxymodel.cpp \
    src/seasidepeoplemodel.cpp \
    src/settingsdatastore.cpp \
    src/localeutils.cpp

target.path=$$LIBDIR

headers.files = $$HEADERS
headers.path = $$INCLUDEDIR

# private headers
HEADERS += src/seasidepeoplemodel_p.h \
           src/settingsdatastore_p.h \
           src/localeutils_p.h

INSTALLS += \
    target \
    headers

OTHER_FILES += \
    README


