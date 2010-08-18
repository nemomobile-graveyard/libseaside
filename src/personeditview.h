/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef PERSONEDITVIEW_H
#define PERSONEDITVIEW_H

#include <MWidgetView>

#include <seasidepersonmodel.h>

class QGraphicsGridLayout;
class MImageWidget;
class MTextEdit;
class MButton;
class MGridLayoutPolicy;
class SeasideLabel;

class PersonEditView: public MWidgetView
{
    Q_OBJECT
    M_VIEW(SeasidePersonModel, MWidgetStyle)

public:
    PersonEditView(MWidgetController *controller);
    virtual ~PersonEditView();

protected:
    virtual void setupModel();
    virtual void init();

    virtual void updatePhone(int row);
    virtual void updateEmail(int row);
    virtual void updateAddress(int row);

protected slots:
    void newPhone(const QString& phone = QString(),
                  Seaside::Location location = Seaside::LocationNone);
    void newEmail(const QString& email = QString());
    void newAddress(const QString& address = QString(),
                    Seaside::Location location = Seaside::LocationNone);
    void deletePhone();
    void deleteEmail();
    void deleteAddress();
    void textChanged();
    void activated(int index);
    void favoriteClicked(bool init = false);

private:
    static const int MaxSections = 4;

    MGridLayoutPolicy *m_mainPolicy;
    MGridLayoutPolicy *m_phonePolicy;
    MGridLayoutPolicy *m_emailPolicy;
    MGridLayoutPolicy *m_addressPolicy;

    int m_phoneCount;
    int m_emailCount;
    int m_addressCount;

    QGraphicsGridLayout *m_headingGrid;
    MButton *m_favButton;
    MImageWidget *m_thumbnail;
    MTextEdit *m_firstNameEdit;
    MTextEdit *m_lastNameEdit;
    MTextEdit *m_companyEdit;

    QMap<MButton *, QList<QGraphicsLayoutItem *> > m_deleteMap;

    Q_DISABLE_COPY(PersonEditView)
};

#endif // PERSONEDITVIEW_H
