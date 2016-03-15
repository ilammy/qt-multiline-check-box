// Copyright (c) 2016, ilammy
//
// Licensed under MIT license (see LICENSE file in the root directory).
// This file may be copied, distributed, and modified only in accordance
// with the terms specified by this license.

#ifndef MULTILINECHECKBOX_MULTILINECHECKBOX_H
#define MULTILINECHECKBOX_MULTILINECHECKBOX_H

#include <QCheckBox>
#include <QScopedPointer>

class MultilineCheckBoxPrivate;

class MultilineCheckBox : public QCheckBox
{
    Q_OBJECT

public:
    explicit MultilineCheckBox(QWidget *parent = 0);
    explicit MultilineCheckBox(const QString& text, QWidget *parent = 0);
    ~MultilineCheckBox();

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    int heightForWidth(int width) const;

protected:
    bool hitButton(const QPoint& pos) const;
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    Q_DISABLE_COPY(MultilineCheckBox)
    Q_DECLARE_PRIVATE(MultilineCheckBox)

    QScopedPointer<MultilineCheckBoxPrivate> d_ptr;
};

#endif // MULTILINECHECKBOX_MULTILINECHECKBOX_H
