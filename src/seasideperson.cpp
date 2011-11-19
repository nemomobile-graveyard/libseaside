#include <QDebug>

#include <QContactBirthday>
#include <QContactName>
#include <QContactFavorite>
#include <QContactPhoneNumber>
#include <QContactEmailAddress>
#include <QContactOnlineAccount>
#include <QContactOrganization>

#include "seasideperson.h"

#if 0
    case AvatarRole:
    {
        QContactAvatar avatar = mContact.detail<QContactAvatar>();
        if(!avatar.imageUrl().isEmpty()) {
            return QUrl(avatar.imageUrl()).toString();
        }
        return QString();
    }
    case ThumbnailRole:
    {
        QContactThumbnail thumb = mContact.detail<QContactThumbnail>();
        return thumb.thumbnail();
    }
    case FavoriteRole:
    {
        QContactFavorite fav = mContact.detail<QContactFavorite>();
        if(!fav.isEmpty())
            return QVariant(fav.isFavorite());
        return false;
    }
    case OnlineAccountUriRole:
    {
        QStringList list;
        foreach (const QContactOnlineAccount& account,
                 mContact.details<QContactOnlineAccount>()){
            if(!account.accountUri().isNull())
                list << account.accountUri();
        }
        return list;
    }
    case OnlineServiceProviderRole:
    {
        QStringList list;
        foreach (const QContactOnlineAccount& account,
                 mContact.details<QContactOnlineAccount>()){
            if(!account.serviceProvider().isNull())
                list << account.serviceProvider();
        }
        return list;
    }
    case IsSelfRole:
    {
        if (isSelfContact(mContact.id().localId()))
            return true;
        return false;
    }
    case EmailAddressRole:
    {
        QStringList list;
        foreach (const QContactEmailAddress& email,
                 mContact.details<QContactEmailAddress>()){
            if(!email.emailAddress().isNull())
                list << email.emailAddress();
        }
        return list;
    }
    case EmailContextRole:
    {
        QStringList list;
        foreach (const QContactEmailAddress& email,
                 mContact.details<QContactEmailAddress>()) {
            if (email.contexts().count() > 0)
                list << email.contexts().at(0);
        }
        return list;
    }
    case PhoneNumberRole:
    {
        QStringList list;
        foreach (const QContactPhoneNumber& phone,
                 mContact.details<QContactPhoneNumber>()){
            if(!phone.number().isNull())
                list << phone.number();
        }
        return list;
    }
    case PhoneContextRole:
    {
        QStringList list;
        foreach (const QContactPhoneNumber& phone,
                 mContact.details<QContactPhoneNumber>()) {
            if (phone.contexts().count() > 0)
                list << phone.contexts().at(0);
            else if (phone.subTypes().count() > 0)
                list << phone.subTypes().at(0);
        }
        return list;
    }
    case AddressRole:
    {
        QStringList list;
        QStringList fieldOrder = priv->localeHelper->getAddressFieldOrder();

        foreach (const QContactAddress& address,
                 mContact.details<QContactAddress>()) {
            QString aStr;
            QString temp;
            QString addy = address.street();
            QStringList streetList = addy.split("\n");

            for (int i = 0; i < fieldOrder.size(); ++i) {
                temp = "";
                if (fieldOrder.at(i) == "street") {
                    if (streetList.count() == 2)
                        temp += streetList.at(0);
                    else
                        temp += addy;
                } else if (fieldOrder.at(i) == "street2") {
                    if (streetList.count() == 2)
                        temp += streetList.at(1);
                }
                else if (fieldOrder.at(i) == "locale")
                    temp += address.locality();
                else if (fieldOrder.at(i) == "region")
                    temp += address.region();
                else if (fieldOrder.at(i) == "zip")
                    temp += address.postcode();
                else if (fieldOrder.at(i) == "country")
                    temp += address.country();

                if (i > 0)
                    aStr += "\n" + temp.trimmed();
                else
                    aStr += temp.trimmed();
            }

            if (aStr == "")
                aStr = address.street() + "\n" + address.locality() + "\n" +
                       address.region() + "\n" + address.postcode() + "\n" +
                       address.country();
           list << aStr;
        }
        return list;
    }
    case AddressStreetRole:
    {
        QStringList list;
        foreach (const QContactAddress& address,
                 mContact.details<QContactAddress>()){
            if(!address.street().isEmpty())
                list << address.street();
        }
        return list;
    }
    case AddressLocaleRole:
    {
        QStringList list;
        foreach (const QContactAddress& address,
                 mContact.details<QContactAddress>()) {
            if(!address.locality().isNull())
                list << address.locality();
        }
        return list;
    }
    case AddressRegionRole:
    {
        QStringList list;
        foreach (const QContactAddress& address,
                 mContact.details<QContactAddress>()) {
            if(!address.region().isNull())
                list << address.region();
        }
        return list;
    }
    case AddressCountryRole:
    {
        QStringList list;
        foreach (const QContactAddress& address,
                 mContact.details<QContactAddress>()) {
            if(!address.country().isNull())
                list << address.country();
        }
        return list;
    }
    case AddressPostcodeRole:
    {
        QStringList list;
        foreach (const QContactAddress& address,
                 mContact.details<QContactAddress>())
            list << address.postcode();
        return list;
    }

    case AddressContextRole:
    {
        QStringList list;
        foreach (const QContactAddress& address,
                 mContact.details<QContactAddress>()) {
            if (address.contexts().count() > 0)
                list << address.contexts().at(0);
        }
        return list;
    }
    case PresenceRole:
    {
        foreach (const QContactPresence& qp,
                 mContact.details<QContactPresence>()) {
            if(!qp.isEmpty())
                if(qp.presenceState() == QContactPresence::PresenceAvailable)
                    return qp.presenceState();
        }
        foreach (const QContactPresence& qp,
                 mContact.details<QContactPresence>()) {
            if(!qp.isEmpty())
                if(qp.presenceState() == QContactPresence::PresenceBusy)
                    return qp.presenceState();
        }
        return QContactPresence::PresenceUnknown;
    }

    case UuidRole:
    {
        QContactGuid guid = mContact.detail<QContactGuid>();
        if(!guid.guid().isNull())
            return guid.guid();
        return QString();
    }

    case WebUrlRole:
    {
        QStringList list;
       foreach(const QContactUrl &url, mContact.details<QContactUrl>()){
        if(!url.isEmpty())
            list << url.url();
       }
        return QStringList(list);
    }

    case WebContextRole:
    {
        QStringList list;
       foreach(const QContactUrl &url, mContact.details<QContactUrl>()){
        if(!url.contexts().isEmpty())
            list << url.contexts();
       }
        return list;
    }

    case NotesRole:
    {
        QContactNote note = mContact.detail<QContactNote>();
        if(!note.isEmpty())
            return note.note();
        return QString();
    }
    case FirstCharacterRole:
    {
        if (isSelfContact(mContact.id().localId()))
            return QString(tr("#"));

        return priv->localeHelper->getBinForString(data(row,
                    DisplayLabelRole).toString());
    }
    case DisplayLabelRole: {
    }

#endif

SeasidePerson::SeasidePerson(QObject *parent)
    : QObject(parent)
{

}

SeasidePerson::SeasidePerson(const QContact &contact, QObject *parent)
    : QObject(parent)
    , mContact(contact)
{
    recalculateDisplayLabel();
}

SeasidePerson::~SeasidePerson()
{
    emit contactRemoved();
}

// QT5: this needs to change type
int SeasidePerson::id() const
{
    return mContact.id().localId();
}

QString SeasidePerson::firstName() const
{
    QContactName nameDetail = mContact.detail<QContactName>();
    return nameDetail.firstName();
}

void SeasidePerson::setFirstName(const QString &name)
{
    qDebug() << Q_FUNC_INFO << "Setting to " << name;
    QContactName nameDetail = mContact.detail<QContactName>();
    nameDetail.setFirstName(name);
    mContact.saveDetail(&nameDetail);
    emit firstNameChanged();
    recalculateDisplayLabel();
}

QString SeasidePerson::lastName() const
{
    QContactName nameDetail = mContact.detail<QContactName>();
    return nameDetail.lastName();
}

void SeasidePerson::setLastName(const QString &name)
{
    qDebug() << Q_FUNC_INFO << "Setting to " << name;
    QContactName nameDetail = mContact.detail<QContactName>();
    nameDetail.setLastName(name);
    mContact.saveDetail(&nameDetail);
    emit lastNameChanged();
    recalculateDisplayLabel();
}

// small helper to avoid inconvenience
static QString generateDisplayLabel(QContact mContact)
{
    //REVISIT: Move this or parts of this to localeutils.cpp
    QString displayLabel;
    QContactName name = mContact.detail<QContactName>();
    QString nameStr1 = name.firstName();
    QString nameStr2 = name.lastName();

    if (!nameStr1.isNull())
        displayLabel.append(nameStr1);

    if (!nameStr2.isNull()) {
        if (!displayLabel.isEmpty())
            displayLabel.append(" ");
        displayLabel.append(nameStr2);
    }

    if (!displayLabel.isEmpty())
        return displayLabel;

    foreach (const QContactPhoneNumber& phone, mContact.details<QContactPhoneNumber>()) {
        if(!phone.number().isNull())
            return phone.number();
    }

    foreach (const QContactOnlineAccount& account,
             mContact.details<QContactOnlineAccount>()){
        if(!account.accountUri().isNull())
            return account.accountUri();
    }

    foreach (const QContactEmailAddress& email,
             mContact.details<QContactEmailAddress>()){
        if(!email.emailAddress().isNull())
            return email.emailAddress();
    }

    QContactOrganization company = mContact.detail<QContactOrganization>();
    if (!company.name().isNull())
        return company.name();

    return "(unnamed)"; // TODO: localisation
}

void SeasidePerson::recalculateDisplayLabel()
{
    QString oldDisplayLabel = displayLabel();
    QString newDisplayLabel = generateDisplayLabel(mContact);

    // TODO: would be lovely if mobility would let us store this somehow
    if (oldDisplayLabel != newDisplayLabel) {
        qDebug() << Q_FUNC_INFO << "Recalculated display label to " <<
            newDisplayLabel;
        mDisplayLabel = newDisplayLabel;
        emit displayLabelChanged();
    }
}

QString SeasidePerson::displayLabel() const
{
    return mDisplayLabel;
}

QString SeasidePerson::companyName() const
{
    qDebug() << Q_FUNC_INFO << "STUB";
    return QString();
}

void SeasidePerson::setCompanyName(const QString &name)
{
    emit companyNameChanged();
}

bool SeasidePerson::favorite() const
{
    QContactFavorite favoriteDetail = mContact.detail<QContactFavorite>();
    return favoriteDetail.isFavorite();
}

void SeasidePerson::setFavorite(bool favorite)
{
    QContactFavorite favoriteDetail = mContact.detail<QContactFavorite>();
    favoriteDetail.setFavorite(favorite);
    mContact.saveDetail(&favoriteDetail);
    emit favoriteChanged();
}

QString SeasidePerson::avatarPath() const
{
    qDebug() << Q_FUNC_INFO << "STUB";
    return QString();
}

void SeasidePerson::setAvatarPath(QString avatarPath)
{
    emit avatarPathChanged();
}

QString SeasidePerson::birthday() const
{
    QContactBirthday day = mContact.detail<QContactBirthday>();
    if(!day.date().isNull())
        return day.date().toString(Qt::SystemLocaleDate);
    return QString();
}

void SeasidePerson::setBirthday(const QString &birthday)
{
    QContactBirthday day = mContact.detail<QContactBirthday>();
    day.setDate(QDate::fromString(birthday, Qt::SystemLocaleDate));
    mContact.saveDetail(&day);
    emit birthdayChanged();
}

QStringList SeasidePerson::phoneNumbers() const
{
    QStringList list;

    foreach (const QContactPhoneNumber& phone, mContact.details<QContactPhoneNumber>()) {
        if (!phone.number().isEmpty())
            list << phone.number();
    }

    qDebug() << Q_FUNC_INFO << "Returning " << list;
    return list;
}

void SeasidePerson::setPhoneNumbers(const QStringList &phoneNumbers)
{
    const QList<QContactPhoneNumber> &oldNumbers = mContact.details<QContactPhoneNumber>();

    if (oldNumbers.count() != phoneNumbers.count()) {
        // this could probably be optimised, but it's easiest: we just remove
        // all the old phone number details, and add new ones
        foreach (QContactPhoneNumber phone, oldNumbers)
            mContact.removeDetail(&phone);

        foreach (const QString &number, phoneNumbers) {
            QContactPhoneNumber phone;
            phone.setNumber(number);
            mContact.saveDetail(&phone);
        }
    } else {
        // assign new numbers to the existing details.
        for (int i = 0; i != phoneNumbers.count(); ++i) {
            QContactPhoneNumber phone = oldNumbers.at(i);
            phone.setNumber(phoneNumbers.at(i));
            mContact.saveDetail(&phone);
        }
    }

    emit phoneNumbersChanged();
}

QContact SeasidePerson::contact() const
{
    return mContact;
}

void SeasidePerson::setContact(const QContact &contact)
{
    // TODO: this should difference the two contacts, and emit the proper
    // signals
    mContact = contact;
}

