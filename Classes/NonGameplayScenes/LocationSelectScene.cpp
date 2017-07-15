#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "LocationSelectScene.h"
#include "ArmsStoreScene.h"
#include "EquipScene.h"
#include "HomeScene.h"
#include "InventoryScene.h"
#include "KoumakanLibraryScene.h"
#include "KourindouScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
#include "RoundSelectScene.h"
// #include "resources.h.dir/map_select.h"
#include <string>

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string LocationSelectScene::TAG{ "LocationSelectScene" };

LocationSelectScene::LocationSelectScene()
{
    gamedata = GameData::getInstance();
    _visibleSize = _director->getVisibleSize();
}

Sprite*
LocationSelectScene::makeLocationOption(const Size& size, const std::string& wordArt,
                                        const std::string& previewPicture, int passedRound,
                                        int totalRound)
{
    auto WordArt = Sprite::create(wordArt);
    WordArt->setScale(0.7);
    auto progress = Label::createWithTTF(to_string(passedRound) + "/" + to_string(totalRound),
                                         "fonts/dengxian.ttf", 16);

    auto PreviewPicture = Sprite::create(previewPicture);
    PreviewPicture->setContentSize(size);
    PreviewPicture->addChild(WordArt);
    PreviewPicture->addChild(progress, 1);
    WordArt->setPosition(Vec2(PreviewPicture->getContentSize().width * 0.7,
                              PreviewPicture->getContentSize().height * 0.7));
    progress->setPosition(Vec2(PreviewPicture->getContentSize().width * 0.5,
                               PreviewPicture->getContentSize().height * 0.3));
    return PreviewPicture;
}

void
LocationSelectScene::menuItemCallback()
{
}

bool
LocationSelectScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

    auto under = Sprite::create("location_select/under.png");
    under->setPosition(_visibleSize / 2);
    addChild(under, 1);

    auto up = Sprite::create("location_select/up.png");
    up->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height * 0.55));
    addChild(up, 2);

    /*返回*/
    auto ret = Button::create("location_select/back.png");
    ret->setTitleColor(Color3B(0, 0, 0));
    ret->setTitleFontSize(30);
    ret->setAnchorPoint(Vec2(0, 1));
    ret->setPosition(Vec2(_visibleSize.width * 0.1, _visibleSize.height * 0.2));
    ret->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->popScene();
        }
    });
    addChild(ret, 2);

    /*特殊地点   红魔馆大图书馆*/
    auto koumakanLibrary = Button::create("location_select/KoumakanLibrary.png");
    koumakanLibrary->setTitleColor(Color3B(0, 0, 0));
    koumakanLibrary->setTitleFontSize(30);
    koumakanLibrary->setAnchorPoint(Vec2(0, 1));
    koumakanLibrary->setPosition(Vec2(_visibleSize.width * 0.35, _visibleSize.height * 0.2));
    koumakanLibrary->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->pushScene(KoumakanLibraryScene::create());
        }
    });
    addChild(koumakanLibrary, 2);

    /*特殊地点   香霖堂*/
    auto Kourindou = Button::create("location_select/Kourindou.png");
    Kourindou->setTitleColor(Color3B(0, 0, 0));
    Kourindou->setTitleFontSize(30);
    Kourindou->setAnchorPoint(Vec2(0, 1));
    Kourindou->setPosition(Vec2(_visibleSize.width * 0.55, _visibleSize.height * 0.2));
    Kourindou->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->pushScene(KourindouScene::create());
        }
    });
    addChild(Kourindou, 2);

    /*特殊地点   军火店*/
    auto ArmsStore = Button::create("location_select/ArmsStore.png");
    ArmsStore->setTitleColor(Color3B(0, 0, 0));
    ArmsStore->setTitleFontSize(30);
    ArmsStore->setAnchorPoint(Vec2(0, 1));
    ArmsStore->setPosition(Vec2(_visibleSize.width * 0.75, _visibleSize.height * 0.2));
    ArmsStore->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->pushScene(ArmsStoreScene::create());
        }
    });
    addChild(ArmsStore, 2);

    LocationList = Menu::create();
    LocationList->setPosition(Vec2(_visibleSize.width * 0.5, _visibleSize.height * 0.6));
    this->addChild(LocationList, 4);

    vector<Location> locations = gamedata->getLocationList();

    for (int i = 0; i < locations.size(); i++) {

        auto PreviewPicture =
            makeLocationOption(Size(200, 100), locations[i].wordArt, locations[i].previewPicture,
                               locations[i].passedRound, locations[i].totalRound);
        // log(">> " + locations[i].passedRound);
        // log(">> " + locations[i].totalRound);
        auto location = MenuItemSprite::create(PreviewPicture, PreviewPicture, PreviewPicture,
                                               [this, locations, i](Ref*) {
                                                   gamedata->switchLocation(locations[i].tag);

                                                   _director->popScene();
                                               });
        LocationList->addChild(location, 4);
    }
    LocationList->alignItemsInColumns(3, 3, 1, NULL);

    this->scheduleUpdate();

    return true;
}

void
LocationSelectScene::onEnter()
{
    Scene::onEnter();

    //获取目前地点
    auto currentlocation = gamedata->getCurrentLocation();

    //背景  使用目前地点的背景
    auto bg = Sprite::create(currentlocation.backgroundPicture);
    bg->setContentSize(_visibleSize);
    bg->setPosition(_visibleSize / 2);
    addChild(bg, 0);
}
void
LocationSelectScene::update(float dt)
{
}