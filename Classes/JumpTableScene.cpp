#include "JumpTableScene.h"
#include "ArmsStoreScene.h"
#include "BackgroundIntroScene.h"
#include "EquipScene.h"
#include "HomeScene.h"
#include "InventoryScene.h"
#include "KnowledgeBaseScene.h"
#include "KoumakanLibraryScene.h"
#include "KourindouScene.h"
#include "MainMenuScene.h"
#include "MapSelectScene.h"
#include "PlaceHolder.h"
#include "RoundSelectScene.h"
#include "SaveScene.h"
#include "StaffScene.h"

#ifdef ADD_MENU_ITEM
#undef ADD_MENU_ITEM
#endif
#define ADD_MENU_ITEM(__TYPE__)                                                                    \
    _jumpTable->addChild(MenuItemSprite::create(                                                   \
        PlaceHolder::createRect(Size(200, 100), #__TYPE__, 16),                                    \
        PlaceHolder::createRect(Size(200, 100), #__TYPE__, 16, Color3B(100, 100, 100)),            \
        [this](Ref*) {                                                                             \
            auto s = __TYPE__::create();                                                           \
            _director->replaceScene(s);                                                            \
        }));

const std::string JumpTableScene::TAG{ "JumpTableScene" };

JumpTableScene::JumpTableScene()
{
    _visibleSize = _director->getVisibleSize();
}

bool
JumpTableScene::init()
{
    /*  1. super init first */

    if (!Scene::init()) {
        return false;
    }

    /*  2. Menu */

    _jumpTable = Menu::create();
    this->addChild(_jumpTable);

    /*  3. MenuItem and Callback */

    ADD_MENU_ITEM(ArmsStoreScene)
    ADD_MENU_ITEM(BackgroundIntroScene);
    ADD_MENU_ITEM(EquipScene);
    ADD_MENU_ITEM(HomeScene);
    ADD_MENU_ITEM(InventoryScene);
    ADD_MENU_ITEM(KnowledgeBaseScene);
    ADD_MENU_ITEM(KoumakanLibraryScene);
    ADD_MENU_ITEM(KourindouScene);
    ADD_MENU_ITEM(MainMenuScene);
    ADD_MENU_ITEM(MapSelectScene);
    ADD_MENU_ITEM(RoundSelectScene);
    ADD_MENU_ITEM(SaveScene);
    ADD_MENU_ITEM(StaffScene);

    _jumpTable->alignItemsInColumns(4, 4, 4, 1, NULL);

    return true;
}
