#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/EventScriptHanding.h"
#include "GameplayScene/GameplayScene.h"
#include "GameplayScene/Player/Player.h"
#include "Layers/ConversationLayer.h"

#include "NonGameplayScenes/HomeScene.h"

EventScriptHanding::EventScriptHanding(Node* node)
{
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(
        "trigger_event", [this](EventCustom* e) { this->eventHandling(e); });
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(
        "conversation_end", [this](EventCustom* e) { this->nextEvent(); });

    this->owner = node;
}

void
EventScriptHanding::eventHandling(EventCustom* e)
{
    auto eventTag = (char*)e->getUserData();
    eventList = GameData::getInstance()->getEventListByTag(eventTag);
    _curEventIndex = 0;
    nextEvent();
}

void
EventScriptHanding::nextEvent()
{
    if (_curEventIndex == eventList.size()) {
        return;
    }

    if (eventList[_curEventIndex].eventType == "conversation") {
        //因为EventScriptHanding是GameplayScene类的友元，所以可以访问私有成员。
        auto node = (GameplayScene*)owner;
        auto layer = ConversationLayer::create(eventList[_curEventIndex].conversationTag);
        layer->setPauseNode(node->mapLayer);
        node->mapLayer->onExit();
        node->addChild(layer, 1000);
        _curEventIndex++;
    } else if (eventList[_curEventIndex].eventType == "action") {
        float totalTime = 0;
        while (_curEventIndex < eventList.size()) {
            if (eventList[_curEventIndex].eventType == "action") {
                float delay = eventList[_curEventIndex].delay;
                float duration = eventList[_curEventIndex].duration;
                if (totalTime < delay + duration) {
                    totalTime = delay + duration;
                }
                eventActionHandling(delay, duration);

                _curEventIndex++;
            } else {
                break;
            }
        }
        auto done = CallFuncN::create(CC_CALLBACK_0(EventScriptHanding::nextEvent, this));
        Sequence* sequence = Sequence::create(DelayTime::create(totalTime), done, NULL);
        auto node = (GameplayScene*)owner;
        node->runAction(sequence);
    } else if (eventList[_curEventIndex].eventType == "update") {
        GameData::getInstance()->updateSave(eventList[_curEventIndex].updateTag);
        _curEventIndex++;
        nextEvent();
    } else if (eventList[_curEventIndex].eventType == "end") {
        Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
        Director::getInstance()->popToRootScene();
        Director::getInstance()->replaceScene(HomeScene::create());
    }

    return;
}

void
EventScriptHanding::eventActionHandling(float delay, float duration)
{
    auto node = (GameplayScene*)owner;
    Sequence* sequence;
    if (eventList[_curEventIndex].jump) {
        auto moveBy = MoveBy::create(duration, Point(-200, 0));
        auto jump = CallFuncN::create(CC_CALLBACK_0(Player::jump, node->curPlayer));
        sequence = Sequence::create(DelayTime::create(delay), jump, moveBy, NULL);
    } else {
        auto moveBy = MoveBy::create(duration, Point(300, 0));
        sequence = Sequence::create(DelayTime::create(delay), moveBy, NULL);
    }
    node->curPlayer->runAction(sequence);
}