#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

//#include "GameData/GameData.h"
#include "AnimateManager.h"
using namespace std;

AnimateManager* AnimateManager::_self;

AnimateManager::AnimateManager()
{
}

AnimateManager*
AnimateManager::getInstance()
{
    if (!_self) {
        _self = new AnimateManager();
    }
    return _self;
}

std::string
AnimateManager::addTexture(const std::string& tag)
{
    if (tag == "Reimu") {
        return "gameplayscene/Reimu/walkFront000.png";
    } else if (tag == "Marisa") {
        return "gameplayscene/Marisa/walkFront000.png";
    }
}

Animation*
AnimateManager::addRunCache(const std::string& tag)
{
    auto runAnimation = Animation::create();

    if (tag == "Reimu") {
        for (int i = 0; i <= 7; i++)
            runAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/walkFront00" +
                                                 std::to_string(i) + ".png");
        runAnimation->setDelayPerUnit(0.15f);
        AnimationCache::getInstance()->addAnimation(runAnimation, "ReimuRunAnimation");
    } else if (tag == "Marisa") {
        for (int i = 0; i <= 7; i++)
            runAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/walkFront00" +
                                                 std::to_string(i) + ".png");
        runAnimation->setDelayPerUnit(0.15f);
        AnimationCache::getInstance()->addAnimation(runAnimation, "MarisaRunAnimation");
    }

    return runAnimation;
}

Animation*
AnimateManager::addJumpCache(const std::string& tag)
{
    auto jumpAnimation = Animation::create();

    if (tag == "Reimu") {
        for (int i = 0; i <= 8; i++)
            jumpAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/jump00" + std::to_string(i) +
                                                  ".png");
        jumpAnimation->setDelayPerUnit(0.11f);
        AnimationCache::getInstance()->addAnimation(jumpAnimation, "ReimuJumpAnimation");
    } else if (tag == "Marisa") {
        for (int i = 0; i <= 9; i++)
            jumpAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/jumpFront00" +
                                                  std::to_string(i) + ".png");
        jumpAnimation->setDelayPerUnit(0.1f);
        AnimationCache::getInstance()->addAnimation(jumpAnimation, "MarisaJumpAnimation");
    }

    return jumpAnimation;
}

Animation*
AnimateManager::addDashCache(const std::string& tag)
{
    auto dashAnimation = Animation::create();

    if (tag == "Reimu") {
        for (int i = 0; i <= 7; i++)
            dashAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/dashFront00" +
                                                  std::to_string(i) + ".png");
        dashAnimation->setDelayPerUnit(0.07f);
        AnimationCache::getInstance()->addAnimation(dashAnimation, "ReimuDashAnimation");
    } else if (tag == "Marisa") {
        for (int i = 0; i <= 8; i++)
            dashAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/dashFront00" +
                                                  std::to_string(i) + ".png");
        dashAnimation->setDelayPerUnit(0.07f);
        AnimationCache::getInstance()->addAnimation(dashAnimation, "MarisaDashAnimation");
    }

    return dashAnimation;
}

std::string
AnimateManager::addEnemyTexture(const std::string& tag)
{
    if (tag == "Enemy") {
        return "gameplayscene/Enemy/opossum-1.png";
    }
}

Animation*
AnimateManager::addEnemyCache(const std::string& tag)
{
    Animation* enemyAnimation = Animation::create();

    if (tag == "Enemy") {
        for (int i = 1; i <= 6; i++)
            enemyAnimation->addSpriteFrameWithFile("gameplayscene/Enemy/opossum-" +
                                                   std::to_string(i) + ".png");
        enemyAnimation->setDelayPerUnit(0.15f);
        AnimationCache::getInstance()->addAnimation(enemyAnimation, "EnemyAnimation");
    }

    return enemyAnimation;
}
