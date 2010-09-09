/*
 * Libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "seasidecellcreators.h"

void CellCreator::updateCell(const QModelIndex& index, MWidget *cell) const
{
  qDebug() << "CellCreator::updateCell(const QModelIndex& index, MWidget *cell) " << index;
  
  if(!index.isValid())
    return;
  
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
  item->setButton(""); //button icon
  item->setDetails(QStringList());
}

void CellCreatorSmall::updateCell(const QModelIndex& index, MWidget *cell) const
{
  qDebug() << "CellCreatorSmall::updateCell(const QModelIndex& index, MWidget *cell) " << index;

  if(!index.isValid())
    return;

  SeasideListItemSmall *item = static_cast<SeasideListItemSmall *>(cell);
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
  item->setButton(""); //button icon
  item->setDetails(QStringList());
}

void CellCreatorLarge::updateCell(const QModelIndex& index, MWidget *cell) const
{
  qDebug() << "CellCreatorLarge::updateCell(const QModelIndex& index, MWidget *cell) " << index;

  if(!index.isValid())
    return;

  SeasideListItemLarge *item = static_cast<SeasideListItemLarge *>(cell);
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
  item->setButton(""); //button icon
  item->setDetails(QStringList());
}

void CellCreatorPhoneSmall::updateCell(const QModelIndex& index, MWidget *cell) const
{
  qDebug() << "CellCreatorPhoneSmall::updateCell(const QModelIndex& index, MWidget *cell) " << index;
  
  if(!index.isValid())
    return;
  
  SeasideListItemSmall *item = static_cast<SeasideListItemSmall *>(cell);
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
  item->setStatus(SEASIDE_FIELD(CommTimestamp, DateTime).toString());
  item->setButton("");
  item->setDetails(SEASIDE_FIELD(PhoneNumbers, StringList));
}

void CellCreatorPhoneLarge::updateCell(const QModelIndex& index, MWidget *cell) const
{
  qDebug() << "CellCreatorPhoneLarge::updateCell(const QModelIndex& index, MWidget *cell) " << index;

  if(!index.isValid())
    return;

  SeasideListItemLarge *item = static_cast<SeasideListItemLarge *>(cell);
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
  item->setStatus(SEASIDE_FIELD(CommTimestamp, DateTime).toString());
  item->setButton("");
  item->setDetails(SEASIDE_FIELD(PhoneNumbers, StringList));
}

void CellCreatorEmailSmall::updateCell(const QModelIndex& index, MWidget *cell) const
{
  qDebug() << "EmailCellCreatorSmall::updateCell(const QModelIndex& index, MWidget *cell) " << index;

  if(!index.isValid())
    return;

  SeasideListItemSmall *item = static_cast<SeasideListItemSmall *>(cell);
  if (!item)
    return;

  SEASIDE_SHORTCUTS
    SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row())

        item->setThumbnail(SEASIDE_FIELD(Avatar, String));
  item->setName(QObject::tr("%1 %2").arg(SEASIDE_FIELD(FirstName, String)).arg(SEASIDE_FIELD(LastName, String)));
  item->setUuid(SEASIDE_FIELD(Uuid, String));
  item->setFavorite(SEASIDE_FIELD(Favorite, Bool));
  item->setPresence(SEASIDE_FIELD(Presence, Int));
  item->setCommFlags(SEASIDE_FIELD(CommType, Int));
  item->setStatus(SEASIDE_FIELD(CommTimestamp, DateTime).toString());
  item->setButton("");
  item->setDetails(SEASIDE_FIELD(EmailAddresses, StringList));
}

void CellCreatorEmailLarge::updateCell(const QModelIndex& index, MWidget *cell) const
{
  qDebug() << "CellCreatorEmailLarge::updateCell(const QModelIndex& index, MWidget *cell) " << index;

  if(!index.isValid())
    return;

  SeasideListItemLarge *item = static_cast<SeasideListItemLarge *>(cell);
  if (!item)
    return;

  SEASIDE_SHORTCUTS
    SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row())

        item->setThumbnail(SEASIDE_FIELD(Avatar, String));
  item->setName(QObject::tr("%1 %2").arg(SEASIDE_FIELD(FirstName, String)).arg(SEASIDE_FIELD(LastName, String)));
  item->setUuid(SEASIDE_FIELD(Uuid, String));
  item->setFavorite(SEASIDE_FIELD(Favorite, Bool));
  item->setPresence(SEASIDE_FIELD(Presence, Int));
  item->setCommFlags(SEASIDE_FIELD(CommType, Int));
  item->setStatus(SEASIDE_FIELD(CommTimestamp, DateTime).toString());
  item->setButton("");
  item->setDetails(SEASIDE_FIELD(EmailAddresses, StringList));
}

void CellCreatorIMSmall::updateCell(const QModelIndex& index, MWidget *cell) const
{
  qDebug() << "CellCreator IMSmall::updateCell(const QModelIndex& index, MWidget *cell) " << index;

  if(!index.isValid())
    return;

  SeasideListItemSmall *item = static_cast<SeasideListItemSmall *>(cell);
  if (!item)
    return;

  SEASIDE_SHORTCUTS
    SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row())

    item->setThumbnail(SEASIDE_FIELD(Avatar, String));
  item->setName(QObject::tr("%1 %2").arg(SEASIDE_FIELD(FirstName, String)).arg(SEASIDE_FIELD(LastName, String)));
  item->setUuid(SEASIDE_FIELD(Uuid, String));
  item->setFavorite(SEASIDE_FIELD(Favorite, Bool));
  item->setPresence(SEASIDE_FIELD(Presence, Int));
  item->setCommFlags(SEASIDE_FIELD(CommType, Int));
  item->setStatus(SEASIDE_FIELD(CommTimestamp, DateTime).toString());
  item->setDetails(SEASIDE_FIELD(IMAccounts, StringList));
}

void CellCreatorIMLarge::updateCell(const QModelIndex& index, MWidget *cell) const
{
  qDebug() << "CellCreatorIMLarge::updateCell(const QModelIndex& index, MWidget *cell) " << index;
  
  if(!index.isValid())
    return;
  
  SeasideListItemLarge *item = static_cast<SeasideListItemLarge *>(cell);
  if (!item)
    return;
  
  SEASIDE_SHORTCUTS
    SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row())
    
    item->setThumbnail(SEASIDE_FIELD(Avatar, String));
  item->setName(QObject::tr("%1 %2").arg(SEASIDE_FIELD(FirstName, String)).arg(SEASIDE_FIELD(LastName, String)));
  item->setUuid(SEASIDE_FIELD(Uuid, String));
  item->setFavorite(SEASIDE_FIELD(Favorite, Bool));
  item->setPresence(SEASIDE_FIELD(Presence, Int));
  item->setCommFlags(SEASIDE_FIELD(CommType, Int));
  item->setStatus(SEASIDE_FIELD(CommTimestamp, DateTime).toString());
  item->setDetails(SEASIDE_FIELD(IMAccounts, StringList));
}

void CellCreatorPhotoSmall::updateCell(const QModelIndex& index, MWidget *cell) const
{
  qDebug() << "CellCreatorPhotoSmall::updateCell(const QModelIndex& index, MWidget *cell) " << index;

  if(!index.isValid())
    return;

  SeasideListItemSmall *item = static_cast<SeasideListItemSmall *>(cell);
  if (!item)
    return;

  SEASIDE_SHORTCUTS
    SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row())

    item->setThumbnail(SEASIDE_FIELD(Avatar, String));
  item->setName(QObject::tr("%1 %2").arg(SEASIDE_FIELD(FirstName, String)).arg(SEASIDE_FIELD(LastName, String)));
  item->setUuid(SEASIDE_FIELD(Uuid, String));
  item->setFavorite(SEASIDE_FIELD(Favorite, Bool));
  item->setPresence(SEASIDE_FIELD(Presence, Int));
  item->setCommFlags(SEASIDE_FIELD(CommType, Int));
  item->setStatus(SEASIDE_FIELD(CommTimestamp, DateTime).toString());
  item->setDetails(QStringList());
}

void CellCreatorPhotoLarge::updateCell(const QModelIndex& index, MWidget *cell) const
{
  qDebug() << "CellCreatorPhotoLarge::updateCell(const QModelIndex& index, MWidget *cell) " << index;

  if(!index.isValid())
    return;

  SeasideListItemLarge *item = static_cast<SeasideListItemLarge *>(cell);
  if (!item)
    return;

  SEASIDE_SHORTCUTS
    SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row())

    item->setThumbnail(SEASIDE_FIELD(Avatar, String));
  item->setName(QObject::tr("%1 %2").arg(SEASIDE_FIELD(FirstName, String)).arg(SEASIDE_FIELD(LastName, String)));
  item->setUuid(SEASIDE_FIELD(Uuid, String));
  item->setFavorite(SEASIDE_FIELD(Favorite, Bool));
  item->setPresence(SEASIDE_FIELD(Presence, Int));
  item->setCommFlags(SEASIDE_FIELD(CommType, Int));
  item->setStatus(SEASIDE_FIELD(CommTimestamp, DateTime).toString());
  item->setDetails(QStringList());
}

