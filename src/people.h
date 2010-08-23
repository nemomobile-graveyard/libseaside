/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PEOPLE_H
#define PEOPLE_H

#include <MWidgetController>

class QAbstractItemModel;
class QModelIndex;
class QUuid;
class SeasideSyncModel;
class SeasideProxyModel;
class SeasidePersonModel;

class SeasidePeople: public MWidgetController
{
    Q_OBJECT

public:
    SeasidePeople(QGraphicsItem *parent = NULL);
    virtual ~SeasidePeople();

    void setItemModel(QAbstractItemModel *itemModel);
    QAbstractItemModel *itemModel();

signals:
    void itemClicked(const QModelIndex& index);
    void editRequest(const QModelIndex& index);
    void callNumber(const QString& number);
    void composeSMS(const QString& number);
    void composeEmail(const QString& address);
    void scrollRequest(const QString &name);
    void scrollRequest(qreal pos);

public slots:
    void filterAll();
    void filterRecent();
    void filterFavorites();
    void filterSearch(const QString& text);
    void setFavorite(const QUuid& uuid, bool favorite);
    void deletePerson(const QUuid& uuid);
    void scrollTo(const QString& name);
    void scrollTo(qreal pos);

private:
    SeasideSyncModel *m_realModel;
    SeasideProxyModel *m_proxyModel;
};

#endif // PEOPLE_H
