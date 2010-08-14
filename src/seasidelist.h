/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDELIST_H
#define SEASIDELIST_H

#include <QUuid>

#include <MList>
#include "seaside.h"

class SeasideListPriv;
class SeasideSyncModel;
class SeasideProxyModel;

/*
 * SeasideList: Styling Notes
 *
 * The object name is set to SeasideList by default, but has no special styles
 * beyond those inherited from MList and MWidget
 *
 * See seasidelistitem.h for information on styling individual list items
 *
 */

class SeasideList: public MList
{
    Q_OBJECT

public:
  enum Detail {
        DetailEmail,
        DetailPhone,
	DetailIM,
        DetailNone
    };

    SeasideList(Detail detail = DetailNone, MWidget *parent = NULL);
    virtual ~SeasideList();

    SeasideSyncModel *sourceModel();
    SeasideProxyModel *proxyModel();

signals:
    void contactSelected(const QUuid& uuid);
    void imAccountsSelected(const QStringList accounts);
    void emailsSelected(const QStringList emails);
    void phonesSelected(const QStringList numbers);

protected slots:
    void handleClick(const QModelIndex& index);

private:
    SeasideListPriv *priv;
    Q_DISABLE_COPY(SeasideList);
};

#endif // SEASIDELIST_H
