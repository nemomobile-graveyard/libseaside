/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "personcommspage.h"

#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MButton>
#include <MButtonGroup>
#include <MLabel>

#include <seaside.h>
#include<seasidelist.h>
#include <seasidepersonmodel.h>

PersonCommsPage::PersonCommsPage(SeasidePersonModel *pm,
                                 SeasideList::CommCat type)
{
    MLayout *layout = new MLayout();
    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    MButtonGroup *btnGroup = new MButtonGroup();
    bool useDetailType = false;
    QString headerText = QString("Select the %1 to %2:");

    switch (type) {
    case SeasideList::CatCall:
        useDetailType = true;
        headerText = headerText.arg("number", "call");
        m_detailList.append(pm->phones().toList());
        break;

    case SeasideList::CatSMS:
        headerText = headerText.arg("number", "SMS");
        foreach (const SeasideDetail& detail, pm->phones()) {
            if (detail.location() == Seaside::LocationMobile)
                m_detailList.append(detail);
        }

        break;
    case SeasideList::CatEmail:
        headerText = headerText.arg("address", "Email");
        m_detailList.append(pm->emails().toList());
        break;
    }

    MLabel *headerLabel = new MLabel(headerText);
    headerLabel->setObjectName("PersonCommsPageHeaderLabel");
    policy->addItem(headerLabel, Qt::AlignLeft);

    int i = 0;
    if (useDetailType) {
        QString type;
        foreach(const SeasideDetail& detail, m_detailList) {
            if (detail.location() == Seaside::LocationHome)
                type = "Home";  // TODO: i18n;
            else if (detail.location() == Seaside::LocationWork)
                type = "Work";  // TODO: i18n;
            else
                type = "Mobile";  // TODO: i18n;
            MButton *btn = new MButton(QString("%1 (%2)").arg(detail.text(), type));
            btn->setObjectName("PersonCommsPageDestButton");
            btnGroup->addButton(btn, i++);
            policy->addItem(btn, Qt::AlignCenter);
        }
    } else {
        foreach(const SeasideDetail& detail, m_detailList) {
            MButton *btn = new MButton(QString("%1").arg(detail.text()));
            btn->setObjectName("PersonCommsPageDestButton");
            btnGroup->addButton(btn, i++);
            policy->addItem(btn, Qt::AlignCenter);
        }
    }
    connect(btnGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(onButtonClicked(int)));

    layout->setPolicy(policy);
    setLayout(layout);
}

void PersonCommsPage::onButtonClicked(int id)
{
    emit destSelected(m_detailList[id].text());
}
