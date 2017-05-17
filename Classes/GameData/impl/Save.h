#ifndef SAVE_H
#define SAVE_H

#include "GameData/Character.h"
#include "GameData/Conversation.h"
#include "GameData/Item.h"
#include "GameData/Location.h"
#include "GameData/SpellCard.h"
#include "cocos2d.h"
#include <string>
#include <vector>

using namespace std;

namespace GameDataImpl {

class Save
{
public:
    static Save* getInstance();

private:
    Save() = default;
    bool init();

private:
    static Save* _self;

private:
    string tag;         //存档标签
    string name;        //存档命名
    string time;        //存档时间
    string locationTag; //当前所处地点或位置的标签

    vector<Character> characterList;
    vector<Location> unlockedLocationList; //已解锁的地点
    vector<Item> availableItemList; //玩家当前可以接触到的所有道具，包括商店中未购买的道具
    vector<SpellCard> spellCardList; //当前拥有的符卡列表，包括角色的专属符卡和商店中未购买的符卡
    vector<Character::Attack> attackList;

    long money;
};

} // namespace GameDataImpl

#endif
