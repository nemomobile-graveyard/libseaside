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

#include <QGraphicsGridLayout>

#include <MAbstractCellCreator>

#include "seasidelist.h"
#include "seasidelistitem.h"
#include "seaside.h"
#include "seasidesyncmodel.h"
#include "seasideproxymodel.h"

#include <MWidgetCreator>
M_REGISTER_WIDGET(SeasideList)

class CellCreator: public MAbstractCellCreator<SeasideListItem>
{
public:
    void updateCell(const QModelIndex& index, MWidget *cell) const
    {
        SeasideListItem *item = static_cast<SeasideListItem *>(cell);
        if (!item)
            return;

        SEASIDE_SHORTCUTS
        SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row())

        item->setThumbnail(SEASIDE_FIELD(Avatar, String));
        item->setName(SEASIDE_FIELD(Name, String));
    }
};

class SeasideListPriv
{
public:
    SeasideSyncModel *sourceModel;
    SeasideProxyModel *proxyModel;
};

SeasideList::SeasideList(MWidget *parent):
        MList(parent)
{
    priv = new SeasideListPriv;
    priv->sourceModel = SeasideSyncModel::instance();
    priv->proxyModel = new SeasideProxyModel;
    priv->proxyModel->sort(0);  // enable custom sorting
    priv->proxyModel->setSourceModel(priv->sourceModel);

    setItemModel(priv->proxyModel);

    setObjectName("SeasideList");
    setViewType("fast");

    setCellCreator(new CellCreator);

    connect(this, SIGNAL(itemClicked(QModelIndex)),
            this, SLOT(handleClick(QModelIndex)));
}

SeasideList::~SeasideList()
{
    SeasideSyncModel::releaseInstance();
    delete priv->proxyModel;
    delete priv;
}

void SeasideList::handleClick(const QModelIndex &index)
{
    SEASIDE_SHORTCUTS
    SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row())
    emit contactSelected(QUuid(SEASIDE_FIELD(Uuid, String)));
}
