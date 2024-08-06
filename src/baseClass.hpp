#ifndef ARCADE_H
#define ARCADE_H
// Above are preprocessor directives that guard against multiple inclusion of the same header file.
#include<SDL.h>
#include<SDL_ttf.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<iostream>
#include<string>
#include <vector>
#include "DSA.hpp"
using namespace std;
class StressReliever{
    protected:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    Mix_Music *backgroundMusic;
    Mix_Chunk *success;
    SDL_Texture* backgroundTexture;
    TTF_Font* font, *dataFont;
    int Width, Height;
    const char *gameName;
    public:
    StressReliever(const char *n = "", int w = 700, int h = 700) : gameName(n), Width(w), Height(h), window(NULL), renderer(NULL), backgroundTexture(NULL), backgroundMusic(NULL), success(NULL), font(NULL), dataFont(NULL) {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
        window = SDL_CreateWindow(gameName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        IMG_Init(IMG_INIT_PNG); 
        IMG_Init(IMG_INIT_JPG);
        TTF_Init(); 
        Mix_Init(MIX_INIT_MP3);
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    }
    ~StressReliever(){
        TTF_CloseFont(font);
        TTF_CloseFont(dataFont);
        TTF_Quit();
        Mix_FreeMusic(backgroundMusic);
        Mix_FreeChunk(success);
        Mix_CloseAudio();
        Mix_Quit();
        SDL_DestroyTexture(backgroundTexture);
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }
    virtual void run() = 0;
    virtual void initialize() = 0;
    virtual void handleEvents() = 0;
};
#endif // concluding preprocessor directive