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

#include "seasideproxymodel.h"
#include "settingsdatastore_p.h"
#include "localeutils_p.h"

class SeasideProxyModelPriv
{
public:
    SeasideProxyModel::FilterType filterType;
    SeasidePeopleModel::PeopleRoles sortType;
    SeasidePeopleModel::PeopleRoles displayType;
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
    setSortType((SeasidePeopleModel::PeopleRoles) priv->settings->getSortOrder());

    priv->settings->getDisplayOrder();
    setDisplayType((SeasidePeopleModel::PeopleRoles) priv->settings->getDisplayOrder());
}

void SeasideProxyModel::setFilter(FilterType filter)
{
    priv->filterType = filter;
    invalidateFilter();
}

void SeasideProxyModel::setSortType(SeasidePeopleModel::PeopleRoles sortType)
{
    priv->sortType = sortType;
    setSortRole(sortType);

    SeasidePeopleModel *model = dynamic_cast<SeasidePeopleModel *>(sourceModel());
    if (model)
        model->setSorting(sortType);

    reset(); //Clear the current sort method and then re-sort
    sort(0, Qt::AscendingOrder);
}

void SeasideProxyModel::setDisplayType(SeasidePeopleModel::PeopleRoles displayType)
{
    priv->displayType = displayType;
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
        //return model->index(source_row, SeasidePeopleModel::FavoriteRole).data(DataRole);
        return (model->data(modelIndex, SeasidePeopleModel::FavoriteRole) == "Favorite");
    }
    else {
        qWarning() << "[SeasideProxyModel] invalid filter type";
        return true;
    }
}

QString SeasideProxyModel::findString(int row, SeasidePeopleModel *model,
                               SeasideProxyModel::StringType strType = SeasideProxyModel::Primary) const {
    QString lStr = QString("");

    if ((priv->sortType != SeasidePeopleModel::FirstNameRole)
        && (priv->sortType != SeasidePeopleModel::LastNameRole))
        return lStr;

    int searchRole = SeasidePeopleModel::FirstNameRole;
    int secondaryRole = SeasidePeopleModel::LastNameRole;

    if (priv->sortType == SeasidePeopleModel::LastNameRole) {
        if (priv->localeHelper->needPronounciationFields()) {
            searchRole = SeasidePeopleModel::LastNameProRole;
            secondaryRole = SeasidePeopleModel::FirstNameProRole;
        } else {
            searchRole = SeasidePeopleModel::LastNameRole;
            secondaryRole = SeasidePeopleModel::FirstNameRole;
        }
    } else {
        if (priv->localeHelper->needPronounciationFields()) {
            searchRole = SeasidePeopleModel::FirstNameProRole;
            secondaryRole = SeasidePeopleModel::LastNameProRole;
        }
    }

    bool primaryFound = false;
    QList<int> roleOrder;
    roleOrder << searchRole << secondaryRole
              << SeasidePeopleModel::CompanyNameRole
              << SeasidePeopleModel::PhoneNumberRole
              << SeasidePeopleModel::OnlineAccountUriRole
              << SeasidePeopleModel::EmailAddressRole
              << SeasidePeopleModel::WebUrlRole;

    for (int i = 0; i < roleOrder.size(); ++i) {
        lStr = model->data(row, roleOrder.at(i)).toString();

        if (!lStr.isEmpty()) {
            if ((strType == SeasideProxyModel::Secondary) && (!primaryFound))
                primaryFound = true;
            else
                return lStr;
        }

        if (priv->localeHelper->needPronounciationFields()) {
            if (roleOrder.at(i) == SeasidePeopleModel::FirstNameProRole)
                lStr = model->data(row, SeasidePeopleModel::FirstNameRole).toString();
            else if (roleOrder.at(i) == SeasidePeopleModel::LastNameProRole)
                lStr = model->data(row, SeasidePeopleModel::LastNameRole).toString();

            if (!lStr.isEmpty()) {
                if ((strType == SeasideProxyModel::Secondary) && (!primaryFound))
                    primaryFound = true;
                else
                    return lStr;
            }
        }

    }

    lStr = QString("");
    return lStr;
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

    int leftRow = left.row();
    int rightRow = right.row();

    const bool isleftSelf = model->data(leftRow, SeasidePeopleModel::IsSelfRole).toBool();
    const bool isrightSelf = model->data(rightRow, SeasidePeopleModel::IsSelfRole).toBool();

    //qWarning() << "[SeasideProxyModel] lessThan isSelf left" << isleftSelf << " right" << isrightSelf;

    //MeCard should always be top of the list
    if(isleftSelf)
        return true;
    if(isrightSelf)
        return false;

    QString lStr = findString(leftRow, model);
    QString rStr = findString(rightRow, model);

    //qWarning() << "[SeasideProxyModel] lessThan " << lStr << "VS" << rStr;

    if (lStr.isEmpty())
        return false;
    else if (rStr.isEmpty())
        return true;

    if (!priv->localeHelper->checkForAlphaChar(lStr))
        return false;
    if (!priv->localeHelper->checkForAlphaChar(rStr))
        return true;

    if (priv->localeHelper->compare(lStr, rStr) == 0) {
        lStr += findString(leftRow, model, SeasideProxyModel::Secondary);
        rStr += findString(rightRow, model, SeasideProxyModel::Secondary);
        return priv->localeHelper->isLessThan(lStr, rStr);
    }

    return priv->localeHelper->isLessThan(lStr, rStr);
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

    if (row < 0 || row > count())
        return listElement;

    listElement["firstcharacter"] = data(index(row, 0),
            SeasidePeopleModel::FirstCharacterRole).toString();
    qDebug() << "fastscroll: " << listElement;
//    listElement["section"] = QVariantMap(QString("firstcharacter"), data(index(row, 0)));
    return listElement;
}


