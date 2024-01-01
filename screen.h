#ifndef SCREENH
#define SCREENH

#include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>

class Screen
{
public:
    Screen(int w, int h, float zoom_scale);

    void inline pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void inline pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void        pixel(int x, int y, uint32_t c);

    void inline line(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void inline line(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b);
    void        line(int x0, int y0, int x1, int y1, uint32_t c);

    void inline triangleWireframe(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void inline triangleWireframe(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b);
    void        triangleWireframe(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t c);

    void inline triangleFilled(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void inline triangleFilled(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b);
    void        triangleFilled(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t c);

    void clear() { memset(pTextureBuffer, 0x00, width*height*sizeof(uint32_t)); };
    void setTitle(const char *title) { SDL_SetWindowTitle(mpWindow, title); };

    void quit();
    void show();
    // void save(const char *file_name);

    int width;
    int height;
    uint32_t* pTextureBuffer;

    bool isOnScreen(int x, int y) { return (x <= width && x >= 0 && y <= height && y >= 0); }

private:
    SDL_Window* mpWindow;
    SDL_Renderer* mpRenderer;
    SDL_Texture* mpTexture;
};

Screen::Screen(int w, int h, float zoom_scale)
{
    width = w; height = h;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(width*zoom_scale, h*zoom_scale, 0, &mpWindow, &mpRenderer);
    SDL_RenderSetScale(mpRenderer, zoom_scale, zoom_scale);
    mpTexture = SDL_CreateTexture(mpRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    pTextureBuffer = new uint32_t[ w * h ];
    clear();  // fill texturebuffer with 0's
}

#include <bits/endian.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t inline col(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { return (r<<24) | (g<<16) | (b<<8) | a; }
#elif __BYTE_ORDER == __BIG_ENDIAN
    uint32_t inline col(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { return (a<<24) | (b<<16) | (g<<8) | r; }
#else
# error "Please fix <bits/endian.h>"
#endif


void inline Screen::pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) { pixel(x, y, col(r,g,b,a   )); }
void inline Screen::pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)            { pixel(x, y, col(r,g,b,0xFF)); }
void        Screen::pixel(int x, int y, uint32_t c)
{
    //sif (x >= 0 && x <= width && y >= 0 && y <= height)
        pTextureBuffer[y*width + x] = c;
}


void inline Screen::line(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b, uint8_t a) { line(x0, y0, x1, y1, col(r,g,b,a   )); }
void inline Screen::line(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b)            { line(x0, y0, x1, y1, col(r,g,b,0xFF)); }
void        Screen::line(int x0, int y0, int x1, int y1, uint32_t c)
{
    if (x0 == x1 && y0 == y1)
    {
        // short-circuit -- no line to draw. this would have otherwise caused
        // a division-by-zero when trying to calculate a.
        pixel(x0, y0, c);
        return;
    }
    if (abs(x1-x0) > abs(y1-y0))
    {
        // line is horizontal-ish: there are more x's than y's, so some y's
        // have multiple x's; however, all x's have one y. therefore, loop
        // through all x's.
        if (x0 > x1)  // make sure x0 <= x1
        {
            int t=x0; x0=x1; x1=t;  // swap x
                t=y0; y0=y1; y1=t;  // swap y
        }
        float a = (float)(y1-y0) / (x1-x0);
        float y = y0;
        for (int x = x0; x <= x1; x++)
        {
            pixel(x, (int)y, c);
            y += a; // lerp y
        }
    }
    else
    {
        // line is vertical-ish: see above but there are more y's than x's.
        if (y0 > y1)  // make sure y0 <= y1
        {
            int t=x0; x0=x1; x1=t;  // swap x
                t=y0; y0=y1; y1=t;  // swap y
        }
        float a = (float)(x1-x0) / (y1-y0);
        float x = x0;
        for (int y = y0; y <= y1; y++)
        {
            pixel((int)x, y, c);
            x += a; // lerp x
        }
    }
}


void inline Screen::triangleWireframe(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) { triangleWireframe(x0, y0, x1, y1, x2, y2, col(r,g,b,a   )); }
void inline Screen::triangleWireframe(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b)            { triangleWireframe(x0, y0, x1, y1, x2, y2, col(r,g,b,0xFF)); }
void        Screen::triangleWireframe(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t c)
{
    // simple -- just draw three lines.
    line(x0, y0, x1, y1, c);
    line(x1, y1, x2, y2, c);
    line(x2, y2, x0, y0, c);
}


void inline Screen::triangleFilled(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) { triangleFilled(x0, y0, x1, y1, x2, y2, col(r,g,b,a   )); }
void inline Screen::triangleFilled(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b)            { triangleFilled(x0, y0, x1, y1, x2, y2, col(r,g,b,0xFF)); }
void        Screen::triangleFilled(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t c)
{
    // Sort points such that pt0 is the bottommost and p2 is the topmost
    int t;
    if (y1 < y0)
    {
        t=x0; x0=x1; x1=t;  // swap x
        t=y0; y0=y1; y1=t;  // swap y
    }
    if (y2 < y0)
    {
        t=x0; x0=x2; x2=t;  // swap x
        t=y0; y0=y2; y2=t;  // swap y
    }
    if (y2 < y1)
    {
        t=x1; x1=x2; x2=t;  // swap x
        t=y1; y1=y2; y2=t;  // swap y
    }

    // split into two triangles; the new point that splits the triangle into
    // two lies on the long edge, from p0 to p2, and therefore doesnt have to
    // be explicitly defined, but will carry over from the first triangle.

    float a02 = (float)(x2-x0) / (y2-y0);  // slope of long edge
    float x02 = x0;
    float at = (float)(x1-x0) / (y1-y0);  // slope of first short edge, p0-->p1
    float xt = x0;
    // lerp top half of triangle
    for (int y = y0; y <= y1; y++)
    {
        int edgeL = (int)(x02>xt? xt : x02);  // find left edge
        int edgeR = (int)(x02>xt? x02 : xt);  // find right edge
        for (int x = edgeL; x <= edgeR; x++) pixel(x, y, c);  // fill horizontal
        x02 += a02;  // lerp long edge x
        xt += at;    // lerp short edge x
    }
    // lerp bottom half of triangle
    at = (float)(x2-x1) / (y2-y1);
    xt = x1;
    for (int y = y1; y <= y2; y++)
    {
        int edgeL = (int)(x02>xt? xt : x02);  // find left edge
        int edgeR = (int)(x02>xt? x02 : xt);  // find right edge
        for (int x = edgeL; x <= edgeR; x++) pixel(x, y, c);  // fill horizontal
        x02 += a02;  // lerp long edge x
        xt += at;    // lerp short edge x
    }

}

void Screen::show()
{
    SDL_UpdateTexture(mpTexture, NULL, pTextureBuffer, width * sizeof(uint32_t));
    SDL_RenderCopy(mpRenderer, mpTexture, NULL, NULL);
    SDL_RenderPresent(mpRenderer);
}

/*
void Screen::save(const char *pFileName)
{
    SDL_Texture* target = SDL_GetRenderTarget(mpRenderer);
    SDL_SetRenderTarget(mpRenderer, mpTexture);
    int width, height;
    SDL_QueryTexture(mpTexture, NULL, NULL, &width, &height);
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(mpRenderer, NULL, surface->format->format, surface->pixels, surface->pitch);
    IMG_SavePNG(surface, pFileName);
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(mpRenderer, target);
}
*/

void Screen::quit()
{
    SDL_DestroyRenderer(mpRenderer);
    SDL_DestroyWindow(mpWindow);
    free(pTextureBuffer);
}


#endif
