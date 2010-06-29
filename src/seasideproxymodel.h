/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDEPROXYMODEL_H
#define SEASIDEPROXYMODEL_H

#include <QSortFilterProxyModel>

class SeasideProxyModelPriv;

class SeasideProxyModel: public QSortFilterProxyModel
{
public:
    SeasideProxyModel();
    virtual ~SeasideProxyModel();

    enum FilterType {
        FilterAll,
        FilterRecent,
        FilterFavorites,
    };

    enum SortType {
        SortName,
        SortRecent,
    };

    virtual void setFilter(FilterType filter);
    virtual void setSortType(SortType sortType);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;

private:
    SeasideProxyModelPriv *priv;
    Q_DISABLE_COPY(SeasideProxyModel);
};

#endif // SEASIDEPROXYMODEL_H
