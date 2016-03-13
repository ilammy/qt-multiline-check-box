// Copyright (c) 2016, ilammy
//
// Licensed under MIT license (see LICENSE file in the root directory).
// This file may be copied, distributed, and modified only in accordance
// with the terms specified by this license.

#ifndef MULTILINECHECKBOX_MULTILINECHECKBOX_H
#define MULTILINECHECKBOX_MULTILINECHECKBOX_H

#include <QCheckBox>

class MultilineCheckBox : public QCheckBox
{
    Q_OBJECT

public:
    explicit MultilineCheckBox(QWidget *parent = 0);
    explicit MultilineCheckBox(const QString& text, QWidget *parent = 0);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    int heightForWidth(int) const;

protected:
    bool hitButton(const QPoint& pos) const;
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    Q_DISABLE_COPY(MultilineCheckBox)

    void recalculateHints() const;
    void recalculateTextDimensions() const;

    QSize sizeForWidth(int width) const;

private:
    mutable QSize m_sizeHint;
    mutable QSize m_minimumSizeHint;
    mutable QRect m_contentRect;
    mutable QRect m_iconRect;
    mutable QRect m_textRect;
    mutable QRect m_focusRect;
    mutable int m_textFlags;
};

#endif // MULTILINECHECKBOX_MULTILINECHECKBOX_H
