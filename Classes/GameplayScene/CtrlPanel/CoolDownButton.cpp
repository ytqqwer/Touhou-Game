#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "CoolDownButton.h"

#include <string>

USING_NS_CC;
using namespace std;

bool
CoolDownButton::init()
{
    if (!Button::init())
        return false;

    initUseCountAndCoolDownTime();
    initRemainingUseCountLabel();
    initRemainingTimeLabel();
    initTouchListener();

    return true;
}

void
CoolDownButton::initRemainingUseCountLabel()
{
    // 如果没有使用次数的限制 (_remainingUseCount == -1), 什么都不做

    if (_remainingUseCount == -1)
        return;

    _remainingUseLabel =
        Label::createWithTTF(to_string(_remainingUseCount), "fonts/dengxian.ttf", 10);
    _remainingUseLabel->setColor(Color3B::WHITE);

    this->addChild(_remainingUseLabel);
    Size btnSize = this->getContentSize();
    _remainingUseLabel->setPosition(Vec2(btnSize.width * 0.1, btnSize.height * 0.9));
}

void
CoolDownButton::initRemainingTimeLabel()
{
    _remainingTimeLabel = Label::createWithTTF("0", "fonts/dengxian.ttf", 36);
    _remainingTimeLabel->setVisible(false);
    this->addChild(_remainingTimeLabel);
}

void
CoolDownButton::freeze()
{
    // TODO, 界面显示
    this->setTouchEnabled(false);
}

void
CoolDownButton::unFreeze()
{
    // TODO, 界面显示
    this->setTouchEnabled(true);
}

void
CoolDownButton::coolDown()
{
    _remainingCoolDownTime = _coolDownTime;

    _remainingTimeLabel->setString(to_string(_remainingCoolDownTime));
    this->scheduleOnce(SEL_SCHEDULE(&CoolDownButton::coolDownBySecondSchedule), 1);
}

void
CoolDownButton::coolDownBySecondSchedule(float unused)
{
    // 类似于递归
    if (--_remainingCoolDownTime == 0) {
        unFreeze();
    } else {
        _remainingTimeLabel->setString(to_string(_remainingCoolDownTime));

        // 下面的注释掉的 selector 形式的 schedule 不能使用
        //   : 同一时间 scheduler 中只能有一个 selector 被 schedule,
        //     这次 selector 还没完成的时候, 不能再注册相同的 selector
        //     所以改用了 lambda 表达式
        //     同时，为了确保 tag 唯一，将 this 指针和剩余冷却时间作为了 tag 的一部分

        // scheduleOnce(SEL_SCHEDULE(&CoolDownButton::coolDownBySecondSchedule), 1);
        scheduleOnce([this](float unused) { this->coolDownBySecondSchedule(unused); }, 1,
                     "coolDown_" + to_string((long)this) + "_" + to_string(_remainingCoolDownTime));
    }
}

void
CoolDownButton::useOnce()
{
    --_remainingUseCount;
    _remainingUseLabel->setString(to_string(_remainingUseCount));

    if (_remainingUseCount == 0) {
        freeze();
    } else {
        freeze();
        coolDown();
    }
}
