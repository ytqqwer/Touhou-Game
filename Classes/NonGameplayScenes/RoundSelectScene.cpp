#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "RoundSelectScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
#include "ui/CocosGUI.h"
// #include "resources.h.dir/main_menu.h"
#include <string>

//需要用到的头文件
#include "GameData/GameData.h"

#include "GameplayScene/GameplayScene.h"

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

    /*****************静态不会变化的板块.创建大概模型*****************/
    //创建返回按钮
    auto backButton = Button::create("roundselectscene/p1.png", "", "");
    backButton->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    backButton->setTitleText("返回");
    backButton->setTitleFontSize(20);
    backButton->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    backButton->addTouchEventListener(
        [](Ref* pSender, Widget::TouchEventType type) { Director::getInstance()->popScene(); });
    this->addChild(backButton);

    //创建游戏开始按钮,等创建好了gameScene会添加一个事件监听器
    auto beginButton = Button::create("roundselectscene/p10.png", "", "");
    beginButton->setTitleText("开始游戏");
    beginButton->setTitleFontSize(20);
    beginButton->setContentSize(Size(_visibleSize.width * 0.3, _visibleSize.height * 0.3));
    beginButton->setPosition(Vec2(_visibleSize.width * 0.9, _visibleSize.height * 0.2));
    beginButton->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::BEGAN) {
            auto gameplayScene = GameplayScene::create();
            TransitionScene* transition = TransitionPageTurn::create(1.0f, gameplayScene, false);
            Director::getInstance()->popToRootScene();
            Director::getInstance()->replaceScene(transition);
        }
    });
    this->addChild(beginButton, 2);

    /***********************创建会变化的模块********************/
    //取得当前所在的地点和回合，gameDate，以及角色,以及当前可以使用的所有角色
    Location location = GameData::getInstance()->getCurrentLocation();
    vector<Round> round = GameData::getInstance()->getRoundList(location.tag);
    vector<string> character = GameData::getInstance()->getOnStageCharacterTagList();
    vector<Character> characters = GameData::getInstance()->getAvailableCharacterList();

    //创建背景
    auto backGround = Sprite::create(location.backgroundPicture);
    backGround->setContentSize(Size(_visibleSize.width, _visibleSize.height));
    backGround->setPosition(Vec2(_visibleSize.width * 0.5, _visibleSize.height * 0.5));
    this->addChild(backGround, -1);

    auto backGround1 = Sprite::create("roundselectscene/layer_1.png");
    backGround1->setContentSize(Size(_visibleSize.width * 0.6, _visibleSize.height * 0.25));
    backGround1->setPosition(Vec2(_visibleSize.width * 0.65, _visibleSize.height * 0.7));
    this->addChild(backGround1);

    auto backGround2 = Sprite::create("roundselectscene/layer_2.png");
    backGround2->setContentSize(Size(_visibleSize.width * 0.6, _visibleSize.height * 0.5));
    backGround2->setPosition(Vec2(_visibleSize.width * 0.65, _visibleSize.height * 0.3));
    this->addChild(backGround2);
    //创建当前场景的场景名字的艺术字
    auto sceneName = Sprite::create(location.wordArt);
    setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    sceneName->setPosition(Vec2(_visibleSize.width * 0.2, _visibleSize.height * 0.8));
    this->addChild(sceneName);

    /******创建关卡描述的部分，对于不同的关卡，只是用set函数来修改其中的图片或者文字，不重复创建，默认为第一个关卡***********/
    //获得当前关卡的预览图片
    auto preView = Sprite::create(round[0].previewPicture);
    preView->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
    preView->setPosition(Vec2(_visibleSize.width * 0.5, _visibleSize.height * 0.7));
    this->addChild(preView);
    //难度字体
    auto difficultText = Text::create("难度", "Arial", 24);
    difficultText->setPosition(Vec2(Vec2(_visibleSize.width * 0.65, _visibleSize.height * 0.75)));
    difficultText->setFontSize(25);
    // difficultText->setContentSize(Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
    this->addChild(difficultText);

    //设置5颗空星，表示难度，然后根据不同的回合难度换成相应的实心
    Sprite* difficult[5];
    for (int i = 0; i < 5; ++i) {
        difficult[i] = Sprite::create("roundselectscene/star_2.png");
        difficult[i]->setContentSize(Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
        // difficult[i]->setScale(0.1);
        difficult[i]->setPosition(
            Vec2(_visibleSize.width * (0.7 + 0.05 * i), _visibleSize.height * 0.75));
        this->addChild(difficult[i]);
    }

    //根据当前难度，切换相应的星形 ,获得当前关卡的难度，并显示出来
    int difficulty(static_cast<int>(round[0].difficulty));
    for (int i = 0; i < difficulty; ++i) {
        difficult[i]->setTexture("roundselectscene/star_1.png");
        // difficult[i]->setScale(0.1);
        difficult[i]->setContentSize(Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
    }

    //获得当前关卡的描述
    auto text = Text::create(round[0].description, "Arial", 24);
    text->setPosition(Vec2(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.65)));
    text->setContentSize(Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
    this->addChild(text);

    /*************创建当前地点的关卡选择,循环创建，不固定,同时设置关卡标签的监听事件，改变上面的固定的值******************/
    for (unsigned int i = 0; i < location.totalRound; ++i) {
        if (i < location.passedRound - 1) { //对于已经通关的关卡的关卡
            auto roundButton = Button::create("roundselectscene/forbid_button.png", "", "");
            roundButton->setPosition(
                Vec2(_visibleSize.width * 0.2, _visibleSize.height * (0.8 - 0.15 * (i + 1))));
            roundButton->setTitleText(round[i].name);
            roundButton->setTitleFontSize(25);
            roundButton->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
                //根据点击的按钮设置当前的关卡难度，同时将当前选择的关卡保存，接口尚未实现，以后添加
                int difficulty(static_cast<int>(round[i].difficulty));
                for (int i = 0; i < difficulty; ++i) {
                    difficult[i]->setTexture("roundselectscene/star_1.png");
                    difficult[i]->setContentSize(
                        Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
                }
                for (int i = difficulty; i < 5; ++i) {
                    difficult[i]->setTexture("roundselectscene/star_2.png");
                    difficult[i]->setContentSize(
                        Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
                }
                preView->setTexture(round[i].previewPicture);
                preView->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
                text->setText(round[i].description);
                GameData::getInstance()->setRoundToPlay(round[i].tag);
            });
            this->addChild(roundButton);
        }

        if (i == location.passedRound - 1) { //对于可以打，但还没通关的关卡
            auto roundButton = Button::create("roundselectscene/forbid_button.png", "", "");
            roundButton->setPosition(
                Vec2(_visibleSize.width * 0.2, _visibleSize.height * (0.8 - 0.15 * (i + 1))));
            roundButton->setTitleText(round[i].name);
            roundButton->setTitleFontSize(25);
            roundButton->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
                //根据点击的按钮设置当前的关卡难度，同时将当前选择的关卡保存，接口尚未实现，以后添加
                int difficulty(static_cast<int>(round[i].difficulty));
                for (int i = 0; i < difficulty; ++i) {
                    difficult[i]->setTexture("roundselectscene/star_1.png");
                    difficult[i]->setContentSize(
                        Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
                }
                for (int i = difficulty; i < 5; ++i) {
                    difficult[i]->setTexture("roundselectscene/star_2.png");
                    difficult[i]->setContentSize(
                        Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
                }
                preView->setTexture(round[i].previewPicture);
                preView->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
                text->setText(round[i].description);
                GameData::getInstance()->setRoundToPlay(round[i].tag);
            });
            this->addChild(roundButton);
        }

        if (i >= location.passedRound) { //对于没有通关的关卡
            auto roundButton = Button::create("roundselectscene/forbid_button.png", "", "");
            roundButton->setPosition(
                Vec2(_visibleSize.width * 0.2, _visibleSize.height * (0.8 - 0.15 * (i + 1))));
            roundButton->setTitleText(round[i].name);
            roundButton->setTitleFontSize(25);
            roundButton->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
                //根据点击的按钮设置当前的关卡难度
                int difficulty(static_cast<int>(round[i].difficulty));
                for (int i = 0; i < difficulty; ++i) {
                    difficult[i]->setTexture("roundselectscene/star_1.png");
                    difficult[i]->setContentSize(
                        Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
                }
                for (int i = difficulty; i < 5; ++i) {
                    difficult[i]->setTexture("roundselectscene/star_2.png");
                    difficult[i]->setContentSize(
                        Size(_visibleSize.width * 0.05, _visibleSize.height * 0.05));
                }
                preView->setTexture(round[i].previewPicture);
                preView->setContentSize(Size(_visibleSize.width * 0.2, _visibleSize.height * 0.2));
                text->setText(round[i].description);
            });
            this->addChild(roundButton);
        }
    }
    //当前场景人物的描述
    for (int i = 0; i < 2; ++i) { //背景
        auto bottom = Sprite::create("roundselectscene/layer_3.png");
        bottom->setPosition(Vec2(_visibleSize.width * (0.5 + 0.2 * i), _visibleSize.height * 0.3));
        bottom->setContentSize(Size(_visibleSize.width * 0.19, _visibleSize.height * 0.4));
        this->addChild(bottom);

        //人物,以及人物信息
        for (unsigned int j = 0; j < characters.size(); ++j) {
            if (characters.at(j).tag == character[i]) {
                auto sprite = Sprite::create(characters.at(j).portrait);
                // sprite->setContentSize(Size(_visibleSize.width * 0.1, _visibleSize.height *
                // 0.15));
                sprite->setScale(0.2);
                sprite->setPosition(
                    Vec2(_visibleSize.width * (0.5 + 0.2 * i), _visibleSize.height * 0.4));
                this->addChild(sprite, 3);

                //设置该角色的符卡
                vector<SpellCard> card =
                    GameData::getInstance()->getCharacterSpellCardList(characters.at(j).tag);
                Sprite* cardSprite[3];
                for (int k = 0; k < card.size(); ++k) {
                    cardSprite[k] = Sprite::create(card.at(k).icon);
                    cardSprite[k]->setPosition(
                        Vec2(_visibleSize.width * (0.45 + 0.2 * i), _visibleSize.height * 0.25));
                    /*cardSprite[k]->setContentSize(Size(_visibleSize.width*0.06,
            _visibleSize.height*0.06));*/
                    cardSprite[k]->setScale(0.8);
                    this->addChild(cardSprite[k], 3);
                }
                //设置角色的道具
                vector<Item> item =
                    GameData::getInstance()->getCharacterItemList(characters.at(j).tag);
                Sprite* itemSprite[3];
                for (int k = 0; k < item.size(); ++k) {
                    itemSprite[k] = Sprite::create(item.at(k).icon);
                    itemSprite[k]->setPosition(
                        Vec2(_visibleSize.width * (0.45 + 0.2 * i), _visibleSize.height * 0.15));
                    /*itemSprite[k]->setContentSize(Size(_visibleSize.width*0.06,
            _visibleSize.height*0.06));*/
                    itemSprite[k]->setScale(0.8);
                    this->addChild(itemSprite[k], 3);
                }

                //角色的修改按钮
                auto button = Button::create("roundselectscene/right.png");
                button->setPosition(
                    Vec2(_visibleSize.width * (0.43 + 0.2 * i), _visibleSize.height * 0.4));
                button->addTouchEventListener([&](Ref* pSender, Widget::TouchEventType type) {
                    GameData::getInstance()->switchOnStageCharacter(j, character[i]);
                    sprite->setTexture(characters.at((j + 1) / characters.size()).portrait);
                    sprite->setContentSize(
                        Size(_visibleSize.width * 0.1, _visibleSize.height * 0.15));
                    //修改符卡
                    vector<SpellCard> card = GameData::getInstance()->getCharacterSpellCardList(
                        characters.at((j + 1) / characters.size()).tag);
                    for (int k = 0; k < card.size(); ++k) {
                        cardSprite[k]->setTexture(card.at(k).icon);
                        cardSprite[k]->setContentSize(
                            Size(_visibleSize.width * 0.06, _visibleSize.height * 0.06));
                    }
                    //修改道具
                    vector<Item> item = GameData::getInstance()->getCharacterItemList(
                        characters.at((j + 1) / characters.size()).tag);
                    for (int k = 0; k < card.size(); ++k) {
                        itemSprite[k]->setTexture(item.at(k).icon);
                        itemSprite[k]->setContentSize(
                            Size(_visibleSize.width * 0.06, _visibleSize.height * 0.06));
                    }
                });
                this->addChild(button, 3);
            }
        }
    }
}

void
RoundSelectScene::onExit()
{
    Scene::onExit();
    this->removeAllChildren();
}

void
RoundSelectScene::update(float dt)
{
}