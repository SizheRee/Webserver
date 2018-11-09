#ifndef QTEST2_H
#define QTEST2_H

#include <QtWidgets/QWidget>

#include <QtCore/QThread>
 
class MyStr :public QObject
{
    Q_OBJECT //必须包含的宏
 
public:
    MyStr (){m_value = "zero";}
 
    QString value() const {return  m_value;}
 
public slots :
    void setValue(QString value );
 
signals: //信号
    void valueChanged(QString newValue); 
private:
    QString m_value;

};

#endif // QTEST2_H