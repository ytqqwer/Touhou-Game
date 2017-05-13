#include "LogoAndDisclaimerScene.h"
#include "NonGameplayScenesCache.h"
#include "resources.h.dir/logo.h"
#include <string>

const std::string LogoAndDisclaimerScene::TAG{ "LogoAndDisclaimerScene" };

LogoAndDisclaimerScene::LogoAndDisclaimerScene()
{
    _visibleSize = _director->getVisibleSize();
}

Scene*
LogoAndDisclaimerScene::create()
{

    /*  1 if found cache */

    auto cached = NonGameplayScenesCache::getInstance()->getScene(TAG);
    if (cached)
        return cached;

    /*  2 if not found cache */

    auto pRet = new (std::nothrow) LogoAndDisclaimerScene();
    if (pRet && pRet->init()) {
        pRet->autorelease();
        NonGameplayScenesCache::getInstance()->addScene(TAG, pRet);
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool
LogoAndDisclaimerScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("LogoAndDisclaimerScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    /*  3. HelloWorld */

    auto helloWorld = Sprite::create(IMG_LOGO_HELLOWORLD);
    helloWorld->setPosition(_visibleSize / 2);
    this->addChild(helloWorld);

    return true;
}

void
LogoAndDisclaimerScene::update(float dt)
{
}
