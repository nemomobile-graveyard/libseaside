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

class SeasideListItemPriv;

/*
 * SeasideListItem: Styling Notes
 *
 * The object name is set to SeasideListItem by default; it contains a
 * MImageWidget thumbnail named SeasideListItemThumbnail, and a SeasideLabel
 * named SeasideListItemName
 *
 * See seasidelabelstyle.h for the special styles that can be set on that
 *
 */

class SeasideListItem: public MWidgetController
{
    Q_OBJECT

public:
    SeasideListItem(MWidget *parent = NULL);
    virtual ~SeasideListItem();

    void setThumbnail(const QString& thumbnail);
    void setName(const QString& name);

signals:
    void clicked();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    SeasideListItemPriv *priv;
    Q_DISABLE_COPY(SeasideListItem);
};

#endif // SEASIDELISTITEM_H
