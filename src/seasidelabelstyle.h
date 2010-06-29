/*
 * libseaside - Library that provides an interface to the Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDELABELSTYLE_H
#define SEASIDELABELSTYLE_H

#include <MWidgetStyle>
#include <QFont>

/*
 * SeasideLabelStyle
 *
 * color            the color of the text
 * pressed-color    the color of text while the mouse is down, if mouse is
 *                    enabled on this label
 * font             the font for the text
 * font-weight      like CSS, can be set to bold, bolder, lighter
 * text-align       like CSS, can be set to left, right, center, justify
 * vertical-align   like CSS, can be set to top, middle (or center), bottom
 *
 */

class SeasideLabelStyle: public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(SeasideLabelStyle)

    M_STYLE_ATTRIBUTE(QColor, color, Color);
    M_STYLE_ATTRIBUTE(QColor, pressedColor, PressedColor);
    M_STYLE_ATTRIBUTE(QFont, font, Font);
    M_STYLE_ATTRIBUTE(QString, fontWeight, FontWeight);
    M_STYLE_ATTRIBUTE(QString, textAlign, TextAlign);
    M_STYLE_ATTRIBUTE(QString, verticalAlign, VerticalAlign);
};

class SeasideLabelStyleContainer: public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(SeasideLabelStyle)
};

#endif // SEASIDELABELSTYLE_H
