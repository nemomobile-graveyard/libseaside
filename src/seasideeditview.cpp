/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QDebug>

#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>

#include <MButton>
#include <MTextEdit>
#include <MComboBox>
#include <MImageWidget>

#include <MLayout>
#include <MGridLayoutPolicy>
#include <MLinearLayoutPolicy>
#include <MBasicLayoutAnimation>

#include <seasidelabel.h>

#include "seasideeditview.h"
#include "imagelabel.h"

static SeasideLabel *createHeader(const QString &text) {
    SeasideLabel *label = new SeasideLabel(text);
    label->setObjectName("EditHeader");
    return label;
}

SeasideEditView::SeasideEditView(MWidgetController *controller): MWidgetView(controller)
{
    m_phoneCount = 0;
    m_emailCount = 0;
    m_addressCount = 0;

    MLayout *layout = new MLayout(controller);
    m_mainPolicy = new MGridLayoutPolicy(layout);

    // row 0: name/company header
    MWidgetController *widget = new MWidgetController;
    widget->setObjectName("SeasideEditHeaderBox");
    widget->setViewType("background");
    widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_mainPolicy->addItem(widget, 0, 0);

    m_headingGrid = new QGraphicsGridLayout(widget);
    m_headingGrid->setContentsMargins(0, 0, 0, 0);
    m_headingGrid->setSpacing(0);

    // header consists of thumbnail chooser button, name edit, and company edit
    m_thumbnail = NULL;

    m_firstNameEdit = new MTextEdit;
    m_firstNameEdit->setPrompt(QObject::tr("First name", "Edit screen prompt for first name"));
    m_headingGrid->addItem(m_firstNameEdit, 0, 1, 1, 1);
    connect(m_firstNameEdit, SIGNAL(textChanged()),
            this, SLOT(textChanged()));

    m_lastNameEdit = new MTextEdit;
    m_lastNameEdit->setPrompt(QObject::tr("Last name", "Edit screen prompt for last name"));
    m_headingGrid->addItem(m_lastNameEdit, 0, 2, 1, 1);
    connect(m_lastNameEdit, SIGNAL(textChanged()),
            this, SLOT(textChanged()));

    m_companyEdit = new MTextEdit;
    m_companyEdit->setPrompt(QObject::tr("Company", "Edit screen prompt for company name"));
    m_headingGrid->addItem(m_companyEdit, 1, 1, 1, 2);
    connect(m_companyEdit, SIGNAL(textChanged()),
            this, SLOT(textChanged()));

    m_favButton = new MButton;
    m_favButton->setObjectName("SeasideEditButtonSmall");
    m_headingGrid->addItem(m_favButton, 0, 3, 2, 1, Qt::AlignTop);
    connect(m_favButton, SIGNAL(clicked()), this, SLOT(favoriteClicked()));

    // row 1: recent section heading
    // row 2: recent section items

    // row 3: phone number section heading
    SeasideLabel *header = createHeader(QObject::tr("Telephone numbers", "Edit screen header for phone numbers"));
    m_mainPolicy->addItem(header, 3, 0);

    // row 4: phone number section items
    widget = new MWidgetController;
    widget->setObjectName("SeasideEditBox");
    widget->setViewType("background");
    m_mainPolicy->addItem(widget, 4, 0);

    QGraphicsLinearLayout *linear = new QGraphicsLinearLayout(Qt::Vertical, widget);
    linear->setContentsMargins(0, 0, 0, 0);
    linear->setSpacing(0);
    layout = new MLayout();
    linear->addItem(layout);
    m_phonePolicy = new MGridLayoutPolicy(layout);

    MButton *button = new MButton(QObject::tr("<b>+ Add a Number</b>", "Edit View Button text to add a phone number"));
    button->setObjectName("SeasideEditButton");
    linear->addItem(button);
    connect(button, SIGNAL(clicked()), this, SLOT(newPhone()));

    // row 5: email section heading
    header = createHeader(QObject::tr("Email", "Edit view header for emails"));
    m_mainPolicy->addItem(header, 5, 0);

    // row 6: email section items
    widget = new MWidgetController;
    widget->setObjectName("SeasideEditBox");
    widget->setViewType("background");
    m_mainPolicy->addItem(widget, 6, 0);

    linear = new QGraphicsLinearLayout(Qt::Vertical, widget);
    linear->setContentsMargins(0, 0, 0, 0);
    linear->setSpacing(0);
    layout = new MLayout();
    linear->addItem(layout);
    m_emailPolicy = new MGridLayoutPolicy(layout);

    button = new MButton(QObject::tr("<b>+ Add an Email</b>", "Edit View Button text to add a emails"));
    button->setObjectName("SeasideEditButton");
    linear->addItem(button);
    connect(button, SIGNAL(clicked()), this, SLOT(newEmail()));

    // row 7: address section heading
    header = createHeader(QObject::tr("Addresses", "Edit view header for addresses"));
    m_mainPolicy->addItem(header, 7, 0);

    // row 8: address section items
    widget = new MWidgetController;
    widget->setObjectName("SeasideEditBox");
    widget->setViewType("background");
    m_mainPolicy->addItem(widget, 8, 0);

    linear = new QGraphicsLinearLayout(Qt::Vertical, widget);
    linear->setContentsMargins(0, 0, 0, 0);
    linear->setSpacing(0);
    layout = new MLayout();
    linear->addItem(layout);
    m_addressPolicy = new MGridLayoutPolicy(layout);

    button = new MButton(QObject::tr("<b>+ Add an Address</b>", "Edit View Button text to add a emails"));
    button->setObjectName("SeasideEditButton");
    linear->addItem(button);
    connect(button, SIGNAL(clicked()), this, SLOT(newAddress()));
}

SeasideEditView::~SeasideEditView()
{
}

void SeasideEditView::init()
{
    QString str;
    SeasidePersonModel *pm = model();

    if (m_thumbnail) {
        delete m_thumbnail;
        m_thumbnail = NULL;
    }

    if (!pm->thumbnail().isEmpty()) {
        m_thumbnail = new MImageWidget(pm->thumbnail());
        m_thumbnail->setObjectName("SeasideContactThumbnail");
        m_thumbnail->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_headingGrid->addItem(m_thumbnail, 0, 0, 2, 1, Qt::AlignVCenter);
    }

    m_firstNameEdit->setText(pm->firstname());
    m_lastNameEdit->setText(pm->lastname());
    m_companyEdit->setText(pm->company());
    favoriteClicked(true);

    // add phone numbers
    foreach (const SeasideDetail& detail, pm->phones())
        newPhone(detail.text(), detail.location());

    // add email addresses
    foreach (const SeasideDetail& detail, pm->emails())
        newEmail(detail.text());

    // add addresses
    foreach (const SeasideDetail& detail, pm->addresses())
        newAddress(detail.text(), detail.location());
}

void SeasideEditView::setupModel()
{
    MWidgetView::setupModel();
    init();
}

static int findRowWithItem(MGridLayoutPolicy *grid, QGraphicsLayoutItem *item)
{
    int rows = grid->rowCount();
    int cols = grid->columnCount();

    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            QGraphicsLayoutItem *li = grid->itemAt(i, j);
            if (li == item)
                return i;

            // recurse if there is another gridlayout
            MLayout *layout = dynamic_cast<MLayout *>(li);
            if (layout) {
                MGridLayoutPolicy *gi;
                gi = dynamic_cast<MGridLayoutPolicy *>(layout->policy());
                if (gi && findRowWithItem(gi, item) != -1)
                    return i;
            }
        }
    }
    return -1;
}

MComboBox *createPhoneCombo()
{
    MComboBox *combo = new MComboBox;
    combo->setTitle(QObject::tr("Phone Type", "Combo box title to select type of phone number Work|Home|Other|Mobile"));
    QStringList list;
    list << QObject::tr("Mobile", "Phone Type for cell phone numbers")
            << QObject::tr("Home","Phone type for home/landline phone numbers")
            << QObject::tr("Work", "Phone Type for business/work numbers")
           << QObject::tr("Other", "Phone Type for other type of numbers");
    combo->addItems(list);
    return combo;
}

MComboBox *createAddressCombo()
{
    MComboBox *combo = new MComboBox;
    combo->setTitle(QObject::tr("Address Type","Edit view Combo box title to select type of address Work| Home| Other"));
    QStringList list;
    list << QObject::tr("Home","Address type for home address type")
            << QObject::tr("Work", "AddressType for business/work address type")
           << QObject::tr("Other", "AddressType for other address type");
    combo->addItems(list);
    return combo;
}

void SeasideEditView::newPhone(const QString& phone, Seaside::Location location)
{
    MTextEdit *edit = new MTextEdit;
    edit->setContentType(M::PhoneNumberContentType);
    edit->setPrompt(QObject::tr("Phone number", "Edit screen prompt for phone number"));
    if (!phone.isEmpty())
        edit->setText(phone);
    m_phonePolicy->addItem(edit, m_phoneCount, 0, Qt::AlignVCenter);
    connect(edit, SIGNAL(textChanged()), this, SLOT(textChanged()));

    MComboBox *combo = createPhoneCombo();
    if (location < Seaside::LocationNone)
        combo->setCurrentIndex(location);
    m_phonePolicy->addItem(combo, m_phoneCount, 1, Qt::AlignVCenter);
    connect(combo, SIGNAL(activated(int)), this, SLOT(activated(int)));

    MButton *button = new MButton();
    button->setObjectName("SeasideEditButtonSmall");
    button->setIconID("icon-m-framework-close-alt");
    m_phonePolicy->addItem(button, m_phoneCount, 2, Qt::AlignVCenter);
    connect(button, SIGNAL(clicked()), this, SLOT(deletePhone()));

    // for now, count never decreases, it's the index of the next row to add
    m_phoneCount++;

    // record what should be deleted when the delete button is pressed
    QList<QGraphicsLayoutItem *> list;
    list << edit << combo << button;
    m_deleteMap[button] = list;
}

void SeasideEditView::newEmail(const QString& email)
{
    MTextEdit *edit = new MTextEdit;
    edit->setContentType(M::EmailContentType);
    edit->setPrompt(QObject::tr("Email address", "Edit screen prompt for email addresses"));
    if (!email.isEmpty())
        edit->setText(email);
    m_emailPolicy->addItem(edit, m_emailCount, 0, Qt::AlignVCenter);
    connect(edit, SIGNAL(textChanged()), this, SLOT(textChanged()));

    MButton *button = new MButton();
    button->setObjectName("SeasideEditButtonSmall");
    button->setIconID("icon-m-framework-close-alt");
    m_emailPolicy->addItem(button, m_emailCount, 1, Qt::AlignVCenter);
    connect(button, SIGNAL(clicked()), this, SLOT(deleteEmail()));

    m_emailCount++;

    // record what should be deleted when the delete button is pressed
    QList<QGraphicsLayoutItem *> list;
    list << edit << button;
    m_deleteMap[button] = list;
}

void SeasideEditView::newAddress(const QString& address, Seaside::Location location)
{
    QStringList fields = address.split(QChar('\n'));
    while (fields.size() < 5)
        fields << "";
    const QString& street  = fields[0];
    const QString& city    = fields[1];
    const QString& state   = fields[2];
    const QString& zip     = fields[3];
    const QString& country = fields[4];

    // record what should be deleted when the delete button is pressed
    QList<QGraphicsLayoutItem *> list;

    MLayout *layout = new MLayout;
    list << layout;
    MGridLayoutPolicy *landscapePolicy = new MGridLayoutPolicy(layout);
    MGridLayoutPolicy *portraitPolicy = new MGridLayoutPolicy(layout);
    layout->setLandscapePolicy(landscapePolicy);
    layout->setPortraitPolicy(portraitPolicy);
    landscapePolicy->setObjectName("EditAddressLandscapePolicy");
    portraitPolicy->setObjectName("EditAddressPortraitPolicy");

    MTextEdit *edit = new MTextEdit;
    list << edit;
    edit->setPrompt(QObject::tr("Street address", "Edit view prompt for street address"));
    if (!street.isEmpty())
        edit->setText(street);
    landscapePolicy->addItem(edit, 0, 0, 1, 2);
    portraitPolicy->addItem(edit, 0, 0, 1, 1);
    connect(edit, SIGNAL(textChanged()), this, SLOT(textChanged()));

    MComboBox *combo = createAddressCombo();
    list << combo;
    if (location < Seaside::LocationNone)
        combo->setCurrentIndex(location - 1);
    landscapePolicy->addItem(combo, 0, 2, 3, 1);
    portraitPolicy->addItem(combo, 0, 1, 2, 1);
    connect(combo, SIGNAL(activated(int)), this, SLOT(activated(int)));

    MButton *button = new MButton();
    list << button;
    button->setObjectName("SeasideEditButtonSmall");
    button->setIconID("icon-m-framework-close-alt");
    landscapePolicy->addItem(button, 0, 3, 3, 1);
    portraitPolicy->addItem(button, 2, 1, 3, 1, Qt::AlignBottom);
    connect(button, SIGNAL(clicked()), this, SLOT(deleteAddress()));

    edit = new MTextEdit;
    list << edit;
    edit->setPrompt(QObject::tr("City", "Edit view prompt for city"));
    if (!city.isEmpty())
        edit->setText(city);
    landscapePolicy->addItem(edit, 1, 0);
    portraitPolicy->addItem(edit, 1, 0);
    connect(edit, SIGNAL(textChanged()), this, SLOT(textChanged()));

    edit = new MTextEdit;
    list << edit;
    edit->setPrompt(QObject::tr("State", "Edit view prompt for state"));
    if (!state.isEmpty())
        edit->setText(state);
    landscapePolicy->addItem(edit, 1, 1);
    portraitPolicy->addItem(edit, 2, 0);
    connect(edit, SIGNAL(textChanged()), this, SLOT(textChanged()));

    edit = new MTextEdit;
    list << edit;
    edit->setPrompt(QObject::tr("ZIP Code", "Edit view prompt for zip"));
    if (!zip.isEmpty())
        edit->setText(zip);
    landscapePolicy->addItem(edit, 2, 0);
    portraitPolicy->addItem(edit, 3, 0);
    connect(edit, SIGNAL(textChanged()), this, SLOT(textChanged()));

    edit = new MTextEdit;
    list << edit;
    edit->setPrompt(QObject::tr("Country", "Edit view promp for country"));
    if (!country.isEmpty())
        edit->setText(country);
    landscapePolicy->addItem(edit, 2, 1);
    portraitPolicy->addItem(edit, 4, 0);
    connect(edit, SIGNAL(textChanged()), this, SLOT(textChanged()));

    m_addressPolicy->addItem(layout, m_addressCount, 0, Qt::AlignVCenter);

    m_addressCount++;

    m_deleteMap[button] = list;
}

static int deleteItems(QObject *sender, MGridLayoutPolicy *policy,
                       QMap<MButton *, QList<QGraphicsLayoutItem *> > deleteMap)
{
    MButton *button = dynamic_cast<MButton *>(sender);
    if (!button)
        return -1;

    int row = findRowWithItem(policy, button);
    if (row != -1) {
        foreach (QGraphicsLayoutItem *item, deleteMap[button])
            delete item;
        deleteMap.remove(button);
    }

    return row;
}

void SeasideEditView::deletePhone()
{
    int row = deleteItems(sender(), m_phonePolicy, m_deleteMap);
    if (row != -1) {
        SeasidePersonModel *pm = model();
        pm->setPhone(row, SeasideDetail());
    }
}

void SeasideEditView::deleteEmail()
{
    int row = deleteItems(sender(), m_emailPolicy, m_deleteMap);
    if (row != -1) {
        SeasidePersonModel *pm = model();
        pm->setEmail(row, SeasideDetail());
    }
}

void SeasideEditView::deleteAddress()
{
    int row = deleteItems(sender(), m_addressPolicy, m_deleteMap);
    if (row != -1) {
        SeasidePersonModel *pm = model();
        pm->setAddress(row, SeasideDetail());
    }
}

static QString readTextEdit(MGridLayoutPolicy *policy, int row, int col)
{
    QString text;
    if (policy) {
        MTextEdit *edit = dynamic_cast<MTextEdit *>(policy->itemAt(row, col));
        if (edit)
            text = edit->text();
        else
            qWarning() << "[SeasideEditView] expected a text edit";
    }
    return text;
}

void SeasideEditView::updatePhone(int row)
{
    Seaside::Location location = Seaside::LocationNone;
    MComboBox *combo = dynamic_cast<MComboBox *>(m_phonePolicy->itemAt(row, 1));
    if (combo)
        location = Seaside::Location(combo->currentIndex());

    QString number = readTextEdit(m_phonePolicy, row, 0);

    SeasidePersonModel *pm = model();
    pm->setPhone(row, SeasideDetail(number, location));
}

void SeasideEditView::updateEmail(int row)
{
    QString email = readTextEdit(m_emailPolicy, row, 0);

    SeasidePersonModel *pm = model();
    pm->setEmail(row, SeasideDetail(email));
}

void SeasideEditView::updateAddress(int row)
{
    Seaside::Location location = Seaside::LocationNone;
    QStringList list;

    MLayout *layout = dynamic_cast<MLayout *>(m_addressPolicy->itemAt(row, 0));
    MGridLayoutPolicy *policy = NULL;
    if (layout)
        policy = dynamic_cast<MGridLayoutPolicy *>(layout->landscapePolicy());

    if (policy) {
        MComboBox *combo = dynamic_cast<MComboBox *>(policy->itemAt(0, 2));
        if (combo)
            location = Seaside::Location(combo->currentIndex() + 1);
    }

    list << readTextEdit(policy, 0, 0);  // street
    list << readTextEdit(policy, 1, 0);  // city
    list << readTextEdit(policy, 1, 1);  // state
    list << readTextEdit(policy, 2, 0);  // zip
    list << readTextEdit(policy, 2, 1);  // country

    QString address = list.join("\n");

    SeasidePersonModel *pm = model();
    pm->setAddress(row, SeasideDetail(address, location));
}

void SeasideEditView::textChanged()
{
    MTextEdit *edit = dynamic_cast<MTextEdit *>(sender());
    if (!edit) {
        qWarning() << "[SeasideEditView] textChanged signal from unexpected source";
        return;
    }

    int row;

    SeasidePersonModel *pm = model();
    if (edit == m_firstNameEdit)
        pm->setFirstName(m_firstNameEdit->text());
    else if (edit == m_lastNameEdit)
        pm->setLastName(m_lastNameEdit->text());
    else if (edit == m_companyEdit)
        pm->setCompany(m_companyEdit->text());
    else if ((row = findRowWithItem(m_phonePolicy, edit)) != -1)
        updatePhone(row);
    else if ((row = findRowWithItem(m_emailPolicy, edit)) != -1)
        updateEmail(row);
    else if ((row = findRowWithItem(m_addressPolicy, edit)) != -1)
        updateAddress(row);
}

void SeasideEditView::activated(int index)
{
    Q_UNUSED(index);

    MComboBox *combo = dynamic_cast<MComboBox *>(sender());
    if (!combo) {
        qWarning() << "[SeasideEditView] activated signal from unexpected source";
        return;
    }

    int row = findRowWithItem(m_phonePolicy, combo);
    if ((row = findRowWithItem(m_phonePolicy, combo)) != -1)
        updatePhone(row);  // this will go find the index itself
    if ((row = findRowWithItem(m_addressPolicy, combo)) != -1)
        updateAddress(row);  // this will go find the index itself
}

void SeasideEditView::favoriteClicked(bool init)
{
    SeasidePersonModel *pm = model();

    bool favorite = pm->favorite();
    if (!init) {
        pm->setFavorite(!favorite);
        favorite = !favorite;
    }

    if (favorite)
        m_favButton->setIconID("icon-m-common-favorite-mark");
    else
        m_favButton->setIconID("icon-m-common-favorite-unmark");
}

M_REGISTER_VIEW(SeasideEditView, MWidgetController)
