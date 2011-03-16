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
#include <MButton>

#include "seasidelistitemsmall.h"
#include "seasidelistitem_p.h"
#include "seasidelabel.h"

#include <MWidgetCreator>
#include <QDebug>

M_REGISTER_WIDGET(SeasideListItemSmall)

#define SCREEN_WIDTH 400

SeasideListItemSmall::SeasideListItemSmall(MWidget *parent)
    : SeasideListItemBase(parent)
{
    setObjectName("SeasideListItemSmall");

    QGraphicsGridLayout *grid = new QGraphicsGridLayout;
   grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(0);
    setLayout(grid);

     priv->status = new SeasideLabel;
     priv->button = new MButton;
     priv->detail = new SeasideLabel;

    priv->image = new MImageWidget();
    priv->image->setObjectName("SeasideListItemSmallThumbnail");
    priv->image->setPreferredSize(64, 64);
    priv->image->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->image, 0, 0, 2, 2);

    priv->name = new SeasideLabel;
    priv->name->setObjectName("SeasideListItemSmallName");
    priv->name->setPreferredWidth(SCREEN_WIDTH);
    priv->name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    grid->addItem(priv->name, 0, 2, 1, 1);

    priv->favorite = new MImageWidget();
    priv->favorite->setObjectName("SeasideListItemSmallFavorite");
    priv->favorite->setPreferredSize(25, 25);
    priv->favorite->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->favorite, 1, 3, 1, 1);

    priv->status = new SeasideLabel;
    priv->status->setObjectName("SeasideListItemSmallStatus");
    priv->name->setPreferredWidth(SCREEN_WIDTH);
    priv->name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    grid->addItem(priv->status, 0, 4, 1, 3);

    priv->presence = new MImageWidget();
    priv->presence->setObjectName("SeasideListItemSmallPresence");
    priv->presence->setPreferredSize(25, 25);
    priv->presence->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->presence, 1, 2, 1, 1);

    priv->email = new MImageWidget();
    priv->email->setObjectName("SeasideListItemSmallEmail");
    priv->email->setPreferredSize(25, 25);
    priv->email->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->email, 1, 5, 1, 1);

    priv->sms = new MImageWidget();
    priv->sms->setObjectName("SeasideListItemSmallSMS");
    priv->sms->setPreferredSize(25, 25);
    priv->sms->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->sms, 1,4, 1, 1);

    priv->im = new MImageWidget();
    priv->im->setObjectName("SeasideListItemSmallIM");
    priv->im->setPreferredSize(25, 25);
    priv->im->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->im, 1, 3, 1, 1);

    priv->call = new MImageWidget();
    priv->call->setObjectName("SeasideListItemSmallCall");
    priv->call->setPreferredSize(25, 25);
    priv->call->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->call, 1, 6, 1, 1);
}

SeasideListItemSmall::~SeasideListItemSmall()
{
}

