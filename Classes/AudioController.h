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

    void clearMusic();
    bool isPlayingMusic();
    void playMusic(const std::string&, bool);
    void stopMusic();
    void resumeMusic();
    std::string getCurrentMusic();
    void recordFormerMusic();
    void resumeFormerMusic();

    unsigned int playEffect(const std::string&);
    void stopEffect(unsigned int);
    void playClickButtonEffect();
    void playReturnButtonEffect();

private:
    std::string currentMusic;
    std::string formerMusic;
};

#endif
