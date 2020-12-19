#ifndef MESSAGERDTIP_H
#define MESSAGERDTIP_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QTimer>

namespace Ui {
class MessageRDTip;
}

enum class AnimationMode {
    NoAnimation = 0x00,         //< 无动画 /
    OpacityAnimation = 0x01,    //< 透明度渐变效果    动画 /
    PosAnimation = 0x02,        //< 滑动效果        动画 /
    AllAnimation = 0xFF         //< 滑动 + 渐变效果 动画 /
};

/**
    @brief 自定义渐变弹框
    @author Llkid
    @date   2020-12-19
    @details 使用 ui 文件进行布局，以一个固定的 widget 为基础，
    在其内部进行动态展示，截取以主面板跳板，实现弹框渐变与任意位置的设置
*/
class MessageRDTip : public QWidget
{
    Q_OBJECT

private:
    explicit MessageRDTip(QWidget *parent = nullptr);

public:
    ~MessageRDTip();
    static MessageRDTip* getInstance();

    /**
      @brief 设置提示框大小和位置
    */
    void setSize(int width, int height);

    /**
      @brief window平台下获取任务栏高度
    */
    int getSystemTaskHeigth(QWidget* parent);

    /**
        @brief 设置 widget 在指定位置显示
    */
    void showAtSpecifiedLocation(const QPoint& position);

    /**
        @brief 设置 widget 在指定位置显示
        @param posx
        @param poxy
    */
    void showAtSpecifiedLocation(int posx, int posy);

    /**
      @brief    显示动画弹框
      @param    texts 显示内容
      @param    timeout 显示时长
      @details  timeout <= 0, 需要手动关闭
    */
    static void showTip(const QStringList& texts, int timeout = 0);

    /**
      @brief    隐藏动画弹框
      @details  none
    */
    static void hideTip();

    static AnimationMode getMode();
    static void setMode(AnimationMode newMode);

private:
    Ui::MessageRDTip *ui;

    static MessageRDTip* instance;     // 唯一实例
    static AnimationMode mode;          // 动画模式

    QParallelAnimationGroup* showGroup; // 动画组
    QPropertyAnimation* showOpacity;    // 透明动画
    QPropertyAnimation* showPos;        // 位置动画

    bool showAnimationEnd;

    QTimer* hideTimer;
    int hideCount;

private:
    void initAnimation();
    void initTimer();
    void readyTimer(int timeout);
    void setContentDetail(const QStringList& texts);
    void showAnimation();
    void keepAnimation();
    void hideAnimation();
};

#endif // MESSAGERDTIP_H
