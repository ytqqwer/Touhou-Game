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

#include "GameData/GameData.h"
#include "GameData/Character.h"
#include "GameData/Conversation.h"
#include "GameData/Item.h"
#include "GameData/Location.h"
#include "GameData/Round.h"
#include "GameData/SpellCard.h"

#include "GameData/EnemyData.h"
#include "GameData/EventData.h"

#include "external/json.h"
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

static json enemyListDom;
static json eventListDom;
static json updateDom;

// 用于支持玩 A 存档时，想存至 B 存档上
// 我们需要记录更改到其他地方，而不能直接记录到 A 上
static json cachedSave;

static string roundToPlayTag;

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
    ofstream saves_json(FileUtils::getInstance()->fullPathForFilename("gamedata/saves.json"),
                        ios_base::trunc | ios_base::out);
    saves_json << setw(4) << savesDom;
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

// conversationListDom[n] -> ConversationIndicator
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

// locationListDom[n]["roundList"][n] -> Round
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

    r.TMXMap = j.at("TMXMap");
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

    c.defaultTexture = it->at("defaultTexture");

    for (auto const& p : it->at("standFrame")) {
        c.standFrame.push_back(p);
    }
    for (auto const& p : it->at("runFrame")) {
        c.runFrame.push_back(p);
    }
    for (auto const& p : it->at("preJumpFrame")) {
        c.preJumpFrame.push_back(p);
    }
    for (auto const& p : it->at("jumpFrame")) {
        c.jumpFrame.push_back(p);
    }
    for (auto const& p : it->at("preFallFrame")) {
        c.preFallFrame.push_back(p);
    }
    for (auto const& p : it->at("fallFrame")) {
        c.fallFrame.push_back(p);
    }
    for (auto const& p : it->at("dashFrame")) {
        c.dashFrame.push_back(p);
    }

    c.standAnimationKey = c.tag + ("StandAnimation");
    c.runAnimationKey = c.tag + ("RunAnimation");
    c.preJumpAnimationKey = c.tag + ("PreJumpAnimation");
    c.jumpAnimationKey = c.tag + ("JumpAnimation");
    c.preFallAnimationKey = c.tag + ("PreFallAnimation");
    c.fallAnimationKey = c.tag + ("FallAnimation");
    c.dashAnimationKey = c.tag + ("DashAnimation");

    c.standFrameDelay = it->at("standFrameDelay");
    c.runFrameDelay = it->at("runFrameDelay");
    c.preJumpFrameDelay = it->at("preJumpFrameDelay");
    c.jumpFrameDelay = it->at("jumpFrameDelay");
    c.preFallFrameDelay = it->at("preFallFrameDelay");
    c.fallFrameDelay = it->at("fallFrameDelay");
    c.dashFrameDelay = it->at("dashFrameDelay");

    c.healthPointBase = it->at("healthPointBase");
    c.manaBase = it->at("manaBase");

    c.walkSpeedBase = it->at("walkSppedBase");
    c.walkMaxSpeed = it->at("walkMaxSpeed");
    c.walkAccelerationTimeBase = it->at("walkAccelerationTimeBase");
    c.walkAccelerationBase = it->at("walkAccelerationBase");

    c.dashAccelerationBase = it->at("dashAccelerationBase");
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
    i.type = (j.at("type") == "NORMAL")
                 ? Item::Type::NORMAL
                 : (j.at("type") == "STRENGTHEN") ? Item::Type::STRENGTHEN : Item::Type::SPECIAL;
    i.isCarriable = j.at("isCarriable");
    i.isUseable = j.at("isUseable");

    i.maxUseCount = j.at("maxUseCount");
    i.healthPointCost = j.at("healthPointCost");
    i.manaCost = j.at("manaCost");
    i.cooldown = j.at("coolDown");

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

    c.price = j.at("price");
}

// enemyListDom[n] -> EnemyData
static void
from_json(const json& j, EnemyData& c)
{
    c.tag = j.at("tag");
    c.name = j.at("name");
    c.face = j.at("face");
    c.defaultTexture = j.at("defaultTexture");

    for (auto const& p : j.at("standFrame")) {
        c.standFrame.push_back(p);
    }
    for (auto const& p : j.at("runFrame")) {
        c.runFrame.push_back(p);
    }
    for (auto const& p : j.at("preJumpFrame")) {
        c.preJumpFrame.push_back(p);
    }
    for (auto const& p : j.at("jumpFrame")) {
        c.jumpFrame.push_back(p);
    }
    for (auto const& p : j.at("preFallFrame")) {
        c.preFallFrame.push_back(p);
    }
    for (auto const& p : j.at("fallFrame")) {
        c.fallFrame.push_back(p);
    }
    for (auto const& p : j.at("dashFrame")) {
        c.dashFrame.push_back(p);
    }
    for (auto const& p : j.at("hitFrame")) {
        c.hitFrame.push_back(p);
    }
    for (auto const& p : j.at("downFrame")) {
        c.downFrame.push_back(p);
    }

    c.standAnimationKey = c.tag + ("StandAnimation");
    c.runAnimationKey = c.tag + ("RunAnimation");
    c.preJumpAnimationKey = c.tag + ("PreJumpAnimation");
    c.jumpAnimationKey = c.tag + ("JumpAnimation");
    c.preFallAnimationKey = c.tag + ("PreFallAnimation");
    c.fallAnimationKey = c.tag + ("FallAnimation");
    c.dashAnimationKey = c.tag + ("DashAnimation");
    c.hitAnimationKey = c.tag + ("HitAnimation");
    c.downAnimationKey = c.tag + ("DownAnimation");

    c.standFrameDelay = j.at("standFrameDelay");
    c.runFrameDelay = j.at("runFrameDelay");
    c.preJumpFrameDelay = j.at("preJumpFrameDelay");
    c.jumpFrameDelay = j.at("jumpFrameDelay");
    c.preFallFrameDelay = j.at("preFallFrameDelay");
    c.fallFrameDelay = j.at("fallFrameDelay");
    c.dashFrameDelay = j.at("dashFrameDelay");
    c.hitFrameDelay = j.at("hitFrameDelay");
    c.downFrameDelay = j.at("downFrameDelay");

    c.healthPoint = j.at("healthPoint");
}

// eventListDom[n] -> EventData
static void
from_json(const json& j, EventData& c)
{
    c.eventType = j.at("eventType");

    if (j.value("eventType", "") == "conversation") {
        c.conversationTag = j.at("conversationTag");

    } else if (j.value("eventType", "") == "action") {
        c.jump = (j.at("jump") == "true") ? true : false;
        c.delay = j.at("delay");
        c.duration = j.at("duration");
    } else if (j.value("eventType", "") == "update") {
        c.updateTag = j.at("updateTag");
    } else if (j.value("eventType", "") == "end") {
    }
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

    ifstream enemies_json(prefix + "gamedata/enemies.json");
    ifstream events_json(prefix + "gamedata/events.json");
    ifstream update_json(prefix + "gamedata/update.json");

#else
    ifstream saves_json(fileUtil->fullPathForFilename("gamedata/saves.json"));
    ifstream characters_json(fileUtil->fullPathForFilename("gamedata/characters.json"));
    ifstream locations_json(fileUtil->fullPathForFilename("gamedata/locations.json"));
    ifstream conversations_json(fileUtil->fullPathForFilename("gamedata/conversations.json"));
    ifstream items_json(fileUtil->fullPathForFilename("gamedata/items.json"));
    ifstream spell_cards_json(fileUtil->fullPathForFilename("gamedata/spell_cards.json"));
    ifstream awards_json(fileUtil->fullPathForFilename("gamedata/awards.json"));

    ifstream enemies_json(fileUtil->fullPathForFilename("gamedata/enemies.json"));
    ifstream events_json(fileUtil->fullPathForFilename("gamedata/events.json"));
    ifstream update_json(fileUtil->fullPathForFilename("gamedata/update.json"));

#endif

    /* 2. 文件流 -> 内存/DOM */

    saves_json >> savesDom;
    characters_json >> characterListDom;
    locations_json >> locationListDom;
    conversations_json >> conversationListDom;
    items_json >> itemListDom;
    spell_cards_json >> spellCardListDom;
    awards_json >> awardListDom;

    enemies_json >> enemyListDom;
    events_json >> eventListDom;
    update_json >> updateDom;

    /* 3. 关闭文件流 */

    saves_json.close();
    characters_json.close();
    locations_json.close();
    conversations_json.close();
    items_json.close();
    spell_cards_json.close();
    awards_json.close();

    enemies_json.close();
    events_json.close();
    update_json.close();

#ifndef NDEBUG
    // 若使用 DEBUG mode 来生成项目
    inDevelop();
#endif

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

//继续游戏？
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
    if (saveCnt == 9) { //存档模板+8个存档
        return -1;
    }

    /*  2. 新建存档，新存档的编号是按序排列的 */

    //找到第一个符合升序排列的下标，因为存档可以保存至任意位置
    //例如存档中已有tag分别为0,1,4的存档，那么就需要创建一个tag为2的存档并插入正确的位置
    //有没有更好的算法？
    int index = 1;
    auto saveList = getSaveList();
    auto it = savesDom["saveList"].begin();
    it++;
    for (auto& v : saveList) {
        if (index >= v.tag) {
            index++;
            it++;
        } else if (index < v.tag) {
            break;
        }
    }

    savesDom["saveList"].insert(it, savesDom["saveList"][0]);
    auto& newSave = savesDom["saveList"].at(index);
    newSave["tag"] = index;
    newSave["name"] = string("save ") + to_string(index);
    newSave["time"] = getCurrentTime();

    switchSave(index);
    syncSaveChangesToFile();

    return index;
}

void
GameData::saveSave(int saveTag)
{
    /* 1. 更改 DOM 树 */

    bool isInsert = false;
    int index = 1;
    auto saveList = getSaveList();
    auto it = savesDom["saveList"].begin();
    it++;
    for (auto& v : saveList) {
        if (saveTag == v.tag) {
            isInsert = false;
            break;
        } else if (saveTag < v.tag) {
            isInsert = true;
            break;
        } else if (saveTag > v.tag) {
            index++;
            it++;
        }
    }

    if (isInsert) {
        savesDom["saveList"].insert(it, savesDom["saveList"][0]);
        auto& newSave = savesDom["saveList"].at(index);
        newSave = cachedSave;
        newSave["tag"] = saveTag;
        newSave["name"] = string("save ") + to_string(saveTag);
        newSave["time"] = getCurrentTime();
    } else {
        savesDom["saveList"][index] = cachedSave;
        auto& save = savesDom["saveList"].at(index);
        save["tag"] = saveTag;
        save["name"] = string("save ") + to_string(saveTag);
        save["time"] = getCurrentTime();
    }

    //一般而言，玩家存档后，期望以后能够默认进入新存档
    savesDom["currentSaveTag"] = saveTag;

    /* 2. 写入文件 */

    syncSaveChangesToFile();
}

void
GameData::switchSave(int saveTag)
{
    // 不保存对当前存档的更改，用户不明确表示保存就不保存

    savesDom["currentSaveTag"] = saveTag;

    auto saveList = getSaveList();
    int index = 1;
    for (auto& v : saveList) {
        if (v.tag != saveTag) {
            index++;
        } else {
            break;
        }
    }
    cachedSave = savesDom["saveList"][index];

    // TODO
    // TEST
    log(">> cachedSave:");
    log("  tag: %d", cachedSave["tag"].get<int>());
    log("  time: %s", cachedSave["time"].get<string>().c_str());
    log("  characterList[0][\"tag\"]: %s",
        cachedSave["characterList"][0]["tag"].get<string>().c_str());
}

void
GameData::updateSave(const std::string& updateTag)
{
    json::const_iterator elementUpdateDom =
        find_if(updateDom.cbegin(), updateDom.cend(), [&updateTag](const json& cToPred) -> bool {
            return cToPred.at("updateTag") == updateTag;
        });

    //更新钱
    increaseMoney(elementUpdateDom->at("money"));

    //更新道具
    const json& Items = elementUpdateDom->at("getItems");
    json& availItemListDom = cachedSave["availableItemList"];
    for (auto const& item : Items) {
        json newItemRecord = { { "tag", item.at("tag") } };
        availItemListDom.push_back(newItemRecord);
    }

    //更新符卡
    const json& SpellCards = elementUpdateDom->at("getSpellCards");
    json& availSpellCardListDom = cachedSave["availableSpellCardList"];
    for (auto const& card : SpellCards) {
        json newCardRecord = { { "tag", card.at("tag") } };
        availSpellCardListDom.push_back(newCardRecord);
    }

    //更新地点以及对话信息
    const json& Locations = elementUpdateDom->at("unlockLocations");
    json& unlockedLocsDom = cachedSave["unlockedLocationList"];
    for (auto const& location : Locations) {
        bool isExist = false;
        //若此地点已存在，则追加新的对话
        for (auto& ul : unlockedLocsDom) {
            if (ul["tag"] == location["tag"]) {
                json& indicatorsDom =
                    ul["conversationIndicators"]; //将当前遍历对象的c..Indi..ors转化为json对象
                const json& indicators = location.at("conversationIndicators");
                for (auto& tag : indicators) {
                    indicatorsDom.push_back(tag); //追加新的对话tag
                }
                isExist = true;
                break;
            }
        }
        //若地点不存在，追加新地点以及对话
        if (!isExist) {
            json newLocationRecord = { { "tag", location.at("tag") },
                                       { "passedRound", 0 },
                                       { "conversationIndicators",
                                         location.at("conversationIndicators") } };
            unlockedLocsDom.push_back(newLocationRecord);
        }
    }
    //增加通过的关卡数
    const json& passedRoundIncrease = elementUpdateDom->at("passedRoundIncrease");
    for (auto const& location : passedRoundIncrease) {
        for (auto& ul : unlockedLocsDom) {
            if (ul["tag"] == location["tag"]) {
                ul["passedRound"] =
                    ul["passedRound"].get<int>() + location["passedRoundIncrease"].get<int>();
                break;
            }
        }
    }

    //更新可选角色
    const json& Characters = elementUpdateDom->at("newCharacters");
    json& charactersDom = cachedSave["characterList"];
    for (auto const& character : Characters) {
        json newLocationRecord = { { "tag", character.at("tag") },
                                   { "itemSlotNum", character.at("itemSlotNum") },
                                   { "spellCardSlotNum", character.at("spellCardSlotNum") },
                                   { "activeItems", character.at("activeItems") },
                                   { "strengthenItems", character.at("strengthenItems") },
                                   { "spellCards", character.at("spellCards") },
                                   { "preferedAttackType", character.at("preferedAttackType") },
                                   { "selectedAttackType", character.at("selectedAttackType") } };
        charactersDom.push_back(newLocationRecord);
    }
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
GameData::getConversationIndicators(const string& locationTag)
{
    const json& unlockedLocListDom = cachedSave["unlockedLocationList"];

    vector<string> tagList;
    for (auto const& ul : unlockedLocListDom) {
        if (ul.at("tag") == locationTag) {
            for (auto const& ctag : ul["conversationIndicators"]) {
                tagList.push_back(ctag);
            }
        }
    }

    if (tagList.size() <= 0) {
        return vector<ConversationIndicator>();
    }

    vector<ConversationIndicator> listRet;
    json::const_iterator it;
    for (auto const& ctag : tagList) {
        for (auto const& it : conversationListDom) {
            if (it.at("conversationTag") == ctag) {
                listRet.push_back(it);
                break;
            }
        }
    }

    return listRet;
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

Location
GameData::getLocationByTag(const string& locationTag)
{
    json::const_iterator it;
    for (it = locationListDom.begin(); it != locationListDom.end(); ++it) {
        if (it->at("tag") == locationTag) {
            break;
        }
    }

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

vector<Location>
GameData::getUnlockedLocationList()
{
    const json& unlockedLocListDom = cachedSave["unlockedLocationList"];

    vector<string> unlockLocationTag;
    for (auto const& location : unlockedLocListDom) {
        unlockLocationTag.push_back(location.at("tag"));
    }

    vector<Location> listRet;
    for (auto const& tag : unlockLocationTag) {
        for (auto const& location : locationListDom) {
            if (location.at("tag") == tag) {
                listRet.push_back(location);
                break;
            }
        }
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
    roundToPlayTag = roundTag;
    return true;
}

vector<EventData>
GameData::getEventListByTag(const string& eventTag)
{
    vector<EventData> eventList;

    /*  1. 找到和 eventTag 匹配的事件列表 */

    json::const_iterator eListDom = find_if(
        eventListDom.cbegin(), eventListDom.cend(),
        [&eventTag](const json& cToPred) -> bool { return cToPred.at("eventTag") == eventTag; });
    const json& listDom = eListDom->at("eventList");

    /*  2. 将 JSON 数组转成 vector<EventData> */

    for_each(listDom.cbegin(), listDom.cend(),
             [&eventList](const json& dToAdd) { eventList.push_back(dToAdd); });

    return eventList;
}

Round
GameData::getRoundToPlay()
{
    const json* roundPtr = nullptr;

    for (auto const& l : locationListDom) {
        for (auto const& r : l["roundList"]) {
            if (r["tag"] == roundToPlayTag) {
                roundPtr = &r;
                goto found;
            }
        }
    }

found:

    // *ptr -> Round
    return *roundPtr;
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

Character
GameData::getCharacterByTag(const string& characterTag)
{
    const json& listDom = cachedSave["characterList"];

    Character character;
    for (auto const& c : listDom) {
        if (c["tag"] == characterTag) {
            character = c;
            break;
        }
    }

    return character;
}

vector<Item>
GameData::getCharacterEquipedItems(const string& characterTag)
{
    auto activeItems = getCharacterActiveItems(characterTag);
    auto strengthenItems = getCharacterStrengthenItems(characterTag);
    vector<Item> allItems;
    allItems.insert(allItems.end(), activeItems.begin(), activeItems.end());
    allItems.insert(allItems.end(), strengthenItems.begin(), strengthenItems.end());
    return allItems;
}

vector<Item>
GameData::getCharacterActiveItems(const string& characterTag)
{
    /* 1. 找到人，并取其装备 tag */
    const json& characterListDom = cachedSave["characterList"];
    const json* activeItemsDomPtr;
    for (auto const& c : characterListDom) {
        if (c["tag"] == characterTag) {
            activeItemsDomPtr = &c["activeItems"];
            break;
        }
    }
    const json& activeItemsDom = *activeItemsDomPtr;

    /* 2. 根据装备列表取得装备详细信息 */
    vector<Item> listRet;

    for (auto const& iTagToFind : activeItemsDom) {
        auto it = find_if(itemListDom.begin(), itemListDom.end(),
                          [&iTagToFind](const json& item) -> bool {
                              if (item.at("tag") == iTagToFind.at("tag")) {
                                  return true;
                              } else {
                                  return false;
                              }
                          });
        listRet.push_back(*it);
    }
    return listRet;
}

vector<pair<Item, int>>
GameData::getCharacterActiveItemsInPair(const string& characterTag)
{
    const json& characterListDom = cachedSave["characterList"];
    const json* activeItemsDomPtr;
    for (auto const& c : characterListDom) {
        if (c["tag"] == characterTag) {
            activeItemsDomPtr = &c["activeItems"];
            break;
        }
    }
    const json& activeItemsDom = *activeItemsDomPtr;

    vector<pair<Item, int>> listRet;

    for (auto const& iToPred : itemListDom) {
        auto it = find_if(activeItemsDom.begin(), activeItemsDom.end(),
                          [&iToPred](const json& activeItem) -> bool {
                              if (activeItem.at("tag") == iToPred.at("tag")) {
                                  return true;
                              } else {
                                  return false;
                              }
                          });
        if (it != activeItemsDom.end()) {
            listRet.push_back(make_pair<Item, int>(iToPred, it->at("slot")));
        }
    }

    return listRet;
}

vector<Item>
GameData::getCharacterStrengthenItems(const string& characterTag)
{
    /* 1. 找到人，并取其装备 tag */
    const json& characterListDom = cachedSave["characterList"];

    const json* ItemsDomPtr;
    for (auto const& c : characterListDom) {
        if (c["tag"] == characterTag) {
            ItemsDomPtr = &c["strengthenItems"];
        }
    }
    const json& dom = *ItemsDomPtr;

    /* 2. 根据装备列表取得装备详细信息 */

    vector<Item> listRet;
    listRet.reserve(dom.size());
    for (auto const& iTagToFind : dom) {
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

// bool
// GameData::changeItem(const string& characterTag, int slot, const string& newItemTag)
//{
//
//    /* 1. 更改已装备道具的记录列表，同时记下旧装备的 tag */
//
//    string oldItemTag;
//
//    json& characterListDom = cachedSave["characterList"];
//
//    for (auto& c : characterListDom) {
//        if (c["tag"] == characterTag) {
//            oldItemTag = c["equipedItemList"][slot];
//            c["equipedItemList"][slot] = newItemTag;
//            break;
//        }
//    }
//
//    /* 2. 根据上一步找到的旧装备 tag，找到旧装备的属性记录 */
//
//    auto oldItemIt =
//        find_if(itemListDom.begin(), itemListDom.end(), [&oldItemTag](const json& iToPred) -> bool
//        {
//            if (iToPred["tag"] == oldItemTag) {
//                return true;
//            } else {
//                return false;
//            }
//        });
//
//    /* 3. 根据传递进来的新装备 tag 形参，找到新装备的属性记录 */
//
//    auto newItemIt =
//        find_if(itemListDom.begin(), itemListDom.end(), [&newItemTag](const json& iToPred) -> bool
//        {
//            if (iToPred["tag"] == newItemTag) {
//                return true;
//            } else {
//                return false;
//            }
//        });
//
//    /* 4. 减去旧装备加成，加上新装备加成 */
//
//    for (auto& c : characterListDom) {
//        if (c["tag"] == characterTag) {
//            // 此处不能使用 -= 或者 +=，因为 nlohmann::json 没有重载 -= 和 += 运算符
//
//            // - 运算符两边的 json 对象都需要显式指定转换成的类型，不能依赖隐式类型转换。
//            // 没有最佳匹配，会引起 ambiguous
//
//            // 旧装备加成
//            // c["healthPointInc"] =
//            //    c["healthPointInc"].get<int>() - oldItemIt->at("healthPointInc").get<int>();
//            // c["manaInc"] = c["manaInc"].get<int>() - oldItemIt->at("manaInc").get<int>();
//            // c["walkSpeedInc"] =
//            //    c["walkSpeedInc"].get<int>() - oldItemIt->at("walkSpeedInc").get<int>();
//            // c["dashAccelerationInc"] = c["dashAccelerationInc"].get<int>() -
//            //                           oldItemIt->at("dashAccelerationInc").get<int>();
//
//            //// 新装备加成
//            // c["healthPointInc"] =
//            //    c["healthPointInc"].get<int>() + newItemIt->at("healthPointInc").get<int>();
//            // c["manaInc"] = c["manaInc"].get<int>() + newItemIt->at("manaInc").get<int>();
//            // c["walkSpeedInc"] =
//            //    c["walkSpeedInc"].get<int>() + newItemIt->at("walkSpeedInc").get<int>();
//            // c["dashAccelerationInc"] = c["dashAccelerationInc"].get<int>() -
//            //                           newItemIt->at("dashAccelerationInc").get<int>();
//            break;
//        }
//    }
//
//    return true;
//}

void
GameData::equipActiveItem(const string& characterTag, const string& itemTag, int slot)
{
    //先卸除所有角色装备的该道具
    json& charactersDom = cachedSave["characterList"];
    for (auto& c : charactersDom) {
        json& activeItemsDom = c["activeItems"];
        vector<pair<string, int>> remain;
        bool found = false;
        for (auto& activeItem : activeItemsDom) {
            if (activeItem.at("tag") == itemTag) {
                found = true;
            } else {
                remain.push_back(
                    make_pair<string, int>(activeItem.at("tag"), activeItem.at("slot")));
            }
        }
        if (found) {
            activeItemsDom.clear();
            for (auto& activeItem : remain) {
                json newRecord = { { "tag", activeItem.first }, { "slot", activeItem.second } };
                activeItemsDom.push_back(newRecord);
            }
        }
    }

    //将该物品装配给指定角色
    for (auto& c : charactersDom) {
        if (c["tag"] == characterTag) {
            json& activeItemsDom = c["activeItems"];
            bool found = false;
            for (auto& activeItem : activeItemsDom) {
                if (activeItem["slot"] == slot) {
                    activeItem["tag"] = itemTag;
                    found = true;
                    break;
                }
            }
            if (!found) {
                json newRecord = { { "tag", itemTag }, { "slot", slot } };
                activeItemsDom.push_back(newRecord);
                break;
            }
        }
    }
}

void
GameData::equipStrengthenItem(const string& itemTag, const string& characterTag)
{
    //先卸除所有角色装备的该道具
    json& charactersDom = cachedSave["characterList"];
    for (auto& c : charactersDom) {
        json& equipedItemsDom = c["strengthenItems"];
        // json对象的erase方法需要提供关键字，删除一整个键值对，但在此只是需要移除“值”里的某些成员
        vector<std::string> remain;
        bool found = false;
        for (auto& tag : equipedItemsDom) {
            if (tag == itemTag) {
                found = true;
            } else if (tag != itemTag) {
                remain.push_back(tag);
            }
        }
        if (found) {
            equipedItemsDom.clear();
            for (auto& tag : remain) {
                equipedItemsDom.push_back(tag);
            }
        }
    }

    //将该物品装配给指定角色
    for (auto& c : charactersDom) {
        if (c["tag"] == characterTag) {
            c["strengthenItems"].push_back(itemTag);
            break;
        }
    }
}

vector<SpellCard>
GameData::getCharacterEquipedSpellCards(const string& characterTag)
{
    /* 1. 找到人，并取其 tag 列表 */

    const json& characterListDom = cachedSave["characterList"];

    const json* equipedCardListDomPtr;
    for (auto const& c : characterListDom) {
        if (c["tag"] == characterTag) {
            equipedCardListDomPtr = &c["spellCards"];
        }
    }
    const json& equipedListDom = *equipedCardListDomPtr;

    /* 2. 根据符卡列表取得装备详细信息 */

    vector<SpellCard> listRet;
    listRet.reserve(equipedListDom.size());

    for (auto const& cTagToFind : equipedListDom) {
        auto it = find_if(spellCardListDom.begin(), spellCardListDom.end(),
                          [&cTagToFind](const json& card) -> bool {
                              if (card.at("tag") == cTagToFind.at("tag")) {
                                  return true;
                              } else {
                                  return false;
                              }
                          });

        listRet.push_back(*it);
    }

    return listRet;
}

vector<pair<SpellCard, int>>
GameData::getCharacterEquipedSpellCardsInPair(const string& characterTag)
{
    const json& characterListDom = cachedSave["characterList"];

    const json* equipedCardListDomPtr;
    for (auto const& c : characterListDom) {
        if (c["tag"] == characterTag) {
            equipedCardListDomPtr = &c["spellCards"];
        }
    }
    const json& equipedListDom = *equipedCardListDomPtr;

    vector<pair<SpellCard, int>> listRet;
    listRet.reserve(equipedListDom.size());

    for (auto const& cardToFind : spellCardListDom) {
        auto it = find_if(equipedListDom.begin(), equipedListDom.end(),
                          [&cardToFind](const json& card) -> bool {
                              if (card.at("tag") == cardToFind.at("tag")) {
                                  return true;
                              } else {
                                  return false;
                              }
                          });
        if (it != equipedListDom.end()) {
            listRet.push_back(make_pair<SpellCard, int>(cardToFind, it->at("slot")));
        }
    }

    return listRet;
}

void
GameData::equipSpellCard(const string& characterTag, const string& cardTag, int slot)
{
    //先卸除所有角色装备的该符卡
    json& charactersDom = cachedSave["characterList"];
    for (auto& c : charactersDom) {
        json& spellCardsDom = c["spellCards"];
        vector<pair<string, int>> remain;
        bool found = false;
        for (auto& card : spellCardsDom) {
            if (card.at("tag") == cardTag) {
                found = true;
            } else {
                remain.push_back(make_pair<string, int>(card.at("tag"), card.at("slot")));
            }
        }
        if (found) {
            spellCardsDom.clear();
            for (auto& card : remain) {
                json newRecord = { { "tag", card.first }, { "slot", card.second } };
                spellCardsDom.push_back(newRecord);
            }
        }
    }

    //将该符卡装配给指定角色
    for (auto& c : charactersDom) {
        if (c["tag"] == characterTag) {
            json& spellCardsDom = c["spellCards"];
            bool found = false;
            for (auto& card : spellCardsDom) {
                if (card["slot"] == slot) {
                    card["tag"] = cardTag;
                    found = true;
                    break;
                }
            }
            if (!found) {
                json newRecord = { { "tag", cardTag }, { "slot", slot } };
                spellCardsDom.push_back(newRecord);
                break;
            }
        }
    }
}

// bool
// GameData::changeSpellCard(const string& characterTag, int slot, const string& spellCardTag)
//{
//    json& characterListDom = cachedSave["characterList"];
//
//    for (auto& c : characterListDom) {
//        if (c["tag"] == characterTag) {
//            c["equipedSpellCardList"][slot] = spellCardTag;
//            break;
//        }
//    }
//
//    return true;
//}

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
        int size = it->at("attackList").size();
        for (int i = 0; i < size; i++) {
            listRet.push_back(it->at("attackList")[i]);
        }
    }
    // if (it != characterListDom.end()) {
    //    listRet.push_back(it->at("attackList")[0]);
    //    listRet.push_back(it->at("attackList")[1]);
    //}
    return listRet;
}

vector<Character::Attack>
GameData::getSelectedAttackList(const string& characterTag)
{
    /* 1. 从存档中找到指定角色，并取其已选择的攻击方式的tag列表 */

    const json& listDom1 = cachedSave["characterList"];
    const json* selectedAttackListPtr;
    for (auto const& c : listDom1) {
        if (c["tag"] == characterTag) {
            selectedAttackListPtr = &c["selectedAttackType"];
        }
    }
    const json& selectAttackList = *selectedAttackListPtr;

    /* 2. 找到指定的角色 */

    const json& listDom2 = characterListDom;
    const json* attackListPtr;
    for (auto const& c : listDom2) {
        if (c["tag"] == characterTag) {
            attackListPtr = &c["attackList"];
            break;
        }
    }
    const json& attackList = *attackListPtr;

    /* 3. 先获得攻击方式列表，再从列表中选取 */

    vector<Character::Attack> listRet;
    listRet.reserve(selectAttackList.size());

    for (auto const& cTagToFind : selectAttackList) {
        auto it =
            find_if(attackList.begin(), attackList.end(), [&cTagToFind](const json& c) -> bool {
                if (c["tag"] == cTagToFind) {
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

void
GameData::unlockItemSlot(const string& characterTag)
{
    json& dom = cachedSave["characterList"];
    for (auto& c : dom) {
        if (c["tag"] == characterTag) {
            c["itemSlotNum"] = c["itemSlotNum"].get<int>() + 1;
            break;
        }
    }
}

void
GameData::unlockSpellCardSlot(const string& characterTag)
{
    json& dom = cachedSave["characterList"];
    for (auto& c : dom) {
        if (c["tag"] == characterTag) {
            c["spellCardSlotNum"] = c["spellCardSlotNum"].get<int>() + 1;
            break;
        }
    }
}

EnemyData
GameData::getEnemyByTag(const string& enemyTag)
{
    const json& listDom = enemyListDom;

    EnemyData enemy;
    for (auto const& c : listDom) {
        if (c["tag"] == enemyTag) {
            enemy = c;
            break;
        }
    }

    return enemy;
}

vector<Item>
GameData::getAvailableItems()
{
    const json& availItemListDom = cachedSave["availableItemList"];

    vector<Item> listRet;
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
            listRet.push_back(iToPred);
        }
    }

    return listRet;
}

#define APP_IMPLEMENT_GET_AVAIL_TYPE_ITEMS(funcName, itemType)                                     \
    vector<Item> GameData::getAvailable##funcName()                                                \
    {                                                                                              \
        const json& availItemListDom = cachedSave["availableItemList"];                            \
        vector<Item> listRet;                                                                      \
        listRet.reserve(availItemListDom.size());                                                  \
        for (auto const& iToPred : itemListDom) {                                                  \
            auto it = find_if(availItemListDom.begin(), availItemListDom.end(),                    \
                              [&iToPred](const json& availI) -> bool {                             \
                                  if (iToPred.at("tag") == availI.at("tag")) {                     \
                                      if (iToPred.at("type") == itemType) {                        \
                                          return true;                                             \
                                      } else {                                                     \
                                          return false;                                            \
                                      }                                                            \
                                  } else {                                                         \
                                      return false;                                                \
                                  }                                                                \
                              });                                                                  \
            if (it != availItemListDom.end()) {                                                    \
                listRet.push_back(iToPred);                                                        \
            }                                                                                      \
        }                                                                                          \
        return listRet;                                                                            \
    }

APP_IMPLEMENT_GET_AVAIL_TYPE_ITEMS(NormalItems, "NORMAL");
APP_IMPLEMENT_GET_AVAIL_TYPE_ITEMS(StrengthenItems, "STRENGTHEN");
APP_IMPLEMENT_GET_AVAIL_TYPE_ITEMS(SpecialItems, "SPECIAL");

vector<Item>
GameData::getAvailableActiveItems()
{
    auto normalItems = getAvailableNormalItems();
    auto specialItems = getAvailableSpecialItems();
    vector<Item> allItems;
    allItems.insert(allItems.end(), normalItems.begin(), normalItems.end());
    allItems.insert(allItems.end(), specialItems.begin(), specialItems.end());
    return allItems;
}

vector<Item>
GameData::getItemsInStore(const string& storeTag)
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

vector<Item>
GameData::getNormalItemsInStore(const string& storeTag)
{
    vector<Item> listRet;
    listRet.reserve(32);

    for (auto const& iToPred : itemListDom) {
        if (iToPred.at("inStore") == storeTag) {
            if (iToPred.at("type") == "NORMAL") {
                listRet.push_back(iToPred);
            }
        }
    }

    return listRet;
}

vector<Item>
GameData::getStrengthenItemsInStore(const string& storeTag)
{
    vector<Item> listRet;
    listRet.reserve(32);

    for (auto const& iToPred : itemListDom) {
        if (iToPred.at("inStore") == storeTag) {
            if (iToPred.at("type") == "STRENGTHEN") {
                listRet.push_back(iToPred);
            }
        }
    }

    return listRet;
}

vector<Item>
GameData::getSpecialItemsInStore(const string& storeTag)
{
    vector<Item> listRet;
    listRet.reserve(32);

    for (auto const& iToPred : itemListDom) {
        if (iToPred.at("inStore") == storeTag) {
            if (iToPred.at("type") == "SPECIAL") {
                listRet.push_back(iToPred);
            }
        }
    }

    return listRet;
}

vector<SpellCard>
GameData::getSpellCardsInStore(const string& storeTag)
{
    vector<SpellCard> listRet;
    listRet.reserve(32);

    for (auto const& iToPred : spellCardListDom) {
        if (iToPred.at("inStore") == storeTag) {
            listRet.push_back(iToPred);
        }
    }

    return listRet;
}

vector<SpellCard>
GameData::getAvailableSpellCards()
{
    const json& availSpellCardListDom = cachedSave["availableSpellCardList"];

    vector<SpellCard> listRet;
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
            listRet.push_back(cToPred);
        }
    }

    return listRet;
}

SpellCard
GameData::getSpellCardByTag(const string& cardTag)
{
    SpellCard card;
    for (auto const& iToPred : spellCardListDom) {
        if (iToPred.at("tag") == cardTag) {
            card = iToPred;
        }
    }
    return card;
}

Item
GameData::getItemByTag(const string& itemTag)
{
    Item item;
    for (auto const& iToPred : itemListDom) {
        if (iToPred.at("tag") == itemTag) {
            item = iToPred;
            break;
        }
    }
    return item;
}

void
GameData::buyItem(const string& itemTag)
{
    Item item = getItemByTag(itemTag);
    int price = item.price;

    json& availItemListDom = cachedSave["availableItemList"];
    json newItemRecord = { { "tag", itemTag } };
    availItemListDom.push_back(newItemRecord);
    increaseMoney(-price);
}

void
GameData::buySpellCard(const string& cardTag)
{
    SpellCard card = getSpellCardByTag(cardTag);
    int price = card.price;

    json& availSpellCardListDom = cachedSave["availableSpellCardList"];
    json newCardRecord = { { "tag", cardTag } };
    availSpellCardListDom.push_back(newCardRecord);
    increaseMoney(-price);
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
    auto get_conversation_indicator_list = ptr->getConversationIndicators("Hakurei Jinja");
    auto get_current_location = ptr->getCurrentLocation();
    auto get_location_list = ptr->getLocationList();
    auto get_unlocked_location_tag_list = ptr->getUnlockedLocationTagList();
    ptr->switchLocation("Hakurei Jinja");
    auto get_round_list = ptr->getRoundList("Hakurei Jinja");
    auto get_avail_characters = ptr->getAvailableCharacterList();
    ptr->switchOnStageCharacter(1, "Marisa");
    ptr->switchOnStageCharacter(2, "Reimu");
    auto get_onstage_char_list = ptr->getOnStageCharacterTagList();
    // ptr->changeItem("Reimu", 0, "I2"); // two same items in one character
    // ptr->changeSpellCard("Reimu", 0, "C2");

    get_avail_characters = ptr->getAvailableCharacterList();

    auto get_character_item_list = ptr->getCharacterActiveItems("Reimu");
    auto get_character_card_list = ptr->getCharacterEquipedSpellCards("Reimu");
    auto get_attack_list = ptr->getAttackList("Reimu");
    ptr->switchAttackType("Reimu", Character::Attack::Type::FOCUS);
    auto get_money_num = ptr->getMoneyNum();
    ptr->increaseMoney(+1024);
    get_money_num = ptr->getMoneyNum();
    auto get_avail_item_s = ptr->getAvailableItems();
    auto get_item_list_in_store_1 = ptr->getItemsInStore("ArmsStore");
    auto get_item_list_in_store_2 = ptr->getItemsInStore("Kourindou");
    auto get_avail_card_s = ptr->getAvailableSpellCards();
    ptr->setRoundToPlay("round 1");
    auto get_round_to_play = ptr->getRoundToPlay();

    /* 2. 测试其返回的结果是否正确 */

    log("\n==================== GameData Test Begin ====================");

    log(">> GetSaveList:");
    for (auto const& s : vector_save) {
        cout << "  tag: " << s.tag << endl;
        cout << "  name: " << s.name << endl;
        cout << "  time: " << s.time << endl;
        cout << "  locationTag: " << s.locationTag << endl;
    }

    log(">> getConversationIndicators:");
    for (auto const& i : get_conversation_indicator_list) {
        cout << "  conversationTag: " << i.conversationTag << endl;
        cout << "  icon: " << i.icon << endl;
        cout << "  name: " << i.name << endl;
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
        cout << "  manaBase: " << c.manaBase << endl;
        cout << "  ..." << endl;
    }

    log(">> getOnStageCharacterTagList: ");
    for (auto const& c : get_onstage_char_list) {
        cout << "  tag: " << c << endl;
    }

    log(">> getCharacterActiveItems: ");
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
        cout << "  price: " << i.price << endl;
    }

    log(">> getCharacterEquipedSpellCards: ");
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

    log(">> getAvailItems:");
    for (auto const& p : get_avail_item_s) {
        cout << "  tag: " << p.tag << endl;

        cout << "  ..." << endl;
    }

    log(">> getAvailSpellCards:");
    for (auto const& p : get_avail_card_s) {
        cout << "  tag: " << p.tag << endl;

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

    log(">> getRoundToPlay()");
    cout << "  tag: " << get_round_to_play.tag << endl;
    cout << "  name: " << get_round_to_play.name << endl;
    cout << "  previewPicture: " << get_round_to_play.previewPicture << endl;
    cout << "  description: " << get_round_to_play.description << endl;
    {
        auto d = get_round_to_play.difficulty;
        cout << "  difficulty: "
             << (d == Round::Difficulty::ONE
                     ? "ONE"
                     : d == Round::Difficulty::TWO
                           ? "TWO"
                           : d == Round::Difficulty::THREE
                                 ? "THREE"
                                 : d == Round::Difficulty::FOUR
                                       ? "FOURE"
                                       : d == Round::Difficulty::FIVE ? "FIVE" : "NONE")
             << endl;
    }
    cout << "  TMXMap: " << get_round_to_play.TMXMap << endl;

    log("==================== GameData Test End ====================");
}

static void
inDevelop()
{
    auto g = GameData::getInstance();
    if (g->getSaveList().size() == 0) {
        g->newGame();
    } else {
        g->continueGame();
    }

    // testSelf();
}
