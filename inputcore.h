#ifndef INPUTCORE_H
#define INPUTCORE_H

#include <QObject>
#include <QQuickItem>
#include "pinyininputmethod.h"

template <class T1, class T2> class QPair;

class InputCore : public QObject
{
    Q_OBJECT
public:
    enum CandidateWords_Style{
        CandidateWords_Horizontal,
        CandidateWords_Vertical
    };
    Q_ENUM(CandidateWords_Style)

    explicit InputCore(QObject *parent = nullptr);

    // className是否监听
    Q_INVOKABLE bool classIsListen(const QString & className, bool regular = true);
    // 重置数据库，包含删除用户词和重置词频
    Q_INVOKABLE void resetDatabase();
    // 绑定根监听
    Q_INVOKABLE void setlistenRoot(QObject *object);

    // 以下函数为结合Qml部分使用，勿手动调用
    // 绑定输入法qml界面
    Q_INVOKABLE void setInputMethodViewer(QQuickItem * object);
    // 绑定输入法输入栏
    Q_INVOKABLE void setInputField(QQuickItem * object);
    // 绑定输入法候选词栏
    Q_INVOKABLE void setCandidateBar(QQuickItem * object);
    // 重载事件过滤器，仅处理键盘事件
    bool eventFilter(QObject *watched, QEvent *event) override;

    QStringList listenClassNameList;

    int getMaxWordsNumber() {
        return maxWordsNumber;
    }
    bool setMaxWordsNumber(int max) {
        if (max < 1 || max >= 64) {
            return false;
        }
        maxWordsNumber = max;
        return true;
    }
signals:

private slots:
    // 完成选词
    void completeChoice(int index, QObject * object = nullptr);
    void setText(QString str);

private:
    // 分割拼音
    QString splitePinyin(const QString &pinyin, int & num);
    // 获得声母（通过韵母）
    QStringList getYunMuByShengMu(const QChar &shenmu);
    // 查找候选词，造句，去重，并设置显示
    void findTranslate(const QString & keyword);
    // 自动造句
    QString autoCreateWords(const QString & splitePy);
    // 根据序号选词，返回的pair::first为已选词，pair::second为剩余的词
    QPair<QString, QString> wordChoice(int index);
    // 完成输入（设置界面显示）
    void completeInput(const QString & text, QObject * object);

    // 激活输入法
    void show(QQuickItem *object = nullptr);
    // 关闭输入法
    void close();

    QQuickItem * inputmethod_viewer = nullptr;
    QQuickItem * input_field = nullptr;
    QQuickItem * candidate_bar = nullptr;

    //一次输入最大字数
    int maxWordsNumber = 10;

    enum InputMode{ImEn, ImCn};
    InputMode m_inputMode;
    CPinyinInputMethod m_pinyinInput;
};

#endif // INPUTCORE_H
