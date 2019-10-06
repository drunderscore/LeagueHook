#pragma once
#include "mathfu/vector.h"
#include <vector>
#include <mutex>

class DebugOverlay; // forward declare because i want things at the top.
struct ImDrawList;

using namespace mathfu;

class OverlayBase
{
public:
    OverlayBase( Vector<unsigned char, 3> col, float lifetime ) : color( col ), maxLifetime( lifetime ), curLifetime( 0.0f ) {}
    Vector<unsigned char, 3> color;
    float curLifetime;
    float maxLifetime;
    virtual void Draw( float delta, ImDrawList *drawList ) = 0;
};

class OverlayCircle : public OverlayBase
{
public:
    OverlayCircle( Vector<float, 3> pos, float radius, Vector<unsigned char, 3> col, float lifetime ) : pos( pos ), radius( radius ), OverlayBase( col, lifetime ) {}
    Vector<float, 3> pos;
    float radius;
    virtual void Draw( float delta, ImDrawList *drawList );
};

class OverlayLine : public OverlayBase
{
public:
    OverlayLine( Vector<float, 3> pos1, Vector<float, 3> pos2, Vector<unsigned char, 3> col, float thick, float lifetime ) : pos1( pos1 ), pos2( pos2 ), thick( thick ), OverlayBase( col, lifetime ) {}
    Vector<float, 3> pos1;
    Vector<float, 3> pos2;
    float thick;
    virtual void Draw( float delta, ImDrawList *drawList );
};

class DebugOverlay
{
private:
    std::vector<OverlayBase *> overlays;
    std::mutex overlays_mutex;
public:
    static void Init();
    void AddCircle( Vector<float, 3> pos, Vector<unsigned char, 3> col, float radius = 5.0f, float lifeTime = 3.0f );
    void AddLine( Vector<float, 3> pos1, Vector<float, 3> pos2, Vector<unsigned char, 3> col, float thick = 1.0f, float lifeTime = 3.0f );
    void Draw( float delta );
};