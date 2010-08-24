/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDEIMAGELABELSTYLE_H
#define SEASIDEIMAGELABELSTYLE_H

#include <MWidgetStyle>
#include <QFont>

class SeasideImageLabelStyle: public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(SeasideImageLabelStyle)

    M_STYLE_ATTRIBUTE(QFont, font, Font);
    M_STYLE_ATTRIBUTE(QColor, color, Color);
    M_STYLE_ATTRIBUTE(QString, image, image);
    M_STYLE_ATTRIBUTE(QSize, imageSize, ImageSize);
    M_STYLE_ATTRIBUTE(int, marginInternal, MarginInternal);
};

class SeasideImageLabelStyleContainer: public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(SeasideImageLabelStyle)
};

#endif // SEASIDEIMAGELABELSTYLE_H
