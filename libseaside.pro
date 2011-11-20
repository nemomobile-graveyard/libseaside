VERSION = 0.1.4

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
LIBS += -licuuc -licui18n

HEADERS += \
    $$MODEL_HEADERS \
    $$STYLE_HEADERS \
    src/seaside.h \
    src/seasidedetail.h \
    src/seasideproxymodel.h \
    src/seasidepeoplemodel.h \
    src/seasideperson.h \
    src/localeutils_p.h

SOURCES += \
    src/seaside.cpp \
    src/seasidedetail.cpp \
    src/seasideproxymodel.cpp \
    src/seasidepeoplemodel.cpp \
    src/seasidepeoplemodel_p.cpp \
    src/seasideperson.cpp \
    src/settingsdatastore.cpp \
    src/localeutils.cpp

target.path=$$LIBDIR

headers.files = $$HEADERS
headers.path = $$INCLUDEDIR

# private headers
HEADERS += src/seasidepeoplemodel_p.h \
           src/settingsdatastore_p.h

INSTALLS += \
    target \
    headers

OTHER_FILES += \
    README


