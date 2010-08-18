/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <MStylableWidget>
#include <MLinearLayoutPolicy>
#include <MAbstractCellCreator>

#include "imagelabelstyle.h"

class MImageWidget;
class MLabel;

class ImageLabel: public MStylableWidget
{
    Q_OBJECT

public:
    ImageLabel(MWidget *parent = NULL);
    virtual ~ImageLabel();

    void setText(const QString &text);

protected:
    virtual void applyStyle();

private:
    M_STYLABLE_WIDGET(ImageLabelStyle)

    MImageWidget *m_image;
    MLabel *m_label;
    QString m_text;

    MLinearLayoutPolicy *m_policy;
};

#endif // IMAGELABEL_H
