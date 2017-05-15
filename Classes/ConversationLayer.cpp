#include "ConversationLayer.h"
#include "PlaceHolder.h"
// #include "resources.h.dir/conversation_layer.h"

ConversationLayer::ConversationLayer()
{
    _visibleSize = _director->getVisibleSize();
}

Scene*
ConversationLayer::createDebugScene()
{
    auto s = Scene::create();
    auto l = ConversationLayer::create();

    if (s && l) {
        s->addChild(l);
        return s;
    } else {
        return nullptr;
    };
}

bool
ConversationLayer::init()
{
    /*  1. super init first */

    if (!Layer::init()) {
        return false;
    }

    /*  2. PlaceHolder */

    auto p = PlaceHolder::createCircle(100, "ConversationLayer");
    p->setPosition(_visibleSize / 2);
    this->addChild(p);

    this->scheduleUpdate();
    return true;
}

void
ConversationLayer::update(float dt)
{
}
