#include "AudioController.h"

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

AudioController::AudioController()
{
}

void
AudioController::clearMusic()
{
    currentMusic = "";
    formerMusic = "";
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

bool
AudioController::isPlayingMusic()
{
    return SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
}

void
AudioController::playMusic(std::string music, bool loop)
{
    if (currentMusic != music) {
        SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        SimpleAudioEngine::getInstance()->playBackgroundMusic(music.c_str(), loop);
        currentMusic = music;
    }
}

void
AudioController::stopMusic()
{
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void
AudioController::resumeMusic()
{
    SimpleAudioEngine::getInstance()->playBackgroundMusic(currentMusic.c_str(), true);
}

std::string
AudioController::getCurrentMusic()
{
    return currentMusic;
}

void
AudioController::recordFormerMusic()
{
    formerMusic = currentMusic;
}

void
AudioController::resumeFormerMusic()
{
    if (currentMusic != formerMusic) {
        playMusic(formerMusic, true);
    }
}

unsigned int
AudioController::playEffect(std::string effect)
{
    return SimpleAudioEngine::getInstance()->playEffect(effect.c_str());
}

void
AudioController::stopEffect(unsigned int effect)
{
    SimpleAudioEngine::getInstance()->stopEffect(effect);
}

void
AudioController::playClickButtonEffect()
{
    SimpleAudioEngine::getInstance()->playEffect("se/button_click.wav");
}

void
AudioController::playReturnButtonEffect()
{
    SimpleAudioEngine::getInstance()->playEffect("se/back_click.wav");
}

AudioController*
AudioController::getInstance()
{
    static AudioController instance;
    return &instance;
}