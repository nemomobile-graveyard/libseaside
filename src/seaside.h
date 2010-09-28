/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDE_H
#define SEASIDE_H

#include <QAbstractItemModel>
#include <QUuid>
#include <QContact>
#include <QtDebug>

class SeasidelListModel;
class SeasideProxyModel;

using namespace QtMobility;

class Seaside
{
public:
    enum Role {
      DataRole = Qt::UserRole + 0,
        SearchRole = Qt::UserRole + 1
    };

    enum Column {
        ColumnFirstName,
        ColumnLastName,
        ColumnCompany,
    //  ColumnTitle, //TODO: Add title
        ColumnBirthday,
        ColumnAnniversary,
        ColumnAvatar,
        ColumnFavorite,
	ColumnisSelf,
        ColumnIMAccounts, //REVISIT
        ColumnEmailAddresses,
        ColumnPhoneNumbers,
        ColumnPhoneContexts,
        ColumnPhoneTypes,
        ColumnAddresses,
        ColumnAddressContexts,
        ColumnPresence,
        ColumnUuid,
        ColumnCommTimestamp,
        ColumnCommType,
        ColumnCommLocation,
        ColumnLast = ColumnCommLocation
    };

    enum Presence {
        PresenceAvailable,
        PresenceAway,
        PresenceOffline,
	PresenceBusy, //REVISIT
        PresenceUnknown
    };

    enum CommType {
        CommCallDialed    = 0x0001,
        CommCallReceived  = 0x0002,
        CommCallMissed    = 0x0004,
        CommSmsSent       = 0x0010,
        CommSmsReceived   = 0x0020,
        CommEmailSent     = 0x0100,
        CommEmailReceived = 0x0200,
        CommIMSent        = 0x1000, //REVISIT
        CommIMReceived    = 0x2000, //REVISIT
        CommNone          = 0x0000
    };

    enum Location {
        LocationMobile,
        LocationHome,
        LocationWork,
	LocationOther, //REVISIT
        LocationNone
    };

    static inline QVariant field(const QAbstractItemModel *model, int row, int column)
    {
        return model->index(row, column).data(DataRole);
    }

    // convenience functions for working with QContact
    // items where we support only one detail
private:
    static QString contactName(const QContact *contact);
    static QString contactFirstName(const QContact *contact);
    static QString contactLastName(const QContact *contact);
    static QString contactAvatarPath(const QContact *contact);
    static QString contactOrganization(const QContact *contact);
    static QDate contactBirthday(const QContact *contact);
    static QDate contactAnniversary(const QContact *contact);
    static bool contactFavorite(const QContact *contact);
    static bool contactisSelf(const QContact *contact);
    static Presence contactPresence(const QContact *contact);
    static QUuid contactUuid(const QContact *contact);
    static QDateTime contactCommTimestamp(const QContact *contact);
    // items where we support multiple details
    static QStringList contactEmailAddresses(const QContact *contact);
    static QStringList contactIMAccounts(QContact *contact); //REVISIT
    static QStringList contactAddresses(const QContact *contact);
    static QStringList contactPhoneNumbers(const QContact *contact);
    static QStringList contactNotes(const QContact *contact);

    friend class SeasideListModel;
    friend class SeasideProxyModel;

};

/*
  Model data for Seaside::DataRole
  --------------------------------
  FirstName           String
  LastName            String
  Company             String
  Birthday            Date
  Anniversary         Date
  Avatar              String      path/filename
  Favorite            Bool
  isSelf              Bool
  IMAccounts          StringList
  EmailAddresses      StringList
  PhoneNumbers        StringList
  PhoneContexts       StringList  of "Home", "Work", or empty
  PhoneTypes          StringList  of "Mobile" or empty
  Addresses           StringList
  AddressContexts     StringList  of "Home", "Work", or empty
  Presence            Int         mapping to Seaside::Presence enum
  Uuid                String
  CommTimestamp       DateTime
  CommType            Int         mapping to Seaside::CommType enum
  CommLocation        Int         mapping to Seaside::Location enum

  Use the following macros to simplify repeated access to fields.
  First, call SEASIDE_SHORTCUTS
  Then call SEASIDE_SET_MODEL_AND_ROW, passing the model and row number, probably
    from a QModelIndex you have.
  Then call SEASIDE_FIELD with the name and corresponding type from the table above,
    with initial caps as shown.
  For example:

  SEASIDE_SHORTCUTS
  SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row());

  bool fav = SEASIDE_FIELD(Favorite, Bool);
  QStringList emails = SEASIDE_FIELD(EmailAddresses, StringList);
*/

#define SEASIDE_SHORTCUTS \
const QAbstractItemModel *seasideModel; \
int seasideRow;

#define SEASIDE_SET_MODEL_AND_ROW(model, row) \
seasideModel = model; \
seasideRow = row;

#define SEASIDE_FIELD(name, type) \
(Seaside::field(seasideModel, seasideRow, Seaside::Column##name).to##type())

class SeasideCommEvent
{
public:
    SeasideCommEvent();
    virtual ~SeasideCommEvent();

    bool operator ==(const SeasideCommEvent& event);

    void setType(Seaside::CommType type);
    void setLocation(Seaside::Location location);
    void setDateTime(const QDateTime& time);
    void setText(const QString& text);
    Seaside::CommType type();
    Seaside::Location location();
    QDateTime dateTime();
    const QString& text();

    QString getFriendlyDateString();

private:
    Seaside::CommType m_type;
    Seaside::Location m_location;
    QDateTime m_dateTime;
    QString m_text;
};

class SeasideDetail
{
public:
    SeasideDetail();
    SeasideDetail(const QString &text, Seaside::Location location = Seaside::LocationNone);
    virtual ~SeasideDetail();
    bool operator ==(const SeasideDetail& detail);

    Seaside::Location location() const;
    const QString& text() const;

    bool isValid() const;

private:
    bool m_valid;
    Seaside::Location m_location;
    QString m_text;
};

#endif // SEASIDE_H
