#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef COOL_DOWN_BUTTON_H
#define COOL_DOWN_BUTTON_H

#include "cocos2d.h"
#include "ui/CocosGUI.h"

using cocos2d::ui::Button;
using cocos2d::Label;

class CoolDownButton : public Button
{
public:
    virtual bool init() override;

public:
    // 供 Scheduler 使用
    virtual void coolDownBySecondSchedule(float remaining);

protected:
    virtual void initUseCountAndCoolDownTime() = 0;
    virtual void initRemainingUseCountLabel();
    virtual void initRemainingTimeLabel();
    virtual void initTouchListener() = 0;

    virtual void freeze();
    virtual void unFreeze();
    virtual void coolDown();
    virtual void useOnce();

protected:
    // 子类需覆盖 initUseCountAndCoolDownTime 正确的赋值以下数据成员
    int _remainingUseCount; // -1 表示没有使用次数限制
    float _coolDownTime;
    float _remainingCoolDownTime = 0;

    Label* _remainingUseLabel;
    Label* _remainingTimeLabel;
};

#endif // COOL_DOWN_BUTTON_H
