#pragma once

#include "R3DRenderer.h"
#include "GameStateInstance.h"
#include "Packet.h"

extern R3DRenderer *g_Renderer;
extern GameStateInstance *g_GameStateInstance;
extern void *g_ProcessPacket;
extern void *g_Client;
extern void *g_GameTime;
extern GameObject *g_LocalPlayer;

class SDK
{
public:
    static auto Init() -> void;
};