#ifndef RENDERABLEH
#define RENDERABLEH

#include <vector>

#include "vector.h"

struct tri { int i0, i1, i2; uint32_t c; };

class Renderable
{
public:
    Renderable(std::vector<vec3> verts, std::vector<tri> tris);

    bool isWireframe;

    std::vector<vec3> verts() const { return m_verts; }
    std::vector<tri> tris() const { return m_tris; }

private:
    std::vector<vec3> m_verts;
    std::vector<tri> m_tris;
};

Renderable::Renderable(std::vector<vec3> verts, std::vector<tri> tris)
{
    std::cout << "New renderable with " << verts.size() << " verts and " << tris.size() << " tris" << std::endl;
    m_verts = verts;
    m_tris = tris;
    isWireframe = false;
}


#endif
