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
#include <QContactFavorite>
#include <QContactGuid>
#include <QContactName>
#include <QContactNote>
#include <QContactOrganization>
#include <QContactOnlineAccount>
#include <QContactPhoneNumber>
#include <QContactPresence>
#include <QSettings>

#include <QContactLocalIdFilter>
#include <QContactManagerEngine>

#include "seasidesyncmodel.h"
#include "seasidesyncmodel_p.h"
#include "seaside.h"
#include "seasidedetail.h"

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
  qWarning() << Q_FUNC_INFO << "getLocalSelfId() DEPRECATED!!! This function is no longer supported. Use getSelfContactId() only.";

  if(priv->manager->hasFeature(QContactManager::SelfContact, QContactType::TypeContact)){
    return QString(priv->manager->selfContactId());
  }else{
    qWarning() << Q_FUNC_INFO << "MeCard not supported";
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

SeasideSyncModel::SeasideSyncModel()
    : priv(new SeasideSyncModelPriv(this))
{
}

QModelIndex SeasideSyncModel::getModelIndex(QContactLocalId id){
  QModelIndex personIndex = index(priv->idToIndex[id], 0);
  return personIndex;
}
 
//This is a debug function, so the synchronous calls will
//not be converted to asynchronous calls.
void SeasideSyncModel::viewDetails(QContactManager* cm)
{
  QList<QContactLocalId> contactIds = cm->contactIds();
  if(!contactIds.isEmpty()){
    QContact a = cm->contact(contactIds.first());
    qDebug() << Q_FUNC_INFO << "Viewing the details of" << a.displayLabel();
    qDebug() << Q_FUNC_INFO << "Contact ID: " << a.localId();

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

//TODO: selfContactId() is a synchronous call
//However, no an async call is not currently implemented
QContactLocalId SeasideSyncModel::getSelfContactId() const
{
  if(priv->manager->hasFeature(QContactManager::SelfContact, QContactType::TypeContact)){
    return priv->manager->selfContactId();
  }else{
    qWarning() << Q_FUNC_INFO << "MeCard not supported";
  }
  return QContactLocalId(0);
}

SeasideSyncModel::~SeasideSyncModel()
{
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
    const QContact &contact = priv->idToContact[id];

    // expect searching on SearchRole, only return text that makes sense to search
    switch (index.column()) {
    case Seaside::ColumnFirstName:  // first name
        {
            QContactName fname = contact.detail<QContactName>();
            QString strName(fname.firstName());            
            return QVariant(strName);
        }

   case Seaside::ColumnLastName:  // last name
        {
            QContactName lname = contact.detail<QContactName>();
            QString strName(lname.lastName());
            return QVariant(strName);
        }

    case Seaside::ColumnCompany:  // company
        {
            QContactOrganization company = contact.detail<QContactOrganization>();
            return QVariant(company.name());
        }

    case Seaside::ColumnBirthday:  // birthday
        {
            QContactBirthday day = contact.detail<QContactBirthday>();
            if (role != Seaside::SearchRole)
                return QVariant(day.date());
            else
                return QVariant();
        }

    case Seaside::ColumnAnniversary:  // anniversary
        {
            QContactAnniversary day = contact.detail<QContactAnniversary>();
            if (role != Seaside::SearchRole)
                return QVariant(day.originalDate());
            else
                return QVariant();
        }

    case Seaside::ColumnAvatar:  // avatar
        {
            if(role != Seaside::SearchRole){
	      QContactAvatar avatar = contact.detail<QContactAvatar>();
	      return QVariant(avatar.imageUrl().toString());
            }
            return QVariant();
        }

    case Seaside::ColumnFavorite:  // favorite
        {
            QContactFavorite favorite = contact.detail<QContactFavorite>();
            return QVariant(favorite.isFavorite());
        }

  case Seaside::ColumnisSelf:  // self
    {  
      if (priv->settings) {
	QContactGuid guid = contact.detail<QContactGuid>();
	QString key = guid.guid();
	key += "/self";
	return priv->settings->value(key, false);
      }
    }


    case Seaside::ColumnIMAccounts:  // IMAccounts
        {
            QStringList list;
	    foreach (const QContactOnlineAccount& account, 
                     contact.details<QContactOnlineAccount>())
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
                     contact.details<QContactEmailAddress>())
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
                     contact.details<QContactPhoneNumber>()) {
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

                 qDebug() << Q_FUNC_INFO << "Seaside::ColumnPhoneNumbers list" << list;
            }
            if (role == Seaside::SearchRole) {
                QStringList searchable;
                foreach (QString number, list)
                    searchable << number.replace(QRegExp("[^0-9*+#]"), "");
                searchable << list;
                qDebug() << Q_FUNC_INFO << "Seaside::ColumnPhoneNumbers searchable" << searchable;
                return QVariant(searchable.join(" "));
            }
            else{
                qDebug() << Q_FUNC_INFO << "Seaside::ColumnPhoneNumbers empty list" << list;
                return QVariant(list);

            }
        }

    case Seaside::ColumnPhoneContexts:  // phone contexts
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            return getContextList(contact.details(QContactPhoneNumber::DefinitionName));
        }

    case Seaside::ColumnPhoneTypes:  // phone types
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            QStringList list;
            foreach (const QContactPhoneNumber& phone,
                     contact.details<QContactPhoneNumber>()) {
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
                     contact.details<QContactAddress>()) {
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
            return getContextList(contact.details(QContactAddress::DefinitionName));
        }

    case Seaside::ColumnPresence:  // presence
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            // report the most available presence status found
            Seaside::Presence presence = Seaside::PresenceUnknown;
            foreach (const QContactPresence& qp,
                     contact.details<QContactPresence>()) {
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
            QContactGuid guid = contact.detail<QContactGuid>();
            return QVariant(guid.guid());
        }

    case Seaside::ColumnCommTimestamp:  // most recent communication
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            foreach (const SeasideCustomDetail& detail,
                     contact.details<SeasideCustomDetail>())
                return QVariant(detail.commTimestamp());
            return QVariant();
        }

    case Seaside::ColumnCommType:  // recent communication type
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            foreach (const SeasideCustomDetail& detail,
                     contact.details<SeasideCustomDetail>())
                return QVariant(detail.commType());
            return QVariant();
        }

    case Seaside::ColumnCommLocation:  // recent communication location
        {
            if (role == Seaside::SearchRole)
                return QVariant();
            foreach (const SeasideCustomDetail& detail,
                     contact.details<SeasideCustomDetail>())
                return QVariant(detail.commLocation());
            return QVariant();
        }

    default:
        qWarning() << Q_FUNC_INFO << "request for data on unexpected column" <<
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
    return &priv->idToContact[id];
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

void SeasideSyncModel::addContacts(const QList<QContact> contactsList, 
                                   int size)
{
    foreach (const QContact &contact, contactsList) {
        qDebug() << Q_FUNC_INFO << "Adding contact " << contact.id() << " local " << contact.localId();
        QContactLocalId id = contact.localId();

        priv->contactIds.push_back(id);
        priv->idToIndex.insert(id, size++);
        priv->idToContact.insert(id, contact);

        QContactGuid guid = contact.detail<QContactGuid>();
        if (!guid.isEmpty()) {
            QUuid uuid(guid.guid());
            priv->uuidToId.insert(uuid, id);
            priv->idToUuid.insert(id, uuid);
        }
    }
}

// FIXME
SeasidePersonModel *SeasideSyncModel::createPersonModel(const QModelIndex& index)
{
#if 0
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
    qDebug() << Q_FUNC_INFO << "list phones " << list;
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
#endif
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
    if (isSelfContact(uuid)) {
        qWarning() << Q_FUNC_INFO << "attempted to remove MeCard";
        return;
    }

    priv->removeContact(priv->uuidToId[uuid]);
}

void SeasideSyncModel::updatePerson(const QContact *contact)
{
    priv->queueContactSave(*contact);
}

// FIXME
void SeasideSyncModel::updatePerson(const SeasidePersonModel *newModel)
{
#if 0
    QContactLocalId id = priv->uuidToId[newModel->uuid()];
    QContact contact = QContact();

    if (id != 0) {
        contact = priv->idToContact[id];
    } else {
        // we are creating a new contact
        QContactGuid guid;
        guid.setGuid(QUuid::createUuid().toString());
        if (!contact.saveDetail(&guid))
            qWarning() << Q_FUNC_INFO << "failed to save guid in new contact";

        QContactAvatar avatar;
        avatar.setImageUrl(QUrl("icon-m-content-avatar-placeholder"));
        if (!contact.saveDetail(&avatar))
          qWarning() << Q_FUNC_INFO << "failed to save avatar in new contact";

        // add the custom seaside detail
        SeasideCustomDetail sd;
        if (!contact.saveDetail(&sd))
          qWarning() << Q_FUNC_INFO << "failed to save seaside detail in new contact";
    }

    SeasidePersonModel *oldModel = createPersonModel(newModel->uuid());

    const QString& newFirstName = newModel->firstname();
    const QString& newLastName = newModel->lastname();
    if ((oldModel->firstname() != newFirstName) || (oldModel->lastname() != newLastName)) {
        QContactName name = contact.detail<QContactName>();
        name.setFirstName(newFirstName);
	name.setLastName(newLastName);
        name.setMiddleName("");
        name.setPrefix("");
        name.setSuffix("");
        if (!contact.saveDetail(&name))
            qWarning() << Q_FUNC_INFO << "failed to update name";
    }
 
    const QString& newCompany = newModel->company();
    if (oldModel->company() != newCompany) {
        QContactOrganization company = contact.detail<QContactOrganization>();
        if (!company.name().isEmpty()) {
            if (!contact.removeDetail(&company))
                qDebug() << Q_FUNC_INFO << "failed to remove company";
        }

        if (!newModel->company().isEmpty()) {
            company.setName(newCompany);

            if (!contact.saveDetail(&company))
                qDebug() << Q_FUNC_INFO << "failed to update company";
        }
     }

    const QVector<SeasideDetail>& newPhones = newModel->phones();
    if (oldModel->phones() != newPhones) {
        foreach (QContactDetail detail, contact.details<QContactPhoneNumber>())
            if (!contact.removeDetail(&detail))
                qWarning() << Q_FUNC_INFO << "failed to remove phone number";   

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

                if (!contact.saveDetail(&phone))
                    qWarning() << Q_FUNC_INFO << "failed to save phone number";

		foreach (const QContactPhoneNumber& phone,
                     contact.details<QContactPhoneNumber>())            
                qWarning() << Q_FUNC_INFO << "to save phone number" << phone;

            }
        }
    }

    const QVector<SeasideDetail>& newIMs = newModel->ims();
    if (oldModel->ims() != newIMs) {
        foreach (QContactDetail detail, contact.details<QContactOnlineAccount>())
            if (!contact.removeDetail(&detail))
                qWarning() << Q_FUNC_INFO << "failed to remove im account";

        foreach (const SeasideDetail& detail, newIMs) {
            if (detail.isValid()) {
                QContactOnlineAccount imAccount;
                QStringList list = (detail.text()).split(":");
                if(list.count() != 3)
                     qWarning() << Q_FUNC_INFO << "im detail missing fields";
                imAccount.setValue("AccountPath", list.at(2));
                 imAccount.setAccountUri(list.at(0));
                 imAccount.setValue("Nickname", list.at(1));
                if (!contact.saveDetail(&imAccount))
                    qWarning() << Q_FUNC_INFO << "failed to save im account";
            }
        }
    }

    const QVector<SeasideDetail>& newEmails = newModel->emails();
    if (oldModel->emails() != newEmails) {
        foreach (QContactDetail detail, contact.details<QContactEmailAddress>())
            if (!contact.removeDetail(&detail))
                qWarning() << Q_FUNC_INFO << "failed to remove email address";

        foreach (const SeasideDetail& detail, newEmails) {
            if (detail.isValid()) {
                QContactEmailAddress email;
                email.setEmailAddress(detail.text());

                if (!contact.saveDetail(&email))
                  qWarning() << Q_FUNC_INFO << "failed to save email address";
            }
        }
    }

    const QVector<SeasideDetail>& newAddresses = newModel->addresses();
    if (oldModel->addresses() != newAddresses) {
        foreach (QContactDetail detail, contact.details<QContactAddress>())
            if (!contact.removeDetail(&detail))
                qWarning() << Q_FUNC_INFO << "failed to remove addresses";

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

                if (!contact.saveDetail(&address))
                    qWarning() << Q_FUNC_INFO << "failed to save address";
            }
        }
    }

      if (oldModel->favorite() != newModel->favorite()) {
        QContactFavorite favorite = contact.detail<QContactFavorite>();
        favorite.setFavorite(newModel->favorite());
        if (!contact.saveDetail(&favorite))
            qWarning() << "[SyncModel] failed to update favorite";
      }

  if (oldModel->self() != newModel->self()) {
    qDebug() << Q_FUNC_INFO << "self id changed";
    if (priv->settings) {
      QContactGuid guid = contact.detail<QContactGuid>();
     QString key = guid.guid();
      key += "/self";
      priv->settings->setValue(key, newModel->self());
      priv->settings->sync();
    }
  }

    priv->queueContactSave(contact);
    delete oldModel;
#endif
}

void SeasideSyncModel::setAvatar(const QUuid& uuid, const QString& path)
{
    QContactLocalId id = priv->uuidToId[uuid];
    QContact &contact = priv->idToContact[id];

    QContactAvatar avatar = contact.detail<QContactAvatar>();
    if (path.isEmpty())
        avatar.setImageUrl(QUrl("icon-m-content-avatar-placeholder"));
    else
        avatar.setImageUrl(QUrl(path));
    if (!contact.saveDetail(&avatar))
        qWarning() << Q_FUNC_INFO << "failed to save avatar";

    priv->queueContactSave(contact);
}

void SeasideSyncModel::setFavorite(const QUuid& uuid, bool favorite)
{
    QContactLocalId id = priv->uuidToId[uuid];
    QContact &contact = priv->idToContact[id];

    QContactFavorite fav= contact.detail<QContactFavorite>();
    fav.setFavorite(favorite);

    if (!contact.saveDetail(&fav)) {
        qWarning() << Q_FUNC_INFO << "failed to save favorite";
        return;
    }

    priv->queueContactSave(contact);
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

  // manually trigger a changed event
  // do it this way to prevent an extra contact fetch unnecessarily
  int row = priv->idToIndex.value(priv->uuidToId[uuid]);
  emit dataChanged(index(row, 0), index(row, Seaside::ColumnLast));
}
