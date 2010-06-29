/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QDebug>

#include <QContact>
#include <QContactAddress>
#include <QContactAnniversary>
#include <QContactAvatar>
#include <QContactBirthday>
#include <QContactEmailAddress>
#include <QContactNote>
#include <QContactOrganization>
#include <QContactPhoneNumber>
#include <QContactPresence>

#include "seaside.h"
#include "seasidedetail.h"

#include <mlibrary.h>
M_LIBRARY

QString Seaside::contactName(QContact *contact)
{
    return contact->displayLabel();
}

QString Seaside::contactAvatarPath(QContact *contact)
{
    QContactAvatar avatar = contact->detail(QContactAvatar::DefinitionName);
    return avatar.imageUrl().toString();
}

QString Seaside::contactOrganization(QContact *contact)
{
    QContactOrganization company = contact->detail(QContactOrganization::DefinitionName);
    return company.name();
}

QDate Seaside::contactBirthday(QContact *contact)
{
    QContactBirthday birthday = contact->detail(QContactBirthday::DefinitionName);
    return birthday.date();
}

QDate Seaside::contactAnniversary(QContact *contact)
{
    QContactAnniversary day = contact->detail(QContactAnniversary::DefinitionName);
    return day.originalDate();
}

bool Seaside::contactFavorite(const QContact *contact)
{
    // TODO: remove this completely
    qWarning() << "contactFavorite DEPRECATED:" << "contact no longer contains favorite info";
    foreach (const SeasideCustomDetail& detail,
             contact->details(SeasideCustomDetail::DefinitionName))
        return detail.favorite();
    return false;
}

Seaside::Presence Seaside::contactPresence(const QContact *contact)
{
    // report the most available presence status found
    Presence presence = PresenceUnknown;

    foreach (const QContactPresence& pres,
             contact->details(QContactPresence::DefinitionName)) {
        QContactPresence::PresenceState state = pres.presenceState();

        if (state == QContactPresence::PresenceAvailable)
            presence = PresenceAvailable;
        else if (state == QContactPresence::PresenceAway ||
                 state == QContactPresence::PresenceExtendedAway) {
            if (presence != PresenceAvailable)
                presence = PresenceAway;
        }
        else if (state == QContactPresence::PresenceOffline) {
            if (presence == PresenceUnknown)
                presence = PresenceOffline;
        }

        // TODO: consider different treatment of these two
        //   QContactOnlineAccount::PresenceBusy:
        //   QContactOnlineAccount::PresenceHidden:
    }
    return presence;
}

QDateTime Seaside::contactCommTimestamp(QContact *contact)
{
    SeasideCustomDetail detail = contact->detail(SeasideCustomDetail::DefinitionName);
    return detail.commTimestamp();
}

QStringList Seaside::contactEmailAddresses(QContact *contact)
{
    QStringList list;
    foreach (const QContactEmailAddress& email,
             contact->details(QContactEmailAddress::DefinitionName))
        list << email.emailAddress();
    return list;
}

QStringList Seaside::contactAddresses(QContact *contact)
{
    QStringList list;
    foreach (const QContactAddress& address,
             contact->details(QContactAddress::DefinitionName))
        list << address.street() + "<br>" + address.locality() + ", " +
                address.region();
    return list;
}

QStringList Seaside::contactPhoneNumbers(QContact *contact)
{
    QStringList list;
    foreach (const QContactPhoneNumber& phone,
             contact->details(QContactPhoneNumber::DefinitionName))
        list << phone.number();
    return list;
}

QStringList Seaside::contactNotes(QContact *contact)
{
    QStringList list;
    foreach (const QContactNote& note,
             contact->details(QContactNote::DefinitionName)) {
        const QString& str = note.note();
        if (str != "Favorite")
            list << note.note();
    }
    return list;
}

SeasideCommEvent::SeasideCommEvent()
{
    m_type = Seaside::CommNone;
    m_location = Seaside::LocationNone;
}

SeasideCommEvent::~SeasideCommEvent()
{
}

bool SeasideCommEvent::operator ==(const SeasideCommEvent& event)
{
    return m_type == event.m_type && m_location == event.m_location &&
            m_dateTime == event.m_dateTime && m_text == event.m_text;
}

void SeasideCommEvent::setType(Seaside::CommType type)
{
    m_type = type;
}

void SeasideCommEvent::setLocation(Seaside::Location location)
{
    m_location = location;
}

void SeasideCommEvent::setDateTime(const QDateTime& dateTime)
{
    m_dateTime = dateTime;
}

void SeasideCommEvent::setText(const QString& text)
{
    m_text = text;
}

Seaside::CommType SeasideCommEvent::type()
{
    return m_type;
}

Seaside::Location SeasideCommEvent::location()
{
    return m_location;
}

QDateTime SeasideCommEvent::dateTime()
{
    return m_dateTime;
}

const QString& SeasideCommEvent::text()
{
    return m_text;
}

QString SeasideCommEvent::getFriendlyDateString()
{
    if (!m_dateTime.isValid())
        return QString();

    QString date;
    if (m_dateTime.date() == QDate::currentDate())
        date = "Today";  // TODO: i18n
    else if (m_dateTime.date() == QDate::currentDate().addDays(-1))
        date = "Yesterday";  // TODO: i18n
    else
        // format string for month and day, using QDateTime format
        date = m_dateTime.toString("MMM d");  // TODO: i18n

    // format string for time, using QDateTime format
    QString time = m_dateTime.toString("hh:mm");  // TODO: i18n

    // format string for merging date and time, %1 = date, %2 = time
    QString format("%1, %2");

    return format.arg(date, time);
}

SeasideDetail::SeasideDetail()
{
    m_valid = false;
}

SeasideDetail::SeasideDetail(const QString &text, Seaside::Location location)
{
    m_valid = true;
    m_location = location;
    m_text = text;
}

SeasideDetail::~SeasideDetail()
{
}

bool SeasideDetail::operator ==(const SeasideDetail& detail)
{
    if (!m_valid && !detail.m_valid)
        return true;
    if (!m_valid || !detail.m_valid)
        return false;
    return m_location == detail.m_location && m_text == detail.m_text;
}

bool SeasideDetail::isValid() const
{
    return m_valid;
}

Seaside::Location SeasideDetail::location() const
{
   return m_location;
}

const QString& SeasideDetail::text() const
{
   return m_text;
}
