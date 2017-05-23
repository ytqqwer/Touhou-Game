#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameData/impl/json.h"
#include "resources.h.dir/gamedata.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <utility>

using json = nlohmann::json;

#include "GameData.h"
#include "GameData/Character.h"
#include "GameData/Conversation.h"
#include "GameData/Item.h"
#include "GameData/Location.h"
#include "GameData/Round.h"
#include "GameData/SpellCard.h"

GameData* GameData::_self; // definition of _self

////////////////////////////////////////////////////////////////////////////////
// FILE LOCAL VARIABLES / FUNCTIONS
// 根据面向对象编程的观点，下面这些变量和函数应该的声明应该写入 GameData.h 头文件，但这会使得
// GameData.h 头文件依赖增多，体积变大。一种解决方案是让 GameData 类维护一个指向 GameDataImpl
// 的指针，将这些变量/函数放入 GameDataImpl 中，但这还需要维护一个 GameDataImpl 类。为简单起见，
// 我们采用 C 语言的特性，使用 static 变量/函数，使 变量/函数 的可见性局限于文件范围内，这样也能
// 达到封装和隐藏的目的。

static json savesDom;
static json characterListDom;
static json locationListDom;
static json conversationListDom;
static json itemListDom;
static json spellCardListDom;
static json awardListDom;

static string
getCurrentTime()
{
    char fmtTimeBuf[17];
    time_t calTime = time(NULL);
    struct tm* brokenDownTimePtr = localtime(&calTime);
    size_t bytesWrite =
        strftime(fmtTimeBuf, sizeof(fmtTimeBuf), "20%y/%m/%d %H:%M", brokenDownTimePtr);

    // char * -> string
    string fmtTimeRet;
    if (bytesWrite == 16) {
        fmtTimeBuf[16] = '\0';
        fmtTimeRet = fmtTimeBuf;
    }

    return fmtTimeRet;
}

/* 以下的 from_json 函数提供给 nlohmann::json 使用，使其能将 json 对象转成我们
 * 我们自定义的对象
 *
 * 每个函数前的注释给出了这个函数是用于哪个 DOM 哪个节点的，勿用在不匹配的 DOM 节点上
 */

// savesDom["saveList"][n] -> Save
static void
from_json(const json& j, Save& s)
{
    s.tag = j.at("tag");
    s.name = j.at("name");
    s.time = j.at("locationTag");
}

// locationListDom[n]["conversationIndicatorList"][n] -> ConversationIndicator
static void
from_json(const json& j, ConversationIndicator& i)
{
    i.conversationTag = j.at("conversationTag");
    i.icon = j.at("icon");
    i.name = j.at("name");
}

// locationListDom[n] -> Location
static void
from_json(const json& j, Location& l)
{
    l.tag = j.at("tag");
    l.name = j.at("name");
    l.wordArt = j.at("wordArt");
    l.previewPicture = j.at("previewPicture");
    l.backgroundPicture = j.at("backgroundPicture");
    l.backgroundMusic = j.at("backgroundMusic");
    l.passedRound = 0;
    l.totalRound = j.at("roundList").size();

    int curSave = savesDom["currentSaveTag"];
    const json& unlockedLocListDom = savesDom["saveList"][curSave]["unlockedLocationList"];
    for (auto const& u : unlockedLocListDom) {
        if (u.at("tag") == l.tag) {
            l.passedRound = u.at("passedRound");
            break;
        }
    }
}

/// locationListDom[n]["roundList"][n] -> Round
static void
from_json(const json& j, Round& r)
{
    r.tag = j.at("tag");
    r.name = j.at("name");
    r.previewPicture = j.at("previewPicture");
    r.description = j.at("description");

    {
        string dStr = j.at("difficulty");

        Round::Difficulty dEnum;
        if (dStr == "ONE") {
            dEnum = Round::Difficulty::ONE;
        } else if (dStr == "TWO") {
            dEnum = Round::Difficulty::TWO;
        } else if (dStr == "THREE") {
            dEnum = Round::Difficulty::THREE;
        } else if (dStr == "FOUR") {
            dEnum = Round::Difficulty::FOUR;
        } else if (dStr == "FIVE") {
            dEnum = Round::Difficulty::FIVE;
        }

        r.difficulty = dEnum;
    }
}

// savesDom["saveList"][n]["characterList"][n] -> Character
static void
from_json(const json& j, Character& c)
{
    c.tag = j.at("tag");
    c.itemSlotNum = j.at("itemSlotNum");
    c.spellCardSlotNum = j.at("spellCardSlotNum");

    json::const_iterator it;

    /* 1. characterListDom 中的信息 */

    for (it = characterListDom.begin(); it != characterListDom.end(); ++it) {
        if (it->at("tag") == c.tag) {
            break;
        }
    }

    c.name = it->at("name");
    c.avatar = it->at("avatar");
    c.portrait = it->at("protrait");

    for (auto const& p : it->at("preview")) {
        c.preview.push_back(p);
    }

    c.healthPointBase = it->at("healthPointBase");
    c.manaBase = it->at("manaBase");
    c.walkAccelerationBase = it->at("walkAccelerationBase");
    c.dashAccelerationBase = it->at("dashAccelerationBase");

    /* 2. savesDom 中的信息 */

    int curSave = savesDom.at("currentSaveTag");
    for (it = savesDom.at("saveList").at(curSave).at("characterList").begin();
         it != savesDom.at("saveList").at(curSave).at("characterList").end(); ++it) {
        if (it->at("tag").get<string>() == c.tag) {
            break;
        }
    }

    c.healthPointInc = it->at("healthPointInc");
    c.manaInc = it->at("manaInc");
    c.walkSpeedInc = it->at("walkSpeedInc");
    c.walkAccelerationInc = it->at("walkAccelerationInc");
    c.dashAccelerationInc = it->at("dashAccelerationInc");
}

// characterListDom[n]["attackList"][n] -> Character::Attack
static void
from_json(const json& j, Character::Attack& a)
{
    // TODO
}

// itemListDom[n] -> Item
static void
from_json(const json& j, Item& i)
{
    i.tag = j.at("tag");
    i.name = j.at("name");
    i.icon = j.at("icon");
    i.description = j.at("description");
    i.type = (j.at("type") == "NORMAL") ? Item::Type::NORMAL : (j.at("type") == "STRENGTHEN")
                                                                   ? Item::Type::STRENGTHEN
                                                                   : Item::Type::SPECIAL;
    i.isCarriable = j.at("isCarriable");
    i.isUseable = j.at("isUseable");

    i.maxUseCount = j.at("maxUseCount");
    i.healthPointCost = j.at("healthPointCost");
    i.manaCost = j.at("manaCost");
    i.cooldown = j.at("coolDown");

    i.healthPointInc = j.at("healthPointInc");
    i.manaInc = j.at("manaInc");
    i.walkSpeedInc = j.at("walkSpeedInc");
    i.walkAccelerationInc = j.at("walkAccelerationInc");
    i.dashAccelerationInc = j.at("dashAccelerationInc");

    i.price = j.at("price");
}

// cardListDom[n] -> SpellCard
static void
from_json(const json& j, SpellCard& c)
{
    c.tag = j.at("tag");
    c.name = j.at("name");
    c.icon = j.at("icon");
    c.description = j.at("description");
    c.manaCost = j.at("manaCost");
    c.cooldown = j.at("coolDown");
}

////////////////////////////////////////////////////////////////////////////////

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

    /* 1. 打开文件流 */

    auto fileUtil = FileUtils::getInstance();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    // 在启动 cocos2d-x 引擎前，Java Activity 已经将 assets/gamedata 拷贝至
    // writablePath/gamedata 了

    string prefix = fileUtil->getWritablePath();

    ifstream saves_json(prefix + "gamedata/saves.json");
    ifstream characters_json(prefix + "gamedata/characters.json");
    ifstream locations_json(prefix + "gamedata/locations.json");
    ifstream conversations_json(prefix + "gamedata/conversations.json");
    ifstream items_json(prefix + "gamedata/items.json");
    ifstream spell_cards_json(prefix + "gamedata/spell_cards.json");
    ifstream awards_json(prefix + "gamedata/awards.json");

#else
    ifstream saves_json(fileUtil->fullPathForFilename("gamedata/saves.json"));
    ifstream characters_json(fileUtil->fullPathForFilename("gamedata/characters.json"));
    ifstream locations_json(fileUtil->fullPathForFilename("gamedata/locations.json"));
    ifstream conversations_json(fileUtil->fullPathForFilename("gamedata/conversations.json"));
    ifstream items_json(fileUtil->fullPathForFilename("gamedata/items.json"));
    ifstream spell_cards_json(fileUtil->fullPathForFilename("gamedata/spell_cards.json"));
    ifstream awards_json(fileUtil->fullPathForFilename("gamedata/awards.json"));

#endif

    /* 2. 文件流 -> 内存/DOM */

    saves_json >> savesDom;
    characters_json >> characterListDom;
    locations_json >> locationListDom;
    conversations_json >> conversationListDom;
    items_json >> itemListDom;
    spell_cards_json >> spellCardListDom;
    awards_json >> awardListDom;

    /* 3. 关闭文件流 */

    saves_json.close();
    characters_json.close();
    locations_json.close();
    conversations_json.close();
    items_json.close();
    spell_cards_json.close();
    awards_json.close();

    return true;
}

int
GameData::getCurrentSaveTag()
{
    return savesDom["currentSaveTag"];
}

vector<Save>
GameData::getSaveList()
{
    int saveCnt = savesDom["saveList"].size();
    vector<Save> listRet;
    listRet.reserve(saveCnt);

    for (auto const& s : savesDom["saveList"]) {
        listRet.push_back(s);
    }

    return listRet;
}

void
GameData::saveSave()
{
    /* 1. 更改存档时间 */

    int curSave = savesDom["currentSaveTag"];
    savesDom["saveList"][curSave]["time"] = getCurrentTime();

    /* 2. 写入文件 */

    // TODO
}

void
GameData::deleteSave(int saveTag)
{
    /* 1. DOM 操作 */

    bool found = false;
    json::const_iterator it;

    for (it = savesDom["saveList"].begin(); it != savesDom["saveList"].end(); ++it) {
        if (it->at("tag") == saveTag) {
            found = true;
            break;
        }
    }

    if (found) {
        // savesDom["saveList"].erase(it); // 太危险，待测试
    }

    /* 2. 写入文件 */
}

void
GameData::switchSave(int saveTag)
{
    /* 1. 保存当前存档的更改 */

    // TODO

    /* 2. 切换到新存档 */

    savesDom["currentSaveTag"] = saveTag;
}

float
GameData::getConversationSpeed()
{
    return savesDom["conversationSpeed"];
}

void
GameData::setConversationSpeed(float relativeSpeed)
{
    savesDom["conversationSpeed"] = relativeSpeed;
}

vector<ConversationIndicator>
GameData::getConversationIndicatorList(const string& locationTag)
{
    /* 1. it 是指向 locationTag 所在 location 的迭代器 */

    json::const_iterator it;
    for (it = locationListDom.begin(); it != locationListDom.end(); ++it) {
        if (it->at("tag") == locationTag) {
            break;
        }
    }

    /* 2. 拷贝 it 迭代器指向的 conversationIndicatorList */

    int saveCnt = it->at("conversationIndicatorList").size();
    vector<ConversationIndicator> listRet;
    listRet.reserve(saveCnt);

    for (auto const& c : it->at("conversationIndicatorList")) {
        listRet.push_back(c);
    }

    return listRet;
}

vector<Dialogue>
GameData::getConversation(const string& conversationTag)
{
    // TODO

    return vector<Dialogue>();
}

Location
GameData::getCurrentLocation()
{
    int curSave = savesDom["currentSaveTag"];
    string locTag = savesDom["saveList"][curSave]["locationTag"];

    // find the tag in locationListDom

    json::const_iterator it;
    for (it = locationListDom.begin(); it != locationListDom.end(); ++it) {
        if (it->at("tag") == locTag) {
            break;
        }
    }

    // *it -> Location
    return *it;
}

vector<Location>
GameData::getLocationList()
{
    vector<Location> listRet;
    listRet.reserve(locationListDom.size());

    for (auto const& l : locationListDom) {
        listRet.push_back(l);
    }

    return listRet;
}

vector<string>
GameData::getUnlockedLocationTagList()
{
    int curSave = savesDom["currentSaveTag"];
    const json& unlockedLocListDom = savesDom["saveList"][curSave]["unlockedLocationList"];
    vector<string> listRet;
    listRet.reserve(unlockedLocListDom.size());

    // 返回 vector<Location> 的代码，保留以备后用
    // for (auto const& locToPred : locationListDom) {
    //     auto it = find_if(unlockedLocListDom.begin(), unlockedLocListDom.end(),
    //                       [&locToPred](const json& unlockedLoc) -> bool {
    //                           if (locToPred.at("tag") == unlockedLoc.at("tag")) {
    //                               return true;
    //                           } else {
    //                               return false;
    //                           }
    //                       });
    //     if (it != unlockedLocListDom.end()) {
    //       listRet.push_back(locToPred.)
    //         listRet.push_back(locToPred);
    //     }
    // }

    for (auto const& l : unlockedLocListDom) {
        listRet.push_back(l.at("tag"));
    }

    return listRet;
}

bool
GameData::switchLocation(const string& newLocationTag)
{
    int curSave = savesDom["currentSaveTag"];
    savesDom["saveList"][curSave]["locationTag"] = newLocationTag;
    return true;
}

vector<Round>
GameData::getRoundList(const string& locationTag)
{
    /* 1. it 是指向 locationTag 所在 location 的迭代器 */

    json::const_iterator it;
    for (it = locationListDom.begin(); it != locationListDom.end(); ++it) {
        if (it->at("tag") == locationTag) {
            break;
        }
    }

    /* 2. 拷贝 it 迭代器指向的 roundList */

    int cnt = it->at("roundList").size();
    vector<Round> listRet;
    listRet.reserve(cnt);

    for (auto const& r : it->at("roundList")) {
        listRet.push_back(r);
    }

    return listRet;
}

vector<Character>
GameData::getAvailableCharacterList()
{
    int curSave = savesDom["currentSaveTag"];
    const json& listDom = savesDom["saveList"][curSave]["characterList"];
    vector<Character> listRet;
    listRet.reserve(listDom.size());

    for (auto const& c : listDom) {
        listRet.push_back(c);
    }

    return listRet;
}

vector<string>
GameData::getOnStageCharacterTagList()
{
    int curSave = savesDom["currentSaveTag"];
    json& onStageListDom = savesDom["saveList"][curSave]["onStageCharacterList"];

    vector<string> characters;
    for (auto const& c : onStageListDom) {
        characters.push_back(c);
    }

    return characters;
}

bool
GameData::switchOnStageCharacter(int nth, const string& characterTag)
{
    int curSave = savesDom["currentSaveTag"];
    json& onStageCharList = savesDom["saveList"][curSave]["onStageCharacterList"];
    onStageCharList[nth] = characterTag;

    return true;
}

vector<Item>
GameData::getCharacterItemList(const string& characterTag)
{
    /* 1. 找到人，并取其装备 tag 列表 */

    int curSave = savesDom["currentSaveTag"];
    const json& characterListDom = savesDom["saveList"][curSave]["characterList"];

    const json* equipedItemListDomPtr;
    for (auto const& c : characterListDom) {
        if (c["tag"] == characterTag) {
            equipedItemListDomPtr = &c["equipedItemList"];
        }
    }
    const json& equipedListDom = *equipedItemListDomPtr;

    /* 2. 根据装备列表取得装备详细信息 */

    vector<Item> listRet;

    listRet.reserve(equipedListDom.size());

    for (auto const& iTagToFind : equipedListDom) {
        auto it = find_if(itemListDom.begin(), itemListDom.end(),
                          [&iTagToFind](const json& item) -> bool {
                              if (item.at("tag") == iTagToFind) {
                                  return true;
                              } else {
                                  return false;
                              }
                          });

        listRet.push_back(*it);
    }

    return listRet;
}

bool
GameData::changeItem(const string& characterTag, int slot, const string& itemTag)
{
    /* 1. 更改已装备 item 记录 */

    int curSave = savesDom["currentSaveTag"];
    json& characterListDom = savesDom["saveList"][curSave]["characterList"];

    for (auto& c : characterListDom) {
        if (c["tag"] == characterTag) {
            c["equipedItemList"][slot] = itemTag;
            break;
        }
    }

    /* 2. 处理 Inc 变化 */

    // TODO

    /* 2.1 减去旧的 */

    /* 2.2 加上新的 */

    return true;
}

vector<SpellCard>
GameData::getCharacterSpellCardList(const string& characterTag)
{
    /* 1. 找到人，并取其装备 tag 列表 */

    int curSave = savesDom["currentSaveTag"];
    const json& characterListDom = savesDom["saveList"][curSave]["characterList"];

    const json* equipedCardListDomPtr;
    for (auto const& c : characterListDom) {
        if (c["tag"] == characterTag) {
            equipedCardListDomPtr = &c["equipedSpellCardList"];
        }
    }
    const json& equipedListDom = *equipedCardListDomPtr;

    /* 2. 根据符卡列表取得装备详细信息 */

    vector<SpellCard> listRet;
    listRet.reserve(equipedListDom.size());

    for (auto const& cTagToFind : equipedListDom) {
        auto it = find_if(spellCardListDom.begin(), spellCardListDom.end(),
                          [&cTagToFind](const json& item) -> bool {
                              if (item.at("tag") == cTagToFind) {
                                  return true;
                              } else {
                                  return false;
                              }
                          });

        listRet.push_back(*it);
    }

    return listRet;
}

bool
GameData::changeSpellCard(const string& characterTag, int slot, const string& spellCardTag)
{
    int curSave = savesDom["currentSaveTag"];
    json& characterListDom = savesDom["saveList"][curSave]["characterList"];

    for (auto& c : characterListDom) {
        if (c["tag"] == characterTag) {
            c["equipedSpellCardList"][slot] = spellCardTag;
            break;
        }
    }

    return true;
}

vector<Character::Attack>
GameData::getAttackList(const string& characterTag)
{
    // TODO
    return vector<Character::Attack>();
}

bool
GameData::switchAttackType(const string& characterTag, Character::Attack::Type type)
{

    // TODO
    // 不存于 json 中

    return true;
}

long
GameData::getMoneyNum()
{
    int curSave = savesDom["currentSaveTag"];
    return savesDom["saveList"][curSave]["money"];
}

void
GameData::increaseMoney(long num)
{
    int curSave = savesDom["currentSaveTag"];
    savesDom["sveList"][curSave]["money"] += num;
}

vector<pair<Item, int>>
GameData::getAvailableItemList()
{
    int curSave = savesDom["currentSaveTag"];
    const json& availItemListDom = savesDom["saveList"][curSave]["availableItemList"];
    vector<pair<Item, int>> listRet;

    listRet.reserve(availItemListDom.size());

    for (auto const& iToPred : itemListDom) {
        auto it = find_if(availItemListDom.begin(), availItemListDom.end(),
                          [&iToPred](const json& availI) -> bool {
                              if (iToPred.at("tag") == availI.at("tag")) {
                                  return true;
                              } else {
                                  return false;
                              }
                          });

        if (it != availItemListDom.end()) {
            listRet.push_back(make_pair<Item, int>(iToPred, it->at("amount")));
        }
    }

    return listRet;
}

vector<Item>
GameData::getItemListInStore(const string& storeTag)
{
    vector<Item> listRet;
    listRet.reserve(32);

    for (auto const& iToPred : itemListDom) {
        if (iToPred.at("inStore") == storeTag) {
            listRet.push_back(iToPred);
        }
    }

    return listRet;
}

vector<pair<SpellCard, int>>
GameData::getAvailableSpellCardList()
{
    int curSave = savesDom["currentSaveTag"];
    const json& availSpellCardListDom = savesDom["saveList"][curSave]["availableSpellCardList"];
    vector<pair<SpellCard, int>> listRet;
    listRet.reserve(availSpellCardListDom.size());

    for (auto const& cToPred : spellCardListDom) {
        auto it = find_if(availSpellCardListDom.begin(), availSpellCardListDom.end(),
                          [&cToPred](const json& availC) -> bool {
                              if (cToPred.at("tag") == availC.at("tag")) {
                                  return true;
                              } else {
                                  return false;
                              }
                          });

        if (it != availSpellCardListDom.end()) {
            listRet.push_back(make_pair<SpellCard, int>(cToPred, it->at("amount")));
        }
    }

    return listRet;
}

void
GameData::buyItem(const string& itemTag)
{
    // TODO
}

void
GameData::ConversationComplete(const string& conversationTag)
{
    // TODO
}

void
GameData::RoundComplete(const string& roundTag)
{
    // TODO
}
