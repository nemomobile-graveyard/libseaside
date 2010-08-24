/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDECOMMSPAGE_H
#define SEASIDECOMMSPAGE_H

#include <MApplicationPage>
#include <seasidelist.h>
#include <seaside.h>

class SeasideCommsPage : public MApplicationPage
{
    Q_OBJECT
public:
    SeasideCommsPage(SeasidePersonModel *pm, SeasideList::CommCat type);

signals:
    void destSelected(QString);

private slots:
    void onButtonClicked(int id);

private:
    QList<SeasideDetail> m_detailList;
};

#endif // SEASIDECOMMSPAGE_H
