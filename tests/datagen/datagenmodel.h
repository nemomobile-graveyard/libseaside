/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef DATAGENMODEL_H
#define DATAGENMODEL_H

#include <QAbstractTableModel>
#include <QUuid>

#include <QContactManager>
#include <QContactManagerEngine>

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
  Column 15:    CommTimestamp       QDateTime
  Column 16:    CommType            int mapping to Seaside::CommType enum
  Column 17:    CommLocation        int mapping to Seaside::Location enum

  * Address strings currently consist of five lines separated by \n, containing
    i) street address, ii) city, iii) state (region), iv) postal code, v) country
*/

typedef struct {
    const char *first;
    const char *last;
   const char *company;
    const char *birthday;
    const char *anniversary;
    const char *avatar;
    const char *favorite;
    const char *im;
    const char *email;
    const char *phone;
    const char *phonecontext;
    const char *phonetype;
    const char *addresses;
    const char *addresscontext;
    const char *presence;
    const char *uuid;
    const char *commtimestamp;
    const char *commtype;
    const char *commlocation;
} PeopleData;

using namespace QtMobility;

class DataGenModelPriv;

class DataGenModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    DataGenModel();
    virtual ~DataGenModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QContact *contact(int row);
    QContactManager *manager();
    QContactManagerEngine *engine();

    static DataGenModel *instance();
    static void releaseInstance();

    static SeasidePersonModel *createPersonModel(const QModelIndex& index);
    SeasidePersonModel *createPersonModel(const QUuid& index);
    QContactLocalId getSelfContactId();

    void deletePerson(const QUuid& uuid);
    void updatePerson(const SeasidePersonModel *newModel);

    void setAvatar(const QUuid& uuid, const QString& path);
    void setFavorite(const QUuid& uuid, bool favorite);
    void setCompany(const QUuid& uuid, QString company);

    static void setGenerateSampleData(bool generate);
    static void setEngine(const QString& engine);

 public slots:
    void createFakeContact(const PeopleData person);
    void createMeCard();

protected:
    void fixIndexMap();
    void addContacts(const QList<QContactLocalId>& contactIds);

protected slots:
    void contactsAdded(const QList<QContactLocalId>& contactIds);
    void contactsChanged(const QList<QContactLocalId>& contactIds);
    void contactsRemoved(const QList<QContactLocalId>& contactIds);
    void dataReset();

private:
    DataGenModelPriv *priv;
    Q_DISABLE_COPY(DataGenModel);
};

#endif // DATAGENMODEL_H
