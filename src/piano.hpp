#include "baseClass.hpp"

const int TOTAL_IMAGES = 42;

// Piano class inherits from StressReliever
class Piano : public StressReliever {
public:
    // Constructor
    Piano(): StressReliever("Piano", 800, 700) {
        initialize();
        loadButtons();
    }

    // Destructor
    ~Piano() {
        Mix_CloseAudio();
    }

    // Main loop for running the piano
    void run() {
        while (!quit && event.type != SDL_QUIT && event.key.keysym.sym != SDLK_ESCAPE) {
            handleEvents();
            renderScreen();
        }
    }

private:
    int currentImageIndex = 0;
    CircularLinkedList<SDL_Texture*> imageList;
    bool quit = false;
    int currentButtonIndex = -1;
    LinkedList<int> initialXPositions;
    LinkedList<int> initialYPositions;

    // Initialize piano settings
    void initialize() {
        loadImages();
        setPosition();
    }

    // Handle SDL events
    void handleEvents() {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_KEYDOWN) {
                handleKeyPress(event.key.keysym.sym);
            }
            if (event.type == SDL_KEYUP) {
                setPosition();
                loadButtons();
            }
        }
    }

    // Handle key press events
    void handleKeyPress(SDL_Keycode keyCode) {
        switch (keyCode) {
            // Handle each key press event
            case SDLK_0: playNoteAndDelay(0); moveButton(0); break;
            case SDLK_1: playNoteAndDelay(1); moveButton(1); break;
            case SDLK_2: playNoteAndDelay(2); moveButton(2); break;
            case SDLK_3: playNoteAndDelay(3); moveButton(3); break;
            case SDLK_4: playNoteAndDelay(4); moveButton(4); break;
            case SDLK_5: playNoteAndDelay(5); moveButton(5); break;
            case SDLK_6: playNoteAndDelay(6); moveButton(6); break;
            case SDLK_7: playNoteAndDelay(7); moveButton(7); break;
            case SDLK_8: playNoteAndDelay(8); moveButton(8); break;
            case SDLK_9: playNoteAndDelay(9); moveButton(9); break;
            case SDLK_ESCAPE: quit = true; break;
            default: break;
        }
    }

    // Move the button based on button index
    void moveButton(int buttonIndex) {
        if (buttonIndex >= 0 && buttonIndex < 10) {
            initialXPositions.moveButton(buttonIndex);
            initialYPositions.moveButton(buttonIndex);
            loadButtons();
        }
    }

    // Play a note and introduce a delay
    void playNoteAndDelay(int note) {
        // Load and play the corresponding music file
        Mix_Music* music = Mix_LoadMUS(("sound/media_" + to_string(note) + ".mp3").c_str());
        if (!music) {
            cerr << "SDL_mixer Load Music Error: " << Mix_GetError() << endl;
            exit(1);
        }

        if (Mix_PlayMusic(music, 1) == -1) {
            cerr << "SDL_mixer Play Music Error: " << Mix_GetError() << endl;
            exit(1);
        }
    }

    // Load images for piano keys
    void loadImages() {
        for (int i = 1; i <= TOTAL_IMAGES; ++i) {
            string filename = "images/Video/" + to_string(i / 10000) + to_string((i / 1000) % 100) + to_string((i / 100) % 10) + to_string((i / 10) % 10) + to_string(i % 10) + ".bmp";
            SDL_Surface* loadedSurface = IMG_Load(filename.c_str());
            if (loadedSurface == NULL) {
                cerr << "Unable to load image " << filename << "! SDL_image Error: " << IMG_GetError() << endl;
                exit(EXIT_FAILURE);
            }

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
            if (texture == NULL) {
                cerr << "Unable to create texture from " << filename << "! SDL Error: " << SDL_GetError() << endl;
                exit(EXIT_FAILURE);
            }

            SDL_FreeSurface(loadedSurface);
            imageList.addNode(texture);
        }
    }

    // Render the piano screen
    void renderScreen() {
        SDL_Texture* currentTexture = imageList.getCurrentTexture();
        SDL_RenderCopy(renderer, currentTexture, NULL, NULL);
        SDL_Delay(50);
        imageList.nextNode();
        loadButtons();
        SDL_RenderPresent(renderer);
    }

    // Set initial positions for piano keys
    void setPosition() {
        int i;
        for (i = 10; i >= 0; i--) {
            initialXPositions.insert(100 + (i * 60)); 
            initialYPositions.insert(475);
        }
    }

    // Load piano keys/buttons
    void loadButtons() {
        int i = 0;
        Node<int>* xCurrent = initialXPositions.head;
        Node<int>* yCurrent = initialYPositions.head;

        for (i = 0; i < 10; i++) {
            if (i % 2 == 0) {
                addButton("images/key1.png", xCurrent->data, yCurrent->data);
            } else {
                addButton("images/key2.png", xCurrent->data, yCurrent->data);
            }
            xCurrent = xCurrent->next;
            yCurrent = yCurrent->next;
        }
    }

    // Add a button to the screen
    void addButton(string imagePath, int x, int y) {
        SDL_Surface* surface = IMG_Load(imagePath.c_str());
        if (!surface) {
            cerr << "Failed to load image " << imagePath << ": " << IMG_GetError() << endl;
            exit(EXIT_FAILURE);
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) {
            cerr << "Failed to create texture from surface: " << SDL_GetError() << endl;
            exit(EXIT_FAILURE);
        }
        SDL_Rect rect = {x, y, 60, 200}; // Adjust the size of the button as needed
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }
};
