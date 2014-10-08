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

#ifndef  IVW_LINKDIALOG_H
#define  IVW_LINKDIALOG_H

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

static const qreal LINKDIALOG_PROCESSOR_GRAPHICSITEM_DEPTH = 1.0f;
static const qreal LINKDIALOG_PROPERTY_GRAPHICSITEM_DEPTH = 2.0f;
static const qreal LINKDIALOG_CONNECTION_GRAPHICSITEM_DEPTH = 3.0f;

namespace inviwo {

IVW_QTEDITOR_API enum InviwoLinkUserGraphicsItemType {
    LinkDialogProcessorGraphicsItemType = 4,
    LinkDialogPropertyGraphicsItemType = 5,
    LinkDialogCurveGraphicsItemType = 6
};


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


/*---------------------------------------------------------------------------------------*/

template <typename T>
class IVW_QTEDITOR_API GraphicsItemData : public QGraphicsRectItem {
public:
    GraphicsItemData(T* item=0) : QGraphicsRectItem() {item_ = item;}
    T* getGraphicsItemData() {return item_;}
    void setGraphicsItemData(T* item) {item_ = item;}
private:
    T* item_;
};

class LinkDialogPropertyGraphicsItem;

class IVW_QTEDITOR_API LinkDialogProcessorGraphicsItem : public GraphicsItemData<Processor> {

public:
    LinkDialogProcessorGraphicsItem();
    ~LinkDialogProcessorGraphicsItem();

    void setProcessor(Processor* processor, bool expandProperties=false);
    Processor* getProcessor() {return getGraphicsItemData();}

    std::vector<LinkDialogPropertyGraphicsItem*> getPropertyItemList() {return propertyGraphicsItems_;}

    QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;

    //override for qgraphicsitem_cast (refer qt documentation)
    enum { Type = UserType + LinkDialogProcessorGraphicsItemType };
    int type() const  {return Type; }

protected:
    virtual void paint(QPainter* p, const QStyleOptionGraphicsItem* options, QWidget* widget);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private:
    LabelGraphicsItem* nameLabel_;
    LabelGraphicsItem* classLabel_;
    std::vector<LinkDialogPropertyGraphicsItem*> propertyGraphicsItems_;
};

/*---------------------------------------------------------------------------------------*/

class IVW_QTEDITOR_API LinkDialogPropertyGraphicsItem : public GraphicsItemData<Property> {

public:
    LinkDialogPropertyGraphicsItem(LinkDialogProcessorGraphicsItem*, Property*);
    ~LinkDialogPropertyGraphicsItem();

    void setProperty(Property* property);

    QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;

    //override for qgraphicsitem_cast (refer qt documentation)
    enum { Type = UserType + LinkDialogPropertyGraphicsItemType };
    int type() const  {return Type; }

    QPointF getShortestBoundaryPointTo(LinkDialogPropertyGraphicsItem*);
    QPointF getShortestBoundaryPointTo(QPointF);

    void updatePositionBasedOnProcessor(bool isComposite=false);

    QPointF calculateArrowCenter(size_t curPort, bool computeRight) const;
    QRectF calculateArrowRect(size_t curPort, bool computeRight) const;
    QRectF calculateArrowRect(DialogConnectionGraphicsItem* cItem, bool computeRight=true) const;
    DialogConnectionGraphicsItem* getArrowConnectionAt(const QPointF pos) const;
    bool isArrowPointedRight(DialogConnectionGraphicsItem* cItem);

    //void addArrow() {arrowCount_++;}
    //int getArrowCount() {return arrowCount_;}
    //void removeArrow() {arrowCount_--;}

    void prepareGeometryChange() {QGraphicsItem::prepareGeometryChange();}

    void addConnectionGraphicsItem(DialogConnectionGraphicsItem*);
    size_t getConnectionGraphicsItemCount() const;
    void removeConnectionGraphicsItem(DialogConnectionGraphicsItem*);

protected:
    virtual void paint(QPainter* p, const QStyleOptionGraphicsItem* options, QWidget* widget);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private:

    LabelGraphicsItem* classLabel_;
    LabelGraphicsItem* typeLabel_;
    LinkDialogProcessorGraphicsItem* processorGraphicsItem_;
    //int arrowCount_;
    std::vector<DialogConnectionGraphicsItem*> connectionItems_;
    bool isCompositeSubProperty_;
    bool isTopItem_;
    bool isBottomItem_;
};

/*---------------------------------------------------------------------------------------*/


class IVW_QTEDITOR_API LinkDialogGraphicsScene : public QGraphicsScene {
public:
    LinkDialogGraphicsScene(QWidget* parent);
    ~LinkDialogGraphicsScene() {}

    template <typename T>
    T* getSceneGraphicsItemAt(const QPointF pos, const Qt::ItemSelectionMode mode= Qt::IntersectsItemShape,
                              Qt::SortOrder order=Qt::DescendingOrder) const {
        QList<QGraphicsItem*> graphicsItems =items(pos, mode, order);

        if (graphicsItems.size() > 0) {
            for (int i=0; i<graphicsItems.size(); i++) {
                T* graphicsItem = qgraphicsitem_cast<T*>(graphicsItems[i]);

                if (graphicsItem)
                    return graphicsItem;
            }
        }

        return 0;
    }

    QGraphicsItem* getPropertyGraphicsItemAt(Property* property);
    void setNetwork(ProcessorNetwork* network) {processorNetwork_ = network;}
    ProcessorNetwork* getNetwork() {return processorNetwork_;}

    void initScene(Processor* srcProcessor, Processor* dstProcessor);
    void clearSceneRepresentations();
    void removeCurrentPropertyLinks();
    void removeAllPropertyLinks();
    void addPropertyLink(Property* srcProperty, Property* dstProperty, bool bidirectional);
    int currentLinkItemsCount();
    void setExpandProperties(bool expand);
    void updatePropertyItemsOfAllProcessors();

    PropertyOwner* src_;
    PropertyOwner* dest_;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* e);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e);
    void keyPressEvent(QKeyEvent* keyEvent);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* e);
    void addPropertyLink(LinkDialogPropertyGraphicsItem* outProperty, LinkDialogPropertyGraphicsItem* inProperty, bool isBidirectional);
    void removePropertyLink(DialogConnectionGraphicsItem* propertyLink);
    void cleanupAfterRemoveLink(DialogConnectionGraphicsItem* propertyLink);
    void addPropertyLink(PropertyLink* propertyLink);
    bool isPropertyLinkBidirectional(DialogConnectionGraphicsItem* propertyLink);
    void makePropertyLinkBidirectional(DialogConnectionGraphicsItem* propertyLink, bool isBidirectional);
    void switchPropertyLinkDirection(DialogConnectionGraphicsItem* propertyLink);
    void initializePorpertyLinkRepresentation(LinkDialogPropertyGraphicsItem* outProperty, LinkDialogPropertyGraphicsItem* inProperty,
            PropertyLink* propLink);
    void addProcessorsItemsToScene(Processor* prcoessor, int xPosition, int yPosition);
    DialogConnectionGraphicsItem* getConnectionGraphicsItem(LinkDialogPropertyGraphicsItem*, LinkDialogPropertyGraphicsItem*);

private:
    DialogCurveGraphicsItem* linkCurve_;
    LinkDialogPropertyGraphicsItem* startProperty_;
    LinkDialogPropertyGraphicsItem* endProperty_;

    std::vector<LinkDialogProcessorGraphicsItem*> processorGraphicsItems_;
    std::vector<DialogConnectionGraphicsItem*> connectionGraphicsItems_;
    std::vector<DialogConnectionGraphicsItem*> currentConnectionGraphicsItems_;

    ProcessorNetwork* processorNetwork_;

    void addConnectionToCurrentList(DialogConnectionGraphicsItem*);
    void removeConnectionFromCurrentList(DialogConnectionGraphicsItem*);
    Property* getParentCompositeProperty(Property* property, Processor* processor);
    bool expandProperties_;
};

/*---------------------------------------------------------------------------------------*/

class IVW_QTEDITOR_API LinkDialogGraphicsView : public QGraphicsView {
public:
    LinkDialogGraphicsView(QWidget* parent);
    ~LinkDialogGraphicsView();
    void setDialogScene(LinkDialogGraphicsScene* scene);
protected:
    void mouseDoubleClickEvent(QMouseEvent* e);
private:
    LinkDialogGraphicsScene* scene_;
};

/*---------------------------------------------------------------------------------------*/

class IVW_QTEDITOR_API CheckableQComboBox : public QComboBox {
    Q_OBJECT
public:
    CheckableQComboBox(QWidget *parent , std::string widgetName, std::vector<std::string> options);
    virtual ~CheckableQComboBox();
    bool isItemChecked(int i);
    std::vector<std::string> getCheckedItems();
public slots:
    void onAutoLinkOptionChecked(const QModelIndex&, const QModelIndex&);
private:
    QStandardItemModel* stdandardModel_;
    std::vector<QStandardItem*> standardItems_;
    std::string widgetName_;
};


class IVW_QTEDITOR_API LinkDialog : public InviwoDockWidget {
    Q_OBJECT
public:
    LinkDialog(QWidget* parent);
    LinkDialog(Processor* src, Processor* dest, QWidget* parent);
    void initDialog(Processor* src, Processor* dest);
    virtual void closeEvent ( QCloseEvent * event );
    int exec();
    ~LinkDialog();

private slots:
    void clickedOkayButton();
    void clickedCancelButton();
    void clickedAutoLinkPushButton();
    void clickedDeleteAllLinksPushButton();
    void expandCompositeProperties(bool);
    void setValidationLevelOfSubPropertiesOfCompositeProperties(Property* compositeProperty, PropertyOwner::InvalidationLevel invalidationLevel);

private:
    void initDialogLayout();
    void updateProcessorLinks();

    LinkDialogGraphicsView* linkDialogView_;
    LinkDialogGraphicsScene* linkDialogScene_;
    QDialogButtonBox* okayCancelbuttonBox_;
    QPushButton* autoLinkPushButton_;
    CheckableQComboBox* autoLinkOptions_;
    QCheckBox* expandCompositeOption_;
    QPushButton* deleteAllLinkPushButton_;
    Processor* src_;
    Processor* dest_;
    QEventLoop eventLoop_;
};

/*---------------------------------------------------------------------------------------*/

} //namespace

#endif //IVW_LINKDIALOG_H