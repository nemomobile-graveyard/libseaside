/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QGraphicsGridLayout>
#include <QGraphicsSceneMouseEvent>
#include <QUuid>

#include <MImageWidget>
#include <MLabel>
#include <MButton>

#include "seasidelistitemlarge.h"
#include "seasidelistitem_p.h"
#include "seasidelabel.h"

#include <MWidgetCreator>
#include <QDebug>

M_REGISTER_WIDGET(SeasideListItemLarge);

#define SCREEN_WIDTH 400

SeasideListItemLarge::SeasideListItemLarge(MWidget *parent)
    : MWidgetController(parent)
    , priv(new SeasideListItemPriv)
{
    setObjectName("SeasideListItemLarge");

    QGraphicsGridLayout *grid = new QGraphicsGridLayout;
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(0);
    setLayout(grid);

    priv->image = new MImageWidget();
    priv->image->setObjectName("SeasideListItemLargeThumbnail");
    priv->image->setPreferredSize(64, 64);
    priv->image->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->image, 0, 0, 2, 2);

    priv->name = new SeasideLabel;
    priv->name->setObjectName("SeasideListItemLargeName");
    priv->name->setPreferredWidth(SCREEN_WIDTH);
    priv->name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    grid->addItem(priv->name, 0, 2, 1, 1);

    priv->detail = new SeasideLabel;
    priv->detail->setObjectName("SeasideListItemLargeDetail");
    priv->name->setPreferredWidth(SCREEN_WIDTH);
    priv->name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    grid->addItem(priv->detail, 1, 2, 1, 1);

    priv->status = new SeasideLabel;
    priv->status->setObjectName("SeasideListItemLargeStatus");
    priv->name->setPreferredWidth(SCREEN_WIDTH);
    priv->name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    grid->addItem(priv->status, 2, 2, 1, 1);

    priv->favorite = new MImageWidget();
    priv->favorite->setObjectName("SeasideListItemLargeFavorite");
    priv->favorite->setPreferredSize(25, 25);
    priv->favorite->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->favorite, 2, 1, 1, 1);

    priv->presence = new MImageWidget();
    priv->presence->setObjectName("SeasideListItemLargePresence");
    priv->presence->setPreferredSize(25, 25);
    priv->presence->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->presence, 2, 0, 1, 1);

    priv->button = new MButton();
    priv->button->setObjectName("SeasideListItemLargeButton");
    priv->button->setPreferredSize(70, 70);
    priv->button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->button, 0, 4, 4, 4);

    QObject::connect(priv->button, SIGNAL(clicked()), this, SIGNAL(buttonClicked()));

    priv->email = new MImageWidget();
    priv->email->setObjectName("SeasideListItemLargeEmail");
    priv->email->setPreferredSize(25, 25);
    priv->email->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->email, 2, 5, 1, 1);

    priv->sms = new MImageWidget();
    priv->sms->setObjectName("SeasideListItemLargeSMS");
    priv->sms->setPreferredSize(25, 25);
    priv->sms->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->sms, 2,4, 1, 1);

    priv->im = new MImageWidget();
    priv->im->setObjectName("SeasideListItemLargeIM");
    priv->im->setPreferredSize(25, 25);
    priv->im->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->im, 2, 3, 1, 1);

    priv->call = new MImageWidget();
    priv->call->setObjectName("SeasideListItemLargeCall");
    priv->call->setPreferredSize(25, 25);
    priv->call->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(priv->call, 2, 6, 1, 1);

}

SeasideListItemLarge::~SeasideListItemLarge()
{
    delete priv;
}

//REVISIT
void SeasideListItemLarge::setPresence(const int presence){
        QString str= "";
	if(presence == Seaside::PresenceAvailable)
		str = "icon-m-common-presence-online";
        else if(presence == Seaside::PresenceAway)
		str = "icon-m-common-presence-away";
	else if(presence == Seaside::PresenceOffline)
		str = "icon-m-common-presence-offline";
	else if(presence == Seaside::PresenceBusy)
		str = "icon-m-common-presence-busy";
	else 
		str = "icon-m-common-presence-unknown";
        if(priv->presence)
    	priv->presence->setImage(str);
}

void SeasideListItemLarge::setFavorite(const bool& favorite){
    QString str= "";
	if(favorite)
                str ="icon-m-common-favorite-mark";
	else
            str = "";
         if(priv->favorite)
         priv->favorite->setImage(str);
}

void SeasideListItemLarge::setButton(const QString & button){
    QString str= button;
    if (str.isEmpty())
        str = "im-accounts-no-background";
        if(priv->button)
        priv->button->setIconID(str);
}

void SeasideListItemLarge::setStatus(const QString& status){
       if(priv->status)
        priv->status->setText(status);
}

void SeasideListItemLarge::setCommFlags(const int flags){
    if(!priv->call || !priv->email || !priv->im || !priv->sms)
        return;

        if((flags & Seaside::CommCallDialed) ==  Seaside::CommCallDialed)
                   priv->call->setImage("icon-m-telephony-call-initiated");
          else if((flags & Seaside::CommCallReceived) == Seaside::CommCallReceived)
                    priv->call->setImage("icon-m-telephony-call-received");
          else if((flags & Seaside::CommCallMissed) == Seaside::CommCallMissed)
                    priv->call->setImage("icon-m-telephony-call-missed");
        if(((flags & Seaside::CommSmsSent) ==  Seaside::CommSmsSent)
            || ((flags & Seaside::CommSmsReceived) == Seaside::CommSmsReceived))
                priv->sms->setImage("Icon-new-SMS");
        if(((flags & Seaside::CommEmailSent ) == Seaside::CommEmailSent)
            || ((flags & Seaside::CommEmailReceived) == Seaside::CommEmailReceived))
                priv->email->setImage("Icon-email");
        if(((flags & Seaside::CommIMSent) ==  Seaside::CommIMSent)
            || ((flags & Seaside::CommIMReceived) == Seaside::CommIMReceived))
                priv->im->setImage("Icon-messages");
}
    
void SeasideListItemLarge::setThumbnail(const QString &thumbnail)
{
    QString str = thumbnail;
    if (str.isEmpty())
        str= "icon-m-content-avatar-placeholder";
    if(priv->image)
    priv->image->setPixmap(getAvatar(str));
}


QPixmap  SeasideListItemLarge::getAvatar(QString avatar)
{
    const QPixmap *photo;
    photo = MTheme::pixmap(avatar, QSize(64, 64));


    // create alpha mask
    QImage mask(64, 64, QImage::Format_ARGB32_Premultiplied);
    QPainter maskpainter(&mask);
    //maskpainter.setRenderHint(QPainter::Antialiasing, true);
    maskpainter.setCompositionMode(QPainter::CompositionMode_Source);
    maskpainter.fillRect(mask.rect(), Qt::transparent);
    maskpainter.setPen(QPen(Qt::transparent));
    maskpainter.setBrush(QBrush(Qt::white));
    maskpainter.drawRoundedRect(1.0, 1.0, 61.0, 61.0, 5.0, 5.0);

    // scale/crop photo
    qreal x, y, w, h;
    if (photo->width() > photo->height()) {
        x = (photo->width() - photo->height()) / 2.0;
        w = photo->height();
        y = 0.0;
        h = photo->height();
    } else {
        y = (photo->height() - photo->width()) / 2.0;
        h = photo->width();
        x = 0.0;
        w = photo->width();
    }
    QImage tmp(64, 64, QImage::Format_ARGB32_Premultiplied);
    QPainter tmppainter(&tmp);
    //tmppainter.setRenderHint(QPainter::Antialiasing, true);
    tmppainter.setCompositionMode(QPainter::CompositionMode_Source);
    tmppainter.drawPixmap(tmp.rect(), *photo, QRectF(x, y, w, h));

    // mask photo
    tmppainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    tmppainter.drawImage(tmp.rect(), mask, mask.rect());

    // create final image
    QImage dst(64, 64, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&dst);
    //painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(dst.rect(), Qt::transparent);

    // draw rounded rect background
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    QPen pen(QColor(0xB0, 0xB0, 0xB0));
    QBrush brush(Qt::white);
    pen.setWidthF(1.0);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawRoundedRect(0.0, 0.0, 63.0, 63.0, 5.0, 5.0);

    // draw masked photo
    painter.drawImage(dst.rect(), tmp, tmp.rect());


    return QPixmap::fromImage(dst);
}

void SeasideListItemLarge::setName(const QString &name)
{
    if(priv->name)
    priv->name->setText(name);
}

 void SeasideListItemLarge::setDetails(const QStringList &details)
{
     if(!priv->detail)
         return;
    priv->detailList = new QStringList(details);
    if(!details.isEmpty() && (details.count() > 0)){
        priv->detail->setText(details[0]);
    }
}

 void SeasideListItemLarge::setUuid(const QString &id)
 {
     if(!id.isNull()){
     priv->uuid = new QUuid(id);
     }
 }

void SeasideListItemLarge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void SeasideListItemLarge::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    if (sceneBoundingRect().contains(event->scenePos()))
        emit clicked();
}
