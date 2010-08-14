CONFIG += meegotouch mobility
TEMPLATE = app
MOC_DIR = .moc
OBJECTS_DIR = .obj
LIBS += -lseaside -lQtContacts
HEADERS += \
    chooser.h
SOURCES = \
    chooser.cpp

LIBS += -L$$PREFIX/lib
INCLUDEPATH += $$PREFIX/include
TARGET = chooser

OTHER_FILES = \
    chooser.css

CONFIG += qt debug
