#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/EquipScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
// #include "resources.h.dir/equip.h"

#include "AudioController.h"

#include "ui/CocosGUI.h"
using namespace ui;

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
    auto bg_1 = Sprite::create("background/equip_scene.png");
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
    auto ret = Button::create("menu/p1.png");
    ret->setTitleText("返回");
    ret->setTitleColor(Color3B::WHITE);
    ret->setTitleFontSize(20);
    ret->setAnchorPoint(Vec2(0, 1));
    ret->setPosition(Vec2(_visibleSize.width * 0.095, _visibleSize.height * 0.265));
    ret->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playReturnButtonEffect();
            Director::getInstance()->popScene();
        }
    });
    ret->setScale9Enabled(true);
    ret->setSize(Size(_visibleSize.width * 0.15, _visibleSize.height * 0.1));
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
        text[i] = Label::create(content[i], "fonts/dengxian.ttf", 20);
        text[i]->setPosition(Vec2(_visibleSize.width * 0.4, _visibleSize.height * height));
        text[i]->setColor(Color3B::BLACK);
        addChild(text[i]);
        height -= 0.07;
    }

    /*攻击方式切换箭头*/
    auto deco_1 = Sprite::create("menu/switch_arrow.png");
    deco_1->setContentSize(Size(_visibleSize.width * 0.04, _visibleSize.height * 0.055));
    deco_1->setPosition(
        Vec2(box_4->getContentSize().width * 0.9, box_4->getContentSize().height * 0.5));
    box_4->addChild(deco_1);

    auto deco_2 = Sprite::create("menu/switch_arrow.png");
    deco_2->setContentSize(Size(_visibleSize.width * 0.04, _visibleSize.height * 0.055));
    deco_2->setPosition(
        Vec2(box_4->getContentSize().width * 0.9, box_3->getContentSize().height * 0.162));
    box_3->addChild(deco_2);

    /*攻击方式*/
    auto attack = Label::create("攻击方式", "fonts/dengxian.ttf", 20);
    attack->setPosition(
        Vec2(box_3->getContentSize().width / 2, box_3->getContentSize().height * 0.85));
    attack->setColor(Color3B::BLACK);
    box_3->addChild(attack);

    /*  init particle touch listener */

    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = [this](const std::vector<Touch*>& touches, Event* event) { return; };
    listener->onTouchesEnded = [this](const std::vector<Touch*>& touches, Event* event) {
        auto touch = touches[0];
        auto _emitter = ParticleFlower::createWithTotalParticles(15);
        _emitter->setTexture(
            Director::getInstance()->getTextureCache()->addImage("particle/stars.png"));
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
        this->addChild(button);
        SCButton.pushBack(button);
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

    auto standAnimation = Animation::create();
    for (auto v : character.standFrame) {
        standAnimation->addSpriteFrameWithFile(v);
    }
    standAnimation->setDelayPerUnit(character.standFrameDelay);
    auto portrait = Sprite::create();
    portrait->setPosition(Vec2(_visibleSize.width * 0.32, _visibleSize.height * 0.74));
    portrait->runAction(RepeatForever::create(Animate::create(standAnimation)));
    addChild(portrait, 1, ++tagSum);

    /*生命*/
    auto text_1 = Label::create(getNumber(character.healthPointBase, 0), "fonts/dengxian.ttf",
                                18); //属性加成暂时使用0代替
    text_1->setPosition(Vec2(_visibleSize.width * 0.47, _visibleSize.height * 0.81));
    text_1->setColor(Color3B::BLACK);
    addChild(text_1, 0, ++tagSum);
    /*灵力*/
    auto text_2 = Label::create(getNumber(character.manaBase, 0), "fonts/dengxian.ttf", 18);
    text_2->setPosition(Vec2(_visibleSize.width * 0.47, _visibleSize.height * 0.74));
    text_2->setColor(Color3B::BLACK);
    addChild(text_2, 0, ++tagSum);
    /*速度*/
    auto text_3 = Label::create(getNumber(character.walkSpeedBase, 0), "fonts/dengxian.ttf", 18);
    text_3->setPosition(Vec2(_visibleSize.width * 0.47, _visibleSize.height * 0.67));
    text_3->setColor(Color3B::BLACK);
    addChild(text_3, 0, ++tagSum);

    /*攻击方式*/
    auto attackList = gamedata->getSelectedAttackList(character.tag);
    double high = 0.73;
    for (int i = 0; i < attackList.size(); i++) {
        Label* attack_text;
        if (attackList[i].type == Character::Attack::Type::FOCUS)
            attack_text =
                Label::create("集中  " + attackList[i].description, "fonts/dengxian.ttf", 20);
        else
            attack_text =
                Label::create("发散  " + attackList[i].description, "fonts/dengxian.ttf", 20);
        attack_text->setAnchorPoint(Vec2(0, 0));
        attack_text->setPosition(Vec2(_visibleSize.width * 0.6, _visibleSize.height * high));
        attack_text->setColor(Color3B::BLACK);
        addChild(attack_text, 1, ++tagSum);
        high -= 0.073;
    }

    /*符卡*/
    auto equipedSpellCards = gamedata->getCharacterEquipedSpellCardsInPair(character.tag);
    for (int i = 0; i < character.spellCardSlotNum; i++) {
        Button* button = Button::create();
        button->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

        button->isIgnoreContentAdaptWithSize();
        button->setScale9Enabled(true);
        button->setContentSize(Size(380, 100));
        auto buttonSize = Size(380, 100);

        bool found = false;
        for (auto const& pair : equipedSpellCards) {
            if (pair.second == i) {
                Sprite* icon = Sprite::create(pair.first.icon);
                icon->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                icon->setPosition(buttonSize.width / 6.0, buttonSize.height / 2.0);
                button->addChild(icon);

                string manaCost;
                string cooldownTime;
                stringstream ss;
                ss << pair.first.manaCost;
                ss >> manaCost;
                ss.clear();
                ss << pair.first.cooldown;
                ss >> cooldownTime;
                auto text = Label::create(pair.first.name + "\n" + "消耗灵力:" + manaCost + "\n" +
                                              "冷却时间:" + cooldownTime + "s",
                                          "fonts/dengxian.ttf", 20);
                text->setAlignment(TextHAlignment::CENTER);
                text->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                text->setPosition(Vec2(buttonSize.width * 3.0 / 5.0, buttonSize.height / 2.0));
                button->addChild(text);

                found = true;
                break;
            }
        }
        if (!found) {
            auto none = Label::create("Select", "fonts/dengxian.ttf", 20);
            none->setColor(Color3B::WHITE);
            none->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            none->setPosition(buttonSize.width / 4.0, buttonSize.height / 2.0);
            button->addChild(none);
        }

        std::function<void(std::string)> func =
            std::bind(&GameData::equipSpellCard, gamedata, character.tag, std::placeholders::_1, i);
        std::function<void()> callBack =
            std::bind(&EquipScene::loadCharacterProperty, this, character);
        button->addTouchEventListener(
            [this, func, callBack](Ref* pSender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {
                    AudioController::getInstance()->playClickButtonEffect();
                    this->addChild(EquipScene::SpellCardMenu::create(func, callBack));
                }
            });

        button->setPosition(
            Vec2(_visibleSize.width * 0.280, _visibleSize.height * (0.46 - i * 0.15)));
        this->addChild(button, 1, ++tagSum);
    }

    /*道具*/
    auto activeItems = gamedata->getCharacterActiveItemsInPair(character.tag);
    for (int i = 0; i < character.itemSlotNum; i++) { // i代表当前槽位
        Button* button = Button::create();
        button->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

        //忽视纹理大小缩放，使得按钮上的非纹理位置也可以点击
        //也只有这样setContentSize才会起作用
        button->isIgnoreContentAdaptWithSize();
        button->setScale9Enabled(true);
        button->setContentSize(Size(380, 100));
        auto buttonSize = Size(380, 100);

        bool found = false;
        for (auto const& pair : activeItems) {
            if (pair.second == i) {

                Sprite* icon = Sprite::create(pair.first.icon);
                icon->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                icon->setPosition(buttonSize.width / 6.0, buttonSize.height / 2.0);
                button->addChild(icon);

                string maxUseCounts;
                string cooldownTime;
                stringstream ss;
                ss << pair.first.maxUseCount;
                ss >> maxUseCounts;
                ss.clear();
                ss << pair.first.cooldown;
                ss >> cooldownTime;
                auto text =
                    Label::create(pair.first.name + "\n" + pair.first.description + "\n使用" +
                                      maxUseCounts + "次\n" + "冷却时间:" + cooldownTime + "s",
                                  "fonts/dengxian.ttf", 20);
                text->setAlignment(TextHAlignment::CENTER);
                text->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                text->setPosition(Vec2(buttonSize.width * 3.0 / 5.0, buttonSize.height / 2.0));
                button->addChild(text);

                found = true;
                break;
            }
        }
        if (!found) {
            auto none = Label::create("Select", "fonts/dengxian.ttf", 20);
            none->setColor(Color3B::WHITE);
            none->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            none->setPosition(buttonSize.width / 4.0, buttonSize.height / 2.0);
            button->addChild(none);
        }

        std::function<void(std::string)> func = std::bind(&GameData::equipActiveItem, gamedata,
                                                          character.tag, std::placeholders::_1, i);
        std::function<void()> callBack =
            std::bind(&EquipScene::loadCharacterProperty, this, character);
        button->addTouchEventListener(
            [this, func, callBack](Ref* pSender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {
                    AudioController::getInstance()->playClickButtonEffect();
                    this->addChild(EquipScene::ItemMenu::create(func, callBack));
                }
            });

        button->setPosition(
            Vec2(_visibleSize.width * 0.580, _visibleSize.height * (0.46 - i * 0.15)));
        this->addChild(button, 1, ++tagSum);
    }
}

void
EquipScene::onExit()
{
    Scene::onExit();

    for (auto& button : SCButton) {
        button->removeFromParentAndCleanup(true);
    }
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
            AudioController::getInstance()->playClickButtonEffect();
            auto equipScene = (EquipScene*)parent;
            equipScene->loadCharacterProperty(character);
        }
    });

    return true;
}

//////////////////////////////////////////////
//以下是菜单项

EquipScene::ItemMenu::ItemMenu(std::function<void(std::string)> func,
                               std::function<void()> callBack)
{
    _visibleSize = Director::getInstance()->getVisibleSize();
    items = GameData::getInstance()->getAvailableActiveItems();
    this->func = func;
    this->callBack = callBack;
}

EquipScene::ItemMenu*
EquipScene::ItemMenu::create(std::function<void(std::string)> func, std::function<void()> callBack)
{
    ItemMenu* pRet = new (std::nothrow) ItemMenu(func, callBack);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool
EquipScene::ItemMenu::init()
{
    if (!Layer::init()) {
        return false;
    }

    //触摸截断
    this->setLocalZOrder(8888);
    this->setTouchEnabled(true);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* t, Event* e) { return true; };
    _director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    /* 构建界面背景 */
    auto layout = Layout::create();
    layout->setContentSize(Size(_visibleSize.width / 3.0, _visibleSize.height));
    layout->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    layout->setBackGroundColor(Color3B(255, 255, 255));
    layout->setBackGroundColorOpacity(50);
    layout->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
    layout->setPosition(Vec2(_visibleSize.width, 0));

    /*道具列表控件*/
    itemTable = TableView::create(this, Size(300, 600));
    itemTable->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    itemTable->setPosition(Vec2(65, 120)); //上移一些位置
    itemTable->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL); //纵向
    itemTable->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN); //从小到大排列
    itemTable->setDelegate(this);                                            //委托代理
    layout->addChild(itemTable);
    itemTable->reloadData();

    /* 按钮 */
    auto cancelButton = Button::create();
    cancelButton->setTitleText("取消");
    cancelButton->setTitleFontSize(36);
    cancelButton->loadTextures("menu/buttonNormal.png", "menu/buttonPressed.png");
    cancelButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    cancelButton->setPosition(Vec2(layout->getSize().width / 2.0, layout->getSize().height / 10.0));
    cancelButton->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playReturnButtonEffect();
            this->removeFromParentAndCleanup(true);
        }
    });
    layout->addChild(cancelButton);

    addChild(layout);

    return true;
}

Size
EquipScene::ItemMenu::tableCellSizeForIndex(TableView* table, ssize_t idx)
{
    return Size(300, 100);
}

TableViewCell*
EquipScene::ItemMenu::tableCellAtIndex(TableView* table, ssize_t idx)
{
    TableViewCell* cell = table->dequeueCell();
    if (!cell) {
        cell = new TableViewCell();
        cell->autorelease();
    } else {
        cell->removeAllChildren();
    }

    if (items.size() > 0) {
        cell->setName(items[idx].tag);

        auto box = PlaceHolder::createRect(Size(300, 90), "", 16, Color3B(91, 155, 213));
        box->setPosition(Vec2(0, 50));
        box->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
        cell->addChild(box);

        auto icon = Sprite::create(items[idx].icon);
        icon->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        icon->setPosition(Vec2(30, 50));
        cell->addChild(icon);

        auto name = Label::create(items[idx].name, "fonts/dengxian.ttf", 20);
        name->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        name->setPosition(Vec2(120, 50));
        name->setColor(Color3B::BLACK);
        cell->addChild(name);

        auto characters = GameData::getInstance()->getAvailableCharacterList();
        string userName = "无";
        bool _canBreak = false;
        for (int k = 0; k < characters.size(); k++) {
            vector<Item> allItems =
                GameData::getInstance()->getCharacterEquipedItems(characters[k].tag);
            for (int j = 0; j < allItems.size(); j++) {
                if (items[idx].tag == allItems[j].tag) {
                    userName = characters[k].name;
                    _canBreak = true;
                    break;
                }
            }
            if (_canBreak) {
                break;
            }
        }
        auto userLabel = Label::create("使用者: ", "fonts/dengxian.ttf", 20);
        userLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        userLabel->setPosition(Vec2(210, 60));
        userLabel->setColor(Color3B::BLACK);
        cell->addChild(userLabel);

        auto status = Label::create(userName, "fonts/dengxian.ttf", 20);
        status->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        status->setPosition(Vec2(210, 40));
        status->setColor(Color3B::BLACK);
        status->setName("status");
        cell->addChild(status);
    }
    return cell;
}

ssize_t
EquipScene::ItemMenu::numberOfCellsInTableView(TableView* table)
{
    return items.size();
}

void
EquipScene::ItemMenu::tableCellTouched(TableView* table, TableViewCell* cell)
{
    AudioController::getInstance()->playClickButtonEffect();
    func(cell->getName());
    callBack();

    table->reloadData();
}

///////////////////////////////////////////////////////////////////
//上下两个类可以很简单的合并成为一个
//然而我把它们写成了两个，因为复制粘贴很方便

EquipScene::SpellCardMenu::SpellCardMenu(std::function<void(std::string)> func,
                                         std::function<void()> callBack)
{
    _visibleSize = Director::getInstance()->getVisibleSize();
    spellCards = GameData::getInstance()->getAvailableSpellCards();
    this->func = func;
    this->callBack = callBack;
}

EquipScene::SpellCardMenu*
EquipScene::SpellCardMenu::create(std::function<void(std::string)> func,
                                  std::function<void()> callBack)
{
    SpellCardMenu* pRet = new (std::nothrow) SpellCardMenu(func, callBack);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool
EquipScene::SpellCardMenu::init()
{
    if (!Layer::init()) {
        return false;
    }

    //触摸截断
    this->setLocalZOrder(8888);
    this->setTouchEnabled(true);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* t, Event* e) { return true; };
    _director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    /* 构建界面背景 */
    auto layout = Layout::create();
    layout->setContentSize(Size(_visibleSize.width / 3.0, _visibleSize.height));
    layout->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    layout->setBackGroundColor(Color3B(255, 255, 255));
    layout->setBackGroundColorOpacity(50);
    layout->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
    layout->setPosition(Vec2(_visibleSize.width, 0));

    /*道具列表控件*/
    spellCardTable = TableView::create(this, Size(300, 600));
    spellCardTable->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    spellCardTable->setPosition(Vec2(65, 120));
    spellCardTable->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL); //纵向
    spellCardTable->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN); //从小到大排列
    spellCardTable->setDelegate(this);                                            //委托代理
    layout->addChild(spellCardTable);
    spellCardTable->reloadData();

    /* 按钮 */
    auto cancelButton = Button::create();
    cancelButton->setTitleText("取消");
    cancelButton->setTitleFontSize(36);
    cancelButton->loadTextures("menu/buttonNormal.png", "menu/buttonPressed.png");
    cancelButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    cancelButton->setPosition(Vec2(layout->getSize().width / 2.0, layout->getSize().height / 10.0));
    cancelButton->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playReturnButtonEffect();
            this->removeFromParentAndCleanup(true);
        }
    });
    layout->addChild(cancelButton);

    addChild(layout);

    return true;
}

Size
EquipScene::SpellCardMenu::tableCellSizeForIndex(TableView* table, ssize_t idx)
{
    return Size(300, 100);
}

TableViewCell*
EquipScene::SpellCardMenu::tableCellAtIndex(TableView* table, ssize_t idx)
{
    TableViewCell* cell = table->dequeueCell();
    if (!cell) {
        cell = new TableViewCell();
        cell->autorelease();
    } else {
        cell->removeAllChildren();
    }

    if (spellCards.size() > 0) {
        cell->setName(spellCards[idx].tag);

        auto box = PlaceHolder::createRect(Size(300, 90), "", 16, Color3B(91, 155, 213));
        box->setPosition(Vec2(0, 50));
        box->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
        cell->addChild(box);

        auto icon = Sprite::create(spellCards[idx].icon);
        icon->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        icon->setPosition(Vec2(30, 50));
        cell->addChild(icon);

        auto name = Label::create(spellCards[idx].name, "fonts/dengxian.ttf", 20);
        name->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        name->setPosition(Vec2(120, 50));
        name->setColor(Color3B::BLACK);
        cell->addChild(name);

        auto characters = GameData::getInstance()->getAvailableCharacterList();
        string userName = "无";
        bool _canBreak = false;
        for (int k = 0; k < characters.size(); k++) {
            vector<SpellCard> cards =
                GameData::getInstance()->getCharacterEquipedSpellCards(characters[k].tag);
            for (int j = 0; j < cards.size(); j++) {
                if (spellCards[idx].tag == cards[j].tag) {
                    userName = characters[k].name;
                    _canBreak = true;
                    break;
                }
            }
            if (_canBreak) {
                break;
            }
        }
        auto userLabel = Label::create("使用者: ", "fonts/dengxian.ttf", 20);
        userLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        userLabel->setPosition(Vec2(210, 60));
        userLabel->setColor(Color3B::BLACK);
        cell->addChild(userLabel);

        auto status = Label::create(userName, "fonts/dengxian.ttf", 20);
        status->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        status->setPosition(Vec2(210, 40));
        status->setColor(Color3B::BLACK);
        status->setName("status");
        cell->addChild(status);
    }
    return cell;
}

ssize_t
EquipScene::SpellCardMenu::numberOfCellsInTableView(TableView* table)
{
    return spellCards.size();
}

void
EquipScene::SpellCardMenu::tableCellTouched(TableView* table, TableViewCell* cell)
{
    AudioController::getInstance()->playClickButtonEffect();
    func(cell->getName());
    callBack();

    table->reloadData();
}
