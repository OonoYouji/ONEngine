#pragma once

/*
* DirectInputのバージョンを指定
*/

#ifdef DIRECTINPUT_VERSION
#undef DIRECTINPUT_VERSION
#endif
#define DIRECTINPUT_VERSION 0x0800

#include <Windows.h> 
#include <dinput.h>
#include <wrl/client.h>
