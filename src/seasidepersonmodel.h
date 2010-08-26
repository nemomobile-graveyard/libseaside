/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDEPERSONMODEL_H
#define SEASIDEPERSONMODEL_H

#include <MWidgetModel>

#include "seaside.h"

class SeasidePersonModel: public MWidgetModel
{
    M_MODEL(SeasidePersonModel)

public:
    bool isEmpty();

    void setPhone(int index, const SeasideDetail& detail);
    void setEmail(int index, const SeasideDetail& detail);
    void setIMAccount(int index, const SeasideDetail& detail);
    void setAddress(int index, const SeasideDetail& detail);

    M_MODEL_PROPERTY(QUuid, uuid, Uuid, true, QUuid());
    M_MODEL_PROPERTY(QString, firstname, FirstName, true, QString())
    M_MODEL_PROPERTY(QString, lastname, LastName, true, QString())
    M_MODEL_PROPERTY(QString, company, Company, true, QString())
    M_MODEL_PROPERTY(QString, thumbnail, Thumbnail, true, QString())
    M_MODEL_PROPERTY(Seaside::Presence, presence, Presence, true, Seaside::PresenceUnknown)
    M_MODEL_PROPERTY(bool, favorite, Favorite, true, false)

    M_MODEL_PROPERTY(QList<SeasideCommEvent>, events, Events, true, QList<SeasideCommEvent>())
    M_MODEL_PROPERTY(QVector<SeasideDetail>, phones, Phones, true, QVector<SeasideDetail>())
    M_MODEL_PROPERTY(QVector<SeasideDetail>, ims, IMs, true, QVector<SeasideDetail>())
    M_MODEL_PROPERTY(QVector<SeasideDetail>, emails, Emails, true, QVector<SeasideDetail>())
    M_MODEL_PROPERTY(QVector<SeasideDetail>, addresses, Addresses, true, QVector<SeasideDetail>())
};

#endif // SEASIDEPERSONMODEL_H
