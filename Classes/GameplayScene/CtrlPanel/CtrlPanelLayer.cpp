#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/CtrlPanel/CtrlPanelLayer.h"
#include "GameData/GameData.h"
#include "GameplayScene/CtrlPanel/CoolDownButton.h"
#include "GameplayScene/CtrlPanel/HPManaBar.h"
#include "GameplayScene/CtrlPanel/ItemButton.h"
#include "GameplayScene/CtrlPanel/SpellCardButton.h"

#include <functional>

#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace std;
using namespace cocos2d::ui;

bool
CtrlPanelLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    _gamedata = GameData::getInstance();
    _visibleSize = Director::getInstance()->getVisibleSize();

    /*  1. 从 GameData 中获得两名出战角色的数据 */

    auto characterTagList = _gamedata->getOnStageCharacterTagList();
    Character characterList[2];
    characterList[0] = _gamedata->getCharacterByTag(characterTagList[0]);
    characterList[1] = _gamedata->getCharacterByTag(characterTagList[1]);

    /*  2. CharacterCtrlPanel
     *     P1 和 P2 使用不同的 CharacterCtrlPanel，
     *     切换人物的时候『隐藏一个，显示一个』, 这样可以不用再重新设置 符卡/道具 的纹理和监听器,
     *     可以不用再切换血条蓝条
     */

    _characterCtrlPanel[0] = Layer::create();
    _characterCtrlPanel[1] = Layer::create();
    _characterCtrlPanel[0]->setVisible(true);
    _characterCtrlPanel[1]->setVisible(false);
    this->addChild(_characterCtrlPanel[0]);
    this->addChild(_characterCtrlPanel[1]);

    initCharacterPanelUIAndListener();

    /*  3. 设置, dash 按钮 */

    // 设置按钮
    auto settingsBtn = Button::create("CloseNormal.png");
    settingsBtn->setPosition(Vec2(_visibleSize.width * 0.060, _visibleSize.height * 0.920));
    settingsBtn->setScale(1.5);
    settingsBtn->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            this->_eventDispatcher->dispatchCustomEvent("settings_key_pressed");
        }
    });
    this->addChild(settingsBtn);

    // dash
    auto dashButton = Button::create("gameplayscene/dashButton.png");
    dashButton->setPosition(Vec2(_visibleSize.width * 0.920, _visibleSize.height * 0.080));
    dashButton->setScale(0.6);
    dashButton->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            this->_eventDispatcher->dispatchCustomEvent("dash_key_pressed");
        }

    });
    this->addChild(dashButton);

    /*  4. Touch, Keyboard, CustomEvent Listener */

    initTouchListener();
    initKeyboardListener();

    return true;
}

void
CtrlPanelLayer::initCharacterPanelUIAndListener()
{
    auto characterTagList = _gamedata->getOnStageCharacterTagList();
    Character characterList[2];
    characterList[0] = _gamedata->getCharacterByTag(characterTagList[0]);
    characterList[1] = _gamedata->getCharacterByTag(characterTagList[1]);

    for (int i : { 0, 1 }) {
        auto panel = _characterCtrlPanel[i];

        /*  1. 道具 Item */

        int increment = 0;
        auto itemList = _gamedata->getCharacterItemList(characterTagList[i]);
        for (int j = 0; j < itemList.size(); j++) {
            Item item = itemList[j];
            CoolDownButton* btn = ItemButton::create(item);

            panel->addChild(btn);
            btn->setPosition(
                Vec2(_visibleSize.width * 0.330 + increment, _visibleSize.height * 0.080));

            increment = increment + btn->getContentSize().width + 50;
        }

        /*  2. 符卡 SpellCard */

        increment = 0;
        auto spellCardList = _gamedata->getCharacterSpellCardList(characterTagList[0]);
        for (int j = 0; j < spellCardList.size(); j++) {
            SpellCard card = spellCardList[j];
            CoolDownButton* btn = SpellCardButton::create(card);

            panel->addChild(btn);
            btn->setPosition(
                Vec2(_visibleSize.width * 0.550 + increment, _visibleSize.height * 0.080));

            increment = increment + btn->getContentSize().width + 50;
        }

        /*  3. 血条, 蓝条 */

        auto hpManaBar = HpManaBar::create();
        hpManaBar->setAnchorPoint(Vec2(0, 1));
        hpManaBar->setPosition(Vec2(_visibleSize.width * 0.090, _visibleSize.height * 0.930));
        // TODO, 追踪 Player
        // hpManaBar->setPlayer(Player *);
        panel->addChild(hpManaBar);

        /*  4. 切换攻击方式按钮 */

        auto switchAttackTypeBtn = Button::create("gameplayscene/switchAttackType.png");
        switchAttackTypeBtn->setPosition(
            Vec2(_visibleSize.width * 0.840, _visibleSize.height * 0.080));
        switchAttackTypeBtn->setScale(1.5);
        switchAttackTypeBtn->addTouchEventListener(
            [this](Ref* pSender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {
                    // 向外部发出事件
                    this->_eventDispatcher->dispatchCustomEvent("switch_attack_type");
                }
            });
        panel->addChild(switchAttackTypeBtn);

        /*  5. 切换角色按钮 */

        // i 不是 0 就是 1, 所以此处用了 [1-i] 来表示非当前角色
        auto switchCharacterBtn = Button::create(characterList[i].circularAvatar);
        switchCharacterBtn->setPosition(
            Vec2(_visibleSize.width * 0.060, _visibleSize.height * 0.820));
        switchCharacterBtn->setScale(0.2);
        switchCharacterBtn->addTouchEventListener(
            [this, i, switchCharacterBtn](Ref* pSender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {
                    // 处理自己
                    this->_characterCtrlPanel[1 - i]->setVisible(true);
                    this->_characterCtrlPanel[_currCharacterIdx]->setVisible(false);
                    _currCharacterIdx = 1 - i;

                    // 向外部发出事件
                    this->_eventDispatcher->dispatchCustomEvent("switch_character");
                }
            });
        panel->addChild(switchCharacterBtn);
    }
}

void
CtrlPanelLayer::initTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        auto location = touch->getLocation();
        if (location.x > _visibleSize.width / 2 && location.y < _visibleSize.height / 2) {
            //触碰右下屏
            _eventDispatcher->dispatchCustomEvent("right_key_pressed");
        } else if (location.x < _visibleSize.width / 2 && location.y < _visibleSize.height / 2) {
            //触碰左下屏
            _eventDispatcher->dispatchCustomEvent("left_key_pressed");
        } else if (location.y >= _visibleSize.height / 2) {
            //触碰上半屏
            _eventDispatcher->dispatchCustomEvent("jump_key_pressed");
        }

        return true;
    };

    listener->onTouchMoved = [this](Touch* touch, Event* e) -> void {
        auto location = touch->getLocation();
        if (location.y >= _visibleSize.height / 2) {
            _eventDispatcher->dispatchCustomEvent("motion_key_released");
        }
    };

    listener->onTouchEnded = [this](Touch* touch, Event* e) -> void {
        _eventDispatcher->dispatchCustomEvent("motion_key_released");
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void
CtrlPanelLayer::initKeyboardListener()
{
    auto listener = EventListenerKeyboard::create();

    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) -> void {
        if (keyCode == EventKeyboard::KeyCode::KEY_A ||
            keyCode == EventKeyboard::KeyCode::KEY_CAPITAL_A) {
            _eventDispatcher->dispatchCustomEvent("left_key_pressed");
        } else if (keyCode == EventKeyboard::KeyCode::KEY_D ||
                   keyCode == EventKeyboard::KeyCode::KEY_CAPITAL_D) {
            _eventDispatcher->dispatchCustomEvent("right_key_pressed");
        } else if (keyCode == EventKeyboard::KeyCode::KEY_S ||
                   keyCode == EventKeyboard::KeyCode::KEY_CAPITAL_S) {
            _eventDispatcher->dispatchCustomEvent("dash_key_pressed");
        } else if (keyCode == EventKeyboard::KeyCode::KEY_W ||
                   keyCode == EventKeyboard::KeyCode::KEY_CAPITAL_W) {
            _eventDispatcher->dispatchCustomEvent("jump_key_pressed");
        };
    };

    listener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) -> void {
        if (keyCode == EventKeyboard::KeyCode::KEY_A ||
            keyCode == EventKeyboard::KeyCode::KEY_CAPITAL_A ||
            keyCode == EventKeyboard::KeyCode::KEY_D ||
            keyCode == EventKeyboard::KeyCode::KEY_CAPITAL_D) {
            _eventDispatcher->dispatchCustomEvent("motion_key_released");
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
