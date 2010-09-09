/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SeasideListItemSmall_H
#define SeasideListItemSmall_H

#include <MWidgetController>
#include "seaside.h"
class SeasideListItemSmallPriv;

/*
 * SeasideListItemSmall: Styling Notes
 *
 * The object name is set to SeasideListItemSmall by default; it contains a
 * MImageWidget thumbnail named SeasideListItemSmallThumbnail, and a SeasideLabel
 * named SeasideListItemSmallName and a SeasideLabel named SeasideListItemSmallData
 *
 * See seasidelabelstyle.h for the special styles that can be set on that
 *
 */

class SeasideListItemSmall: public MWidgetController
{
    Q_OBJECT

public:
    SeasideListItemSmall(MWidget *parent = NULL);
    virtual ~SeasideListItemSmall();

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
    SeasideListItemSmallPriv *priv;
    QPixmap  getAvatar(QString avatar);
    Q_DISABLE_COPY(SeasideListItemSmall);
};

#endif // SeasideListItemSmall_H
