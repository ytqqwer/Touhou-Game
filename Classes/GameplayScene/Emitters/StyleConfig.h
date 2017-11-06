#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef STYLE_CONFIG_H
#define STYLE_CONFIG_H

#include <string>

using namespace std;

enum class StyleType
{
    LASER,
    ODDEVEN,
    PARABOLA,
    PARALLEL,
    SCATTER,
};

struct BulletConfig
{
    string name;
    int harm;
    int length;
    int width;
    int _categoryBitmask;
    int _collisionBitmask;
    int _contactTestBitmask;
};

struct StyleConfig
{

    /* Style类型 */
    StyleType style;

    /* 公共参数 */
    float bulletDuration;            //子弹飞行时间
    float frequency;                 //发射频率
    unsigned int countThenChangePos; //连发后更新位置
    unsigned int number;             //条数

    float totalDuration;     //总持续时间
    unsigned int cycleTimes; //总发射次数

    /* 子弹参数 */
    BulletConfig bc;

    /* Parabola参数 */
    int height;   //高度
    int distance; // 距离

    /* Parallel参数 */
    float interval; //间距，以36像素为单位

    /* Scatter参数 */
    int startAngle; //起始角度
    int endAngle;   //终止角度

    int deltaAngle; //角度增量
};

#endif