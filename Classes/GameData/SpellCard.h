#ifndef SPELL_CARD_H
#define SPELL_CARD_H

#include "Item.h"
#include <string>
#include <vector>
using namespace std;

struct SpellCard
{
    string tag;         //标签
    string name;        //符卡名
    string icon;        //图标地址
    string description; //功能介绍

    int manaCost;   //灵力消耗
    float cooldown; //冷却时间

    string exclusiveCharacterTag; //角色专属符卡，如果为空则说明是购买的通用符卡
                                  //通用符卡将显示在所有角色的符卡列表中
    string currentUserTag; //当前装备该符卡的角色，用来判断符卡是否重复选择
    int inSlot;            //所在栏位

    /* 与商店有关的属性 */

    int price;                    //售价
    Item::EquipmentStatus status; //符卡的状态
    string inStore;               //所在商店
};

#endif
