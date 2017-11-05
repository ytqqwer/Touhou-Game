#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/KourindouScene.h"
#include "Layers/SettingsLayer.h"
#include "NonGameplayScenes/EquipScene.h"
#include "NonGameplayScenes/InventoryScene.h"
#include "NonGameplayScenes/KnowledgeBaseScene.h"
#include "NonGameplayScenes/KourindouPurchaseScene.h"
#include "NonGameplayScenes/LocationSelectScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"

#include "GameData/Conversation.h"
#include "GameData/GameData.h"
#include "GameData/Location.h"

#include "resources.h.dir/kourindou.h"

#include "AudioController.h"

#include "ui/CocosGUI.h"
using namespace ui;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string KourindouScene::TAG{ "KourindouScene" };

KourindouScene::KourindouScene()
{
    _visibleSize = _director->getVisibleSize();
}

bool
KourindouScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("KourindouScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    auto location = GameData::getInstance()->getLocationByTag("Kourindou");
    /*背景*/
    auto bg_1 = Sprite::create(location.backgroundPicture);
    bg_1->setContentSize(_visibleSize);
    bg_1->setPosition(_visibleSize / 2);
    addChild(bg_1);

    /*购买按钮*/
    auto button_start = Button::create(IMG_KOURINDOU_BIG_BUTTON);
    button_start->setTitleFontName("fonts/dengxian.ttf");
    button_start->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.705));
    button_start->setTitleText("购买");
    button_start->setTitleFontSize(25);
    button_start->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playReturnButtonEffect();
            TransitionScene* transition =
                TransitionFade::create(0.5f, KourindouPurchaseScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(button_start);

    /*整备按钮*/
    auto button_equip = Button::create(IMG_KOURINDOU_BIG_BUTTON);
    button_equip->setTitleFontName("fonts/dengxian.ttf");
    button_equip->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.525));
    button_equip->setTitleText("整备");
    button_equip->setTitleFontSize(25);
    button_equip->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition = TransitionFade::create(0.5f, EquipScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(button_equip);

    /*道具库按钮*/
    auto button_inventory = Button::create(IMG_KOURINDOU_BIG_BUTTON);
    button_inventory->setTitleFontName("fonts/dengxian.ttf");
    button_inventory->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.345));
    button_inventory->setTitleText("道具库");
    button_inventory->setTitleFontSize(25);
    button_inventory->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition = TransitionFade::create(0.5f, InventoryScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(button_inventory);

    /*其他地图按钮*/
    auto button_map = Button::create(IMG_KOURINDOU_BIG_BUTTON);
    button_map->setTitleFontName("fonts/dengxian.ttf");
    button_map->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.165));
    button_map->setTitleText("前往其他地图");
    button_map->setTitleFontSize(25);
    button_map->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition =
                TransitionFade::create(0.5f, LocationSelectScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(button_map);

    /*地点,进度...等背景*/
    auto info_bg = Sprite::create(IMG_KOURINDOU_LITTLE_BUTTON_LAYOUT);
    info_bg->setAnchorPoint(Vec2(1, 1));
    info_bg->setContentSize(Size(480, 120));
    info_bg->setOpacity(50);
    info_bg->setPosition(Vec2(_visibleSize.width * 0.98, _visibleSize.height * 0.98));
    addChild(info_bg);

    /*钱币*/
    auto coin = Sprite::create("item/coin.png");
    coin->setAnchorPoint(Vec2(0.5, 0.5));
    coin->setPosition(Vec2(50, 60));
    info_bg->addChild(coin);

    money_text = Label::create("", "fonts/NotoSansCJKsc-Black.otf", 20);
    money_text->setAnchorPoint(Vec2(0.5, 0.5));
    money_text->setColor(Color3B::WHITE);
    money_text->setPosition(Vec2(100, 60));
    info_bg->addChild(money_text);

    /*系统商店*/
    auto store_button = Button::create(IMG_KOURINDOU_LITTLE_BUTTON);
    store_button->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    store_button->setAnchorPoint(Vec2(0.5, 0.5));
    store_button->setPosition(Vec2(200, 60));
    store_button->setTitleText("系统商店");
    store_button->setTitleFontSize(60);
    store_button->setTitleColor(Color3B::BLACK);
    store_button->setScale(0.33);
    store_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
        }
    });
    info_bg->addChild(store_button);

    /*资料库按钮*/
    auto know_button = Button::create(IMG_KOURINDOU_LITTLE_BUTTON);
    know_button->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    know_button->setAnchorPoint(Vec2(0.5, 0.5));
    know_button->setPosition(Vec2(300, 60));
    know_button->setTitleText("资料库");
    know_button->setTitleFontSize(60);
    know_button->setTitleColor(Color3B::BLACK);
    know_button->setScale(0.33);
    know_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition =
                TransitionFade::create(0.5f, KnowledgeBaseScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    info_bg->addChild(know_button);

    /*设置按钮*/
    auto set_button = Button::create(IMG_KOURINDOU_LITTLE_BUTTON);
    set_button->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    set_button->setAnchorPoint(Vec2(0.5, 0.5));
    set_button->setPosition(Vec2(400, 60));
    set_button->setTitleText("设置");
    set_button->setTitleColor(Color3B::BLACK);
    set_button->setTitleFontSize(60);
    set_button->setScale(0.33);
    set_button->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            auto lay = SettingsLayer::create("HomeScene");
            this->addChild(lay, 5);
        }
    });
    info_bg->addChild(set_button);

    /*  init particle touch listener */

    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = [this](const std::vector<Touch*>& touches, Event* event) { return; };
    listener->onTouchesEnded = [this](const std::vector<Touch*>& touches, Event* event) {
        auto touch = touches[0];
        auto _emitter = ParticleFlower::createWithTotalParticles(15);
        _emitter->setTexture(
            Director::getInstance()->getTextureCache()->addImage("Particle/stars.png"));
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
KourindouScene::onEnter()
{
    Scene::onEnter();

    auto location = GameData::getInstance()->getLocationByTag("Kourindou");

    /*背景音乐*/
    AudioController::getInstance()->playMusic(location.backgroundMusic, true);

    /*钱币*/
    char str[10];
    sprintf(str, "%ld", GameData::getInstance()->getMoneyNum());
    money_text->setString(str);

    /*对话图标*/
    double dis = 0.34;
    auto conversations = GameData::getInstance()->getConversationIndicators("Kourindou");
    for (int i = 0; i < conversations.size(); i++) {
        auto indicatorButton = ConversationIndicatorButton::create(conversations[i]);
        this->addChild(indicatorButton);
        vector_indicator.pushBack(indicatorButton);
        indicatorButton->setPosition(Vec2(_visibleSize.width * dis, _visibleSize.height * 0.13));
        dis += 0.08;
    }
}

void
KourindouScene::onExit()
{
    Scene::onExit();
    for (auto i : vector_indicator) {
        i->removeFromParentAndCleanup(false);
    }
    vector_indicator.clear();
}
