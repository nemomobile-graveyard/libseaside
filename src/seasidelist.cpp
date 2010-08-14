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
#include "seasidesyncmodel.h"
#include "seasideproxymodel.h"
#include "seasidelistmodel.h"

#include <MWidgetCreator>
M_REGISTER_WIDGET(SeasideList)


class CellCreator: public MAbstractCellCreator<SeasideListItem>
{
public:
    CellCreator(const QString type)
	{
                itemType = type;
	}

    void updateCell(const QModelIndex& index, MWidget *cell) const
    {
        SeasideListItem *item = static_cast<SeasideListItem *>(cell);
        if (!item)
            return;

        SEASIDE_SHORTCUTS
        SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row())

        item->setThumbnail(SEASIDE_FIELD(Avatar, String));
        item->setName(QObject::tr("%1 %2").arg(SEASIDE_FIELD(FirstName, String)).arg(SEASIDE_FIELD(LastName, String)));
        item->setUuid(SEASIDE_FIELD(Uuid, String));
        item->setFavorite(SEASIDE_FIELD(Favorite, Bool)); //REVISIT
        item->setPresence(SEASIDE_FIELD(Presence, Int));
        item->setCommFlags(SEASIDE_FIELD(CommType, Int));
        //item->setCommFlags((0|3|5|7)); //0|3|5|7 for all
        item->setStatus(SEASIDE_FIELD(CommTimestamp, DateTime).toString());
        //item->setStatus("This is a status string");
        item->setButton("");

        if(itemType == "Phone")
        	item->setDetails(SEASIDE_FIELD(PhoneNumbers, StringList));
        else if(itemType == "Email")
        	item->setDetails(SEASIDE_FIELD(EmailAddresses, StringList));
        else if(itemType == "IM")
        	item->setDetails(SEASIDE_FIELD(IMAccounts, StringList));
	else
		item->setDetails(SEASIDE_FIELD(PhoneNumbers, StringList));	
    }
private:
    QString itemType;
};

class SeasideListPriv
{
public:
    SeasideSyncModel *sourceModel;
    SeasideProxyModel *proxyModel;
};

SeasideList::SeasideList(Detail detail, MWidget *parent):
        MList(parent)
{
    
    priv = new SeasideListPriv;
    priv->sourceModel = SeasideSyncModel::instance();
    priv->proxyModel = new SeasideProxyModel;
    priv->proxyModel->sort(0);  // enable custom sorting
    priv->proxyModel->setSourceModel(priv->sourceModel);

     qDebug() << "SETTING ITEM MODEL";
     //setItemModel(new SeasideListModel(detail));
    setItemModel(priv->proxyModel);

    setObjectName("SeasideList");
    setViewType("fast");

    QString type = "";
    if(detail == SeasideList::DetailPhone)
            type = "Phone";
    else if(detail == SeasideList::DetailEmail)
           type = "Email";
    else if(detail == SeasideList::DetailIM)
            type = "IM";
    else
            type = "Phone";

    setCellCreator(new CellCreator(type)); //REVISIT

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
   //REVISIT should only emit signals of detail enabled
   emit imAccountsSelected(QStringList(SEASIDE_FIELD(IMAccounts, StringList)));
   emit emailsSelected(QStringList(SEASIDE_FIELD(EmailAddresses, StringList)));
   emit phonesSelected(QStringList(SEASIDE_FIELD(PhoneNumbers, StringList)));
}


