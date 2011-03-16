/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SeasideListItemLarge_H
#define SeasideListItemLarge_H

#include <MWidgetController>
#include "seaside.h"
struct SeasideListItemPriv;

/*
 * SeasideListItemLarge: Styling Notes
 *
 * The object name is set to SeasideListItemLarge by default; it contains a
 * MImageWidget thumbnail named SeasideListItemLargeThumbnail, and a SeasideLabel
 * named SeasideListItemLargeName and a SeasideLabel named SeasideListItemLargeData
 *
 * See seasidelabelstyle.h for the special styles that can be set on that
 *
 */

class SeasideListItemLarge: public MWidgetController
{
    Q_OBJECT

public:
    SeasideListItemLarge(MWidget *parent = NULL);
    virtual ~SeasideListItemLarge();

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
    Q_DISABLE_COPY(SeasideListItemLarge);
};

#endif // SeasideListItemLarge_H
