/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright (c) 2011, Robin Burchell.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDEPEOPLEMODEL_P_H
#define SEASIDEPEOPLEMODEL_P_H

#include <QObject>
#include <QVector>
#include <QStringList>
#include <QSettings>
#include <QContactGuid>

#include "seasidepeoplemodel.h"
#include "localeutils_p.h"

class SeasidePeopleModelPriv : public QObject
{
    Q_OBJECT
public:

    QContactManager *manager;
    QContactFetchHint currentFetchHint;
    QList<QContactSortOrder> sortOrder;
    QContactFilter currentFilter;
    QList<QContactLocalId> contactIds;
    QMap<QContactLocalId, int> idToIndex;
    QMap<QContactLocalId, QContact> idToContact;
    QMap<QUuid, QContactLocalId> uuidToId;
    QMap<QContactLocalId, QUuid> idToUuid;

    QVector<QStringList> data;
    QStringList headers;
    QSettings *settings;
    LocaleUtils *localeHelper;
    QContactGuid currentGuid;

    explicit SeasidePeopleModelPriv(SeasidePeopleModel* /*parent*/){}

    virtual ~SeasidePeopleModelPriv()
    {
        delete manager;
        delete settings;
    }

    QList<QContact> contactsPendingSave;

private:
    Q_DISABLE_COPY(SeasidePeopleModelPriv);
};

#endif // SEASIDEPEOPLEMODEL_P_H
