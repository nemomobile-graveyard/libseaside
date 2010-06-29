/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QObject>
#include <QUuid>

class Target: public QObject
{
    Q_OBJECT

public slots:
    void contactClicked(const QUuid& uuid);
};
