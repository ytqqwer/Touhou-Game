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
AnimateManager::addPlayerTexture(const std::string& tag)
{
    if (tag == "Reimu") {
        return "gameplayscene/Reimu/walkFront000.png";
    } else if (tag == "Marisa") {
        return "gameplayscene/Marisa/walkFront000.png";
    }
}

Animation*
AnimateManager::addStandCache(const std::string& tag)
{
    auto standAnimation = Animation::create();

    if (tag == "Reimu") {
        for (int i = 0; i <= 9; i++)
            standAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/stand00" +
                                                   std::to_string(i) + ".png");
        standAnimation->setDelayPerUnit(0.1f);
        AnimationCache::getInstance()->addAnimation(standAnimation, "ReimuStandAnimation");
    } else if (tag == "Marisa") {
        for (int i = 0; i <= 9; i++)
            standAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/stand00" +
                                                   std::to_string(i) + ".png");
        standAnimation->setDelayPerUnit(0.1f);
        AnimationCache::getInstance()->addAnimation(standAnimation, "MarisaStandAnimation");
    }

    return standAnimation;
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
AnimateManager::addPreJumpCache(const std::string& tag)
{
    auto preJumpAnimation = Animation::create();

    if (tag == "Reimu") {
        for (int i = 0; i <= 0; i++)
            preJumpAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/jump00" +
                                                     std::to_string(i) + ".png");
        preJumpAnimation->setDelayPerUnit(0.1f);
        AnimationCache::getInstance()->addAnimation(preJumpAnimation, "ReimuPreJumpAnimation");
    } else if (tag == "Marisa") {
        for (int i = 1; i <= 4; i++)
            preJumpAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/jump00" +
                                                     std::to_string(i) + ".png");
        preJumpAnimation->setDelayPerUnit(0.08f);
        AnimationCache::getInstance()->addAnimation(preJumpAnimation, "MarisaPreJumpAnimation");
    }

    return preJumpAnimation;
}

Animation*
AnimateManager::addJumpCache(const std::string& tag)
{
    auto jumpAnimation = Animation::create();

    if (tag == "Reimu") {
        for (int i = 1; i <= 1; i++)
            jumpAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/jump00" + std::to_string(i) +
                                                  ".png");
        jumpAnimation->setDelayPerUnit(0.2f);
        AnimationCache::getInstance()->addAnimation(jumpAnimation, "ReimuJumpAnimation");
    } else if (tag == "Marisa") {
        for (int i = 5; i <= 5; i++)
            jumpAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/jump00" +
                                                  std::to_string(i) + ".png");
        jumpAnimation->setDelayPerUnit(0.2f);
        AnimationCache::getInstance()->addAnimation(jumpAnimation, "MarisaJumpAnimation");
    }

    return jumpAnimation;
}

Animation*
AnimateManager::addPreFallCache(const std::string& tag)
{
    auto preFallAnimation = Animation::create();

    if (tag == "Reimu") {
        for (int i = 1; i <= 1; i++)
            preFallAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/jump00" +
                                                     std::to_string(i) + ".png");
        preFallAnimation->setDelayPerUnit(0.1f);
        AnimationCache::getInstance()->addAnimation(preFallAnimation, "ReimuPreFallAnimation");
    } else if (tag == "Marisa") {
        for (int i = 6; i <= 9; i++)
            preFallAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/jump00" +
                                                     std::to_string(i) + ".png");
        preFallAnimation->setDelayPerUnit(0.08f);
        AnimationCache::getInstance()->addAnimation(preFallAnimation, "MarisaPreFallAnimation");
    }

    return preFallAnimation;
}

Animation*
AnimateManager::addFallCache(const std::string& tag)
{
    auto fallAnimation = Animation::create();

    if (tag == "Reimu") {
        for (int i = 2; i <= 4; i++)
            fallAnimation->addSpriteFrameWithFile("gameplayscene/Reimu/jump00" + std::to_string(i) +
                                                  ".png");
        fallAnimation->setDelayPerUnit(0.1f);
        AnimationCache::getInstance()->addAnimation(fallAnimation, "ReimuFallAnimation");
    } else if (tag == "Marisa") {
        for (int i = 10; i <= 11; i++)
            fallAnimation->addSpriteFrameWithFile("gameplayscene/Marisa/jump0" + std::to_string(i) +
                                                  ".png");
        fallAnimation->setDelayPerUnit(0.15f);
        AnimationCache::getInstance()->addAnimation(fallAnimation, "MarisaFallAnimation");
    }

    return fallAnimation;
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
    if (tag == "opossum") {
        return "gameplayscene/Enemy/opossum-1.png";
    } else if (tag == "frog") {
        return "gameplayscene/Enemy/frog-idle-1.png";
    }
}

Animation*
AnimateManager::addEnemyCache(const std::string& tag)
{
    Animation* enemyAnimation = Animation::create();

    if (tag == "opossum") {
        for (int i = 1; i <= 6; i++)
            enemyAnimation->addSpriteFrameWithFile("gameplayscene/Enemy/opossum-" +
                                                   std::to_string(i) + ".png");
        enemyAnimation->setDelayPerUnit(0.15f);
        AnimationCache::getInstance()->addAnimation(enemyAnimation, "OpossumAnimation");
    } else if (tag == "frog") {
        for (int i = 1; i <= 4; i++)
            enemyAnimation->addSpriteFrameWithFile("gameplayscene/Enemy/frog-idle-" +
                                                   std::to_string(i) + ".png");
        enemyAnimation->setDelayPerUnit(0.15f);
        AnimationCache::getInstance()->addAnimation(enemyAnimation, "FrogAnimation");
    }

    return enemyAnimation;
}
