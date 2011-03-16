/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SeasideListItemLarge_H
#define SeasideListItemLarge_H

#include <MWidgetController>
#include "seaside.h"
#include "seasidelistitembase.h"

/*
 * SeasideListItemLarge: Styling Notes
 *
 * The object name is set to SeasideListItemLarge by default; it contains a
 * MImageWidget thumbnail named SeasideListItemLargeThumbnail, and a SeasideLabel
 * named SeasideListItemLargeName and a SeasideLabel named SeasideListItemLargeData
 *
 * See seasidelabelstyle.h for the special styles that can be set on that
 *
 */

class SeasideListItemLarge : public SeasideListItemBase
{
    Q_OBJECT

public:
    SeasideListItemLarge(MWidget *parent = NULL);
    virtual ~SeasideListItemLarge();

private:
    Q_DISABLE_COPY(SeasideListItemLarge);
};

#endif // SeasideListItemLarge_H
