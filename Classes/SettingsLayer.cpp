#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "SettingsLayer.h"
#include "PlaceHolder.h"
// #include "resources.h.dir/settings_layer.h"

SettingsLayer::SettingsLayer()
{
    _visibleSize = _director->getVisibleSize();
}

Scene*
SettingsLayer::createDebugScene()
{
    auto s = Scene::create();
    auto l = SettingsLayer::create();

    if (s && l) {
        s->addChild(l);
        return s;
    } else {
        return nullptr;
    };
}

bool
SettingsLayer::init()
{
    /*  1. super init first */

    if (!Layer::init()) {
        return false;
    }

    /*  2. PlaceHolder */

    auto p = PlaceHolder::createCircle(100, "SettingsLayer");
    p->setPosition(_visibleSize / 2);
    this->addChild(p);

    this->scheduleUpdate();
    return true;
}

void
SettingsLayer::update(float dt)
{
}
