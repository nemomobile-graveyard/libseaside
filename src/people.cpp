/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QModelIndex>
#include <QDebug>

#include <MApplication>

#include <MWidgetView>
#include <MWidgetModel>

#include <seasidesyncmodel.h>
#include <seasideproxymodel.h>

#include "people.h"

SeasidePeople::SeasidePeople(QGraphicsItem *parent):
        MWidgetController(new MWidgetModel, parent)
{
    m_realModel = SeasideSyncModel::instance();
    m_proxyModel = new SeasideProxyModel;
    m_proxyModel->sort(0);  // enable custom sorting
    m_proxyModel->setSourceModel(m_realModel);
}

SeasidePeople::~SeasidePeople()
{
    delete m_proxyModel;
    m_realModel->releaseInstance();
}

QAbstractItemModel *SeasidePeople::itemModel()
{
    return m_proxyModel;
}

void SeasidePeople::filterAll()
{
    if (m_proxyModel) {
        m_proxyModel->setFilter(SeasideProxyModel::FilterAll);
        m_proxyModel->setFilterRegExp("");
    }
}

void SeasidePeople::filterRecent()
{
    if (m_proxyModel) {
        m_proxyModel->setFilter(SeasideProxyModel::FilterRecent);
        m_proxyModel->setFilterRegExp("");
    }
}

void SeasidePeople::filterFavorites()
{
    if (m_proxyModel) {
        m_proxyModel->setFilter(SeasideProxyModel::FilterFavorites);
        m_proxyModel->setFilterRegExp("");
    }
}

void SeasidePeople::filterSearch(const QString& text)
{
    if (m_proxyModel) {
        m_proxyModel->setFilter(SeasideProxyModel::FilterAll);
        m_proxyModel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive,
                                              QRegExp::FixedString));
    }
}

void SeasidePeople::setFavorite(const QUuid& uuid, bool favorite)
{
    m_realModel->setFavorite(uuid, favorite);
}

void SeasidePeople::deletePerson(const QUuid& uuid)
{
    m_realModel->deletePerson(uuid);
}

void SeasidePeople::scrollTo(const QString &name)
{
    emit scrollRequest(name);
}

void SeasidePeople::scrollTo(qreal pos)
{
    emit scrollRequest(pos);
}
