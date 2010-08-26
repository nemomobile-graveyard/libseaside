/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "seasidepersonmodel.h"

bool SeasidePersonModel::isEmpty()
{
    if (!firstname().isEmpty() || !lastname().isEmpty() || !company().isEmpty() ||!thumbnail().isEmpty())
        return false;
    if (!phones().isEmpty() || !emails().isEmpty() || !addresses().isEmpty())
        return false;
    return true;
}

void SeasidePersonModel::setPhone(int index, const SeasideDetail& detail)
{
    QVector<SeasideDetail>& vector = _phones();
    if (index >= vector.count())
        vector.resize(index + 1);
    vector[index] = detail;
    memberModified(SeasidePersonModel::Phones);
}

void SeasidePersonModel::setIMAccount(int index, const SeasideDetail& detail)
{
    QVector<SeasideDetail>& vector = _ims();
    if (index >= vector.count())
        vector.resize(index + 1);
    vector[index] = detail;
    memberModified(SeasidePersonModel::IMs);
}

void SeasidePersonModel::setEmail(int index, const SeasideDetail& detail)
{
    QVector<SeasideDetail>& vector = _emails();
    if (index >= vector.count())
        vector.resize(index + 1);
    vector[index] = detail;
    memberModified(SeasidePersonModel::Emails);
}

void SeasidePersonModel::setAddress(int index, const SeasideDetail& detail)
{
    QVector<SeasideDetail>& vector = _addresses();
    if (index >= vector.count())
        vector.resize(index + 1);
    vector[index] = detail;
    memberModified(SeasidePersonModel::Addresses);
}
