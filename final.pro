
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport serialport

TARGET = blockingmaster
TEMPLATE = app

HEADERS += \
    dialog.h \
    qcustomplot.h

SOURCES += \
    main.cpp \
    dialog.cpp \
    qcustomplot.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/serialport/blockingmaster
INSTALLS += target

FORMS += \
    dialog.ui
