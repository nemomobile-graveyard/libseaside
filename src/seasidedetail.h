/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDEDETAIL_H
#define SEASIDEDETAIL_H

#include <QDateTime>

#include <QContactDetail>

using namespace QtMobility;

class SeasideCustomDetail: public QContactDetail
{
public:
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(SeasideCustomDetail, "Seaside");
    Q_DECLARE_LATIN1_CONSTANT(FieldCommTimestamp, "CommTimestamp");
    Q_DECLARE_LATIN1_CONSTANT(FieldCommType, "CommType");
    Q_DECLARE_LATIN1_CONSTANT(FieldCommLocation, "CommLocation");

    void setCommTimestamp(const QDateTime& timestamp) { setValue(FieldCommTimestamp, timestamp); }
    QDateTime commTimestamp() const { return value<QDateTime>(FieldCommTimestamp); }

    void setCommType(int type) { setValue(FieldCommType, type); }
    int commType() const { return value<int>(FieldCommType); }

    void setCommLocation(int location) { setValue(FieldCommLocation, location); }
    int commLocation() const { return value<int>(FieldCommLocation); }
};

#endif // SEASIDEDETAIL_H
