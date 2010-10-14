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
#include <QContactOnlineAccount>
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

SeasideSyncModel *SeasideSyncModelPriv::theSyncModel = NULL;
int SeasideSyncModelPriv::theRefCount = 0;
QString SeasideSyncModelPriv::theEngine = "default";

SeasideSyncModel *SeasideSyncModel::instance()
{
    if (!SeasideSyncModelPriv::theSyncModel)
        SeasideSyncModelPriv::theSyncModel = new SeasideSyncModel;
    SeasideSyncModelPriv::theRefCount++;
    return SeasideSyncModelPriv::theSyncModel;
}

QString SeasideSyncModel::getLocalSelfId()
{
  qWarning() << "[SyncModel] getLocalSelfId() DEPRECATED!!! This function is no longer supported. Use getSelfContactId() only.";

  if(priv->manager->hasFeature(QContactManager::SelfContact, QContactType::TypeContact)){
    return QString(priv->manager->selfContactId());
  }else{
    qWarning() << "[SyncModel] MeCard not supported";
  }
  return QString();
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
      qWarning() << "[SyncModel] failed to save new detail definition";
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
    priv->headers.append("isSelf");
    priv->headers.append("IM Accounts");
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

    // FIXME: temporary hack to provide images to dialer and sms
    MTheme::addPixmapDirectory(IMAGES_DIR);

    if (SeasideSyncModelPriv::theEngine == "default") {
      qDebug() << "[SeasideSyncModel] Engine is default";
      if (QContactManager::availableManagers().contains("tracker")) {
	priv->manager = new QContactManager("tracker");
	qDebug() << "[SeasideSyncModel] Manager is tracker";
      }
      else if (QContactManager::availableManagers().contains("memory")) {
	priv->manager = new QContactManager("memory");
	qDebug() << "[SeasideSyncModel] Manager is memory";
      }else{
	priv->manager = new QContactManager("");
	qDebug() << "[SeasideSyncModel] Manager is empty";
      }      
    }
    else
      priv->manager = new QContactManager(SeasideSyncModelPriv::theEngine);

    priv->settings = new QSettings("MeeGo", "libseaside");

    updateDefinitions(priv->manager);

  //is meCard supported by manager/engine
    if(priv->manager->hasFeature(QContactManager::SelfContact, QContactType::TypeContact))
    {     
      QContactId contactId;
      QContact meContact;
      QContactManager::Error error(QContactManager::NoError);      
      const QContactLocalId meCardId(priv->manager->selfContactId());

      //if we have a valid selfId
      if((error == QContactManager::NoError) && (meCardId != 0)){
	qDebug() << "[SeasideSyncModel] valid selfId, error" << error << "id " << meCardId;
        //check if contact with selfId exists
        meContact = priv->manager->contact(meCardId, QStringList() << QContactName::DefinitionName);

        //if contact doesn't exist, create it
        if(meContact.localId() != meCardId){
          qDebug() << "[SeasideSyncModel] self contact does not exist, create meCard with selfId. Local id: " << meContact.localId() <<"is not self id " << meCardId;
          contactId.setLocalId(meCardId);
          meContact.setId(contactId);
          if (!priv->manager->saveContact(&meContact))
            qWarning() << "[SyncModel] failed to save mecard contact";
          createMeCard(meContact);
        }else{
	  qDebug() << "SeasideSyncModel::SeasideSyncModel() id is valid and MeCard exists" << meCardId;
	  //If it does exist, check that contact has valid firstname
          meContact = priv->manager->contact(meCardId, QStringList() << QContactName::DefinitionName);
          QString firstname = meContact.detail(QContactName::DefinitionName).value(QContactName::FieldFirstName);
	  qDebug() << "[SeasideSyncModel] meCard has firstname" << firstname;
	  //if no firstname, then update meCard
	  if(firstname.isEmpty() || firstname.isNull()){
            createMeCard(meContact);
	  }else{//else do nothing
	    qDebug() << "SeasideSyncModel::SeasideSyncModel() VALID MECARD EXISTS";
	  }
        }
      }else{
	qWarning() << "[SeasideSyncModel] no valid meCard Id provided";
      }
    }else{
      qWarning() << "SeasideSyncModel::SeasideSyncModel() MeCard Not supported";
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

QModelIndex SeasideSyncModel::getModelIndex(QContactLocalId id){
  QModelIndex personIndex = index(priv->idToIndex[id], 0);
  return personIndex;
}
 
void SeasideSyncModel::viewDetails(QContactManager* cm)
{
  QList<QContactLocalId> contactIds = cm->contactIds();
  if(!contactIds.isEmpty()){
    QContact a = cm->contact(contactIds.first());
    qDebug() << "Viewing the details of" << a.displayLabel();
    qDebug() << "Contact ID: " << a.localId();

    QList<QContactDetail> allDetails = a.details();
    for (int i = 0; i < allDetails.size(); i++) {
      QContactDetail detail = allDetails.at(i);
      QContactDetailDefinition currentDefinition = cm->detailDefinition(detail.definitionName());
      QMap<QString, QContactDetailFieldDefinition> fields = currentDefinition.fields();

      qDebug("\tDetail #%d (%s):", i, detail.definitionName().toAscii().constData());
      foreach (const QString& fieldKey, fields.keys()) {
	qDebug() << "\t\t" << fieldKey << "(" << fields.value(fieldKey).dataType() << ") =" << detail.value(fieldKey);      }
      qDebug();
    }
  }
}

void SeasideSyncModel::createMeCard(QContact &card)
{
  QContact *contact = new QContact(card);
    
  QContactGuid guid;
  guid.setGuid(QUuid::createUuid().toString());
  if (!contact->saveDetail(&guid))
    qWarning() << "[SyncModel] failed to save guid in mecard contact";
  
  QContactAvatar avatar;
  avatar.setImageUrl(QUrl("icon-m-content-avatar-placeholder"));
  if (!contact->saveDetail(&avatar))
      qWarning() << "[SyncModel] failed to save avatar in mecard contact";
  
  // add the custom seaside detail
  SeasideCustomDetail sd;
  if (!contact->saveDetail(&sd))
    qWarning() << "[SyncModel] failed to save seaside detail in mecard contact";
  
  QContactName name;
  name.setFirstName(QObject::tr("Me","Default string to describe self if no self contact information found, default created with [Me] as firstname"));
  name.setLastName("");
  if (!contact->saveDetail(&name))
    qWarning() << "[SyncModel] failed to save mecard name";
  
  bool favorite = false;
  if (priv->settings) {
    QString key = guid.guid();
    key += "/favorite";
    priv->settings->setValue(key, favorite);
  }

  bool isSelf = true;
  if (priv->settings) {
    QString key = guid.guid();
    key += "/self";
    priv->settings->setValue(key, isSelf);
  }
  
  if (!priv->manager->saveContact(contact))
    qWarning() << "[SyncModel] failed to save mecard contact";

  const QContactLocalId meCardId = contact->localId();
  qDebug() << "[SyncModel] meCardId generated: " << meCardId;
}

QContactLocalId SeasideSyncModel::getSelfContactId() const
{
  if(priv->manager->hasFeature(QContactManager::SelfContact, QContactType::TypeContact)){
    return priv->manager->selfContactId();
  }else{
    qWarning() << "[SyncModel] MeCard not supported";
  }
  return QContactLocalId(0);
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
                str = QObject::tr("Home", "ContextList type for home type");
                break;
            }
            else if (context == "Work") {
                str = QObject::tr("Work", "ContextList type for work type");
                break;
            }
 	   else if (context == "Mobile") {
               str = QObject::tr("Mobile", "ContextList type for mobile type");
                break;
            }
           else if (context == "Other") {
               str = QObject::tr("Other","ContextList for other type");
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

    case Seaside::ColumnCompany:  // company
        {
            QContactOrganization company = contact->detail<QContactOrganization>();
            return QVariant(company.name());
        }

    case Seaside::ColumnBirthday:  // birthday
        {
            QContactBirthday day = contact->detail<QContactBirthday>();
            if (role != Seaside::SearchRole)
                return QVariant(day.date());
            else
                return QVariant();
        }

    case Seaside::ColumnAnniversary:  // anniversary
        {
            QContactAnniversary day = contact->detail<QContactAnniversary>();
            if (role != Seaside::SearchRole)
                return QVariant(day.originalDate());
            else
                return QVariant();
        }

    case Seaside::ColumnAvatar:  // avatar
        {
            if(role != Seaside::SearchRole){
            QContactAvatar avatar = contact->detail<QContactAvatar>();
            return QVariant(avatar.imageUrl().toString());
             }
            return QVariant();
        }

    case Seaside::ColumnFavorite:  // favorite
        {
	  //            if (role != Seaside::SearchRole) // TODO: this should always return false
          //      return QVariant();
            if (priv->settings) {
                QContactGuid guid = contact->detail<QContactGuid>();
                QString key = guid.guid();
                key += "/favorite";
                return priv->settings->value(key, false);
            }
	    //    return QVariant(false);
        }

  case Seaside::ColumnisSelf:  // self
    {  
      if (priv->settings) {
	QContactGuid guid = contact->detail<QContactGuid>();
	QString key = guid.guid();
	key += "/self";
	return priv->settings->value(key, false);
      }
    }


    case Seaside::ColumnIMAccounts:  // IMAccounts
        {
            QStringList list;
	    foreach (const QContactOnlineAccount& account, 
                     contact->details<QContactOnlineAccount>())
                list << account.accountUri() + ":" +account.value("Nickname")+":"+account.value("AccountPath") ;

	    if (role != Seaside::SearchRole)
	      return QVariant(list.join(" "));
	    else
	      return QVariant(list);
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
                     contact->details<QContactPhoneNumber>()) {
                // Tracker backend throws out formatting characters, so this
                // is a temporary fix to make them look nicer, but it means
                // hard-coding American-style phone numbers

                //qDebug() << "Seaside::ColumnPhoneNumbers phone" << phone;

                // TODO: i18n //REVISIT
                QString number = phone.number().replace("[^0-9]", "");
                int count = number.count();
                if (count == phone.number().count()) {
                    if (count == 10) {
                        list << QObject::tr("(%1) %2-%3", "Ten digit  phone number format (###) ###-####").
                                arg(number.left(3)).
                                arg(number.mid(3, 3)).
                                arg(number.right(4));
                    }
                    else if (count == 7) {
                        list << QObject::tr("%1-%3", "Seven digit phone number format ###-####").
                                arg(number.left(3)).
                                arg(number.right(4));
                    }
                    else
                        list << number;
                }
                else
                    list << phone.number();

                 qDebug() << "Seaside::ColumnPhoneNumbers list" << list;
            }
            if (role == Seaside::SearchRole) {
                QStringList searchable;
                foreach (QString number, list)
                    searchable << number.replace(QRegExp("[^0-9*#]"), "");
                searchable << list;
                qDebug() << "Seaside::ColumnPhoneNumbers searchable" << searchable;
                return QVariant(searchable.join(" "));
            }
            else{
                qDebug() << "Seaside::ColumnPhoneNumbers empty list" << list;
                return QVariant(list);

            }
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
                        str = QObject::tr("Mobile", "Phone type for mobile type");
                        break;
                    }else if (subtype == QContactPhoneNumber::SubTypeLandline) { //REVISIT
                        str = QObject::tr("Landline", "Phone type for landline type");
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
                else if (state == QContactPresence::PresenceOffline || //REVISIT
                         state == QContactPresence::PresenceHidden){
                    if (presence == Seaside::PresenceUnknown)
                        presence = Seaside::PresenceOffline;
                } 
		else if (state == QContactPresence::PresenceBusy){
			presence = Seaside::PresenceBusy;
		}
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
                index.column() << " role : " << role;
        return QVariant();
    }
}

QVariant SeasideSyncModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);

    if (role == Seaside::SearchRole) {
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
        if (contexts[i] == "Work") //REVISIT
            location = Seaside::LocationWork;
	else if (contexts[i] == "Home")
            location = Seaside::LocationHome;
	else if (contexts[i] == "Other")
            location = Seaside::LocationOther;
        if (types[i] == "Mobile")
            location = Seaside::LocationMobile;
        SeasideDetail phone(list[i], location);
        details.append(phone);
    }
    qDebug() << "SeasideSyncModel::createPersonModel list phones " << list;
    model->setPhones(details);

    //handle im accounts
    details.clear();
    foreach (QString account, SEASIDE_FIELD(IMAccounts, StringList)) {
        SeasideDetail detail(account, Seaside::LocationNone);
        details.append(detail);
    }
    model->setIMs(details);

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
    Seaside::Location location;
    for (int i=0; i<size; i++) {
        // FIXME: address in person model is stored with HTML line breaks
	if (contexts[i] == "Work") //REVISIT
            location = Seaside::LocationWork;
	else if (contexts[i] == "Home")
            location =Seaside::LocationHome;
	else if (contexts[i] == "Other")
            location = Seaside::LocationOther;
	else
            location = Seaside::LocationHome;
        SeasideDetail address(list[i], location);
        details.append(address);
    }
    model->setAddresses(details);

    model->setPresence((Seaside::Presence)(SEASIDE_FIELD(Presence, Int)));
    model->setFavorite(SEASIDE_FIELD(Favorite, Bool));
    model->setisSelf(SEASIDE_FIELD(isSelf, Bool));

    return model;
}

SeasidePersonModel *SeasideSyncModel::createPersonModel(const QUuid& uuid)
{
    QContactLocalId id = priv->uuidToId[uuid];
    QModelIndex personIndex = index(priv->idToIndex[id], 0);
    return createPersonModel(personIndex);
}

bool SeasideSyncModel::isSelfContact(const QContactLocalId id)
{
  if(id == priv->manager->selfContactId())
    return true;
  return false;
}

bool SeasideSyncModel::isSelfContact(const QUuid id){
  return isSelfContact(priv->uuidToId[id]);
}

void SeasideSyncModel::deletePerson(const QUuid& uuid)
{
  if(priv->manager->selfContactId() != priv->uuidToId[uuid])
    {
      if (!priv->manager->removeContact(priv->uuidToId[uuid]))
	qWarning() << "[SyncModel] failed to delete contact";
    }else{
    qWarning() << "[SyncModel] attempted to remove MeCard";
  }
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
        if (!company.name().isEmpty()) {
            if (!contact->removeDetail(&company))
                qDebug() << "[SyncModel] failed to remove company";
        }

        if (!newModel->company().isEmpty()) {
            company.setName(newCompany);

            if (!contact->saveDetail(&company))
                qDebug() << "[SyncModel] failed to update company";
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
 		if(detail.location() == Seaside::LocationWork)
			phone.setContexts(QContactDetail::ContextWork); //REVISIT
		else if(detail.location() == Seaside::LocationHome)
			phone.setContexts(QContactDetail::ContextHome);
		else if(detail.location() == Seaside::LocationOther)
			phone.setContexts(QContactDetail::ContextOther);
		else
			phone.setContexts(QContactDetail::ContextHome);//default
                if (detail.location() == Seaside::LocationMobile)
                    phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
                else
                    phone.setSubTypes(QContactPhoneNumber::SubTypeLandline);

                if (!contact->saveDetail(&phone))
                    qWarning() << "[SyncModel] failed to save phone number";

		foreach (const QContactPhoneNumber& phone,
                     contact->details<QContactPhoneNumber>())            
                qWarning() << "[SyncModel] to save phone number" << phone;

            }
        }
    }

    const QVector<SeasideDetail>& newIMs = newModel->ims();
    if (oldModel->ims() != newIMs) {
        foreach (QContactDetail detail, contact->details<QContactOnlineAccount>())
            if (!contact->removeDetail(&detail))
                qWarning() << "[SyncModel] failed to remove im account";

        foreach (const SeasideDetail& detail, newIMs) {
            if (detail.isValid()) {
                QContactOnlineAccount imAccount;
                QStringList list = (detail.text()).split(":");
                if(list.count() != 3)
                     qWarning() << "[SyncModel] im detail missing fields";
                imAccount.setValue("AccountPath", list.at(2));
                 imAccount.setAccountUri(list.at(0));
                 imAccount.setValue("Nickname", list.at(1));
                if (!contact->saveDetail(&imAccount))
                    qWarning() << "[SyncModel] failed to save im account";
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
                qWarning() << "[SyncModel] failed to remove addresses";

        foreach (const SeasideDetail& detail, newAddresses) {
            if (detail.isValid()) {
                QContactAddress address;

		if(detail.location() == Seaside::LocationWork)
			address.setContexts(QContactDetail::ContextWork); //REVISIT
		else if(detail.location() == Seaside::LocationHome)
			address.setContexts(QContactDetail::ContextHome);
		else if(detail.location() == Seaside::LocationOther)
			address.setContexts(QContactDetail::ContextOther);
		else
			address.setContexts(QContactDetail::ContextHome);//default

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

  if (oldModel->self() != newModel->self()) {
    qDebug() << "[SeasideSyncModel] self id changed";
    if (priv->settings) {
      QContactGuid guid = contact->detail<QContactGuid>();
     QString key = guid.guid();
      key += "/self";
      priv->settings->setValue(key, newModel->self());
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
  Q_UNUSED(uuid);
  Q_UNUSED(company);
  qWarning() << "[SeasideSyncModel::setCompany] DEPRECATED FUNCTION";
}

void SeasideSyncModel::setisSelf(const QUuid& uuid, bool self)
{
  if (!priv->settings)
    return;

  QString key = uuid.toString();
  key += "/self";
  priv->settings->setValue(key, self);
  priv->settings->sync();

  QList<QContactLocalId> list;
  list.append(priv->uuidToId[uuid]);

  // writing to QSettings doesn't cause a change event, so manually call
  contactsChanged(list);
}


