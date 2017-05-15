#include "KnowledgeBaseScene.h"
#include "NonGameplayScenesCache.h"
#include "PlaceHolder.h"
// #include "resources.h.dir/knowledge_base.h"
#include <string>

// 静态数据成员必须在类定义 *外* 进行初始化
// 为保证编译时静态数据成员最后只存在于一个目标文件中
// 这个定义也不能写入 .h 文件中，放在对应的 .cpp 文件的开头是最好选择
const std::string KnowledgeBaseScene::TAG{ "KnowledgeBaseScene" };

KnowledgeBaseScene::KnowledgeBaseScene()
{
    _visibleSize = _director->getVisibleSize();
}

bool
KnowledgeBaseScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

/*  2. debug info */

#ifndef NDEBUG
    auto sceneTag = Label::createWithTTF("KnowledgeBaseScene", "fonts/arial.ttf", 16);
    sceneTag->setAnchorPoint(Vec2(0, 1));
    sceneTag->setPosition(Vec2(0, _visibleSize.height));
    sceneTag->setColor(Color3B::WHITE);
    this->addChild(sceneTag);
#endif

    /*  3. PlaceHolder */

    auto p = PlaceHolder::createCircle(100, "KnowledgeBaseScene");
    p->setPosition(_visibleSize / 2);
    this->addChild(p);

    /*  4. schedule */

    this->scheduleUpdate();

    return true;
}

void
KnowledgeBaseScene::update(float dt)
{
}
