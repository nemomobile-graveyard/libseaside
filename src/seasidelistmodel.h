/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef SEASIDELISTMODEL_H
#define SEASIDELISTMODEL_H

#include <QAbstractTableModel>
#include <QUuid>
#include <QContactManager>
#include "seasidelist.h"

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
  //Column 6:     Favorite            bool
  Column 7:     Email Addresses     QStringList
  Column 8:     Phone Numbers       QStringList
  Column 9:     Phone Contexts      QStringList of "Home", "Work", or empty
  Column 10:    Phone Types         QStringList of "Mobile" or empty
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

using namespace QtMobility;

class SeasideListModelPriv;

class SeasideListModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    SeasideListModel(SeasideList::Detail detail = SeasideList::DetailNone);
    virtual ~SeasideListModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

protected:
    void fixIndexMap();
    void addContacts(const QList<QContactLocalId>& contactIds);

protected slots:
    void contactsAdded(const QList<QContactLocalId>& contactIds);
    void contactsChanged(const QList<QContactLocalId>& contactIds);
    void contactsRemoved(const QList<QContactLocalId>& contactIds);
    void dataReset();

private:
    SeasideListModelPriv *priv;
    Q_DISABLE_COPY(SeasideListModel);
};

#endif // SEASIDELISTMODEL_H
