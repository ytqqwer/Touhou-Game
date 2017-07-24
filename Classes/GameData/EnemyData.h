#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef ENEMYDATA_H
#define ENEMYDATA_H

#include <string>
#include <vector>
using namespace std;

struct EnemyData
{
    string tag;  //敌人标签
    string name; //敌人名字
    //属性
    int healthPoint;
};

#endif
