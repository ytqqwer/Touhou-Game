#ifndef ITEM_H
#define ITEM_H

#include <string>
using namespace std;

struct Item
{
    string tag;         //物品标签
    string name;        //物品名
    string icon;        //图标地址
    string description; //功能介绍

    int maxUseCount;     //使用次数
    int healthPointCost; //生命值消耗，为负时减，为正时加
    int manaCost;        //灵力消耗，为负时减，为正时加
    float cooldown;      //冷却时间

    bool isCarriable; //是否可以携带
    bool isUseable;   //是否可以在gameplay场景中主动使用

    int price; //售价
    bool isSpecial;
};

#endif
