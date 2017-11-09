#include "lua_conversation_layer.hpp"
#include "../../Classes/Layers/ConversationLayer.h"
#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"

int lua_tg_conversation_layer_ConversationLayer_playScreenEffect(lua_State* tolua_S)
{
    int argc = 0;
    ConversationLayer* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ConversationLayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ConversationLayer*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_tg_conversation_layer_ConversationLayer_playScreenEffect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ConversationLayer:playScreenEffect");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_playScreenEffect'", nullptr);
            return 0;
        }
        ConversationLayer* ret = cobj->playScreenEffect(arg0);
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ConversationLayer:playScreenEffect",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_tg_conversation_layer_ConversationLayer_playScreenEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_tg_conversation_layer_ConversationLayer_setDialogueInterval(lua_State* tolua_S)
{
    int argc = 0;
    ConversationLayer* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ConversationLayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ConversationLayer*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_tg_conversation_layer_ConversationLayer_setDialogueInterval'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ConversationLayer:setDialogueInterval");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_setDialogueInterval'", nullptr);
            return 0;
        }
        cobj->setDialogueInterval(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ConversationLayer:setDialogueInterval",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_tg_conversation_layer_ConversationLayer_setDialogueInterval'.",&tolua_err);
#endif

    return 0;
}
int lua_tg_conversation_layer_ConversationLayer_changeBgp(lua_State* tolua_S)
{
    int argc = 0;
    ConversationLayer* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ConversationLayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ConversationLayer*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_tg_conversation_layer_ConversationLayer_changeBgp'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ConversationLayer:changeBgp");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_changeBgp'", nullptr);
            return 0;
        }
        ConversationLayer* ret = cobj->changeBgp(arg0);
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ConversationLayer:changeBgp",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_tg_conversation_layer_ConversationLayer_changeBgp'.",&tolua_err);
#endif

    return 0;
}
int lua_tg_conversation_layer_ConversationLayer_changeBgm(lua_State* tolua_S)
{
    int argc = 0;
    ConversationLayer* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ConversationLayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ConversationLayer*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_tg_conversation_layer_ConversationLayer_changeBgm'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ConversationLayer:changeBgm");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_changeBgm'", nullptr);
            return 0;
        }
        ConversationLayer* ret = cobj->changeBgm(arg0);
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ConversationLayer:changeBgm",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_tg_conversation_layer_ConversationLayer_changeBgm'.",&tolua_err);
#endif

    return 0;
}
int lua_tg_conversation_layer_ConversationLayer_changeCharacter(lua_State* tolua_S)
{
    int argc = 0;
    ConversationLayer* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ConversationLayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ConversationLayer*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_tg_conversation_layer_ConversationLayer_changeCharacter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        std::string arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ConversationLayer:changeCharacter");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ConversationLayer:changeCharacter");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_changeCharacter'", nullptr);
            return 0;
        }
        ConversationLayer* ret = cobj->changeCharacter(arg0, arg1);
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    if (argc == 3) 
    {
        std::string arg0;
        std::string arg1;
        std::string arg2;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ConversationLayer:changeCharacter");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ConversationLayer:changeCharacter");

        ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ConversationLayer:changeCharacter");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_changeCharacter'", nullptr);
            return 0;
        }
        ConversationLayer* ret = cobj->changeCharacter(arg0, arg1, arg2);
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    if (argc == 4) 
    {
        std::string arg0;
        std::string arg1;
        std::string arg2;
        cocos2d::Color3B arg3;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ConversationLayer:changeCharacter");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ConversationLayer:changeCharacter");

        ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ConversationLayer:changeCharacter");

        ok &= luaval_to_color3b(tolua_S, 5, &arg3, "ConversationLayer:changeCharacter");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_changeCharacter'", nullptr);
            return 0;
        }
        ConversationLayer* ret = cobj->changeCharacter(arg0, arg1, arg2, arg3);
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    if (argc == 5) 
    {
        std::string arg0;
        std::string arg1;
        std::string arg2;
        cocos2d::Color3B arg3;
        std::string arg4;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ConversationLayer:changeCharacter");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ConversationLayer:changeCharacter");

        ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ConversationLayer:changeCharacter");

        ok &= luaval_to_color3b(tolua_S, 5, &arg3, "ConversationLayer:changeCharacter");

        ok &= luaval_to_std_string(tolua_S, 6,&arg4, "ConversationLayer:changeCharacter");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_changeCharacter'", nullptr);
            return 0;
        }
        ConversationLayer* ret = cobj->changeCharacter(arg0, arg1, arg2, arg3, arg4);
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    if (argc == 6) 
    {
        std::string arg0;
        std::string arg1;
        std::string arg2;
        cocos2d::Color3B arg3;
        std::string arg4;
        cocos2d::Vec2 arg5;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ConversationLayer:changeCharacter");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ConversationLayer:changeCharacter");

        ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ConversationLayer:changeCharacter");

        ok &= luaval_to_color3b(tolua_S, 5, &arg3, "ConversationLayer:changeCharacter");

        ok &= luaval_to_std_string(tolua_S, 6,&arg4, "ConversationLayer:changeCharacter");

        ok &= luaval_to_vec2(tolua_S, 7, &arg5, "ConversationLayer:changeCharacter");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_changeCharacter'", nullptr);
            return 0;
        }
        ConversationLayer* ret = cobj->changeCharacter(arg0, arg1, arg2, arg3, arg4, arg5);
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ConversationLayer:changeCharacter",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_tg_conversation_layer_ConversationLayer_changeCharacter'.",&tolua_err);
#endif

    return 0;
}
int lua_tg_conversation_layer_ConversationLayer_setPauseNode(lua_State* tolua_S)
{
    int argc = 0;
    ConversationLayer* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ConversationLayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ConversationLayer*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_tg_conversation_layer_ConversationLayer_setPauseNode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Node* arg0;

        ok &= luaval_to_object<cocos2d::Node>(tolua_S, 2, "cc.Node",&arg0, "ConversationLayer:setPauseNode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_setPauseNode'", nullptr);
            return 0;
        }
        cobj->setPauseNode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ConversationLayer:setPauseNode",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_tg_conversation_layer_ConversationLayer_setPauseNode'.",&tolua_err);
#endif

    return 0;
}
int lua_tg_conversation_layer_ConversationLayer_playSoundEffect(lua_State* tolua_S)
{
    int argc = 0;
    ConversationLayer* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ConversationLayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ConversationLayer*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_tg_conversation_layer_ConversationLayer_playSoundEffect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ConversationLayer:playSoundEffect");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_playSoundEffect'", nullptr);
            return 0;
        }
        ConversationLayer* ret = cobj->playSoundEffect(arg0);
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ConversationLayer:playSoundEffect",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_tg_conversation_layer_ConversationLayer_playSoundEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_tg_conversation_layer_ConversationLayer_changeSpeakerColor(lua_State* tolua_S)
{
    int argc = 0;
    ConversationLayer* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ConversationLayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ConversationLayer*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_tg_conversation_layer_ConversationLayer_changeSpeakerColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::Color3B arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ConversationLayer:changeSpeakerColor");

        ok &= luaval_to_color3b(tolua_S, 3, &arg1, "ConversationLayer:changeSpeakerColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_changeSpeakerColor'", nullptr);
            return 0;
        }
        ConversationLayer* ret = cobj->changeSpeakerColor(arg0, arg1);
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ConversationLayer:changeSpeakerColor",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_tg_conversation_layer_ConversationLayer_changeSpeakerColor'.",&tolua_err);
#endif

    return 0;
}
int lua_tg_conversation_layer_ConversationLayer_changeText(lua_State* tolua_S)
{
    int argc = 0;
    ConversationLayer* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ConversationLayer",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ConversationLayer*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_tg_conversation_layer_ConversationLayer_changeText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        std::string arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ConversationLayer:changeText");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ConversationLayer:changeText");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_changeText'", nullptr);
            return 0;
        }
        ConversationLayer* ret = cobj->changeText(arg0, arg1);
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ConversationLayer:changeText",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_tg_conversation_layer_ConversationLayer_changeText'.",&tolua_err);
#endif

    return 0;
}
int lua_tg_conversation_layer_ConversationLayer_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ConversationLayer",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ConversationLayer:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_create'", nullptr);
            return 0;
        }
        ConversationLayer* ret = ConversationLayer::create(arg0);
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ConversationLayer:create",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_tg_conversation_layer_ConversationLayer_create'.",&tolua_err);
#endif
    return 0;
}
int lua_tg_conversation_layer_ConversationLayer_createDebugScene(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ConversationLayer",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_createDebugScene'", nullptr);
            return 0;
        }
        cocos2d::Scene* ret = ConversationLayer::createDebugScene();
        object_to_luaval<cocos2d::Scene>(tolua_S, "cc.Scene",(cocos2d::Scene*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ConversationLayer:createDebugScene",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_tg_conversation_layer_ConversationLayer_createDebugScene'.",&tolua_err);
#endif
    return 0;
}
int lua_tg_conversation_layer_ConversationLayer_currInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ConversationLayer",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_tg_conversation_layer_ConversationLayer_currInstance'", nullptr);
            return 0;
        }
        ConversationLayer* ret = ConversationLayer::currInstance();
        object_to_luaval<ConversationLayer>(tolua_S, "ConversationLayer",(ConversationLayer*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ConversationLayer:currInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_tg_conversation_layer_ConversationLayer_currInstance'.",&tolua_err);
#endif
    return 0;
}
static int lua_tg_conversation_layer_ConversationLayer_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (ConversationLayer)");
    return 0;
}

int lua_register_tg_conversation_layer_ConversationLayer(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ConversationLayer");
    tolua_cclass(tolua_S,"ConversationLayer","ConversationLayer","cc.Layer",nullptr);

    tolua_beginmodule(tolua_S,"ConversationLayer");
        tolua_function(tolua_S,"playScreenEffect",lua_tg_conversation_layer_ConversationLayer_playScreenEffect);
        tolua_function(tolua_S,"setDialogueInterval",lua_tg_conversation_layer_ConversationLayer_setDialogueInterval);
        tolua_function(tolua_S,"changeBgp",lua_tg_conversation_layer_ConversationLayer_changeBgp);
        tolua_function(tolua_S,"changeBgm",lua_tg_conversation_layer_ConversationLayer_changeBgm);
        tolua_function(tolua_S,"changeCharacter",lua_tg_conversation_layer_ConversationLayer_changeCharacter);
        tolua_function(tolua_S,"setPauseNode",lua_tg_conversation_layer_ConversationLayer_setPauseNode);
        tolua_function(tolua_S,"playSoundEffect",lua_tg_conversation_layer_ConversationLayer_playSoundEffect);
        tolua_function(tolua_S,"changeSpeakerColor",lua_tg_conversation_layer_ConversationLayer_changeSpeakerColor);
        tolua_function(tolua_S,"changeText",lua_tg_conversation_layer_ConversationLayer_changeText);
        tolua_function(tolua_S,"create", lua_tg_conversation_layer_ConversationLayer_create);
        tolua_function(tolua_S,"createDebugScene", lua_tg_conversation_layer_ConversationLayer_createDebugScene);
        tolua_function(tolua_S,"currInstance", lua_tg_conversation_layer_ConversationLayer_currInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(ConversationLayer).name();
    g_luaType[typeName] = "ConversationLayer";
    g_typeCast["ConversationLayer"] = "ConversationLayer";
    return 1;
}
TOLUA_API int register_all_tg_conversation_layer(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"tg",0);
	tolua_beginmodule(tolua_S,"tg");

	lua_register_tg_conversation_layer_ConversationLayer(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

