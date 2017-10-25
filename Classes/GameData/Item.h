#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ITEM_H
#define ITEM_H

#include <string>
using namespace std;

struct Item
{
    enum class Type
    {
        NORMAL,
        STRENGTHEN,
        SPECIAL,
        OTHER,
    };

    string tag;         //物品标签
    string name;        //物品名
    string icon;        //图标地址
    string description; //功能介绍
    Type type;
    bool isCarriable; //是否可以携带
    bool isUseable;   //是否可以在gameplay场景中主动使用

    int maxUseCount;     //使用次数
    int healthPointCost; //生命值消耗
    int manaCost;        //灵力消耗
    float cooldown;      //冷却时间

    int price; //售价
};

#endif
