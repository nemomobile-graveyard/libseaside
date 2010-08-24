/*
 * Libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QDebug>

#include <QVector>
#include <QContactAddress>
#include <QContactAnniversary>
#include <QContactAvatar>
#include <QContactBirthday>
#include <QContactEmailAddress>
#include <QContactGuid>
#include <QContactName>
#include <QContactNote>
#include <QContactOrganization>
#include <QContactPhoneNumber>
#include <QContactPresence>

#include <QContactDetailFilter>

#include <MTheme>

#include "seasidelistmodel.h"
#include "seasidesyncmodel.h"
#include "seaside.h"
#include "seasidedetail.h"
#include "seasidepersonmodel.h"

class SeasideListModelPriv
{
public:
    QContactManager *manager;
    QContactFilter currentFilter;
    QList<QContactLocalId> contactIds;
    QMap<QContactLocalId, int> idToIndex;
    QMap<QContactLocalId, QContact *> idToContact;
    QMap<QUuid, QContactLocalId> uuidToId;
    QMap<QContactLocalId, QUuid> idToUuid;

    QVector<QStringList> data;
    QStringList headers;
    QStringList names;
    QStringList avatars;
    QList<QUuid> uuids;
    QStringList datatype;
};

SeasideListModel::SeasideListModel(SeasideList::Detail detail)
{
    priv = new SeasideListModelPriv;

    priv->headers.append("First Name");
    priv->headers.append("Last Name");
    priv->headers.append("Avatar");
    priv->headers.append("Uuid");
    priv->headers.append("Detail");

    // FIXME: temporarily create sync model instance to generate fake data
    SeasideSyncModel *instance = SeasideSyncModel::instance();
    QContactManager *manager = instance->manager();
    if (!manager) {
        qWarning() << "failed creating contact manager";
        return;
    }

    qDebug() << "\n\nLISTMODEL!\n\n";
    QList<QContact> contacts;
    QContactDetailFilter filter;
    if (detail == SeasideList::DetailEmail) {
        filter.setDetailDefinitionName(QContactEmailAddress::DefinitionName);
        contacts = manager->contacts(filter);
    }
    else if (detail == SeasideList::DetailPhone) {
        filter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName);
        contacts = manager->contacts(filter);
    }
    else
        contacts = manager->contacts();

    qDebug() << "CONTACTS:" << contacts.size();
    foreach (const QContact& contact, contacts) {
        QString name = Seaside::contactName(&contact);
        qDebug() << "CONTACTS name:" << name;
    }
}

SeasideListModel::~SeasideListModel()
{
    foreach (QContact *contact, priv->idToContact.values())
        delete contact;
    delete priv->manager;
    delete priv;
}

int SeasideListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return priv->contactIds.size();
}

int SeasideListModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return priv->headers.size();
}

static QVariant getContextList(const QList<QContactDetail>& details)
{
    QStringList list;
    foreach (const QContactDetail& detail, details) {
        QString str;
        foreach (const QString& context, detail.contexts()) {
            if (context == "Home") {
                str = "Home";
                break;
            }
            else if (context == "Work") {
                str = "Work";
                break;
            }
        }
        list << str;
    }
    return QVariant(list);
}

QVariant SeasideListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Seaside::SearchRole && role != Seaside::DataRole)
        return QVariant();

    QContactLocalId id = priv->contactIds[index.row()];
    QContact *contact = priv->idToContact[id];

    if (!contact)
        return QVariant();

    // expect searching on SearchRole, only return text that makes sense to search

    switch (index.column()) {
    case Seaside::ColumnFirstName:  // first name
        {
            QContactName fname = contact->detail<QContactName>();
            QString strName(fname.firstName());            
            return QVariant(strName);
        }

   case Seaside::ColumnLastName:  // last name
        {
            QContactName lname = contact->detail<QContactName>();
            QString strName(lname.lastName());
            return QVariant(strName);
        }

    case Seaside::ColumnBirthday:  // birthday
        {
            QContactBirthday day = contact->detail<QContactBirthday>();
            if (role == Seaside::SearchRole)
                return QVariant(day.date().toString("MMMM dd yyyy"));
            else
                return QVariant(day.date());
        }

    case Seaside::ColumnAnniversary:  // anniversary
        {
            QContactAnniversary day = contact->detail<QContactAnniversary>();
            if (role == Seaside::SearchRole)
                return QVariant(day.originalDate().toString("MMMM dd yyyy"));
            else
                return QVariant(day.originalDate());
        }

    case Seaside::ColumnAvatar:  // avatar
        {
            QContactAvatar avatar = contact->detail<QContactAvatar>();
            // almost conceivable someone could want to search by the avatar filename,
            //   so I'll leave this on SearchRole for the moment
            return QVariant(avatar.imageUrl().toString());
        }

    case Seaside::ColumnFavorite:  // favorite
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            foreach (const SeasideCustomDetail& detail, contact->details<SeasideCustomDetail>())
                return QVariant(detail.favorite());
            return QVariant(false);
        }

    case Seaside::ColumnEmailAddresses:  // emails
        {
            QStringList list;
            foreach (const QContactEmailAddress& email,
                     contact->details<QContactEmailAddress>())
                list << email.emailAddress();
            if (role == Seaside::SearchRole)
                return QVariant(list.join(" "));
            else
                return QVariant(list);
        }

    case Seaside::ColumnPhoneNumbers:  // phones
        {
            QStringList list;
            foreach (const QContactPhoneNumber& phone,
                     contact->details<QContactPhoneNumber>())
                list << phone.number();
            if (role == Seaside::SearchRole) {
                QStringList searchable;
                foreach (QString number, list)
                    searchable << number.replace(QRegExp("[^0-9*#]"), "");
                searchable << list;
                return QVariant(searchable.join(" "));
            }
            else
                return QVariant(list);
        }

    case Seaside::ColumnPhoneContexts:  // phone contexts
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            return getContextList(contact->details(QContactPhoneNumber::DefinitionName));
        }

    case Seaside::ColumnPhoneTypes:  // phone types
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            QStringList list;
            foreach (const QContactPhoneNumber& phone,
                     contact->details<QContactPhoneNumber>()) {
                QString str;
                foreach (const QString& subtype, phone.subTypes()) {
                    // TODO: handle MessagingCapable, Pager, etc...
                    if (subtype == QContactPhoneNumber::SubTypeMobile) {
                        str = "Mobile";
                        break;
                    }
                }
                list << str;
            }
            return QVariant(list);
        }

    case Seaside::ColumnAddresses:  // addresses
        {
            QStringList list;
            foreach (const QContactAddress& address,
                     contact->details<QContactAddress>()) {
	      list << address.street() << "\n" << address.locality() << "\n" << address.region() << "\n" << address.postcode() << "\n" << address.country();
            }
            if (role == Seaside::SearchRole)
                return QVariant(list.join(" "));
            else
                return QVariant(list);
        }

    case Seaside::ColumnAddressContexts:  // address contexts
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            return getContextList(contact->details(QContactAddress::DefinitionName));
        }

    case Seaside::ColumnPresence:  // presence
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            // report the most available presence status found
            Seaside::Presence presence = Seaside::PresenceUnknown;
            foreach (const QContactPresence& qp,
                     contact->details<QContactPresence>()) {
                QContactPresence::PresenceState state = qp.presenceState();

                if (state == QContactPresence::PresenceAvailable)
                    presence = Seaside::PresenceAvailable;
                else if (state == QContactPresence::PresenceAway ||
                         state == QContactPresence::PresenceExtendedAway) {
                    if (presence != Seaside::PresenceAvailable)
                        presence = Seaside::PresenceAway;
                }
                else if (state == QContactPresence::PresenceOffline) {
                    if (presence == Seaside::PresenceUnknown)
                        presence = Seaside::PresenceOffline;
                }

                // TODO: consider different treatment of these two
                //   QContactOnlineAccount::PresenceBusy:
                //   QContactOnlineAccount::PresenceHidden:
            }
            return QVariant(presence);
        }

    case Seaside::ColumnUuid:  // uuid
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            QContactGuid guid = contact->detail<QContactGuid>();
            return QVariant(guid.guid());
        }

    case Seaside::ColumnCommTimestamp:  // most recent communication
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            foreach (const SeasideCustomDetail& detail,
                     contact->details<SeasideCustomDetail>())
                return QVariant(detail.commTimestamp());
            return QVariant();
        }

    case Seaside::ColumnCommType:  // recent communication type
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            foreach (const SeasideCustomDetail& detail,
                     contact->details<SeasideCustomDetail>())
                return QVariant(detail.commType());
            return QVariant();
        }

    case Seaside::ColumnCommLocation:  // recent communication location
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            foreach (const SeasideCustomDetail& detail,
                     contact->details<SeasideCustomDetail>())
                return QVariant(detail.commLocation());
            return QVariant();
        }

    default:
        qWarning() << "[SyncModel] request for data on unexpected column" <<
                index.column();
        return QVariant();
    }
}

QVariant SeasideListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);

    if (role == Seaside::SearchRole) {
        if (section < priv->headers.size()) {
            return QVariant(priv->headers.at(section));
        }
    }
    return QVariant();
}

void SeasideListModel::fixIndexMap()
{
    int i=0;
    priv->idToIndex.clear();
    foreach (const QContactLocalId& id, priv->contactIds)
        priv->idToIndex.insert(id, i);
}

void SeasideListModel::addContacts(const QList<QContactLocalId>& contactIds) {
    int size = priv->contactIds.size();

    foreach (const QContactLocalId& id, contactIds) {
        priv->contactIds.push_back(id);
        priv->idToIndex.insert(id, size);
        QContact *contact = new QContact(priv->manager->contact(id));
        priv->idToContact.insert(id, contact);

        QContactGuid guid = contact->detail<QContactGuid>();
        if (!guid.isEmpty()) {
            QUuid uuid(guid.guid());
            priv->uuidToId.insert(uuid, id);
            priv->idToUuid.insert(id, uuid);
        }
    }
}

void SeasideListModel::contactsAdded(const QList<QContactLocalId>& contactIds)
{
    qDebug() << "[ListModel] contacts added:" << contactIds;
    int size = priv->contactIds.size();
    int added = contactIds.size();

    beginInsertRows(QModelIndex(), size, size + added - 1);
    addContacts(contactIds);
    endInsertRows();
}

void SeasideListModel::contactsChanged(const QList<QContactLocalId>& contactIds)
{
    qDebug() << "[SyncModel] contacts changed:" << contactIds;
    // NOTE: this implementation sends one dataChanged signal with the minimal range
    //   that covers all the changed contacts, but it could be more efficient to send
    //   multiple dataChanged signals, though more work to find them
    int min = priv->contactIds.size();
    int max = 0;

    foreach (const QContactLocalId& id, contactIds) {
        int index = priv->idToIndex.value(id);
        if (index < min)
            min = index;
        if (index > max)
            max = index;
        // FIXME: this looks like it may be wrong, could lead to multiple entries
        *priv->idToContact[id] = priv->manager->contact(id);
    }

    // FIXME: unfortunate that we can't easily identify what changed
    if (min <= max)
        emit dataChanged(index(min, 0), index(max, Seaside::ColumnLast));
}

void SeasideListModel::contactsRemoved(const QList<QContactLocalId>& contactIds)
{
    qDebug() << "[SyncModel] contacts removed:" << contactIds;
    // FIXME: the fact that we're only notified after removal may mean that we must
    //   store the full contact in the model, because the data could be invalid
    //   when the view goes to access it

    QList<int> removed;
    foreach (const QContactLocalId& id, contactIds)
        removed.push_front(priv->idToIndex.value(id));
    qSort(removed);

    // NOTE: this could check for adjacent rows being removed and send fewer signals
    int size = removed.size();
    for (int i=0; i<size; i++) {
        // remove in reverse order so the other index numbers will not change
        int index = removed.takeLast();
        beginRemoveRows(QModelIndex(), index, index);
        QContactLocalId id = priv->contactIds.takeAt(index);
        delete priv->idToContact[id];
        priv->idToContact.remove(id);
        priv->idToIndex.remove(id);

        QUuid uuid = priv->idToUuid[id];
        if (!uuid.isNull()) {
            priv->idToUuid.remove(id);
            priv->uuidToId.remove(uuid);
        }
        endRemoveRows();
    }
    fixIndexMap();
}

void SeasideListModel::dataReset()
{
    qDebug() << "[SyncModel] data reset";
    beginResetModel();

    foreach (QContact *contact, priv->idToContact.values())
        delete contact;

    priv->contactIds.clear();
    priv->idToContact.clear();
    priv->idToIndex.clear();
    priv->uuidToId.clear();
    priv->idToUuid.clear();

    addContacts(priv->manager->contactIds(priv->currentFilter));

    endResetModel();
}
