#include "AudioController.h"

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

AudioController::AudioController()
{
}

bool
AudioController::isPlayingMusic()
{
    return SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
}

void
AudioController::playMusic(std::string music, bool loop)
{
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    SimpleAudioEngine::getInstance()->playBackgroundMusic(music.c_str(), loop);
    currentMusic = music;
}

void
AudioController::playEffect(std::string effect)
{
    SimpleAudioEngine::getInstance()->playEffect(effect.c_str());
}

void
AudioController::playClickButtonEffect()
{
    SimpleAudioEngine::getInstance()->playEffect("button_click.wav");
}

void
AudioController::playReturnButtonEffect()
{
    SimpleAudioEngine::getInstance()->playEffect("back_click.wav");
}

void
AudioController::stopMusic()
{
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

std::string
AudioController::getCurrentMusic()
{
    return currentMusic;
}

AudioController*
AudioController::getInstance()
{
    static AudioController instance;
    return &instance;
}