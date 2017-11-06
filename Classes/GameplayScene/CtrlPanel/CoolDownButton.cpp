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

    subClassPreInit();

    initRemainingUseLabel();
    initRemainingCoolDownTimeLabel();
    initTouchListener();

    return true;
}

void
CoolDownButton::initRemainingUseLabel()
{
    // 如果没有使用次数的限制 (_remainingUseCount == -1), 什么都不做

    if (_remainingUseCount == -1)
        return;

    _remainingUseLabel =
        Label::create(to_string(_remainingUseCount), "fonts/NotoSansCJKsc-Black.otf", 10);
    _remainingUseLabel->setColor(Color3B::WHITE);

    this->addChild(_remainingUseLabel);
    Size btnSize = this->getContentSize();
    _remainingUseLabel->setPosition(Vec2(btnSize.width * 0.1, btnSize.height * 0.1));
}

void
CoolDownButton::initRemainingCoolDownTimeLabel()
{
    _remainingCoolDownTimeLabel = Label::create("0", "fonts/NotoSansCJKsc-Black.otf", 28);
    _remainingCoolDownTimeLabel->setVisible(false);

    this->addChild(_remainingCoolDownTimeLabel);
    _remainingCoolDownTimeLabel->setPosition(this->getContentSize() / 2);
}

void
CoolDownButton::freeze()
{
    this->setColor(Color3B(64, 64, 64));
    this->setTouchEnabled(false);
}

void
CoolDownButton::unFreeze()
{
    this->setColor(Color3B(0xff, 0xff, 0xff));
    this->setTouchEnabled(true);
}

void
CoolDownButton::coolDown()
{
    if (_coolDownTime == 0) {
        unFreeze();
        return;
    }
    _remainingCoolDownTime = _coolDownTime;

    _remainingCoolDownTimeLabel->setString(to_string((int)_remainingCoolDownTime));
    _remainingCoolDownTimeLabel->setVisible(true);
    this->scheduleOnce(SEL_SCHEDULE(&CoolDownButton::coolDownBySecondSchedule), 1);
}

void
CoolDownButton::coolDownBySecondSchedule(float unused)
{
    --_remainingCoolDownTime;

    // 类似于递归
    if (_remainingCoolDownTime == 0) {
        unFreeze();
        _remainingCoolDownTimeLabel->setVisible(false);
    } else {
        // 转型为 `int' 以截断小数点以后的部分
        _remainingCoolDownTimeLabel->setString(to_string((int)_remainingCoolDownTime));

        // 下面的注释掉的 selector 形式的 schedule 不能使用
        //   : 同一时间 scheduler 中只能有一个 selector 被 schedule,
        //     这次 selector 还没完成的时候, 不能再注册相同的 selector
        //     所以改用了 lambda 表达式
        //     同时，为了确保 tag 唯一，将 this 指针和剩余冷却时间作为了 tag 的一部分

        // scheduleOnce(SEL_SCHEDULE(&CoolDownButton::coolDownBySecondSchedule), 1);
        scheduleOnce([this](float unused) { this->coolDownBySecondSchedule(unused); }, 1,
                     "coolDown_" + to_string((long)this) + "_" +
                         to_string((int)_remainingCoolDownTime));
    }
}

void
CoolDownButton::useOnce()
{
    // -1 表示没有使用次数限制
    if (_remainingUseCount == -1) {
        freeze();
        coolDown();
    } else {
        --_remainingUseCount;
        _remainingUseLabel->setString(to_string(_remainingUseCount));
        freeze();

        if (_remainingUseCount != 0)
            coolDown();
    }
}
