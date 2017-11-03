#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef BOSS_HP_BAR_H
#define BOSS_HP_BAR_H

#include "cocos2d.h"

class BossHpBar : public cocos2d::Sprite
{
public:
    static BossHpBar* create(const Node* target, const int maxHpValue, const std::string& face);

    virtual bool init() override;

    //移除注册有监听器的子节点要更加小心处理，及时手动移除
    //如果在BOSS持续受到伤害时移除并清理BOSS血条
    //由于移除子节点时只是停止调度器和停止回调，并不会自动停止监听器
    //监听器仍在接受事件，而其事件处理代码中捕获的变量却已经被释放
    //故提供此函数移除监听器
    //顺带一提，大多数的监听器都是在进入或离开场景前就加载好的，移除时直接全部移除。
    void removeListener();

private:
    BossHpBar(const Node* target, const int maxHpValue, const std::string& face);

private:
    const Node* _target;
    std::string _face;

    int _maxHp;
    Sprite* _hpBar;
    Sprite* _hpStencil;
    Sprite* _hpBarBlack;
    cocos2d::ClippingNode* hpClippingNode;

    Sprite* avatar;
    Sprite* centerObject;
    Sprite* lifeGage;

    cocos2d::EventListenerCustom* customListener;
};

#endif // BOSS_HP_BAR_H
