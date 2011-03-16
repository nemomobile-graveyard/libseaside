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

#include <MImageWidget>
#include <MLabel>
#include <MButton>

#include "seasidelistitem.h"
#include "seasidelistitem_p.h"
#include "seasidelabel.h"

#include <MWidgetCreator>

M_REGISTER_WIDGET(SeasideListItem);

#define SCREEN_WIDTH 400

SeasideListItem::SeasideListItem(MWidget *parent, int mode)
    : SeasideListItemBase(parent)
{
    setObjectName("SeasideListItem");

    if(mode == 0){ //large list item

    QGraphicsGridLayout *grid = new QGraphicsGridLayout;
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(0);
    setLayout(grid);

    priv->image = new MImageWidget();
    priv->image->setObjectName("SeasideListItemThumbnail");
    priv->image->setPreferredSize(64, 64);
    priv->image->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->image, 0, 0, 2, 2);

    priv->name = new SeasideLabel;
    priv->name->setObjectName("SeasideListItemName");
    priv->name->setPreferredWidth(SCREEN_WIDTH);
    priv->name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    grid->addItem(priv->name, 0, 2, 1, 1);

    priv->detail = new SeasideLabel;
    priv->detail->setObjectName("SeasideListItemDetail");
    priv->name->setPreferredWidth(SCREEN_WIDTH);
    priv->name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    grid->addItem(priv->detail, 1, 2, 1, 1);

    priv->status = new SeasideLabel;
    priv->status->setObjectName("SeasideListItemStatus");
    priv->name->setPreferredWidth(SCREEN_WIDTH);
    priv->name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    grid->addItem(priv->status, 2, 2, 1, 1);

    priv->favorite = new MImageWidget();
    priv->favorite->setObjectName("SeasideListItemFavorite");
    priv->favorite->setPreferredSize(25, 25);
    priv->favorite->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->favorite, 2, 1, 1, 1);

    priv->presence = new MImageWidget();
    priv->presence->setObjectName("SeasideListItemPresence");
    priv->presence->setPreferredSize(25, 25);
    priv->presence->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->presence, 2, 0, 1, 1);

    priv->button = new MButton();
    priv->button->setObjectName("SeasideListItemButton");
    priv->button->setPreferredSize(70, 70);
    priv->button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->button, 0, 4, 4, 4);

    QObject::connect(priv->button, SIGNAL(clicked()), this, SIGNAL(buttonClicked()));

    priv->email = new MImageWidget();
    priv->email->setObjectName("SeasideListItemEmail");
    priv->email->setPreferredSize(25, 25);
    priv->email->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->email, 2, 5, 1, 1);

    priv->sms = new MImageWidget();
    priv->sms->setObjectName("SeasideListItemSMS");
    priv->sms->setPreferredSize(25, 25);
    priv->sms->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->sms, 2,4, 1, 1);

    priv->im = new MImageWidget();
    priv->im->setObjectName("SeasideListItemIM");
    priv->im->setPreferredSize(25, 25);
    priv->im->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->im, 2, 3, 1, 1);

    priv->call = new MImageWidget();
    priv->call->setObjectName("SeasideListItemCall");
    priv->call->setPreferredSize(25, 25);
    priv->call->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->call, 2, 6, 1, 1);

}else { //small list item

    QGraphicsGridLayout *grid = new QGraphicsGridLayout;
   grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(0);
    setLayout(grid);

     priv->status = new SeasideLabel;
     priv->button = new MButton;
     priv->detail = new SeasideLabel;

    priv->image = new MImageWidget();
    priv->image->setObjectName("SeasideListItemThumbnail");
    priv->image->setPreferredSize(64, 64);
    priv->image->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->image, 0, 0, 2, 2);

    priv->name = new SeasideLabel;
    priv->name->setObjectName("SeasideListItemName");
    priv->name->setPreferredWidth(SCREEN_WIDTH);
    priv->name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    grid->addItem(priv->name, 0, 2, 1, 1);

    priv->favorite = new MImageWidget();
    priv->favorite->setObjectName("SeasideListItemFavorite");
    priv->favorite->setPreferredSize(25, 25);
    priv->favorite->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->favorite, 1, 3, 1, 1);

    priv->status = new SeasideLabel;
    priv->status->setObjectName("SeasideListItemStatus");
    priv->name->setPreferredWidth(SCREEN_WIDTH);
    priv->name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    grid->addItem(priv->status, 0, 4, 1, 3);

    priv->presence = new MImageWidget();
    priv->presence->setObjectName("SeasideListItemPresence");
    priv->presence->setPreferredSize(25, 25);
    priv->presence->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->presence, 1, 2, 1, 1);

    priv->email = new MImageWidget();
    priv->email->setObjectName("SeasideListItemEmail");
    priv->email->setPreferredSize(25, 25);
    priv->email->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->email, 1, 5, 1, 1);

    priv->sms = new MImageWidget();
    priv->sms->setObjectName("SeasideListItemSMS");
    priv->sms->setPreferredSize(25, 25);
    priv->sms->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->sms, 1,4, 1, 1);

    priv->im = new MImageWidget();
    priv->im->setObjectName("SeasideListItemIM");
    priv->im->setPreferredSize(25, 25);
    priv->im->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->im, 1, 3, 1, 1);

    priv->call = new MImageWidget();
    priv->call->setObjectName("SeasideListItemCall");
    priv->call->setPreferredSize(25, 25);
    priv->call->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->call, 1, 6, 1, 1);
}
}

SeasideListItem::~SeasideListItem()
{
}

