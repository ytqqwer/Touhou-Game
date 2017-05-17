#include "GameData/impl/Save.h"
#include "GameData/Character.h"
#include "GameData/Conversation.h"
#include "GameData/Item.h"
#include "GameData/Round.h"
#include "GameData/Location.h"
#include "GameData/SpellCard.h"

using namespace GameDataImpl;

Save* Save::_self;

Save*
Save::getInstance()
{
    if (!_self) {
        _self = new (std::nothrow) Save();
        if (!(_self && _self->init())) {
            delete _self;
            _self = nullptr;
        }
    }
    return _self;
}

bool
Save::init()
{
    return true;
}
