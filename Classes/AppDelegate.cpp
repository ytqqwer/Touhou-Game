#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "AppDelegate.h"
#include "GameData.h"
#include "LogoAndDisclaimerScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void
AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,
// don't modify or remove this function
static int
register_all_packages()
{
    return 0; // flag for packages manager
}

bool
AppDelegate::applicationDidFinishLaunching()
{

    /*  0. some definitions */

    auto designResolution = Size(1280, 720);

    /*  1. director and glview */

    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();

    if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) ||        \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect(
            "TohouGame", cocos2d::Rect(0, 0, designResolution.width, designResolution.height));
#else
        glview = GLViewImpl::create("TohouGame");
#endif
        director->setOpenGLView(glview);
    }

    /*  2. set and FPS & display it on screen */

    director->setDisplayStats(true);
    director->setAnimationInterval(1.0f / 60);

    /*  3. resolution and resource */

    glview->setDesignResolutionSize(designResolution.width, designResolution.height,
                                    ResolutionPolicy::FIXED_WIDTH);
    auto physicalSize = glview->getFrameSize();
#ifndef NDEBUG
    log(">> physical size of the device is %.0f x %.0f", physicalSize.width, physicalSize.height);
#endif

    /*  4. other packages? just leave it alone */

    register_all_packages();

    /*  5. configure game */

    auto audioEngine = CocosDenshion::SimpleAudioEngine::getInstance();
    audioEngine->setBackgroundMusicVolume(GameData::getInstance()->getSavedBgmVolume());
    audioEngine->setEffectsVolume(GameData::getInstance()->getSavedEffectsVolume());

    /*  6. run with scence */

    auto scene = LogoAndDisclaimerScene::create();
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is
// invoked.
void
AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be paused
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void
AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
