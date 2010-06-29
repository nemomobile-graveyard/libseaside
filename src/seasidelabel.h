/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDELABEL_H
#define SEASIDELABEL_H

#include <MStylableWidget>

#include "seasidelabelstyle.h"

class MLabel;
class SeasideLabelPriv;

/*
 * SeasideLabel: Styling Notes
 *
 * See seasidelabelstyle.h for descriptions of the special styles available
 *
 */

class SeasideLabel: public MStylableWidget
{
    Q_OBJECT

public:
    SeasideLabel(const QString &text = QString(), QGraphicsItem *parent = NULL);
    virtual ~SeasideLabel();

    void setMouseEnabled(bool enable);
    bool isMouseEnabled();

    void setText(const QString& text);

signals:
    void clicked(SeasideLabel *label);

protected slots:
    virtual void applyStyle();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void cancelEvent(MCancelEvent *event);

private:
    SeasideLabelPriv *priv;
    Q_DISABLE_COPY(SeasideLabel)
    M_STYLABLE_WIDGET(SeasideLabelStyle)
};

#endif // SEASIDELABEL_H
