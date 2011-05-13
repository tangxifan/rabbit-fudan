#ifndef ELECTRONICPART_H
#define ELECTRONICPART_H

class QGraphicsLineItem;
class QGraphicsSvgItem;
class QGraphicsRectItem;

class ElectronicPart
{
public:
    ElectronicPart();

    QGraphicsSvgItem *svgPart;
    QGraphicsLineItem *wirePart;

    double angle;
};

#endif // ELECTRONICPART_H
