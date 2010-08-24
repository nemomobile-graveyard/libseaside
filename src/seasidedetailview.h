/*
 * meego-handset-people - Contacts application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SEASIDEDETAILVIEW_H
#define SEASIDEDETAILVIEW_H

#include <MWidgetView>

#include <seasidepersonmodel.h>
#include <seaside.h>

class QGraphicsLinearLayout;
class MLayout;
class MGridLayoutPolicy;
class SeasideLabel;

class SeasideDetailView: public MWidgetView
{
    Q_OBJECT
    M_VIEW(SeasidePersonModel, MWidgetStyle)

public:
    SeasideDetailView(MWidgetController *controller);
    virtual ~SeasideDetailView();

protected:
    virtual void setupModel();
    virtual void addSection(int index, const QString& title);
    virtual void addRecent(const SeasideDetail& detail);
    virtual void addPhone(const SeasideDetail& detail);
    virtual void addEmail(const SeasideDetail& detail);
    virtual void addAddress(const SeasideDetail& detail);

signals:
    void callNumber(const QString& number);
    void composeSMS(const QString& number);
    void composeEmail(const QString& address);
    void viewRequest(qreal ypos, qreal height);

protected slots:
    void toggleSection(SeasideLabel *label);
    void callNumber(SeasideLabel *label);
    void composeSMS(SeasideLabel *label);
    void composeEmail(SeasideLabel *label);

private:
    MWidgetController *m_controller;
    MGridLayoutPolicy *m_mainPolicy;

    bool *m_visible;
    SeasideLabel **m_headers;
    MWidgetController **m_widgets;
    QGraphicsLinearLayout **m_layouts;

    QMap<SeasideLabel *, SeasideDetail> m_labelToDetail;

    void clearArrays();

    Q_DISABLE_COPY(SeasideDetailView)
};

#endif // SEASIDEDETAILVIEW_H
