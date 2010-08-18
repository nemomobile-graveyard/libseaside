/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QDebug>

#include <MLayout>
#include <MLinearLayoutPolicy>

#include <MImageWidget>
#include <MLabel>

#include "imagelabel.h"

ImageLabel::ImageLabel(MWidget *parent): MStylableWidget(parent)
{
    MLayout *layout = new MLayout();
    layout->setAnimation(NULL);
    m_policy = new MLinearLayoutPolicy(layout, Qt::Horizontal);
    setLayout(layout);

    m_image = NULL;
    m_label = new MLabel;
    m_label->setTextElide(true);
    m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_policy->addItem(m_label);
}

ImageLabel::~ImageLabel()
{
}

void ImageLabel::setText(const QString &text)
{
    m_text = text;
}

void ImageLabel::applyStyle()
{
    if (!m_image && !style()->image().isNull()) {
        m_image = new MImageWidget();
        m_image->setImage(style()->image(), style()->imageSize());
        m_policy->insertItem(0, m_image);
    }
    m_label->setFont(style()->font());
    m_label->setColor(style()->color());
    m_label->setText(m_text);

    m_policy->setItemSpacing(0, style()->marginInternal());

    // TODO: MStylableWidget should probably do this
    setContentsMargins(style()->marginLeft() + style()->paddingLeft(),
        style()->marginTop() + style()->paddingTop(),
        style()->marginRight() + style()->paddingRight(),
        style()->marginBottom() + style()->paddingBottom());
}
