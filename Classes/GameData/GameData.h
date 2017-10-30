#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "cocos2d.h"
#include <string>
#include <utility>
#include <vector>

// 前向声明无法声明全称限定的类型名（C++ 的坑）
// 所以不再使用前向声明，直接包含头文件
#include "GameData/Character.h"
#include "GameData/Conversation.h"
#include "GameData/Item.h"
#include "GameData/Location.h"
#include "GameData/Round.h"
#include "GameData/Save.h"
#include "GameData/SpellCard.h"

#include "GameData/EnemyData.h"
#include "GameData/EventData.h"

USING_NS_CC;
using namespace std;

class GameData
{
public:
    static GameData* getInstance();

    ////////////////////////////////////////////////////////////////////////////////
    // 存档读取、切换、覆盖 Save
    // 不提供删除存档功能

    int getCurrentSaveTag();
    vector<Save> getSaveList();

    // 在 MainMenuScene 中由【新游戏】按钮使用。现有存档数已满时【新游戏】失败
    bool newGame();
    // 在 MainMenuScene 中使用
    void continueGame();

    // 返回新存档的 tag
    // 存档数量有上限，若现有存档数已满，创建新存档的操作会失败，返回 -1
    int newSave();

    //将当前存档缓存保存至指定栏位中，并写入文件
    void saveSave(int saveTag);
    //切换存档，用指定存档替换当前的存档缓存
    void switchSave(int saveTag);
    //更新当前的存档缓存
    void updateSave(const std::string& updateTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 背景音乐、特效音量大小 BGM / Effect

    // [0, 1]
    float getSavedBgmVolume();
    void saveBgmVolume(float newVolume);

    float getSavedEffectsVolume();
    void saveEffectsVolume(float newVolume);

    ////////////////////////////////////////////////////////////////////////////////
    // 对话 Conversation

    // 调整对话显示速度，[0, 1]
    float getSavedConversationSpeed();
    void saveConversationSpeed(float relativeSpeed);

    // 返回 vector.empty() = true 时表示这个地点中无对话
    // 在 HomeScene 中使用
    vector<ConversationIndicator> getConversationIndicators(const string& locationTag);

    // 通过对话编号返回详细对话内容，包括对话的双方，对话的内容，对话过程中人物的变化
    // 在 ConversationLayer 中使用
    vector<Dialogue> getConversation(const string& conversationTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 地点关卡与进度 Location / Round / Process
    // 区分 Location / Map / Round , Location 是地点，Map 特指 TMX 地图，Round 则是一个个关卡

    // 获取当前所处地点的详情
    // 在 HomeScene RoundSelectScene 中使用
    Location getCurrentLocation();

    // 获取指定地点的详情
    Location getLocationByTag(const string& locationTag);

    // 返回所有的地点
    // 在 LocationSelectScene 中使用
    vector<Location> getLocationList();

    // 配合 getLocationList 使用，这个接口只返回已解锁地点的 tag
    // 在 LocationSelectScene 中使用
    vector<string> getUnlockedLocationTagList();
    vector<Location> getUnlockedLocationList();

    // 在 LocationSelectScene 中使用
    bool switchLocation(const string& newLocationTag);

    // 返回这个地点包含的关卡信息
    // 在 RoundSelectScene 中使用
    vector<Round> getRoundList(const string& locationTag);

    // 设置要玩的关卡。每个 Round 的 tag 都是唯一的，所以参数中不需要再传入 locationTag
    // 在 RoundSelectScene 中使用
    bool setRoundToPlay(const string& roundTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 人物属性与钱币 Character / Money

    // 返回现在可使用的角色的列表
    vector<Character> getAvailableCharacterList();

    // 返回选定的要出战的角色的列表
    // 在 RoundSelectScene 中使用
    vector<string> getOnStageCharacterTagList();

    // 获得角色的列表后，传入角色标签，自动切换为队伍其他角色
    // 在 RoundSelectScene 中使用
    bool switchOnStageCharacter(int nth, const string& characterTag);

    // 获得指定角色的信息
    Character getCharacterByTag(const string& characterTag);

    // 获得指定角色使用或装备的全部道具
    vector<Item> getCharacterEquipedItems(const string& characterTag);
    // 获得指定角色装备的主动道具
    vector<Item> getCharacterActiveItems(const string& characterTag);
    // 获得指定角色装备的主动道具，带槽位指示信息
    vector<pair<Item, int>> getCharacterActiveItemsInPair(const string& characterTag);
    // 获得指定角色使用的强化道具
    vector<Item> getCharacterStrengthenItems(const string& characterTag);

    //装备主动道具
    void equipActiveItem(const string& characterTag, const string& itemTag, int slot);
    //装备强化道具
    void equipStrengthenItem(const string& characterTag, const string& itemTag);

    // 获得指定角色的符卡列表
    // 包括专属符卡和购买的通用符卡(暂未加以区分)
    vector<SpellCard> getCharacterEquipedSpellCards(const string& characterTag);
    vector<pair<SpellCard, int>> getCharacterEquipedSpellCardsInPair(const string& characterTag);

    //装备符卡
    void equipSpellCard(const string& characterTag, const string& cardTag, int slot);

    //// 获得指定的道具列表后，更换角色装备的道具
    // bool changeItem(const string& characterTag, int slot, const string& itemTag);
    //// 获得角色的符卡列表后，传入角色标签，符卡标签和栏位，更换该栏位的符卡
    // bool changeSpellCard(const string& characterTag, int slot, const string& spellCardTag);

    // 获得指定角色的攻击方式列表，包括集中和发散两种攻击方式
    vector<Character::Attack> getAttackList(const string& characterTag);

    // 获得指定角色的已选择的攻击方式
    vector<Character::Attack> getSelectedAttackList(const string& characterTag);

    // int 类型可能不足以存储钱币数量，故使用 long 类型
    long getMoneyNum();

    // num 可为正可为负，为正时增加钱币数，为负时减少钱币数
    void increaseMoney(long num);

    void unlockItemSlot(const string& characterTag);
    void unlockSpellCardSlot(const string& characterTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 敌人 Enemy

    // 获得指定敌人的信息
    EnemyData getEnemyByTag(const string& enemyTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 道具与符卡与符咒 Item / Card / Spell

    // 符卡和道具本身并没有数量的概念，全部独一无二，因此数量是没有意义的属性
    // 换句话说，即使有数量，那么数量的值也只能是1

    vector<Item> getAvailableItems();
    vector<Item> getAvailableNormalItems();
    vector<Item> getAvailableStrengthenItems();
    vector<Item> getAvailableSpecialItems();

    vector<Item> getAvailableActiveItems();

    vector<Item> getItemsInStore(const string& storeTag);
    vector<Item> getNormalItemsInStore(const string& storeTag);
    vector<Item> getStrengthenItemsInStore(const string& storeTag);
    vector<Item> getSpecialItemsInStore(const string& storeTag);
    vector<SpellCard> getSpellCardsInStore(const string& storeTag);

    vector<SpellCard> getAvailableSpellCards();

    SpellCard getSpellCardByTag(const string& cardTag);
    Item getItemByTag(const string& itemTag);

    void buyItem(const string& itemTag);
    void buySpellCard(const string& cardTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 对话/通关奖励 Award

    void ConversationComplete(const string& conversationTag);
    void RoundComplete(const string& roundTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 资料库

    // 待定

    ////////////////////////////////////////////////////////////////////////////////
    // 事件 EventData
    vector<EventData> getEventListByTag(const string& eventTag);

    ////////////////////////////////////////////////////////////////////////////////
    // Gameplay 场景专用

    Round getRoundToPlay();

    // 获得角色的攻击方式列表后，传入角色标签，更换的攻击方式类型，自动切换为下一个攻击方式
    bool switchAttackType(const string& characterTag, Character::Attack::Type type);

private:
    GameData();
    bool init();

private:
    static GameData* _self;
};

#endif
