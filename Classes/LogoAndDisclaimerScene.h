#ifndef LOGOANDDISCLAIMERSCENE_H
#define LOGOANDDISCLAIMERSCENE_H

#include "AppMacros.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

class LogoAndDisclaimerScene : public Scene
{
public:
    APP_SCENE_CREATE_FUNC(LogoAndDisclaimerScene, TAG);

    virtual bool init();
    virtual void update(float dt);

    void switchToJumpTableScene();

private:
    LogoAndDisclaimerScene();

private:
    static const std::string TAG;

    // intorspection
    Size _visibleSize;

    // game logic data
    bool switchToJumpTable = false; // C++11 in place initialization
};

#endif // LOGOANDDISCLAIMERSCENE_H
