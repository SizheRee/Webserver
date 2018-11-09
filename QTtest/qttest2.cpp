
#include "qttest2.h"
#include "ui_widget.h"

#include <QtCore/QDebug>
#include <QtCore/QObject>
 
void MyStr::setValue(QString value)
{
    if(value != m_value)
    {
        m_value = value;
        emit valueChanged(value);
    }
}

int main(int argc, char *argv[])
{
    MyStr a;
    MyStr b;
    QObject::connect(&a,SIGNAL(valueChanged(QString)),&b,SLOT(setValue(QString)));
    a.setValue("this is A");
    return 0;
}

// int main(int argc, char *argv[])
// {
//     QApplication a(argc, argv);
//     QMainWindow *hw = new QMainWindow();    //main GUI window
//     Ui::Form createUi;      //createUi is not a real window
//     createUi.setupUi(hw);   //createUi is a tool for creating the GUI
//     QTextBrowser *textBrowser=createUi.textBrowser;

//     QString strResult;
//     //Dec
//     long numDec = 800;
//     QString strMod = QObject::tr("Normal: %1");
//     strResult = strMod.arg(numDec);  //%1是占位符，第一个arg函数参数变量转后的字符串填充到 %1 位置
    
//     textBrowser->append(strMod);
//     textBrowser->append(strResult);

//     myThread k;
//     myThread b;

//     QObject::connect(&k, SIGNAL(MsgSignal(QString)),
//               &b, SLOT(setValue(QString)));//此处connect的第五个参数默认变成Qt::QueuedConnection

//     hw->show();             //hw is the real window
// 	a.exec();
//     return 0;
// }