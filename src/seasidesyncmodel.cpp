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
#include <QSettings>

#include <MTheme>

#include "seasidesyncmodel.h"
#include "seaside.h"
#include "seasidedetail.h"
#include "seasidepersonmodel.h"

class SeasideSyncModelPriv
{
public:
    static SeasideSyncModel *theSyncModel;
    static int theRefCount;

    // FIXME: temporary
    static bool generateSampleData;
    static QString theEngine;

    QContactManager *manager;
    QContactFilter currentFilter;
    QList<QContactLocalId> contactIds;
    QMap<QContactLocalId, int> idToIndex;
    QMap<QContactLocalId, QContact *> idToContact;
    QMap<QUuid, QContactLocalId> uuidToId;
    QMap<QContactLocalId, QUuid> idToUuid;

    QVector<QStringList> data;
    QStringList headers;

    QSettings *settings;
};

// FIXME: <temporary>
bool SeasideSyncModelPriv::generateSampleData = false;
QString SeasideSyncModelPriv::theEngine = "default";
// FIXME: </temporary>

SeasideSyncModel *SeasideSyncModelPriv::theSyncModel = NULL;
int SeasideSyncModelPriv::theRefCount = 0;

void SeasideSyncModel::setGenerateSampleData(bool generate)
{
    SeasideSyncModelPriv::generateSampleData = generate;
}

void SeasideSyncModel::setEngine(const QString& engine)
{
    SeasideSyncModelPriv::theEngine = engine;
}

SeasideSyncModel *SeasideSyncModel::instance()
{
    if (!SeasideSyncModelPriv::theSyncModel)
        SeasideSyncModelPriv::theSyncModel = new SeasideSyncModel;
    SeasideSyncModelPriv::theRefCount++;
    return SeasideSyncModelPriv::theSyncModel;
}

void SeasideSyncModel::releaseInstance()
{
    if (SeasideSyncModelPriv::theRefCount > 0)
        SeasideSyncModelPriv::theRefCount--;
    if (!SeasideSyncModelPriv::theRefCount && SeasideSyncModelPriv::theSyncModel) {
        delete SeasideSyncModelPriv::theSyncModel;
        SeasideSyncModelPriv::theSyncModel = NULL;
    }
}

// FIXME: <temporary>
typedef struct {
  const char *first;
  const char *last;
} PeopleData;

static PeopleData people_data[] = {
    { "Aaron",   "Alewife"     },
    { "Harold",  "Hansen"      },
    { "Hayat",   "Harwood",    },
    { "Neal",    "Nagarajan",  },
    { "Teresa",  "Thyme"       },
};

static QString oneOf(QList<const char *> strings)
{
    int count = strings.count();
    if (count > 0)
        return QString(strings.at(qrand() % count));
    return QString();
}

static QString generateEmail(const char *firstName)
{
    QString str(firstName);
    int num = qrand() % 20;
    if (num)
        str += QString().setNum(num);
    str += "@example.";
    str += oneOf(QList<const char *>() << "com" << "net" << "org");
    return str.toLower();
}

static QString generateStreet()
{
    QString address;
    address.setNum(qrand() % 1990 + 10);

    QList<const char *> strings;
    strings << "25th Ave" << "Massachusetts Ave" << "Pennsylvania Ave" << "Downing St";
    return address + " " + oneOf(strings);
}

static QString generateCity()
{
    QList<const char *> strings;
    strings << "Hillsboro" << "Cambridge" << "Washington" << "London";
    return oneOf(strings);
}

static QString generateState()
{
    QList<const char *> strings;
    strings << "OR" << "MA" << "DC";
    return oneOf(strings);
}

static QString generateZip()
{
    QString zip;
    zip = QString("%1").arg(qrand() % 100000, 5, 10, QChar('0'));
    if (qrand() % 2)
        zip += QString("-%1").arg(qrand() % 10000, 4, 10, QChar('0'));
    return zip;
}

static QString generatePhoneNumber(int areaCode)
{
    return QString("(%1) 555-%2").arg(areaCode).arg(qrand() % 9000 + 1000);
}

static QDateTime generateRecentTimestamp()
{
    if (qrand() % 2)
        return QDateTime();
    QDateTime time = QDateTime::currentDateTime();
    time = time.addDays(-(qrand() % 60));
    return time.addSecs(-(qrand() % (24 * 60 * 60)));
}
// FIXME: </temporary>

static void updateDefinitions(QContactManager *manager) {
    QContactDetailDefinition seaside;

    QContactDetailFieldDefinition favorite;
    favorite.setDataType(QVariant::Bool);
    seaside.insertField(SeasideCustomDetail::FieldFavorite, favorite);

    QContactDetailFieldDefinition commTimestamp;
    commTimestamp.setDataType(QVariant::DateTime);
    seaside.insertField(SeasideCustomDetail::FieldCommTimestamp, commTimestamp);

    QContactDetailFieldDefinition commType;
    commType.setDataType(QVariant::Int);
    seaside.insertField(SeasideCustomDetail::FieldCommType, commType);

    QContactDetailFieldDefinition commLocation;
    commLocation.setDataType(QVariant::Int);
    seaside.insertField(SeasideCustomDetail::FieldCommLocation, commLocation);

    seaside.setName(SeasideCustomDetail::DefinitionName);
    seaside.setUnique(true);
    if (!manager->saveDetailDefinition(seaside))
        qDebug() << "[SyncModel] failed to save new detail definition";
}

SeasideSyncModel::SeasideSyncModel()
{
    priv = new SeasideSyncModelPriv;

    priv->headers.append("First Name");
    priv->headers.append("Last Name");
    priv->headers.append("Company");
    priv->headers.append("Birthday");
    priv->headers.append("Anniversary");
    priv->headers.append("Avatar");
    priv->headers.append("Favorite");
    priv->headers.append("Email Addresses");
    priv->headers.append("Phone Numbers");
    priv->headers.append("Phone Contexts");
    priv->headers.append("Phone Types");
    priv->headers.append("Addresses");
    priv->headers.append("Address Contexts");
    priv->headers.append("Presence");
    priv->headers.append("Uuid");
    priv->headers.append("CommTimestamp");
    priv->headers.append("CommType");
    priv->headers.append("CommLocation");

    // dropping for now:
    //   unread email flag
    //   unread sms flag

    // FIXME: temporary hack to provide images to dialer and sms
    MTheme::addPixmapDirectory(IMAGES_DIR);

    if (SeasideSyncModelPriv::theEngine == "default") {
        if (QContactManager::availableManagers().contains("tracker")) {
            priv->manager = new QContactManager("tracker");
            if (priv->manager->contactIds().count() == 0)
                SeasideSyncModelPriv::generateSampleData = true;
        }
        else if (QContactManager::availableManagers().contains("memory")) {
            priv->manager = new QContactManager("memory");
            SeasideSyncModelPriv::generateSampleData = true;
        }
        else
            priv->manager = new QContactManager("");
    }
    else
        priv->manager = new QContactManager(SeasideSyncModelPriv::theEngine);

    priv->settings = new QSettings("MeeGo", "libseaside");

    updateDefinitions(priv->manager);

    if (SeasideSyncModelPriv::generateSampleData) {
        // FIXME: make sure the same contact details get generated every time [DEBUG]
        qsrand(0);

        int rows = sizeof(people_data) / sizeof(PeopleData);
        for (int i=0; i<rows; i++)
            createFakeContact(people_data[i].first, people_data[i].last);
        if (priv->settings)
            priv->settings->sync();
    }

    connect(priv->manager, SIGNAL(contactsAdded(QList<QContactLocalId>)),
            this, SLOT(contactsAdded(QList<QContactLocalId>)));
    connect(priv->manager, SIGNAL(contactsChanged(QList<QContactLocalId>)),
            this, SLOT(contactsChanged(QList<QContactLocalId>)));
    connect(priv->manager, SIGNAL(contactsRemoved(QList<QContactLocalId>)),
            this, SLOT(contactsRemoved(QList<QContactLocalId>)));
    connect(priv->manager, SIGNAL(dataChanged()), this, SLOT(dataReset()));

    dataReset();
}

SeasideSyncModel::~SeasideSyncModel()
{
    foreach (QContact *contact, priv->idToContact.values())
        delete contact;
    delete priv->manager;
    delete priv;
}

int SeasideSyncModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return priv->contactIds.size();
}

int SeasideSyncModel::columnCount(const QModelIndex& parent) const
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

QVariant SeasideSyncModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Seaside::DataRole)
        return QVariant();

    QContactLocalId id = priv->contactIds[index.row()];
    QContact *contact = priv->idToContact[id];

    if (!contact)
        return QVariant();

    // expect searching on DisplayRole, only return text that makes sense to search

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

    case Seaside::ColumnCompany:  // company
        {
            QContactOrganization company = contact->detail<QContactOrganization>();
            //return QVariant(company.name());
	    if (role == Qt::DisplayRole)
                return QVariant(company.name());
            if (priv->settings) {
                QContactGuid guid = contact->detail<QContactGuid>();
                QString key = guid.guid();
                key += "/company";
                return priv->settings->value(key, company.name());
            }
            return QVariant(company.name());

        }

    case Seaside::ColumnBirthday:  // birthday
        {
            QContactBirthday day = contact->detail<QContactBirthday>();
            if (role == Qt::DisplayRole)
                return QVariant(day.date().toString("MMMM dd yyyy"));
            else
                return QVariant(day.date());
        }

    case Seaside::ColumnAnniversary:  // anniversary
        {
            QContactAnniversary day = contact->detail<QContactAnniversary>();
            if (role == Qt::DisplayRole)
                return QVariant(day.originalDate().toString("MMMM dd yyyy"));
            else
                return QVariant(day.originalDate());
        }

    case Seaside::ColumnAvatar:  // avatar
        {
            QContactAvatar avatar = contact->detail<QContactAvatar>();
            // almost conceivable someone could want to search by the avatar filename,
            //   so I'll leave this on DisplayRole for the moment
            return QVariant(avatar.imageUrl().toString());
        }

    case Seaside::ColumnFavorite:  // favorite
        {
            if (role == Qt::DisplayRole)
                return QVariant();
            if (priv->settings) {
                QContactGuid guid = contact->detail<QContactGuid>();
                QString key = guid.guid();
                key += "/favorite";
                return priv->settings->value(key, false);
            }
            return QVariant(false);
        }

    case Seaside::ColumnEmailAddresses:  // emails
        {
            QStringList list;
            foreach (const QContactEmailAddress& email,
                     contact->details<QContactEmailAddress>())
                list << email.emailAddress();
            if (role == Qt::DisplayRole)
                return QVariant(list.join(" "));
            else
                return QVariant(list);
        }

    case Seaside::ColumnPhoneNumbers:  // phones
        {
            QStringList list;
            foreach (const QContactPhoneNumber& phone,
                     contact->details<QContactPhoneNumber>()) {
                // Tracker backend throws out formatting characters, so this
                // is a temporary fix to make them look nicer, but it means
                // hard-coding American-style phone numbers

                // TODO: i18n
                QString number = phone.number().replace("[^0-9]", "");
                int count = number.count();
                if (count == phone.number().count()) {
                    if (count == 10) {
                        list << QString("(%1) %2-%3").
                                arg(number.left(3)).
                                arg(number.mid(3, 3)).
                                arg(number.right(4));
                    }
                    else if (count == 7) {
                        list << QString("%1-%3").
                                arg(number.left(3)).
                                arg(number.right(4));
                    }
                    else
                        list << number;
                }
                else
                    list << phone.number();
            }
            if (role == Qt::DisplayRole) {
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
            if (role == Qt::DisplayRole)
                return QVariant();
            return getContextList(contact->details(QContactPhoneNumber::DefinitionName));
        }

    case Seaside::ColumnPhoneTypes:  // phone types
        {
            if (role == Qt::DisplayRole)
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
                list << address.street() + "\n" + address.locality() + "\n" +
                        address.region() + "\n" + address.postcode() + "\n" +
                        address.country();
            }
            if (role == Qt::DisplayRole)
                return QVariant(list.join(" "));
            else
                return QVariant(list);
        }

    case Seaside::ColumnAddressContexts:  // address contexts
        {
            if (role == Qt::DisplayRole)
                return QVariant();
            return getContextList(contact->details(QContactAddress::DefinitionName));
        }

    case Seaside::ColumnPresence:  // presence
        {
            if (role == Qt::DisplayRole)
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
            if (role == Qt::DisplayRole)
                return QVariant();
            QContactGuid guid = contact->detail<QContactGuid>();
            return QVariant(guid.guid());
        }

    case Seaside::ColumnCommTimestamp:  // most recent communication
        {
            if (role == Qt::DisplayRole)
                return QVariant();
            foreach (const SeasideCustomDetail& detail,
                     contact->details<SeasideCustomDetail>())
                return QVariant(detail.commTimestamp());
            return QVariant();
        }

    case Seaside::ColumnCommType:  // recent communication type
        {
            if (role == Qt::DisplayRole)
                return QVariant();
            foreach (const SeasideCustomDetail& detail,
                     contact->details<SeasideCustomDetail>())
                return QVariant(detail.commType());
            return QVariant();
        }

    case Seaside::ColumnCommLocation:  // recent communication location
        {
            if (role == Qt::DisplayRole)
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

QVariant SeasideSyncModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);

    if (role == Qt::DisplayRole) {
        if (section < priv->headers.size()) {
            return QVariant(priv->headers.at(section));
        }
    }
    return QVariant();
}

QContact *SeasideSyncModel::contact(int row)
{
    // TODO: validate
    QContactLocalId id = priv->contactIds[row];
    return priv->idToContact[id];
}

QContactManager *SeasideSyncModel::manager()
{
    return priv->manager;
}

// FIXME: <temporary>
void SeasideSyncModel::createFakeContact(const char *firstName, const char *lastName)
{
    if (!SeasideSyncModelPriv::generateSampleData)
        return;

    QContact contact;

    QContactGuid guid;
    guid.setGuid(QUuid::createUuid().toString());
    if (!contact.saveDetail(&guid))
        qWarning() << "[SyncModel] failed to save guid";

    QContactName name;
    name.setFirstName(firstName);
    name.setLastName(lastName);
    if (!contact.saveDetail(&name))
        qWarning() << "[SyncModel] failed to save name";

    QContactAvatar avatar;
    avatar.setImageUrl(QUrl("icon-m-content-avatar-placeholder"));
    if (!contact.saveDetail(&avatar))
        qWarning() << "[SyncModel] failed to save avatar";

    for (int i=0; i<3; i++) {
        if (qrand() % 2) {
            QContactEmailAddress email;
            email.setEmailAddress(generateEmail(firstName));
            if (!contact.saveDetail(&email))
                qWarning() << "[SyncModel] failed to save email";
        }
    }

    if (qrand() % 2) {
        /*QContactOrganization company;
        company.setName("Intel Corporation");
        if (!contact.saveDetail(&company))
	  qWarning() << "[SyncModel] failed to save company";*/
      if (priv->settings) {
	  QString key = guid.guid();
	  key += "/company";
	  priv->settings->setValue(key, QString("Intel Corp"));
      }
    }	

    for (int j=qrand()%3; j>0; j--) {
        QContactAddress address;
        address.setStreet(generateStreet());
        address.setLocality(generateCity());
        if (qrand() % 4) {
            address.setRegion(generateState());
            if (qrand() % 2)
                address.setCountry("USA");
            address.setPostcode(generateZip());
        }
        else
            address.setCountry("UK");
        address.setContexts(qrand() % 2 ?
                            QContactDetail::ContextHome :
                            QContactDetail::ContextWork);
        if (!contact.saveDetail(&address))
            qWarning() << "[SyncModel] failed to save address";
    }

    int areaCode = qrand() % 700;
    if (areaCode < 200)
        areaCode = 503;
    for (int j=qrand()%4; j>0; j--) {
        QContactPhoneNumber phone;
        phone.setNumber(generatePhoneNumber(areaCode));
        phone.setContexts(qrand() % 2 ?
                          QContactDetail::ContextHome :
                          QContactDetail::ContextWork);
        if (qrand() % 2)
            phone.setSubTypes(QContactPhoneNumber::SubTypeLandline);
        else
            phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
        if (!contact.saveDetail(&phone))
            qWarning() << "[SyncModel] failed to save phone";
    }

    for (int j=0; j<2; j++) {
        if (qrand() % 8 == 0) {
            QContactPresence presence;
            int state = qrand() % 3;
            if (state == 0)
                presence.setPresenceState(QContactPresence::PresenceAvailable);
            else if (state == 1)
                presence.setPresenceState(QContactPresence::PresenceAway);
            else
                presence.setPresenceState(QContactPresence::PresenceOffline);
            if (!contact.saveDetail(&presence))
                qWarning() << "[SyncModel] failed to save online account";
        }
    }

    // percent chance to make contact a Favorite
    const int percentFavorite = 20;
    bool favorite = (qrand() % 100 < percentFavorite) ? true : false;
    if (priv->settings) {
        QString key = guid.guid();
        key += "/favorite";
        priv->settings->setValue(key, favorite);
    }

    // add the seaside custom detail
    SeasideCustomDetail sd;
    sd.setCommTimestamp(generateRecentTimestamp());
    sd.setCommType(qrand() % Seaside::CommNone);
    sd.setCommLocation(qrand() % Seaside::LocationNone);
    if (!contact.saveDetail(&sd))
        qWarning() << "[SyncModel] failed to save seaside detail";

    if (!priv->manager->saveContact(&contact))
        qWarning() << "[SyncModel] failed to save contact";
}

void SeasideSyncModel::addRandom()
{
    if (!SeasideSyncModelPriv::generateSampleData)
        return;

    int rows = sizeof(people_data) / sizeof(PeopleData);
    createFakeContact(people_data[qrand() % rows].first,
                      people_data[qrand() % rows].last);
}

void SeasideSyncModel::removeRandom()
{
    if (!SeasideSyncModelPriv::generateSampleData)
        return;

    int rows = priv->contactIds.size();
    if (rows > 0)
        priv->manager->removeContact(priv->contactIds[qrand() % rows]);
}

void SeasideSyncModel::fixIndexMap()
{
    int i=0;
    priv->idToIndex.clear();
    foreach (const QContactLocalId& id, priv->contactIds)
        priv->idToIndex.insert(id, i++);
}

void SeasideSyncModel::addContacts(const QList<QContactLocalId>& contactIds) {
    int size = priv->contactIds.size();

    foreach (const QContactLocalId& id, contactIds) {
        priv->contactIds.push_back(id);
        priv->idToIndex.insert(id, size++);
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

void SeasideSyncModel::contactsAdded(const QList<QContactLocalId>& contactIds)
{
    qDebug() << "[SyncModel] contacts added:" << contactIds;
    int size = priv->contactIds.size();
    int added = contactIds.size();

    beginInsertRows(QModelIndex(), size, size + added - 1);
    addContacts(contactIds);
    endInsertRows();
}

void SeasideSyncModel::contactsChanged(const QList<QContactLocalId>& contactIds)
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
        QContact *contact = priv->idToContact[id];
        if (contact)
            *contact = priv->manager->contact(id);
    }

    // FIXME: unfortunate that we can't easily identify what changed
    if (min <= max)
        emit dataChanged(index(min, 0), index(max, Seaside::ColumnLast));
}

void SeasideSyncModel::contactsRemoved(const QList<QContactLocalId>& contactIds)
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

void SeasideSyncModel::dataReset()
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

SeasidePersonModel *SeasideSyncModel::createPersonModel(const QModelIndex& index)
{
    SeasidePersonModel *model = new SeasidePersonModel;

    if (!index.isValid())
        return model;

    // create a person model with all details for this row
    QVector<SeasideDetail> details;
    QStringList list, contexts, types;
    int size;

    SEASIDE_SHORTCUTS
    SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row());

    // handle basic information
    model->setUuid(QUuid(SEASIDE_FIELD(Uuid, String)));
    model->setFirstName(SEASIDE_FIELD(FirstName, String));
    model->setLastName(SEASIDE_FIELD(LastName, String));
    model->setCompany(SEASIDE_FIELD(Company, String));
    model->setThumbnail(SEASIDE_FIELD(Avatar, String));

    QDateTime timestamp = SEASIDE_FIELD(CommTimestamp, DateTime);
    if (timestamp.isValid()) {
        SeasideCommEvent event;
        event.setDateTime(timestamp);
        // TODO: get real data, not random
        event.setType((Seaside::CommType)SEASIDE_FIELD(CommType, Int));
        event.setLocation((Seaside::Location)SEASIDE_FIELD(CommLocation, Int));
        QList<SeasideCommEvent> list;
        list.append(event);
        model->setEvents(list);
    }

    // handle phone numbers
    list = SEASIDE_FIELD(PhoneNumbers, StringList);
    contexts = SEASIDE_FIELD(PhoneContexts, StringList);
    types = SEASIDE_FIELD(PhoneTypes, StringList);
    size = list.size();

    for (int i=0; i<size; i++) {
        Seaside::Location location = Seaside::LocationHome;
        if (contexts[i] == "Work")
            location = Seaside::LocationWork;
        if (types[i] == "Mobile")
            location = Seaside::LocationMobile;
        SeasideDetail phone(list[i], location);
        details.append(phone);
    }
    model->setPhones(details);

    // handle emails
    details.clear();
    foreach (QString email, SEASIDE_FIELD(EmailAddresses, StringList)) {
        SeasideDetail detail(email, Seaside::LocationHome);
        details.append(detail);
    }
    model->setEmails(details);

    // handle addresses
    details.clear();
    list = SEASIDE_FIELD(Addresses, StringList);
    contexts = SEASIDE_FIELD(AddressContexts, StringList);
    size = list.size();
    for (int i=0; i<size; i++) {
        // FIXME: address in person model is stored with HTML line breaks
        SeasideDetail address(list[i], contexts[i] == "Work" ?
                              Seaside::LocationWork : Seaside::LocationHome);
        details.append(address);
    }
    model->setAddresses(details);

    model->setPresence((Seaside::Presence)(SEASIDE_FIELD(Presence, Int)));
    model->setFavorite(SEASIDE_FIELD(Favorite, Bool));

    return model;
}

SeasidePersonModel *SeasideSyncModel::createPersonModel(const QUuid& uuid)
{
    QContactLocalId id = priv->uuidToId[uuid];
    QModelIndex personIndex = index(priv->idToIndex[id], 0);
    return createPersonModel(personIndex);
}

void SeasideSyncModel::deletePerson(const QUuid& uuid)
{
    if (!priv->manager->removeContact(priv->uuidToId[uuid]))
      qWarning() << "[SyncModel] failed to delete contact"; 
    //TODO: Test deleting contacts with only one field entered and Others group
}

void SeasideSyncModel::updatePerson(const SeasidePersonModel *newModel)
{
    QContactLocalId id = priv->uuidToId[newModel->uuid()];
    QContact *contact = priv->idToContact[id];
    if (!contact) {
        // we are creating a new contact
        contact = new QContact;

        QContactGuid guid;
        guid.setGuid(QUuid::createUuid().toString());
        if (!contact->saveDetail(&guid))
            qWarning() << "[SyncModel] failed to save guid in new contact";

        QContactAvatar avatar;
        avatar.setImageUrl(QUrl("icon-m-content-avatar-placeholder"));
        if (!contact->saveDetail(&avatar))
            qWarning() << "[SyncModel] failed to save avatar in new contact";

        // add the custom seaside detail
        SeasideCustomDetail sd;
        if (!contact->saveDetail(&sd))
            qWarning() << "[SyncModel] failed to save seaside detail in new contact";
    }
    SeasidePersonModel *oldModel = createPersonModel(newModel->uuid());

    const QString& newFirstName = newModel->firstname();
    const QString& newLastName = newModel->lastname();
    if ((oldModel->firstname() != newFirstName) || (oldModel->lastname() != newLastName)) {
        QContactName name = contact->detail<QContactName>();
        name.setFirstName(newFirstName);
	name.setLastName(newLastName);
        name.setMiddleName("");
        name.setPrefix("");
        name.setSuffix("");
        if (!contact->saveDetail(&name))
            qWarning() << "[SyncModel] failed to update name";
    }
 
    const QString& newCompany = newModel->company();
    if (oldModel->company() != newCompany) {
        QContactOrganization company = contact->detail<QContactOrganization>();
	/* if (!company.name().isEmpty()) {
            if (!contact->removeDetail(&company))
                qWarning() << "[SyncModel] failed to remove company";
        }

        if (!newModel->company().isEmpty()) {
            company.setName(newCompany);

            if (!contact->saveDetail(&company))
                qWarning() << "[SyncModel] failed to update company";
		}*/
	if (priv->settings) {
            QContactGuid guid = contact->detail<QContactGuid>();
            QString key = guid.guid();
            key += "/company";
            priv->settings->setValue(key, newCompany);
            priv->settings->sync();
        }
    }

    const QVector<SeasideDetail>& newPhones = newModel->phones();
    if (oldModel->phones() != newPhones) {
        foreach (QContactDetail detail, contact->details<QContactPhoneNumber>())
            if (!contact->removeDetail(&detail))
                qWarning() << "[SyncModel] failed to remove phone number";

        foreach (const SeasideDetail& detail, newPhones) {
            if (detail.isValid()) {
                QContactPhoneNumber phone;
                phone.setNumber(detail.text());
                phone.setContexts(detail.location() == Seaside::LocationWork ?
                                  QContactDetail::ContextWork :
                                  QContactDetail::ContextHome);
                if (detail.location() == Seaside::LocationMobile)
                    phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
                else
                    phone.setSubTypes(QContactPhoneNumber::SubTypeLandline);

                if (!contact->saveDetail(&phone))
                    qWarning() << "[SyncModel] failed to save phone number";
            }
        }
    }

    const QVector<SeasideDetail>& newEmails = newModel->emails();
    if (oldModel->emails() != newEmails) {
        foreach (QContactDetail detail, contact->details<QContactEmailAddress>())
            if (!contact->removeDetail(&detail))
                qWarning() << "[SyncModel] failed to remove email address";

        foreach (const SeasideDetail& detail, newEmails) {
            if (detail.isValid()) {
                QContactEmailAddress email;
                email.setEmailAddress(detail.text());

                if (!contact->saveDetail(&email))
                    qWarning() << "[SyncModel] failed to save email address";
            }
        }
    }

    const QVector<SeasideDetail>& newAddresses = newModel->addresses();
    if (oldModel->addresses() != newAddresses) {
        foreach (QContactDetail detail, contact->details<QContactAddress>())
            if (!contact->removeDetail(&detail))
                qWarning() << "[SyncModel] failed to remove phone number";

        foreach (const SeasideDetail& detail, newAddresses) {
            if (detail.isValid()) {
                QContactAddress address;
                address.setContexts(detail.location() == Seaside::LocationWork ?
                                    QContactDetail::ContextWork :
                                    QContactDetail::ContextHome);

                QStringList list = detail.text().split(QChar('\n'));
                while (list.count() < 5)
                    list << "";
                address.setStreet(list[0]);
                address.setLocality(list[1]);
                address.setRegion(list[2]);
                address.setPostcode(list[3]);
                address.setCountry(list[4]);

                if (!contact->saveDetail(&address))
                    qWarning() << "[SyncModel] failed to save address";
            }
        }
    }

    if (oldModel->favorite() != newModel->favorite()) {
        if (priv->settings) {
            QContactGuid guid = contact->detail<QContactGuid>();
            QString key = guid.guid();
            key += "/favorite";
            priv->settings->setValue(key, newModel->favorite());
            priv->settings->sync();
        }
    }

    if (!priv->manager->saveContact(contact)) {
        qWarning() << "[SyncModel] failed to update contact";

        // make sure data shown to user matches what is really in the database
        *contact = priv->manager->contact(id);
    }

    delete oldModel;
}

void SeasideSyncModel::setAvatar(const QUuid& uuid, const QString& path)
{
    QContactLocalId id = priv->uuidToId[uuid];
    QContact *contact = priv->idToContact[id];
    if (!contact)
        return;

    QContactAvatar avatar = contact->detail<QContactAvatar>();
    if (path.isEmpty())
        avatar.setImageUrl(QUrl("icon-m-content-avatar-placeholder"));
    else
        avatar.setImageUrl(QUrl(path));
    if (!contact->saveDetail(&avatar))
        qWarning() << "[SyncModel] failed to save avatar";

    if (!priv->manager->saveContact(contact))
        qWarning() << "[SyncModel] failed to save contact while setting avatar";
}

void SeasideSyncModel::setFavorite(const QUuid& uuid, bool favorite)
{
    if (!priv->settings)
        return;

    QString key = uuid.toString();
    key += "/favorite";
    priv->settings->setValue(key, favorite);
    priv->settings->sync();

    QList<QContactLocalId> list;
    list.append(priv->uuidToId[uuid]);

    // writing to QSettings doesn't cause a change event, so manually call
    contactsChanged(list);
}

void SeasideSyncModel::setCompany(const QUuid& uuid, QString company)
{
    if (!priv->settings)
        return;

    QString key = uuid.toString();
    key += "/company";
    priv->settings->setValue(key, company);
    priv->settings->sync();

    QList<QContactLocalId> list;
    list.append(priv->uuidToId[uuid]);

    // writing to QSettings doesn't cause a change event, so manually call
    contactsChanged(list);
}
