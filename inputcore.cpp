#include "inputcore.h"
#include <iostream>
#include <QGuiApplication>
#include <QWindow>
#include <QKeyEvent>
#include <QMetaObject>
#include <QPair>
#include <QDebug>
#include "qmlhelper.h"

InputCore::InputCore(QObject *parent) : QObject(parent)
{
    connect(&m_pinyinInput, SIGNAL(pinyinTextChanged(QString)), SLOT(setText(QString)));
    m_inputMode = ImCn;
}

void InputCore::setlistenRoot(QObject *object)
{
    if (!object) {
        qDebug() << "The object in `InputCore::listenTo` is nullptr";
        return;
    }
    object->installEventFilter(this);
}

bool InputCore::classIsListen(const QString & className, bool regular)
{
    if (className.isEmpty()) {
        return false;
    }

    foreach (const QString & str, listenClassNameList) {
        if (className.contains(str, Qt::CaseInsensitive))
            return true;
    }
    return false;
    //正则匹配
//    if (regular) {
//        foreach (const QString & str, listenClassNameList) {
////            qDebug() << className << QRegExp(str) << className.indexOf(QRegExp(str));
//            if (className.indexOf(QRegExp(str)) != -1)
//                return true;
//        }
//        return false;
//    }
//    else {
//        return listenClassNameList.contains(className);
//    }
}
void InputCore::resetDatabase()
{
}

void InputCore::setInputMethodViewer(QQuickItem *object)
{
    inputmethod_viewer = object;
}

void InputCore::setInputField(QQuickItem *object)
{
    input_field = object;
}

void InputCore::setCandidateBar(QQuickItem *object)
{
    candidate_bar = object;
    connect(candidate_bar, SIGNAL(wordClicked(int)),
            this, SLOT(completeChoice(int)));
}

void InputCore::show(QQuickItem * object)
{
    if (object) {
        QMetaObject::invokeMethod(
                    inputmethod_viewer, "setCoordinate",
                    Q_ARG(QVariant, QVariant(QmlHelper().getAbsolutePos(object))),
                    Q_ARG(QVariant, QVariant(object->property("height"))));
    }
    QMetaObject::invokeMethod(inputmethod_viewer, "show");
}

void InputCore::close()
{
    QMetaObject::invokeMethod(input_field, "clear");
    QMetaObject::invokeMethod(candidate_bar, "clear");
    QMetaObject::invokeMethod(inputmethod_viewer, "close");
}

bool InputCore::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        QQuickItem * activeItem = watched->property("activeFocusItem").value<QQuickItem*>();
        if (activeItem->property("readOnly").toBool()
                || activeItem->property("inputMethodHints").toInt() & 66536) { // Qt.ImhDigitsOnly == 66536
            close();
            return false;
        }
        if (classIsListen(activeItem->metaObject()->className(), true)) {
            //非英文、不可见、且按字母键时，激活输入法
            if (inputmethod_viewer->property("chinese").toBool() && !inputmethod_viewer->property("visible").toBool()
                    && keyEvent->key() >= Qt::Key_A && keyEvent->key() <= Qt::Key_Z) {
                show(activeItem);
            }
            if (inputmethod_viewer->property("chinese").toBool() && inputmethod_viewer->property("visible").toBool()) {
                switch (keyEvent->key()) {
                case Qt::Key_Escape:
                    close();
                    return true;
                case Qt::Key_Return:
                case Qt::Key_Enter:
                    completeInput(input_field->property("text").toString().replace("'", ""), activeItem);
                    return true;
                case Qt::Key_Equal:
                    QMetaObject::invokeMethod(candidate_bar, "nextPage");
                    return true;
                case Qt::Key_Minus:
                    QMetaObject::invokeMethod(candidate_bar, "prePage");
                    return true;
                case Qt::Key_Backspace:
                    QMetaObject::invokeMethod(input_field, "backspace");
                    findTranslate(input_field->property("text").toString());
                    return true;
                default:
                    if ((keyEvent->key() >= Qt::Key_A && keyEvent->key() <= Qt::Key_Z)
                            || keyEvent->key() == Qt::Key_Apostrophe) {
                        //汉字数量上限，多了会卡
                        if (input_field->property("text").toString().split("'").size() > maxWordsNumber) {
                            event->accept();
                            return true;
                        }
                        QString keyword = input_field->property("text").toString()
                                + QChar(keyEvent->key()).toLower();
                        findTranslate(keyword);
                    }
                    else if (keyEvent->key() == Qt::Key_Space
                             || (keyEvent->key() >= Qt::Key_1 && keyEvent->key() <= Qt::Key_9)) {
                        int index = (keyEvent->key() == Qt::Key_Space)
                                ? 1 : keyEvent->text().toInt();
                        completeChoice(index, activeItem);
                    }
                    return true;
                }
            }
        }
        if ((keyEvent->modifiers() == Qt::ControlModifier) && keyEvent->key() == Qt::Key_Space) {
            inputmethod_viewer->setProperty("chinese", !inputmethod_viewer->property("chinese").toBool());
            return true;
        }
    }
    return false;
}

void InputCore::findTranslate(const QString &keyword)
{
    if (keyword.trimmed().isEmpty()) {
        close();
        return;
    }
    candidate_bar->setProperty("page", 0);

    int num = 0;
    QString splitePy = splitePinyin(keyword, num);
    input_field->setProperty("text", splitePy.replace("%\'", "\'"));
    splitePy.replace("\'", "%\'");

    m_pinyinInput.Matching(splitePy, m_inputMode == ImEn);
    candidate_bar->setProperty("wordslist", m_pinyinInput.HanziModel);
}

void InputCore::setText(QString str)
{
    //m_labPyText->setVisible(str.size());
    //m_labPyText->setText(str);
}

QString InputCore::autoCreateWords(const QString &splitePy)
{
    QString remainWords = splitePy;
    QString result;

    while (!remainWords.isEmpty()) {
        m_pinyinInput.Matching(remainWords);
        if (m_pinyinInput.HanziModel.isEmpty()) {
            break;
        }
        int index = 0;
        for (int i = 0; i != m_pinyinInput.HanziModel[0].size(); ++i) {
            index = remainWords.indexOf("'", index+1);
        }
        if (index == -1) {
            remainWords.clear();
        }
        else {
            remainWords.remove(0, index+1);
        }
        result += m_pinyinInput.HanziModel[0];
        qApp->processEvents();
    }

    return result;
}

QPair<QString, QString> InputCore::wordChoice(int index)
{
    QString inputText = input_field->property("text").toString();
    QVariant choiceVar;
    QMetaObject::invokeMethod(
                candidate_bar, "getText",
                Q_RETURN_ARG(QVariant, choiceVar),
                Q_ARG(QVariant, QVariant(index)));
    QString choiceText = choiceVar.toString();
    int idx = 0;
    for (int i = 0; i != choiceText.size(); ++i) {
        idx = inputText.indexOf("'", idx+1);
    }

    //idx == -1意味着选词前即已没有分号，选词完成后则本次输入完成
    QString selectSource = ((idx == -1) ? inputText : inputText.mid(0, idx+1));
//    auto toDbPinyin = [](const QString & pinyin) -> QString{
//        QString dbpinyin = pinyin;

//        if (pinyin.endsWith("'")) {
//            dbpinyin.remove(dbpinyin.size()-1, 1);
//        }
//        dbpinyin.replace("'", "%'");
//        dbpinyin += "%";
//        return dbpinyin;
//    };
    //IMDB->updateData(toDbPinyin(selectSource), choiceText);

    if (idx == -1) {
        inputText.clear();
    }
    else {
        inputText.remove(0, idx+1);
    }
    input_field->setProperty("text", inputText);
    QMetaObject::invokeMethod(
                input_field, "appendSelected",
                Q_ARG(QVariant, QVariant(choiceText)),
                Q_ARG(QVariant, QVariant(selectSource)));

    QString allSelectedWords = input_field->property("selectedWords").toString();
    QString allSelectedSource = input_field->property("selectedSource").toString();
    if (idx == -1 && allSelectedWords != choiceText) {
        //IMDB->updateData(toDbPinyin(allSelectedSource), allSelectedWords);
    }
    candidate_bar->setProperty("page", 0);

    return QPair<QString, QString>(allSelectedWords, inputText);
}

void InputCore::completeChoice(int index, QObject *object)
{
    if (index > candidate_bar->property("wordsPerPage").toInt()) {
        return;
    }
    auto choiceResult = wordChoice(index);
    //onclicked
    if (!object) {
        object = qApp->focusObject();
    }
    //选词完毕
    if (input_field->property("text").toString().isEmpty()) {
        completeInput(choiceResult.first, object);
    }
    else {
        findTranslate(choiceResult.second);
    }
}

void InputCore::completeInput(const QString & text, QObject *object)
{
    QString newText = object->property("text").toString();
    int curPos = object->property("cursorPosition").toInt();
    newText.insert(curPos, text);
    curPos += text.size();

    object->setProperty("text", newText);
    object->setProperty("cursorPosition", curPos);

    close();
}

QString InputCore::splitePinyin(const QString &pinyin, int & num)
{
    QString shenmu = "bpmfdtnlgkhjqxzcsywr";
    QStringList zcs = QString("z c s").split(" ");
    QStringList yunmuA = QString("a o e ai ao ou ei er an ang en eng").split(" ");

    QString result;

    if (pinyin.contains("\'"))
    {
        bool endfgf = pinyin.endsWith("\'");
        QStringList children = pinyin.split("\'", QString::SkipEmptyParts);
        for (int i = 0; i < children.size(); ++i)
        {
            int cur_num = 0;
            if (!result.isEmpty() && !result.endsWith("%\'")) // 每次进入一定的新的字的拼音
            {
                result += "%\'";
            }
            result += splitePinyin(children.at(i), cur_num);
            num += cur_num;
        }
        if (endfgf)
        {
            result += "\'";
        }
        return result;
    }
    int cur_index = 0;
    while (cur_index < pinyin.size())
    {
        if (shenmu.contains(pinyin.at(cur_index))) // 是声母
        {
            int ym = 1;
            int h = 0; // zh ch sh标记
            // zh ch sh 多加一位
            if (zcs.contains(pinyin.at(cur_index)) && cur_index + 1 < pinyin.size() && pinyin.at(cur_index + 1) == 'h')
            {
                h = 1;
                ym++;
            }

            QStringList yunmu = getYunMuByShengMu(pinyin.at(cur_index));

            // 贪心查找 （尽可能长的找到满足的）
            while ( (ym + cur_index) < pinyin.size() )
            {
                bool find = false;
                for (int i = 0; i < yunmu.size(); ++i)
                {
                    QString c_py = yunmu.at(i);
                    if (c_py.startsWith(pinyin.mid(cur_index + 1 + h, ym - h)))
                    {
                        find = true;
                    }
                }
                if (!find)
                {
                    if (ym > h)
                    {
                        ym--;
                    }
                    break;
                }
                ym++;
            }

            if (!result.isEmpty() && !result.endsWith("%\'"))
            {
                result += "%\'";
            }
            result += pinyin.mid(cur_index, ym + 1);
            cur_index += ym;
        }
        else
        {
            if (result.endsWith("g") // 如果是特殊的几个韵母结束的，到这里应该截取下来，重新匹配
                    || result.endsWith("n")
                    || result.endsWith("r") )
            {
                int last_index = result.lastIndexOf("%\'");
                QString last;
                if (last_index != -1)
                {
                    last = result.mid(last_index + 2);
                }
                else
                {
                    last = result;
                }
                last.remove(last.size() - 1, 1);
                if (!last.isEmpty() && getYunMuByShengMu(last.at(0)).contains(last.mid(1))) // 判断截取之后是否是可以组合的拼音
                {
                    result.remove(result.size() - 1, 1);
                    cur_index -= 1;
                    continue;
                }
            }

            if (yunmuA.contains(pinyin.at(cur_index)))
            {
                int ym = 0;
                while ((ym + cur_index) < pinyin.size())
                {
                    bool find = false;
                    for (int i = 0; i < yunmuA.size(); ++i)
                    {
                        QString c_py = yunmuA.at(i);
                        if (c_py.startsWith(pinyin.mid(cur_index, ym + 1)))
                        {
                            find = true;
                        }
                    }
                    if (find)
                    {
                        ym++;
                    }
                    else
                    {
                        break;
                    }
                }

                if (!result.isEmpty() && !result.endsWith("%\'"))
                {
                    result += "%\'";
                }
                result += pinyin.mid(cur_index, ym);
                cur_index += ym - 1;
            }
            else
            {
                if (!result.isEmpty() && !result.endsWith("%\'"))
                {
                    result += "%\'";
                }
                result += pinyin.at(cur_index);
            }
        }

        num++;
        cur_index++;
    }
    if (num == 0)
    {
        num++;
    }

    return result;
}

QStringList InputCore::getYunMuByShengMu(const QChar &shenmu)
{
    QStringList yunmu;
    switch (shenmu.toLatin1()) // 单独为每一个声母指定可匹配的韵母
    {
    case 'b':
        yunmu = QString("a o ai ei ao an en ang eng i ie iao ian in ing u").split(" ", QString::SkipEmptyParts);
        break;
    case 'p':
        yunmu = QString("a o ai ei ao ou an en ang eng i ie iao ian in ing u").split(" ", QString::SkipEmptyParts);
        break;
    case 'm':
        yunmu = QString("a e o ai ei ao an en ang eng i ie iao iu ian in ing u").split(" ", QString::SkipEmptyParts);
        break;
    case 'f':
        yunmu = QString("a o ei ou an en ang eng u").split(" ", QString::SkipEmptyParts);
        break;
    case 'd':
        yunmu = QString("a o ai ei ao ou an en ang eng i ia ie iao ian iu ian ing u uo ui uan un ong").split(" ", QString::SkipEmptyParts);
        break;
    case 't':
        yunmu = QString("a e ai ei ao ou an ang eng i ie iao ian ing u uo ui uan un ong").split(" ", QString::SkipEmptyParts);
        break;
    case 'n':
    case 'l':
        yunmu = QString("a e o ai ei ao ou an en ang eng i ia ie iao ian iu ian in iang ing u uo ui uan un ong v ve").split(" ", QString::SkipEmptyParts);
        break;
    case 'g':
    case 'k':
    case 'h':
        yunmu = QString("a e ai ei ao ou an en ang eng u ua uo uai ui uan un uang ong").split(" ", QString::SkipEmptyParts);
        break;
    case 'j':
    case 'q':
    case 'x':
        yunmu = QString("i ia ie iao iu ian in iang ing v u ue ve van uan un vn iong").split(" ", QString::SkipEmptyParts);
        break;
    case 'z':
        yunmu = QString("a e i ai ei ao ou an en ang eng u ua uo uai ui uan un uang ong").split(" ", QString::SkipEmptyParts);
        break;
    case 'c':
        yunmu = QString("a e i ai ao ou an en ang eng u ua uo uai ui uan un uang ong").split(" ", QString::SkipEmptyParts);
        break;
    case 's':
        yunmu = QString("a e i ai ei ao ou an en ang eng u ua uo uai ui uan un uang ong").split(" ", QString::SkipEmptyParts);
        break;
    case 'r':
        yunmu = QString("e i ao ou an en ang eng u ua uo ui uan un uang ong").split(" ", QString::SkipEmptyParts);
        break;
    case 'y':
        yunmu = QString("a e i o ao ou an in ang ing u ue uan un ong").split(" ", QString::SkipEmptyParts);
        break;
    case 'w':
        yunmu = QString("a o ai ei an en ang eng u").split(" ", QString::SkipEmptyParts);
        break;
    default:
        break;
    }

    return yunmu;
}
