#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/ArmsStoreScene.h"
#include "Layers/SettingsLayer.h"
#include "NonGameplayScenes/ArmsStorePurchaseScene.h"
#include "NonGameplayScenes/EquipScene.h"
#include "NonGameplayScenes/InventoryScene.h"
#include "NonGameplayScenes/KnowledgeBaseScene.h"
#include "NonGameplayScenes/LocationSelectScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"

#include "GameData/Conversation.h"
#include "GameData/GameData.h"
#include "GameData/Location.h"

// #include "resources.h.dir/arms_store.h"

#include "ui/CocosGUI.h"
using namespace ui;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string ArmsStoreScene::TAG{ "ArmsStoreScene" };

ArmsStoreScene::ArmsStoreScene()
{
    _visibleSize = _director->getVisibleSize();
}

bool
ArmsStoreScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("ArmsStoreScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    /*背景*/
    auto bg_1 = Sprite::create();
    bg_1->setContentSize(_visibleSize);
    bg_1->setPosition(_visibleSize / 2);
    addChild(bg_1);

    /*地点,进度...等背景*/
    auto info_bg = Sprite::create("homescene/p3.png");
    info_bg->setAnchorPoint(Vec2(0, 1));
    info_bg->setPosition(Vec2(_visibleSize.width * 0.52, _visibleSize.height));
    info_bg->setContentSize(Size(_visibleSize.width * 580 / 1280, _visibleSize.height * 106 / 720));
    info_bg->setOpacity(150);
    addChild(info_bg);

    /*地点艺术字*/
    // auto wordArt = Sprite::create();
    // wordArt->setTexture("KoumakanLibraryScene/word_art.png");
    // wordArt->setAnchorPoint(Vec2(0, 1));
    // wordArt->setContentSize(Size(_visibleSize.width * 0.15, _visibleSize.height * 0.077));
    // wordArt->setPosition(Vec2(_visibleSize.width * 0.53, _visibleSize.height * 0.955));
    // addChild(wordArt);

    /*钱币*/
    auto coin = Label::createWithTTF("钱币:", "fonts/dengxian.ttf", 20);
    coin->setAnchorPoint(Vec2(0, 1));
    coin->setPosition(Vec2(_visibleSize.width * 0.69, _visibleSize.height * 0.94));
    coin->setColor(Color3B::BLACK);
    addChild(coin);
    money_text = Label::createWithTTF("", "fonts/dengxian.ttf", 20);
    money_text->setPosition(Vec2(_visibleSize.width * 0.745, _visibleSize.height * 0.925));
    addChild(money_text);

    /*购买按钮*/
    auto button_start = Button::create("homescene/p1.png");
    button_start->setTitleFontName("fonts/dengxian.ttf");
    button_start->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.705));
    button_start->setTitleText("购买");
    button_start->setTitleFontSize(25);
    button_start->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect("button_click.wav");
            TransitionScene* transition =
                TransitionFade::create(0.5f, ArmsStorePurchaseScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(button_start);

    /*整备按钮*/
    auto button_equip = Button::create("homescene/p1.png");
    button_equip->setTitleFontName("fonts/dengxian.ttf");
    button_equip->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.525));
    button_equip->setTitleText("整备");
    button_equip->setTitleFontSize(25);
    button_equip->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect("button_click.wav");
            TransitionScene* transition = TransitionFade::create(0.5f, EquipScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(button_equip);

    /*道具库按钮*/
    auto button_inventory = Button::create("homescene/p1.png");
    button_inventory->setTitleFontName("fonts/dengxian.ttf");
    button_inventory->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.345));
    button_inventory->setTitleText("道具库");
    button_inventory->setTitleFontSize(25);
    button_inventory->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect("button_click.wav");
            TransitionScene* transition = TransitionFade::create(0.5f, InventoryScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(button_inventory);

    /*其他地图按钮*/
    auto button_map = Button::create("homescene/p1.png");
    button_map->setTitleFontName("fonts/dengxian.ttf");
    button_map->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.165));
    button_map->setTitleText("前往其他地图");
    button_map->setTitleFontSize(25);
    button_map->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect("button_click.wav");
            TransitionScene* transition =
                TransitionFade::create(0.5f, LocationSelectScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(button_map);

    /*资料库按钮*/
    auto know_button = Button::create("homescene/p5.png");
    know_button->setTitleFontName("fonts/dengxian.ttf");
    know_button->setAnchorPoint(Vec2(0, 1));
    know_button->setPosition(Vec2(_visibleSize.width * 0.84, _visibleSize.height * 0.99));
    know_button->setTitleText("资料库");
    know_button->setTitleFontSize(20);
    know_button->setScale(0.87);
    know_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED)
            SimpleAudioEngine::getInstance()->playEffect("button_click.wav");
        TransitionScene* transition = TransitionFade::create(0.5f, KnowledgeBaseScene::create());
        Director::getInstance()->pushScene(transition);
    });
    addChild(know_button);

    /*系统商店*/
    auto store_button = Button::create("homescene/p5.png");
    store_button->setTitleFontName("fonts/dengxian.ttf");
    store_button->setAnchorPoint(Vec2(0, 1));
    store_button->setPosition(Vec2(_visibleSize.width * 0.774, _visibleSize.height * 0.99));
    store_button->setTitleText("系统商店");
    store_button->setTitleFontSize(20);
    store_button->setScale(0.87);
    store_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED)
            SimpleAudioEngine::getInstance()->playEffect("button_click.wav");
    });
    addChild(store_button);

    /*设置按钮*/
    auto set_button = Button::create("homescene/p5.png");
    set_button->setTitleFontName("fonts/dengxian.ttf");
    set_button->setAnchorPoint(Vec2(0, 1));
    set_button->setPosition(Vec2(_visibleSize.width * 0.905, _visibleSize.height * 0.99));
    set_button->setTitleText("设置");
    set_button->setTitleFontSize(20);
    set_button->setScale(0.87);
    set_button->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect("button_click.wav");
            auto lay = SettingsLayer::create("HomeScene");
            this->addChild(lay, 5);
        }
    });
    addChild(set_button);

    return true;
}

void
ArmsStoreScene::onEnter()
{
    Scene::onEnter();

    /*背景音乐*/
    auto playMusic = SimpleAudioEngine::getInstance();
    // playMusic->stopBackgroundMusic();
    // playMusic->playBackgroundMusic(" ", true);

    /*钱币*/
    // auto money = GameData::getInstance()->getMoneyNum();
    // stringstream ss;
    // ss << money;
    char str[10];
    sprintf(str, "%ld", GameData::getInstance()->getMoneyNum());
    money_text->setColor(Color3B::BLACK);
    money_text->setString(str);

    /*对话图标*/
    double dis = 0.34;
    auto conversations = GameData::getInstance()->getConversationIndicators("ArmsStore");
    for (int i = 0; i < conversations.size(); i++) {
        auto indicatorButton = ConversationIndicatorButton::create(conversations[i]);
        this->addChild(indicatorButton);
        vector_indicator.pushBack(indicatorButton);
        indicatorButton->setPosition(Vec2(_visibleSize.width * dis, _visibleSize.height * 0.13));
        dis += 0.08;
    }
}

void
ArmsStoreScene::onExit()
{
    Scene::onExit();
    for (auto i : vector_indicator) {
        i->removeFromParentAndCleanup(false);
    }
    vector_indicator.clear();
}
