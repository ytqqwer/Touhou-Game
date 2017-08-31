#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/LoadingLayer.h"
#include "GameplayScene/common.h"
#include "ui/CocosGUI.h"
#include <string>

using namespace std;

bool
LoadingLayer::init()
{
    /* 1. super init first */

    if (!Layer::init()) {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 visibleOrigin = Director::getInstance()->getVisibleOrigin();

    /* 2. 触摸截断 */
    this->setLocalZOrder(8888);
    this->setTouchEnabled(true);
    EventDispatcher* eventDispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* t, Event* e) { return true; };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,
                                                                                          this);

    /* 3.创建背景 */
    auto background = Sprite::create("settings_layer/background.png");
    background->setAnchorPoint(Vec2(0.5, 0.5));
    background->setScale(1.3);
    background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    addChild(background);

    /* 4.初始化进度条 */

    Sprite* progressBackground = Sprite::create("gameplayscene/sliderTrack.png"); //进度条底图
    Size progressBackSize = progressBackground->getContentSize();
    progressBackground->setPosition(
        Vec2(visibleOrigin.x + 100 + progressBackSize.width / 2, visibleOrigin.y + 60));
    this->addChild(progressBackground);

    loadingProgress =
        ProgressTimer::create(Sprite::create("gameplayscene/sliderProgress.png")); //进度条
    loadingProgress->setBarChangeRate(Vec2(1, 0));                                 //设置横向
    loadingProgress->setType(ProgressTimer::Type::BAR);                            //条状
    loadingProgress->setMidpoint(Vec2(0, 1)); //设置从左到右变化
    Size progressSize = loadingProgress->getContentSize();
    loadingProgress->setPosition(
        Vec2(visibleOrigin.x + 100 + progressSize.width / 2, visibleOrigin.y + 60));
    loadingProgress->setPercentage(0.0f);
    this->addChild(loadingProgress);

    text = Label::createWithTTF("加载中", "fonts/dengxian.ttf", 20);
    text->setPosition(
        Vec2(visibleOrigin.x + 100 + progressBackSize.width / 2, visibleOrigin.y + 60));
    text->setColor(Color3B::BLUE);
    text->setBMFontSize(20);
    this->addChild(text);

    /* 5.初始化监听器 */

    ELC = Director::getInstance()->getEventDispatcher()->addCustomEventListener(
        "loading_event", [this](EventCustom* e) {
            auto _loadingInfo = (LoadingInfo*)e->getUserData();
            progress = progress + _loadingInfo->progress;
            loadingProgress->setPercentage(progress);
            text->setString(_loadingInfo->information);
            if (progress >= 100) {
                std::function<void(Ref*)> delayRemove = [&](Ref*) {
                    _eventDispatcher->removeEventListener(ELC);
                    this->removeFromParentAndCleanup(true);
                };
                this->runAction(Sequence::create(DelayTime::create(0.50),
                                                 CallFuncN::create(delayRemove), NULL));
            }

        });

    return true;
}
