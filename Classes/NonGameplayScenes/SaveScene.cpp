#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "SimpleAudioEngine.h"
#include "NonGameplayScenes/SaveScene.h"
#include "NonGameplayScenes/MainMenuScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
// #include "resources.h.dir/save.h"

#include "ui/CocosGUI.h"
using namespace ui;
using namespace CocosDenshion;

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string SaveScene::TAG{ "SaveScene" };

SaveScene::SaveScene(bool isSaveAction)
{
    _isSaveAction = isSaveAction;
    _visibleSize = _director->getVisibleSize();
}

Scene*
SaveScene::create(bool isSaveAction)
{
    /*  1 if found cache */

    auto cached = NonGameplayScenesCache::getInstance()->getScene(TAG);
    if (cached)
        return cached;

    /*  2 if not found cache */

    auto pRet = new (std::nothrow) SaveScene(isSaveAction);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        NonGameplayScenesCache::getInstance()->addScene(TAG, pRet);
        return pRet;
    } else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool
SaveScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("SaveScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    /*  3. PlaceHolder */

    auto p = PlaceHolder::createCircle(100, "SaveScene");
    p->setPosition(_visibleSize / 2);
    this->addChild(p);

    auto BackButton = Button::create();
    BackButton->setTitleText("返回");
    BackButton->setTitleFontName("fonts/dengxian.ttf");
    BackButton->setTitleColor(Color3B(194, 134, 11));
    BackButton->setTitleFontSize(50);
    BackButton->setAnchorPoint(Vec2(0, 0));
    BackButton->setPosition(Vec2(_visibleSize.width * 0.8, _visibleSize.height * 0.43));
    BackButton->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
			SimpleAudioEngine::getInstance()->playEffect("back_click.wav");
			Director::getInstance()->popScene();
        }
    });
    addChild(BackButton);

    return true;
}
