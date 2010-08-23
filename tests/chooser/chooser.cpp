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

#include <QGraphicsLinearLayout>

#include <MApplication>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MAction>
#include <MTheme>

#include "seasidelist.h"
#include "chooser.h"

int main(int argc, char *argv[])
{
    MApplication app(argc, argv);

    MApplicationWindow window;
    window.show();

    MTheme::loadCSS("chooser.css");

    MApplicationPage page;
    QGraphicsLinearLayout *linear = new QGraphicsLinearLayout;
    linear->setContentsMargins(0, 0, 0, 0);
    linear->setSpacing(0);
    page.centralWidget()->setLayout(linear);
    
    SeasideList *list = new SeasideList(SeasideList::DetailPhone);
    linear->addItem(list);
    list->createSearchBar(&page);

    Target *target = new Target;
    MAction *m_actionSearch = new MAction("<b>Search</b>", target );  // TODO: i18n
    m_actionSearch->setLocation(MAction::ToolBarLocation);
    page.addAction(m_actionSearch);

    QObject::connect(m_actionSearch, SIGNAL(triggered()), 
		     list, SLOT(searchClicked()));
    QObject::connect(list, SIGNAL(contactSelected(QUuid)),
                     target, SLOT(contactClicked(QUuid)));
    QObject::connect(list, SIGNAL(emailsSelected(QStringList)),
                     target, SLOT(emailsClicked(QStringList)));
    QObject::connect(list, SIGNAL(imAccountsSelected(QStringList)),
                     target, SLOT(imsClicked(QStringList)));
    QObject::connect(list, SIGNAL(phonesSelected(QStringList)),
                     target, SLOT(phonesClicked(QStringList)));

    page.appear();

    return app.exec();
}

void Target::contactClicked(const QUuid& uuid)
{
    qWarning() << "Contact clicked:" << uuid.toString();
}

void Target::emailsClicked(const QStringList & emails)
{
   qWarning() << "Contact emails:" << emails;
}

void Target::phonesClicked(const QStringList & phones)
{
   qWarning() << "Contact phones:" << phones;
}

void Target::imsClicked(const QStringList & ims)
{
   qWarning() << "Contact ims:" << ims;
}

