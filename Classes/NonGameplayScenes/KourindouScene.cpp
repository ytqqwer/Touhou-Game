#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "KourindouScene.h"
#include "EquipScene.h"
#include "InventoryScene.h"
#include "LocationSelectScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
#include "RoundSelectScene.h"
#include "ui/CocosGUI.h"
// #include "resources.h.dir/kourindou.h"
#include "HomeScene.h"
#include "KoumakanLibraryScene.h"
#include "RoundSelectScene.h"
#include "SimpleAudioEngine.h"
#include <string>
#include <vector>
#include "ArmsStorePurchaseScene.h"
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

    /*  4. schedule */

    this->scheduleUpdate();
    /*background*/
    auto bg_1 = Sprite::create("kourindouscene/shop_1.jpg");
    bg_1->setContentSize(_visibleSize);
    bg_1->setPosition(_visibleSize / 2);
    addChild(bg_1);

    /*background2*/
    auto info_bg = Sprite::create("kourindouscene/p3.png");
    info_bg->setAnchorPoint(Vec2(0, 1));
    info_bg->setPosition(Vec2(_visibleSize.width * 0.52, _visibleSize.height));
    info_bg->setContentSize(Size(_visibleSize.width * 580 / 1280, _visibleSize.height * 106 / 720));
    info_bg->setOpacity(150);
    addChild(info_bg);

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

    /*购买按钮*/
    auto button_start = Button::create("kourindouscene/p1.png");
    button_start->setTitleFontName("fonts/dengxian.ttf");
    button_start->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.705));
    button_start->setTitleText("购买");
    button_start->setTitleFontSize(18);
    button_start->setScale9Enabled(true);
    button_start->setSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.12));
    addChild(button_start);
	button_start->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			Director::getInstance()->pushScene(ArmsStorePurchaseScene::create());
		}
	});

    /*整备按钮*/
    auto button_equip = Button::create("kourindouscene/p1.png");
    button_equip->setTitleFontName("fonts/dengxian.ttf");
    button_equip->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.525));
    button_equip->setTitleText("整备");
    button_equip->setTitleFontSize(18);
    button_equip->setScale9Enabled(true);
    button_equip->setSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.12));
    button_equip->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED)
            Director::getInstance()->pushScene(EquipScene::create());
    });
    addChild(button_equip);

    /*道具库按钮*/
    auto button_inventory = Button::create("kourindouscene/p1.png");
    button_inventory->setTitleFontName("fonts/dengxian.ttf");
    button_inventory->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.345));
    button_inventory->setTitleText("道具库");
    button_inventory->setTitleFontSize(18);
    button_inventory->setScale9Enabled(true);
    button_inventory->setSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.12));
    button_inventory->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->pushScene(InventoryScene::create());
        }
    });
    addChild(button_inventory);

    /*其他地图按钮*/
    auto button_map = Button::create("kourindouscene/p1.png");
    button_map->setTitleFontName("fonts/dengxian.ttf");
    button_map->setPosition(Vec2(_visibleSize.width * 0.743, _visibleSize.height * 0.165));
    button_map->setTitleText("前往其他地图");
    button_map->setTitleFontSize(18);
    button_map->setScale9Enabled(true);
    button_map->setSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.12));
    button_map->setTitleFontName("fonts/dengxian.ttf");
    button_map->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->pushScene(LocationSelectScene::create());
        }
    });
    addChild(button_map);

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

    return true;
}

void
KourindouScene::update(float dt)
{
}
