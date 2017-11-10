#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/BackgroundIntroScene.h"
#include "JumpTableScene.h"
#include "NonGameplayScenes/MainMenuScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"

#include "resources.h.dir/background_introduce.h"

#include "AudioController.h"

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string BackgroundIntroScene::TAG{ "BackgroundIntroScene" };

BackgroundIntroScene::BackgroundIntroScene()
{
    _visibleSize = _director->getVisibleSize();
}

bool
BackgroundIntroScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("BackgroundIntroScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    /*  init particle touch listener */

    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = [this](const std::vector<Touch*>& touches, Event* event) { return; };
    listener->onTouchesEnded = [this](const std::vector<Touch*>& touches, Event* event) {
        auto touch = touches[0];
        auto _emitter = ParticleFlower::createWithTotalParticles(15);
        _emitter->setTexture(
            Director::getInstance()->getTextureCache()->addImage("particle/stars.png"));
        this->addChild(_emitter, 10);
        _emitter->setPosition(touch->getLocation());
        _emitter->setDuration(0.5);
        _emitter->setEmissionRate(30);
        _emitter->setLife(0.4);
        _emitter->setLifeVar(0.1);
        _emitter->setAutoRemoveOnFinish(true);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void
BackgroundIntroScene::onEnter()
{
    Scene::onEnter();

    AudioController::getInstance()->playMusic("bgm/bgm000.mp3", true);

    background = Sprite::create();
    this->addChild(background);
    label = Label::createWithTTF("", "fonts/dengxian.ttf", 25);
    label->setMaxLineWidth(600);
    label->setLineSpacing(20);
    label->setColor(Color3B(200, 200, 200));
    background->addChild(label);

    std::function<void(Ref*)> step1 = [&](Ref*) {
        background->setTexture(IMG_BACKGROUND_INTRODUCE_SEQUENCE_1);
        background->setContentSize(_visibleSize * 1.2);
        background->setPosition(_visibleSize / 2);
        label->setString("站在博丽神社门前的灵梦看着将要落山的太阳\n"
                         "『又好像是平静的一天呢』，她喃喃自语......");
        label->setPosition(_visibleSize.width / 3, _visibleSize.height * 2 / 3);
        background->runAction(MoveBy::create(5.0, Vec2(50, 0)));
    };

    std::function<void(Ref*)> step2 = [&](Ref*) {
        background->setTexture(IMG_BACKGROUND_INTRODUCE_SEQUENCE_2);
        background->setContentSize(_visibleSize * 1.2);
        background->setPosition(_visibleSize / 2);
        label->setString("『让我们把幻想乡变得更 『安静』 一些吧』\n"
                         "另一组看着夕阳的三人这么想到");
        label->setPosition(_visibleSize.width * 0.4, _visibleSize.height * 3 / 4);
        background->runAction(MoveBy::create(5.0, Vec2(-50, 0)));
    };

    std::function<void(Ref*)> step3 = [&](Ref*) {
        background->setTexture(IMG_BACKGROUND_INTRODUCE_SEQUENCE_3);
        background->setContentSize(_visibleSize * 1.3);
        background->setPosition(_visibleSize / 2);
        label->setString("幻想乡是否如同灵梦说的那样是平静\n"
                         "...还是说... 『安静』 呢 ?");
        label->setPosition(_visibleSize.width * 4 / 5, _visibleSize.height * 4 / 5);
        background->runAction(MoveBy::create(5.0, Vec2(50, 0)));
    };

    std::function<void(Ref*)> step4 = [&](Ref*) {
        _director->replaceScene(MainMenuScene::create());
    };

    auto sequence = Sequence::create(
        CallFuncN::create(step1), Sequence::create(FadeIn::create(0.3f), DelayTime::create(2.5f),
                                                   FadeOut::create(0.3f), NULL),
        CallFuncN::create(step2), Sequence::create(FadeIn::create(0.3f), DelayTime::create(2.5f),
                                                   FadeOut::create(0.3f), NULL),
        CallFuncN::create(step3), Sequence::create(FadeIn::create(0.3f), DelayTime::create(2.5f),
                                                   FadeOut::create(0.3f), NULL),
        CallFuncN::create(step4), NULL);
    background->runAction(sequence);

    sequence = Sequence::create(Sequence::create(FadeIn::create(0.3f), DelayTime::create(2.5f),
                                                 FadeOut::create(0.3f), NULL),
                                Sequence::create(FadeIn::create(0.3f), DelayTime::create(2.5f),
                                                 FadeOut::create(0.3f), NULL),
                                Sequence::create(FadeIn::create(0.3f), DelayTime::create(2.5f),
                                                 FadeOut::create(0.3f), NULL),
                                NULL);
    label->runAction(sequence);
}

void
BackgroundIntroScene::onExit()
{
    Scene::onExit();

    this->removeAllChildrenWithCleanup(true);
}
