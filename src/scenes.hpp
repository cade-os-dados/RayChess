#pragma once
#include "raylib_win32.hpp"
#include <raylib.h>
#include "net/interface.hpp"

enum SCENE
{
    MENU_SCENE,
    NETWORK_SIDE_SCENE,
    GAME_SCENE,
    CONTINUE_SCENE,
    WAIT_CLIENT_RESPONSE_SCENE,
    CLIENT_WRITE_IP_SCENE
};