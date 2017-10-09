#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

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

    int price; //售价
};

#endif
