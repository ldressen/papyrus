#include <iostream>
#include <vector>
#include <string>

#include <SDL3/SDL.h>

#include "Renderer.h"
#include "Editor.h"
#include "util.h"

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("papyrus", 800, 600, SDL_WINDOW_RESIZABLE);
    CSF(SDL_StartTextInput(window));

    Renderer sr{window};

    Editor editor{};

    bool running = true;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_TEXT_INPUT)
            {
                editor.handleTextInput(event.text.text);
            }
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                SDL_Keycode key = event.key.key;
                SDL_Keymod mod = event.key.mod;
                switch (key)
                {
                case SDLK_BACKSPACE:
                    editor.handleBackSpace();
                    break;
                case SDLK_RETURN:
                    editor.handleReturn();
                    break;
                case SDLK_LEFT:
                    editor.handleLeft(mod);
                    break;
                case SDLK_RIGHT:
                    editor.handleRight(mod);
                    break;
                case SDLK_UP:
                    editor.handleUp();
                    break;
                case SDLK_DOWN:
                    editor.handleDown();
                    break;
                case SDLK_TAB:
                    editor.handleTab();
                    break;
                case SDLK_LSHIFT:
                    editor.handleShift(event.type);
                    break;

                // IO
                case SDLK_F1:
                    editor.loadFile("./test.txt");
                    break;
                case SDLK_F2:
                    editor.saveFileAs("./test2.txt");
                    break;

                default:
                    break;
                }
            }
            else if (event.type == SDL_EVENT_KEY_UP)
            {
                SDL_Keycode key = event.key.key;
                SDL_Keymod mod = event.key.mod;

                switch (key)
                {
                case SDLK_LSHIFT:
                    editor.handleShift(event.type);
                    break;
                }
            }
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
        sr.update(editor);
    }
    CSF(SDL_StopTextInput(window));
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}