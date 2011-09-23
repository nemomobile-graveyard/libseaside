/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright (c) 2010, Intel Corporation.
 * Copyright (c) 2011, Robin Burchell.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

// Qt
#include <QDebug>
#include <QSettings>

// Mobility
#include <QContactLocalIdFilter>
#include <QContactGuid>
#include <QContactAvatar>
#include <QContactName>

// Us
#include "seaside.h"
#include "seasidedetail.h"
#include "seasidesyncmodel_p.h"

static void updateDefinitions(QContactManager *manager)
{
    QContactDetailDefinition seaside;

    QContactDetailFieldDefinition commTimestamp;
    commTimestamp.setDataType(QVariant::DateTime);
    seaside.insertField(SeasideCustomDetail::FieldCommTimestamp, commTimestamp);

    QContactDetailFieldDefinition commType;
    commType.setDataType(QVariant::Int);
    seaside.insertField(SeasideCustomDetail::FieldCommType, commType);

    QContactDetailFieldDefinition commLocation;
    commLocation.setDataType(QVariant::Int);
    seaside.insertField(SeasideCustomDetail::FieldCommLocation, commLocation);

    seaside.setName(SeasideCustomDetail::DefinitionName);
    seaside.setUnique(true);

    //TODO: This is a synchronous call that needs to be async.
    //However, tracker doesn't support saving details via async calls currently.
    if (!manager->saveDetailDefinition(seaside))
      qWarning() << Q_FUNC_INFO << "failed to save new detail definition";
}

SeasideSyncModelPriv::SeasideSyncModelPriv(SeasideSyncModel *parent)
    : QObject(parent)
    , q(parent)
{
    headers.append("First Name");
    headers.append("Last Name");
    headers.append("Company");
    headers.append("Birthday");
    headers.append("Anniversary");
    headers.append("Avatar");
    headers.append("Favorite");
    headers.append("isSelf");
    headers.append("IM Accounts");
    headers.append("Email Addresses");
    headers.append("Phone Numbers");
    headers.append("Phone Contexts");
    headers.append("Phone Types");
    headers.append("Addresses");
    headers.append("Address Contexts");
    headers.append("Presence");
    headers.append("Uuid");
    headers.append("CommTimestamp");
    headers.append("CommType");
    headers.append("CommLocation");

    if (SeasideSyncModelPriv::theEngine == "default") {
        qDebug() << Q_FUNC_INFO << "Engine is default";
        qDebug() << Q_FUNC_INFO << QContactManager::availableManagers();

        if (QContactManager::availableManagers().contains("tracker")) {
            manager = new QContactManager("tracker");
        } else if (QContactManager::availableManagers().contains("memory")) {
            manager = new QContactManager("memory");

            // this caused me a lot of headaches; let's stop it doing the same to others.
            qWarning() << Q_FUNC_INFO << "Only recognised tracker engine available is 'memory'";
            qWarning() << Q_FUNC_INFO << "Changes to contacts WILL NOT be persistent!";
        } else {
            manager = new QContactManager("");
        }

        qDebug() << Q_FUNC_INFO << "Manager is " << manager->managerName();
    } else {
        manager = new QContactManager(SeasideSyncModelPriv::theEngine);
    }

    settings = new QSettings("MeeGo", "libseaside");

    updateDefinitions(manager);

    if (manager->hasFeature(QContactManager::SelfContact, QContactType::TypeContact)) {
        // self contact supported by manager - let's try fetch the me card
        QContactManager::Error error(QContactManager::NoError);
        const QContactLocalId meCardId(manager->selfContactId());

        //if we have a valid selfId
        if ((error == QContactManager::NoError) && (meCardId != 0)) {
            qDebug() << Q_FUNC_INFO << "valid selfId, error" << error << "id " << meCardId;
            //check if contact with selfId exists
            QContactLocalIdFilter idListFilter;
            idListFilter.setIds(QList<QContactLocalId>() << meCardId);

            QContactFetchRequest *meFetchRequest = new QContactFetchRequest(this);
            connect(meFetchRequest,
                    SIGNAL(stateChanged(QContactAbstractRequest::State)),
                    SLOT(onMeFetchRequestStateChanged(QContactAbstractRequest::State)));
            meFetchRequest->setFilter(idListFilter);
            meFetchRequest->setManager(manager);
            meFetchRequest->start();
        } else {
            qWarning() << Q_FUNC_INFO << "no valid meCard Id provided";
        }
    } else {
        qWarning() << Q_FUNC_INFO << "MeCard Not supported";
    }

    connect(manager, SIGNAL(contactsAdded(QList<QContactLocalId>)),
            this, SLOT(contactsAdded(QList<QContactLocalId>)));
    connect(manager, SIGNAL(contactsChanged(QList<QContactLocalId>)),
            this, SLOT(contactsChanged(QList<QContactLocalId>)));
    connect(manager, SIGNAL(contactsRemoved(QList<QContactLocalId>)),
            this, SLOT(contactsRemoved(QList<QContactLocalId>)));
    connect(manager, SIGNAL(dataChanged()), this, SLOT(dataReset()));

    dataReset();
}

SeasideSyncModelPriv::~SeasideSyncModelPriv()
{
    delete manager;
}

void SeasideSyncModelPriv::queueContactSave(QContact contactToSave)
{
    contactsPendingSave.append(contactToSave);

    if (contactToSave.localId()) {
        // we save the contact to our model as well; if it existed previously.
        // this covers our QContactManager being slow at informing us about saves
        // with the slight problem that our data may be a little inconsistent if
        // the QContactManager decides to save differently from what we asked
        // it to - but this is ok, because the save request finishing will fix that.
        int rowId = idToIndex.value(contactToSave.localId());
        qDebug() << Q_FUNC_INFO << "Faked save for " << contactToSave.localId() << " row " << rowId;
        idToContact[contactToSave.localId()] = contactToSave;
        emit q->dataChanged(q->index(rowId, 0), q->index(rowId, Seaside::ColumnLast));
    }

    // TODO: in a more complicated implementation, we'd only save
    // on a timer instead of flushing all the time
    savePendingContacts();
}

// helper function to check validity of sender and stuff.
template<typename T> inline T *checkRequest(QObject *sender, QContactAbstractRequest::State requestState)
{
    qDebug() << Q_FUNC_INFO << "Request state: " << requestState;
    T *request = qobject_cast<T *>(sender);
    if (!request) {
        qWarning() << Q_FUNC_INFO << "NULL request pointer";
        return 0;
    }

    if (request->error() != QContactManager::NoError) {
        qDebug() << Q_FUNC_INFO << "Error" << request->error()
                 << "occurred during request!";
        request->deleteLater();
        return 0;
    }

    if (requestState != QContactAbstractRequest::FinishedState &&
        requestState != QContactAbstractRequest::CanceledState)
    {
        // ignore
        return 0;
    }

    return request;
}

void SeasideSyncModelPriv::savePendingContacts()
{
    QContactSaveRequest *saveRequest = new QContactSaveRequest(this);
    connect(saveRequest,
            SIGNAL(stateChanged(QContactAbstractRequest::State)),
            SLOT(onSaveStateChanged(QContactAbstractRequest::State)));
    saveRequest->setContacts(contactsPendingSave);
    saveRequest->setManager(manager);

    foreach (const QContact &contact, contactsPendingSave)
        qDebug() << Q_FUNC_INFO << "Saving " << contact.id();

    if (!saveRequest->start()) {
        qWarning() << Q_FUNC_INFO << "Save request failed: " << saveRequest->error();
        delete saveRequest;
    }

    contactsPendingSave.clear();
}

void SeasideSyncModelPriv::onSaveStateChanged(QContactAbstractRequest::State requestState)
{
    QContactSaveRequest *saveRequest = checkRequest<QContactSaveRequest>(sender(), requestState);
    if (!saveRequest)
        return;

    QList<QContact> contactList = saveRequest->contacts();

    foreach (const QContact &new_contact, contactList) {
        qDebug() << Q_FUNC_INFO << "Successfully saved " << new_contact.id();

        // make sure data shown to user matches what is
        // really in the database
        QContactLocalId id = new_contact.localId();
        idToContact[id] = new_contact;
    }

    saveRequest->deleteLater();
}

void SeasideSyncModelPriv::removeContact(QContactLocalId contactId)
{
    QContactRemoveRequest *removeRequest = new QContactRemoveRequest(this);
    removeRequest->setManager(manager);
    connect(removeRequest,
            SIGNAL(stateChanged(QContactAbstractRequest::State)),
            SLOT(onRemoveStateChanged(QContactAbstractRequest::State)));
    removeRequest->setContactId(contactId);
    qDebug() << Q_FUNC_INFO << "Removing " << contactId;

    if (!removeRequest->start()) {
        qWarning() << Q_FUNC_INFO << "Remove request failed";
        delete removeRequest;
    }
}

void SeasideSyncModelPriv::onRemoveStateChanged(QContactAbstractRequest::State requestState)
{
    QContactRemoveRequest *removeRequest = checkRequest<QContactRemoveRequest>(sender(), requestState);
    if (!removeRequest)
        return;

    qDebug() << Q_FUNC_INFO << "Removed" << removeRequest->contactIds();
    removeRequest->deleteLater();
}

void SeasideSyncModelPriv::dataReset()
{
    qDebug() << Q_FUNC_INFO << "data reset";
    QContactFetchRequest *fetchRequest = new QContactFetchRequest(this);
    fetchRequest->setManager(manager);
    connect(fetchRequest,
            SIGNAL(stateChanged(QContactAbstractRequest::State)),
            SLOT(onDataResetFetchChanged(QContactAbstractRequest::State)));
    fetchRequest->setFilter(currentFilter);

    if (!fetchRequest->start()) {
        qWarning() << Q_FUNC_INFO << "Fetch request failed";
        delete fetchRequest;
        return;
    }
}

void SeasideSyncModelPriv::onDataResetFetchChanged(QContactAbstractRequest::State requestState)
{
    QContactFetchRequest *fetchRequest = checkRequest<QContactFetchRequest>(sender(), requestState);
    if (!fetchRequest)
        return;

    QList<QContact> contactsList = fetchRequest->contacts();
    int size = 0;

    qDebug() << Q_FUNC_INFO << "Starting model reset";
    q->beginResetModel();

    contactIds.clear();
    idToContact.clear();
    idToIndex.clear();
    uuidToId.clear();
    idToUuid.clear();

    q->addContacts(contactsList, size);

    q->endResetModel();
    qDebug() << Q_FUNC_INFO << "Done with model reset";
    fetchRequest->deleteLater();
}

void SeasideSyncModelPriv::contactsAdded(const QList<QContactLocalId>& contactIds)
{
    if (contactIds.size() == 0)
        return;

    QContactLocalIdFilter filter;
    filter.setIds(contactIds);

    QContactFetchRequest *fetchRequest = new QContactFetchRequest(this);
    fetchRequest->setManager(manager);
    connect(fetchRequest,
            SIGNAL(stateChanged(QContactAbstractRequest::State)),
            SLOT(onAddedFetchChanged(QContactAbstractRequest::State)));
    fetchRequest->setFilter(filter);
    qDebug() << Q_FUNC_INFO << "Fetching new contacts " << contactIds;

    if (!fetchRequest->start()) {
        qWarning() << Q_FUNC_INFO << "Fetch request failed";
        delete fetchRequest;
        return;
    }
}

void SeasideSyncModelPriv::onAddedFetchChanged(QContactAbstractRequest::State requestState)
{
    QContactFetchRequest *fetchRequest = checkRequest<QContactFetchRequest>(sender(), requestState);
    if (!fetchRequest)
        return;

    QList<QContact> addedContactsList = fetchRequest->contacts();

    int size = contactIds.size();
    int added = addedContactsList.size();

    q->beginInsertRows(QModelIndex(), size, size + added - 1);
    q->addContacts(addedContactsList, size);
    q->endInsertRows();

    qDebug() << Q_FUNC_INFO << "Done updating model after adding"
        << added << "contacts";
    fetchRequest->deleteLater();
}

void SeasideSyncModelPriv::contactsRemoved(const QList<QContactLocalId>& contactIds)
{
    qDebug() << Q_FUNC_INFO << "contacts removed:" << contactIds;

    QList<int> removed;
    foreach (const QContactLocalId& id, contactIds)
        removed.push_front(idToIndex.value(id));
    qSort(removed);

    // NOTE: this could check for adjacent rows being removed and send fewer signals
    int size = removed.size();
    for (int i=0; i<size; i++) {
        // remove in reverse order so the other index numbers will not change
        int index = removed.takeLast();
        q->beginRemoveRows(QModelIndex(), index, index);
        QContactLocalId id = this->contactIds.takeAt(index);

        idToContact.remove(id);
        idToIndex.remove(id);

        QUuid uuid = idToUuid[id];
        if (!uuid.isNull()) {
            idToUuid.remove(id);
            uuidToId.remove(uuid);
        }
        q->endRemoveRows();
    }
    q->fixIndexMap();
}

void SeasideSyncModelPriv::contactsChanged(const QList<QContactLocalId>& contactIds)
{
    if (contactIds.size() == 0)
        return;

    QContactLocalIdFilter filter;
    filter.setIds(contactIds);

    QContactFetchRequest *fetchRequest = new QContactFetchRequest(this);
    fetchRequest->setManager(manager);
    connect(fetchRequest,
            SIGNAL(stateChanged(QContactAbstractRequest::State)),
            SLOT(onChangedFetchChanged(QContactAbstractRequest::State)));
    fetchRequest->setFilter(filter);

    qDebug() << Q_FUNC_INFO << "Fetching changed contacts " << contactIds;

    if (!fetchRequest->start()) {
        qWarning() << Q_FUNC_INFO << "Fetch request failed";
        delete fetchRequest;
        return;
    }
}

void SeasideSyncModelPriv::onChangedFetchChanged(QContactAbstractRequest::State requestState)
{
    QContactFetchRequest *fetchRequest = checkRequest<QContactFetchRequest>(sender(), requestState);
    if (!fetchRequest)
        return;

    // NOTE: this implementation sends one dataChanged signal with
    // the minimal range that covers all the changed contacts, but it
    // could be more efficient to send multiple dataChanged signals,
    // though more work to find them
    int min = contactIds.size();
    int max = 0;

    QList<QContact> changedContactsList = fetchRequest->contacts();

    foreach (const QContact &changedContact, changedContactsList) {
        qDebug() << Q_FUNC_INFO << "Fetched changed contact " << changedContact.id();
        int index = idToIndex.value(changedContact.localId());

        if (index < min)
            min = index;

        if (index > max)
            max = index;

        // FIXME: this looks like it may be wrong,
        // could lead to multiple entries
        idToContact[changedContact.localId()] = changedContact;
    }

    // FIXME: unfortunate that we can't easily identify what changed
    if (min <= max)
        emit q->dataChanged(q->index(min, 0), q->index(max, Seaside::ColumnLast));

    qDebug() << Q_FUNC_INFO << "Done updating model after contacts update";
    fetchRequest->deleteLater();
}

// For Me card support
void SeasideSyncModelPriv::onMeFetchRequestStateChanged(QContactAbstractRequest::State requestState)
{
    QContactFetchRequest *fetchRequest = checkRequest<QContactFetchRequest>(sender(), requestState);
    if (!fetchRequest)
        return;

    // Check if we need to save Me contact again
    bool saveMe = false;

    if (fetchRequest->contacts().size() == 0) {
        qDebug() << Q_FUNC_INFO << "No Me contact, saving one";
        saveMe = true;
    } else {
        const QContactName &name = fetchRequest->contacts()[0].detail<QContactName>();
        if (name.firstName().isEmpty() || name.firstName().isNull()) {
            qDebug() << Q_FUNC_INFO << "Empty value for Me contact; saving again";
            saveMe = true;
        }
    }

    if (saveMe)
        createMeCard();

    fetchRequest->deleteLater();
}

// for Me card support
void SeasideSyncModelPriv::createMeCard()
{
  QContact contact;
  QContactId contactId;
  contactId.setLocalId(manager->selfContactId());

  qDebug() << Q_FUNC_INFO << "self contact does not exist, creating";
  contact.setId(contactId);

  QContactGuid guid;
  guid.setGuid(QUuid::createUuid().toString());
  if (!contact.saveDetail(&guid))
    qWarning() << Q_FUNC_INFO << "failed to save guid in mecard contact";

  QContactAvatar avatar;
  avatar.setImageUrl(QUrl("icon-m-content-avatar-placeholder"));
  if (!contact.saveDetail(&avatar))
      qWarning() << Q_FUNC_INFO << "failed to save avatar in mecard contact";

  // add the custom seaside detail
  SeasideCustomDetail sd;
  if (!contact.saveDetail(&sd))
    qWarning() << Q_FUNC_INFO << "failed to save seaside detail in mecard contact";

  QContactName name;
  name.setFirstName(QObject::tr("Me","Default string to describe self if no self contact information found, default created with [Me] as firstname"));
  name.setLastName("");
  if (!contact.saveDetail(&name))
    qWarning() << Q_FUNC_INFO << "failed to save mecard name";

  bool isSelf = true;
  if (settings) {
    QString key = guid.guid();
    key += "/self";
    settings->setValue(key, isSelf);
  }

  queueContactSave(contact);
}

