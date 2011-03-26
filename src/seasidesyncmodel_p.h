/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright (c) 2011, Robin Burchell.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDESYNCMODEL_P_H
#define SEASIDESYNCMODEL_P_H

// Qt
#include <QObject>
#include <QVector>
#include <QStringList>
#include <QSettings>

// Us
#include "seasidesyncmodel.h"

class SeasideSyncModelPriv : public QObject
{
    Q_OBJECT
public:
    static SeasideSyncModel *theSyncModel;
    static int theRefCount;
    static QString theEngine;

    QContactManager *manager;
    QContactFilter currentFilter;
    QList<QContactLocalId> contactIds;
    QMap<QContactLocalId, int> idToIndex;
    QMap<QContactLocalId, QContact> idToContact;
    QMap<QUuid, QContactLocalId> uuidToId;
    QMap<QContactLocalId, QUuid> idToUuid;

    QVector<QStringList> data;
    QStringList headers;
    QSettings *settings;

    explicit SeasideSyncModelPriv(SeasideSyncModel *parent);
    virtual ~SeasideSyncModelPriv();

    /*! Queues a \a contact for asynchronous saving after calls
     * to QContact::saveDetail(), etc.
     */
    void queueContactSave(QContact contact);

    /*! Removes a given \a contactId asynchronously.
     */
    void removeContact(QContactLocalId contactId);

private slots:
    void onSaveStateChanged(QContactAbstractRequest::State requestState);
    void onRemoveStateChanged(QContactAbstractRequest::State requestState);
    void onDataResetFetchChanged(QContactAbstractRequest::State requestState);
    void onAddedFetchChanged(QContactAbstractRequest::State requestState);
    void onChangedFetchChanged(QContactAbstractRequest::State requestState);
    void onMeFetchRequestStateChanged(QContactAbstractRequest::State requestState);

    void contactsAdded(const QList<QContactLocalId>& contactIds);
    void contactsChanged(const QList<QContactLocalId>& contactIds);
    void contactsRemoved(const QList<QContactLocalId>& contactIds);
    void dataReset();

private:
    QList<QContact> contactsPendingSave;

    void savePendingContacts();
    void createMeCard();

    SeasideSyncModel *q;

private:
    friend class SeasideSyncModel;
    Q_DISABLE_COPY(SeasideSyncModelPriv);
};

#endif // SEASIDESYNCMODEL_P_H
