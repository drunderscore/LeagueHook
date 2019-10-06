#pragma once
#include "NetworkDebug.h"
#include "PacketHandler.h"
#include "Logger.h"
#include "Globals.h"

NetworkDebug::NetworkDebug( const char *ip, unsigned short port )
{
    this->ip = ip;
    this->port = port;

    WSADATA wsa;
    if ( WSAStartup( MAKEWORD( 2, 0 ), &wsa ) )
    {
        Globals::Log->LogError( "Unable to initialize WSA for NetworkDebug!" );
        return;
    }

    if ( ( sock = socket( AF_INET, SOCK_DGRAM, 0 ) ) == -1 )
    {
        Globals::Log->LogError( "Unable to initialize socket for NetworkDebug to %s:%d", ip, port );
        return;
    }

    if ( !InetPton( AF_INET, ip, &addr.sin_addr.s_addr ) )
        Globals::Log->LogWarning( "failed to convert address?" );
    addr.sin_family = AF_INET;
    addr.sin_port = htons( port );

    int ret = 0;
    if ( ( ret = connect( sock, ( SOCKADDR * )&addr, sizeof( addr ) ) ) == -1 )
    {
        Globals::Log->LogError( "Unable to connect NetworkDebug to %s:%d (returned %d)", ip, port, ret );
        return;
    }

    isReady = true;

    Globals::Log->LogInfo( "NetworkDebug successful for %s:%d", ip, port );
    NetworkLoop();
}

void NetworkDebug::NetworkLoop()
{
    int size;
    char data[1024 * 8];
    char tell[1] { kAck };
    send( sock, tell, sizeof( tell ), 0 );
    while ( isReady )
    {
        if ( ( size = recv( sock, data, sizeof( data ), 0 ) ) != -1 )
        {
            Packet *p = ( Packet * )data;
            HandlePacket( p );
        }
    }
    Globals::Log->LogDebug( "NetworkDebug cleanup..." );
    closesocket( sock );
    WSACleanup();
}

void NetworkDebug::Close()
{
    isReady = false;
    Globals::Log->LogInfo( "Closing NetworkDebug for socket %s:%d", ip.c_str(), port );
}

bool NetworkDebug::IsReady()
{
    return isReady;
}

void NetworkDebug::Init()
{
    Globals::Log->LogInfo( "Preparing to init general NetworkDebug" );
    InitializeHandlers();
    Globals::Debug = new NetworkDebug( "127.0.0.1", 37645 );
}