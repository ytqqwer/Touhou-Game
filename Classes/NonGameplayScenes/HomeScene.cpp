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

// #include "resources.h.dir/home.h"

#include "AudioController.h"

#include "ui/CocosGUI.h"
using namespace ui;

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

    /* 返回按钮*/
    auto ret = Button::create("", "", "");
    ret->setTitleText("返回");
    ret->setTitleColor(Color3B(0, 0, 0));
    ret->setTitleFontSize(20);
    ret->setAnchorPoint(Vec2(0, 1));
    ret->setPosition(Vec2(0, _visibleSize.height));
    ret->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->replaceScene(MainMenuScene::create());
        }
    });
    addChild(ret, 2);

    /*出发按钮*/
    auto button_start = Button::create("homescene/p1.png");
    button_start->setTitleFontName("fonts/dengxian.ttf");
    button_start->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.705));
    button_start->setTitleText("出发");
    button_start->setTitleFontSize(25);
    button_start->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition = TransitionFade::create(0.5f, RoundSelectScene::create());
            Director::getInstance()->replaceScene(transition);
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
            AudioController::getInstance()->playClickButtonEffect();
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
            AudioController::getInstance()->playClickButtonEffect();
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
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition =
                TransitionFade::create(0.5f, LocationSelectScene::create());
            Director::getInstance()->pushScene(transition);
        }
    });
    addChild(button_map);

    /*更换角色*/
    auto role_change = Button::create("homescene/p4.png");
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
    auto info_bg = Sprite::create("homescene/p3.png");
    info_bg->setAnchorPoint(Vec2(0, 1));
    info_bg->setPosition(Vec2(_visibleSize.width * 0.52, _visibleSize.height));
    info_bg->setOpacity(150);
    addChild(info_bg);

    /*资料库按钮*/
    auto know_button = Button::create("homescene/p5.png");
    know_button->setTitleFontName("fonts/dengxian.ttf");
    know_button->setAnchorPoint(Vec2(0, 1));
    know_button->setPosition(Vec2(_visibleSize.width * 0.84, _visibleSize.height * 0.99));
    know_button->setTitleText("资料库");
    know_button->setTitleFontSize(20);
    know_button->setScale(0.87);
    know_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            TransitionScene* transition =
                TransitionFade::create(0.5f, KnowledgeBaseScene::create());
            Director::getInstance()->pushScene(transition);
        }
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
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
        }
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
            AudioController::getInstance()->playClickButtonEffect();
            auto lay = SettingsLayer::create("HomeScene");
            this->addChild(lay, 5);
        }
    });
    addChild(set_button);

    /*进度*/
    auto progress = Label::createWithTTF("进度:", "fonts/dengxian.ttf", 20);
    progress->setAnchorPoint(Vec2(0, 1));
    progress->setPosition(Vec2(_visibleSize.width * 0.615, _visibleSize.height * 0.94));
    progress->setColor(Color3B::BLACK);
    addChild(progress);
    prog_text = Label::createWithTTF("", "fonts/dengxian.ttf", 20);
    addChild(prog_text);

    /*钱币*/
    auto coin = Label::createWithTTF("钱币:", "fonts/dengxian.ttf", 20);
    coin->setAnchorPoint(Vec2(0, 1));
    coin->setPosition(Vec2(_visibleSize.width * 0.69, _visibleSize.height * 0.94));
    coin->setColor(Color3B::BLACK);
    addChild(coin);
    money_text = Label::createWithTTF("", "fonts/dengxian.ttf", 20);
    money_text->setPosition(Vec2(_visibleSize.width * 0.745, _visibleSize.height * 0.925));
    addChild(money_text);

    backGround = Sprite::create();
    addChild(backGround, -1);
    wordArt = Sprite::create();
    addChild(wordArt);

    personPortrait = Sprite::create();
    personPortrait->setAnchorPoint(Vec2(0, 0));
    personPortrait->setPosition(Vec2(_visibleSize.width * 0.05, 0));
    personPortrait->setScale(1.3);
    addChild(personPortrait);

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
    backGround->setPosition(_visibleSize / 2);

    /*地点艺术字*/
    wordArt->setTexture(location.wordArt);
    wordArt->setAnchorPoint(Vec2(0, 1));
    wordArt->setContentSize(Size(_visibleSize.width * 0.15, _visibleSize.height * 0.077));
    wordArt->setPosition(Vec2(_visibleSize.width * 0.53, _visibleSize.height * 0.955));

    /*具体进度*/
    int cnt = 0;
    char str[10];
    sprintf(str, "%d", location.passedRound);
    cnt = strlen(str);
    str[cnt++] = '/';
    sprintf(str + cnt, "%d", location.totalRound);
    string prog = str;
    prog_text->setPosition(Vec2(_visibleSize.width * 0.67, _visibleSize.height * 0.925));
    prog_text->setColor(Color3B::BLACK);
    prog_text->setString(prog);

    /*具体钱币*/
    sprintf(str, "%ld", gamedata->getMoneyNum());
    prog = str;
    money_text->setColor(Color3B::BLACK);
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
    card = gamedata->getCharacterSpellCardList(people_array[people_order].tag);
    string img;
    double width = 0.041, height = 0.065;
    for (int i = 0; i < 3; i++) {
        if (i < card.size())
            img = card[i].icon;
        else {
            img = "homescene/p2.png";
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
