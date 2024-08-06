#include <sstream>
#include "baseClass.hpp"

const int maxLineWidth = 45;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 700;
class LTexture
{
public:
    LTexture()
    {
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
    ~LTexture()
    {
        free();
    }
    bool loadFromRenderedText(SDL_Renderer *_renderer, TTF_Font *Font, string textureText, SDL_Color textColor)
    {
        free();
        SDL_Surface *textSurface = TTF_RenderText_Solid(Font, textureText.c_str(), textColor);
        if (textSurface != NULL)
        {
            mTexture = SDL_CreateTextureFromSurface(_renderer, textSurface);
            if (mTexture == NULL)
            {
                cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << endl;
            }
            else
            {
                mWidth = textSurface->w;
                mHeight = textSurface->h;
            }

            SDL_FreeSurface(textSurface);
        }
        else
        {
            cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << endl;
        }

        return mTexture != NULL;
    }

    void free()
    {
        if (mTexture != NULL)
        {
            SDL_DestroyTexture(mTexture);
            mTexture = NULL;
            mWidth = 0;
            mHeight = 0;
        }
    }
    void render(SDL_Renderer *_renderer, int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE)
    {
        SDL_Rect renderQuad = {x, y, mWidth, mHeight};
        if (clip != NULL)
        {
            renderQuad.w = clip->w;
            renderQuad.h = clip->h;
        }

        SDL_RenderCopyEx(_renderer, mTexture, clip, &renderQuad, angle, center, flip);
    }
    int getWidth()
    {
        return mWidth;
    }
    int getHeight()
    {
        return mHeight;
    }

private:
    SDL_Texture *mTexture;
    int mWidth;
    int mHeight;
};
//child class of main stress reliever class
class Journaling : public StressReliever
{
private:
    SDL_Texture *background1, *background2, *buttonNext, *buttonViewNotes, *buttonBack, *buttonDeleteNotes;
    SDL_Color textColor;
    LTexture gPromptTextTexture, gInputTextTexture;
    SDL_Rect buttonBackRect, buttonNextRect, buttonViewNotesRect, buttonDeleteRect;
    bool quit, viewNotesClicked, deleteNotesClicked, switchToNextScreen, renderText;
    string inputText, selectedEntryData;
    int selectedEntryNumber;
    DoublyLinkedList DoublyLinkedList;

public:
    Journaling() : StressReliever("Journaling", 800, 700), selectedEntryNumber(-1)
    {
        quit = viewNotesClicked = deleteNotesClicked = switchToNextScreen = renderText = false;
        inputText = selectedEntryData = "";
        initialize();
    }
    ~Journaling()
    {
        cleanup();
    }
    void run()
    {
        SDL_StartTextInput();
        while (!quit && event.type != SDL_QUIT && event.key.keysym.sym != SDLK_ESCAPE)
        {

            handleEvents();
            SDL_RenderClear(renderer);

            if (switchToNextScreen)
            {
                SwitchToNextScreen();
            }
            else
            {
                renderTexture(background1, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                renderTexture(buttonNext, renderer, buttonNextRect.x, buttonNextRect.y, buttonNextRect.w, buttonNextRect.h);
                renderTexture(buttonViewNotes, renderer, buttonViewNotesRect.x, buttonViewNotesRect.y, buttonViewNotesRect.w, buttonViewNotesRect.h);
                renderTexture(buttonDeleteNotes, renderer, buttonDeleteRect.x, buttonDeleteRect.y, buttonDeleteRect.w, buttonDeleteRect.h);
                RenderText(selectedEntryData, 450, 140, true); 
                if (viewNotesClicked)
                {
                    ViewNotesClicked();
                }
                else if (deleteNotesClicked)
                {
                    DeleteNotesClicked();
                }
                else
                {
                    // Display notes on the side
                    renderNotesList(renderer, textColor, true);
                }
            }

            SDL_RenderPresent(renderer);
        }

        SDL_StopTextInput();
    }
private:
        //function to initialise all textures, fonts, texts
    void initialize()
    {
        background1 = loadTexture("images/journaling.png", renderer);
        background2 = loadTexture("images/journaling2.png", renderer);
        buttonBack = loadTexture("images/button_back.png", renderer);
        buttonNext = loadTexture("images/button_next.png", renderer);
        buttonViewNotes = loadTexture("images/button_view.png", renderer);
        buttonDeleteNotes = loadTexture("images/button_delete.png", renderer);
        textColor = {100, 10, 0, 0xFF};
        buttonBackRect = {550, 500, 200, 50};
        buttonNextRect = {100, 100, 200, 50};
        buttonViewNotesRect = {100, 200, 200, 50};
        buttonDeleteRect = {100, 300, 200, 50};

        font = TTF_OpenFont("fonts/Oswald-Bold.ttf", 28);
        if (!font)
        {
            cerr << "Failed to load font. SDL_ttf Error: " << TTF_GetError() << endl;
            return;
        }

        if (!gPromptTextTexture.loadFromRenderedText(renderer, font, "Happy Journaling!", textColor) ||
            !gInputTextTexture.loadFromRenderedText(renderer, font, " ", textColor))
        {
            cerr << "Failed to render text!" << endl;
            return;
        }
    }
    void handleMouseButtonDown()
    {
        //checking where the mouse is clicked
        SDL_Point mousePoint = {event.button.x, event.button.y};

    //when its on save button on second screen
        if (switchToNextScreen && SDL_PointInRect(&mousePoint, &buttonBackRect))
        {

            DoublyLinkedList.addNode(inputText);
            // Switch back to the previous screen after saving the note
            switchToNextScreen = false;
            viewNotesClicked = false;
            deleteNotesClicked = false;
        }
        //when its on create button
        else if (!switchToNextScreen && SDL_PointInRect(&mousePoint, &buttonNextRect))
        {
            // Switch to the next screen
            inputText = "";
            switchToNextScreen = true;
        }
        //when its on view notes button
        else if (!switchToNextScreen && SDL_PointInRect(&mousePoint, &buttonViewNotesRect))
        {
            inputText = "";
            selectedEntryNumber = -1;
            deleteNotesClicked = false;
            viewNotesClicked = true;
            selectedEntryData = "";  
            RenderText(selectedEntryData, 450, 140, true); 
        }
        //when its on delete button
        else if (!switchToNextScreen && SDL_PointInRect(&mousePoint, &buttonDeleteRect))
        {
            inputText = "";
            selectedEntryNumber = -1;
            deleteNotesClicked = true;
            viewNotesClicked = false;
            selectedEntryData = "";  
            RenderText(selectedEntryData, 450, 140, true); 
        }
    }
    void handleEvents()
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                    return;
                }
            }
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                handleMouseButtonDown();
            }
            else if (switchToNextScreen)
            {
                // Handle text input for the second screen
                handleTextInput(inputText, renderText, textColor);
            }
            else if (viewNotesClicked)
            {
                //it will ask for the entry  number to view
                handleFirstScreenTextInput(inputText, renderText, textColor, selectedEntryNumber, selectedEntryData, true, false);
            }
            else if (deleteNotesClicked)
            {
                //it will ask for the entry  number to delete
                handleFirstScreenTextInput(inputText, renderText, textColor, selectedEntryNumber, selectedEntryData, false, true);
            }
        }
    }
    void DeleteNotesClicked()
    {
        RenderText("Enter Entry Number To Delete:", 460, 55, true);
        gInputTextTexture.render(renderer, (SCREEN_WIDTH - gInputTextTexture.getWidth()) / 2, gPromptTextTexture.getHeight());

        if (renderText)
        {
            if (inputText != "")
            {
                gInputTextTexture.loadFromRenderedText(renderer, font, inputText.c_str(), textColor);
            }
            else
            {
                gInputTextTexture.loadFromRenderedText(renderer, font, " ", textColor);
            }
        }
    }
    void ViewNotesClicked()
    {  
        RenderText("Enter Entry Number To View:", 460, 55, true);
        gInputTextTexture.render(renderer, (SCREEN_WIDTH - gInputTextTexture.getWidth()) / 2, gPromptTextTexture.getHeight());

        if (renderText)
        {
            if (inputText != "")
            {
                gInputTextTexture.loadFromRenderedText(renderer, font, inputText.c_str(), textColor);
            }
            else
            {
                gInputTextTexture.loadFromRenderedText(renderer, font, " ", textColor);
            }
        }
        // Check if a valid entry number is selected and fetch the associated data
        if (selectedEntryNumber != -1)
        {
            node *selectedNode = DoublyLinkedList.getNodeByIndex(selectedEntryNumber);
            if (selectedNode)
            {
                selectedEntryData = selectedNode->getData();
                // Render the associated data on the screen
                RenderText(selectedEntryData, 450, 140, true);
            }
            else{
                selectedEntryData = "Wrong Entry number!";  
                RenderText(selectedEntryData, 450, 140, true); 
            }
        }
    }
    //when next screen is switched (the screen where we put journal entry)
    void SwitchToNextScreen()
    {
        renderTexture(background2, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        renderTexture(buttonBack, renderer, buttonBackRect.x, buttonBackRect.y, buttonBackRect.w, buttonBackRect.h);

        gPromptTextTexture.render(renderer, (SCREEN_WIDTH - gPromptTextTexture.getWidth()) / 2, 0);
        gInputTextTexture.render(renderer, (SCREEN_WIDTH - gInputTextTexture.getWidth()) / 2, gPromptTextTexture.getHeight());

        if (renderText)
        {
            if (inputText != "")
            {
                RenderText(inputText.c_str(), 240, 80, true);
            }
            else
            {

                RenderText(" ", 240, 80, true);
            }
        }
    }
    SDL_Texture *loadTexture(const string &path, SDL_Renderer *renderer)
    {
        SDL_Surface *loadedSurface = IMG_Load(path.c_str());
        if (!loadedSurface)
        {
            cerr << "Unable to load image " << path << ". IMG_Error: " << IMG_GetError() << endl;
            return NULL;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (!texture)
        {
            cerr << "Unable to create texture from " << path << ". SDL_Error: " << SDL_GetError() << endl;
        }

        SDL_FreeSurface(loadedSurface);

        return texture;
    }
    //displays texture string on screen
    void renderTexture(SDL_Texture *texture, SDL_Renderer *renderer, int x, int y, int w, int h)
    {
        SDL_Rect renderQuad = {x, y, w, h};
        SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
    }

    void cleanup()
    {
        SDL_DestroyTexture(background1);
        SDL_DestroyTexture(background2);
        SDL_DestroyTexture(buttonNext);
        SDL_DestroyTexture(buttonBack);
        SDL_DestroyTexture(buttonViewNotes);
        SDL_DestroyTexture(buttonDeleteNotes);
    }
    //displays notes list
    void renderNotesList(SDL_Renderer *renderer, const SDL_Color &textColor, bool wrapText)
    {
        int listX = 430;
        int listY = 100;
        int lineHeight = 70;
        selectedEntryData = "";
        RenderText(selectedEntryData, 450, 140, true);
        LTexture noteTextTexture;

        node *current = DoublyLinkedList.head;

        int noteCount = 0;
        while (current != NULL)
        {
            std::string noteText = "Entry No. " + std::to_string(noteCount + 1) + ": " + current->getDateTime();

            if (wrapText)
            {
                int wrapLength = 25;
                std::vector<std::string> wrappedLines;

                // Wrap the text
                size_t startPos = 0;
                while (startPos < noteText.length())
                {
                    size_t endPos = std::min(startPos + wrapLength, noteText.length());
                    size_t lastSpace = noteText.find_last_of(" ", endPos);
                    if (lastSpace != std::string::npos && lastSpace > startPos)
                    {
                        endPos = lastSpace;
                    }
                    wrappedLines.push_back(noteText.substr(startPos, endPos - startPos));
                    startPos = endPos + 1;
                }

                int yOffset = listY;
                for (const auto &line : wrappedLines)
                {
                    noteTextTexture.loadFromRenderedText(renderer, font, line, textColor);
                    noteTextTexture.render(renderer, listX, yOffset);

                    yOffset += noteTextTexture.getHeight(); // Adjust this value for spacing
                }
            }
            else
            {
                noteTextTexture.loadFromRenderedText(renderer, font, noteText, textColor);
                noteTextTexture.render(renderer, listX, listY);
            }

            listY += lineHeight;
            current = current->next;
            noteCount++;
        }

        noteTextTexture.free();
    }
    //displays text on screen
    void RenderText(string text, int x, int y, bool wrapText)
    {
        SDL_Color textColor = {100, 205, 139};
        LTexture noteTextTexture;
        //to wrap the text so it doesnt go out of the width of screen
        if (wrapText)
        {
            int wrapLength = 25;
            vector<string> wrappedLines;

            // Wrap the text
            size_t startPos = 0;
            while (startPos < text.length())
            {
                size_t endPos = min(startPos + wrapLength, text.length());
                size_t lastSpace = text.find_last_of(" ", endPos);
                if (lastSpace != string::npos && lastSpace > startPos)
                {
                    endPos = lastSpace;
                }
                wrappedLines.push_back(text.substr(startPos, endPos - startPos));
                startPos = endPos + 1;
            }

            int yOffset = y;
            for (const auto &line : wrappedLines)
            {
                noteTextTexture.loadFromRenderedText(renderer, font, line, textColor);
                noteTextTexture.render(renderer, x, yOffset);

                yOffset += noteTextTexture.getHeight(); // value for spacing
            }
        }
        else
        {
            noteTextTexture.loadFromRenderedText(renderer, font, text, textColor);
            noteTextTexture.render(renderer, x, y);
        }
    }
    //when a keyboard key is pressed this function displays that input on screen
    void handleTextInput(string &inputText, bool &renderText, SDL_Color textColor)
    {

        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
            {
                inputText.pop_back();
                renderText = true;
            }
            else if (event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
            {
                SDL_SetClipboardText(inputText.c_str());
            }
            else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
            {
                char *tempText = SDL_GetClipboardText();
                inputText = tempText;
                SDL_free(tempText);
                renderText = true;
            }
        }
        else if (event.type == SDL_TEXTINPUT)
        {
            if (!(SDL_GetModState() & KMOD_CTRL && (event.text.text[0] == 'c' || event.text.text[0] == 'C' || event.text.text[0] == 'v' || event.text.text[0] == 'V')))
            {
                inputText += event.text.text;
                renderText = true;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            SDL_Point mousePoint = {event.button.x, event.button.y};
        }
    }
    //same function as above but for first screen
    void handleFirstScreenTextInput(string &inputText, bool &renderText, SDL_Color textColor, int &selectedEntryNumber, string &selectedEntryData, bool view, bool dlt)
    {

        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
            {
                inputText.pop_back();
                renderText = true;
            }
            else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
            {
                // Try to convert the input to an integer to get the selected entry number
                try
                {
                    selectedEntryNumber = stoi(inputText);
                    if (selectedEntryNumber <= 0 || selectedEntryNumber > DoublyLinkedList.size())
                    {
                        // Invalid entry number, reset and render an error message
                        inputText = "";
                        selectedEntryNumber = -1;
                        selectedEntryData = "Invalid entry number!";
                        renderText = true;
                    }
                    else
                    {
                        if (view)
                        {
                            node *selectedNode = DoublyLinkedList.getNodeByIndex(selectedEntryNumber);
                            inputText = "";
                            renderText = true;
                            if (selectedNode)
                            {
                                selectedEntryData = selectedNode->getData();
                            }
                        }
                        else if (dlt)
                        {
                            node *selectedNode = DoublyLinkedList.getNodeByIndex(selectedEntryNumber);
                            if (selectedNode)
                            {
                                inputText = "";
                                bool check = DoublyLinkedList.deleteNode(selectedEntryNumber);
                                if(!check){
                                    selectedEntryData = "Wrong Entry number!";  
                                    RenderText(selectedEntryData, 450, 140, true); 
                                }
                                else{
                                    selectedEntryData = "Note " + to_string(selectedEntryNumber) + " deleted.";  
                                    RenderText(selectedEntryData, 450, 140, true); 
                                }
                            }
                        }
                    }
                }
                catch (invalid_argument &e)
                {
                    // Invalid input, reset and render an error message
                    inputText = "";
                    selectedEntryNumber = -1;
                    RenderText("Invalid Input:", 330, 200, 10);
                    renderText = true;
                }
            }
        }
        else if (event.type == SDL_TEXTINPUT)
        {
            if (!(SDL_GetModState() & KMOD_CTRL && (event.text.text[0] == 'c' || event.text.text[0] == 'C' || event.text.text[0] == 'v' || event.text.text[0] == 'V')))
            {
                inputText += event.text.text;
                renderText = true;
            }
        }
    }
};
