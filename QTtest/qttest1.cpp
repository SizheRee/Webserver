//main.cpp
#include "testui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow *hw = new QMainWindow();    //main GUI window
    Ui::Form createUi;      //createUi is not a real window
    createUi.setupUi(hw);   //createUi is a tool for creating the GUI
    QTextBrowser *textBrowser=createUi.textBrowser;

    QString strResult;
    //Dec
    long numDec = 800;
    QString strMod = QObject::tr("Normal: %1");
    strResult = strMod.arg(numDec);  //%1是占位符，第一个arg函数参数变量转后的字符串填充到 %1 位置
    
    textBrowser->append(strMod);
    textBrowser->append(strResult);

    hw->show();             //hw is the real window

    return a.exec();
}