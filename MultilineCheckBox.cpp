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

class MultilineCheckBoxPrivate
{
    Q_DECLARE_PUBLIC(MultilineCheckBox)
    MultilineCheckBox *q_ptr;

public:
    MultilineCheckBoxPrivate(MultilineCheckBox *checkBox);

    QSize sizeForWidth(int width) const;

    unsigned textFlags() const;
    unsigned visualAlignment() const;

    mutable QSize sizeHint;
    mutable QSize minimumSizeHint;

    void updateSizeHints() const;

    mutable QRect contentRect;
    mutable QRect iconRect;
    mutable QRect textRect;
    mutable QRect focusRect;

    void updateContentRects() const;
};

MultilineCheckBoxPrivate::MultilineCheckBoxPrivate(MultilineCheckBox *checkBox)
  : q_ptr(checkBox)
{
}

//
// Construction
//

MultilineCheckBox::MultilineCheckBox(QWidget *parent)
  : QCheckBox(parent)
  , d_ptr(new MultilineCheckBoxPrivate(this))
{
    QSizePolicy policy = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed, QSizePolicy::CheckBox);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
}

MultilineCheckBox::MultilineCheckBox(const QString& text, QWidget *parent)
  : QCheckBox(text, parent)
  , d_ptr(new MultilineCheckBoxPrivate(this))
{
    QSizePolicy policy = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed, QSizePolicy::CheckBox);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);
}

MultilineCheckBox::~MultilineCheckBox()
{
    // QScopedPointer needs an explicit destructor when applied to forward-declared types
}

//
// Sizing and layout
//

QSize MultilineCheckBox::sizeHint() const
{
    Q_D(const MultilineCheckBox);

    d->updateSizeHints();

    return d->sizeHint;
}

QSize MultilineCheckBox::minimumSizeHint() const
{
    Q_D(const MultilineCheckBox);

    d->updateSizeHints();

    return d->minimumSizeHint;
}

void MultilineCheckBoxPrivate::updateSizeHints() const
{
    Q_Q(const MultilineCheckBox);

    q->ensurePolished();

    // Actual size hint is calculated by sizeForWidth() as a special case. It will find a suitable
    // size for check box content when there are no restrictions on it placed by the layout.

    sizeHint = sizeForWidth(-1);

    // We cannot calculate a suitable minimum size because width and height of the check box are
    // interdependent and the best option depends on the restrictions placed by the layout. But
    // we know nothing of these restrictions. so we use the minimum possible height and width.

    minimumSizeHint.setHeight(sizeForWidth(q->maximumWidth()).height());
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
    Q_D(const MultilineCheckBox);

    return d->sizeForWidth(width).height();
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
    Q_D(const MultilineCheckBox);

    d->updateContentRects();

    QRect hitRect = d->focusRect;

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
    Q_D(MultilineCheckBox);

    d->updateContentRects();

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
        if (!icon().isNull())
        {
            QPixmap pixmap = option.icon.pixmap(iconSize(), isEnabled() ? QIcon::Normal : QIcon::Disabled);

            painter.drawItemPixmap(d->iconRect, d->visualAlignment(), pixmap);
        }

        if (!text().isEmpty())
        {
            painter.drawItemText(d->textRect, d->textFlags(), palette(), isEnabled(), text(), foregroundRole());
        }
    }

    if (option.state & QStyle::State_HasFocus)
    {
        QStyleOptionButton focusOption = option;
        focusOption.rect = d->focusRect;
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, focusOption);
    }
}

//
//
//

unsigned MultilineCheckBoxPrivate::textFlags() const
{
    Q_Q(const MultilineCheckBox);

    unsigned textFlags = Qt::TextWordWrap;

    textFlags |= visualAlignment();

    if (q->text().isRightToLeft())
    {
        textFlags |= Qt::TextForceRightToLeft;
    }
    else
    {
        textFlags |= Qt::TextForceLeftToRight;
    }

    if (!q->shortcut().isEmpty())
    {
        textFlags |= Qt::TextShowMnemonic;

        QStyleOptionButton option;
        q->initStyleOption(&option);

        if (!q->style()->styleHint(QStyle::SH_UnderlineShortcut, &option, q))
        {
            textFlags |= Qt::TextHideMnemonic;
        }
    }

    return textFlags;
}

unsigned MultilineCheckBoxPrivate::visualAlignment() const
{
    Q_Q(const MultilineCheckBox);

    QStyleOptionButton option;
    q->initStyleOption(&option);

    return QStyle::visualAlignment(option.direction, Qt::AlignLeft | Qt::AlignVCenter);
}

void MultilineCheckBoxPrivate::updateContentRects() const
{
    Q_Q(const MultilineCheckBox);

    QStyleOptionButton option;
    q->initStyleOption(&option);

    //

    contentRect = q->style()->subElementRect(QStyle::SE_CheckBoxContents, &option, q);

    //

    iconRect = QRect();

    if (!option.icon.isNull())
    {
        iconRect = q->style()->itemPixmapRect(contentRect, visualAlignment(), option.icon.pixmap(option.iconSize, QIcon::Normal));
    }

    //

    textRect = contentRect;

    if (!option.icon.isNull())
    {
        // As in QCommonStyle::drawControl(CE_CheckBoxLabel)

        if (option.direction == Qt::RightToLeft)
        {
            textRect.setRight(textRect.right() - option.iconSize.width() - 4);
        }
        else
        {
            textRect.setLeft(textRect.left() + option.iconSize.width() + 4);
        }
    }

    //

    // As in QCommonStyle::subElementRect(QStyle::SE_CheckBoxFocusRect)

    if (option.icon.isNull() && option.text.isEmpty())
    {
        focusRect = q->style()->subElementRect(QStyle::SE_CheckBoxIndicator, &option, q);
        focusRect.adjust(1, 1, -1, -1);
    }
    else
    {
        QRect iconFocusRect;
        QRect textFocusRect;

        if (!option.icon.isNull())
        {
            iconFocusRect = iconRect;
        }

        if (!option.text.isEmpty())
        {
            textFocusRect = q->style()->itemTextRect(option.fontMetrics, textRect, textFlags(), option.state & QStyle::State_Enabled, option.text);
        }

        focusRect = iconFocusRect | textFocusRect;
        focusRect.adjust(-3, -2, 3, 2);
        focusRect = focusRect.intersected(option.rect);
    }
}

QSize MultilineCheckBoxPrivate::sizeForWidth(int width) const
{
    Q_Q(const MultilineCheckBox);

    QStyleOptionButton option;
    q->initStyleOption(&option);

    // First of all, clamp the width to is minimium possible amount if it is set.

    if (q->minimumWidth() > 0)
    {
        width = qMax(width, q->minimumWidth());
    }

    // Then we need to account for various fixed margins. The first one would be contents margin.

    QSize contentsMarginSize(q->contentsMargins().left() + q->contentsMargins().right(),
                             q->contentsMargins().top() + q->contentsMargins().bottom());

    // Then there is ths icon...

    QSize iconSize = option.iconSize;
    QSize iconMarginSize = QSize(option.iconSize.width() + 4, 0);

    // And we need to reserve the space for check box indicator. Calculate its size as implemented
    // in QCommonStyle::sizeFromContents(QStyle::CT_CheckBox) along with magic numbers from there.

    QSize indicatorSize;
    QSize indicatorMarginSize;

    {
        int w = q->style()->pixelMetric(QStyle::PM_IndicatorWidth, &option, q);
        int h = q->style()->pixelMetric(QStyle::PM_IndicatorHeight, &option, q);

        int margins = 0;

        if (!option.icon.isNull() || !option.text.isEmpty())
        {
            margins = 4 + q->style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &option, q);
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
        width = q->maximumWidth();
    }

    width -= iconMarginSize.width();
    width -= contentsMarginSize.width();
    width -= indicatorMarginSize.width();
    width += 4;

    unsigned flags = textFlags();

    QFontMetrics fontMetrics = q->fontMetrics();

    QRect textRect = fontMetrics.boundingRect(0, 0, width, 2000, flags, option.text);

    if (tryWidth)
    {
        if ((textRect.height() < 4 * fontMetrics.lineSpacing()) && (textRect.width() > width / 2))
        {
            textRect = fontMetrics.boundingRect(0, 0, width / 2, 2000, flags, option.text);
        }

        if ((textRect.height() < 2 * fontMetrics.lineSpacing()) && (textRect.width() > width / 4))
        {
            textRect = fontMetrics.boundingRect(0, 0, width / 4, 2000, flags, option.text);
        }
    }

    // Add up the sizes and make sure that the result fits into minimum size
    // and can hold at least the indicator.

    return (textRect.size() + contentsMarginSize + indicatorMarginSize + iconMarginSize)
            .expandedTo(q->minimumSize())
            .expandedTo(indicatorSize)
            .expandedTo(iconSize);
}
