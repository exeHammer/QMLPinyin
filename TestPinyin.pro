QT += quick sql

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        inputcore.cpp \
        main.cpp \
        pinyin/dictbuilder.cpp \
        pinyin/dictlist.cpp \
        pinyin/dicttrie.cpp \
        pinyin/lpicache.cpp \
        pinyin/matrixsearch.cpp \
        pinyin/mystdlib.cpp \
        pinyin/ngram.cpp \
        pinyin/pinyinime.cpp \
        pinyin/searchutility.cpp \
        pinyin/spellingtable.cpp \
        pinyin/spellingtrie.cpp \
        pinyin/splparser.cpp \
        pinyin/sync.cpp \
        pinyin/userdict.cpp \
        pinyin/utf16char.cpp \
        pinyin/utf16reader.cpp \
        pinyininputmethod.cpp \
        qmlhelper.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    inputcore.h \
    pinyin/atomdictbase.h \
    pinyin/dictbuilder.h \
    pinyin/dictdef.h \
    pinyin/dictlist.h \
    pinyin/dicttrie.h \
    pinyin/lpicache.h \
    pinyin/matrixsearch.h \
    pinyin/mystdlib.h \
    pinyin/ngram.h \
    pinyin/pinyinime.h \
    pinyin/searchutility.h \
    pinyin/spellingtable.h \
    pinyin/spellingtrie.h \
    pinyin/splparser.h \
    pinyin/sync.h \
    pinyin/userdict.h \
    pinyin/utf16char.h \
    pinyin/utf16reader.h \
    pinyininputmethod.h \
    qmlhelper.h
