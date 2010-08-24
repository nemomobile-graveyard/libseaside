/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QDebug>

#include <QStringList>

#include "seasideproxymodel.h"
#include "seasidesyncmodel.h"
#include "seaside.h"

class SeasideProxyModelPriv
{
public:
    SeasideProxyModel::FilterType filterType;
    SeasideProxyModel::SortType sortType;
};

SeasideProxyModel::SeasideProxyModel()
{
    priv = new SeasideProxyModelPriv;
    priv->filterType = FilterAll;
    priv->sortType = SortName;
    setDynamicSortFilter(true);
    setFilterKeyColumn(-1);
}

SeasideProxyModel::~SeasideProxyModel()
{
    delete priv;
}

void SeasideProxyModel::setFilter(FilterType filter)
{
    priv->filterType = filter;
    invalidateFilter();
}

void SeasideProxyModel::setSortType(SortType sortType)
{
    priv->sortType = sortType;
    if (priv->sortType == SortRecent)
        sort(Seaside::ColumnCommTimestamp, Qt::DescendingOrder);
    else
        sort(Seaside::ColumnFirstName, Qt::AscendingOrder);
}

bool SeasideProxyModel::filterAcceptsRow(int source_row,
                                         const QModelIndex& source_parent) const
{
    // TODO: add communication history

    if (!QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent))
        return false;

    if (priv->filterType == FilterAll)
        return true;

    SeasideSyncModel *model = dynamic_cast<SeasideSyncModel *>(sourceModel());
    if (!model)
        return true;

    QContact *contact = model->contact(source_row);

    if (priv->filterType == FilterRecent) {
        QDateTime cutoff = QDateTime::currentDateTime();
        cutoff = cutoff.addDays(-30);
        const QDateTime& dt = Seaside::contactCommTimestamp(contact);
        return dt.isValid() && (dt > cutoff);
    }
    else if (priv->filterType == FilterFavorites) {
        SEASIDE_SHORTCUTS;
        SEASIDE_SET_MODEL_AND_ROW(model, source_row);
        return SEASIDE_FIELD(Favorite, Bool);
    }
    else {
        qWarning() << "[ProxyModel] invalid filter type";
        return true;
    }
}

bool SeasideProxyModel::lessThan(const QModelIndex& left,
                                 const QModelIndex& right) const
{
    if (priv->sortType == SortName) {
        const QString& lStr = left.data(Seaside::DataRole).toString();
        const QString& rStr = right.data(Seaside::DataRole).toString();
        //qWarning() << "[ProxyModel] lessThan " << lStr << "VS" << rStr;
        return QString::localeAwareCompare(lStr, rStr) < 0;
    }
    else if (priv->sortType == SortRecent) {
        const QDateTime& lDate = left.data(Seaside::DataRole).toDateTime();
        const QDateTime& rDate = right.data(Seaside::DataRole).toDateTime();
        return lDate < rDate;
    }
    return false;
}
