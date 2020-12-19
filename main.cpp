#include <QApplication>
#include "messagerdtip.h"
#include <QWidget>
#include <QPushButton>


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

//    QWidget w;
//    QPushButton pb("button", &w);
//    QObject::connect(&pb, &QPushButton::clicked, [] {
//        MessageRDTip::setMode(MessageRDTip::AllAnimation);
//        MessageRDTip::showTip({
//                                  "这是 1 行信息",
//                                  "这是 2 行信息",
//                                  "这是 3 行信息",
//                                  "这是 4 行信息"
//                              }, 5);
//    });
//    w.resize(200, 100);
//    w.show();
    MessageRDTip::showTip({
                              "这是 1 行信息",
                              "这是 2 行信息",
                              "这是 3 行信息",
                              "这是 4 行信息"
                          }, 5);

    return app.exec();
}
