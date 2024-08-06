#include "baseClass.hpp"
class BalloonGame : public StressReliever{
public:
    BalloonGame() : StressReliever("Balloon Game", 800, 700),
        balloons(NULL),
        poppedBalloons(0),
        quit(false) {
        initSDL();
    }

    ~BalloonGame(){
        cleanup();
    }

    void run() {
    // Main game loop: continue as long as the user has not quit,
    // there is no SDL_QUIT event, and the key pressed is not ESCAPE.
    while (!quit && event.type != SDL_QUIT && event.key.keysym.sym != SDLK_ESCAPE) {
        // Handle user input events
        handleEvents();

        // Randomly spawn balloons with a 2% probability
        if (rand() % 100 < 2) {
            spawnBalloon();
        }

        // Update the position of non-popped balloons moving them upwards
        Balloon* currentBalloon = balloons;
        while (currentBalloon != NULL) {
            if (!currentBalloon->popped) {
                currentBalloon->y -= 1;
            }
            currentBalloon = currentBalloon->next;
        }

        // Remove popped balloons from the linked list and free memory
        Balloon* prevBalloon = NULL;
        currentBalloon = balloons;
        while (currentBalloon != NULL) {
            if (currentBalloon->popped) {
                if (prevBalloon != NULL) {
                    prevBalloon->next = currentBalloon->next;
                    Balloon* nextBalloon = currentBalloon->next;
                    delete currentBalloon;
                    currentBalloon = nextBalloon;
                } else {
                    Balloon* nextBalloon = currentBalloon->next;
                    delete currentBalloon;
                    balloons = nextBalloon;
                    currentBalloon = balloons;
                }
            } else {
                prevBalloon = currentBalloon;
                currentBalloon = currentBalloon->next;
            }
        }

        // Set the background color and clear the renderer
        SDL_SetRenderDrawColor(renderer, 255, 255, 204, 255);
        SDL_RenderClear(renderer);

        // Render the background texture
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

        // Render non-popped balloons on the screen
        currentBalloon = balloons;
        while (currentBalloon != NULL) {
            if (!currentBalloon->popped) {
                // Create an SDL_Rect representing the balloon's position and dimensions
                SDL_Rect balloonRect = {
                    currentBalloon->x - currentBalloon->width / 2,
                    currentBalloon->y - currentBalloon->height / 2,
                    currentBalloon->width,
                    currentBalloon->height
                };

                // Render the balloon's texture at the specified position and dimensions
                SDL_RenderCopy(renderer, currentBalloon->texture, NULL, &balloonRect);
            }
            // Move to the next balloon in the linked list
            currentBalloon = currentBalloon->next;
        }

        // Present the renderer to display the rendered scene
        SDL_RenderPresent(renderer);

        // Introduce a short delay to control the frame rate (10 milliseconds in this case)
        SDL_Delay(10);
    }
}


private:
    struct Balloon {
        int x, y;
        int radius;
        int height;
        int width;
        SDL_Texture* texture;
        bool popped;
        Balloon* next;
    };
    Balloon* balloons;
    int poppedBalloons;
    bool quit;

    void initSDL(){
        success = Mix_LoadWAV("sound/balloonpop.wav");
        font = TTF_OpenFont("fonts/ariali.ttf", 24);
        backgroundTexture = loadTexture("images/BalloonBg.jpg");
        if (!success || !font || !backgroundTexture) {
            cerr << "Failed to load fond, background Image or sound"<< endl;
        }
        srand(static_cast<unsigned>(time(NULL)));
    }
   void handleEvents() {
    // Process all pending events in the event queue
    while (SDL_PollEvent(&event)) {
        // Check if a key is pressed down
        if (event.type == SDL_KEYDOWN) {
            // Check if the pressed key is ESCAPE, and set quit flag if true
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                quit = true;
            }
        }
        // Check if the window's close button is clicked
        else if (event.type == SDL_QUIT) {
            quit = true;
        }
        // Check if a mouse button is pressed down
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            // Get the current mouse position
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // Iterate through the linked list of balloons to check for clicks
            Balloon* currentBalloon = balloons;
            while (currentBalloon != NULL) {
                // Check if the balloon is not popped
                if (!currentBalloon->popped) {
                    // Calculate the squared distance between mouse and balloon center
                    int dx = mouseX - currentBalloon->x;
                    int dy = mouseY - currentBalloon->y;
                    int distanceSquared = dx * dx + dy * dy;

                    // Check if the mouse click is within the balloon's radius
                    if (distanceSquared <= currentBalloon->radius * currentBalloon->radius) {
                        // Mark the balloon as popped
                        currentBalloon->popped = true;
                        poppedBalloons++;

                        // Play a success sound effect
                        Mix_PlayChannel(-1, success, 0);

                        // Check if the player has popped balloons in multiples of 5
                        if (poppedBalloons % 5 == 0) {
                            // Determine an affirmation message based on the count
                            int affirmationIndex = poppedBalloons / 5;
                            const char* affirmationText = NULL;

                            // Assign an affirmation message based on the index
                            switch (affirmationIndex) {
                                case 1:
                                    affirmationText = "Good Job!";
                                    break;
                                case 2:
                                    affirmationText = "Wow!";
                                    break;
                                case 3:
                                    affirmationText = "Good One!";
                                    break;
                                default:
                                    affirmationText = "Keep Going!";
                                    break;
                            }

                            // Display an affirmation message near the popped balloon
                            displayAffirmation(currentBalloon->x, currentBalloon->y, affirmationText);
                        }
                    }
                }
                // Move to the next balloon in the linked list
                currentBalloon = currentBalloon->next;
            }
        }
    }
}

    void initialize() {
    // Randomly spawn a balloon with a 2% probability
    if (rand() % 100 < 2) {
        spawnBalloon();
    }

    // Update the position of non-popped balloons, moving them upwards
    Balloon* currentBalloon = balloons;
    while (currentBalloon != NULL) {
        if (!currentBalloon->popped) {
            currentBalloon->y -= 1;
        }
        currentBalloon = currentBalloon->next;
    }

    // Remove popped balloons from the linked list and free memory
    Balloon* prevBalloon = NULL;
    currentBalloon = balloons;
    while (currentBalloon != NULL) {
        if (currentBalloon->popped) {
            if (prevBalloon != NULL) {
                prevBalloon->next = currentBalloon->next;
                Balloon* nextBalloon = currentBalloon->next;
                delete currentBalloon;
                currentBalloon = nextBalloon;
            } else {
                Balloon* nextBalloon = currentBalloon->next;
                delete currentBalloon;
                balloons = nextBalloon;
                currentBalloon = balloons;
            }
        } else {
            prevBalloon = currentBalloon;
            currentBalloon = currentBalloon->next;
        }
    }

    // Set the background color and clear the renderer
    SDL_SetRenderDrawColor(renderer, 255, 255, 204, 255);
    SDL_RenderClear(renderer);

    // Render the background texture
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    // Render non-popped balloons on the screen
    currentBalloon = balloons;
    while (currentBalloon != NULL) {
        if (!currentBalloon->popped) {
            // Create an SDL_Rect representing the balloon's position and dimensions
            SDL_Rect balloonRect = {
                currentBalloon->x - currentBalloon->width / 2,
                currentBalloon->y - currentBalloon->height / 2,
                currentBalloon->width,
                currentBalloon->height
            };

            // Render the balloon's texture at the specified position and dimensions
            SDL_RenderCopy(renderer, currentBalloon->texture, NULL, &balloonRect);
        }
        // Move to the next balloon in the linked list
        currentBalloon = currentBalloon->next;
    }
}

    void cleanup(){
        while (balloons != NULL) {
            Balloon* nextBalloon = balloons->next;
            delete balloons;
            balloons = nextBalloon;
        }
    }

    SDL_Texture* loadTexture(const char* path){
        SDL_Texture* texture = NULL;
        SDL_Surface* surface = IMG_Load(path);
        if (surface == NULL) {
            cerr << "IMG_Load Error: " << IMG_GetError() << endl;
            return NULL;
        }

        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture == NULL) {
            cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << endl;
        }

        SDL_FreeSurface(surface);
        return texture;
    }

    void spawnBalloon(){
        Balloon* balloon = new Balloon;
        balloon->x = rand() % Width;
        balloon->y = Height;
        balloon->height = 150;
        balloon->width = 60;
        balloon->radius = 50;
        balloon->texture = loadTexture("images/balloon.png");
        balloon->popped = false;
        balloon->next = balloons;
        balloons = balloon;
    }

    void displayAffirmation(int x, int y, const char* text) {
    // Render the given text to create an SDL_Texture
    SDL_Texture* affirmationTexture = renderText(text);

    // Check if the rendering was successful
    if (affirmationTexture != NULL) {
        // Define the SDL_Rect representing the position and dimensions of the rendered text
        SDL_Rect textRect = {x, y, 200, 50};

        // Copy the rendered text texture onto the renderer at the specified position and dimensions
        SDL_RenderCopy(renderer, affirmationTexture, NULL, &textRect);

        // Present the renderer to display the rendered text
        SDL_RenderPresent(renderer);

        // Delay for 1000 milliseconds (1 second) to display the affirmation
        SDL_Delay(1000);

        // Destroy the rendered text texture to free up memory
        SDL_DestroyTexture(affirmationTexture);
    }
}


    SDL_Texture* renderText(const char* text){
        SDL_Color textColor = {255, 0, 0, 255};
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, textColor);
        if (textSurface == NULL) {
            cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << endl;
            return NULL;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (texture == NULL) {
            cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << endl;
        }

        SDL_FreeSurface(textSurface);
        return texture;
    }
};










