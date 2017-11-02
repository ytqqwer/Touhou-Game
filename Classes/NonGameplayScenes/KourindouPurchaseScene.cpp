#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/KourindouPurchaseScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"

#include "AudioController.h"
#include "Layers/ConfirmButton.h"

#include "ui/CocosGUI.h"
using namespace ui;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string KourindouPurchaseScene::TAG{ "KourindouPurchaseScene" };

KourindouPurchaseScene::KourindouPurchaseScene()
{
    gamedata = GameData::getInstance();
    _visibleSize = _director->getVisibleSize();
    currentType = Item::Type::NORMAL;
}

bool
KourindouPurchaseScene::init()
{
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
    auto backButton = Button::create("menu/p1.png", "", "");
    backButton->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    backButton->setTitleText("返回");
    backButton->setTitleFontSize(20);
    backButton->setTitleColor(Color3B::WHITE);
    backButton->setContentSize(Size(_visibleSize.width * 0.15, _visibleSize.height * 0.15));
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

    /*解锁栏位*/
    auto unlockColumn = Button::create("menu/white.png", "", "");
    unlockColumn->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.45));
    unlockColumn->setTitleText("解锁栏位");
    unlockColumn->setTitleFontSize(20);
    unlockColumn->setTitleColor(Color3B(91, 155, 213));
    unlockColumn->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            currentType = Item::Type::OTHER;
            itemTable->reloadData();
        }
    });
    addChild(unlockColumn, 0);

    /*道具列表控件*/
    itemTable = TableView::create(this, Size(750, 500));
    itemTable->setPosition(Vec2(20, 0));
    itemTable->setDirection(cocos2d::extension::ScrollView::Direction::VERTICAL); //纵向
    itemTable->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN); //从小到大排列
    itemTable->setDelegate(this);                                            //委托代理
    bg_3->addChild(itemTable);

    return true;
}

void
KourindouPurchaseScene::onEnter()
{
    Scene::onEnter();

    /*加载道具列表*/
    normalItems = gamedata->getNormalItemsInStore("Kourindou");
    strengthenItems = gamedata->getStrengthenItemsInStore("Kourindou");
    characters = gamedata->getAvailableCharacterList();

    currentType = Item::Type::NORMAL;
    itemTable->reloadData(); //加载数据
}

void
KourindouPurchaseScene::onExit()
{
    Scene::onExit();
}

Size
KourindouPurchaseScene::tableCellSizeForIndex(TableView* table, ssize_t idx)
{
    if (currentType == Item::Type::OTHER)
        return Size(750, 170);
    else
        return Size(750, 100);
}

TableViewCell*
KourindouPurchaseScene::tableCellAtIndex(TableView* table, ssize_t idx)
{
    TableViewCell* cell = table->dequeueCell();
    if (!cell) {
        cell = new TableViewCell();
        cell->autorelease();
    } else {
        cell->removeAllChildren();
    }

    if (currentType == Item::Type::OTHER) {
        auto characters = gamedata->getAvailableCharacterList();
        if (characters.size() > 0) {
            auto characterTag = characters[idx].tag;

            auto box = PlaceHolder::createRect(Size(720, 150), "", 16, Color3B(91, 155, 213));
            box->setPosition(Vec2(0, 100));
            box->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
            cell->addChild(box);

            auto standAnimation = Animation::create();
            for (auto v : characters[idx].standFrame) {
                standAnimation->addSpriteFrameWithFile(v);
            }
            standAnimation->setDelayPerUnit(characters[idx].standFrameDelay);
            auto portrait = Sprite::create();
            portrait->runAction(RepeatForever::create(Animate::create(standAnimation)));
            portrait->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            portrait->setPosition(Vec2(50, 85));
            cell->addChild(portrait);

            /*道具栏解锁相关*/
            do {
                auto itemSlotLabel = Label::createWithTTF("道具栏", "fonts/dengxian.ttf", 20);
                itemSlotLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                itemSlotLabel->setPosition(Vec2(150, 100));
                itemSlotLabel->setColor(Color3B::BLACK);
                cell->addChild(itemSlotLabel);

                string temp;
                stringstream ss;
                ss << characters[idx].itemSlotNum;
                ss >> temp;
                auto itemSlotNumerLabel =
                    Label::createWithTTF(temp + "/3", "fonts/dengxian.ttf", 20);
                itemSlotNumerLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                itemSlotNumerLabel->setPosition(Vec2(150, 70));
                itemSlotNumerLabel->setColor(Color3B::BLACK);
                cell->addChild(itemSlotNumerLabel);

                auto unlockItemSlotButton = Button::create("menu/white.png", "", "");
                unlockItemSlotButton->setPosition(Vec2(300, 85));
                unlockItemSlotButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                unlockItemSlotButton->setTitleFontSize(30);
                unlockItemSlotButton->setTitleColor(Color3B::BLACK);
                unlockItemSlotButton->setTitleText("已满");
                cell->addChild(unlockItemSlotButton);
                if (characters[idx].itemSlotNum < 3) {
                    unlockItemSlotButton->setTitleText("5000钱币");
                    unlockItemSlotButton->addTouchEventListener(
                        [this, characterTag, itemSlotNumerLabel,
                         unlockItemSlotButton](Ref* pSender, Widget::TouchEventType type) {
                            if (type == Widget::TouchEventType::ENDED) {
                                if (gamedata->getMoneyNum() < 5000) {
                                    unlockItemSlotButton->setTitleText("余额不足");
                                    return;
                                }
                                auto character =
                                    GameData::getInstance()->getCharacterByTag(characterTag);
                                if (character.itemSlotNum >= 3) {
                                    //栏位解锁完毕后，再次点击将无效
                                    return;
                                }

                                std::function<void()> func =
                                    std::bind(&GameData::unlockItemSlot, gamedata, characterTag);
                                std::function<void()> callBack =
                                    [itemSlotNumerLabel, unlockItemSlotButton, characterTag]() {
                                        auto character = GameData::getInstance()->getCharacterByTag(
                                            characterTag); //获得新数据
                                        string temp;
                                        stringstream ss;
                                        ss << character.itemSlotNum;
                                        ss >> temp;
                                        itemSlotNumerLabel->setString(temp + "/3");

                                        if (character.itemSlotNum >= 3) {
                                            unlockItemSlotButton->setTitleText("已满");
                                        }
                                    };
                                this->addChild(ConfirmButton::create(func, callBack));
                            }
                        });
                }
            } while (0);

            /*符卡栏解锁相关*/
            do {
                auto cardSlotLabel = Label::createWithTTF("符卡栏", "fonts/dengxian.ttf", 20);
                cardSlotLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                cardSlotLabel->setPosition(Vec2(450, 100));
                cardSlotLabel->setColor(Color3B::BLACK);
                cell->addChild(cardSlotLabel);

                string temp;
                stringstream ss;
                ss << characters[idx].spellCardSlotNum;
                ss >> temp;
                auto cardSlotNumerLabel =
                    Label::createWithTTF(temp + "/3", "fonts/dengxian.ttf", 20);
                cardSlotNumerLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                cardSlotNumerLabel->setPosition(Vec2(450, 70));
                cardSlotNumerLabel->setColor(Color3B::BLACK);
                cell->addChild(cardSlotNumerLabel);

                auto unlockCardSlotButton = Button::create("menu/white.png", "", "");
                unlockCardSlotButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
                unlockCardSlotButton->setPosition(Vec2(600, 85));
                unlockCardSlotButton->setTitleFontSize(30);
                unlockCardSlotButton->setTitleColor(Color3B::BLACK);
                unlockCardSlotButton->setTitleText("已满");
                cell->addChild(unlockCardSlotButton);
                if (characters[idx].spellCardSlotNum < 3) {
                    unlockCardSlotButton->setTitleText("5000钱币");
                    unlockCardSlotButton->addTouchEventListener(
                        [this, characterTag, cardSlotNumerLabel,
                         unlockCardSlotButton](Ref* pSender, Widget::TouchEventType type) {
                            if (type == Widget::TouchEventType::ENDED) {
                                if (gamedata->getMoneyNum() < 5000) {
                                    unlockCardSlotButton->setTitleText("余额不足");
                                    return;
                                }
                                auto character =
                                    GameData::getInstance()->getCharacterByTag(characterTag);
                                if (character.spellCardSlotNum >= 3) {
                                    //栏位解锁完毕后，再次点击将无效
                                    return;
                                }

                                std::function<void()> func = std::bind(
                                    &GameData::unlockSpellCardSlot, gamedata, characterTag);
                                std::function<void()> callBack =
                                    [cardSlotNumerLabel, unlockCardSlotButton, characterTag]() {
                                        auto character = GameData::getInstance()->getCharacterByTag(
                                            characterTag); //获得新数据
                                        string temp;
                                        stringstream ss;
                                        ss << character.spellCardSlotNum;
                                        ss >> temp;
                                        cardSlotNumerLabel->setString(temp + "/3");

                                        if (character.spellCardSlotNum >= 3) {
                                            unlockCardSlotButton->setTitleText("已满");
                                        }
                                    };
                                this->addChild(ConfirmButton::create(func, callBack));
                            }
                        });
                }

            } while (0);
        }
    } else if (currentType == Item::Type::NORMAL || currentType == Item::Type::STRENGTHEN) {
        vector<Item> currentItems;
        if (currentType == Item::Type::STRENGTHEN)
            currentItems = strengthenItems;
        else
            currentItems = normalItems;

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

            auto name = Label::createWithTTF(currentItems[idx].name, "fonts/dengxian.ttf", 20);
            name->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            name->setPosition(Vec2(130, 50));
            name->setColor(Color3B::BLACK);
            cell->addChild(name);

            auto description =
                Label::createWithTTF(currentItems[idx].description, "fonts/dengxian.ttf", 20);
            description->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            description->setPosition(Vec2(350, 50));
            description->setColor(Color3B::BLACK);
            cell->addChild(description);

            string temp = "";
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
            auto status = Label::createWithTTF(temp, "fonts/dengxian.ttf", 20);
            status->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            status->setPosition(Vec2(600, 50));
            status->setColor(Color3B::BLACK);
            status->setName("status");
            cell->addChild(status);
        }
    }

    return cell;
}

ssize_t
KourindouPurchaseScene::numberOfCellsInTableView(TableView* table)
{
    if (currentType == Item::Type::NORMAL)
        return normalItems.size();
    else if (currentType == Item::Type::STRENGTHEN)
        return strengthenItems.size();
    else if (currentType == Item::Type::OTHER)
        return characters.size();
}

void
KourindouPurchaseScene::tableCellTouched(TableView* table, TableViewCell* cell)
{
    auto gamedata = GameData::getInstance();
    if (currentType == Item::Type::NORMAL || currentType == Item::Type::STRENGTHEN) {
        auto status = (Label*)cell->getChildByName("status");
        if (status->getString() == "已购买") {
            return;
        }

        string tag = cell->getName();
        auto item = gamedata->getItemByTag(tag);
        auto money = gamedata->getMoneyNum();
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
        //解锁栏位
    }
}
