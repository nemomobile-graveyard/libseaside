/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef SEASIDEPROXYMODEL_H
#define SEASIDEPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "seasidepeoplemodel.h"

class SeasideProxyModelPriv;

class SeasideProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_ENUMS(FilterType)

public:
    SeasideProxyModel(QObject *parent = 0);
    virtual ~SeasideProxyModel();

    enum FilterType {
        FilterAll,
        FilterFavorites,
    };

    enum StringType {
        Primary,
        Secondary,
    };

    Q_INVOKABLE virtual void setFilter(FilterType filter);
    Q_INVOKABLE virtual void setSortType(SeasidePeopleModel::PeopleRoles sortType);
    Q_INVOKABLE virtual void setDisplayType(SeasidePeopleModel::PeopleRoles displayType);
    Q_INVOKABLE void setModel(SeasidePeopleModel *model);
    Q_INVOKABLE int getSourceRow(int row);

    // for fastscroll support
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    int count() const;
    Q_INVOKABLE QVariantMap get(int row);

signals:
    void countChanged();

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;

private slots:
    void readSettings();
    QString findString(int row, SeasidePeopleModel *model, SeasideProxyModel::StringType) const;

private:
    SeasideProxyModelPriv *priv;
    Q_DISABLE_COPY(SeasideProxyModel);
};

#endif // SEASIDEPROXYMODEL_H
