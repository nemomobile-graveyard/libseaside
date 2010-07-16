/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDESYNCMODEL_H
#define SEASIDESYNCMODEL_H

#include <QAbstractTableModel>
#include <QUuid>

#include <QContactManager>

class SeasidePersonModel;

/*
  Model data for Seaside::DataRole
  --------------------------------
  Column 0:     First Name          QString
  Column 1:     Last Name           QString
  Column 2:     Company             QString
  Column 3:     Birthday            QDate
  Column 4:     Anniversary         QDate
  Column 5:     Avatar              QString path/filename
  Column 6:     Favorite            bool
  Column 7:     Email Addresses     QStringList
  Column 8:     Phone Numbers       QStringList
  Column 9:     Phone Contexts      QStringList of "Home", "Work", or empty
  Column 10:     Phone Types         QStringList of "Mobile" or empty
  Column 11:    Addresses           QStringList*
  Column 12:    Address Contexts    QStringList of "Home", "Work", or empty
  Column 13:    Presence            int mapping to Seaside::Presence enum
  Column 14:    Uuid                QString

  // Second Class Citizens
  // - not clear if this data will really be provided here
  Column 15:    CommTimestamp       QDateTime
  Column 16:    CommType            int mapping to Seaside::CommType enum
  Column 17:    CommLocation        int mapping to Seaside::Location enum

  * Address strings currently consist of five lines separated by \n, containing
    i) street address, ii) city, iii) state (region), iv) postal code, v) country
*/

using namespace QtMobility;

class SeasideSyncModelPriv;

class SeasideSyncModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    SeasideSyncModel();
    virtual ~SeasideSyncModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QContact *contact(int row);
    QContactManager *manager();

    static SeasideSyncModel *instance();
    static void releaseInstance();

    static SeasidePersonModel *createPersonModel(const QModelIndex& index);
    SeasidePersonModel *createPersonModel(const QUuid& index);

    void deletePerson(const QUuid& uuid);
    void updatePerson(const SeasidePersonModel *newModel);

    void setAvatar(const QUuid& uuid, const QString& path);
    void setFavorite(const QUuid& uuid, bool favorite);

    // FIXME: temporary
    static void setGenerateSampleData(bool generate);
    static void setEngine(const QString& engine);

public slots:
    // FIXME: temporary debug
    void createFakeContact(const char *firstName, const char *lastName);
    void addRandom();
    void removeRandom();

protected:
    void fixIndexMap();
    void addContacts(const QList<QContactLocalId>& contactIds);

protected slots:
    void contactsAdded(const QList<QContactLocalId>& contactIds);
    void contactsChanged(const QList<QContactLocalId>& contactIds);
    void contactsRemoved(const QList<QContactLocalId>& contactIds);
    void dataReset();

private:
    SeasideSyncModelPriv *priv;
    Q_DISABLE_COPY(SeasideSyncModel);
};

#endif // SEASIDESYNCMODEL_H
