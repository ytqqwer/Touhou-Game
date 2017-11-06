#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/ArmsStorePurchaseScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"

#include "AudioController.h"
#include "Layers/ConfirmButton.h"

#include "ui/CocosGUI.h"
using namespace ui;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string ArmsStorePurchaseScene::TAG{ "ArmsStorePurchaseScene" };

ArmsStorePurchaseScene::ArmsStorePurchaseScene()
{
    gamedata = GameData::getInstance();
    _visibleSize = _director->getVisibleSize();
    currentType = Item::Type::NORMAL;
}

bool
ArmsStorePurchaseScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {

        return false;
    }

    /*background*/
    auto backGround = Sprite::create("background/blue_moon.png");
    backGround->setContentSize(Size(_visibleSize.width, _visibleSize.height));
    backGround->setPosition(Vec2(_visibleSize.width * 0.5, _visibleSize.height * 0.5));
    addChild(backGround, 0, 1);
    /*background2*/
    auto bg_2 =
        PlaceHolder::createRect(Size(_visibleSize.width * 0.859, _visibleSize.height * 0.78), "",
                                16, Color3B(91, 155, 213));
    bg_2->setPosition(_visibleSize / 2);
    addChild(bg_2, 0, 2);
    /*background3*/
    auto bg_3 = PlaceHolder::createRect(
        Size(_visibleSize.width * 0.595, _visibleSize.height * 0.707), "", 16, Color3B::WHITE);
    bg_3->setAnchorPoint(Vec2(0, 0));
    bg_3->setPosition(Vec2(_visibleSize.width * 0.29, _visibleSize.height * 0.141));
    addChild(bg_3, 0, 3);

    /*返回按钮*/
    auto backButton = Button::create("menu/buttonNormal.png");
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

    /*符卡*/
    auto strongProps = Button::create("menu/white.png", "", "");
    strongProps->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.6));
    strongProps->setTitleText("符卡");
    strongProps->setTitleFontSize(20);
    strongProps->setTitleColor(Color3B(91, 155, 213));
    strongProps->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            currentType = Item::Type::OTHER;
            itemTable->reloadData();
        }
    });
    addChild(strongProps, 0);

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
ArmsStorePurchaseScene::onEnter()
{
    Scene::onEnter();

    /*加载道具列表*/
    normalItems = gamedata->getNormalItemsInStore("ArmsStore");
    /*加载符卡列表*/
    spellCards = gamedata->getSpellCardsInStore("ArmsStore");

    currentType = Item::Type::NORMAL;
    itemTable->reloadData(); //加载数据
}

void
ArmsStorePurchaseScene::onExit()
{
    Scene::onExit();
}

Size
ArmsStorePurchaseScene::tableCellSizeForIndex(TableView* table, ssize_t idx)
{
    return Size(750, 100);
}

TableViewCell*
ArmsStorePurchaseScene::tableCellAtIndex(TableView* table, ssize_t idx)
{
    TableViewCell* cell = table->dequeueCell();
    if (!cell) {
        cell = new TableViewCell();
        cell->autorelease();
    } else {
        cell->removeAllChildren();
    }

    if (currentType == Item::Type::NORMAL) {
        vector<Item> currentItems = normalItems;
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

            auto description =
                Label::create(currentItems[idx].description, "fonts/dengxian.ttf", 20);
            description->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            description->setPosition(Vec2(400, 50));
            description->setColor(Color3B::BLACK);
            cell->addChild(description);

            string temp;
            string money;
            stringstream ss;
            ss << currentItems[idx].price;
            ss >> money;
            temp = money + "钱币";
            auto availableItems = gamedata->getAvailableItems();
            for (int i = 0; i < availableItems.size(); i++) {
                if (currentItems[idx].tag == availableItems[i].tag) {
                    temp = "已购买";
                    break;
                }
            }
            auto status = Label::create(temp, "fonts/dengxian.ttf", 20);
            status->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            status->setPosition(Vec2(650, 50));
            status->setColor(Color3B::BLACK);
            status->setName("status");
            cell->addChild(status);
        }
    } else if (currentType == Item::Type::OTHER) {
        vector<SpellCard> currentCards = spellCards;
        if (currentCards.size() > 0) {
            cell->setName(currentCards[idx].tag);

            auto box = PlaceHolder::createRect(Size(720, 90), "", 16, Color3B(91, 155, 213));
            box->setPosition(Vec2(0, 50));
            box->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
            cell->addChild(box);

            auto icon = Sprite::create(currentCards[idx].icon);
            icon->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            icon->setPosition(Vec2(30, 50));
            cell->addChild(icon);

            auto name = Label::create(currentCards[idx].name, "fonts/dengxian.ttf", 20);
            name->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            name->setPosition(Vec2(130, 50));
            name->setColor(Color3B::BLACK);
            cell->addChild(name);

            auto description =
                Label::create(currentCards[idx].description, "fonts/dengxian.ttf", 20);
            description->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            description->setPosition(Vec2(400, 50));
            description->setColor(Color3B::BLACK);
            cell->addChild(description);

            string temp = "";
            string money;
            stringstream ss;
            ss << currentCards[idx].price;
            ss >> money;
            temp = money + "钱币";
            auto availableCards = gamedata->getAvailableSpellCards();
            for (int i = 0; i < availableCards.size(); i++) {
                if (currentCards[idx].tag == availableCards[i].tag) {
                    temp = "已购买";
                    break;
                }
            }
            auto status = Label::create(temp, "fonts/dengxian.ttf", 20);
            status->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            status->setPosition(Vec2(650, 50));
            status->setColor(Color3B::BLACK);
            status->setName("status");
            cell->addChild(status);
        }
    }

    return cell;
}

ssize_t
ArmsStorePurchaseScene::numberOfCellsInTableView(TableView* table)
{
    if (currentType == Item::Type::OTHER)
        return spellCards.size();
    else if (currentType == Item::Type::NORMAL)
        return normalItems.size();
}

void
ArmsStorePurchaseScene::tableCellTouched(TableView* table, TableViewCell* cell)
{
    auto status = (Label*)cell->getChildByName("status");
    if (status->getString() == "已购买") {
        return;
    }
    auto gamedata = GameData::getInstance();
    string tag = cell->getName();
    auto money = gamedata->getMoneyNum();

    if (currentType == Item::Type::NORMAL) {
        auto item = gamedata->getItemByTag(tag);
        if (item.price > money) {
            status->setString("余额不足");
            return;
        }

        std::function<void()> refreshCell = [cell]() {
            auto status = (Label*)cell->getChildByName("status");
            status->setString("已购买");
        };
        std::function<void()> func = std::bind(&GameData::buyItem, gamedata, tag);
        this->addChild(ConfirmButton::create(func, refreshCell));
    } else if (currentType == Item::Type::OTHER) {
        auto card = gamedata->getSpellCardByTag(tag);
        if (card.price > money) {
            status->setString("余额不足");
            return;
        }

        std::function<void()> refreshCell = [cell]() {
            auto status = (Label*)cell->getChildByName("status");
            status->setString("已购买");
        };
        std::function<void()> func = std::bind(&GameData::buySpellCard, gamedata, tag);
        this->addChild(ConfirmButton::create(func, refreshCell));
    }
}