#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ANIMATE_MANAGER_H
#define ANIMATE_MANAGER_H

#include "cocos2d.h"
USING_NS_CC;

class AnimateManager
{
public:
    AnimateManager();

    static AnimateManager* getInstance();

    std::string addPlayerTexture(const std::string& tag);
    Animation* addStandCache(const std::string& tag);
    Animation* addRunCache(const std::string& tag);
    Animation* addPreJumpCache(const std::string& tag);
    Animation* addJumpCache(const std::string& tag);
    Animation* addPreFallCache(const std::string& tag);
    Animation* addFallCache(const std::string& tag);
    Animation* addDashCache(const std::string& tag);

    std::string addEnemyTexture(const std::string& tag);
    Animation* addEnemyCache(const std::string& tag);

private:
    static AnimateManager* _self;
};

#endif
