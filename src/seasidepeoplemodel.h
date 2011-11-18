/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef SEASIDEPEOPLEMODEL_H
#define SEASIDEPEOPLEMODEL_H

#include <QProcess>
#include <QAbstractListModel>

#include <QUuid>
#include <QContactManagerEngine>

QTM_USE_NAMESPACE
class SeasidePeopleModelPriv;
class SeasidePerson;

class SeasidePeopleModel: public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(PeopleRoles)
    Q_ENUMS(FilterRoles)

public:
    SeasidePeopleModel(QObject *parent = 0);
    virtual ~SeasidePeopleModel();

    enum PeopleRoles {
        PersonRole = Qt::UserRole
    };

    //From QAbstractListModel
    Q_INVOKABLE virtual int rowCount(const QModelIndex& parent= QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;

    //QML API
    Q_INVOKABLE bool savePerson(SeasidePerson *person);
    Q_INVOKABLE SeasidePerson *person(int row) const;
    Q_INVOKABLE void deletePerson(SeasidePerson *person);

private:
    SeasidePeopleModelPriv *priv;
    friend class SeasidePeopleModelPriv;
    Q_DISABLE_COPY(SeasidePeopleModel);
};

#endif // SEASIDEPEOPLEMODEL_H
