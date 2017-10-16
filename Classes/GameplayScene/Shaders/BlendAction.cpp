#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "GameplayScene/Shaders/BlendAction.h"

BlendAction*
BlendAction::create(float duration, Color4B blendColor, bool reverse)
{
    BlendAction* bA = new (std::nothrow) BlendAction();
    if (bA && bA->initWithDuration(duration, blendColor, reverse)) {
        bA->autorelease();
        return bA;
    }

    delete bA;
    return nullptr;
}

void
BlendAction::startWithTarget(Node* target)
{
    ActionInterval::startWithTarget(target);

    _pstate = target->getGLProgramState();
    _pstate->retain();

    auto program =
        GLProgram::createWithFilenames("Shaders/blend_color.vsh", "Shaders/blend_color.fsh");
    auto pstate = GLProgramState::create(program);
    target->setGLProgramState(pstate);

    pstate->setUniformCallback("u_alpha", [this](GLProgram* p, Uniform* u) {
        p->setUniformLocationWith1f(u->location, _percent);
    });
    auto blendColor = Vec4(_color.r / 255.0, _color.g / 255.0, _color.b / 255.0, _color.a / 255.0);
    pstate->setUniformVec4("u_blendColor", blendColor);
}

void
BlendAction::update(float t)
{
    if (_target && !isDone()) {
        _percent = _reverse ? 1.0 - t : t;
    }
}

void
BlendAction::stop()
{
    getTarget()->setGLProgramState(_pstate);
    _pstate->autorelease();
}

BlendAction*
BlendAction::clone() const
{
    return BlendAction::create(_duration, _color, _reverse);
}

BlendAction*
BlendAction::reverse(void) const
{
    return BlendAction::create(_duration, _color, -_reverse);
}

bool
BlendAction::initWithDuration(float duration, Color4B blendColor, bool reverse)
{
    if (ActionInterval::initWithDuration(duration)) {
        _color = blendColor;
        _reverse = reverse;
        return true;
    }

    return false;
}
