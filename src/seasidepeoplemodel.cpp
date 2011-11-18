/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDebug>

#include <QVector>
#include <QContactAddress>
#include <QContactAvatar>
#include <QContactThumbnail>
#include <QContactBirthday>
#include <QContactEmailAddress>
#include <QContactGuid>
#include <QContactName>
#include <QContactNickname>
#include <QContactNote>
#include <QContactOrganization>
#include <QContactOnlineAccount>
#include <QContactUnionFilter>
#include <QContactFavorite>
#include <QContactPhoneNumber>
#include <QContactUrl>
#include <QContactNote>
#include <QContactPresence>
#include <QSettings>
#include <QContactDetailFilter>
#include <QContactLocalIdFilter>
#include <QContactManagerEngine>
#include <QFile>

#include "seasideperson.h"
#include "seasidepeoplemodel.h"
#include "seasidepeoplemodel_p.h"

SeasidePeopleModel::SeasidePeopleModel(QObject *parent)
    : QAbstractListModel(parent)
    , priv(new SeasidePeopleModelPriv(this))
{
    QHash<int, QByteArray> roles;
    roles.insert(PersonRole, "person");
    setRoleNames(roles);
}

SeasidePeopleModel::~SeasidePeopleModel()
{
}

int SeasidePeopleModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return priv->contactIds.size();
}

bool SeasidePeopleModel::savePerson(SeasidePerson *person)
{
    QContact contact = person->contact();

    priv->contactsPendingSave.append(contact);

    if (contact.localId()) {
        // we save the contact to our model as well; if it existed previously.
        // this covers our QContactManager being slow at informing us about saves
        // with the slight problem that our data may be a little inconsistent if
        // the QContactManager decides to save differently from what we asked
        // it to - but this is ok, because the save request finishing will fix that.
        int rowId = priv->idToIndex.value(contact.localId());
        emit dataChanged(index(rowId, 0), index(rowId, 0));
        qDebug() << Q_FUNC_INFO << "Faked save for " << contact.localId() << " row " << rowId;
    }

    // TODO: in a more complicated implementation, we'd only save
    // on a timer instead of flushing all the time
    priv->savePendingContacts();

    return true;
}

SeasidePerson *SeasidePeopleModel::person(int row) const
{
    if (row < 0 || row >= priv->contactIds.count())
        return 0;

   QContactLocalId id = priv->contactIds[row];
   return priv->idToContact[id];
}

void SeasidePeopleModel::deletePerson(SeasidePerson *person)
{
    // TODO: fake remove the contact, so it appears to happen faster
    QContactRemoveRequest *removeRequest = new QContactRemoveRequest(this);
    removeRequest->setManager(priv->manager);
    connect(removeRequest,
            SIGNAL(stateChanged(QContactAbstractRequest::State)),
            priv, SLOT(onRemoveStateChanged(QContactAbstractRequest::State)));
    removeRequest->setContactId(person->contact().id().localId());
    qDebug() << Q_FUNC_INFO << "Removing " << person->contact().id().localId();

    if (!removeRequest->start()) {
        qWarning() << Q_FUNC_INFO << "Remove request failed";
        delete removeRequest;
    }
}

QVariant SeasidePeopleModel::data(const QModelIndex& index, int role) const
{
    SeasidePerson *aperson = person(index.row());
    if (!aperson)
        return QVariant();

    if (role == PersonRole)
        return QVariant::fromValue<SeasidePerson *>(aperson);
    else
        return QVariant();
}

