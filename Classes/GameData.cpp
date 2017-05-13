#include "GameData.h"
#include "GameData/CardDescription.h"
#include "GameData/CharacterDescription.h"
#include "GameData/ConversationDescription.h"
#include "GameData/MapDescription.h"
#include "GameData/PropDescription.h"
#include "GameData/SaveDescription.h"
#include "GameData/SpellDescription.h"

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

SaveDescription
GameData::getSaveDescription(int saveTag)
{
}

void
GameData::saveSave()
{
}

void
GameData::switchSave(int saveTag)
{
}
void
GameData::deleteSave(int saveTag)
{
}

string
GameData::getSceneBgpName(const string& sceneTag)
{
}

string
GameData::getSceneBgmName(const string& sceneTag)
{
}
float
GameData::getConversationSpeed()
{
}
void
GameData::setConversationSpeed(float relativeSpeed)
{
}
vector<pair<int, pair<string, string>>>
GameData::getSceneConversations(const string& sceneTag)
{
}

ConversationDescription
GameData::getConversationDescription(const string& sceneTag)
{
}

pair<string, int>
GameData::getCurrentMapAndProcessInfo()
{
}

vector<MapDescription>
GameData::getMapAndProcessInfoList()
{
}
MapDescription
GameData::getMapDescription(int mapTag)
{
}

vector<int>
GameData::getAvailableCharacters()
{
}

CharacterDescription
GameData::getCharacterShortDescription(int characterTag)
{
}

CharacterDescription
GameData::getCharacterDescription()
{
}

pair<vector<int>, vector<int>>
GameData::getCharacterEquipmentList(int characterTag)
{
}

long
GameData::getMoneyNum()
{
}

void
GameData::increaseMoney(long num)
{
}

vector<PropDescription>
GameData::getPropList()
{
}

PropDescription
GameData::getPropShortDescription(int propTag)
{
}

PropDescription
GameData::getPropDescription(int propTag)
{
}

CardDescription
GameData::getCardShortDescription(int cardTag)
{
}
CardDescription
GameData::getCardDescription(int cardTag)
{
}
SpellDescription
GameData::getSpellShortDescription(int spellTag)
{
}
SpellDescription
GameData::getSpellDescription(int spellTag)
{
}

// FAKE IMPLEMENTATION END
////////////////////////////////////////////////////////////////////////////////

