#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include <string>
#include <vector>

#include "EquipScene.h"
#include "HomeScene.h"
#include "InventoryScene.h"
#include "KoumakanLibraryScene.h"
#include "LocationSelectScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
#include "RoundSelectScene.h"
#include "SimpleAudioEngine.h"

// #include "resources.h.dir/home.h"
#include <string>

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string HomeScene::TAG{ "HomeScene" };

HomeScene::HomeScene()
{
    gamedata = GameData::getInstance();
    _visibleSize = _director->getVisibleSize();
    people_array = gamedata->getAvailableCharacterList();
    sum = order = 0;
    loc = gamedata->getCurrentLocation();
    music = "";
}

bool
HomeScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }
    /* 返回*/
    auto ret = Button::create("", "", "");
    ret->setTitleText("返回");
    ret->setTitleColor(Color3B(0, 0, 0));
    ret->setTitleFontSize(20);
    ret->setAnchorPoint(Vec2(0, 1));
    ret->setPosition(Vec2(0, _visibleSize.height));
    ret->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->popScene();
        }
    });
    addChild(ret, 2);

    /*出发按钮*/
    auto button_start = Button::create("homescene/p1.png");
    button_start->setTitleFontName("fonts/dengxian.ttf");
    button_start->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.705));
    button_start->setTitleText("出发");
    button_start->setTitleFontSize(25);
    button_start->setScale9Enabled(true);
    button_start->setSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.12));

    button_start->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->pushScene(RoundSelectScene::create());
        }
    });
    addChild(button_start);

    /*整备按钮*/
    auto button_equip = Button::create("homescene/p1.png");
    button_equip->setTitleFontName("fonts/dengxian.ttf");
    button_equip->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.525));
    button_equip->setTitleText("整备");
    button_equip->setTitleFontSize(25);
    button_equip->setScale9Enabled(true);
    button_equip->setSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.12));
    button_equip->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED)
            Director::getInstance()->pushScene(EquipScene::create());
    });
    addChild(button_equip);

    /*道具库按钮*/
    auto button_inventory = Button::create("homescene/p1.png");
    button_inventory->setTitleFontName("fonts/dengxian.ttf");
    button_inventory->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.345));
    button_inventory->setTitleText("道具库");
    button_inventory->setTitleFontSize(25);
    button_inventory->setScale9Enabled(true);
    button_inventory->setSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.12));
    button_inventory->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->pushScene(InventoryScene::create());
        }
    });
    addChild(button_inventory);

    /*其他地图按钮*/
    auto button_map = Button::create("homescene/p1.png");
    button_map->setTitleFontName("fonts/dengxian.ttf");
    button_map->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.165));
    button_map->setTitleText("前往其他地图");
    button_map->setTitleFontSize(25);
    button_map->setScale9Enabled(true);
    button_map->setSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.12));
    button_map->setTitleFontName("fonts/dengxian.ttf");
    button_map->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->pushScene(LocationSelectScene::create());
        }
    });
    addChild(button_map);

    /*更换角色*/
    auto role_change = Button::create("homescene/p4.png");
    role_change->setAnchorPoint(Vec2(0, 0));
    role_change->setPosition(Vec2(_visibleSize.width * 0.01, _visibleSize.height * 0.45));
    role_change->setScale9Enabled(true);
    role_change->setSize(Size(_visibleSize.width * 55 / 1280, _visibleSize.height * 108 / 720));
    role_change->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            person[order]->setPosition(Vec2(-_visibleSize.width, 0));
            order++;
            order %= people_array.size();
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
    info_bg->setContentSize(Size(_visibleSize.width * 580 / 1280, _visibleSize.height * 106 / 720));
    info_bg->setOpacity(150);
    addChild(info_bg);

    /*设置按钮*/
    auto set_button = Button::create("homescene/p5.png");
    set_button->setTitleFontName("fonts/dengxian.ttf");
    set_button->setAnchorPoint(Vec2(0, 1));
    set_button->setPosition(Vec2(_visibleSize.width * 0.905, _visibleSize.height * 0.99));
    set_button->setScale9Enabled(true);
    set_button->setSize(Size(_visibleSize.width * 100 / 1280, _visibleSize.height * 100 / 720));
    set_button->setTitleText("设置");
    set_button->setTitleFontSize(20);
    set_button->setScale(0.87);
    set_button->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            auto lay = SettingsLayer::create("HomeScene");
            this->addChild(lay, 5);
        }
    });
    addChild(set_button);

    /*资料库按钮*/
    auto know_button = Button::create("homescene/p5.png");
    know_button->setTitleFontName("fonts/dengxian.ttf");
    know_button->setAnchorPoint(Vec2(0, 1));
    know_button->setPosition(Vec2(_visibleSize.width * 0.84, _visibleSize.height * 0.99));
    know_button->setScale9Enabled(true);
    know_button->setSize(Size(_visibleSize.width * 100 / 1280, _visibleSize.height * 100 / 720));
    know_button->setTitleText("资料库");
    know_button->setTitleFontSize(20);
    know_button->setScale(0.87);
    know_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED)
            Director::getInstance()->pushScene(KoumakanLibraryScene::create());
    });
    addChild(know_button);

    /*系统商店*/
    auto store_button = Button::create("homescene/p5.png");
    store_button->setTitleFontName("fonts/dengxian.ttf");
    store_button->setAnchorPoint(Vec2(0, 1));
    store_button->setPosition(Vec2(_visibleSize.width * 0.774, _visibleSize.height * 0.99));
    store_button->setScale9Enabled(true);
    store_button->setSize(Size(_visibleSize.width * 100 / 1280, _visibleSize.height * 100 / 720));
    store_button->setTitleText("系统商店");
    store_button->setTitleFontSize(20);
    store_button->setScale(0.87);
    addChild(store_button);

    /*进度*/
    auto progress = Label::createWithTTF("进度:", "fonts/dengxian.ttf", 20);
    progress->setAnchorPoint(Vec2(0, 1));
    progress->setPosition(Vec2(_visibleSize.width * 0.615, _visibleSize.height * 0.94));
    progress->setColor(Color3B::BLACK);
    addChild(progress);

    /*钱币*/
    auto coin = Label::createWithTTF("钱币:", "fonts/dengxian.ttf", 20);
    coin->setAnchorPoint(Vec2(0, 1));
    coin->setPosition(Vec2(_visibleSize.width * 0.69, _visibleSize.height * 0.94));
    coin->setColor(Color3B::BLACK);
    addChild(coin);

    bg = Sprite::create();
    addChild(bg, -1);
    word = Sprite::create();
    addChild(word);
    prog_text = Label::createWithTTF("", "fonts/dengxian.ttf", 20);
    addChild(prog_text);
    money_text = Label::createWithTTF("", "fonts/dengxian.ttf", 20);
    addChild(money_text);
    for (int i = 0; i < min(4, (int)people_array.size()); i++) {
        person[i] = ImageView::create(people_array[i].portrait);
        person[i]->setAnchorPoint(Vec2(0, 0));
        person[i]->setPosition(Vec2(-_visibleSize.width, 0));
        person[i]->setScale(508.0 / 1280 * _visibleSize.width / person[0]->getContentSize().width);
        addChild(person[i]);
    }
    return true;
}

void
HomeScene::update(float dt)
{
}
void
HomeScene::onEnter()
{
    Scene::onEnter();

    loc = gamedata->getCurrentLocation();

    /*背景音乐*/
    if (music != loc.backgroundMusic) {
        auto play = SimpleAudioEngine::getInstance();
        play->stopBackgroundMusic();
        play->playBackgroundMusic(loc.backgroundMusic.c_str(), true);
        music = loc.backgroundMusic;
    }

    /*背景*/

    bg->setTexture(loc.backgroundPicture);
    bg->setContentSize(_visibleSize);
    bg->setPosition(_visibleSize / 2);

    /*地点艺术字*/
    word->setTexture(loc.wordArt);
    word->setAnchorPoint(Vec2(0, 1));
    word->setContentSize(Size(_visibleSize.width * 0.15, _visibleSize.height * 0.077));
    word->setPosition(Vec2(_visibleSize.width * 0.53, _visibleSize.height * 0.955));
    word->setColor(Color3B(254, 250, 153));
    word->setOpacity(150);

    /*具体进度*/
    int cnt = 0;
    char str[10];
    sprintf(str, "%d", loc.passedRound);
    cnt = strlen(str);
    str[cnt++] = '/';
    sprintf(str + cnt, "%d", loc.totalRound);
    string prog = str;
    prog_text->setPosition(Vec2(_visibleSize.width * 0.67, _visibleSize.height * 0.925));
    prog_text->setColor(Color3B::BLACK);
    prog_text->setString(prog);

    /*具体钱币*/
    sprintf(str, "%ld", gamedata->getMoneyNum());
    prog = str;
    money_text->setPosition(Vec2(_visibleSize.width * 0.745, _visibleSize.height * 0.925));
    money_text->setColor(Color3B::BLACK);
    money_text->setString(prog);

    /*人物*/

    getPeople();

    /*对话图标*/
    double dis = 0.36;
    auto conversation = gamedata->getConversationIndicatorList(loc.tag);
    for (int i = 0; i < min(3, (int)conversation.size()); i++) {
        auto newTalk = Sprite::create("homescene/p7.png");
        newTalk->setContentSize(
            Size(70.0 / 1280 * _visibleSize.width, 67.2 / 720 * _visibleSize.height));
        newTalk->setOpacity(255);
        newTalk->setPosition(Vec2(_visibleSize.width * (dis - 0.01), _visibleSize.height * 0.17));
        addChild(newTalk, 2, ++sum);

        auto talk_button = Button::create(conversation[i].icon);
        static double scale =
            100.0 / 1280 * _visibleSize.width / talk_button->getContentSize().width;
        talk_button->setAnchorPoint(Vec2(0, 1));
        talk_button->setPosition(Vec2(_visibleSize.width * dis, _visibleSize.height * 0.15));
        talk_button->setScale9Enabled(true);
        talk_button->setScale(scale);
        addChild(talk_button, 1, ++sum);
        talk_button->addTouchEventListener(
            CC_CALLBACK_2(HomeScene::getDialogue, this, conversation[i].conversationTag, newTalk));
        dis += 0.10;
    }
}
void
HomeScene::getPeople()
{
    /*人物*/
    person[order]->setPosition(Vec2(_visibleSize.width * 0.05, 0));

    /*卡片*/
    card = gamedata->getCharacterSpellCardList(people_array[order].tag);
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
        cards[i]->setContentSize(Size(_visibleSize.width * width, _visibleSize.width * height));
    }
}
void
HomeScene::getDialogue(Ref* pSender, Widget::TouchEventType type, string tag, Sprite* talk)
{
    if (type == Widget::TouchEventType::ENDED) {
        talk->setOpacity(0);
    }
}
void
HomeScene::onExit()
{
    for (int i = 1; i <= sum; i++) {
        removeChildByTag(i);
    }
    sum = 0;
    Scene::onExit();
}
