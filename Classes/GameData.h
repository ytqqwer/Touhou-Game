#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "cocos2d.h"
#include <string>
#include <utility>
#include <vector>

#include "GameData/Character.h"
#include "GameData/Conversation.h"
#include "GameData/Item.h"
#include "GameData/Location.h"
#include "GameData/Round.h"
#include "GameData/Save.h"
#include "GameData/SpellCard.h"

USING_NS_CC;
using namespace std;

class GameData
{
public:
    static GameData* getInstance();

    ////////////////////////////////////////////////////////////////////////////////
    // 存档读取、删除、切换 Save

    int getCurrentSaveTag();
    vector<Save> getSaveList();
    void saveSave();
    void deleteSave(const string& saveTag);
    void switchSave(const string& saveTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 对话 Conversation

    // 调整对话显示速度，1 为标准速度，1 以下为减缓，1 以上为加快，与 1 的比例为速度相对比
    float getConversationSpeed();
    void setConversationSpeed(float relativeSpeed);

    // 通过对话编号返回详细对话内容，包括对话的双方，对话的内容，对话过程中人物的变化
    vector<Dialogue> getConversation(const string& conversationTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 地点与进度 Location / Process
    // 区分 Location / Map / Round , Location 是地点，Map 特指 TMX 地图，Round 则是一个个关卡

    // 获取当前所处地点的详情
    Location getCurrentLocation();

    // 只返回已解锁地点的信息
    // 在 LocationSelectScene 中使用
    vector<Location> getLocationList();

    // 获得指定地点的详情
    Location getLocation(const string& locationTag);

    // 获得指定关卡的详情
    // 在 RoundSelectScene 中使用
    Round getRound(const string& roundTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 人物属性与钱币 Character / Money

    // 返回现在可使用的角色的列表
    vector<Character> getAvailableCharacters();

    // 获得角色的列表后，传入角色标签，自动切换为队伍其他角色
    bool switchCharacter(const string& characterTag);

    // 在 EquipScene 中使用
    Character getCharacter(const string& characterTag);

    //获得指定角色使用或装备的道具列表
    vector<Item> getCharacterItemList(const string& characterTag);

    //获得指定的道具列表后，更换角色装备的道具
    bool changeItem(const string& itemTag, int slot, const string& characterTag);

    //获得指定角色的符卡列表，包括专属符卡和购买的通用符卡
    vector<SpellCard> getCharacterSpellCardList(const string& characterTag);

    //获得角色的符卡列表后，传入角色标签，符卡标签和栏位，更换该栏位的符卡
    bool changeSpellCard(const string& spellCardTag, int slot, const string& characterTag);

    // 获得指定角色的攻击方式列表，包括集中和发散两种攻击方式
    vector<Character::Attack> getAttackTypeList(const string& characterTag);

    //获得角色的攻击方式列表后，传入角色标签，更换的攻击方式类型，自动切换为下一个攻击方式
    bool switchAttackType(Character::Attack::Type type, const string& characterTag);

    // int 类型可能不足以存储钱币数量，故使用 long 类型
    long getMoneyNum();

    // num 可为正可为负，为正时增加钱币数，为负时减少钱币数
    void increaseMoney(long num);

    ////////////////////////////////////////////////////////////////////////////////
    // 道具与符卡与符咒 Item / Card / Spell

    // 在 InventoryScene 中使用
    vector<Item> getItemList();

    // 在 EquipScene，InventoryScene 中使用
    Item getItem(const string& itemTag);

    vector<SpellCard> getSpellCardList();
    SpellCard getSpellCard(const string& spellcardTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 资料库

    // 待定

private:
    GameData();
    bool init();
    void FAKE_INIT();

private:
    static GameData* _self;
};

#endif
