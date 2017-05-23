#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "RoundSelectScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
#include "ui/CocosGUI.h"
// #include "resources.h.dir/main_menu.h"
#include <string>

using namespace ui;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string RoundSelectScene::TAG{ "RoundSelectScene" };

RoundSelectScene::RoundSelectScene()
{
    _visibleSize = _director->getVisibleSize();
}

bool
RoundSelectScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("RoundSelectScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    /*  3. backButton */
    auto backButton = Button::create("", "", "");
    backButton->setScale(1.8);
    backButton->setTitleFontName("微软雅黑");
    backButton->setTitleFontSize(14);
    backButton->setTitleText("back");
    backButton->setPosition(Vec2(_visibleSize.width * 0.5, _visibleSize.height * 0.5));
    backButton->addTouchEventListener(
        [=](Ref* pSender, Widget::TouchEventType type) { Director::getInstance()->popScene(); });
    this->addChild(backButton);

    /*  4. schedule */

    this->scheduleUpdate();

    return true;
}

void
RoundSelectScene::update(float dt)
{
}
