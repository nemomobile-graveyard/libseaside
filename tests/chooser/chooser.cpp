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
    
    SeasideList *list = new SeasideList;
    linear->addItem(list);

    Target *target = new Target;
    QObject::connect(list, SIGNAL(contactSelected(QUuid)),
                     target, SLOT(contactClicked(QUuid)));

    page.appear();

    return app.exec();
}

void Target::contactClicked(const QUuid& uuid)
{
    qCritical() << "Contact clicked:" << uuid.toString();
}
