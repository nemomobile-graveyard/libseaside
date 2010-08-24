/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDEIMAGELABEL_H
#define SEASIDEIMAGELABEL_H

#include <MStylableWidget>
#include <MLinearLayoutPolicy>
#include <MAbstractCellCreator>

#include "seasideimagelabelstyle.h"

class MImageWidget;
class MLabel;

class SeasideImageLabel: public MStylableWidget
{
    Q_OBJECT

public:
    SeasideImageLabel(MWidget *parent = NULL);
    virtual ~SeasideImageLabel();

    void setText(const QString &text);

protected:
    virtual void applyStyle();

private:
    M_STYLABLE_WIDGET(SeasideImageLabelStyle)

    MImageWidget *m_image;
    MLabel *m_label;
    QString m_text;

    MLinearLayoutPolicy *m_policy;
};

#endif // SEASIDEIMAGELABEL_H
