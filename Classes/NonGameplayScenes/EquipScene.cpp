#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/EquipScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
// #include "resources.h.dir/equip.h"

#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
using namespace ui;
using namespace CocosDenshion;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string EquipScene::TAG{ "EquipScene" };

// 本场景编写时存在大量的绝对坐标定位代码，难以维护
// 应该先创建各个区块，再对区块进行排布，以相对定位方式添加相应的元素
// 虽然每个元素都指定了一个唯一tag，方便移除
// 但是不如直接清除各个区块的元素直观
// 各个区块也可以方便地移动位置，而不需要更改子元素的定位

EquipScene::EquipScene()
{
    _visibleSize = _director->getVisibleSize();
    gamedata = GameData::getInstance();
    tagSum = 0;
}

bool
EquipScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

    /*背景*/
    auto bg_1 = Sprite::create("equipscene/p1.png");
    bg_1->setContentSize(_visibleSize);
    bg_1->setPosition(_visibleSize / 2);
    addChild(bg_1);

    /*大框*/
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
    auto ret = Button::create("equipscene/p4.png");
    ret->setTitleText("返回");
    ret->setTitleColor(Color3B::WHITE);
    ret->setTitleFontSize(20);
    ret->setAnchorPoint(Vec2(0, 1));
    ret->setPosition(Vec2(_visibleSize.width * 0.095, _visibleSize.height * 0.265));
    ret->setScale9Enabled(true);
    ret->setSize(Size(_visibleSize.width * 0.15, _visibleSize.height * 0.1));
    ret->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect("back_click.wav");
            Director::getInstance()->popScene();
        }
    });
    addChild(ret);

    /*包围盒*/
    auto box_1 = getRect(0.27, 0.46, 0.0017, 0.002);
    box_1->setAnchorPoint(Vec2(0, 0));
    box_1->setPosition(Vec2(_visibleSize.width * 0.275, _visibleSize.height * 0.141));
    addChild(box_1);

    auto box_2 = getRect(0.3, 0.46, 0.0017, 0.002);
    box_2->setAnchorPoint(Vec2(0, 0));
    box_2->setPosition(Vec2(_visibleSize.width * 0.585, _visibleSize.height * 0.141));
    addChild(box_2);

    auto box_3 = getRect(0.3, 0.22, 0.0017, 0.0027);
    box_3->setAnchorPoint(Vec2(0, 0));
    box_3->setPosition(Vec2(_visibleSize.width * 0.585, _visibleSize.height * 0.63));
    addChild(box_3);

    auto box_4 = getRect(0.3, 0.22 / 3, 0.0017, 0.0027);
    box_4->setPosition(box_3->getContentSize() / 2);
    box_3->addChild(box_4);

    /*属性文字*/
    Label* text[3];
    string content[3] = { "生命", "灵力", "速度" };
    double height = 0.81;
    for (int i = 0; i < 3; i++) {
        text[i] = Label::createWithTTF(content[i], "fonts/dengxian.ttf", 20);
        text[i]->setPosition(Vec2(_visibleSize.width * 0.4, _visibleSize.height * height));
        text[i]->setColor(Color3B::BLACK);
        addChild(text[i]);
        height -= 0.07;
    }

    /*攻击方式切换箭头*/
    auto deco_1 = Sprite::create("equipscene/p2.png");
    deco_1->setContentSize(Size(_visibleSize.width * 0.04, _visibleSize.height * 0.055));
    deco_1->setPosition(
        Vec2(box_4->getContentSize().width * 0.9, box_4->getContentSize().height * 0.5));
    box_4->addChild(deco_1);

    auto deco_2 = Sprite::create("equipscene/p2.png");
    deco_2->setContentSize(Size(_visibleSize.width * 0.04, _visibleSize.height * 0.055));
    deco_2->setPosition(
        Vec2(box_4->getContentSize().width * 0.9, box_3->getContentSize().height * 0.162));
    box_3->addChild(deco_2);

    /*攻击方式*/
    auto attack = Label::createWithTTF("攻击方式", "fonts/dengxian.ttf", 20);
    attack->setPosition(
        Vec2(box_3->getContentSize().width / 2, box_3->getContentSize().height * 0.85));
    attack->setColor(Color3B::BLACK);
    box_3->addChild(attack);

    return true;
}

void
EquipScene::onEnter()
{
    Scene::onEnter();

    /*人物列表*/
    auto characterList = gamedata->getAvailableCharacterList();
    double dis = 0.7925;
    for (int i = 0; i < characterList.size(); i++) {
        auto button = EquipScene::SelectCharacterButton::create(characterList[i], this);
        button->setPosition(Vec2(_visibleSize.width * 0.17, _visibleSize.height * dis));
        dis -= 0.13;
        addChild(button);

        if (i == 0)
            loadCharacterProperty(characterList[0]);
    }
}

void
EquipScene::loadCharacterProperty(const Character& character)
{
    for (int i = 1; i <= tagSum; i++) {
        removeChildByTag(i);
    }
    tagSum = 0;
    auto portrait = Sprite::create(character.portrait);
    portrait->setContentSize(
        Size(_visibleSize.width * 0.08, _visibleSize.width * 0.08 * 600 / 450));
    portrait->setPosition(Vec2(_visibleSize.width * 0.32, _visibleSize.height * 0.74));
    addChild(portrait, 1, ++tagSum);

    /*生命*/
    auto text_1 = Label::createWithTTF(
        getNumber(character.healthPointBase, character.healthPointInc), "fonts/dengxian.ttf", 18);
    text_1->setPosition(Vec2(_visibleSize.width * 0.47, _visibleSize.height * 0.81));
    text_1->setColor(Color3B::BLACK);
    addChild(text_1, 0, ++tagSum);
    /*灵力*/
    auto text_2 = Label::createWithTTF(getNumber(character.manaBase, character.manaInc),
                                       "fonts/dengxian.ttf", 18);
    text_2->setPosition(Vec2(_visibleSize.width * 0.47, _visibleSize.height * 0.74));
    text_2->setColor(Color3B::BLACK);
    addChild(text_2, 0, ++tagSum);
    /*速度*/
    auto text_3 = Label::createWithTTF(getNumber(character.walkSpeedBase, character.walkSpeedInc),
                                       "fonts/dengxian.ttf", 18);
    text_3->setPosition(Vec2(_visibleSize.width * 0.47, _visibleSize.height * 0.67));
    text_3->setColor(Color3B::BLACK);
    addChild(text_3, 0, ++tagSum);

    /*攻击方式*/
    auto attackList = gamedata->getSelectedAttackList(character.tag);
    double high = 0.73;
    for (int i = 0; i < attackList.size(); i++) {
        Label* attack_text;
        if (attackList[i].type == Character::Attack::Type::FOCUS)
            attack_text = Label::createWithTTF("集中  " + attackList[i].description,
                                               "fonts/dengxian.ttf", 20);
        else
            attack_text = Label::createWithTTF("发散  " + attackList[i].description,
                                               "fonts/dengxian.ttf", 20);
        attack_text->setAnchorPoint(Vec2(0, 0));
        attack_text->setPosition(Vec2(_visibleSize.width * 0.6, _visibleSize.height * high));
        attack_text->setColor(Color3B::BLACK);
        addChild(attack_text, 1, ++tagSum);
        high -= 0.073;
    }

    /*符卡*/
    high = 0.51;
    Sprite* card;
    auto cards = gamedata->getCharacterSpellCardList(character.tag);
    for (int i = 0; i < 3; i++) {
        if (i < cards.size()) {
            card = Sprite::create(cards[i].icon);
            card->setContentSize(
                Size(_visibleSize.width * 0.045, _visibleSize.width * 0.045 * 65 / 41));
            char k1[10], k2[10];
            sprintf(k1, "%d", cards[i].manaCost);
            sprintf(k2, "%.1f", cards[i].cooldown);
            auto card_text = Label::create(cards[i].name + "\n" + "消耗灵力:" + k1 + "\n" +
                                               "冷却时间:" + k2 + "s",
                                           "fonts/dengxian.ttf", 20);
            card_text->setPosition(
                Vec2(_visibleSize.width * 0.45, _visibleSize.height * (high + 0.01)));
            card_text->setLineSpacing(10);
            card_text->setAlignment(TextHAlignment::CENTER);
            addChild(card_text, 1, ++tagSum);
        } else {
            card = Sprite::create("equipscene/p3.png");
            card->setContentSize(Size(_visibleSize.width * 0.07, _visibleSize.width * 0.07));
        }
        card->setColor(Color3B::WHITE);
        card->setPosition(Vec2(_visibleSize.width * 0.32, _visibleSize.height * high));
        addChild(card, 1, ++tagSum);
        high -= 0.14;
    }

    /*道具*/
    high = 0.51;
    Sprite* tool;
    auto tools = gamedata->getCharacterItemList(character.tag);
    for (int i = 0; i < 3; i++) {
        if (i < tools.size()) {
            tool = Sprite::create(tools[i].icon);
            tool->setContentSize(
                Size(_visibleSize.width * 0.045, _visibleSize.width * 0.045 * 61 / 43));
            char k1[10], k2[10];
            sprintf(k1, "%d", tools[i].maxUseCount);
            sprintf(k2, "%.1f", tools[i].cooldown);
            auto tool_text = Label::create(tools[i].name + "\n" + tools[i].description + "\n使用" +
                                               k1 + "次\n" + "冷却时间:" + k2 + "s",
                                           "fonts/dengxian.ttf", 20);
            tool_text->setPosition(
                Vec2(_visibleSize.width * 0.76, _visibleSize.height * (high + 0.01)));
            tool_text->setLineSpacing(2);
            tool_text->setAlignment(TextHAlignment::CENTER);
            addChild(tool_text, 1, ++tagSum);
        } else {
            tool = Sprite::create("equipscene/p3.png");
            tool->setContentSize(Size(_visibleSize.width * 0.07, _visibleSize.width * 0.07));
        }
        tool->setColor(Color3B::WHITE);
        tool->setPosition(Vec2(_visibleSize.width * 0.628, _visibleSize.height * high));
        addChild(tool, 1, ++tagSum);
        high -= 0.14;
    }
}

void
EquipScene::onExit()
{
    Scene::onExit();
}

Sprite*
EquipScene::getRect(double width, double height, double k1, double k2)
{

    auto rect_1 = PlaceHolder::createRect(
        Size(_visibleSize.width * width, _visibleSize.height * height), "", 16, Color3B::WHITE);
    auto rect_2 = PlaceHolder::createRect(
        Size(_visibleSize.width * (width - k1), _visibleSize.height * (height - k2)), "", 16,
        Color3B(91, 155, 213));
    rect_2->setPosition(rect_1->getContentSize() / 2);
    rect_1->addChild(rect_2);

    return rect_1;
}

string
EquipScene::getNumber(int k1, int k2)
{
    int len;
    char str[20];
    string value;
    sprintf(str, "%d", k1);
    len = strlen(str);
    str[len] = '+';
    len++;
    sprintf(str + len, "%d", k2);
    value = str;

    return value;
}

EquipScene::SelectCharacterButton*
EquipScene::SelectCharacterButton::create(const Character& character, const Node* parent)
{
    auto pRet = new (std::nothrow) EquipScene::SelectCharacterButton(character, parent);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

EquipScene::SelectCharacterButton::SelectCharacterButton(const Character& character,
                                                         const Node* parent)
    : character(character)
    , parent(parent)
{
}

bool
EquipScene::SelectCharacterButton::init()
{
    if (!Button::init())
        return false;

    loadTextureNormal(character.rectAvatar);
    this->setContentSize(Size(_director->getVisibleSize().width * 0.16,
                              _director->getVisibleSize().width * 0.16 * 64 / 160));
    this->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            SimpleAudioEngine::getInstance()->playEffect("button_click.wav");
            auto equipScene = (EquipScene*)parent;
            equipScene->loadCharacterProperty(character);
        }
    });

    return true;
}
