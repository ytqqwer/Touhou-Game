#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/InventoryScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
// #include "resources.h.dir/inventory.h"

#include "AudioController.h"

#include "ui/CocosGUI.h"
using namespace ui;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string InventoryScene::TAG{ "InventoryScene" };

InventoryScene::InventoryScene()
{
    gamedata = GameData::getInstance();
    _visibleSize = _director->getVisibleSize();
    currentType = Item::Type::NORMAL;
}

bool
InventoryScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("InventoryScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    /*background*/
    auto backGround = Sprite::create("background/blue_moon.png");
    backGround->setContentSize(Size(_visibleSize.width, _visibleSize.height));
    backGround->setPosition(Vec2(_visibleSize.width * 0.5, _visibleSize.height * 0.5));
    addChild(backGround, 0);
    /*background2*/
    auto bg_2 =
        PlaceHolder::createRect(Size(_visibleSize.width * 0.859, _visibleSize.height * 0.78), "",
                                16, Color3B(91, 155, 213));
    bg_2->setPosition(_visibleSize / 2);
    addChild(bg_2, 0);
    /*background3*/
    auto bg_3 = PlaceHolder::createRect(
        Size(_visibleSize.width * 0.595, _visibleSize.height * 0.707), "", 16, Color3B::WHITE);
    bg_3->setAnchorPoint(Vec2(0, 0));
    bg_3->setPosition(Vec2(_visibleSize.width * 0.29, _visibleSize.height * 0.141));
    addChild(bg_3, 0);

    /*返回按钮*/
    auto backButton = Button::create("menu/p1.png", "", "");
    backButton->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    backButton->setTitleText("返回");
    backButton->setTitleFontSize(20);
    backButton->setTitleColor(Color3B::WHITE);
    backButton->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playReturnButtonEffect();
            Director::getInstance()->popScene();
        }
    });
    addChild(backButton, 0);

    /*普通道具*/
    auto normalProps = Button::create("menu/white.png", "", "");
    normalProps->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.75));
    normalProps->setTitleText("普通道具");
    normalProps->setTitleFontSize(20);
    normalProps->setTitleColor(Color3B(91, 155, 213));
    normalProps->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            currentType = Item::Type::NORMAL;
            itemTable->reloadData();
        }
    });
    addChild(normalProps, 0);

    /*强化道具*/
    auto strongProps = Button::create("menu/white.png", "", "");
    strongProps->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.6));
    strongProps->setTitleText("强化道具");
    strongProps->setTitleFontSize(20);
    strongProps->setTitleColor(Color3B(91, 155, 213));
    strongProps->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            currentType = Item::Type::STRENGTHEN;
            itemTable->reloadData();
        }
    });
    addChild(strongProps, 0);

    /*特殊道具*/
    auto specialProps = Button::create("menu/white.png", "", "");
    specialProps->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.45));
    specialProps->setTitleText("特殊道具");
    specialProps->setTitleFontSize(20);
    specialProps->setTitleColor(Color3B(91, 155, 213));
    specialProps->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            currentType = Item::Type::SPECIAL;
            itemTable->reloadData();
        }
    });
    addChild(specialProps, 0);

    /*道具列表控件*/
    itemTable = TableView::create(this, Size(750, 500));
    itemTable->setPosition(Vec2(20, 0));
    itemTable->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL); //纵向
    itemTable->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN); //从小到大排列
    itemTable->setDelegate(this);                                            //委托代理
    bg_3->addChild(itemTable);

    /*  init particle touch listener */

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
InventoryScene::onEnter()
{
    Scene::onEnter();

    /*加载道具列表*/
    normalItems = gamedata->getAvailableNormalItems();
    strengthenItems = gamedata->getAvailableStrengthenItems();
    specialItems = gamedata->getAvailableSpecialItems();

    currentType = Item::Type::NORMAL;
    itemTable->reloadData(); //加载数据
}

void
InventoryScene::onExit()
{
    Scene::onExit();
}

Size
InventoryScene::tableCellSizeForIndex(TableView* table, ssize_t idx)
{
    return Size(750, 100);
}

// Size
// InventoryScene::cellSizeForTable(TableView * table)
//{
//	return Size(750,100);
//}

TableViewCell*
InventoryScene::tableCellAtIndex(TableView* table, ssize_t idx)
{
    TableViewCell* cell = table->dequeueCell();
    if (!cell) {
        cell = new TableViewCell();
        cell->autorelease();
    } else {
        cell->removeAllChildren();
    }

    vector<Item> currentItems;
    if (currentType == Item::Type::NORMAL)
        currentItems = normalItems;
    else if (currentType == Item::Type::STRENGTHEN)
        currentItems = strengthenItems;
    else if (currentType == Item::Type::SPECIAL)
        currentItems = specialItems;

    if (currentItems.size() > 0) {
        cell->setName(currentItems[idx].tag);

        auto box = PlaceHolder::createRect(Size(720, 90), "", 16, Color3B(91, 155, 213));
        box->setPosition(Vec2(0, 50));
        box->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
        cell->addChild(box);

        auto icon = Sprite::create(currentItems[idx].icon);
        icon->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        icon->setPosition(Vec2(30, 50));
        cell->addChild(icon);

        auto name = Label::create(currentItems[idx].name, "fonts/dengxian.ttf", 20);
        name->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        name->setPosition(Vec2(130, 50));
        name->setColor(Color3B::BLACK);
        cell->addChild(name);

        auto description = Label::create(currentItems[idx].description, "fonts/dengxian.ttf", 20);
        description->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        description->setPosition(Vec2(370, 50));
        description->setColor(Color3B::BLACK);
        cell->addChild(description);

        auto characters = gamedata->getAvailableCharacterList();
        string userName = "无";
        bool _canBreak = false;
        for (int k = 0; k < characters.size(); k++) {
            vector<Item> allItems = gamedata->getCharacterEquipedItems(characters[k].tag);
            for (int j = 0; j < allItems.size(); j++) {
                if (currentItems[idx].tag == allItems[j].tag) {
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
        userLabel->setPosition(Vec2(620, 60));
        userLabel->setColor(Color3B::BLACK);
        cell->addChild(userLabel);

        auto status = Label::create(userName, "fonts/dengxian.ttf", 20);
        status->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        status->setPosition(Vec2(620, 40));
        status->setColor(Color3B::BLACK);
        status->setName("status");
        cell->addChild(status);
    }
    return cell;
}

ssize_t
InventoryScene::numberOfCellsInTableView(TableView* table)
{
    if (currentType == Item::Type::NORMAL)
        return normalItems.size();
    else if (currentType == Item::Type::STRENGTHEN)
        return strengthenItems.size();
    else if (currentType == Item::Type::SPECIAL)
        return specialItems.size();
}

void
InventoryScene::tableCellTouched(TableView* table, TableViewCell* cell)
{
    if (currentType == Item::Type::STRENGTHEN) {

        std::function<void(std::string)> func = std::bind(&GameData::equipStrengthenItem, gamedata,
                                                          cell->getName(), std::placeholders::_1);
        std::function<void(std::string)> callBack = [cell](std::string characterTag) {
            auto status = (Label*)cell->getChildByName("status");
            auto character = GameData::getInstance()->getCharacterByTag(characterTag);
            status->setString(character.name);
        };
        this->addChild(InventoryScene::characterMenu::create(func, callBack));
    }
}

InventoryScene::characterMenu*
InventoryScene::characterMenu::create(std::function<void(std::string)> func,
                                      std::function<void(std::string)> callBack)
{
    characterMenu* pRet = new (std::nothrow) characterMenu();
    if (pRet && pRet->init(func, callBack)) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool
InventoryScene::characterMenu::init(std::function<void(std::string)> func,
                                    std::function<void(std::string)> callBack)
{
    if (!Layer::init()) {
        return false;
    }

    _visibleSize = _director->getVisibleSize();

    //触摸截断
    this->setLocalZOrder(8888);
    this->setTouchEnabled(true);
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* t, Event* e) { return true; };
    _director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    /* 构建界面背景 */
    auto layout = Layout::create();
    layout->setContentSize(Size(_visibleSize.width / 2.0, _visibleSize.height / 2.0));
    layout->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    layout->setBackGroundColor(Color3B(255, 255, 255));
    layout->setBackGroundColorOpacity(90);
    layout->setAnchorPoint(Vec2(0.5, 0.5));
    layout->setPosition(Vec2(_visibleSize.width / 2.0, _visibleSize.height / 2.0));

    /* 按钮组*/
    auto layoutSize = layout->getSize();

    auto characters = GameData::getInstance()->getAvailableCharacterList();
    int index = 0;
    for (auto const& c : characters) {
        auto selectButton = Button::create();
        auto characterTag = c.tag;
        selectButton->setTitleText(characterTag);
        selectButton->setTitleFontSize(36);
        selectButton->loadTextures("menu/buttonNormal.png", "menu/buttonPressed.png");
        selectButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        selectButton->setScale(0.7);
        selectButton->setPosition(
            Vec2(layoutSize.width * (1.0 + index) / 4.0, layoutSize.height * 3.0 / 4.0));
        selectButton->addTouchEventListener(
            [this, func, characterTag, callBack](Ref* pSender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {
                    AudioController::getInstance()->playClickButtonEffect();
                    func(characterTag);
                    callBack(characterTag);
                    this->removeFromParentAndCleanup(true);
                }
            });
        layout->addChild(selectButton);
        index++;
    }

    auto cancelButton = Button::create();
    cancelButton->setTitleText("取消");
    cancelButton->setTitleFontSize(36);
    cancelButton->loadTextures("menu/buttonNormal.png", "menu/buttonPressed.png");
    cancelButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    cancelButton->setScale(0.7);
    cancelButton->setPosition(Vec2(layoutSize.width / 2.0, layoutSize.height / 4.0));
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
