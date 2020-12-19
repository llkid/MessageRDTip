#include "messagerdtip.h"
#include "ui_messagerdtip.h"

#include <QApplication>
#include <QScreen>
#include <QDebug>

MessageRDTip* MessageRDTip::instance = nullptr;
AnimationMode MessageRDTip::mode = AnimationMode::AllAnimation;

MessageRDTip::MessageRDTip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageRDTip),
    showGroup(new QParallelAnimationGroup(this))
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_QuitOnClose);       // 当是最后一个窗口时，退出主程序

    setSize(310, 210);
    connect(ui->btnClose, &QAbstractButton::clicked, this, &MessageRDTip::hideTip);
    connect(qApp, &QApplication::aboutToQuit, this, &MessageRDTip::close);      // 主程序退出后，关闭该窗口

    initAnimation();    // 动画属性设置
    initTimer();        // 定时器初始化
}

MessageRDTip::~MessageRDTip()
{
    qDebug() << "destructor called...";
    delete ui;
}

MessageRDTip *MessageRDTip::getInstance()
{
    if (!instance) {
        instance = new MessageRDTip;
    }
    return instance;
}

void MessageRDTip::setSize(int width, int height)
{
    // 设置面板大小
    this->setFixedSize(width, height);
    ui->tipArea->setFixedSize(width, height);
}

 int MessageRDTip::getSystemTaskHeigth(QWidget *parent)
{
    return parent->screen()->geometry().height()
            - parent->screen()->availableGeometry().height();
 }

 void MessageRDTip::showAtSpecifiedLocation(const QPoint &position)
 {
     // 设置，面板位置
     this->move(position.x(), position.y());
 }

 void MessageRDTip::showAtSpecifiedLocation(int posx, int posy)
 {
     // 设置，面板位置
     this->move(posx, posy);
 }

void MessageRDTip::showTip(const QStringList &texts, int timeout)
{
    // 仅在 ui 线程
    if (!instance) {
        instance = new MessageRDTip;
    }

    instance->readyTimer(timeout);
    instance->setWindowModality(Qt::WindowModal);
    instance->setContentDetail(texts);
    instance->showAnimation();
}

void MessageRDTip::hideTip()
{
    return instance ? instance->hideAnimation() : void();
}

AnimationMode MessageRDTip::getMode()
{
    return mode;
}

void MessageRDTip::setMode(AnimationMode newMode)
{
    mode = mode != newMode ? newMode : mode;
}

void MessageRDTip::initAnimation()
{
    showOpacity = new QPropertyAnimation(ui->tipArea, "windowOpacity");
    if (static_cast<int>(mode) & static_cast<int>(AnimationMode::OpacityAnimation)) {
        showOpacity->setDuration(1500);
        showOpacity->setStartValue(0);
    } else {
        showOpacity->setDuration(0);
        showOpacity->setStartValue(1);
    }
    showOpacity->setEndValue(1);
    showGroup->addAnimation(showOpacity);

    showPos = new QPropertyAnimation(ui->tipArea, "pos");
    const QRect rect = this->screen()->geometry();
    const int posx = rect.width() - this->width();
    const int posy = rect.height() - this->height() - getSystemTaskHeigth(this);
    showAtSpecifiedLocation(posx, posy);

    const QPoint hideStartPos{ 0, ui->tipArea->height() };
    const QPoint showStartPos{ 0, 0 };
    if (static_cast<int>(mode) & static_cast<int>(AnimationMode::PosAnimation)) {
        showPos->setDuration(1500);
        showPos->setStartValue(hideStartPos);
    } else {
        showPos->setDuration(0);
        showPos->setStartValue(showStartPos);
    }
    showPos->setEndValue(showStartPos);
    showGroup->addAnimation(showPos);

    connect(showGroup, &QParallelAnimationGroup::finished, [this]{
        // back消失动画结束关闭窗口
        if (showGroup->direction() == QAbstractAnimation::Backward) {
            // Qt::WA_DeleteOnClose 后手动设置为 null
            instance = nullptr;
            qApp->disconnect(this);
            // 关闭时设置为非模态，方式主窗口被遮挡，待测试
            this->setWindowModality(Qt::NonModal);
            this->close();
        } else {
            // 配合 keepAnimation
            showAnimationEnd = true;
            // 配合定时关闭
            if (hideCount > 0)
                hideTimer->start();
        }
    });
}

void MessageRDTip::initTimer()
{
    hideTimer = new QTimer(this);
    hideTimer->setInterval(1000); //1s间隔
    connect(hideTimer, &QTimer::timeout, [this]{
        if (hideCount > 1) {
            --hideCount;
            ui->btnClose->setText(QString("%1 S").arg(hideCount));
        } else {
            ui->btnClose->setText(QString("Close"));
            hideTimer->stop();
            hideTip();
        }
    });
}

void MessageRDTip::readyTimer(int timeout)
{
    // 先设置，在显示动画结束再 start 开始计时器
    hideCount = timeout;
    hideTimer->stop();

    // 这部分根据需要设置
    hideCount > 0
            ? ui->btnClose->setText(QString("%1 S").arg(hideCount))
            : ui->btnClose->setText(QString("Close"));
}

void MessageRDTip::setContentDetail(const QStringList &texts)
{
    QString tip_text("<p style='line-height:120%'>");
    for (const QString &text : texts) {
        if (text.isEmpty())
            continue;
        tip_text += text + "<br>";
    }
    tip_text += "</p>";
    ui->contentLabel->setText(tip_text);
}

void MessageRDTip::showAnimation()
{
    showGroup->setDirection(QAbstractAnimation::Forward);
    // 停止正在进行的动画重新
    if (showGroup->state() == QAbstractAnimation::Running) {
        showGroup->stop();
    }
    showGroup->start();
    show();
}

void MessageRDTip::keepAnimation()
{
    // show 没有完成，或者正在动画中才进入
    if (!showAnimationEnd || showGroup->state() != QAbstractAnimation::Stopped) {
        showGroup->setDirection(QAbstractAnimation::Forward);
        showGroup->start();
        show();
    }
}

void MessageRDTip::hideAnimation()
{
    // Backward反向执行动画
    showGroup->setDirection(QAbstractAnimation::Backward);
    showGroup->start();
}
