/*
 * 这个文件是 GameData 的类定义。这个头文件可能会有一些看来比较奇怪的地方，以下一一说明。
 *
 *  1. 『前向声明』了很多 *Description *Effect
 *
 *     几乎每个 cpp 文件都会包含此头文件。如果此头文件再去包含其他一大堆头文件，整个项目将会形成非常
 *     庞大和复杂难以管理的编译依赖。如果你做过工程，应该会了解这种庞大又复杂编译依赖对项目的负面影响。
 *
 *  2. 大量使用无具体语义的类型（比如 pair<vector<int>, vector<int> 来返回值
 *
 *     我们要使用这种无语义的类型主要原因在于为每一个非基本类型构建类是一个极其耗费时间的工作，尤其是
 *     在这种类型还比较多的时候。我们的项目没有足够的时间来做这些锦上添花的事情。
 *
 *     我们在开发时会通过
 *        - 集中时间一起开发
 *        - 经常沟通
 *        - 函数返回值的详尽注释
 *     来尽量减少这种无语义类型对代码理解的不利影响。
 *
 *  3. getShortDescription，这是什么东西，和 getDescription 返回类型相同？
 *
 *     有些地方我们不需要关于一个 地图/道具/符卡 的具体信息，只需要它的名字和图片。这个时候 GameData
 *     没有必要去计算一个 地图/道具/符卡 的所有信息，所以我们引入 shortDescription。
 *
 *     shortDescription 仅包含一些基本的信息，未设置的信息会设置为 NULL 以避免误使用。
 */

#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "cocos2d.h"
#include <string>
#include <utility>
#include <vector>

// 前向声明
// 请按字典序排列
struct CardDescription;
struct CharacterDescription;
struct ConversationDescription;
struct MapDescription;
struct PropDescription;
struct SaveDescription;
struct SpellDescription;

USING_NS_CC;
using namespace std;

class GameData
{
public:
    static GameData* getInstance();

    ////////////////////////////////////////////////////////////////////////////////
    // 1. 存档读取与切换 Save

    int getCurrentSaveTag();
    SaveDescription getSaveDescription(int saveTag);
    void saveSave();
    void switchSave(int saveTag);
    void deleteSave(int saveTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 场景背景 BGP(BackGround Picture) / BGM(BackGround Music)

    // 现阶段由且仅由 ResourceLoaderScene 使用，不同的关卡有不同的加载时展示图
    string getSceneBgpName(const string& sceneTag);

    // 现阶段由且仅由 GameplayScene 使用，不同的关卡有不同的背景音乐
    string getSceneBgmName(const string& sceneTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 对话 Conversation

    // 1 为标准速度，1 以下为减缓，1 以上为加快，与 1 的比例为速度相对比
    float getConversationSpeed();
    void setConversationSpeed(float relativeSpeed);

    // 若返回空，则表明这个场景中现无对话
    // 外层 pair.first 为对话编号，pair.second 为对话双方
    // 内层 pair.first 为对话甲方姓名，pair.second 为对话乙方姓名
    // 在 HomeScene，KourindouScene，KoumakanLibraryScene， ArmsStoreScene 中使用
    vector<pair<int, pair<string, string>>> getSceneConversations(const string& sceneTag);

    // Description 包括对话的双方，对话的内容，对话过程中人物的变化
    ConversationDescription getConversationDescription(const string& sceneTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 地图与进度 Map / Process

    pair<string, int> getCurrentMapAndProcessInfo();

    // 返回的 vector<MapDescription> 是仅包含名字，地图预览图，进度的 shortDescription
    // 在 MapSelectScene 中使用
    vector<MapDescription> getMapAndProcessInfoList();

    // 返回这个地图中有什么道路，每个道路名字，每个道路的具体情况介绍
    // 在 RoundSelectScene 中使用
    MapDescription getMapDescription(int mapTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 人物属性与钱币 Character / Money

    // 返回现在可使用的角色 tag 的列表
    vector<int> getAvailableCharacters();

    // shortDescription 仅包含人物的名字，立绘图片名
    // 在 HomeScene，ConversationLayer 中使用
    CharacterDescription getCharacterShortDescription(int characterTag);

    // fullDescription 包含人物名字，头像，立绘，生命，灵力，速度，攻击方式，攻击力等各种人物属性
    // 在 PreparationScene 中使用
    CharacterDescription getCharacterDescription();

    // pair.first 是道具tag, pair.second 是符卡tag
    // 在 HomeScene，PreparationScene 中使用
    pair<vector<int>, vector<int>> getCharacterEquipmentList(int characterTag);

    // int 类型可能不足以存储钱币数量，故使用 long 类型
    long getMoneyNum();

    // num 可为正可为负，为正时增加钱币数，为负时减少钱币数
    void increaseMoney(long num);

    ////////////////////////////////////////////////////////////////////////////////
    // 道具与符卡与符咒 Prop / Card / Spell

    // 返回的 PropDescription 是 fullDescription
    // 在 PropsLibScene 中使用
    vector<PropDescription> getPropList();

    // shortDescription 仅包含道具名字、图片、冷却时间
    // 在 RoundSelectScene 中使用
    PropDescription getPropShortDescription(int propTag);

    // fullDescription 包含道具的所有信息
    // 在 PreparationScene，ProplLibScene 中使用
    PropDescription getPropDescription(int propTag);

    CardDescription getCardShortDescription(int cardTag);
    CardDescription getCardDescription(int cardTag);
    SpellDescription getSpellShortDescription(int spellTag);
    SpellDescription getSpellDescription(int spellTag);

    ////////////////////////////////////////////////////////////////////////////////
    // 资料库

private:
    GameData();
    bool init();
    void FAKE_INIT();

private:
    static GameData* _self;
};

#endif
