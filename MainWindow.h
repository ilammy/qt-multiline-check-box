// Copyright (c) 2016, ilammy
//
// Licensed under MIT license (see LICENSE file in the root directory).
// This file may be copied, distributed, and modified only in accordance
// with the terms specified by this license.

#ifndef MULTILINECHECKBOX_MAINWINDOW_H
#define MULTILINECHECKBOX_MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QScopedPointer<Ui::MainWindow> ui;
};

#endif // MULTILINECHECKBOX_MAINWINDOW_H
