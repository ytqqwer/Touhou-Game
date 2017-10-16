#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef BLEND_ACTION_H
#define BLEND_ACTION_H

#include "cocos2d.h"
USING_NS_CC;

class BlendAction : public ActionInterval
{
public:
    static BlendAction* create(float duration, Color4B blendColor, bool reverse = false);

    virtual BlendAction* clone() const override;
    virtual BlendAction* reverse(void) const override;

    virtual void update(float t) override;
    virtual void startWithTarget(Node* target) override;
    virtual void stop() override;

    CC_CONSTRUCTOR_ACCESS : BlendAction() {}
    virtual ~BlendAction() {}

    bool initWithDuration(float duration, Color4B blendColor, bool reverse);

protected:
    GLProgramState* _pstate;
    bool _reverse;
    float _percent;
    Color4B _color;

private:
    BlendAction(const BlendAction&) = delete;
    BlendAction& operator=(const BlendAction&) = delete;
};

#endif
