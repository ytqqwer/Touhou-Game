#ifndef ITEM_H
#define ITEM_H

#include <string>
using namespace std;

struct Item
{
    enum class Type
    {
        NORMAL,     //普通
        STRENGTHEN, //强化
        SPECIAL,    //特殊
    };

    enum class EquipmentStatus
    {
        UNPURCHASED, //未购买
        UNUSED,      //未使用
        USED,        //已使用
    };

    string tag;         //物品标签
    string name;        //物品名
    string icon;        //图标地址
    string description; //功能介绍

    int maxUseCount;     //使用次数
    int healthPointCost; //生命值消耗
    int manaCost;        //灵力消耗
    float cooldown;      //冷却时间

    string currentUserTag; //当前使用或装备该道具的角色
    int inSlot;            //所在栏位

    Item::Type type;  //普通，强化，特殊
    bool isCarriable; //是否可以携带
    bool isUseable;   //是否可以在gameplay场景中主动使用

    /* 与商店有关的属性 */

    int price;                         //售价
    Item::EquipmentStatus equipStatus; //物品的状态
    string inStore;                    //所在商店

    /* 以下是物品的加成效果 */

    //暂略
};

#endif
