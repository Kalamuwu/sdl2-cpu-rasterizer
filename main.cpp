#include <iostream>
#include <SDL2/SDL.h>

#include "macros.h"
#include "screen.h"
#include "vector.h"
#include "camera.h"
#include "renderable.h"

int main(int argc, char **argv)
{

    // init camera
    Camera cam(       // Camera is...
        vec3(0,0,0),  // ... at 0,0,0,
        vec3(0,0,1),  // ... looking at +z,
        vec3(0,1,0),  // ... with +y as "up",
        70,           // ... and an vfov of 70

        WINDOW_WIDTH, WINDOW_HEIGHT
    );

    // init objects
    Renderable cube = Renderable(
        std::vector<vec3> {
            vec3(-2, -0.5, 5),
            vec3(-2,  0.5, 5),
            vec3(-1,  0.5, 5),
            vec3(-1, -0.5, 5),
            vec3(-2, -0.5, 6),
            vec3(-2,  0.5, 6),
            vec3(-1,  0.5, 6),
            vec3(-1, -0.5, 6)
        }, std::vector<tri> {
            { 1, 0, 2, 0xFFFFFFFF },
            { 3, 0, 2, 0xFFFFFFFF },

            { 5, 4, 6, 0xFFFFFFFF },
            { 7, 4, 6, 0xFFFFFFFF },

            { 1, 0, 5, 0xFF0000FF },
            { 4, 0, 5, 0xFF0000FF },

            { 2, 3, 6, 0xFFC000FF },
            { 7, 3, 6, 0xFFC000FF },

            { 4, 0, 7, 0x00FF00FF },
            { 3, 0, 7, 0x00FF00FF },

            { 5, 1, 6, 0x0000FFFF },
            { 2, 1, 6, 0x0000FFFF }
        });

    // just this cube, for now
    Renderable* world[] = { &cube };

    // wait for exit signal
    SDL_Event e;
    while (true)
    {
        // do render
        cam.screen()->clear();
        for (Renderable* pObj : world) cam.render(pObj);
        cam.screen()->show();

        // poll quit events
        while (SDL_PollEvent((&e)))
            if (e.type == SDL_QUIT) goto quit;

        // poll keyboard state
        const uint8_t* kb = SDL_GetKeyboardState(NULL);
        vec3 move(
            kb[SDL_SCANCODE_D] - kb[SDL_SCANCODE_A],  // D and A are left and right
            kb[SDL_SCANCODE_Q] - kb[SDL_SCANCODE_E],  // Q and E are up and down
            kb[SDL_SCANCODE_W] - kb[SDL_SCANCODE_S]   // W and S are forward and backward
        );

        // left shift will make the cube wireframe
        cube.isWireframe = (bool)kb[SDL_SCANCODE_LSHIFT];

        // do keyboard actions
        cam.translate(move * 0.05);

        // cap framerate
        SDL_Delay(10);
    }

quit:
    cam.screen()->quit();
    SDL_Quit();
    exit(0);
}
