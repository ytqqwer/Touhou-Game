#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#ifndef AUDIO_CONTROLLER_H
#define AUDIO_CONTROLLER_H

#include "cocos2d.h"
#include <string>
USING_NS_CC;

class AudioController
{
public:
    static AudioController* getInstance();

    AudioController();

    bool isPlayingMusic();
    void playMusic(std::string, bool);
    void playEffect(std::string);
    void playClickButtonEffect();
    void playReturnButtonEffect();
    void stopMusic();
    std::string getCurrentMusic();

private:
    std::string currentMusic;
};

#endif
