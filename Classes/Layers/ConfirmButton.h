#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef CONFIRM_BUTTON_H
#define CONFIRM_BUTTON_H

#include "cocos2d.h"
#include <string>

USING_NS_CC;
using namespace std;

class ConfirmButton : public Layer
{
public:
    static ConfirmButton* create(std::function<void()> func, std::function<void()> callBack);
    virtual bool init();

private:
    ConfirmButton(std::function<void()> func, std::function<void()> callBack);

private:
    Size _visibleSize;
    std::function<void()> func;
    std::function<void()> callBack;
};
#endif
