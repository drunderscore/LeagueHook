#pragma once
#include "NetworkDebug.h"

class Packet
{
public:
    NetworkCmd id;
};

void InitializeHandlers();
typedef void( *PacketHandleFunc )( Packet *p );
void HandlePacket( Packet *p );