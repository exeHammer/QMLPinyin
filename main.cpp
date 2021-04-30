#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "inputcore.h"
#include "qmlhelper.h"

static QObject *inputCoreInstance(QQmlEngine *qmlengine, QJSEngine * engine)
{
    Q_UNUSED(qmlengine);
    InputCore * ins = new InputCore(engine);
    ins->listenClassNameList << "TextField" << "TextArea" << "TextInput";
    return ins;
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterSingletonType<InputCore>(
                "InputMethod", 1, 0,
                "Input", inputCoreInstance);
    QmlHelper qmlHelper;
    engine.rootContext()->setContextProperty("helper", &qmlHelper);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
