/*
 * Libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef CELLCREATORS_H
#define CELLCREATORS_H

#include <MAbstractCellCreator>
#include "seasidelistitem.h"
#include "seasidelistitemsmall.h"
#include "seasidelistitemlarge.h"

class CellCreator: public MAbstractCellCreator<SeasideListItem> //default
{
        public:
            void updateCell(const QModelIndex& index, MWidget *cell) const;
 };

class CellCreatorSmall: public MAbstractCellCreator<SeasideListItemSmall>
{
        public:
            void updateCell(const QModelIndex& index, MWidget *cell) const;
 };

class CellCreatorLarge: public MAbstractCellCreator<SeasideListItemLarge>
{
        public:
            void updateCell(const QModelIndex& index, MWidget *cell) const;
 };

class CellCreatorPhoneSmall: public MAbstractCellCreator<SeasideListItemSmall>
{
public:
    void updateCell(const QModelIndex& index, MWidget *cell) const;
};

class CellCreatorPhoneLarge: public MAbstractCellCreator<SeasideListItemLarge>
{
    public:
    void updateCell(const QModelIndex& index, MWidget *cell) const;
};

class CellCreatorEmailSmall: public MAbstractCellCreator<SeasideListItemSmall>
{
 public:
  void updateCell(const QModelIndex& index, MWidget *cell) const;
};

class CellCreatorEmailLarge: public MAbstractCellCreator<SeasideListItemLarge>
{
 public:
  void updateCell(const QModelIndex& index, MWidget *cell) const;
};

class CellCreatorIMSmall: public MAbstractCellCreator<SeasideListItemSmall>
{
 public:
  void updateCell(const QModelIndex& index, MWidget *cell) const;
};

class CellCreatorIMLarge: public MAbstractCellCreator<SeasideListItemLarge>
{
 public:
  void updateCell(const QModelIndex& index, MWidget *cell) const;
};

class CellCreatorPhotoSmall: public MAbstractCellCreator<SeasideListItemSmall>
{
 public:
  void updateCell(const QModelIndex& index, MWidget *cell) const;
};

class CellCreatorPhotoLarge: public MAbstractCellCreator<SeasideListItemLarge>
{
 public:
  void updateCell(const QModelIndex& index, MWidget *cell) const;
};

#endif // CELLCREATORS_H
