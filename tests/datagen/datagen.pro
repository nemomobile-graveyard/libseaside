TEMPLATE = app
TARGET = datagen
MOC_DIR = .moc
OBJECTS_DIR = .obj
MGEN_OUTDIR = .gen
LIBS += -L$$PREFIX/lib
INCLUDEPATH += $$PREFIX/include
CONFIG += meegotouch mobility qt debug link_pkgconfig
PKGCONFIG += meegotouch QtContacts
LIBS += -lseaside
HEADERS += datagenmodel.h
SOURCES +=  main.cpp \
            datagenmodel.cpp


