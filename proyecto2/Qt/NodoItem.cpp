#include "NodoItem.h"
#include <QGraphicsSceneMouseEvent>

NodoItem::NodoItem(int id, float x, float y, float r, QGraphicsItem *parent)
    : QGraphicsEllipseItem(x - r, y - r, 2*r, 2*r, parent), nodoId(id)
{
    setBrush(Qt::gray);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

int NodoItem::getId() const
{
    return nodoId;
}

void NodoItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit nodoClickeado(nodoId);
    QGraphicsEllipseItem::mousePressEvent(event);
}
