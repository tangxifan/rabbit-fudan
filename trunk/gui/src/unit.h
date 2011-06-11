#ifndef UNIT_H
#define UNIT_H

#include <QString>
#include <QVector>
#include <QPointF>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsItemGroup>
#include <QGraphicsSvgItem>

#define VOFFSET 10      //
#define HOLEGAP 9       //
#define SLOTGAP 18      //
#define TOLEFTEDGE 23   //
#define POWERTOEDGE 8   //
#define TOTOPEDGE 44    //

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

    void unitAdd();
    void setUnitType(const QString &type);
    void setUnitName(const QString &name);
    void setUnitValue(const QString &value);
    void appendUnitPin(QString pin);
    void drawPins();
    void setUnitWireVisible(bool visible);
    void setTypeVisible(struct Show *show);
    void setSelectVisible(struct Show *show);
    void setWasSelected(bool wasSelected);
    int getZ();
    bool wasSelected();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPointF bbCoordConvert(QString &bbCoord);
    void addResistor();
    void addCapacitor();
    void addTransistor();
    void addIC();
    void addWire();

    QGraphicsSvgItem *m_svg;
    QList<QGraphicsLineItem *> m_wire;
    QString m_unitType;
    QString m_unitName;
    QString m_unitValue;
    QVector<QPointF> m_unitPin;
    qreal m_minPinY;
    double m_angle;
    int m_z;
    bool m_wasSelected;
};

#endif // UNIT_H
