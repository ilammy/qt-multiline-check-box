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
        checkBoxOption.icon = QIcon();
        painter.drawControl(QStyle::CE_CheckBox, checkBoxOption);
    }
#if 0
    {
        painter.drawItemText(m_textRect, m_textFlags, option.palette, isEnabled(), option.text, foregroundRole());
    }

#else

    {
        unsigned alignment = QStyle::visualAlignment(option.direction, Qt::AlignLeft | Qt::AlignVCenter);

        if (!option.icon.isNull())
        {
            QPixmap pixmap = option.icon.pixmap(option.iconSize, option.state & QStyle::State_Enabled ? QIcon::Normal : QIcon::Disabled);

            painter.drawItemPixmap(m_iconRect, alignment, pixmap);
        }

        if (!option.text.isEmpty())
        {
            painter.drawItemText(m_textRect, alignment | m_textFlags, option.palette,
                option.state & QStyle::State_Enabled, option.text, foregroundRole());
        }
    }

#if 0
    if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
        uint alignment = visualAlignment(btn->direction, Qt::AlignLeft | Qt::AlignVCenter);

        if (!proxy()->styleHint(SH_UnderlineShortcut, btn, widget))
            alignment |= Qt::TextHideMnemonic;
        QPixmap pix;
        QRect textRect = btn->rect;
        if (!btn->icon.isNull()) {
            pix = btn->icon.pixmap(btn->iconSize, btn->state & State_Enabled ? QIcon::Normal : QIcon::Disabled);
            proxy()->drawItemPixmap(p, btn->rect, alignment, pix);
            if (btn->direction == Qt::RightToLeft)
                textRect.setRight(textRect.right() - btn->iconSize.width() - 4);
            else
                textRect.setLeft(textRect.left() + btn->iconSize.width() + 4);
        }
        if (!btn->text.isEmpty()){
            proxy()->drawItemText(p, textRect, alignment | Qt::TextShowMnemonic,
                btn->palette, btn->state & State_Enabled, btn->text, QPalette::WindowText);
        }
    }
#endif
#endif


    if (option.state & QStyle::State_HasFocus)
    {
        QStyleOption focusOption = option;
        focusOption.rect = m_focusRect;
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, focusOption);
    }


#if 0
    {
        painter.setPen(Qt::red);
        painter.drawRect(m_contentRect.adjusted(0, 0, -1, -1));
        painter.setPen(Qt::magenta);
        painter.drawRect(m_textRect.adjusted(0, 0, -1, -1));
        painter.setPen(Qt::blue);
        painter.drawRect(m_focusRect.adjusted(0, 0, -1, -1));
        painter.setPen(Qt::green);
        painter.drawRect(style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option, this).adjusted(0, 0, -1, -1));
        painter.setPen(Qt::yellow);
        painter.drawRect(QRect(QPoint(0, 0), sizeForWidth(width())).adjusted(0, 0, -1, -1));
#if 0
        painter.setPen(Qt::cyan);
        painter.drawRect(QRect(QPoint(0, 0), m_minimumSizeHint).adjusted(0, 0, -1, -1));

        painter.setPen(Qt::black);
        painter.drawRect(QRect(QPoint(0, 0), m_sizeHint).adjusted(0, 0, -1, -1));
#endif
    }
#endif
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

#include <QApplication>

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

    m_sizeHint = m_sizeHint.expandedTo(QApplication::globalStrut());
    m_minimumSizeHint = m_minimumSizeHint.expandedTo(QApplication::globalStrut());
}

#if 0

Q_D(const QCheckBox);
if (d->sizeHint.isValid())
    return d->sizeHint;
ensurePolished();
QFontMetrics fm = fontMetrics();
QStyleOptionButton opt;
initStyleOption(&opt);
QSize sz = style()->itemTextRect(fm, QRect(), Qt::TextShowMnemonic, false,
                                 text()).size();
if (!opt.icon.isNull())
    sz = QSize(sz.width() + opt.iconSize.width() + 4, qMax(sz.height(), opt.iconSize.height()));
d->sizeHint = (style()->sizeFromContents(QStyle::CT_CheckBox, &opt, sz, this)
              .expandedTo(QApplication::globalStrut()));
return d->sizeHint;

#endif

void MultilineCheckBox::recalculateTextDimensions() const
{
    QStyleOptionButton option;
    initStyleOption(&option);

    // m_textFlags

    m_textFlags = Qt::TextWordWrap;

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

    // m_contentRect

    m_contentRect = style()->subElementRect(QStyle::SE_CheckBoxContents, &option, this);

    // m_iconRect

    m_iconRect = QRect();

    if (!option.icon.isNull())
    {
        unsigned alignment = QStyle::visualAlignment(option.direction, Qt::AlignLeft | Qt::AlignVCenter);

        m_iconRect = style()->itemPixmapRect(m_contentRect, alignment,
            option.icon.pixmap(option.iconSize, QIcon::Normal));
    }

    // m_textRect

    // As in QCommonStyle::drawControl(CE_CheckBoxLabel)

    m_textRect = m_contentRect;

    if (!option.icon.isNull())
    {
        if (option.direction == Qt::RightToLeft)
        {
            m_textRect.setRight(m_textRect.right() - option.iconSize.width() - 4);
        }
        else
        {
            m_textRect.setLeft(m_textRect.left() + option.iconSize.width() + 4);
        }
    }

#if 1
    // As in QCommonStyle::subElementRect(QStyle::SE_CheckBoxFocusRect)

    if (option.icon.isNull() && option.text.isEmpty())
    {
        m_focusRect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option, this);
        m_focusRect.adjust(1, 1, -1, -1);
    }
    else
    {
        QRect cr = QStyle::visualRect(option.direction, option.rect, style()->subElementRect(QStyle::SE_CheckBoxContents, &option, this));
#if 0
        QRect iconRect;
        QRect textRect;

        if (!option.text.isEmpty())
        {
            // add Qt::TextWordWrap
            textRect = style()->itemTextRect(option.fontMetrics, cr,
                Qt::AlignAbsolute | Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic |
                    Qt::TextWordWrap,
                option.state & QStyle::State_Enabled,
                option.text);
        }

        if (!option.icon.isNull())
        {
            iconRect = style()->itemPixmapRect(cr,
                Qt::AlignAbsolute | Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic,
                option.icon.pixmap(option.iconSize, QIcon::Normal));

            if (!textRect.isEmpty())
            {
#if 0
                textRect.translate(iconRect.right() + 4, 0);
#else
                textRect.moveLeft(iconRect.right() + 4);
#endif
            }
        }

        m_focusRect = iconRect | textRect;
        m_focusRect.adjust(-3, -2, 3, 2);
        m_focusRect = m_focusRect.intersected(option.rect);
        m_focusRect = QStyle::visualRect(option.direction, option.rect, m_focusRect);
#else
        QRect iconRect;
        QRect textRect;

        if (!option.icon.isNull())
        {
#if 1
            iconRect = m_iconRect;
#else
            iconRect = QRect(QPoint(0, 0), option.iconSize);
            iconRect.moveCenter(m_iconRect.center());
#endif
        }

        if (!option.text.isEmpty())
        {
            unsigned alignment = QStyle::visualAlignment(option.direction, Qt::AlignLeft | Qt::AlignVCenter);

            textRect = style()->itemTextRect(option.fontMetrics, m_textRect,
                alignment | m_textFlags,
                option.state & QStyle::State_Enabled,
                option.text);
        }

        m_focusRect = iconRect | textRect;
        m_focusRect.adjust(-3, -2, 3, 2);
        m_focusRect = m_focusRect.intersected(option.rect);
        //m_focusRect = QStyle::visualRect(option.direction, option.rect, m_focusRect);
#endif
    }
#else
#if 0
    if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
        if (btn->icon.isNull() && btn->text.isEmpty()) {
            r = subElementRect(SE_CheckBoxIndicator, opt, widget);
            r.adjust(1, 1, -1, -1);
            break;
        }
        // As above, deal with the logical first, then convert it back to screen coords.
        QRect cr = visualRect(btn->direction, btn->rect,
                              subElementRect(SE_CheckBoxContents, btn, widget));

        QRect iconRect, textRect;
        if (!btn->text.isEmpty()) {
            textRect = itemTextRect(opt->fontMetrics, cr, Qt::AlignAbsolute | Qt::AlignLeft
                                    | Qt::AlignVCenter | Qt::TextShowMnemonic,
                                    btn->state & State_Enabled, btn->text);
        }
        if (!btn->icon.isNull()) {
            iconRect = itemPixmapRect(cr, Qt::AlignAbsolute | Qt::AlignLeft | Qt::AlignVCenter
                                    | Qt::TextShowMnemonic,
                               btn->icon.pixmap(btn->iconSize, QIcon::Normal));
            if (!textRect.isEmpty())
                textRect.translate(iconRect.right() + 4, 0);
        }
        r = iconRect | textRect;
        r.adjust(-3, -2, 3, 2);
        r = r.intersected(btn->rect);
        r = visualRect(btn->direction, btn->rect, r);
    }
#endif
    m_focusRect = option.fontMetrics.boundingRect(m_textRect, m_textFlags, option.text).adjusted(-hMargin, -vMargin, +hMargin, +vMargin);
#endif
}

bool MultilineCheckBox::hitButton(const QPoint& pos) const
{
    QStyleOptionButton option;
    initStyleOption(&option);

    QRect hitRect = m_focusRect;

    // If the check box has an icon or text visible then extend the focus rectangle to include
    // the check box indicator as well. However, if only indicator is shown then leave it as is.

    if (!option.icon.isNull() || !option.text.isEmpty())
    {
        if (option.direction == Qt::RightToLeft)
        {
            hitRect.setRight(contentsRect().right());
        }
        else
        {
            hitRect.setLeft(contentsRect().left());
        }
    }

    return hitRect.contains(pos);
}

#include <QResizeEvent>

void MultilineCheckBox::resizeEvent(QResizeEvent *event)
{
    QCheckBox::resizeEvent(event);

    // Ensure that the height of the check box allows everything to be visible. Minimum size is
    // used in sizeForWidth() calculation so we temporarily set it is zero.

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
    QStyleOptionButton option;
    initStyleOption(&option);

    // First of all, clamp the width to is minimium possible amount if it is set.

    if (minimumWidth() > 0)
    {
        width = qMax(width, minimumWidth());
    }

    // Then we need to account for various fixed margins. The first one would be contents margin.

    QSize contentsMarginSize(contentsMargins().left() + contentsMargins().right(),
                             contentsMargins().top() + contentsMargins().bottom());

    // Then there is ths icon...

    QSize iconSize = option.iconSize;
    QSize iconMarginSize = QSize(option.iconSize.width() + 4, 0);

    // And we need to reserve the space for check box indicator. Calculate its size as implemented
    // in QCommonStyle::sizeFromContents(QStyle::CT_CheckBox) along with magic numbers from there.

    QSize indicatorSize;
    QSize indicatorMarginSize;

    {
        int w = style()->pixelMetric(QStyle::PM_IndicatorWidth, &option, this);
        int h = style()->pixelMetric(QStyle::PM_IndicatorHeight, &option, this);

        int margins = 0;

        if (!option.icon.isNull() || !option.text.isEmpty())
        {
            margins = 4 + style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &option, this);
        }

        indicatorSize = QSize(w, h);
        indicatorMarginSize = QSize(w + margins, 4);
    }

    // Now we must fit the text into whatever area is left and determine the necessary height.
    // However, if width is set to -1 then we are calculating a size hint and do not know the
    // actual required width. We will guess a suitable width just as QLabel does.

    bool tryWidth = (width < 0);

    if (width < 0)
    {
        width = maximumWidth();
    }

    width -= iconMarginSize.width();
    width -= contentsMarginSize.width();
    width -= indicatorMarginSize.width();
    width += 4;

    unsigned alignment = QStyle::visualAlignment(option.direction, Qt::AlignLeft);
    unsigned flags = m_textFlags | alignment;

    QRect textRect = fontMetrics().boundingRect(0, 0, width, 2000, flags, option.text);

    if (tryWidth)
    {
        if ((textRect.height() < 4 * fontMetrics().lineSpacing()) && (textRect.width() > width / 2))
        {
            textRect = fontMetrics().boundingRect(0, 0, width / 2, 2000, flags, option.text);
        }

        if ((textRect.height() < 2 * fontMetrics().lineSpacing()) && (textRect.width() > width / 4))
        {
            textRect = fontMetrics().boundingRect(0, 0, width / 4, 2000, flags, option.text);
        }
    }

    // Add up the sizes and make sure that the result fits into minimum size
    // and can hold at least the indicator.

    return (textRect.size() + contentsMarginSize + indicatorMarginSize + iconMarginSize)
            .expandedTo(minimumSize())
            .expandedTo(indicatorSize)
            .expandedTo(iconSize);

#if 0

    const QSize contentsSize(br.width() + hextra, br.height() + vextra);

    QSize actualSize = (contentsSize + contentMargins).expandedTo(minimumSize());



    if (tryWidth && (br.height() < 4 * fontMetrics().lineSpacing()) && (br.width() > width / 2))
    {
        br = fontMetrics().boundingRect(0, 0, width / 2, 2000, flags, text());
    }

    if (tryWidth && (br.height() < 2 * fontMetrics().lineSpacing()) && (br.width() > width / 4))
    {
        br = fontMetrics().boundingRect(0, 0, width / 4, 2000, flags, text());
    }

    // apply later, as in QCommonStyle::sizeFromContents(QStyle::CT_CheckBox).
    actualSize.setHeight(qMax(actualSize.height(), indicatorSize.height()));




    QSize contentMargins(contentsMargins().left() + contentsMargins().right(),
                         contentsMargins().top() + contentsMargins().bottom());

    QRect br;



    int hextra = contentMargins.width();
    int vextra = contentMargins.height();



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

    bool tryWidth = (width < 0);

    if (width < 0)
    {
        width = maximumSize().width();
    }

    width -= hextra;
    width -= contentMargins.width();

#if 1
    {
        // As in QCommonStyle::sizeFromContents(QStyle::CT_CheckBox)



        int margins = 0;

        if (!option.icon.isNull() || !option.text.isEmpty())
        {
            margins = 4 + style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &option, this);
        }

        width -= margins + 4;
    }
#endif
    br = fontMetrics().boundingRect(0, 0, width, 2000, flags, text());

    if (tryWidth && (br.height() < 4 * fontMetrics().lineSpacing()) && (br.width() > width / 2))
    {
        br = fontMetrics().boundingRect(0, 0, width / 2, 2000, flags, text());
    }

    if (tryWidth && (br.height() < 2 * fontMetrics().lineSpacing()) && (br.width() > width / 4))
    {
        br = fontMetrics().boundingRect(0, 0, width / 4, 2000, flags, text());
    }

    const QSize contentsSize(br.width() + hextra, br.height() + vextra);
    QSize actualSize = (contentsSize + contentMargins).expandedTo(minimumSize());

    {
        // As in QCommonStyle::sizeFromContents(QStyle::CT_CheckBox)

        QStyleOptionButton option;
        initStyleOption(&option);

        int w = style()->pixelMetric(QStyle::PM_IndicatorWidth, &option, this);
        int h = style()->pixelMetric(QStyle::PM_IndicatorHeight, &option, this);

        int margins = 0;

        if (!option.icon.isNull() || !option.text.isEmpty())
        {
            margins = 4 + style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &option, this);
        }


        actualSize += QSize(w + margins, 4);
        actualSize.setHeight(qMax(actualSize.height(), h));
    }

    return actualSize;


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
#endif
}
