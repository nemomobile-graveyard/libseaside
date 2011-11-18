/*
 * Copyright 2011 Intel Corporation.
 * Copyright 2011 Robin Burchell
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDebug>

#include <QStringList>
#include <QFileSystemWatcher>

#include "seasideperson.h"
#include "seasideproxymodel.h"
#include "settingsdatastore_p.h"
#include "localeutils_p.h"

class SeasideProxyModelPriv
{
public:
    SeasideProxyModel::FilterType filterType;
    SettingsDataStore *settings;
    LocaleUtils *localeHelper;
    QFileSystemWatcher *settingsFileWatcher;
};

SeasideProxyModel::SeasideProxyModel(QObject *parent)
{
    Q_UNUSED(parent);
    priv = new SeasideProxyModelPriv;
    priv->filterType = FilterAll;
    priv->settings = SettingsDataStore::self();
    priv->localeHelper = LocaleUtils::self();
    setDynamicSortFilter(true);
    setFilterKeyColumn(-1);

    priv->settingsFileWatcher = new QFileSystemWatcher(this);
    priv->settingsFileWatcher->addPath(priv->settings->getSettingsStoreFileName());
    connect(priv->settingsFileWatcher, SIGNAL(fileChanged(QString)),
            this, SLOT(readSettings()));

    readSettings();
}

SeasideProxyModel::~SeasideProxyModel()
{
    delete priv;
}

void SeasideProxyModel::readSettings() 
{
    priv->settings->syncDataStore();

    priv->settings->getDisplayOrder();
}

void SeasideProxyModel::setFilter(FilterType filter)
{
    priv->filterType = filter;
    invalidateFilter();
}

void SeasideProxyModel::setModel(SeasidePeopleModel *model)
{
    setSourceModel(model);
    readSettings();
}

int SeasideProxyModel::getSourceRow(int row)
{
    return mapToSource(index(row, 0)).row();
}

bool SeasideProxyModel::filterAcceptsRow(int source_row,
                                  const QModelIndex& source_parent) const
{
    // TODO: add communication history
    //if (!QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent))
    //    return false;

    if (priv->filterType == FilterAll)
        return true;

    SeasidePeopleModel *model = dynamic_cast<SeasidePeopleModel *>(sourceModel());
    if (!model)
        return true;

    if (priv->filterType == FilterFavorites) {
        QModelIndex modelIndex = sourceModel()->index(source_row, 0, source_parent);
        return model->person(modelIndex.row()) ? model->person(modelIndex.row())->favorite() : false;
    } else {
        qWarning() << "[SeasideProxyModel] invalid filter type";
        return true;
    }
}

bool SeasideProxyModel::lessThan(const QModelIndex& left,
                          const QModelIndex& right) const
{
    SeasidePeopleModel *model = dynamic_cast<SeasidePeopleModel *>(sourceModel());
    if (!model)
        return true;

    // TODO: this should not be here
    qDebug("fastscroll: emitting countChanged");
    emit const_cast<SeasideProxyModel*>(this)->countChanged();

    SeasidePerson *leftPerson = model->person(left.row());
    SeasidePerson *rightPerson = model->person(right.row());

    if (!leftPerson)
        return false;
    else if (!rightPerson)
        return true;

    return priv->localeHelper->isLessThan(leftPerson->displayLabel(), rightPerson->displayLabel());
}

// needed for fastscroll
int SeasideProxyModel::count() const
{
    return rowCount(QModelIndex());
}

// needed for fastscroll
QVariantMap SeasideProxyModel::get(int row)
{
    QVariantMap listElement;
    listElement["firstcharacter"] = "?";

    SeasidePeopleModel *model = dynamic_cast<SeasidePeopleModel *>(sourceModel());
    if (!model)
        return listElement;

    SeasidePerson *person = model->person(row);

    listElement["firstcharacter"] = person->displayLabel().at(0); // TODO: won't be correct for localisation, also, this may need to be in SeasidePerson
    return listElement;
}


