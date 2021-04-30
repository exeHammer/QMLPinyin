#include "qmlhelper.h"
#include <QGuiApplication>
#include <QEvent>
#include <QWindow>
#include <QKeyEvent>
#include <QDebug>
#include <QQuickItem>

QmlHelper::QmlHelper(QObject *parent) : QObject(parent)
{

}

QString QmlHelper::getClassName(QObject *object) const
{
    qDebug() << object->metaObject()->className();
    return object ? object->metaObject()->className() : "";
}

void QmlHelper::keyClickUpper(int key) const
{
    qApp->postEvent(qApp->focusWindow(),
                    new QKeyEvent(QKeyEvent::KeyPress, key, Qt::ShiftModifier, QChar(key).toUpper()));
}

QPointF QmlHelper::getAbsolutePos(QQuickItem *object) const
{
    if (!object) {
        return QPointF(0, 0);
    }
    QPointF currentPos(object->position());
    return getAbsolutePos(object->parentItem()) + currentPos;
}
