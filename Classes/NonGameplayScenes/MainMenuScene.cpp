#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/MainMenuScene.h"
#include "GameData/GameData.h"
#include "JumpTableScene.h"
#include "Layers/SettingsLayer.h"
#include "NonGameplayScenes/HomeScene.h"
#include "NonGameplayScenes/SaveScene.h"
#include "NonGameplayScenes/StaffScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"

#include "AudioController.h"

#include "resources.h.dir/main_menu.h"

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string MainMenuScene::TAG{ "MainMenuScene" };

MainMenuScene::MainMenuScene()
{
    _visibleSize = _director->getVisibleSize();
}

bool
MainMenuScene::init()
{
    /* 1. super init first */

    if (!Scene::init()) {
        return false;
    }

/* 2. debug info */

#ifndef NDEBUG

    auto JTButton = Button::create("", "", "");
    JTButton->setTitleText("跳转表");
    JTButton->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    JTButton->setTitleColor(Color3B(194, 134, 11));
    JTButton->setTitleFontSize(30);
    JTButton->setAnchorPoint(Vec2(0, 0));
    JTButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.13));
    JTButton->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition = TransitionFade::create(1.0f, JumpTableScene::create());
            Director::getInstance()->replaceScene(transition);
        }
    });
    addChild(JTButton);

#endif

    /* 3. init background */

    backGround = Sprite::create(IMG_MAIN_MENU_BACKGROUND);
    backGround->setScale(1.35);
    backGround->setPosition(_visibleSize / 2);
    addChild(backGround, -1);

    auto test1 = Sprite::create("comboLimit.png");
    test1->setPosition(_visibleSize.width * 0.2, _visibleSize.height * 0.13);
    BlendFunc cbl = { GL_SRC_ALPHA, GL_ONE };
    test1->setBlendFunc(cbl);
    addChild(test1);
    auto test2 = Sprite::create("comboLimit.png");
    test2->setPosition(_visibleSize.width * 0.4, _visibleSize.height * 0.33);
    BlendFunc cb2 = { GL_ONE, GL_SRC_ALPHA };
    test2->setBlendFunc(cb2);
    addChild(test2);

    /* 4. init particle touch listener */

    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesEnded = [this](const std::vector<Touch*>& touches, Event* event) {
        auto touch = touches[0];
        auto _emitter = ParticleFlower::createWithTotalParticles(15);
        _emitter->setTexture(
            Director::getInstance()->getTextureCache()->addImage("Particle/stars.png"));
        this->addChild(_emitter, 10);
        _emitter->setPosition(touch->getLocation());
        _emitter->setDuration(0.5);
        //_emitter->setEmissionRate(_emitter->getTotalParticles()/ _emitter->getLife());
        _emitter->setEmissionRate(30);
        _emitter->setLife(0.4);
        _emitter->setLifeVar(0.1);
        _emitter->setAutoRemoveOnFinish(true);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    /* 5. init button */

    /*新游戏*/
    auto NGButton = Button::create("", "", "");
    NGButton->setTitleText("新游戏");
    NGButton->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    NGButton->setTitleColor(Color3B(194, 134, 11));
    NGButton->setTitleFontSize(50);
    NGButton->setAnchorPoint(Vec2(0, 0));
    NGButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.71));
    NGButton->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            auto canNew = GameData::getInstance()->newGame();
            if (canNew) {
                AudioController::getInstance()->stopMusic();
                AudioController::getInstance()->playClickButtonEffect();
                TransitionScene* transition = TransitionFade::create(1.0f, HomeScene::create());
                Director::getInstance()->replaceScene(transition);
            }
        }
    });
    addChild(NGButton);

    /*继续上次游戏*/
    auto CButton = Button::create("", "", "");
    CButton->setTitleText("继续游戏");
    CButton->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    CButton->setTitleColor(Color3B(194, 134, 11));
    CButton->setTitleFontSize(50);
    CButton->setAnchorPoint(Vec2(0, 0));
    CButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.57));
    CButton->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            auto tag = GameData::getInstance()->getCurrentSaveTag();
            if (tag == 0) {
                return;
            }
            GameData::getInstance()->continueGame();
            AudioController::getInstance()->stopMusic();
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition = TransitionFade::create(1.0f, HomeScene::create());
            Director::getInstance()->replaceScene(transition);
        }
    });
    addChild(CButton);

    /*载入游戏*/
    auto LGButton = Button::create("", "", "");
    LGButton->setTitleText("载入游戏");
    LGButton->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    LGButton->setTitleColor(Color3B(194, 134, 11));
    LGButton->setTitleFontSize(50);
    LGButton->setAnchorPoint(Vec2(0, 0));
    LGButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.43));
    LGButton->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition = TransitionFade::create(1.0f, SaveScene::create(false));
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(LGButton);

    /*设置*/
    auto SGButton = Button::create("", "", "");
    SGButton->setTitleText("设置");
    SGButton->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    SGButton->setTitleColor(Color3B::WHITE);
    SGButton->setTitleFontSize(30);
    SGButton->setAnchorPoint(Vec2(0, 0));
    SGButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.34));
    SGButton->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            auto lay = SettingsLayer::create("MainMenuScene");
            this->addChild(lay, 5);
        }
    });
    addChild(SGButton);

    /*职员表*/
    auto ZGButton = Button::create("", "", "");
    ZGButton->setTitleText("职员表");
    ZGButton->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    ZGButton->setTitleColor(Color3B::WHITE);
    ZGButton->setTitleFontSize(30);
    ZGButton->setAnchorPoint(Vec2(0, 0));
    ZGButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.27));
    addChild(ZGButton);
    ZGButton->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition = TransitionFade::create(1.0f, StaffScene::create());
            Director::getInstance()->replaceScene(transition);
        }
    });

    /*退出游戏*/
    auto exitButton = Button::create("", "", "");
    exitButton->setTitleText("退出");
    exitButton->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    exitButton->setTitleColor(Color3B::WHITE);
    exitButton->setTitleFontSize(25);
    exitButton->setAnchorPoint(Vec2(0, 0));
    exitButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.2));
    exitButton->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->end();
        }
    });
    addChild(exitButton);

    auto menu = Menu::create(nullptr);

    return true;
}

void
MainMenuScene::onEnter()
{
    Scene::onEnter();

    AudioController::getInstance()->playMusic("bgm/bgm000.mp3", true);

    auto move = MoveBy::create(40, Vec2(0, _visibleSize.height / 4));
    auto move_back = move->reverse();
    auto seq = Sequence::create(move, move_back, nullptr);
    backGround->runAction(RepeatForever::create(seq));
    backGround->setPosition(_visibleSize / 2);
}

void
MainMenuScene::onExit()
{
    Scene::onExit();

    backGround->stopAllActions();
}
