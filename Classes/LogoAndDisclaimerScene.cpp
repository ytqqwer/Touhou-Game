#include "LogoAndDisclaimerScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
#include "resources.h.dir/logo.h"
#include <string>

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string LogoAndDisclaimerScene::TAG{ "LogoAndDisclaimerScene" };

LogoAndDisclaimerScene::LogoAndDisclaimerScene()
{
    _visibleSize = _director->getVisibleSize();
}

bool
LogoAndDisclaimerScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("LogoAndDisclaimerScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    /*  3. PlaceHolder */

    auto p = PlaceHolder::createCircle(100, "LogoAndDisclaimerScene");
    p->setPosition(_visibleSize / 2);
    this->addChild(p);

    return true;
}

void
LogoAndDisclaimerScene::update(float dt)
{
    /*  1. 现在是否该切换到下一个场景？ */

    if (switchToJumpTable) {
        switchToJumpTableScene();
    }
}

void
LogoAndDisclaimerScene::switchToJumpTableScene()
{
    // auto s = JumpTableScene::create();
    // _director->replaceScene(s);
}
