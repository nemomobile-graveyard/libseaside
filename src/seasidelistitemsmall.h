/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SeasideListItemSmall_H
#define SeasideListItemSmall_H

#include <MWidgetController>
#include "seaside.h"
#include "seasidelistitembase.h"

/*
 * SeasideListItemSmall: Styling Notes
 *
 * The object name is set to SeasideListItemSmall by default; it contains a
 * MImageWidget thumbnail named SeasideListItemSmallThumbnail, and a SeasideLabel
 * named SeasideListItemSmallName and a SeasideLabel named SeasideListItemSmallData
 *
 * See seasidelabelstyle.h for the special styles that can be set on that
 *
 */

class SeasideListItemSmall : public SeasideListItemBase
{
    Q_OBJECT

public:
    SeasideListItemSmall(MWidget *parent = NULL);
    virtual ~SeasideListItemSmall();

private:
    Q_DISABLE_COPY(SeasideListItemSmall);
};

#endif // SeasideListItemSmall_H
