/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <seasidepersonmodel.h>
#include <seasidesyncmodel.h>

#include "seasideperson.h"

SeasidePerson::SeasidePerson(const QModelIndex& index, MWidget *parent):
        MWidgetController(SeasideSyncModel::createPersonModel(index), parent)
{
    setObjectName("SeasidePerson");
}

SeasidePerson::~SeasidePerson()
{
}
