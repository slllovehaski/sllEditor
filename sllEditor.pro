QT += widgets uitools


HEADERS         = mainwindow.h \
    textfinder.h \
    xbelreader.h \
    xbelwriter.h \
    codeeditor.h \
    textfilter.h
SOURCES         = main.cpp \
                  mainwindow.cpp \
    textfinder.cpp \
    xbelreader.cpp \
    xbelwriter.cpp \
    codeeditor.cpp \
    textfilter.cpp

RESOURCES = textfinder.qrc

FORMS +=

DISTFILES += \
    input.txt
