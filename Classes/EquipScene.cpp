#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "EquipScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
#include "ui/CocosGUI.h"
// #include "resources.h.dir/equip.h"
#include <string>
using namespace ui;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string EquipScene::TAG{ "EquipScene" };

EquipScene::EquipScene()
{
    _visibleSize = _director->getVisibleSize();
    gamedata = GameData::getInstance();
}

bool
EquipScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

    /*背景1*/
    auto bg_1 = Sprite::create("equipscene/p1.png");
    bg_1->setContentSize(_visibleSize);
    bg_1->setPosition(_visibleSize / 2);
    addChild(bg_1);

    /*背景2*/
    auto bg_2 =
        PlaceHolder::createRect(Size(_visibleSize.width * 0.859, _visibleSize.height * 0.78), "",
                                16, Color3B(91, 155, 213));
    bg_2->setPosition(_visibleSize / 2);
    addChild(bg_2);

    /*头像列表背景*/
    auto bg_3 = PlaceHolder::createRect(
        Size(_visibleSize.width * 0.16, _visibleSize.height * 0.707), "", 16, Color3B::WHITE);
    bg_3->setAnchorPoint(Vec2(0, 0));
    bg_3->setPosition(Vec2(_visibleSize.width * 0.09, _visibleSize.height * 0.141));
    addChild(bg_3);

    /*返回按钮*/
    auto ret = Button::create("equipscene/p2.png");
    ret->setTitleText("返回");
    ret->setTitleColor(Color3B::WHITE);
    ret->setTitleFontSize(20);
    ret->setAnchorPoint(Vec2(0, 1));
    ret->setPosition(Vec2(_visibleSize.width * 0.095, _visibleSize.height * 0.265));
    ret->setScale9Enabled(true);
    ret->setSize(Size(_visibleSize.width * 0.15, _visibleSize.height * 0.1));
    ret->addTouchEventListener(
        [](Ref* pSender, Widget::TouchEventType type) { Director::getInstance()->popScene(); });
    addChild(ret);

    return true;
}

void
EquipScene::update(float dt)
{
}

void
EquipScene::onEnter()
{
    Scene::onEnter();

    /*人物列表*/
    auto people = gamedata->getAvailableCharacterList();
    double dis = 0.792;
    for (int i = 0; i < people.size(); i++) {
        auto button = ImageView::create(people[i].rectAvatar);
        button->setScale9Enabled(true);
        button->setContentSize(Size(_visibleSize.width * 0.16, _visibleSize.height * 0.113));
        button->setPosition(Vec2(_visibleSize.width * 0.17, _visibleSize.height * dis));
        dis -= 0.14;
        addChild(button);
    }
    dis -= 0.04;
    auto people_list = Label::createWithTTF("（头像列表）", "fonts/dengxian.ttf", 20);
    people_list->setPosition(Vec2(_visibleSize.width * 0.17, _visibleSize.height * dis));
    people_list->setColor(Color3B::BLACK);
    addChild(people_list);
}

void
EquipScene::onExit()
{
    Scene::onExit();

    /*人物列表*/
}
