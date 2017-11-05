#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/HomeScene.h"
#include "Layers/SettingsLayer.h"
#include "NonGameplayScenes/EquipScene.h"
#include "NonGameplayScenes/InventoryScene.h"
#include "NonGameplayScenes/KnowledgeBaseScene.h"
#include "NonGameplayScenes/KoumakanLibraryScene.h"
#include "NonGameplayScenes/LocationSelectScene.h"
#include "NonGameplayScenes/MainMenuScene.h"
#include "NonGameplayScenes/RoundSelectScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"

#include "GameData/Conversation.h"
#include "GameData/Location.h"

#include "resources.h.dir/home.h"

#include "AudioController.h"

#include "ui/CocosGUI.h"
using namespace ui;

#include "cocos-ext.h"
using namespace cocos2d::extension;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string HomeScene::TAG{ "HomeScene" };

HomeScene::HomeScene()
    : people_order(0)
{
    gamedata = GameData::getInstance();
    _visibleSize = _director->getVisibleSize();
}

bool
HomeScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto UpdateButton = Button::create();
    UpdateButton->setTitleText("新增测试数据");
    UpdateButton->setTitleFontName("fonts/dengxian.ttf");
    UpdateButton->setTitleColor(Color3B::BLACK);
    UpdateButton->setTitleFontSize(30);
    UpdateButton->setAnchorPoint(Vec2(0, 1));
    UpdateButton->setPosition(Vec2(_visibleSize.width * 0.03, _visibleSize.height));
    UpdateButton->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            auto tag = GameData::getInstance()->getCurrentSaveTag();
            if (tag == 0) {
                return;
            }
            AudioController::getInstance()->playClickButtonEffect();
            GameData::getInstance()->updateSave("U1");
        }
    });
    addChild(UpdateButton);

#endif

    /*出发按钮*/
    auto button_start = Button::create(IMG_HOME_BIG_BUTTON);
    button_start->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    button_start->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.705));
    button_start->setTitleText("出发");
    button_start->setTitleFontSize(75);
    button_start->setTitleColor(Color3B::BLACK);
    button_start->setScale(0.33);
    // button_start->setScale9Enabled(true);
    // button_start->setCapInsets(Rect(50, 70, 567, 242));
    // button_start->setContentSize(Size(200,100));
    button_start->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition = TransitionFade::create(0.5f, RoundSelectScene::create());
            Director::getInstance()->replaceScene(transition);
        }
    });
    addChild(button_start);

    /*整备按钮*/
    auto button_equip = Button::create(IMG_HOME_BIG_BUTTON);
    button_equip->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    button_equip->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.525));
    button_equip->setTitleText("整备");
    button_equip->setTitleFontSize(75);
    button_equip->setTitleColor(Color3B::BLACK);
    button_equip->setScale(0.33);
    button_equip->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition = TransitionFade::create(0.5f, EquipScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(button_equip);

    /*道具库按钮*/
    auto button_inventory = Button::create(IMG_HOME_BIG_BUTTON);
    button_inventory->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    button_inventory->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.345));
    button_inventory->setTitleText("道具库");
    button_inventory->setTitleFontSize(75);
    button_inventory->setTitleColor(Color3B::BLACK);
    button_inventory->setScale(0.33);
    button_inventory->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition = TransitionFade::create(0.5f, InventoryScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(button_inventory);

    /*其他地图按钮*/
    auto button_map = Button::create(IMG_HOME_BIG_BUTTON);
    button_map->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    button_map->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.165));
    button_map->setTitleText("前往其他地图");
    button_map->setTitleFontSize(75);
    button_map->setTitleColor(Color3B::BLACK);
    button_map->setScale(0.33);
    button_map->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition =
                TransitionFade::create(0.5f, LocationSelectScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(button_map);

    /*更换角色*/
    auto role_change = Button::create(IMG_HOME_SWITCH_CHARACTER_BUTTON);
    role_change->setAnchorPoint(Vec2(0, 0));
    role_change->setPosition(Vec2(_visibleSize.width * 0.01, _visibleSize.height * 0.45));
    role_change->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            people_order++;
            people_order %= people_array.size();
            this->getPeople();
        }
    });
    addChild(role_change);

    /*卡片*/
    cards[0] = Sprite::create();
    cards[0]->setPosition(Vec2(_visibleSize.width * 0.042, _visibleSize.height * 0.33));
    addChild(cards[0]);

    cards[1] = Sprite::create();
    cards[1]->setPosition(Vec2(_visibleSize.width * 0.042, _visibleSize.height * 0.2));
    addChild(cards[1]);

    cards[2] = Sprite::create();
    cards[2]->setPosition(Vec2(_visibleSize.width * 0.042, _visibleSize.height * 0.07));
    addChild(cards[2]);

    /*地点,进度...等背景*/
    auto information_Layout = Scale9Sprite::create(IMG_HOME_LITTLE_BUTTON_LAYOUT);
    information_Layout->setAnchorPoint(Vec2(1, 1));
    information_Layout->setContentSize(Size(680, 120));
    information_Layout->setPosition(Vec2(_visibleSize.width * 0.98, _visibleSize.height * 0.98));
    information_Layout->setOpacity(50);
    addChild(information_Layout);

    /*设置按钮*/
    auto set_button = Button::create(IMG_HOME_LITTLE_BUTTON);
    set_button->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    set_button->setAnchorPoint(Vec2(0.5, 0.5));
    set_button->setPosition(Vec2(600, 60));
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
    information_Layout->addChild(set_button);

    /*资料库按钮*/
    auto know_button = Button::create(IMG_HOME_LITTLE_BUTTON);
    know_button->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    know_button->setAnchorPoint(Vec2(0.5, 0.5));
    know_button->setPosition(Vec2(500, 60));
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
    information_Layout->addChild(know_button);

    /*系统商店*/
    auto store_button = Button::create(IMG_HOME_LITTLE_BUTTON);
    store_button->setTitleFontName("fonts/NotoSansCJKsc-Black.otf");
    store_button->setAnchorPoint(Vec2(0.5, 0.5));
    store_button->setPosition(Vec2(400, 60));
    store_button->setTitleText("系统商店");
    store_button->setTitleFontSize(60);
    store_button->setTitleColor(Color3B::BLACK);
    store_button->setScale(0.33);
    store_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
        }
    });
    information_Layout->addChild(store_button);

    /*进度*/
    prog_text = Label::create("", "fonts/NotoSansCJKsc-Black.otf", 20);
    prog_text->setAnchorPoint(Vec2(0.5, 0.5));
    prog_text->setPosition(Vec2(200, 60));
    prog_text->setColor(Color3B::WHITE);
    information_Layout->addChild(prog_text);

    /*钱币*/
    auto coin = Sprite::create("item/coin.png");
    coin->setAnchorPoint(Vec2(0.5, 0.5));
    coin->setPosition(Vec2(250, 60));
    information_Layout->addChild(coin);

    money_text = Label::create("", "fonts/NotoSansCJKsc-Black.otf", 20);
    money_text->setAnchorPoint(Vec2(0.5, 0.5));
    money_text->setColor(Color3B::WHITE);
    money_text->setPosition(Vec2(300, 60));
    information_Layout->addChild(money_text);

    /*艺术字*/
    wordArt = Sprite::create();
    wordArt->setAnchorPoint(Vec2(0.5, 0.5));
    wordArt->setPosition(Vec2(140, 60));
    information_Layout->addChild(wordArt);

    //背景
    backGround = Sprite::create();
    backGround->setAnchorPoint(Vec2(0.5, 0.5));
    backGround->setPosition(_visibleSize / 2);
    this->addChild(backGround, -1);

    //立绘
    personPortrait = Sprite::create();
    personPortrait->setAnchorPoint(Vec2(0, 0));
    personPortrait->setPosition(Vec2(_visibleSize.width * 0.05, 0));
    personPortrait->setScale(1.3);
    this->addChild(personPortrait);

    /* init particle touch listener */

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
HomeScene::onEnter()
{
    Scene::onEnter();

    auto location = gamedata->getCurrentLocation();
    people_array = gamedata->getAvailableCharacterList();

    /*背景音乐*/
    AudioController::getInstance()->playMusic(location.backgroundMusic, true);

    /*背景*/
    backGround->setTexture(location.backgroundPicture);
    backGround->setContentSize(_visibleSize);

    /*地点艺术字*/
    wordArt->setTexture(location.wordArt);

    /*具体进度*/
    int cnt = 0;
    char str[10];
    sprintf(str, "%d", location.passedRound);
    cnt = strlen(str);
    str[cnt++] = '/';
    sprintf(str + cnt, "%d", location.totalRound);
    string prog = str;
    prog_text->setString(prog);

    /*具体钱币*/
    sprintf(str, "%ld", gamedata->getMoneyNum());
    prog = str;
    money_text->setString(prog);

    /*人物*/
    getPeople();

    /*对话图标*/
    double dis = 0.34;
    auto conversations = gamedata->getConversationIndicators(location.tag);
    for (int i = 0; i < conversations.size(); i++) {
        ConversationIndicatorButton* indicatorButton =
            ConversationIndicatorButton::create(conversations[i]);
        this->addChild(indicatorButton);
        vector_indicator.pushBack(indicatorButton);
        indicatorButton->setPosition(Vec2(_visibleSize.width * dis, _visibleSize.height * 0.13));
        dis += 0.08;
    }
}

void
HomeScene::getPeople()
{
    /*肖像*/
    personPortrait->setTexture(people_array.at(people_order).portrait);

    /*卡片*/
    card = gamedata->getCharacterEquipedSpellCards(people_array[people_order].tag);
    string img;
    double width = 0.041, height = 0.065;
    for (int i = 0; i < 3; i++) {
        if (i < card.size())
            img = card[i].icon;
        else {
            img = IMG_HOME_CHARACTER_CARD_DISABLE;
            width = height = 0.047;
        }
        cards[i]->setTexture(img);
    }
}

void
HomeScene::onExit()
{
    Scene::onExit();

    for (auto i : vector_indicator) {
        i->removeFromParentAndCleanup(false);
    }
    vector_indicator.clear();
}
