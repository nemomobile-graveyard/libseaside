/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "seasidecommspage.h"

#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MButton>
#include <MButtonGroup>
#include <MLabel>

#include <seaside.h>
#include <seasidelist.h>
#include <seasidepersonmodel.h>

SeasideCommsPage::SeasideCommsPage(SeasidePersonModel *pm,
                                 SeasideList::CommCat type)
{
    MLayout *layout = new MLayout();
    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    MButtonGroup *btnGroup = new MButtonGroup();
    bool useDetailType = false;
    QString headerText = QObject::tr("Select the %1 to %2:", "Prompt to chooser %1= number|address|chatcontact to %2 = call|SMS|email|IM");

    switch (type) {
    case SeasideList::CatCall:
        useDetailType = true;
        headerText = headerText.arg(QObject::tr("number", "Noun for phone number"), QObject::tr("call", "Verb describing action i.e. to call a number"));
        m_detailList.append(pm->phones().toList());
        break;

    case SeasideList::CatSMS:
        headerText = headerText.arg(QObject::tr("number", "Noun for phone number"), QObject::tr("SMS", "Verb describing action i.e. to SMS a number"));
        foreach (const SeasideDetail& detail, pm->phones()) {
            if (detail.location() == Seaside::LocationMobile)
                m_detailList.append(detail);
        }

        break;
    case SeasideList::CatEmail:
        headerText = headerText.arg(QObject::tr("address", "Noun for address"), QObject::tr("email","Verb describing action i.e. to email an address"));
        m_detailList.append(pm->emails().toList());
        break;

    case SeasideList::CatIM:
        headerText = headerText.arg(QObject::tr("chat contact", "Noun for im contact"), QObject::tr("IM","Verb describing action i.e. to IM a chat contact"));
        m_detailList.append(pm->emails().toList());
        break;
    }

    MLabel *headerLabel = new MLabel(headerText);
    headerLabel->setObjectName("SeasideCommsPageHeaderLabel");
    policy->addItem(headerLabel, Qt::AlignLeft);

    int i = 0;
    if (useDetailType) {
        QString type;
        foreach(const SeasideDetail& detail, m_detailList) {
            if (detail.location() == Seaside::LocationHome)
                type = QObject::tr("Home", "Location of communication home phone, home address, etc");
            else if (detail.location() == Seaside::LocationWork)
                type = QObject::tr("Work", "Location of communication work phone, work address, etc");
            else
                type = QObject::tr("Mobile", "Location of communication mobile phone, mobile email, etc");
            MButton *btn = new MButton(QObject::tr("%1 (%2)", "%1 = Communication data ie 321-3333 or jon@gmail.com, %2=Location of communication ie work or home").arg(detail.text(), type));
            btn->setObjectName("SeasideCommsPageDestButton");
            btnGroup->addButton(btn, i++);
            policy->addItem(btn, Qt::AlignCenter);
        }
    } else {
        foreach(const SeasideDetail& detail, m_detailList) {
            MButton *btn = new MButton(QObject::tr("%1", "Raw communication data ie 321-3333 or jon@gmail.com").arg(detail.text()));
            btn->setObjectName("SeasideCommsPageDestButton");
            btnGroup->addButton(btn, i++);
            policy->addItem(btn, Qt::AlignCenter);
        }
    }
    connect(btnGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(onButtonClicked(int)));

    layout->setPolicy(policy);
    setLayout(layout);
}

void SeasideCommsPage::onButtonClicked(int id)
{
    emit destSelected(m_detailList[id].text());
}
