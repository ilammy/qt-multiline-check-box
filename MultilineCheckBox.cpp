// Copyright (c) 2016, ilammy
//
// Licensed under MIT license (see LICENSE file in the root directory).
// This file may be copied, distributed, and modified only in accordance
// with the terms specified by this license.

#include "MultilineCheckBox.h"

#include <QApplication>
#include <QPainter>
#include <QResizeEvent>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStylePainter>

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

//
// Sizing and layout
//

QSize MultilineCheckBox::sizeHint() const
{
    QSize sizeHint;
    QSize minimumSizeHint;

    calculateSizeHints(sizeHint, minimumSizeHint);

    return sizeHint;
}

QSize MultilineCheckBox::minimumSizeHint() const
{
    QSize sizeHint;
    QSize minimumSizeHint;

    calculateSizeHints(sizeHint, minimumSizeHint);

    return minimumSizeHint;
}

void MultilineCheckBox::calculateSizeHints(QSize& sizeHint, QSize& minimumSizeHint) const
{
    ensurePolished();

    // Actual size hint is calculated by sizeForWidth() as a special case. It will find a suitable
    // size for check box content when there are no restrictions on it placed by the layout.

    sizeHint = sizeForWidth(-1);

    // We cannot calculate a suitable minimum size because width and height of the check box are
    // interdependent and the best option depends on the restrictions placed by the layout. But
    // we know nothing of these restrictions. so we use the minimum possible height and width.

    minimumSizeHint.setHeight(sizeForWidth(maximumWidth()).height());
    minimumSizeHint.setWidth(sizeForWidth(0).width());

    // Finally, apply some minor adjustments made by QLabel for the sake of consistency.

    if (sizeHint.height() < minimumSizeHint.height())
    {
        minimumSizeHint.setHeight(sizeHint.height());
    }

    sizeHint = sizeHint.expandedTo(QApplication::globalStrut());
    minimumSizeHint = minimumSizeHint.expandedTo(QApplication::globalStrut());
}

int MultilineCheckBox::heightForWidth(int width) const
{
    return sizeForWidth(width).height();
}

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

//
// Hit-testing
//

bool MultilineCheckBox::hitButton(const QPoint& pos) const
{
    QRect contentRect, iconRect, textRect, focusRect;
    calculateContentRectangles(contentRect, iconRect, textRect, focusRect);

    QRect hitRect = focusRect;

    // If the check box has an icon or text visible then extend the focus rectangle to include
    // the check box indicator as well. However, if only indicator is shown then leave it as is.

    if (!icon().isNull() || !text().isEmpty())
    {
        if (layoutDirection() == Qt::RightToLeft)
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

//
// Painting
//

void MultilineCheckBox::paintEvent(QPaintEvent*)
{
    QRect contentRect, iconRect, textRect, focusRect;
    calculateContentRectangles(contentRect, iconRect, textRect, focusRect);

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

    {
        unsigned alignment = calculateVisualAlignment();
        unsigned textFlags = calculateTextFlags();

        if (!icon().isNull())
        {
            QPixmap pixmap = option.icon.pixmap(iconSize(), isEnabled() ? QIcon::Normal : QIcon::Disabled);

            painter.drawItemPixmap(iconRect, alignment, pixmap);
        }

        if (!text().isEmpty())
        {
            painter.drawItemText(textRect, textFlags, palette(), isEnabled(), text(), foregroundRole());
        }
    }

    if (option.state & QStyle::State_HasFocus)
    {
        QStyleOptionButton focusOption = option;
        focusOption.rect = focusRect;
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, focusOption);
    }
}

//
//
//

unsigned MultilineCheckBox::calculateTextFlags() const
{
    unsigned textFlags = Qt::TextWordWrap;

    textFlags |= calculateVisualAlignment();

    if (text().isRightToLeft())
    {
        textFlags |= Qt::TextForceRightToLeft;
    }
    else
    {
        textFlags |= Qt::TextForceLeftToRight;
    }

    if (!shortcut().isEmpty())
    {
        textFlags |= Qt::TextShowMnemonic;

        QStyleOptionButton option;
        initStyleOption(&option);

        if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &option, this))
        {
            textFlags |= Qt::TextHideMnemonic;
        }
    }

    return textFlags;
}

unsigned MultilineCheckBox::calculateVisualAlignment() const
{
    return QStyle::visualAlignment(layoutDirection(), Qt::AlignLeft | Qt::AlignVCenter);
}

void MultilineCheckBox::calculateContentRectangles(QRect& contentRect, QRect& iconRect, QRect& textRect, QRect& focusRect) const
{
    QStyleOptionButton option;
    initStyleOption(&option);

    unsigned alignment = calculateVisualAlignment();

    //

    contentRect = style()->subElementRect(QStyle::SE_CheckBoxContents, &option, this);

    //

    iconRect = QRect();

    if (!icon().isNull())
    {
        iconRect = style()->itemPixmapRect(contentRect, alignment, icon().pixmap(iconSize(), QIcon::Normal));
    }

    //

    textRect = contentRect;

    if (!icon().isNull())
    {
        // As in QCommonStyle::drawControl(CE_CheckBoxLabel)

        if (layoutDirection() == Qt::RightToLeft)
        {
            textRect.setRight(textRect.right() - iconSize().width() - 4);
        }
        else
        {
            textRect.setLeft(textRect.left() + iconSize().width() + 4);
        }
    }

    //

    // As in QCommonStyle::subElementRect(QStyle::SE_CheckBoxFocusRect)

    if (icon().isNull() && text().isEmpty())
    {
        focusRect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option, this);
        focusRect.adjust(1, 1, -1, -1);
    }
    else
    {
        QRect iconFocusRect;
        QRect textFocusRect;

        if (!icon().isNull())
        {
            iconFocusRect = iconRect;
        }

        if (!text().isEmpty())
        {
            textFocusRect = style()->itemTextRect(fontMetrics(), textRect, calculateTextFlags(), isEnabled(), text());
        }

        focusRect = iconFocusRect | textFocusRect;
        focusRect.adjust(-3, -2, 3, 2);
        focusRect = focusRect.intersected(rect());
    }
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

    unsigned flags = calculateTextFlags();

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
}
