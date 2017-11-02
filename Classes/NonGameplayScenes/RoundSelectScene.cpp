#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/RoundSelectScene.h"
#include "NonGameplayScenes/HomeScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"

// #include "resources.h.dir/main_menu.h"

#include "GameData/GameData.h"
#include "Layers/LoadingLayer.h"

#include "AudioController.h"

#include "ui/CocosGUI.h"
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

    return true;
}

void
RoundSelectScene::onEnter()
{
    Scene::onEnter();

    //取得当前所在的地点和回合，gameDate，以及角色,以及当前可以使用的所有角色
    Location location = GameData::getInstance()->getCurrentLocation();

    /*背景音乐*/
    AudioController::getInstance()->playMusic(location.backgroundMusic, true);

    /*****************静态不会变化的板块.创建大概模型*****************/
    //创建背景
    auto backGround = Sprite::create(location.backgroundPicture);
    backGround->setContentSize(Size(_visibleSize.width, _visibleSize.height));
    backGround->setPosition(Vec2(_visibleSize.width * 0.5, _visibleSize.height * 0.5));
    this->addChild(backGround);

    auto backGround1 = Sprite::create("menu/layer_1.png");
    backGround1->setContentSize(Size(_visibleSize.width * 0.6, _visibleSize.height * 0.25));
    backGround1->setPosition(Vec2(_visibleSize.width * 0.65, _visibleSize.height * 0.7));
    this->addChild(backGround1);

    auto backGround2 = Sprite::create("menu/layer_2.png");
    backGround2->setContentSize(Size(_visibleSize.width * 0.6, _visibleSize.height * 0.5));
    backGround2->setPosition(Vec2(_visibleSize.width * 0.65, _visibleSize.height * 0.3));
    this->addChild(backGround2);

    //创建返回按钮
    auto backButton = Button::create("menu/p1.png", "", "");
    backButton->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    backButton->setTitleText("返回");
    backButton->setTitleFontSize(20);
    backButton->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playReturnButtonEffect();
            Director::getInstance()->replaceScene(HomeScene::create());
        }
    });
    this->addChild(backButton);

    //创建游戏开始按钮,等创建好了gameScene会添加一个事件监听器
    auto beginButton = Button::create("menu/p10.png", "", "");
    beginButton->setTitleText("开始游戏");
    beginButton->setTitleFontSize(20);
    beginButton->setPosition(Vec2(_visibleSize.width * 0.9, _visibleSize.height * 0.2));
    beginButton->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            //进入游戏场景前需要做一些清理工作
            AudioController::getInstance()->playClickButtonEffect();
            AudioController::getInstance()->clearMusic();
            auto loadingLayer = LoadingLayer::create(this->selectedMap);
            this->addChild(loadingLayer);
        }
    });
    this->addChild(beginButton, 2);

    /***********************创建会变化的模块********************/

    //创建当前场景的场景名字的艺术字
    auto sceneName = Sprite::create(location.wordArt);
    setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    sceneName->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.8));
    this->addChild(sceneName);

    /******创建关卡描述的部分，对于不同的关卡，只是用set函数来修改其中的图片或者文字，不重复创建，默认为第一个关卡***********/
    rounds = GameData::getInstance()->getRoundList(location.tag);
    //获得当前关卡的预览图片
    preViewPic = Sprite::create(rounds[0].previewPicture);
    preViewPic->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    preViewPic->setPosition(Vec2(_visibleSize.width * 0.5, _visibleSize.height * 0.7));
    this->addChild(preViewPic);
    //难度字体
    auto difficultText = Text::create("难度", "Arial", 24);
    difficultText->setPosition(Vec2(Vec2(_visibleSize.width * 0.65, _visibleSize.height * 0.75)));
    difficultText->setFontSize(25);
    this->addChild(difficultText);
    //设置tmx地图
    selectedMap = rounds[0].TMXMap;

    //设置5颗空星，表示难度，然后根据不同的回合难度换成相应的实心
    for (int i = 0; i < 5; ++i) {
        difficult[i] = Sprite::create("menu/star_2.png");
        difficult[i]->setContentSize(Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
        // difficult[i]->setScale(0.1);
        difficult[i]->setPosition(
            Vec2(_visibleSize.width * (0.7 + 0.05 * i), _visibleSize.height * 0.75));
        this->addChild(difficult[i]);
    }

    //根据当前难度，切换相应的星形 ,获得当前关卡的难度，并显示出来
    int difficulty(static_cast<int>(rounds[0].difficulty));
    for (int i = 0; i < difficulty; ++i) {
        difficult[i]->setTexture("menu/star_1.png");
        // difficult[i]->setScale(0.1);
        difficult[i]->setContentSize(Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
    }

    //获得当前关卡的描述
    preViewText = Text::create(rounds[0].description, "Arial", 24);
    preViewText->setPosition(Vec2(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.65)));
    preViewText->setContentSize(Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
    this->addChild(preViewText);

    /*************创建当前地点的关卡选择,循环创建，不固定,同时设置关卡标签的监听事件，改变上面的固定的值******************/
    for (unsigned int i = 0; i < location.totalRound; ++i) {
        if (i < location.passedRound - 1) { //对于已经通关的关卡的关卡
            auto roundButton = Button::create("menu/forbid_button.png", "", "");
            roundButton->setPosition(
                Vec2(_visibleSize.width * 0.2, _visibleSize.height * (0.8 - 0.15 * (i + 1))));
            roundButton->setTitleText(rounds[i].name);
            roundButton->setTitleFontSize(25);
            //为了捕获局部变量i，使用值传递方式进行捕获
            roundButton->addTouchEventListener(
                [i, this](Ref* pSender, Widget::TouchEventType type) {
                    if (type == Widget::TouchEventType::ENDED) {
                        AudioController::getInstance()->playClickButtonEffect();
                        setRoundInformation(rounds[i]);
                        GameData::getInstance()->setRoundToPlay(rounds[i].tag);
                    }
                });
            this->addChild(roundButton);
        }

        if (i == location.passedRound - 1) { //对于可以打，但还没通关的关卡
            auto roundButton = Button::create("menu/forbid_button.png", "", "");
            roundButton->setPosition(
                Vec2(_visibleSize.width * 0.2, _visibleSize.height * (0.8 - 0.15 * (i + 1))));
            roundButton->setTitleText(rounds[i].name);
            roundButton->setTitleFontSize(25);
            roundButton->addTouchEventListener(
                [i, this](Ref* pSender, Widget::TouchEventType type) {
                    if (type == Widget::TouchEventType::ENDED) {
                        AudioController::getInstance()->playClickButtonEffect();
                        setRoundInformation(rounds[i]);
                        GameData::getInstance()->setRoundToPlay(rounds[i].tag);
                    }

                });
            this->addChild(roundButton);
        }

        if (i >= location.passedRound) { //对于没有通关的关卡
            auto roundButton = Button::create("menu/forbid_button.png", "", "");
            roundButton->setPosition(
                Vec2(_visibleSize.width * 0.2, _visibleSize.height * (0.8 - 0.15 * (i + 1))));
            roundButton->setTitleText(rounds[i].name);
            roundButton->setTitleFontSize(25);
            roundButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
                if (type == Widget::TouchEventType::ENDED) {
                    AudioController::getInstance()->playClickButtonEffect();
                    setRoundInformation(rounds[i]);
                }

            });
            this->addChild(roundButton);
        }
    }

    //当前场景人物的描述
    auto bottom1 = Sprite::create("menu/layer_3.png");
    bottom1->setPosition(Vec2(_visibleSize.width * (0.5), _visibleSize.height * 0.3));
    bottom1->setContentSize(Size(_visibleSize.width * 0.19, _visibleSize.height * 0.4));
    this->addChild(bottom1);

    auto bottom2 = Sprite::create("menu/layer_3.png");
    bottom2->setPosition(Vec2(_visibleSize.width * (0.7), _visibleSize.height * 0.3));
    bottom2->setContentSize(Size(_visibleSize.width * 0.19, _visibleSize.height * 0.4));
    this->addChild(bottom2);

    //人物,以及人物信息
    vector<string> onStageCharacter = GameData::getInstance()->getOnStageCharacterTagList();
    p1 = GameData::getInstance()->getCharacterByTag(onStageCharacter[0]);
    p2 = GameData::getInstance()->getCharacterByTag(onStageCharacter[1]);

    //设置p1的肖像
    p1Portrait = Sprite::create();
    p1Portrait->setPosition(Vec2(_visibleSize.width * (0.5), _visibleSize.height * 0.4));
    this->addChild(p1Portrait, 3);

    auto p1StandAnimation = Animation::create();
    for (auto v : p1.standFrame) {
        p1StandAnimation->addSpriteFrameWithFile(v);
    }
    p1StandAnimation->setDelayPerUnit(p1.standFrameDelay);
    p1Portrait->runAction(RepeatForever::create(Animate::create(p1StandAnimation)));

    //设置p1的符卡
    setSpellCard(p1.tag, p1CardSprites, 0.45);

    //设置p1的道具
    setItem(p1.tag, p1ItemSprites, 0.45);

    // p1的修改按钮
    auto p1Button = Button::create("menu/right.png");
    p1Button->setPosition(Vec2(_visibleSize.width * (0.43), _visibleSize.height * 0.4));
    this->addChild(p1Button, 3);

    p1Button->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            auto characters = GameData::getInstance()->getAvailableCharacterList();
            if (characters.size() <= GameData::getInstance()->getOnStageCharacterTagList().size()) {
                return;
            }
            Character character;
            //取得将要更换的角色
            int p1_Order = 0;
            for (int i = 0; i < characters.size(); i++) {
                if (p1.tag == characters[i].tag) {
                    p1_Order = i;
                    break;
                }
            }
            for (int i = 0; i < characters.size(); i++) {
                if (p1.tag != characters[p1_Order].tag) {
                    if (p2.tag != characters[p1_Order].tag) {
                        character = characters[p1_Order];
                    }
                }
                p1_Order = (++p1_Order) % characters.size();
            }
            //更换角色
            GameData::getInstance()->switchOnStageCharacter(1, character.tag);
            p1 = character;
            //替换立绘
            auto standAnimation = Animation::create();
            for (auto v : p1.standFrame) {
                standAnimation->addSpriteFrameWithFile(v);
            }
            standAnimation->setDelayPerUnit(p1.standFrameDelay);
            p1Portrait->stopAllActions();
            p1Portrait->runAction(RepeatForever::create(Animate::create(standAnimation)));
            //替换符卡
            setSpellCard(character.tag, p1CardSprites, 0.45);
            //替换道具
            setItem(character.tag, p1ItemSprites, 0.45);
        }
    });

    //设置p2的肖像
    p2Portrait = Sprite::create();
    p2Portrait->setPosition(Vec2(_visibleSize.width * (0.7), _visibleSize.height * 0.4));
    this->addChild(p2Portrait, 3);

    auto p2StandAnimation = Animation::create();
    for (auto v : p2.standFrame) {
        p2StandAnimation->addSpriteFrameWithFile(v);
    }
    p2StandAnimation->setDelayPerUnit(p2.standFrameDelay);
    p2Portrait->runAction(RepeatForever::create(Animate::create(p2StandAnimation)));

    //设置p2的符卡
    setSpellCard(p2.tag, p2CardSprites, 0.65);

    //设置p2的道具
    setItem(p2.tag, p2ItemSprites, 0.65);

    // p2的修改按钮
    auto p2Button = Button::create("menu/right.png");
    p2Button->setPosition(Vec2(_visibleSize.width * (0.63), _visibleSize.height * 0.4));
    this->addChild(p2Button, 3);
    p2Button->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            auto characters = GameData::getInstance()->getAvailableCharacterList();
            if (characters.size() <= GameData::getInstance()->getOnStageCharacterTagList().size()) {
                return;
            }
            Character character;
            //取得将要更换的角色
            int p2_Order = 0;
            for (int i = 0; i < characters.size(); i++) {
                if (p2.tag == characters[i].tag) {
                    p2_Order = i;
                    break;
                }
            }
            for (int i = 0; i < characters.size(); i++) {
                if (p2.tag != characters[p2_Order].tag) {
                    if (p1.tag != characters[p2_Order].tag) {
                        character = characters[p2_Order];
                    }
                }
                p2_Order = (++p2_Order) % characters.size();
            }
            //更换角色
            GameData::getInstance()->switchOnStageCharacter(2, character.tag);
            p2 = character;
            //替换立绘
            auto standAnimation = Animation::create();
            for (auto v : p2.standFrame) {
                standAnimation->addSpriteFrameWithFile(v);
            }
            standAnimation->setDelayPerUnit(p2.standFrameDelay);
            p2Portrait->stopAllActions();
            p2Portrait->runAction(RepeatForever::create(Animate::create(standAnimation)));
            //替换符卡
            setSpellCard(character.tag, p2CardSprites, 0.65);
            //替换道具
            setItem(character.tag, p2ItemSprites, 0.65);
        }
    });
}

void
RoundSelectScene::onExit()
{
    Scene::onExit();
    this->removeAllChildren();
}

void
RoundSelectScene::setRoundInformation(Round round)
{
    //根据点击的按钮设置当前的关卡难度，同时将当前选择的关卡保存，接口尚未实现，以后添加
    int difficulty(static_cast<int>(round.difficulty));
    for (int i = 0; i < difficulty; ++i) {
        difficult[i]->setTexture("menu/star_1.png");
        difficult[i]->setContentSize(Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
    }
    for (int i = difficulty; i < 5; ++i) {
        difficult[i]->setTexture("menu/star_2.png");
        difficult[i]->setContentSize(Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
    }
    preViewPic->setTexture(round.previewPicture);
    preViewPic->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    preViewText->setText(round.description);
    selectedMap = round.TMXMap;
}

void
RoundSelectScene::setSpellCard(const std::string& characterTag, Vector<Sprite*>& sprites, float pos)
{
    for (auto i : sprites) {
        i->removeFromParentAndCleanup(true);
    }
    vector<SpellCard> cards = GameData::getInstance()->getCharacterEquipedSpellCards(characterTag);
    for (int i = 0; i < cards.size(); ++i) {
        auto cardIcon = Sprite::create(cards.at(i).icon);
        cardIcon->setPosition(
            Vec2(_visibleSize.width * (pos + 0.05 * i), _visibleSize.height * 0.25));
        cardIcon->setScale(0.8);
        sprites.pushBack(cardIcon);
        this->addChild(cardIcon, 3);
    }
}

void
RoundSelectScene::setItem(const std::string& characterTag, Vector<Sprite*>& sprites, float pos)
{
    for (auto i : sprites) {
        i->removeFromParentAndCleanup(true);
    }
    vector<Item> items = GameData::getInstance()->getCharacterActiveItems(characterTag);
    for (int i = 0; i < items.size(); ++i) {
        auto itemIcon = Sprite::create(items.at(i).icon);
        itemIcon->setPosition(
            Vec2(_visibleSize.width * (pos + 0.05 * i), _visibleSize.height * 0.15));
        itemIcon->setScale(0.8);
        sprites.pushBack(itemIcon);
        this->addChild(itemIcon, 3);
    }
}
