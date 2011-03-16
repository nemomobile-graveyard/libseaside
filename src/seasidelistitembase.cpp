/*
 * Libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QGraphicsSceneMouseEvent>

#include <MImageWidget>
#include <MButton>

#include "seaside.h"
#include "seasidelistitembase.h"
#include "seasidelistitem_p.h"

SeasideListItemBase::SeasideListItemBase(MWidget *parent)
    : MWidgetController(parent)
    , priv(new SeasideListItemPriv)
{

}

SeasideListItemBase::~SeasideListItemBase()
{
    delete priv;
}

//REVISIT
void SeasideListItemBase::setPresence(const int presence){
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

void SeasideListItemBase::setFavorite(const bool& favorite){
    QString str= "";
	if(favorite)
                str ="icon-s-common-favorite-mark";
	else
	  str = "icon-s-common-favorite-unmark";          
         if(priv->favorite)
         priv->favorite->setImage(str);
}

void SeasideListItemBase::setButton(const QString & button){
    QString str= button;
    if (str.isEmpty())
        str = "im-accounts-no-background";
        if(priv->button)
        priv->button->setIconID(str);
}

void SeasideListItemBase::setStatus(const QString& status){
       if(priv->status)
        priv->status->setText(status);
}

void SeasideListItemBase::setCommFlags(const int flags){
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
    
void SeasideListItemBase::setThumbnail(const QString &thumbnail)
{
    QString str = thumbnail;
    if (str.isEmpty())
        str= "icon-m-content-avatar-placeholder";
    if(priv->image)
    priv->image->setPixmap(getAvatar(str));
}


QPixmap  SeasideListItemBase::getAvatar(QString avatar)
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

void SeasideListItemBase::setName(const QString &name)
{
    if(priv->name)
    priv->name->setText(name);
}

 void SeasideListItemBase::setDetails(const QStringList &details)
{
     if(!priv->detail)
         return;
    priv->detailList = new QStringList(details);
    if(!details.isEmpty() && (details.count() > 0)){
        priv->detail->setText(details[0]);
    }
}

 void SeasideListItemBase::setUuid(const QString &id)
 {
     if(!id.isNull()){
     priv->uuid = new QUuid(id);
     }
 }

void SeasideListItemBase::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void SeasideListItemBase::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    if (sceneBoundingRect().contains(event->scenePos()))
        emit clicked();
}
