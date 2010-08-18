/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDELISTITEM_H
#define SEASIDELISTITEM_H

#include <MWidgetController>
#include "seaside.h"
class SeasideListItemPriv;

/*
 * SeasideListItem: Styling Notes
 *
 * The object name is set to SeasideListItem by default; it contains a
 * MImageWidget thumbnail named SeasideListItemThumbnail, and a SeasideLabel
 * named SeasideListItemName and a SeasideLabel named SeasideListItemData
 *
 * See seasidelabelstyle.h for the special styles that can be set on that
 *
 */

class SeasideListItem: public MWidgetController
{
    Q_OBJECT

public:
    SeasideListItem(MWidget *parent = NULL, int mode =0);
    virtual ~SeasideListItem();

    void setThumbnail(const QString& thumbnail);
    void setName(const QString& name);
    void setUuid(const QString& id);
    void setDetails(const QStringList& detail); //REVISIT phone or email or IM
    void setPresence(const int presence);
    void setFavorite(const bool& favorite);
    void setButton(const QString& hasButton);
    void setStatus(const QString& status);
    void setCommFlags(const int flags);
    
signals:
    void clicked();
    void buttonClicked();

protected slots:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    SeasideListItemPriv *priv;
    QPixmap  getAvatar(QString avatar);
    Q_DISABLE_COPY(SeasideListItem);
};

#endif // SEASIDELISTITEM_H
