/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PERSONCOMMSPAGE_H
#define PERSONCOMMSPAGE_H

#include <MApplicationPage>
#include <seasidelist.h>
#include <seaside.h>

class PersonCommsPage : public MApplicationPage
{
    Q_OBJECT
public:
    PersonCommsPage(SeasidePersonModel *pm, SeasideList::CommCat type);

signals:
    void destSelected(QString);

private slots:
    void onButtonClicked(int id);

private:
    QList<SeasideDetail> m_detailList;
};

#endif // PERSONCOMMSPAGE_H
