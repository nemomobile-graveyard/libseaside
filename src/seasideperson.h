/*
 * Copyright 2011 Robin Burchell
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef SEASIDEPERSON_H
#define SEASIDEPERSON_H

#include <QObject>
#include <QContact>

QTM_USE_NAMESPACE

class SeasidePerson : public QObject
{
    Q_OBJECT
public:
    explicit SeasidePerson(QObject *parent = 0);
    ~SeasidePerson();

    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName NOTIFY firstNameChanged)
    QString firstName() const;
    void setFirstName(const QString &name);

    Q_PROPERTY(QString lastName READ lastName WRITE setLastName NOTIFY lastNameChanged)
    QString lastName() const;
    void setLastName(const QString &name);

    Q_PROPERTY(QString displayLabel READ displayLabel NOTIFY displayLabelChanged)
    QString displayLabel() const;

    Q_PROPERTY(QString companyName READ companyName WRITE setCompanyName NOTIFY companyNameChanged)
    QString companyName() const;
    void setCompanyName(const QString &name);

    Q_PROPERTY(bool favorite READ favorite WRITE setFavorite NOTIFY favoriteChanged)
    bool favorite() const;
    void setFavorite(bool favorite);

    Q_PROPERTY(QString avatarPath READ avatarPath WRITE setAvatarPath NOTIFY avatarPathChanged)
    QString avatarPath() const;
    void setAvatarPath(QString avatarPath);

    Q_PROPERTY(QString birthday READ birthday WRITE setBirthday NOTIFY birthdayChanged)
    QString birthday() const;
    void setBirthday(const QString &birthday);

    QContact contact() const;
    void setContact(const QContact &contact);

    void recalculateDisplayLabel();

signals:
    void contactRemoved();
    void firstNameChanged();
    void lastNameChanged();
    void displayLabelChanged();
    void companyNameChanged();
    void favoriteChanged();
    void avatarPathChanged();
    void birthdayChanged();

private:
    // TODO: private class
    explicit SeasidePerson(const QContact &contact, QObject *parent = 0);
    QContact mContact;
    QString mDisplayLabel;

    friend class SeasidePeopleModelPriv;
    /*
    void Changed();
    void Changed();
    void Changed();
    void Changed();
    void Changed();
    void Changed();
    void Changed();
    void Changed();
    void Changed();
    void Changed();
    */
};

Q_DECLARE_METATYPE(SeasidePerson *);

// uuid, presence, self contact

/*
        BirthdayRole,
        OnlineAccountUriRole,
        OnlineServiceProviderRole,
        EmailAddressRole,
        EmailContextRole,
        PhoneNumberRole,
        PhoneContextRole,
        AddressRole,
        AddressStreetRole,
        AddressLocaleRole,
        AddressRegionRole,
        AddressCountryRole,
        AddressPostcodeRole,
        AddressContextRole,
        WebUrlRole,
        WebContextRole,
        NotesRole,
        FirstCharacterRole,
        DisplayLabelRole
*/

#endif // SEASIDEPERSON_H
