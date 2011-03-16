/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDELISTITEM_P_H
#define SEASIDELISTITEM_P_H

class QStringList;
class QUuid;

class MImageWidget;
class MButton;

class SeasideLabel;

#include "seasidelabel.h"

struct SeasideListItemPriv
{
    MImageWidget *image;
    SeasideLabel *name;
    SeasideLabel *detail;
    SeasideLabel *status;
    QStringList  *detailList;
    MImageWidget *favorite;
    MImageWidget *presence;
    MButton *button;
    MImageWidget *email;
    MImageWidget *sms;
    MImageWidget *im;
    MImageWidget *call;
    QUuid *uuid;
};

#endif // SEASIDELISTITEM_P_H
