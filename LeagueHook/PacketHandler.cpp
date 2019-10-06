#include "PacketHandler.h"
#include "NetworkDebug.h"

#include "Globals.h"
#include "Logger.h"
#include "ImGuiRender.h"

#include <vector>
#include <map>
#include "mathfu/vector.h"

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
    unsigned char r, g, b;
};

class PacketDrawLine : public Packet
{
public:
    float x1, y1, z1, x2, y2, z2, thick, lifeTime;
    unsigned char r, g, b;
};

void HandleLogPrint( PacketLogPrint *p )
{
    Globals::Log->LogInfo( "NetDebug: %s", p->text );
}

void HandleDrawCircle( PacketDrawCircle *p )
{
    Globals::Overlay->AddCircle( Vector<float, 3>( p->x, p->y, p->z ), Vector<unsigned char, 3>( p->r, p->g, p->b ), p->radius, p->lifeTime );
}

void HandleDrawLine( PacketDrawLine *p )
{
    Globals::Overlay->AddLine( Vector<float, 3>( p->x1, p->y1, p->z1 ), Vector<float, 3>( p->x2, p->y2, p->z2 ), Vector<unsigned char, 3>( p->r, p->g, p->b ), p->thick, p->lifeTime );
}

void InitializeHandlers()
{
    handlers[kPrintLog] = ( PacketHandleFunc )HandleLogPrint;
    handlers[kDrawCircle] = ( PacketHandleFunc )HandleDrawCircle;
    handlers[kDrawLine] = ( PacketHandleFunc )HandleDrawLine;
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