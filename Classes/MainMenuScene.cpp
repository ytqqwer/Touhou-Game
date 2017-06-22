#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "MainMenuScene.h"
#include "GameData.h"
#include "HomeScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
#include "RoundSelectScene.h"
#include "SaveScene.h"
#include "SettingsLayer.h"
#include "ui/CocosGUI.h"

using namespace ui;

// #include "resources.h.dir/main_menu.h"
#include <string>

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string MainMenuScene::TAG{ "MainMenuScene" };

MainMenuScene::MainMenuScene()
{
    gamedata = GameData::getInstance();
    _visibleSize = _director->getVisibleSize();
}

bool
MainMenuScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("MainMenuScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    /*  3. PlaceHolder

    auto p = PlaceHolder::createCircle(100, "MainMenuScene");
    p->setPosition(_visibleSize / 2);
    this->addChild(p);

    /*  4. schedule */

    // this->scheduleUpdate();

    /*新游戏*/
    auto NGButton = Button::create("", "", "");
    NGButton->setTitleText("新游戏");
    NGButton->setTitleFontName("fonts/dengxian.ttf");
    NGButton->setTitleColor(Color3B(194, 134, 11));
    NGButton->setTitleFontSize(50);
    NGButton->setAnchorPoint(Vec2(0, 0));
    NGButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.57));
    NGButton->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        gamedata = GameData::getInstance();
        auto canNew = gamedata->newGame();

        //有不能创建新存档的BUG，暂时注释掉
        // if (canNew) {
        //    Director::getInstance()->pushScene(HomeScene::create());
        //}

        Director::getInstance()->pushScene(HomeScene::create());
    });
    addChild(NGButton);

    /*载入游戏*/
    auto LGButton = Button::create("", "", "");
    LGButton->setTitleText("载入游戏");
    LGButton->setTitleFontName("fonts/dengxian.ttf");
    LGButton->setTitleColor(Color3B(194, 134, 11));
    LGButton->setTitleFontSize(50);
    LGButton->setAnchorPoint(Vec2(0, 0));
    LGButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.43));
    LGButton->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        Director::getInstance()->pushScene(SaveScene::create());
    });
    addChild(LGButton);

    /*设置*/
    auto SGButton = Button::create("", "", "");
    SGButton->setTitleText("设置");
    SGButton->setTitleFontName("fonts/dengxian.ttf");
    SGButton->setTitleColor(Color3B(194, 134, 11));
    SGButton->setTitleFontSize(30);
    SGButton->setAnchorPoint(Vec2(0, 0));
    SGButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.34));
    SGButton->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            auto lay = SettingsLayer::create("MainMenuScene");
            this->addChild(lay, 5);
        }
    });

    addChild(SGButton);
    /*职员表*/
    auto ZGButton = Button::create("", "", "");
    ZGButton->setTitleText("职员表");
    ZGButton->setTitleFontName("fonts/dengxian.ttf");
    ZGButton->setTitleColor(Color3B(194, 134, 11));
    ZGButton->setTitleFontSize(30);
    ZGButton->setAnchorPoint(Vec2(0, 0));
    ZGButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.27));
    addChild(ZGButton);
    /*退出游戏*/
    auto ret = Button::create("", "", "");
    ret->setTitleText("退出");
    ret->setTitleFontName("fonts/dengxian.ttf");
    ret->setTitleColor(Color3B(184, 134, 11));
    ret->setTitleFontSize(25);
    ret->setAnchorPoint(Vec2(0, 0));
    ret->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.2));

    ret->addTouchEventListener(
        [](Ref* pSender, Widget::TouchEventType type) { Director::getInstance()->end(); });
    addChild(ret);

    return true;
}

void
MainMenuScene::onEnter()
{
    Scene::onEnter();

    //每次进入该场景时都会创建一个背景图片，等待以后改进
    // auto loc = gamedata->getCurrentLocation();
    /*背景*/
    // auto bg = Sprite::create(loc.backgroundPicture);
    auto bg = Sprite::create("mainmenuscene/begin.png");
    // bg->setContentSize(_visibleSize);
    bg->setScale(1.35);
    bg->setPosition(_visibleSize / 2);
    addChild(bg, -1);

    auto move = MoveBy::create(40, Vec2(0, _visibleSize.height / 4));
    auto move_back = move->reverse();
    auto seq = Sequence::create(move, move_back, nullptr);
    bg->runAction(RepeatForever::create(seq));
}
void
MainMenuScene::update(float dt)
{
}
