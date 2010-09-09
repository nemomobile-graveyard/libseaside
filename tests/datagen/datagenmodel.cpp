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
#include <QContactManager>
#include <QContactDetailDefinition>
#include <QContactOnlineAccount>
#include <QSettings>
#include <QContactFetchRequest>
#include <QContactLocalIdFilter>
#include <QContactLocalIdFetchRequest>
#include <QContactManagerEngine>

#include "datagenmodel.h"
#include "seaside.h"
#include "seasidedetail.h"
#include "seasidepersonmodel.h"

//test mecard
static PeopleData people_data[]= {
 { "Me",   "Me", "Intel Me",
   "10-0-1969", "02-23-1998", "icon-m-content-avatar-placeholder",
   "false", "/com/gmail/me/:me:me.gmail.com", "me@test.com",
   "(323)-234-2386", "Work", "Mobile",
   "000 Me St., Hillsboro, OR, USA, 00000", "Home", "Availiable",
   "01111111-1111-1111-1111-11111111 1111", "2010-01-01 01:01:01", "CallDialed",
   "Home" },
 { "Aaron",   "Alewife", "Intel 1",
   "10-0-1971", "02-23-1999", "icon-m-content-avatar-placeholder",
   "true", "/com/gmail/test1/:test1:test1.gmail.com", "test1@test.com",
   "(323)-234-2387", "Work", "Mobile",
   "111 First St., Hillsboro, OR, USA, 11111", "Home", "Availiable",
   "GENERATE_GUID", "2010-01-01 01:01:01", "CallDialed", "Home" },
{ "Harold",  "Hansen", "Intel",
  "10-0-1972", "02-23-2000", "DEFAULT_PIC",
  "false", "/com/gmail/test2/:test2:test2.gmail.com", "test2@gmail.com",
  "3232342388","Home", "",
  "222 Second St., Hillsboro, OR, UK, 22222", "Work", "Away",
  "GENERATE_GUID", "2010-02-02 02:02:02", "CallReceived", "Work" },
 { "Hayat",   "Harwood", "Intel",
   "10-0-1973", "02-23-2001", "",
   "true", "/com/gmail/test3/:test3:test3.gmail.com", "test3@gmail.com",
   "323-234-2389", "Work", "Mobile",
   "333 Third St., Hillsboro, OR, USA, 33333","Home", "Busy",
   "GENERATE_GUID", "2010-03-03 03:03:03", "CallMissed", "Mobile"  },
    { "Neal",    "Nagarajan", "Intel",
      "10-0-1974", "02-23-2002", "DEFAULT_PIC",
      "false", "/com/gmail/test4:test4:test4.gmail.com", "test4@gmail.com",
      "323-234-2390", "", "Mobile",
      "444 Forth St., Hillsboro, OR, UK, 44444","", "Unknown",
      "GENERATE_GUID", "2010-04-04 04:04:04", "SmsSent", "Other"  },
  { "Teresa",  "Thyme", "Intel",
    "10-0-1975", "02-23-2003", "avatar23",
    "true", "/com/gmail/test5/:test5:test5.gmail.com", "test5@gmail.com",
    "+44 3232342391","Home", "",
    "555 Fifth St., Hillsboro, OR, USA, 55555","Work", "Available",
    "GENERATE_GUID", "2010-05-05 05:05:05", "SmsReceived", "Home"  }
};

class DataGenModelPriv
{
public:
    static DataGenModel *theDataModel;
    static int theRefCount;
    static const QString selfGuid;
 
    static bool generateSampleData;
    static QString theEngine;

    QContactManager *manager;
    //QContactManagerEngine *engine;
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

bool DataGenModelPriv::generateSampleData = false;
QString DataGenModelPriv::theEngine = "default";
const QString DataGenModelPriv::selfGuid = "01111111-1111-1111-1111-11111111 1111";

void DataGenModel::setGenerateSampleData(bool generate)
{
    DataGenModelPriv::generateSampleData = generate;
}

void DataGenModel::setEngine(const QString& engine)
{
    DataGenModelPriv::theEngine = engine;
}

DataGenModel *DataGenModelPriv::theDataModel = NULL;
int DataGenModelPriv::theRefCount = 0;

DataGenModel *DataGenModel::instance()
{
    if (!DataGenModelPriv::theDataModel)
        DataGenModelPriv::theDataModel = new DataGenModel;
    DataGenModelPriv::theRefCount++;
    return DataGenModelPriv::theDataModel;
}

void DataGenModel::releaseInstance()
{
    if (DataGenModelPriv::theRefCount > 0)
        DataGenModelPriv::theRefCount--;
    if (!DataGenModelPriv::theRefCount && DataGenModelPriv::theDataModel) {
        delete DataGenModelPriv::theDataModel;
        DataGenModelPriv::theDataModel = NULL;
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
    if (!manager->saveDetailDefinition(seaside)) //unsupported
       qDebug() << "[DataGenModel] failed to save new detail definition";
}

DataGenModel::DataGenModel()
{
    priv = new DataGenModelPriv;

    priv->headers.append("First Name");
    priv->headers.append("Last Name");
    priv->headers.append("Company");
    priv->headers.append("Birthday");
    priv->headers.append("Anniversary");
    priv->headers.append("Avatar");
    priv->headers.append("Favorite");
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

    if (DataGenModelPriv::theEngine == "default") {
        if (QContactManager::availableManagers().contains("tracker")) {
            priv->manager = new QContactManager("tracker");
             /*QContactLocalIdFetchRequest request;
            QList<QContactLocalId> listOfIds = request.ids();
            priv->engine->startRequest(&request);
            priv->engine->waitForRequestFinished(&request, 1000);
             if(listOfIds.count() == 0)
                 DataGenModelPriv::generateSampleData = true;*/
            if (priv->manager->contactIds().count() == 0)
                DataGenModelPriv::generateSampleData = true;
        }
	else if (QContactManager::availableManagers().contains("memory")) {
	              priv->manager = new QContactManager("memory");
                      DataGenModelPriv::generateSampleData = true;
                  }else{
            priv->manager = new QContactManager("");
        }
    }
    else
        priv->manager = new QContactManager(DataGenModelPriv::theEngine);

    priv->settings = new QSettings("MeeGo", "libseaside");

    updateDefinitions(priv->manager);

    if (DataGenModelPriv::generateSampleData) {       

        int rows = sizeof(people_data) / sizeof(PeopleData);
        for (int i=0; i<rows; i++)
            createFakeContact(people_data[i]);
        if (priv->settings)
            priv->settings->sync();
    }

     QContact *contact;
    if(priv->manager->hasFeature(QContactManager::SelfContact, QContactType::TypeContact))
    {     
      QContactId contactId;
      const QContactLocalId meCardId(priv->manager->selfContactId());
      contact = priv->idToContact[meCardId];
    }else{
        QContactLocalId id = priv->uuidToId[DataGenModelPriv::selfGuid];
	contact = priv->idToContact[id];
    }
    
    if(!contact){
      createMeCard();
    }else{
      qWarning() << "DataGenModel::DataGenModel() MeCard exists";
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

void DataGenModel::createMeCard()
{
  QContact *contact = new QContact;
  if(priv->manager->hasFeature(QContactManager::SelfContact, QContactType::TypeContact))
    {     
      QContactId contactId;
      const QContactLocalId meCardId(priv->manager->selfContactId());
      contactId.setLocalId(meCardId);
      contact->setId(contactId);
    }     
    
  QContactGuid guid;
  guid.setGuid(DataGenModelPriv::selfGuid);
  if (!contact->saveDetail(&guid))
    qWarning() << "[DataGenModel] failed to save guid in mecard contact";
  
  QContactAvatar avatar;
  avatar.setImageUrl(QUrl("icon-m-content-avatar-placeholder"));
  if (!contact->saveDetail(&avatar))
      qWarning() << "[DataGenModel] failed to save avatar in mecard contact";
  
  // add the custom seaside detail
  SeasideCustomDetail sd;
  if (!contact->saveDetail(&sd))
    qWarning() << "[DataGenModel] failed to save seaside detail in mecard contact";
  
  QContactName name;
  name.setFirstName(QObject::tr("Me","Default string to describe self if no self contact information found, default created with [Me] as firstname"));
  name.setLastName("");
  if (!contact->saveDetail(&name))
    qWarning() << "[DataGenModel] failed to save mecard name";
  
  bool favorite = false;
  if (priv->settings) {
    QString key = guid.guid();
    key += "/favorite";
    priv->settings->setValue(key, favorite);
  }
  
  if (!priv->manager->saveContact(contact))
    qWarning() << "[DataGenModel] failed to save mecard contact";
}

QContactLocalId DataGenModel::getSelfContactId()
{
  if(priv->manager->hasFeature(QContactManager::SelfContact, QContactType::TypeContact)){
    	 return priv->manager->selfContactId();
    }else{
      QContactLocalId id = priv->uuidToId[DataGenModelPriv::selfGuid];
      if(id)
	return id;
    }
    return QContactLocalId();
}

void DataGenModel::createFakeContact(const PeopleData person)
{
    if (!DataGenModelPriv::generateSampleData)
        return;

    QContact contact;
      QContactGuid guid;
    if(QString(person.uuid) == "GENERATE_GUID"){
      guid.setGuid(person.uuid);
      if (!contact.saveDetail(&guid))
        qWarning() << "[DataGenModel] failed to save guid";
    }else{
      guid.setGuid(QUuid::createUuid().toString());
      if (!contact.saveDetail(&guid))
        qWarning() << "[DataGenModel] failed to save guid";
    }

    QContactName name;
    name.setFirstName(person.first);
    name.setLastName(person.last);
    if (!contact.saveDetail(&name))
        qWarning() << "[DataGenModel] failed to save name";

     QContactAvatar avatar;
    if(QString(person.avatar)!="DEFAULT_PIC"){
      avatar.setImageUrl(QUrl(person.avatar));
      if (!contact.saveDetail(&avatar))
        qWarning() << "[DataGenModel] failed to save avatar";
    }else{
      avatar.setImageUrl(QUrl("icon-m-content-avatar-placeholder"));
      if (!contact.saveDetail(&avatar))
        qWarning() << "[DataGenModel] failed to save avatar";
    }

   // int rows = sizeof(person.im) / sizeof(char*);
   // for(int i=0; i < rows; i++){
      QContactOnlineAccount imAccount;
      QStringList list = QString(person.im).split(":");
      if(list.count() != 3)
        qWarning() << "[DataGenModel] im detail missing fields";
      imAccount.setValue("AccountPath", list.at(2));
      imAccount.setAccountUri(list.at(0));
      imAccount.setValue("Nickname", list.at(1));
      if (!contact.saveDetail(&imAccount))
        qWarning() << "[DataGenModel] failed to save im account";
 // }
	
    //rows = sizeof(person.email) / sizeof(char*);
   // for(int i=0; i < rows; i++){
      QContactEmailAddress email;
      email.setEmailAddress(person.email);
      if (!contact.saveDetail(&email))
        qWarning() << "[DataGenModel] failed to save email";
  //  }

        /*QContactOrganization company;
        company.setName("Intel Corporation");
        if (!contact.saveDetail(&company))
          qWarning() << "[DataGenModel] failed to save company";*/
      if (priv->settings) {
	  QString key = guid.guid();
	  key = "/company";
          priv->settings->setValue(key, QString(person.company));
      }

      QContactAddress address;
      //rows = sizeof(person.addresses) / sizeof(char*);
     // for(int i=0; i < rows; i++){
        QStringList splitAddress = QString(person.addresses).split(",");
	address.setStreet(splitAddress[0]);
	address.setLocality(splitAddress[1]);
	address.setRegion(splitAddress[2]);
	address.setCountry(splitAddress[3]);
	address.setPostcode(splitAddress[4]);
   //   }
       
      if(QString(person.addresscontext)== "Home")
	address.setContexts(QContactDetail::ContextHome);
      else if(QString(person.addresscontext) == "Work")
	address.setContexts(QContactDetail::ContextWork);
      else if(QString(person.addresscontext) == "Other")
	address.setContexts(QContactDetail::ContextWork);
      else
	address.setContexts(QContactDetail::ContextHome); //default
        if (!contact.saveDetail(&address))
            qWarning() << "[DataGenModel] failed to save address";

        QContactPhoneNumber phone;
        //rows = sizeof(person.phone) / sizeof(char*);
       // for(int i=0; i < rows; i++){
          phone.setNumber(person.phone);

        if(QString(person.phonecontext) == "Home")
	  phone.setContexts(QContactDetail::ContextHome);
        else if(QString(person.phonecontext) == "Work")
	  phone.setContexts(QContactDetail::ContextWork);
        else if(QString(person.phonecontext) == "Other")
	  phone.setContexts(QContactDetail::ContextWork);
	else
	  phone.setContexts(QContactDetail::ContextHome); //default
	
      //  if ((i % 2) == 0)
            phone.setSubTypes(QContactPhoneNumber::SubTypeLandline);
    //    else
   //        phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
        if (!contact.saveDetail(&phone))
          qWarning() << "[DataGenModel] failed to save phone";
  //  }

	QContactPresence presence;
        if (QString(person.presence) == "Available")
	  presence.setPresenceState(QContactPresence::PresenceAvailable);
        else if (QString(person.presence) == "Away")
	  presence.setPresenceState(QContactPresence::PresenceAway);
        else if (QString(person.presence) == "Busy")
	  presence.setPresenceState(QContactPresence::PresenceBusy);
	else 
	  presence.setPresenceState(QContactPresence::PresenceOffline); //unknown?
	if (!contact.saveDetail(&presence))
          qWarning() << "[DataGenModel] failed to save online account";

        bool favorite = QString(person.favorite) == "true" ? true : false;
	if (priv->settings) {
	  QString key = guid.guid();
	  key = "/favorite";
	  priv->settings->setValue(key, favorite);
	}

	SeasideCustomDetail sd;       
       QDateTime stamp;
        stamp.fromString(QString(person.commtimestamp), Qt::TextDate);
        sd.setCommTimestamp(stamp);
        if(QString(person.commtype) == "CallDialed")
	  sd.setCommType(Seaside::CommCallDialed);
        else if(QString(person.commtype) == "CallReceived")
	  sd.setCommType(Seaside::CommCallReceived);
        else if(QString(person.commtype )== "CallMissed")
	  sd.setCommType(Seaside::CommCallMissed);
        else if(QString(person.commtype) == "SmsReceived")
	  sd.setCommType(Seaside::CommSmsReceived);
        else if(QString(person.commtype) == "SmsSent")
	  sd.setCommType(Seaside::CommSmsSent);
        else if(QString(person.commtype) == "EmailReceived")
	  sd.setCommType(Seaside::CommEmailReceived);
        else if(QString(person.commtype) == "EmailSent")
	  sd.setCommType(Seaside::CommEmailSent);
	else
	  sd.setCommType(Seaside::CommNone);

        if(QString(person.commlocation )== "Home")
	  sd.setCommLocation(Seaside::LocationHome);
        else if(QString(person.commlocation) == "Work")
	  sd.setCommLocation(Seaside::LocationWork);
        else if(QString(person.commlocation) == "Mobile")
	  sd.setCommLocation(Seaside::LocationMobile);
        else if(QString(person.commlocation) == "Other")
	  sd.setCommLocation(Seaside::LocationOther);
	else
	  sd.setCommLocation(Seaside::LocationNone);

    if (!contact.saveDetail(&sd))
        qWarning() << "[DataGenModel] failed to save seaside detail";

    if (!priv->manager->saveContact(&contact))
        qWarning() << "[DataGenModel] failed to save contact";

}

DataGenModel::~DataGenModel()
{
    foreach (QContact *contact, priv->idToContact.values())
        delete contact;
    delete priv->manager;
    delete priv;
}

int DataGenModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return priv->contactIds.size();
}

int DataGenModel::columnCount(const QModelIndex& parent) const
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

QVariant DataGenModel::data(const QModelIndex& index, int role) const
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
          //  if (role == Qt::DisplayRole)
                //return QVariant(company.name());
            if (priv->settings) {
                QContactGuid guid = contact->detail<QContactGuid>();
                QString key = guid.guid();
                key += "/company";
                return priv->settings->value(key, company.name());
            }
        }

    case Seaside::ColumnBirthday:  // birthday
        {
            QContactBirthday day = contact->detail<QContactBirthday>();
            if (role != Qt::DisplayRole)
                return QVariant(day.date());
            else
                return QVariant();
        }

    case Seaside::ColumnAnniversary:  // anniversary
        {
            QContactAnniversary day = contact->detail<QContactAnniversary>();
            if (role != Qt::DisplayRole)
                return QVariant(day.originalDate());
            else
                return QVariant();
        }

    case Seaside::ColumnAvatar:  // avatar
        {
            if(role != Qt::DisplayRole){
            QContactAvatar avatar = contact->detail<QContactAvatar>();
            return QVariant(avatar.imageUrl().toString());
             }
            return QVariant();
        }

    case Seaside::ColumnFavorite:  // favorite
        {
            if (role != Qt::DisplayRole) // TODO: this should always return false
                return QVariant();
            if (priv->settings) {
                QContactGuid guid = contact->detail<QContactGuid>();
                QString key = guid.guid();
                key += "/favorite";
                return priv->settings->value(key, false);
            }
            return QVariant(false);
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

                qWarning() << "Seaside::ColumnPhoneNumbers phone" << phone;

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

                 qWarning() << "Seaside::ColumnPhoneNumbers list" << list;
            }
            if (role == Qt::DisplayRole) {
                QStringList searchable;
                foreach (QString number, list)
                    searchable << number.replace(QRegExp("[^0-9*#]"), "");
                searchable << list;
                qWarning() << "Seaside::ColumnPhoneNumbers searchable" << searchable;
                return QVariant(searchable.join(" "));
            }
            else{
                qWarning() << "Seaside::ColumnPhoneNumbers empty list" << list;
                return QVariant(list);

            }
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
        qWarning() << "[DataGenModel] request for data on unexpected column" <<
                index.column() << " role : " << role;
        return QVariant();
    }
}

QVariant DataGenModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);

    if (role == Qt::DisplayRole) {
        if (section < priv->headers.size()) {
            return QVariant(priv->headers.at(section));
        }
    }
    return QVariant();
}

QContact *DataGenModel::contact(int row)
{
    // TODO: validate
    QContactLocalId id = priv->contactIds[row];
    return priv->idToContact[id];
}

QContactManager *DataGenModel::manager()
{
    return priv->manager;
}

QContactManagerEngine *DataGenModel::engine()
{
    //return priv->engine;
    return NULL;
}

void DataGenModel::fixIndexMap()
{
    int i=0;
    priv->idToIndex.clear();
    foreach (const QContactLocalId& id, priv->contactIds)
        priv->idToIndex.insert(id, i++);
}

void DataGenModel::addContacts(const QList<QContactLocalId>& contactIds) {
    int size = priv->contactIds.size();

    foreach (const QContactLocalId& id, contactIds) {
        priv->contactIds.push_back(id);
        priv->idToIndex.insert(id, size++);

        /*QContactFetchRequest request;
        QContactLocalIdFilter filter;
        QList<QContactLocalId> ids;
        ids.append(id);
        filter.setIds(ids);
        request.setFilter(filter);

       priv->engine->startRequest(&request);
       priv->engine->waitForRequestFinished(&request, 1000);

        QVERIFY(request.isFinished());
        QVERIFY(!request.contacts.isEmpty());

        QList<QContact> filteredList = request.contacts();
         QContact *contact = new QContact(&filteredList.at(0));*/
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

void DataGenModel::contactsAdded(const QList<QContactLocalId>& contactIds)
{
    qDebug() << "[DataGenModel] contacts added:" << contactIds;
    int size = priv->contactIds.size();
    int added = contactIds.size();

    beginInsertRows(QModelIndex(), size, size + added - 1);
    addContacts(contactIds);
    endInsertRows();
}

void DataGenModel::contactsChanged(const QList<QContactLocalId>& contactIds)
{
    qDebug() << "[DataGenModel] contacts changed:" << contactIds;
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
        if (contact){
           /*  QContactFetchRequest request;
             QContactLocalIdFilter filter;
             QList<QContactLocalId> ids;
             ids.append(id);
             filter.setIds(ids);
             request.setFilter(filter);

            priv->engine->startRequest(&request);
            priv->engine->waitForRequestFinished(&request, 1000);

             QVERIFY(request.isFinished());
             QVERIFY(!request.contacts.isEmpty());

             QList<QContact> filteredList = request.contacts();
              *contact = &filteredList.at(0);*/
            *contact = priv->manager->contact(id);
        }
    }

    // FIXME: unfortunate that we can't easily identify what changed
    if (min <= max)
        emit dataChanged(index(min, 0), index(max, Seaside::ColumnLast));
}

void DataGenModel::contactsRemoved(const QList<QContactLocalId>& contactIds)
{
    qDebug() << "[DataGenModel] contacts removed:" << contactIds;
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

void DataGenModel::dataReset()
{
    qDebug() << "[DataGenModel] data reset";
    beginResetModel();

    foreach (QContact *contact, priv->idToContact.values())
        delete contact;

    priv->contactIds.clear();
    priv->idToContact.clear();
    priv->idToIndex.clear();
    priv->uuidToId.clear();
    priv->idToUuid.clear();

    /*QContactLocalIdFetchRequest request;
    request.setFilter(priv->currentFilter);

   priv->engine->startRequest(&request);
   priv->engine->waitForRequestFinished(&request, 1000);

  QVERIFY(request.isFinished());
  QVERIFY(!request.contacts.isEmpty());

   QList<QContactLocalId> listOfIds = request.ids();
   addContacts(listOfIds);*/

   addContacts(priv->manager->contactIds(priv->currentFilter));

    endResetModel();
}

SeasidePersonModel *DataGenModel::createPersonModel(const QModelIndex& index)
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

    return model;
}

SeasidePersonModel *DataGenModel::createPersonModel(const QUuid& uuid)
{
    QContactLocalId id = priv->uuidToId[uuid];
    QModelIndex personIndex = index(priv->idToIndex[id], 0);
    return createPersonModel(personIndex);
}

void DataGenModel::deletePerson(const QUuid& uuid)
{
    if (!priv->manager->removeContact(priv->uuidToId[uuid]))
        qWarning() << "[DataGenModel] failed to delete contact";
}

void DataGenModel::updatePerson(const SeasidePersonModel *newModel)
{
    QContactLocalId id = priv->uuidToId[newModel->uuid()];
    QContact *contact = priv->idToContact[id];
    if (!contact) {
        // we are creating a new contact
        contact = new QContact;

        QContactGuid guid;
        guid.setGuid(QUuid::createUuid().toString());
        if (!contact->saveDetail(&guid))
            qWarning() << "[DataGenModel] failed to save guid in new contact";

        QContactAvatar avatar;
        avatar.setImageUrl(QUrl("icon-m-content-avatar-placeholder"));
        if (!contact->saveDetail(&avatar))
            qWarning() << "[DataGenModel] failed to save avatar in new contact";

        // add the custom seaside detail
        SeasideCustomDetail sd;
        if (!contact->saveDetail(&sd))
            qWarning() << "[DataGenModel] failed to save seaside detail in new contact";
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
            qWarning() << "[DataGenModel] failed to update name";
    }
 
    const QString& newCompany = newModel->company();
    if (oldModel->company() != newCompany) {
        QContactOrganization company = contact->detail<QContactOrganization>();
       /* if (!company.name().isEmpty()) {
            if (!contact->removeDetail(&company))
                qWarning() << "[DataGenModel] failed to remove company";
        }

        if (!newModel->company().isEmpty()) {
            company.setName(newCompany);

            if (!contact->saveDetail(&company))
                qWarning() << "[DataGenModel] failed to update company";
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
                qWarning() << "[DataGenModel] failed to remove phone number";

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

                qWarning() << "[DataGenModel] to save phone number" << phone;

                if (!contact->saveDetail(&phone))
                    qWarning() << "[DataGenModel] failed to save phone number";
            }
        }
    }

  const QVector<SeasideDetail>& newIMs = newModel->ims();
    if (oldModel->ims() != newIMs) {
        foreach (QContactDetail detail, contact->details<QContactOnlineAccount>())
            if (!contact->removeDetail(&detail))
                qWarning() << "[DataGenModel] failed to remove im account";

        foreach (const SeasideDetail& detail, newIMs) {
            if (detail.isValid()) {
                QContactOnlineAccount imAccount;
                QStringList list = (detail.text()).split(":");
                if(list.count() != 3)
                     qWarning() << "[DataGenModel] im detail missing fields";
                imAccount.setValue("AccountPath", list.at(2));
                 imAccount.setAccountUri(list.at(0));
                 imAccount.setValue("Nickname", list.at(1));
                if (!contact->saveDetail(&imAccount))
                    qWarning() << "[DataGenModel] failed to save im account";
            }
        }
    }

    const QVector<SeasideDetail>& newEmails = newModel->emails();
    if (oldModel->emails() != newEmails) {
        foreach (QContactDetail detail, contact->details<QContactEmailAddress>())
            if (!contact->removeDetail(&detail))
                qWarning() << "[DataGenModel] failed to remove email address";

        foreach (const SeasideDetail& detail, newEmails) {
            if (detail.isValid()) {
                QContactEmailAddress email;
                email.setEmailAddress(detail.text());

                if (!contact->saveDetail(&email))
                    qWarning() << "[DataGenModel] failed to save email address";
            }
        }
    }

    const QVector<SeasideDetail>& newAddresses = newModel->addresses();
    if (oldModel->addresses() != newAddresses) {
        foreach (QContactDetail detail, contact->details<QContactAddress>())
            if (!contact->removeDetail(&detail))
                qWarning() << "[DataGenModel] failed to remove phone number";

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
                    qWarning() << "[DataGenModel] failed to save address";
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
        qWarning() << "[DataGenModel] failed to update contact";

        // make sure data shown to user matches what is really in the database        
      /*   QContactFetchRequest request;
         QContactLocalIdFilter filter;
         QList<QContactLocalId> ids;
         ids.append(id);
         filter.setIds(ids);
         request.setFilter(filter);

        priv->engine->startRequest(&request);
        priv->engine->waitForRequestFinished(&request, 1000);

         QVERIFY(request.isFinished());
         QVERIFY(!request.contacts.isEmpty());

         QList<QContact> filteredList = request.contacts();
          *contact = new QContact(&filteredList.at(0));*/
        *contact = priv->manager->contact(id);
    }

    delete oldModel;
}

void DataGenModel::setAvatar(const QUuid& uuid, const QString& path)
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
        qWarning() << "[DataGenModel] failed to save avatar";

    if (!priv->manager->saveContact(contact))
        qWarning() << "[DataGenModel] failed to save contact while setting avatar";
}

void DataGenModel::setFavorite(const QUuid& uuid, bool favorite)
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

void DataGenModel::setCompany(const QUuid& uuid, QString company)
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
