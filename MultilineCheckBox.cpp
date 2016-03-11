// Copyright (c) 2016, ilammy
//
// Licensed under MIT license (see LICENSE file in the root directory).
// This file may be copied, distributed, and modified only in accordance
// with the terms specified by this license.

#include "MultilineCheckBox.h"

#include <QPainter>
#include <QStyleOption>

#include <QStylePainter>
#include <QStyleOptionButton>

MultilineCheckBox::MultilineCheckBox(QWidget *parent)
  : QCheckBox(parent)
{
    QSizePolicy policy = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed, QSizePolicy::CheckBox);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
}

MultilineCheckBox::MultilineCheckBox(const QString& text, QWidget *parent)
  : QCheckBox(text, parent)
{
    QSizePolicy policy = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed, QSizePolicy::CheckBox);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
}

void MultilineCheckBox::paintEvent(QPaintEvent*)
{
    recalculateHints();
    recalculateTextDimensions();

    //     +--PM_IndicatorWidth x PM_IndicatorHeight
    //     |
    // +---|----------------------------+ <----- rect()
    // | +-v--------------------------+ |
    // | |[x] |Some very long text    |<------ contentsRect()
    // | |    |that spans multiple    | |
    // | |  | |lines                  | |
    // | +--| |-----------------------+ |
    // +----|-|-------------------------+
    //      | |
    //      >-< --- PM_CheckBoxLabelSpacing

    QStylePainter painter(this);

    QStyleOptionButton option;
    initStyleOption(&option);

    {
        QStyleOptionButton checkBoxOption = option;
        checkBoxOption.state &= ~QStyle::State_HasFocus;
        checkBoxOption.text.clear();
        painter.drawControl(QStyle::CE_CheckBox, checkBoxOption);
    }

    {
        painter.drawItemText(m_textRect, m_textFlags, option.palette, isEnabled(), option.text, foregroundRole());
    }

    if (option.state & QStyle::State_HasFocus)
    {
        QStyleOption textOption = option;
        textOption.rect = m_focusRect;
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, textOption);
    }
}

QSize MultilineCheckBox::sizeHint() const
{
    recalculateHints();
    return m_sizeHint;
}

QSize MultilineCheckBox::minimumSizeHint() const
{
    recalculateHints();
    return m_minimumSizeHint;
}

void MultilineCheckBox::recalculateHints() const
{
    ensurePolished();

    m_sizeHint = sizeForWidth(-1);

    m_minimumSizeHint.rheight() = sizeForWidth(width()).height();
    m_minimumSizeHint.rwidth()  = sizeForWidth(0).width();

    if (m_sizeHint.height() < m_minimumSizeHint.height())
    {
        m_minimumSizeHint.rheight() = m_sizeHint.height();
    }
}

void MultilineCheckBox::recalculateTextDimensions() const
{
    QStyleOptionButton option;
    initStyleOption(&option);

    Qt::Alignment hAlign = option.direction == Qt::RightToLeft ? Qt::AlignRight : Qt::AlignLeft;

    m_textFlags = Qt::TextWordWrap;

    m_textFlags |= hAlign;

    if (option.text.isRightToLeft())
    {
        m_textFlags |= Qt::TextForceRightToLeft;
    }
    else
    {
        m_textFlags |= Qt::TextForceLeftToRight;
    }

    if (!shortcut().isEmpty())
    {
        m_textFlags |= Qt::TextShowMnemonic;

        if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &option, this))
        {
            m_textFlags |= Qt::TextHideMnemonic;
        }
    }

    int hMargin = style()->pixelMetric(QStyle::PM_FocusFrameHMargin, &option, this);
    int vMargin = style()->pixelMetric(QStyle::PM_FocusFrameVMargin, &option, this);

    m_textRect = style()->subElementRect(QStyle::SE_CheckBoxContents, &option, this);

#define ADJUST 1

#if ADJUST
    hMargin += 1;
    vMargin += 3;
    m_textRect.adjust(0, 2, 0, 2);
#endif

    m_focusRect = option.fontMetrics.boundingRect(m_textRect, m_textFlags, option.text).adjusted(-hMargin, -vMargin, +hMargin, +vMargin);
}

bool MultilineCheckBox::hitButton(const QPoint& pos) const
{
    QStyleOptionButton option;
    initStyleOption(&option);

    QRect hitRect = m_focusRect;

    if (option.direction == Qt::RightToLeft)
    {
        hitRect.setRight(contentsRect().right());
    }
    else
    {
        hitRect.setLeft(contentsRect().left());
    }

    return hitRect.contains(pos);
}

#include <QResizeEvent>

void MultilineCheckBox::resizeEvent(QResizeEvent *event)
{
    QCheckBox::resizeEvent(event);

    int minWidth = minimumWidth();
    setMinimumSize(0, 0);
    int minHeight = heightForWidth(event->size().width());
    setMinimumSize(minWidth, minHeight);
}

int MultilineCheckBox::heightForWidth(int width) const
{
    return sizeForWidth(width).height();
}

QSize MultilineCheckBox::sizeForWidth(int width) const
{
    // As in QLabel

    if (minimumWidth() > 0)
    {
        width = qMax(width, minimumWidth());
    }

    QSize contentMargins(contentsMargins().left() + contentsMargins().right(),
                         contentsMargins().top() + contentsMargins().bottom());

    QRect br;

    int align = Qt::AlignLeft | Qt::TextWordWrap;

    int hextra = contentMargins.width();
    int vextra = contentMargins.height();

    int flags = align;

#if 0
    if (!shortcut().isEmpty())
    {
#ifndef QT_NO_SHORTCUT
    if (!shortcut().isEmpty())
    {
        flags |= Qt::TextShowMnemonic;

        QStyleOption styleOption;
        styleOption.initFrom(this);

        if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &styleOption, this))
        {
            flags |= Qt::TextHideMnemonic;
        }
    }
#endif
#endif

    bool tryWidth = (width < 0) && (align & Qt::TextWordWrap);

    if (width < 0)
    {
        if (align & Qt::TextWordWrap)
        {
            width = maximumSize().width();
        }
        else
        {
            width = 2000;
        }
    }

    width -= hextra;
    width -= contentMargins.width();

    QStyleOption option;
    option.initFrom(this);

    int x = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option, this).width();

    br = fontMetrics().boundingRect(x, 0, width, 2000, flags, text());

    if (tryWidth && (br.height() < 4 * fontMetrics().lineSpacing()) && (br.width() > width / 2))
    {
        br = fontMetrics().boundingRect(x, 0, width / 2, 2000, flags, text());
    }

    if (tryWidth && (br.height() < 2 * fontMetrics().lineSpacing()) && (br.width() > width / 4))
    {
        br = fontMetrics().boundingRect(x, 0, width / 4, 2000, flags, text());
    }
#if ADJUST
    const QSize contentsSize(x + br.width() + hextra, br.height() + vextra + 4);
#else
    const QSize contentsSize(x + br.width() + hextra, br.height() + vextra);
#endif
    return (contentsSize + contentMargins).expandedTo(minimumSize());


#if 0
    bool tryWidth = (w < 0) && (align & Qt::TextWordWrap);
    if (tryWidth)
        w = qMin(fm.averageCharWidth() * 80, q->maximumSize().width());
    else if (w < 0)
        w = 2000;
    w -= (hextra + contentsMargin.width());
    br = fm.boundingRect(0, 0, w ,2000, flags, text);
    if (tryWidth && br.height() < 4*fm.lineSpacing() && br.width() > w/2)
        br = fm.boundingRect(0, 0, w/2, 2000, flags, text);
    if (tryWidth && br.height() < 2*fm.lineSpacing() && br.width() > w/4)
        br = fm.boundingRect(0, 0, w/4, 2000, flags, text);




    Q_Q(const QLabel);
    if(q->minimumWidth() > 0)
        w = qMax(w, q->minimumWidth());
    QSize contentsMargin(leftmargin + rightmargin, topmargin + bottommargin);

    QRect br;

    int hextra = 2 * margin;
    int vextra = hextra;
    QFontMetrics fm = q->fontMetrics();

    if (pixmap && !pixmap->isNull())
        br = pixmap->rect();
#ifndef QT_NO_PICTURE
    else if (picture && !picture->isNull())
        br = picture->boundingRect();
#endif
#ifndef QT_NO_MOVIE
    else if (movie && !movie->currentPixmap().isNull())
        br = movie->currentPixmap().rect();
#endif
    else if (isTextLabel) {
        int align = QStyle::visualAlignment(textDirection(), QFlag(this->align));
        // Add indentation
        int m = indent;

        if (m < 0 && q->frameWidth()) // no indent, but we do have a frame
            m = fm.width(QLatin1Char('x')) - margin*2;
        if (m > 0) {
            if ((align & Qt::AlignLeft) || (align & Qt::AlignRight))
                hextra += m;
            if ((align & Qt::AlignTop) || (align & Qt::AlignBottom))
                vextra += m;
        }

        if (control) {
            ensureTextLayouted();
            const qreal oldTextWidth = control->textWidth();
            // Calculate the length of document if w is the width
            if (align & Qt::TextWordWrap) {
                if (w >= 0) {
                    w = qMax(w-hextra-contentsMargin.width(), 0); // strip margin and indent
                    control->setTextWidth(w);
                } else {
                    control->adjustSize();
                }
            } else {
                control->setTextWidth(-1);
            }

            QSizeF controlSize = control->size();
            br = QRect(QPoint(0, 0), QSize(qCeil(controlSize.width()), qCeil(controlSize.height())));

            // restore state
            control->setTextWidth(oldTextWidth);
        } else {
            // Turn off center alignment in order to avoid rounding errors for centering,
            // since centering involves a division by 2. At the end, all we want is the size.
            int flags = align & ~(Qt::AlignVCenter | Qt::AlignHCenter);
            if (hasShortcut) {
                flags |= Qt::TextShowMnemonic;
                QStyleOption opt;
                opt.initFrom(q);
                if (!q->style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, q))
                    flags |= Qt::TextHideMnemonic;
            }

            bool tryWidth = (w < 0) && (align & Qt::TextWordWrap);
            if (tryWidth)
                w = qMin(fm.averageCharWidth() * 80, q->maximumSize().width());
            else if (w < 0)
                w = 2000;
            w -= (hextra + contentsMargin.width());
            br = fm.boundingRect(0, 0, w ,2000, flags, text);
            if (tryWidth && br.height() < 4*fm.lineSpacing() && br.width() > w/2)
                br = fm.boundingRect(0, 0, w/2, 2000, flags, text);
            if (tryWidth && br.height() < 2*fm.lineSpacing() && br.width() > w/4)
                br = fm.boundingRect(0, 0, w/4, 2000, flags, text);
        }
    } else {
        br = QRect(QPoint(0, 0), QSize(fm.averageCharWidth(), fm.lineSpacing()));
    }

    const QSize contentsSize(br.width() + hextra, br.height() + vextra);
    return (contentsSize + contentsMargin).expandedTo(q->minimumSize());
#endif
}
