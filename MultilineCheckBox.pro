# Copyright (c) 2016, ilammy
#
# Licensed under MIT license (see LICENSE file in the root directory).
# This file may be copied, distributed, and modified only in accordance
# with the terms specified by this license.

TARGET = MultilineCheckBox

TEMPLATE = app

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    MultilineCheckBox.cpp \

HEADERS += \
    MainWindow.h \
    MultilineCheckBox.h \

FORMS += \
    MainWindow.ui

OTHER_FILES += \
    README.md \
    LICENSE
