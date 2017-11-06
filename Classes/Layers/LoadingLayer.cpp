#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "Layers/LoadingLayer.h"
#include "GameplayScene/common.h"
#include "NonGameplayScenesCache.h"
#include "ui/CocosGUI.h"
#include <string>

using namespace std;

LoadingLayer*
LoadingLayer::create(const std::string& map)
{
    LoadingLayer* pRet = new (std::nothrow) LoadingLayer(map);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

LoadingLayer::LoadingLayer(const std::string& map)
{
    _map = map;
    progress = 0;
}

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
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* t, Event* e) {

        /* 显示粒子特效 */
        auto _emitter = ParticleFlower::createWithTotalParticles(15);
        _emitter->setTexture(
            Director::getInstance()->getTextureCache()->addImage("Particle/stars.png"));
        this->addChild(_emitter, 10);
        _emitter->setPosition(t->getLocation());
        _emitter->setDuration(0.5);
        _emitter->setEmissionRate(30);
        _emitter->setLife(0.4);
        _emitter->setLifeVar(0.1);
        _emitter->setAutoRemoveOnFinish(true);

        return true;
    };
    EventDispatcher* eventDispatcher = Director::getInstance()->getEventDispatcher();
    eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    /* 3.创建背景 */
    auto background = Sprite::create("background/blue_moon.png");
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

    text = Label::create("加载中", "fonts/dengxian.ttf", 20);
    text->setPosition(
        Vec2(visibleOrigin.x + 100 + progressBackSize.width / 2, visibleOrigin.y + 60));
    text->setColor(Color3B::BLUE);
    text->setBMFontSize(20);
    this->addChild(text);

    return true;
}

void
LoadingLayer::onEnter()
{
    /* 重要！必须首先重载父类的初始化函数 */
    Layer::onEnter();

    gameplayScene = GameplayScene::create(this->_map);
    //避免后续初始化过程中被释放
    gameplayScene->retain();

    this->initBackGround();
    this->initMap();
    this->initAnimationCache();
    this->initCharacter();
    this->initCtrlPanel();
    this->initArea();
    this->initCamera();
    this->initPhysicsContactListener();
    this->initCustomEventListener();
    gameplayScene->scheduleUpdate();
}

void
LoadingLayer::initBackGround()
{
    gameplayScene->initBackgroundAndForeground();

    //由于加载十分迅速，如果不延迟几乎察觉不到有读取界面的存在
    this->scheduleOnce(
        [this](float dt) {
            progress = progress + 10;
            loadingProgress->setPercentage(progress);
            text->setString("加载背景");

        },
        0.1, "bg");
}

void
LoadingLayer::initMap()
{
    gameplayScene->initMap();

    this->scheduleOnce(
        [this](float dt) {
            progress = progress + 40;
            loadingProgress->setPercentage(progress);
            text->setString("加载地图");
        },
        0.4, "map");
}

void
LoadingLayer::initAnimationCache()
{
    gameplayScene->initAnimationCache();

    this->scheduleOnce(
        [this](float dt) {
            progress = progress + 30;
            loadingProgress->setPercentage(progress);
            text->setString("加载动画缓存");
        },
        0.7, "animationCache");
}

void
LoadingLayer::initCharacter()
{
    gameplayScene->initCharacter();

    this->scheduleOnce(
        [this](float dt) {
            progress = progress + 10;
            loadingProgress->setPercentage(progress);
            text->setString("加载角色");
        },
        0.8, "character");
}

void
LoadingLayer::initCtrlPanel()
{
    gameplayScene->initCtrlPanel();
}

void
LoadingLayer::initArea()
{
    gameplayScene->initArea();
}

void
LoadingLayer::initCamera()
{
    gameplayScene->initCamera();
}

void
LoadingLayer::initPhysicsContactListener()
{
    gameplayScene->initPhysicsContactListener();
}

void
LoadingLayer::initCustomEventListener()
{
    gameplayScene->initCustomEventListener();

    this->scheduleOnce(
        [this](float dt) {
            progress = progress + 10;
            loadingProgress->setPercentage(progress);
            text->setString("加载事件");

            if (progress >= 100) {
                Scene* scene = this->gameplayScene;
                this->removeFromParentAndCleanup(true);
                //"this"已经被销毁，故需要提前保存其中的指向游戏场景的指针
                Director::getInstance()->popToRootScene();
                NonGameplayScenesCache::getInstance()->removeAllScenes();
                Director::getInstance()->replaceScene(scene);
                //先前手动retain一次，再配合使用一次release
                scene->release();
            }

        },
        1.0, "customEvent");
}
