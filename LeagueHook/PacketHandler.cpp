#include "PacketHandler.h"
#include "NetworkDebug.h"

#include "Globals.h"
#include "Logger.h"
#include "ImGuiRender.h"

#include <vector>
#include <map>

std::map<NetworkCmd, PacketHandleFunc> handlers;

class PacketLogPrint : public Packet
{
public:
    char text[512];
};

class PacketDrawCircle : public Packet
{
public:
    float x, y, z, radius, lifeTime;
};

void HandleLogPrint( PacketLogPrint *p )
{
    Globals::Log->LogInfo( "ND: %s (0x%x, 0x%x)", p->text, p->text[0], p->text[1] );
}

void HandleDrawCircle( PacketDrawCircle *p )
{
    AddCircle( p->x, p->y, p->z, p->radius, p->lifeTime );
}

void InitializeHandlers()
{
    handlers[kPrintLog] = ( PacketHandleFunc )HandleLogPrint;
    handlers[kDrawCircle] = ( PacketHandleFunc )HandleDrawCircle;
}

void HandlePacket( Packet *p )
{
    if ( handlers.find( p->id ) == handlers.end() )
    {
        Globals::Log->LogWarning( "Received NetworkDebug packet %d (0x%x) without a handler?", p->id, p->id );
        return;
    }

    Globals::Log->LogDebug( "Handling packet %d (0x%x)", p->id, p->id );
    handlers[p->id]( p );
}