#ifndef UNIT_H
#define UNIT_H

#include <QString>
#include <QVector>
#include <QPointF>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsItemGroup>

#include "electronicpart.h"

struct Show {
    bool m_showResistor;
    bool m_showCapacitor;
    bool m_showTransistor;
    bool m_showIC;
    bool m_hideSelected;
};

class Unit : public QGraphicsItemGroup
{
public:
    Unit();

    ElectronicPart *unitAdd();
    void setUnitType(const QString &type);
    void setUnitName(const QString &name);
    void setUnitValue(const QString &value);
    void appendUnitPin(QString pin);
    void drawPins();
    void setTypeVisible(struct Show *show);
    void setSelectVisible(struct Show *show);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPointF bbCoordConvert(QString &bbCoord);
    void addResistor();
    void addCapacitor();
    void addTransistor();
    void addIC();
    void addWire();

    ElectronicPart *m_unit;
    QString m_unitType;
    QString m_unitName;
    QString m_unitValue;
    QVector<QPointF> m_unitPin;
};

#endif // UNIT_H
