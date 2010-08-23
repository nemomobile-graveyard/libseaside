/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QDebug>

#include <QGraphicsSceneMouseEvent>

#include <MLabel>

#include <MLayout>
#include <MLinearLayoutPolicy>

#include "seasidelabel.h"

class SeasideLabelPriv
{
public:
    MLabel *label;
    bool mouseEnabled;
};

SeasideLabel::SeasideLabel(const QString &text, QGraphicsItem *parent):
    MStylableWidget(parent)
{
    priv = new SeasideLabelPriv;
    priv->mouseEnabled = false;

    MLayout *layout = new MLayout(this);
    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Horizontal);

    priv->label = new MLabel(text);
    priv->label->setTextElide(true);
    policy->addItem(priv->label, Qt::AlignLeft);
}

SeasideLabel::~SeasideLabel()
{
    delete priv;
}

void SeasideLabel::setMouseEnabled(bool enable)
{
    priv->mouseEnabled = enable;
}

bool SeasideLabel::isMouseEnabled()
{
    return priv->mouseEnabled;
}

void SeasideLabel::setText(const QString &text)
{
    if(priv->label)
     priv->label->setText(text);
}

void SeasideLabel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    if (!priv->mouseEnabled)
        MStylableWidget::mousePressEvent(event);
    else
        priv->label->setColor(style()->pressedColor());
}

void SeasideLabel::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    if (!priv->mouseEnabled)
        MStylableWidget::mousePressEvent(event);
    else {
        priv->label->setColor(style()->color());
        if (sceneBoundingRect().contains(event->scenePos()))
            emit clicked(this);
    }
}

void SeasideLabel::cancelEvent(MCancelEvent *event)
{
    if (!priv->mouseEnabled)
        MStylableWidget::cancelEvent(event);
    else
        priv->label->setColor(style()->color());
}

static int getFontWeight(const QString& weight)
{
    int rval = QFont::Normal;
    if (weight == "bold")
        rval = QFont::Bold;
    else if (weight == "bolder")
        rval = QFont::Black;
    else if (weight == "lighter")
        rval = QFont::Light;
    return rval;
}

static Qt::Alignment getAlignment(const QString& textAlign, const QString& vertAlign)
{
    Qt::Alignment align = 0;
    if (textAlign == "left")
        align |= Qt::AlignLeft;
    else if (textAlign == "right")
        align |= Qt::AlignRight;
    else if (textAlign == "center")
        align |= Qt::AlignCenter;
    else if (textAlign == "justify")
        align |= Qt::AlignJustify;
    if (vertAlign == "top")
        align |= Qt::AlignTop;
    else if (vertAlign == "middle" || vertAlign == "center")
        align |= Qt::AlignVCenter;
    else if (vertAlign == "bottom")
        align |= Qt::AlignBottom;
    return align;
}

void SeasideLabel::applyStyle()
{
    priv->label->setColor(style()->color());

    QFont font = style()->font();
    font.setWeight(getFontWeight(style()->fontWeight()));
    priv->label->setFont(font);

    priv->label->setAlignment(getAlignment(style()->textAlign(), style()->verticalAlign()));
}
