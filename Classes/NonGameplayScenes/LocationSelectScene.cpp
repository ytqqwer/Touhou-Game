#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "NonGameplayScenes/LocationSelectScene.h"
#include "NonGameplayScenes/ArmsStoreScene.h"
#include "NonGameplayScenes/HomeScene.h"
#include "NonGameplayScenes/KoumakanLibraryScene.h"
#include "NonGameplayScenes/KourindouScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"

#include "GameData/Location.h"

// #include "resources.h.dir/map_select.h"

#include "AudioController.h"

#include "ui/CocosGUI.h"
using namespace ui;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string LocationSelectScene::TAG{ "LocationSelectScene" };

LocationSelectScene::LocationSelectScene()
{
    gamedata = GameData::getInstance();
    _visibleSize = _director->getVisibleSize();
}

bool
LocationSelectScene::init()
{
    if (!Scene::init()) {
        return false;
    }

    /*背景*/
    backGround = Sprite::create();
    backGround->setPosition(_visibleSize / 2);
    addChild(backGround);

    auto under = Sprite::create("location_select/under.png");
    under->setPosition(_visibleSize / 2);
    addChild(under);

    auto up = Sprite::create("location_select/up.png");
    up->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height * 0.55));
    addChild(up);

    /*返回*/
    auto ret = Button::create("homescene/p1.png");
    ret->setTitleFontName("fonts/dengxian.ttf");
    ret->setTitleText("返回");
    ret->setTitleFontSize(30);
    ret->setAnchorPoint(Vec2(0, 1));
    ret->setPosition(Vec2(_visibleSize.width * 0.1, _visibleSize.height * 0.2));
    ret->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playReturnButtonEffect();
            Director::getInstance()->popScene();
        }
    });
    addChild(ret, 2);

    /*特殊地点   红魔馆大图书馆*/
    auto koumakanLibrary = Button::create("homescene/p1.png");
    koumakanLibrary->setTitleFontName("fonts/dengxian.ttf");
    koumakanLibrary->setTitleText("红魔馆大图书馆");
    koumakanLibrary->setTitleFontSize(30);
    koumakanLibrary->setAnchorPoint(Vec2(0, 1));
    koumakanLibrary->setPosition(Vec2(_visibleSize.width * 0.35, _visibleSize.height * 0.2));
    koumakanLibrary->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            GameData::getInstance()->switchLocation("KoumakanLibrary");
            TransitionScene* transition =
                TransitionFade::create(0.5f, KoumakanLibraryScene::create());
            Director::getInstance()->popToRootScene();
            Director::getInstance()->replaceScene(transition);
        }
    });
    addChild(koumakanLibrary, 2);

    /*特殊地点   香霖堂*/
    auto Kourindou = Button::create("homescene/p1.png");
    Kourindou->setTitleFontName("fonts/dengxian.ttf");
    Kourindou->setTitleText("香霖堂");
    Kourindou->setTitleFontSize(30);
    Kourindou->setAnchorPoint(Vec2(0, 1));
    Kourindou->setPosition(Vec2(_visibleSize.width * 0.55, _visibleSize.height * 0.2));
    Kourindou->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            GameData::getInstance()->switchLocation("Kourindou");
            TransitionScene* transition = TransitionFade::create(0.5f, KourindouScene::create());
            Director::getInstance()->popToRootScene();
            Director::getInstance()->replaceScene(transition);
        }
    });
    addChild(Kourindou, 2);

    /*特殊地点   军火店*/
    auto ArmsStore = Button::create("homescene/p1.png");
    ArmsStore->setTitleFontName("fonts/dengxian.ttf");
    ArmsStore->setTitleText("夜雀烤鳗店");
    ArmsStore->setTitleFontSize(30);
    ArmsStore->setAnchorPoint(Vec2(0, 1));
    ArmsStore->setPosition(Vec2(_visibleSize.width * 0.75, _visibleSize.height * 0.2));
    ArmsStore->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            AudioController::getInstance()->playClickButtonEffect();
            GameData::getInstance()->switchLocation("ArmsStore");
            TransitionScene* transition = TransitionFade::create(0.5f, ArmsStoreScene::create());
            Director::getInstance()->popToRootScene();
            Director::getInstance()->replaceScene(transition);
        }
    });
    addChild(ArmsStore, 2);

    return true;
}

void
LocationSelectScene::onEnter()
{
    Scene::onEnter();

    //获取目前地点
    auto currentlocation = gamedata->getCurrentLocation();

    //背景  使用目前地点的背景
    backGround->setTexture(currentlocation.backgroundPicture);
    backGround->setContentSize(_visibleSize);

    //重新获取全部地点
    locations = gamedata->getUnlockedLocationList();
    //排除特例
    vector<Location> locationsExclude;
    for (int i = 0; i < locations.size(); i++) {
        if (locations[i].tag == "Kourindou" || locations[i].tag == "KoumakanLibrary" ||
            locations[i].tag == "ArmsStore") {
            continue;
        }
        locationsExclude.push_back(locations[i]);
    }
    locations = locationsExclude;
    locationsExclude.clear();

    //重新生成地点选择菜单
    LocationList = Menu::create();
    LocationList->setPosition(Vec2(_visibleSize.width * 0.5, _visibleSize.height * 0.6));
    for (int i = 0; i < locations.size(); i++) {
        auto menuItem = SelectLocationMenuItem::create(locations[i]);
        LocationList->addChild(menuItem, 8);
    }
    //动态排列
    ValueVector rows;
    auto _size = locations.size();
    int _columnsInRow = 4;
    while (_size) {
        if (_size > _columnsInRow) {
            rows.push_back(Value(_columnsInRow));
            _size -= _columnsInRow;
        } else {
            rows.push_back(Value(_size));
            _size = 0;
        }
    }
    LocationList->alignItemsInColumnsWithArray(rows);

    this->addChild(LocationList, 4);
}

void
LocationSelectScene::onExit()
{
    Scene::onExit();
    locations.clear();
    LocationList->removeAllChildrenWithCleanup(true);
    LocationList->removeFromParentAndCleanup(true);
}

SelectLocationMenuItem*
SelectLocationMenuItem::create(const Location& i)
{
    SelectLocationMenuItem* pRet;
    pRet = new (std::nothrow) SelectLocationMenuItem(i);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

SelectLocationMenuItem::SelectLocationMenuItem(const Location& i)
{
    location = i;
}

bool
SelectLocationMenuItem::init()
{
    if (!MenuItemSprite::init())
        return false;

    auto WordArt = Sprite::create(location.wordArt);
    WordArt->setScale(0.7);
    auto progress =
        Label::createWithTTF(to_string(location.passedRound) + "/" + to_string(location.totalRound),
                             "fonts/dengxian.ttf", 16);

    auto PreviewPicture = Sprite::create(location.previewPicture);
    PreviewPicture->setContentSize(Size(200, 100));
    PreviewPicture->addChild(WordArt);
    PreviewPicture->addChild(progress, 1);
    WordArt->setPosition(Vec2(PreviewPicture->getContentSize().width * 0.7,
                              PreviewPicture->getContentSize().height * 0.7));
    progress->setPosition(Vec2(PreviewPicture->getContentSize().width * 0.5,
                               PreviewPicture->getContentSize().height * 0.3));

    this->setNormalImage(PreviewPicture);
    this->setSelectedImage(PreviewPicture);

    this->initWithCallback(CC_CALLBACK_0(SelectLocationMenuItem::callBack, this));

    return true;
}

void
SelectLocationMenuItem::callBack()
{
    AudioController::getInstance()->playClickButtonEffect();
    GameData::getInstance()->switchLocation(location.tag);
    TransitionScene* transition = TransitionFade::create(0.5f, HomeScene::create());
    Director::getInstance()->popToRootScene();
    Director::getInstance()->replaceScene(transition);
}
