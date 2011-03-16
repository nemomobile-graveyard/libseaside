/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDELISTITEM_H
#define SEASIDELISTITEM_H

#include <MWidgetController>
#include "seaside.h"
#include "seasidelistitembase.h"

/*
 * SeasideListItem: Styling Notes
 *
 * The object name is set to SeasideListItem by default; it contains a
 * MImageWidget thumbnail named SeasideListItemThumbnail, and a SeasideLabel
 * named SeasideListItemName and a SeasideLabel named SeasideListItemData
 *
 * See seasidelabelstyle.h for the special styles that can be set on that
 *
 */

class SeasideListItem : public SeasideListItemBase
{
    Q_OBJECT

public:
    SeasideListItem(MWidget *parent = NULL, int mode =0);
    virtual ~SeasideListItem();

private:
    Q_DISABLE_COPY(SeasideListItem);
};

#endif // SEASIDELISTITEM_H
