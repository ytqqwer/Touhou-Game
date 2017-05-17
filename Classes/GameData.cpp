#include "GameData.h"
#include "GameData/Character.h"
#include "GameData/Conversation.h"
#include "GameData/Item.h"
#include "GameData/Location.h"
#include "GameData/Round.h"
#include "GameData/SpellCard.h"
#include "GameData/impl/Save.h"
#include "resources.h.dir/gamedata.h"

GameData* GameData::_self; // definition of _self

GameData::GameData()
{
    // TODO
    // GameData 内存对象初始化
}

GameData*
GameData::getInstance()
{
    if (!_self) {
        // 这个对象并不大，而且会一直存在，不需要 delete 它
        _self = new (std::nothrow) GameData();

        if (!(_self && _self->init())) {
            delete _self;
            _self = nullptr;
        }
    }

    return _self;
}

bool
GameData::init()
{

    FAKE_INIT();

    // TODO
    // GameData 逻辑初始化

    return true;
}
////////////////////////////////////////////////////////////////////////////////
// FAKE IMPLEMENTATION BEGIN
// 请按照在 GameData.h 中的声明顺序排列

void
GameData::FAKE_INIT()
{
}

int
GameData::getCurrentSaveTag()
{
    return 1;
}

void
GameData::saveSave()
{
}

void
GameData::deleteSave(const string& saveTag)
{
}

void
GameData::switchSave(const string& saveTag)
{
}

float
GameData::getConversationSpeed()
{
    return 0.0f;
}

void
GameData::setConversationSpeed(float relativeSpeed)
{
}

vector<Dialogue>
GameData::getConversation(const string& conversationTag)
{
    return vector<Dialogue>();
}

Location
GameData::getCurrentLocation()
{
    return Location();
}

vector<Location>
GameData::getLocationList()
{
    return vector<Location>();
}

Location
GameData::getLocation(const string& LocationTag)
{
    return Location();
}

Round
GameData::getRound(const string& RoundTag)
{
    return Round();
}

vector<Character>
GameData::getAvailableCharacters()
{
    return vector<Character>();
}

bool
GameData::switchCharacter(const string& characterTag)
{
    return false;
}

Character
GameData::getCharacter(const string& characterTag)
{
    return Character();
}

vector<Item>
GameData::getCharacterItemList(const string& characterTag)
{
    return vector<Item>();
}

bool
GameData::changeItem(const string& itemTag, int slot, const string& characterTag)
{
    return false;
}

vector<SpellCard>
GameData::getCharacterSpellCardList(const string& characterTag)
{
    return vector<SpellCard>();
}

bool
GameData::changeSpellCard(const string& spellCardTag, int slot, const string& characterTag)
{
    return false;
}

vector<Character::Attack>
GameData::getAttackTypeList(const string& characterTag)
{
    return vector<Character::Attack>();
}

bool
GameData::switchAttackType(Character::Attack::Type type, const string& characterTag)
{
    return false;
}

long
GameData::getMoneyNum()
{
    return 0;
}

void
GameData::increaseMoney(long num)
{
}

vector<Item>
GameData::getItemList()
{
    return vector<Item>();
}

Item
GameData::getItem(const string& itemTag)
{
    return Item();
}

vector<SpellCard>
GameData::getSpellCardList()
{
    return vector<SpellCard>();
}

SpellCard
GameData::getSpellCard(const string& spellcardTag)
{
    return SpellCard();
}

// FAKE IMPLEMENTATION END
////////////////////////////////////////////////////////////////////////////////
