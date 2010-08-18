/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDELIST_H
#define SEASIDELIST_H

#include <QUuid>

#include <MList>
#include "seaside.h"

class SeasideListPriv;
class SeasideSyncModel;
class SeasideProxyModel;
class SeasidePersonModel;
class QPersistentModelIndex;
class SeasidePerson;

/*
 * SeasideList: Styling Notes
 *
 * The object name is set to SeasideList by default, but has no special styles
 * beyond those inherited from MList and MWidget
 *
 * See seasidelistitem.h for information on styling individual list items
 *
 */

class SeasideList: public MList
{
    Q_OBJECT

public:
  enum Detail {
        DetailEmail,
        DetailPhone,
	DetailIM,
        DetailNone
    };

    SeasideList(Detail detail = DetailNone, MWidget *parent = NULL);
    virtual ~SeasideList();

    SeasideSyncModel *sourceModel();
    SeasideProxyModel *proxyModel();

signals:
    void contactSelected(const QUuid& uuid);
    void imAccountsSelected(const QStringList accounts);
    void emailsSelected(const QStringList emails);
    void phonesSelected(const QStringList numbers);

   void editRequest(const QModelIndex& index);
   void viewRequest(qreal ypos, qreal height);

protected slots:
    void handleClick(const QModelIndex& index);
    void handleDetailClick(const QModelIndex &index);
    void createDetailPage(const QModelIndex &index); 
    /*void searchClicked();
    void searchChanged();
    void searchClear();
    void searchCancel();
    void searchCommit();*/
    void detailBack();
    //    void commBack();
    void editCurrent();
    void createEditPage(const QModelIndex& index = QModelIndex(),
                        const QString& title = QString());
    void editModified();
    void editSave();
    void editBack();
    void callNumber(const QString& number);
    void composeSMS(const QString& number);
    void composeEmail(const QString& address);
    //void repositionOverlays();
    //void scrollTo(qreal pos);
    void scrollIntoView(qreal ypos, qreal height);

private:
    SeasideListPriv *priv;
    SeasideList::Detail detailType;
    QPersistentModelIndex m_currentIndex;
    MApplicationPage *m_detailPage;
    MApplicationPage *m_editPage;
    SeasidePerson *m_currentPerson;
    Q_DISABLE_COPY(SeasideList);

    //SeasideWindow *m_window;
    //MApplicationPage *m_mainPage;
   //PersonCommsPage *m_commPage;

    //QGraphicsWidget *m_topSpacer;
    //QGraphicsWidget *m_bottomSpacer;

    //SeasidePeople *m_people;

    //MWidgetController *m_searchWidget;
    //MTextEdit *m_searchEdit;

    SeasidePersonModel *m_editModel;
    bool m_editModelModified;
};

#endif // SEASIDELIST_H
