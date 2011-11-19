/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <QDebug>
#include <string.h>
#include <unicode/ulocdata.h>
#include <unicode/uchriter.h>

#include "localeutils_p.h"
#include "seasidepeoplemodel.h"

LocaleUtils *LocaleUtils::mSelf = 0;

LocaleUtils::LocaleUtils(QObject *parent) :
    QObject(parent)
{
}

LocaleUtils::~LocaleUtils()
{
}

LocaleUtils *LocaleUtils::self()
{
    if (!mSelf) {
        mSelf = new LocaleUtils();
    }

    return mSelf;
}

QString LocaleUtils::getLanguage() const
{
    return QLocale::system().name();
}

QLocale::Country LocaleUtils::getCountry() const
{
    qDebug() << Q_FUNC_INFO << "FIXME: I may be incorrect";
    return QLocale::system().country();
}

int LocaleUtils::defaultValues(QString type) const
{
    Q_UNUSED(type);
    QLocale::Country country = getCountry();

    //REVISIT: Right now, all supported lanaguages
    //use the same setting for sort and display
    switch(country) {
        case QLocale::Japan:
        case QLocale::China:
        case QLocale::Taiwan:
        case QLocale::RepublicOfKorea:
        case QLocale::DemocraticRepublicOfKorea:
        case QLocale::Sweden:
        case QLocale::Norway:
        case QLocale::Hungary:
        case QLocale::France:
//            return SeasidePeopleModel::LastNameRole;
        default:
//            return SeasidePeopleModel::FirstNameRole;
            break;
    }
}

int LocaleUtils::defaultSortVal() const
{
    return defaultValues("sort");
}

int LocaleUtils::defaultDisplayVal() const
{
    return defaultValues("display");
}

QStringList LocaleUtils::getAddressFieldOrder() const
{
    QStringList fieldOrder;
    QLocale::Country country = getCountry();

    if ((country == QLocale::China) || (country == QLocale::Taiwan))
        fieldOrder << "country" << "region" << "locale" << "street" << "street2" << "zip";
    else if (country == QLocale::Japan)
        fieldOrder << "country" << "zip" << "region" << "locale" << "street" << "street2";
    else if ((country == QLocale::DemocraticRepublicOfKorea) ||
             (country == QLocale::RepublicOfKorea))
        fieldOrder << "country" << "region" << "locale" << "street" << "street2" << "zip";
    else
        fieldOrder << "street" << "street2" << "locale" << "region" << "zip" << "country";

    return fieldOrder;
}

bool LocaleUtils::needPronounciationFields() const {
    QStringList fieldOrder;
    QLocale::Country country = getCountry();

    if (country == QLocale::Japan)
        return true;
    return false;
}

bool LocaleUtils::usePhoneBookCol() const
{
    //Need to use the default collator for Japanese and
    //Korean as the PhoneBook collator is not valid for them 
    //REVISIT: Should this go in the local library?
    QLocale::Country country = getCountry();

    if ((country == QLocale::DemocraticRepublicOfKorea) ||
       (country == QLocale::RepublicOfKorea) ||
       (country == QLocale::Japan))
        return false;

    return true;
}

int LocaleUtils::compare(QString lStr, QString rStr)
{
    qDebug() << Q_FUNC_INFO << "FIXME: I may be incorrect";
    return lStr < rStr;
#if 0
    if (usePhoneBookCol())
        return locale->comparePhoneBook(lStr, rStr);
    else
        return locale->compare(lStr, rStr);
#endif
}

bool LocaleUtils::isLessThan(QString lStr, QString rStr)
{
    if (lStr == "#")
        return false;
    if (rStr == "#")
        return true;

    return lStr < rStr;
#if 0
    if (usePhoneBookCol())
        return locale->lessThanPhoneBook(lStr, rStr);
    else
        return locale->lessThan(lStr, rStr);
#endif
}

bool LocaleUtils::checkForAlphaChar(QString str)
{
    const ushort *strShort = str.utf16();
    UnicodeString uniStr = UnicodeString(static_cast<const UChar *>(strShort));

    //REVISIT: Might need to use a locale aware version of char32At()
    return u_hasBinaryProperty(uniStr.char32At(0), UCHAR_ALPHABETIC);
}

QString LocaleUtils::getExemplarForString(QString str)
{
    QStringList indexes = getIndexBarChars();
    int i = 0;

    for (; i < indexes.size(); i++) {
        if (compare(str, indexes.at(i)) == 0)
            return indexes.at(i);

        if (isLessThan(str, indexes.at(i))) {
            if (i == 0)
                return str;
            if (i == indexes.size() - 1)
                return indexes.at(i);
            return indexes.at(i-1);
        }
    }
    
    return QString(tr("#"));
}

QString LocaleUtils::getBinForString(QString str)
{
    //REVISIT: Might need to use a locale aware version of toUpper() and at()
    if (!checkForAlphaChar(str))
        return QString(tr("#"));

    qDebug() << Q_FUNC_INFO << "FIXME: I return invalid results, HACK";
    QString temp(str.at(0).toUpper());
    return temp;
    
    //The proper bin for these locales does not correspond
    //with a bin listed in the index bar
    QLocale::Country country = getCountry();
    if ((country == QLocale::Taiwan) || (country == QLocale::China))
        return temp;

    return getExemplarForString(temp);
}

QStringList LocaleUtils::getIndexBarChars()
{
    UErrorCode  status = U_ZERO_ERROR;
    QStringList default_list = QStringList() << "A" << "B" << "C" << "D" << "E"
                                             << "F" << "G" << "H" << "I" << "J"
                                             << "K" << "L" << "M" << "N" << "O"
                                             << "P" << "Q" << "R" << "S" << "T"
                                             << "U" << "V" << "W" << "Y" << "Z";
    QStringList list;

    QLocale::Country country = getCountry();
    QString locale = getLanguage();
    const char *name = locale.toLatin1().constData();

    //REVISIT: ulocdata_getExemplarSet() does not return the index characters
    //We need to query the locale data directly using the resource bundle 
    UResourceBundle *resource = ures_open(NULL, name, &status);

    if (!U_SUCCESS(status))
        return default_list;

    qint32 size;
    const UChar *indexes = ures_getStringByKey(resource,
                                               "ExemplarCharactersIndex",
                                               &size, &status);
    if (!U_SUCCESS(status))
        return default_list;

    //REVISIT:  This is work around for an encoding issue with KOR chars
    //returned by ICU. Use the compatiblity Jamo unicode values instead
    if ((country == QLocale::DemocraticRepublicOfKorea) ||
       (country == QLocale::RepublicOfKorea)) {
        int i = 0;
        static const QChar unicode[] = {0x3131, 0x3134, 0x3137, 0x3139, 0x3141,
                                        0x3142, 0x3145, 0x3147, 0x3148, 0x314A,
                                        0x314B, 0x314C, 0x314D, 0x314E};
        size = sizeof(unicode) / sizeof(QChar);
        QString jamo = QString::fromRawData(unicode, size);

        for (i = 0; i < jamo.length(); i++)
            list << jamo.at(i);
    }

    else {
        UCharCharacterIterator iter = UCharCharacterIterator(indexes, size);
        UChar c = iter.first();

        for (; c != CharacterIterator::DONE; c = iter.next()) {
            QString temp(c);
            if ((c != ' ') && (c != '[') && (c != ']')) { 
                //Check for exemplars that are two characters
                //These are denoted by '{}'
                if (c == '{') {
                    c = iter.next();
                    temp = "";
                    for (; c != '}'; c = iter.next())
                        temp += QString(c);
                }
                list << temp;
            }
        }
    }

    ures_close(resource);
    if (list.isEmpty())
        return default_list;

    if ((country == QLocale::Taiwan) || (country == QLocale::Japan) ||
       (country == QLocale::DemocraticRepublicOfKorea) ||
       (country == QLocale::RepublicOfKorea))
        list << "A" << "Z";

    list << QString(tr("#"));
    return list;
}

