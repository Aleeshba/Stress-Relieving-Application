#include "baseClass.hpp"
class TreeNode
{
public:
    string question;
    TreeNode *trueOption;
    TreeNode *falseOption;

    TreeNode(string q, TreeNode *trueOpt, TreeNode *falseOpt): question(q), trueOption(trueOpt), falseOption(falseOpt) {}
};
class Questionnaire : public StressReliever
{


public:
    Questionnaire() : StressReliever("Questionnaire", 800, 300), buttonHeight(50), buttonWidth(100), quit(false),
                      trueButtonTexture(NULL), falseButtonTexture(NULL), root(NULL), currentNode(NULL)
    {
        initialize();

        root = new TreeNode{"Did your day go well?",
                    new TreeNode{"Did you find moments of joy?",
                            new TreeNode{"Did you engage in activities you enjoy?",
                                    new TreeNode{"Great! Did you connect with others?",
                                            new TreeNode{"Sounds like a good day! Try journaling to express your emotions", NULL, NULL},
                                            new TreeNode{"Its okay! A good connection with yourself is more important", NULL, NULL}},
                                    new TreeNode{"Did you practice self-care?",
                                            new TreeNode{"Great! Remember you are important.", NULL, NULL},
                                            new TreeNode{"Its okay to put yourself first. Try playing a relaxing game", NULL, NULL}}},
                            new TreeNode{"I hope you do find them. Did you face challenges?",
                                    new TreeNode{"Did you seek support from others?",
                                            new TreeNode{"Thats great! Never feel ashamed to ask for help", NULL, NULL},
                                            new TreeNode{"Oh! you should talk to a friend or family member", NULL, NULL}},
                                    new TreeNode{"Its okay! Did you practice coping strategies?",
                                            new TreeNode{"I'm glad you are able to cope with it", NULL, NULL},
                                            new TreeNode{"A stress relieving game is a great coping mechanism", NULL, NULL}}}},
                    new TreeNode{"Oh I'm sorry to hear that. Did you take care of yourself?",
                            new TreeNode{"Good! Did you prioritize your physical health?",
                                    new TreeNode{"EXCELLENT! Care for your body, it's your only home.", NULL, NULL},
                                            new TreeNode{"Prioritize your well-being, your body will thank you.", NULL, NULL}},
                            new TreeNode{"Oh why not? Did you prioritize your mental well-being?",
                                    new TreeNode{"I'm glad. You can play piano and sooth your mind", NULL, NULL},
                                            new TreeNode{"Take a break to relax and paint your heart", NULL, NULL}}}};

        currentNode = root;
    }

    ~Questionnaire()
    {
        cleanup();
    }
    void run()
    {
        while (!quit && currentNode && event.type != SDL_QUIT && event.key.keysym.sym != SDLK_ESCAPE)
        {
            askQuestion(currentNode);
            handleEvents();
        }
    }

private:
    SDL_Texture *trueButtonTexture;
    SDL_Texture *falseButtonTexture;

    TreeNode *root;
    TreeNode *currentNode;
    bool quit;
    int buttonWidth;
    int buttonHeight;
    void cleanup()
    {
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(trueButtonTexture);
        SDL_DestroyTexture(falseButtonTexture);
        delete root;
    }

    void initialize()
    {
        font = TTF_OpenFont("fonts/arial.ttf", 26); // font size
        backgroundTexture = loadTexture(renderer, "images/images (5).png");
        if (!font || !backgroundTexture)
        {
            cerr << "Failed to load image or font: " << TTF_GetError() << endl;
            cleanup();
        }

        trueButtonTexture = loadTexture(renderer, "images/yes.png");
        falseButtonTexture = loadTexture(renderer, "images/no.png");
    }

    SDL_Texture *loadTexture(SDL_Renderer *renderer, string filePath)
    {
        SDL_Surface *surface = IMG_Load(filePath.c_str());
        if (!surface)
        {
            cerr << "Error loading texture: " << IMG_GetError() << endl;
            return NULL;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!texture)
        {
            cerr << "Error creating texture: " << SDL_GetError() << endl;
            return NULL;
        }

        return texture;
    }

  void askQuestion(TreeNode* node)
{
    // Render the background
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    // Render the question text
    SDL_Color textColor = {0, 0, 82, 180}; // Purple text color
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, node->question.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Get the dimensions of the rendered text
    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

    // Define the rectangle for rendering the text
    SDL_Rect textRect = {160, 50, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Clean up the texture and surface resources
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);

    // Render buttons only if it's not a leaf node
    if (node->trueOption || node->falseOption)
    {
        // Render the "True" button
        SDL_Rect trueButtonRect = {200, 150, buttonWidth, buttonHeight};
        SDL_RenderCopy(renderer, trueButtonTexture, NULL, &trueButtonRect);

        // Render the "False" button
        SDL_Rect falseButtonRect = {350, 150, buttonWidth, buttonHeight};
        SDL_RenderCopy(renderer, falseButtonTexture, NULL, &falseButtonRect);
    }

    // Present the renderer's contents
    SDL_RenderPresent(renderer);
}


    void handleEvents()
{
    // Process all pending SDL events
    while (SDL_PollEvent(&event))
    {
        // Check if a key was pressed
        if (event.type == SDL_KEYDOWN)
        {
            // Check if the pressed key is the ESC key
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                // Set the quit flag to true to exit the application
                quit = true;
            }
        }
        // Check if the user closed the application window
        if (event.type == SDL_QUIT)
        {
            // Set the quit flag to true to exit the application
            quit = true;
        }
        // Check if the user pressed a mouse button
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            // Get the current mouse coordinates
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            // Check if the mouse click is within the boundaries of the true option button
            if (mouseX >= 200 && mouseX <= 200 + buttonWidth && mouseY >= 150 && mouseY <= 150 + buttonHeight)
            {
                // Move to the next node based on the true option
                currentNode = currentNode->trueOption;
            }
            // Check if the mouse click is within the boundaries of the false option button
            else if (mouseX >= 350 && mouseX <= 350 + buttonWidth && mouseY >= 150 && mouseY <= 150 + buttonHeight)
            {
                // Move to the next node based on the false option
                currentNode = currentNode->falseOption;
            }

            // Check if the current node is a leaf node (no true or false options)
            if (!currentNode->trueOption && !currentNode->falseOption)
            {
                // Display the message associated with the leaf node
                askQuestion(currentNode);

                // Present the renderer's contents
                SDL_RenderPresent(renderer);

                // Pause for a short duration (2 seconds in this case)
                SDL_Delay(2000);

                // Set the quit flag to true to exit the application
                quit = true;
            }
        }
    }
}

    
};

