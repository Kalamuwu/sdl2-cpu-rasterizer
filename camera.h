#ifndef CAMERAH
#define CAMERAH

#include <vector>

#include "vector.h"
#include "macros.h"
#include "screen.h"
#include "renderable.h"

struct xy {
    int x;
    int y;
};

class Camera
{
public:
    Camera(vec3 lookfrom, vec3 lookat, vec3 up, float vfov, int image_width, int image_height);

    Screen* screen() { return mp_screen; }

    // screen info getters
    int window_width() const { return mp_screen->width; }
    int window_height() const { return mp_screen->height; }

    // cam info getters
    double aspect() const { return m_aspect; }
    double vfov() const { return m_vfov; }
    double focal() const { return m_focal; }

    // drawers
    void point(vec3 pt, uint32_t c);
    void line(vec3 pt0, vec3 pt1, uint32_t c);
    void triangleWireframe(vec3 pt0, vec3 pt1, vec3 pt2, uint32_t c);
    void triangleFilled(vec3 pt0, vec3 pt1, vec3 pt2, uint32_t c);
    void render(Renderable* pObj);

    // movers
    void translate(vec3 delta);

private:

    Screen* mp_screen;

    double m_aspect;           // Aspect ratio
    double m_vfov;             // Vertical fov
    double m_focal;            // Focal length
    double m_viewport_height;  // Height of viewport, in world-space units
    double m_viewport_width;   // Width of viewport, in world-space units

    vec3 m_lookfrom;           // Camera origin
    vec3 m_lookat;             // Point to look towards
    vec3 m_vup;                // Normalized "up" vector
    vec3 u, v, w;              // Normalized camera frame basis vectors

    vec3 pixel_du;             // Distance traveled u for one pixel horizontally
    vec3 pixel_dv;             // Distance traveled v for one pixel vertically
    vec3 m_00_loc;             // Location of 0,0 pixel

    xy project(vec3 loc);

};

Camera::Camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, int image_width, int image_height)
{
    // setup screen
    m_aspect = (float)image_width / image_height;
    mp_screen = new Screen(image_width, image_height, WINDOW_ZOOM);
    mp_screen->clear(); mp_screen->show();
    mp_screen->setTitle("Camera");

    // save camera vector variables
    m_vfov = vfov;
    m_lookfrom = lookfrom;
    m_lookat = lookat;
    m_vup = vup;

    // calculate frame basis vectors
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(m_vup, w));
    v = unit_vector(cross(w, u));

    // calculate view angles and lengths and shit
    m_focal = w.length();
    float theta = m_vfov * M_PI/180;
    float half_height = tan(theta/2);
    m_viewport_height = m_focal * 2*half_height;
    m_viewport_width = m_viewport_height * m_aspect;

    // calculate screen edge vectors
    vec3 viewport_u = m_viewport_width  *  u;  // Vector from 0,0 across horizontal top edge
    vec3 viewport_v = m_viewport_height * -v;  // Vector from 0,0 down vertical left edge

    // calculate pixel delta vectors
    pixel_du = viewport_u / image_width;
    pixel_dv = viewport_v / image_height;

    // finally... calculate location of 0,0
    vec3 viewport_upper_left = m_lookfrom - (m_focal * w) - viewport_u/2 - viewport_v/2;
    m_00_loc = viewport_upper_left + 0.5*(pixel_du+pixel_dv);
}

xy Camera::project(vec3 loc)
{
    // apply camera translation
    loc -= m_lookfrom;
    // project to viewport
    const float mul = m_focal / loc.z();
    float x = loc.x() * mul;
    float y = loc.y() * mul;
    // convert from world-space to screen-space
    float nx = x * mp_screen->width  / m_viewport_width;
    float ny = y * mp_screen->height / m_viewport_height;
    // shift 0,0 from middle to top-left
    nx += mp_screen->width  / 2;
    ny += mp_screen->height / 2;
    // round and return
    return { (int) (nx+0.5), (int) (ny+0.5) };
}


void Camera::point(vec3 pt, uint32_t c)
{
    xy pos = project(pt);
    mp_screen->pixel(pos.x, pos.y, c);
}

void Camera::line(vec3 pt0, vec3 pt1, uint32_t c)
{
    xy pos0 = project(pt0);
    xy pos1 = project(pt1);
    mp_screen->line(pos0.x, pos0.y, pos1.x, pos1.y, c);
}

void Camera::triangleWireframe(vec3 pt0, vec3 pt1, vec3 pt2, uint32_t c)
{
    xy pos0 = project(pt0);
    xy pos1 = project(pt1);
    xy pos2 = project(pt2);
    mp_screen->triangleWireframe(pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, c);
}

void Camera::triangleFilled(vec3 pt0, vec3 pt1, vec3 pt2, uint32_t c)
{
    xy pos0 = project(pt0);
    xy pos1 = project(pt1);
    xy pos2 = project(pt2);
    mp_screen->triangleFilled(pos0.x, pos0.y, pos1.x, pos1.y, pos2.x, pos2.y, c);
}


void Camera::render(Renderable* pObj)
{
    std::vector<xy> projected;
    for (vec3 v : pObj->verts())
        projected.push_back( project(v) );

    if (pObj->isWireframe)
        for (tri t : pObj->tris())
        {
            xy p0 = projected.at(t.i0);
            xy p1 = projected.at(t.i1);
            xy p2 = projected.at(t.i2);
            mp_screen->triangleWireframe(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, t.c);
        }
    else
        for (tri t : pObj->tris())
        {
            xy p0 = projected.at(t.i0);
            xy p1 = projected.at(t.i1);
            xy p2 = projected.at(t.i2);
            mp_screen->triangleFilled(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, t.c);
        }
}

void Camera::translate(vec3 delta)
{
    m_lookfrom += delta;
}

#endif
