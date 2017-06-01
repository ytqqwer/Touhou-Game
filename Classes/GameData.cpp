#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

/*
 *  这个文件实现 GameData.h 中定义的数据接口。实现方式很不 elegant ;)，因为我们在其中混杂
 *
 * 在实现 GameData 的接口时我们要用到一些辅助函数/变量。为了不污染 GameData.h 接口定义头文件，
 * 并没有将这些函数/变量的声明放入 GameData 类定义中，而是将这些辅助函数做成了【scope 和
 * linkage 均为文件级的 static 函数】放入了本文件中。当然，也有接口继承这种面向对象的方法
 * 来解决这个问题。但是那种实现有一定的复杂性。当实现的复杂性超过面向对象方法的复杂性时，
 * 我们会考虑使用面向对象方法来做。
 *
 * --- 本文件自上而下由三部分组成 ---
 *
 *   1. 用于辅助 GameData 接口实现的 static 函数/变量
 *      - 其中包括提供给 nolmann::json 使用，将 json 节点转为自定义类对象的 from_json 函数。
 *        为了实现的方便，这些 from_json 函数不仅仅是数据的简单转换，也包含了游戏逻辑
 *       （暂时找不到 elegant 的方法来分离），会用到那些辅助 GameData 实现的 static 变量，
 *        很难将其作成单独的编译单元
 *
 *   2. GameData 接口的实现
 *      - 对文件的操作
 *      - 对第 1 部分所定义的 json DOM 树的操作
 *
 *   3. 开发阶段的特殊初始化和测试 GameData 的 InDevelop 函数以及其子函数
 *      - 这些函数有会用到 GameData 内部实现的具体设施，不能作成单独的编译单元
 */

#include "GameData.h"
#include "GameData/Character.h"
#include "GameData/Conversation.h"
#include "GameData/Item.h"
#include "GameData/Location.h"
#include "GameData/Round.h"
#include "GameData/SpellCard.h"

#include "GameData/impl/json.h"
using json = nlohmann::json;

#include <ctime>
#include <fstream>
#include <iostream>
#include <utility>

GameData* GameData::_self; // definition of _self

////////////////////////////////////////////////////////////////////////////////
// 第一部分：用于辅助 GameData 接口实现的 static 函数/变量

static json savesDom;
static json characterListDom;
static json locationListDom;
static json conversationListDom;
static json itemListDom;
static json spellCardListDom;
static json awardListDom;

// 用于支持玩 A 存档时，想存至 B 存档上
// 我们需要记录更改到其他地方，而不能直接记录到 A 上
static json cachedSave;

// 在开发阶段使用，用于特殊初始化 GameData 和测试 GameData
// 此处是函数声明，定义在此文件的最后
static void inDevelop();

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

static void
syncSaveChangesToFile()
{
    // TODO
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
    s.time = j.at("time");
    s.locationTag = j.at("locationTag");
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

    const json& unlockedLocListDom = cachedSave["unlockedLocationList"];

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
    c.rectAvatar = it->at("rectAvatar");
    c.circularAvatar = it->at("circularAvatar");
    c.portrait = it->at("protrait");

    for (auto const& p : it->at("preview")) {
        c.preview.push_back(p);
    }

    c.healthPointBase = it->at("healthPointBase");
    c.manaBase = it->at("manaBase");
    c.walkAccelerationBase = it->at("walkAccelerationBase");
    c.dashAccelerationBase = it->at("dashAccelerationBase");

    /* 2. savesDom 中的信息 */

    for (it = cachedSave.at("characterList").begin(); it != cachedSave.at("characterList").end();
         ++it) {
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
    a.tag = j.at("tag");
    a.description = j.at("description");
    a.type =
        (j.at("type") == "FOCUS") ? Character::Attack::Type::FOCUS : Character::Attack::Type::SPLIT;
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
// 第二部分：GameData 接口的实现

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

    // 若 GameData 正在开发阶段
    inDevelop();

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
    vector<Save> listRet;
    listRet.reserve(4);

    for (auto const& s : savesDom["saveList"]) {

        // tag 为 0 的存档是存档模板，非实际存档
        if (s["tag"] != 0) {
            listRet.push_back(s);
        }
    }

    return listRet;
}

bool
GameData::newGame()
{
    return (newSave() >= 0) ? true : false;
}

void
GameData::continueGame()
{
    int curSave = getCurrentSaveTag();
    switchSave(curSave);
}

int
GameData::newSave()
{
    /*  1. 判断存档个数是否已满 */

    int saveCnt = savesDom["saveList"].size();
    if (saveCnt == 4) {
        return -1;
    }

    /*  2. 新建存档，新存档的编号是最后一个存档的编号 + 1 */

    int newSaveTag = savesDom["saveList"].back()["tag"].get<int>() + 1;
    savesDom["saveList"].push_back(savesDom["saveList"][0]);

    auto& newSave = savesDom["saveList"].back();
    newSave["tag"] = newSaveTag;
    newSave["name"] = string("save ") + to_string(newSaveTag);
    newSave["time"] = getCurrentTime();

    switchSave(newSaveTag);
    syncSaveChangesToFile();

    return newSaveTag;
}

void
GameData::deleteSave(int saveTag)
{
    // TODO 玩家要是删除自己正在玩的存档怎么办

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
        savesDom["saveList"].erase(it); // 危险，待测试
    }

    /* 2. 同步至文件 */

    syncSaveChangesToFile();
}

void
GameData::saveSave(int saveTag)
{
    /* 1. 更改 DOM 树 */

    cachedSave["time"] = getCurrentTime();
    savesDom["saveList"][saveTag] = cachedSave;

    /* 2. 写入文件 */

    syncSaveChangesToFile();
}

void
GameData::switchSave(int saveTag)
{
    // 不保存对当前存档的更改，用户不明确表示保存就不保存

    savesDom["currentSaveTag"] = saveTag;
    cachedSave = savesDom["saveList"][saveTag];

    // TODO
    // TEST
    log(">> cachedSave:");
    log("  tag: %d", cachedSave["tag"].get<int>());
    log("  time: %s", cachedSave["time"].get<string>().c_str());
    log("  characterList[0][\"tag\"]: %s",
        cachedSave["characterList"][0]["tag"].get<string>().c_str());
}

// 音量（Volume）/ 对话速度（ConversationSpedd） 类接口有类似的内部实现。
// 此处作成宏，让编译器帮我们生成

#define APP_IMPLEMENT_GET_AND_SAVE(funcName, domNodeName)                                          \
    float GameData::getSaved##funcName() { return savesDom[#domNodeName]; }                        \
                                                                                                   \
    void GameData::save##funcName(float newVolume)                                                 \
    {                                                                                              \
        if (newVolume >= 0 && newVolume <= 1) {                                                    \
            savesDom[#domNodeName] = newVolume;                                                    \
        }                                                                                          \
    }

APP_IMPLEMENT_GET_AND_SAVE(BgmVolume, bgmVolume)
APP_IMPLEMENT_GET_AND_SAVE(EffectsVolume, effectsVolume)
APP_IMPLEMENT_GET_AND_SAVE(ConversationSpeed, conversationSpeed)

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
    string locTag = cachedSave["locationTag"];

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
    const json& unlockedLocListDom = cachedSave["unlockedLocationList"];

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
    cachedSave["locationTag"] = newLocationTag;
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

bool
GameData::setRoundToPlay(const string& roundTag)
{
    return true;
}

vector<Character>
GameData::getAvailableCharacterList()
{
    const json& listDom = cachedSave["characterList"];
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
    json& onStageListDom = cachedSave["onStageCharacterList"];

    vector<string> characters;
    for (auto const& c : onStageListDom) {
        characters.push_back(c);
    }

    return characters;
}

bool
GameData::switchOnStageCharacter(int nth, const string& characterTag)
{
    cachedSave["onStageCharacterList"][nth - 1] = characterTag;

    return true;
}

vector<Item>
GameData::getCharacterItemList(const string& characterTag)
{
    /* 1. 找到人，并取其装备 tag 列表 */

    const json& characterListDom = cachedSave["characterList"];

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
GameData::changeItem(const string& characterTag, int slot, const string& newItemTag)
{

    /* 1. 更改已装备道具的记录列表，同时记下旧装备的 tag */

    string oldItemTag;

    json& characterListDom = cachedSave["characterList"];

    for (auto& c : characterListDom) {
        if (c["tag"] == characterTag) {
            oldItemTag = c["equipedItemList"][slot];
            c["equipedItemList"][slot] = newItemTag;
            break;
        }
    }

    /* 2. 根据上一步找到的旧装备 tag，找到旧装备的属性记录 */

    auto oldItemIt =
        find_if(itemListDom.begin(), itemListDom.end(), [&oldItemTag](const json& iToPred) -> bool {
            if (iToPred["tag"] == oldItemTag) {
                return true;
            } else {
                return false;
            }
        });

    /* 3. 根据传递进来的新装备 tag 形参，找到新装备的属性记录 */

    auto newItemIt =
        find_if(itemListDom.begin(), itemListDom.end(), [&newItemTag](const json& iToPred) -> bool {
            if (iToPred["tag"] == newItemTag) {
                return true;
            } else {
                return false;
            }
        });

    /* 4. 减去旧装备加成，加上新装备加成 */

    for (auto& c : characterListDom) {
        if (c["tag"] == characterTag) {
            // 此处不能使用 -= 或者 +=，因为 nlohmann::json 没有重载 -= 和 += 运算符

            // - 运算符两边的 json 对象都需要显式指定转换成的类型，不能依赖隐式类型转换。
            // 没有最佳匹配，会引起 ambiguous

            // 旧装备加成
            c["healthPointInc"] =
                c["healthPointInc"].get<int>() - oldItemIt->at("healthPointInc").get<int>();
            c["manaInc"] = c["manaInc"].get<int>() - oldItemIt->at("manaInc").get<int>();
            c["walkSpeedInc"] =
                c["walkSpeedInc"].get<int>() - oldItemIt->at("walkSpeedInc").get<int>();
            c["dashAccelerationInc"] = c["dashAccelerationInc"].get<int>() -
                                       oldItemIt->at("dashAccelerationInc").get<int>();

            // 新装备加成
            c["healthPointInc"] =
                c["healthPointInc"].get<int>() + newItemIt->at("healthPointInc").get<int>();
            c["manaInc"] = c["manaInc"].get<int>() + newItemIt->at("manaInc").get<int>();
            c["walkSpeedInc"] =
                c["walkSpeedInc"].get<int>() + newItemIt->at("walkSpeedInc").get<int>();
            c["dashAccelerationInc"] = c["dashAccelerationInc"].get<int>() -
                                       newItemIt->at("dashAccelerationInc").get<int>();
            break;
        }
    }

    return true;
}

vector<SpellCard>
GameData::getCharacterSpellCardList(const string& characterTag)
{
    /* 1. 找到人，并取其装备 tag 列表 */

    const json& characterListDom = cachedSave["characterList"];

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
    json& characterListDom = cachedSave["characterList"];

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
    vector<Character::Attack> listRet;
    listRet.reserve(2);

    auto it = find_if(characterListDom.begin(), characterListDom.end(),
                      [&characterTag](const json& c) -> bool {
                          if (c["tag"] == characterTag) {
                              return true;
                          } else {
                              return false;
                          }

                      });

    if (it != characterListDom.end()) {
        listRet.push_back(it->at("attackList")[0]);
        listRet.push_back(it->at("attackList")[1]);
    }

    return listRet;
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
    return cachedSave["money"];
}

void
GameData::increaseMoney(long num)
{
    cachedSave["money"] = cachedSave["money"].get<long>() + num;
}

vector<pair<Item, int>>
GameData::getAvailableItemList()
{
    const json& availItemListDom = cachedSave["availableItemList"];

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
    const json& availSpellCardListDom = cachedSave["availableSpellCardList"];

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

////////////////////////////////////////////////////////////////////////////////
// 第三部分：开发阶段测试和特殊初始化 In Development

static void
testSelf()
{
    /* 1. 看其运行时会不会崩溃 */

    auto ptr = GameData::getInstance();

    auto get_current_save_tag = ptr->getCurrentSaveTag();
    auto vector_save = ptr->getSaveList();
    ptr->saveSave(get_current_save_tag);
    // ptr->deleteSave(get_current_save_tag); // too dangerous
    // ptr->switchSave(get_current_save_tag); // too dangerous
    ptr->saveBgmVolume(0.7);
    auto get_saved_bgm_volume = ptr->getSavedBgmVolume();
    ptr->saveEffectsVolume(0.7);
    auto get_saved_effects_volume = ptr->getSavedEffectsVolume();
    ptr->saveConversationSpeed(0.7);
    auto get_saved_conversation_speed = ptr->getSavedConversationSpeed();
    auto get_conversation_indicator_list = ptr->getConversationIndicatorList("Hakurei Jinja");
    auto get_conversation = ptr->getConversation("1");
    auto get_current_location = ptr->getCurrentLocation();
    auto get_location_list = ptr->getLocationList();
    auto get_unlocked_location_tag_list = ptr->getUnlockedLocationTagList();
    ptr->switchLocation("Hakurei Jinja");
    auto get_round_list = ptr->getRoundList("Hakurei Jinja");
    auto get_avail_characters = ptr->getAvailableCharacterList();
    ptr->switchOnStageCharacter(1, "Marisa");
    ptr->switchOnStageCharacter(2, "Reimu");
    auto get_onstage_char_list = ptr->getOnStageCharacterTagList();
    ptr->changeItem("Reimu", 0, "I2"); // two same items in one character
    ptr->changeSpellCard("Reimu", 0, "C2");

    get_avail_characters = ptr->getAvailableCharacterList();

    auto get_character_item_list = ptr->getCharacterItemList("Reimu");
    auto get_character_card_list = ptr->getCharacterSpellCardList("Reimu");
    auto get_attack_list = ptr->getAttackList("Reimu");
    ptr->switchAttackType("Reimu", Character::Attack::Type::FOCUS);
    auto get_money_num = ptr->getMoneyNum();
    ptr->increaseMoney(+1024);
    get_money_num = ptr->getMoneyNum();
    auto get_avail_item_list = ptr->getAvailableItemList();
    auto get_item_list_in_store_1 = ptr->getItemListInStore("ArmsStore");
    auto get_item_list_in_store_2 = ptr->getItemListInStore("Kourindou");
    auto get_avail_card_list = ptr->getAvailableSpellCardList();

    /* 2. 测试其返回的结果是否正确 */

    log("\n==================== GameData Test Begin ====================");

    log(">> GetSaveList:");
    for (auto const& s : vector_save) {
        cout << "  tag: " << s.tag << endl;
        cout << "  name: " << s.name << endl;
        cout << "  time: " << s.time << endl;
        cout << "  locationTag: " << s.locationTag << endl;
    }

    log(">> getConversationIndicatorList:");
    for (auto const& i : get_conversation_indicator_list) {
        cout << "  conversationTag: " << i.conversationTag << endl;
        cout << "  icon: " << i.icon << endl;
        cout << "  name: " << i.name << endl;
    }

    log(">> getConversation:");
    for (auto const& d : get_conversation) {
        cout << "  characterName: " << d.characterName << endl;
        cout << "  characterNameWrodArt" << d.characterNameWordArt << endl;
        cout << "  characterPicPosition"
             << (d.characterPicPosition == Dialogue::PicturePosition::LEFT
                     ? "LEFT"
                     : d.characterPicPosition == Dialogue::PicturePosition::MID ? "MID" : "RIGHT")
             << endl;
        cout << "  content: " << d.content << endl;
        cout << "  backgrouondPicture: " << d.backgroundPicture << endl;
        cout << "  backgroundMusic: " << d.backgroundMusic << endl;
        cout << "  soundEffect: " << d.soundEffect << endl;
        cout << "  screenEffect: "
             << (d.screenEffect == Dialogue::ScreenEffect::NONE ? "NONE" : "SHAKE");
    }

    log(">> getCurrentLocation: ");
    auto l = get_current_location;
    cout << "  tag: " << l.tag << endl;
    cout << "  name: " << l.name << endl;
    cout << "  wordArt: " << l.wordArt << endl;
    cout << "  previewPicture: " << l.previewPicture << endl;
    cout << "  backgroundPicture: " << l.backgroundPicture << endl;
    cout << "  backgroundMusic: " << l.backgroundMusic << endl;
    cout << "  passedRound: " << l.passedRound << endl;
    cout << "  totalRound: " << l.totalRound << endl;

    log(">> getLocationList: ");
    for (auto const& l : get_location_list) {
        cout << "  tag: " << l.tag << endl;
        cout << "  name: " << l.name << endl;
        cout << "  previewPicture: " << l.previewPicture << endl;
        cout << "  backgroundPicture: " << l.backgroundPicture << endl;
        cout << "  backgroundMusic: " << l.backgroundMusic << endl;
        cout << "  passedRound: " << l.passedRound << endl;
        cout << "  totalRound: " << l.totalRound << endl;
    }

    log(">> getUnlockedLocationTagList: ");
    for (auto const& t : get_unlocked_location_tag_list) {
        cout << "  tag: " << t << endl;
    }

    log(">> getAvailCharacterList: ");
    for (auto const& c : get_avail_characters) {
        cout << "  tag: " << c.tag << endl;
        cout << "  name: " << c.name << endl;
        cout << "  avatar: " << c.name << endl;
        cout << "  itemSlotNum: " << c.itemSlotNum << endl;
        cout << "  spellCardSlotNum: " << c.spellCardSlotNum << endl;
        cout << "  healthPointBase: " << c.healthPointBase << endl;
        cout << "  healthPointInc: " << c.healthPointInc << endl;
        cout << "  manaBase: " << c.manaBase << endl;
        cout << "  manaInc: " << c.manaInc << endl;
        cout << "  ..." << endl;
    }

    log(">> getOnStageCharacterTagList: ");
    for (auto const& c : get_onstage_char_list) {
        cout << "  tag: " << c << endl;
    }

    log(">> getCharacterItemList: ");
    for (auto const& i : get_character_item_list) {
        cout << "  tag: " << i.tag << endl;
        cout << "  name: " << i.name << endl;
        cout << "  icon: " << i.icon << endl;
        cout << "  description: " << i.description << endl;
        cout << "  type: " << (i.type == Item::Type::NORMAL
                                   ? "NORMAL"
                                   : i.type == Item::Type::STRENGTHEN ? "STRENGTHEN" : "SPECIAL")
             << endl;
        cout << "  isCarriable: " << i.isCarriable << endl;
        cout << "  isUseable: " << i.isUseable << endl;
        cout << "  maxUseCount: " << i.maxUseCount << endl;
        cout << "  healthPointCost: " << i.healthPointCost << endl;
        cout << "  manaCost: " << i.manaCost << endl;
        cout << "  coolDown: " << i.cooldown << endl;
        cout << "  healthPointInc: " << i.healthPointInc << endl;
        cout << "  manaInc: " << i.manaInc << endl;
        cout << "  walkSpeedInc: " << i.walkSpeedInc << endl;
        cout << "  walkAccelerationInc: " << i.walkAccelerationInc << endl;
        cout << "  dashAccelerationInc: " << i.dashAccelerationInc << endl;
        cout << "  price: " << i.price << endl;
    }

    log(">> getCharacterSpellCardList: ");
    for (auto const& c : get_character_card_list) {
        cout << "  tag: " << c.tag << endl;
        cout << "  ..." << endl;
    }

    log(">> getAttackList:");
    for (auto const& a : get_attack_list) {
        cout << "  tag: " << a.tag << endl;
        cout << "  description: " << a.description << endl;
        cout << "  type: " << ((a.type == Character::Attack::Type::FOCUS) ? "FOCUS" : "SPLIT")
             << endl;
    }

    log(">> getAvailItemList:");
    for (auto const& p : get_avail_item_list) {
        cout << "  tag: " << p.first.tag << endl;
        cout << "  amount: " << p.second << endl;

        cout << "  ..." << endl;
    }

    log(">> getAvailSpellCardList:");
    for (auto const& p : get_avail_card_list) {
        cout << "  tag: " << p.first.tag << endl;
        cout << "  amount: " << p.second << endl;
        cout << "  ..." << endl;
    }

    log(">> getItemListInStore(\"ArmsStore\"):");
    for (auto const& i : get_item_list_in_store_1) {
        cout << "  tag: " << i.tag << endl;
        cout << "  ..." << endl;
    }

    log(">> getItemListInStore(\"Kourindou\")");
    for (auto const& i : get_item_list_in_store_2) {
        cout << "  tag: " << i.tag << endl;
        cout << "  ..." << endl;
    }

    log("==================== GameData Test End ====================");
}

static void
inDevelop()
{
    auto ptr = GameData::getInstance();

    ptr->newGame();

    testSelf();
}
