/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QGraphicsGridLayout>
#include <QGraphicsSceneMouseEvent>
#include <QUuid>

#include <MImageWidget>
#include <MLabel>

#include "seasidelistitem.h"
#include "seasidelabel.h"

#include <MWidgetCreator>
M_REGISTER_WIDGET(SeasideListItem)

class SeasideListItemPriv
{
public:
    MImageWidget *image;
    SeasideLabel *name;
    QUuid uuid;
};

SeasideListItem::SeasideListItem(MWidget *parent): MWidgetController(parent)
{
    priv = new SeasideListItemPriv;

    setObjectName("SeasideListItem");

    QGraphicsGridLayout *grid = new QGraphicsGridLayout;
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(0);
    setLayout(grid);

    priv->image = new MImageWidget("empty");
    priv->image->setObjectName("SeasideListItemThumbnail");
    priv->image->setPreferredSize(64, 64);
    priv->image->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->image, 0, 0);

    priv->name = new SeasideLabel;
    priv->name->setObjectName("SeasideListItemName");
    grid->addItem(priv->name, 0, 1);
}

SeasideListItem::~SeasideListItem()
{
    delete priv;
}

void SeasideListItem::setThumbnail(const QString &thumbnail)
{
    QString str = thumbnail;
    if (str.isEmpty())
        str= "default-thumbnail";
    priv->image->setImage(str);
}

void SeasideListItem::setName(const QString &name)
{
    priv->name->setText(name);
}

void SeasideListItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void SeasideListItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    if (sceneBoundingRect().contains(event->scenePos()))
        emit clicked();
}
