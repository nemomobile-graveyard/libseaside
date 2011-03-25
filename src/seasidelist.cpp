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

#include <QGraphicsGridLayout>

#include <MAbstractCellCreator>

#include "seasidelist.h"
#include "seasideperson.h"
#include "seasidelistitem.h"
#include "seasidesyncmodel.h"
#include "seasideproxymodel.h"
#include "seasidecellcreators.h"

#include <MWidgetCreator>
#include <QtDBus/QtDBus>
#include <MAction>
#include <MApplicationPage>
#include <MTextEdit>
#include <MButton>
#include  <QGraphicsLinearLayout>

#include <seasidepersonmodel.h>

M_REGISTER_WIDGET(SeasideList);

class SeasideListPriv
{
public:
    SeasideSyncModel *sourceModel;
    SeasideProxyModel *proxyModel;
};

SeasideList::SeasideList(Detail detail, SeasideCard view, MWidget *parent):
        MList(parent), detailType(detail), m_listView(view)
{
    m_detailPage = NULL;
    m_editPage = NULL;
    m_mainPage = NULL;
    //m_commPage = NULL;

    m_bottomSpacer = NULL;

    m_searchWidget = NULL;
    m_searchEdit = NULL;

    m_currentPerson = NULL;
    m_editModel = NULL;
    m_editModelModified = NULL; 

    priv = new SeasideListPriv;
    priv->sourceModel = SeasideSyncModel::instance();
    priv->proxyModel = new SeasideProxyModel;
    priv->proxyModel->sort(0);  // enable custom sorting
    priv->proxyModel->setSourceModel(priv->sourceModel);

    setItemModel(priv->proxyModel);

    setObjectName("SeasideList");
    setViewType("fast");
     
     if(detail == SeasideList::DetailPhone){
      if(view == SeasideList::SmallCard){
           setCellCreator(new CellCreatorPhoneSmall);
      }else{
           setCellCreator(new CellCreatorPhoneLarge);
      }
    } else if(detail == SeasideList::DetailEmail){
      if(view == SeasideList::SmallCard){
          setCellCreator(new CellCreatorEmailSmall);
      }else{
          setCellCreator(new CellCreatorEmailLarge);
      }     
    }else if(detail == SeasideList::DetailIM){
      if(view == SeasideList::SmallCard){
              setCellCreator(new CellCreatorIMSmall);
      }else{
              setCellCreator(new CellCreatorIMLarge);
      }           
    }else if(detail == SeasideList::DetailPhoto){
        if(view == SeasideList::SmallCard){
                setCellCreator(new CellCreatorPhotoSmall);
        }else{
                setCellCreator(new CellCreatorPhotoLarge);
        }
    }else{
           setCellCreator(new CellCreator);
    }

    connect(this, SIGNAL(itemClicked(QModelIndex)),
            this, SLOT(handleClick(QModelIndex)));
    connect(this, SIGNAL(itemClicked(QModelIndex)),
            this, SLOT(handleDetailClick(QModelIndex)));

    connect(this, SIGNAL(itemClicked(QModelIndex)),
         this, SLOT(createDetailPage(QModelIndex)));
     connect(this, SIGNAL(editRequest(QModelIndex)),
         this, SLOT(createEditPage(QModelIndex)));

}

SeasideList::~SeasideList()
{
    SeasideSyncModel::releaseInstance();
    delete priv->proxyModel;
    delete priv;
}


/*void SeasideList::createCommPage(SeasidePersonModel *pm, CommCat type)
{
    //if (m_commPage)
    //    return;

    searchCancel();

    //m_commPage = new SeasideCommsPage(pm, type);

   // connect(m_commPage, SIGNAL(backButtonClicked()),
    //        this, SLOT(commBack()));

    switch (type) {
    case CatCall:
        connect(m_commPage, SIGNAL(destSelected(QString)),
                this, SLOT(callNumber(QString)));
        break;

    case CatSMS:
        connect(m_commPage, SIGNAL(destSelected(QString)),
                this, SLOT(composeSMS(QString)));
        break;

    case CatEmail:
        connect(m_commPage, SIGNAL(destSelected(QString)),
                this, SLOT(composeEmail(QString)));
        break;
    }
    connect(m_commPage, SIGNAL(destSelected(QString)),
            this, SLOT(commBack()));

    m_commPage->appear(MApplicationPage::DestroyWhenDismissed);
    }
*/

void SeasideList::createSearchBar(MApplicationPage * page)
{

    m_mainPage = page;
    if(m_mainPage != NULL){

     connect(m_mainPage, SIGNAL(exposedContentRectChanged()),
                this, SLOT(repositionOverlays()));

    QGraphicsLinearLayout *linear = new QGraphicsLinearLayout(Qt::Vertical);
    linear->setContentsMargins(0, 0, 0, 0);
    linear->setSpacing(0);
    linear->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    m_mainPage->centralWidget()->setLayout(linear);

    m_bottomSpacer = new QGraphicsWidget;
    m_bottomSpacer->setPreferredHeight(0);
    linear->addItem(m_bottomSpacer); 

    m_searchWidget = new MWidgetController;
    m_searchWidget->setViewType("background");
    m_searchWidget->setObjectName("SeasideSearch");
    m_searchWidget->setParentItem(m_mainPage);
    m_searchWidget->setMinimumWidth(600);
    m_searchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    m_searchWidget->setZValue(1);
    m_searchWidget->hide();
  
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal, m_searchWidget);
    layout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    m_searchEdit = new MTextEdit;
    m_searchEdit->setObjectName("SeasideSearchEdit");
    m_searchEdit->setPreferredWidth(400);
    m_searchEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    m_searchEdit->setPrompt(QObject::tr("Tap to start searching people","List widget's prompt for search default text"));
    layout->addItem(m_searchEdit);
    layout->setAlignment(m_searchEdit, Qt::AlignVCenter);
    connect(m_searchEdit, SIGNAL(returnPressed()), this, SLOT(searchCommit()));

    // uncomment this line to enable dynamic search
    connect(m_searchEdit, SIGNAL(textChanged()), this, SLOT(searchChanged()));

    MButton *button = new MButton();
    button->setIconID("icon-m-framework-close-alt");
    button->setObjectName("SeasideSearchButton");
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    layout->addItem(button);
    layout->setAlignment(button, Qt::AlignVCenter);
    connect(button, SIGNAL(clicked()), this, SLOT(searchClear()));

    button = new MButton(QObject::tr("<b>Search</b>", "Search button for list widget"));
    button->setObjectName("SeasideSearchButton");
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    layout->addItem(button);
    layout->setAlignment(button, Qt::AlignVCenter);
    connect(button, SIGNAL(clicked()), this, SLOT(searchCommit()));

    m_mainPage->appear();
    repositionOverlays();

}else{
      qWarning() << "SeasideList::createSearchBar: m_mainPage is NULL ";
    }
}

void SeasideList::searchClicked()
{
    m_searchWidget->show();
    m_bottomSpacer->setPreferredHeight(m_searchWidget->size().height());
}

void SeasideList::searchClear()
{
    if (m_searchEdit) {
        m_searchEdit->clear();
        m_searchEdit->setFocus();
    }
}

void SeasideList::searchChanged()
{
    if (m_searchEdit)
       filterSearch(m_searchEdit->text());
}

void SeasideList::filterSearch(const QString& text)
{
    if (priv->proxyModel) {
        priv->proxyModel->setFilter(SeasideProxyModel::FilterAll);
        priv->proxyModel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive,
                                              QRegExp::FixedString));
    }
}

void SeasideList::searchCancel()
{
    if (m_searchEdit)
        m_searchEdit->clearFocus();

    if (m_searchWidget) {
        m_searchWidget->hide();
        m_bottomSpacer->setPreferredHeight(0);
    }
}

void SeasideList::searchCommit()
{
    searchChanged();
    searchCancel();
    }

void SeasideList::commBack()
{
    /*if (m_commPage) {
        m_commPage->dismiss();
        m_commPage = NULL;
    }*/
}

void SeasideList::handleClick(const QModelIndex &index)
{
    SEASIDE_SHORTCUTS
    SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row())
   emit contactSelected(QUuid(SEASIDE_FIELD(Uuid, String)));
}

void SeasideList::handleDetailClick(const QModelIndex &index)
{
  SEASIDE_SHORTCUTS
    SEASIDE_SET_MODEL_AND_ROW(index.model(), index.row())
  
  if(detailType == SeasideList::DetailPhone){  
    emit phonesSelected(QStringList(SEASIDE_FIELD(PhoneNumbers, StringList)));
  } else if(detailType == SeasideList::DetailEmail){      
    emit emailsSelected(QStringList(SEASIDE_FIELD(EmailAddresses, StringList)));
  }else if(detailType == SeasideList::DetailIM){ 
    emit imAccountsSelected(QStringList(SEASIDE_FIELD(IMAccounts, StringList)));
 }else if(detailType == SeasideList::DetailPhoto){
        emit photoContactSelected(QUuid(SEASIDE_FIELD(Uuid, String)));
  } else{
    emit phonesSelected(QStringList(SEASIDE_FIELD(PhoneNumbers, StringList))) ;
  }
}

void SeasideList::createDetailPage(const QModelIndex &index)
{
    if (m_detailPage)
            return;

        if (!index.isValid())
            return;

        searchCancel();

        m_currentIndex = index;

        m_currentPerson = new SeasidePerson(index);
        m_currentPerson->setViewType("seasideDetailView");

        connect(m_currentPerson, SIGNAL(callNumber(const QString&)),
                this, SLOT(callNumber(const QString&)));
        connect(m_currentPerson, SIGNAL(composeSMS(const QString&)),
                this, SLOT(composeSMS(const QString&)));
        connect(m_currentPerson, SIGNAL(composeEmail(const QString&)),
                this, SLOT(composeEmail(const QString&)));
	/*connect(m_currentPerson, SIGNAL(composeIM(const QString&)),
	  this, SLOT(composeIM(const QString&)));      */ 
        connect(m_currentPerson, SIGNAL(viewRequest(qreal,qreal)),
                this, SLOT(scrollIntoView(qreal,qreal)));

        m_detailPage = new MApplicationPage;
        m_detailPage->setTitle(QObject::tr("Contact Detail", "Screen title for the MApplicationPage" ));
        m_detailPage->setCentralWidget(m_currentPerson);

        MAction *action = new MAction(QObject::tr("<b>Edit</b>", "Edit button text for list widget"), this);  
        action->setLocation(MAction::ToolBarLocation);
        m_detailPage->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(editCurrent()));

        connect(m_detailPage, SIGNAL(backButtonClicked()), this, SLOT(detailBack()));

        m_detailPage->appear(MApplicationPage::DestroyWhenDismissed);
}

void SeasideList::callNumber(const QString& number)
{
    qDebug() << "Attempting call to:" << number;

    // hard-coded details of the MeeGo Dialer application
    QDBusInterface dialer("com.meego.dialer", "/com/meego/dialer",
                          "com.meego.dialer");
    if (!dialer.isValid()) {
        qWarning() << "Dialing" << number << "- could not find dialer app";
        return;
    }

    QDBusReply<void> reply = dialer.call(QDBus::BlockWithGui, "call", number);
    if (!reply.isValid())
        qWarning() << "Dialing" << number << "failed:" <<
                reply.error().message();
}

void SeasideList::composeSMS(const QString& number)
{
    qDebug() << "Attempting to compose SMS to" << number;

    // hard-coded details of the MeeGo SMS application
    QDBusInterface sms("com.meego.sms", "/", "com.meego.sms");
    if (!sms.isValid()) {
        qWarning() << "Composing SMS to" << number << "- could not find SMS app";
        return;
    }

    QDBusReply<void> reply = sms.call(QDBus::BlockWithGui, "showdialogpage", number);
    if (!reply.isValid())
        qWarning() << "Composing SMS to" << number << "failed:" <<
                reply.error().message();
}

void SeasideList::composeEmail(const QString& address)
{
    qDebug() << "Composing email to" << address << "- not implemented";
}

/*void SeasideList::composeIM(const QString& address)
{
    qDebug() << "Composing IM to" << address << "- not implemented";
    }*/

/*void SeasideList::scrollIntoView(qreal ypos, qreal height)
{
    // effects: scrolls the defined vertical area into view, or as much as possible

    MApplicationPage *page = activeApplicationWindow()->currentPage();
    QGraphicsWidget *widget = page->centralWidget();
    MPannableWidget *viewport = page->pannableViewport();
    if (!widget || !viewport)
        return;

    QRectF exposed = page->exposedContentRect();
    int viewportHeight = exposed.height();

    qreal scrollpos = viewport->physics()->position().y();

    // check to see if it's already fully visible
    if (scrollpos < ypos && scrollpos + viewportHeight > ypos + height)
        return;

    // if bottom end is off-screen, scroll up until bottom is visible
    qreal scrollto = scrollpos;
    if (scrollpos + viewportHeight < ypos + height)
        scrollto += ypos + height - scrollpos - viewportHeight;

    // if top end is off-screen, scroll down until top is visible
    if (scrollto > ypos)
        scrollto = ypos;

    viewport->physics()->stop();
    viewport->physics()->setPosition(QPointF(0, scrollto));
}
*/
void SeasideList::detailBack()
{
    m_currentIndex = QModelIndex();
    m_currentPerson = NULL;
    if (m_detailPage) {
        m_detailPage->dismiss();
        m_detailPage = NULL;
    }
}

void SeasideList::editCurrent()
{
    createEditPage(m_currentIndex);
}

void SeasideList::createEditPage(const QModelIndex &index, const QString& title)
{
    if (m_editPage)
        return;

    QModelIndex useIndex;

    if (index.isValid())
        useIndex = index;
    else
        useIndex = m_currentIndex;

    if (useIndex.isValid())
        m_editModel = SeasideSyncModel::createPersonModel(useIndex);
    else  // create empty edit model to add new contact
        m_editModel = new SeasidePersonModel;
    MWidgetController *person = new MWidgetController(m_editModel);
    person->setViewType("seasideEdit");

    m_editModelModified = false;
    connect(m_editModel, SIGNAL(modified(QList<const char*>)),
            this, SLOT(editModified()));

    m_editPage = new MApplicationPage;
    if (title.isNull())
        m_editPage->setTitle(QObject::tr("Edit Contact", "Edit screen title for toolbar"));
    else
        m_editPage->setTitle(title);
    m_editPage->setCentralWidget(person);

    MAction *action = new MAction(QObject::tr("<b>Save</b>", "Edit screen button text to save changes"), this);  // TODO: i18n
    action->setLocation(MAction::ToolBarLocation);
    m_editPage->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(editSave()));

    connect(m_editPage, SIGNAL(backButtonClicked()), this, SLOT(editBack()));

    m_editPage->appear(MApplicationPage::DestroyWhenDismissed);
}

void SeasideList::editModified()
{
    m_editModelModified = true;
}

void SeasideList::editSave()
{
    if (m_editModel && m_editModelModified && !m_editModel->isEmpty()) {
        priv->sourceModel->updatePerson(m_editModel);
        if (m_currentPerson)
            m_currentPerson->setModel(priv->sourceModel->createPersonModel(m_editModel->uuid()));
        m_editPage->dismiss();
        m_editPage = NULL;
    }
}

void SeasideList::editBack()
{
    if (m_editPage) {
        m_editPage->dismiss();
        m_editPage = NULL;
    }
}

void SeasideList::repositionOverlays()
{
    if(m_mainPage){
    QRectF exposed = m_mainPage->exposedContentRect();

    if (m_searchWidget)
        m_searchWidget->setPos(0, exposed.bottom() - m_searchWidget->preferredHeight());
 }
}

