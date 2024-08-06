#include "flowFree.hpp"
#include "paint.hpp"
#include "balloonGame.hpp"
#include "journaling.hpp"
#include "questionnare.hpp"
#include "piano.hpp"
struct MenuOption{
    SDL_Rect rect;
    SDL_Texture *texture;
    MenuOption(SDL_Rect rect, SDL_Texture *texture) : rect(rect), texture(texture) {}
};
//the main menu class that displays all the buttons that lead us to different parts of the project
class MainMenu{
    public:
    MainMenu() : _window(NULL), _renderer(NULL), Screen1Texture(NULL), Screen2Texture(NULL){
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
        _window = SDL_CreateWindow("Stress Releiver", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 900, 700, SDL_WINDOW_SHOWN);
        _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
        currentSubMenu = quit = count = false;
        IMG_Init(IMG_INIT_PNG); 
        IMG_Init(IMG_INIT_JPG);
        TTF_Init();
        Mix_Init(MIX_INIT_MP3);
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    }
    ~MainMenu(){
        Mix_FreeMusic(backgroundMusic);
        Mix_CloseAudio();
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
    }
    void run() {
        bool running = init();
        if (!running) {
            cout << "Failed to initialize the game." << endl;
            return;
        }
        SDL_Event event;
        while (!quit && event.type != SDL_QUIT && event.key.keysym.sym != SDLK_ESCAPE) {
            SDL_RenderClear(_renderer);
                if (currentSubMenu == 0) {
                    renderMainMenu();
                    handleMainMenuEvents(event);
                } else {
                    renderSubMenu();
                    handleSubMenuEvents(event);
                }
        }
        cleanup();
    }
    SDL_Texture *LoadTexture(const char *filename, SDL_Renderer *_renderer){
        SDL_Surface *surface = IMG_Load(filename);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
    }
    private:
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    Mix_Music *backgroundMusic;
    SDL_Texture *Screen1Texture, *Screen2Texture;
    SDL_Color fontColor = {255, 255, 255,255};
    SDL_Rect textRect;
    vector<MenuOption *> gameOptions;
    bool currentSubMenu, quit, count;
    //initializing the textures
    bool init(){
        Screen1Texture = LoadTexture("images/main_screen.png", _renderer);
        Screen2Texture = LoadTexture("images/menu_screen.png", _renderer);
        if (!Screen1Texture || !Screen2Texture){
            cout << "Failed to initialize." << endl;
            return false;
        }
        renderMainMenu();
        return true;
    }
    void renderSubMenu(){
        SDL_RenderCopy(_renderer, Screen2Texture, nullptr, nullptr);
        for (MenuOption *option : gameOptions){
                SDL_RenderCopy(_renderer, option->texture, nullptr, &option->rect);
            }
        SDL_RenderPresent(_renderer);
        if(count == false){
            Questionnaire question;
            question.run();
            count = true;
        }
    }
    void createSubMenuOptions(){
        const char *gameOptionPaths[] = {
            "images/calmCanvas.png",
            "images/penThoughts.png",
            "images/freeFlow.png",
            "images/popIt.png",
            "images/melodyCraft.png"};
            //sizing and setting dimensions of button options
        const int gameOptionWidth = 150;
        const int gameOptionHeight = 150;
        const int gameOptionPadding = 20;
        const int optionsPerRow = 3;
        const int optionsPerColumn = 2;
        int totalOptions = 5; //number of option on sub menu
        int optionsInFirstLine = totalOptions % optionsPerRow;
        int optionsInSecondLine = optionsPerRow - optionsInFirstLine;
        int totalWidthFirstLine = optionsInFirstLine * gameOptionWidth + (optionsInFirstLine - 1) * gameOptionPadding;
        int totalWidthSecondLine = optionsInSecondLine * gameOptionWidth + (optionsInSecondLine - 1) * gameOptionPadding;
        int startXFirstLine = (900 - totalWidthFirstLine) / 3;
        int startXSecondLine = (900 - totalWidthSecondLine) / 3;
        for (int i = 0; i < totalOptions; ++i){
            int optionRow = i / optionsPerRow;
            int optionCol = i % optionsPerRow;
            int optionX, optionY;
            if (optionRow == 0){
                optionX = startXFirstLine + optionCol * (gameOptionWidth + gameOptionPadding);
                optionY = 300;
            }
            else{
                optionX = startXSecondLine + optionCol * (gameOptionWidth + gameOptionPadding);
                optionY = 300 + gameOptionHeight + gameOptionPadding;
            }
            SDL_Rect rect{optionX, optionY, gameOptionWidth, gameOptionHeight};
            SDL_Texture *texture = LoadTexture(gameOptionPaths[i], _renderer);
            gameOptions.push_back(new MenuOption(rect, texture));
        }
    }
    // the main menu has 1 play button
    void handleMainMenuEvents(SDL_Event &event){
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                quit = true;
            }
            else if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE){
                    quit = true;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN){
                int x, y;
                SDL_GetMouseState(&x, &y);
                SDL_Point tempPoint = {x, y};
                SDL_Rect imageRect = {Width/2 - 45, 500, 150, 150};
                    if (SDL_PointInRect(&tempPoint, &imageRect)){
                        currentSubMenu = 1;
                        createSubMenuOptions();
                        break;
                    }
                }
        }
    }
    void drawImage(SDL_Texture* texture, int x, int y, int w, int h) {
        SDL_Rect imageRect = {x, y, w, h};  // Adjust the size and position as needed
        SDL_RenderCopy(_renderer, texture, nullptr, &imageRect);
        SDL_RenderPresent(_renderer);
    }
    //the submenu has 5 options to choose from
    void handleSubMenuEvents(SDL_Event &event){
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                quit = true;
            }
            else if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE){
                    quit = true;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN){
                int x, y;
                SDL_GetMouseState(&x, &y);
                for (int i = 0; i < gameOptions.size(); ++i){
                    SDL_Point tempPoint = {x, y};
                    if (SDL_PointInRect(&tempPoint, &gameOptions[i]->rect)){
                        if (i == 0){
                            PaintApp paint;
                            paint.run();
                        }
                        else if (i == 1){
                            Journaling journaling;
                            journaling.run();                   
                        }
                        else if (i == 2){
                            FlowFree flowfree;
                            flowfree.run();
                        }
                        else if (i == 3){
                            BalloonGame game;
                            game.run();
                        }
                        else if (i == 4){
                            Piano piano;
                            piano.run();
                        }
                        break;
                    }
                }
            }
        }
    }
    void renderMainMenu(){
        SDL_RenderCopy(_renderer, Screen1Texture, nullptr, nullptr);
        //rendering image for play button
        SDL_Texture *texture = LoadTexture("images/start.png", _renderer);
        drawImage(texture, Width/2 - 45, 500, 150, 150);
    }
    void cleanup() {
            SDL_DestroyTexture(Screen1Texture);
            SDL_DestroyTexture(Screen2Texture);
            for (MenuOption* option : gameOptions) {
                SDL_DestroyTexture(option->texture);
                delete option;
            }
            gameOptions.clear();
            SDL_DestroyRenderer(_renderer);
            SDL_DestroyWindow(_window);
    }
};