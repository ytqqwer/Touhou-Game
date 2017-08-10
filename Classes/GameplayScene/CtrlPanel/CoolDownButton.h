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
    /*  子类必须在 subClassPreInit 中做如下操作：
     *    - 初始化 _remainingUseCount, _coolDownTime
     *    - 设置 Button 的纹理和大小
     */
    virtual void subClassPreInit() = 0;
    virtual void initRemainingUseLabel();
    virtual void initRemainingCoolDownTimeLabel();
    virtual void initTouchListener() = 0;

    virtual void freeze();
    virtual void unFreeze();
    virtual void coolDown();
    virtual void useOnce();

protected:
    int _remainingUseCount; // -1 表示没有使用次数限制
    float _coolDownTime;
    float _remainingCoolDownTime = 0;

    Label* _remainingUseLabel;
    Label* _remainingCoolDownTimeLabel;
};

#endif // COOL_DOWN_BUTTON_H
