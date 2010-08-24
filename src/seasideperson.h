/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PERSON_H
#define PERSON_H

#include <QModelIndex>

#include <MWidgetController>

#include <seaside.h>

class SeasidePerson: public MWidgetController
{
    Q_OBJECT

public:
    SeasidePerson(const QModelIndex& index, MWidget *parent = NULL);
    virtual ~SeasidePerson();

signals:
    void callNumber(const QString& number);
    void composeSMS(const QString& number);
    void composeEmail(const QString& address);
    void viewRequest(qreal ypos, qreal height);
};

#endif // PERSON_H
