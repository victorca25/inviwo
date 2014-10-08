/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2012-2014 Inviwo Foundation
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Main file author: Sathish Kottravel
 *
 *********************************************************************************/

#ifndef  IVW_LINKDIALOG_CURVEGRAPHICSITEMS_H
#define  IVW_LINKDIALOG_CURVEGRAPHICSITEMS_H

#include <inviwo/qt/editor/inviwoqteditordefine.h>
#include <QGraphicsView>
#include <QDialog>
#include <QGraphicsRectItem>
#include <QPushButton>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QPainterPath>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QStandardItemModel>
#include <QEventLoop>
#include <QCheckBox>

#include <inviwo/core/links/processorlink.h>
#include <inviwo/core/links/linkconditions.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/qt/widgets/labelgraphicsitem.h>
#include <inviwo/qt/editor/connectiongraphicsitem.h>
#include <inviwo/qt/widgets/inviwodockwidget.h>

#include <inviwo/qt/editor/linkdialog/linkdialoggraphicsitems.h>

namespace inviwo {

class IVW_QTEDITOR_API DialogCurveGraphicsItem : public CurveGraphicsItem {

public:
    DialogCurveGraphicsItem(QPointF startPoint, QPointF endPoint, uvec3 color=uvec3(38,38,38));
    ~DialogCurveGraphicsItem();

    //override for qgraphicsitem_cast (refer qt documentation)
    enum { Type = UserType + LinkDialogCurveGraphicsItemType };
    int type() const  {return Type; }
protected:
    virtual QPainterPath obtainCurvePath(QPointF startPoint, QPointF endPoint) const;
};

//////////////////////////////////////////////////////////////////////////

class LinkDialogPropertyGraphicsItem;
class LinkDialogProcessorGraphicsItem;

class IVW_QTEDITOR_API DialogConnectionGraphicsItem : public DialogCurveGraphicsItem {
public:
    DialogConnectionGraphicsItem(LinkDialogPropertyGraphicsItem* startProperty, LinkDialogPropertyGraphicsItem* endProperty,
                                 PropertyLink* propertyLink);
    ~DialogConnectionGraphicsItem();

    PropertyLink* getPropertyLink() const { return propertyLink_; }
    LinkDialogPropertyGraphicsItem* getStartProperty() const { return startPropertyGraphicsItem_; }
    LinkDialogPropertyGraphicsItem* getEndProperty() const { return endPropertyGraphicsItem_; }

    void switchDirection();
    bool isBidirectional();
    void initialize();
    void deinitialize();
    void setStartArrowHeadIndex(size_t index) {startArrowHeadIndex_ = index;}
    void setEndArrowHeadIndex(size_t index) {endArrowHeadIndex_ = index;}
    size_t getStartArrowHeadIndex() { return startArrowHeadIndex_;}
    size_t getEndArrowHeadIndex() { return endArrowHeadIndex_;}
    void updateStartEndPoint();
    void updateConnectionDrawing();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e);

private:
    LinkDialogPropertyGraphicsItem* startPropertyGraphicsItem_;
    LinkDialogPropertyGraphicsItem* endPropertyGraphicsItem_;
    size_t startArrowHeadIndex_;
    size_t endArrowHeadIndex_;
    PropertyLink* propertyLink_;
};

} //namespace

#endif //IVW_LINKDIALOG_CURVEGRAPHICSITEMS_H