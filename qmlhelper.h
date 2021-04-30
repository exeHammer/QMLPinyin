#ifndef QMLHELPER_H
#define QMLHELPER_H

#include <QObject>
#include <QPointF>

class QQuickItem;
class QmlHelper : public QObject
{
    Q_OBJECT
public:
    explicit QmlHelper(QObject *parent = nullptr);
    // 返回参数object的类名
    Q_INVOKABLE QString getClassName(QObject * object) const;
    // 对focusWindow输入大写的key
    Q_INVOKABLE void keyClickUpper(int key) const;
    // 获取qml组件object在进程窗口中的绝对坐标
    Q_INVOKABLE QPointF getAbsolutePos(QQuickItem * object) const;

signals:

public slots:
};

#endif // QMLHELPER_H
