#include <fstream>
#include "baseClass.hpp"
const int Width = 800;
const int Height = 700;
int GRID_SIZE = 5;
const int LEVELS = 5;
const int MAX = 8;
int CELL_SIZE = 520 / GRID_SIZE;

// Structure for RGBA color
struct ColorRGBA
{
    Uint8 r, g, b, a;
};

// Array of colors for dots
const ColorRGBA COLORS_ARRAY[] = {
    {255, 255, 255, 255}, //White
    {245, 128, 196, 255}, //Color 1
    {117, 117, 255, 255}, //Color 2
    {187, 145, 241, 255}, //Color 3
    {141, 73, 123, 255},  //Color 4
    {123, 231, 240, 255}, //Color 5
    {121, 247, 167, 255}, //Color 6
};

// Class representing the game grid
class Grid
{
public:
    Grid()
    {
        reset();
    }

    // Method to reset the grid
    void reset()
    {
        for (int row = 0; row < GRID_SIZE; ++row)
        {
            for (int col = 0; col < GRID_SIZE; ++col)
            {
                data[row][col] = 0;
                vis[row][col] = false;
            }
        }
    }

    // Method to load grid data from a file
    void loadFromFile(string filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        for (int row = 0; row < GRID_SIZE; row++)
        {
            string line;
            if (getline(file, line))
            {
                for (int col = 0; col < GRID_SIZE && col < line.size(); col++)
                {
                    int val = line[col] - '0';
                    data[row][col] = val;
                }
            }
        }
        file.close();
    }

    // Method to get the value at a specific grid position
    int getValue(int row, int col) const
    {
        return data[row][col];
    }

    // Method to set the value at a specific grid position
    void setValue(int row, int col, int value)
    {
        data[row][col] = value;
    }

    // Method to check if a move is valid
    bool valid(int i, int j, int &n)
    {
        return (i >= 0 && j >= 0 && i <= n - 1 && j <= n - 1 && vis[i][j] == false);
    }

    // Method to get the next color to fill
    pair<int, int> get_color()
    {
        for (int i = 0; i < GRID_SIZE; i++)
        {
            for (int j = 0; j < GRID_SIZE; j++)
            {
                if (data[i][j] != 0 && !vis[i][j])
                {
                    return {i, j};
                }
            }
        }
        return {-1, -1};
    }

    // Method to solve the puzzle
    bool solve(int i, int j, int color)
    {
        vis[i][j] = true; // Mark the current cell as visited

        // Explore all possible moves (up, down, left, right)
        for (int p = 0; p < 4; p++)
        {
            int i1 = i + dx[p];
            int j1 = j + dy[p];
            
            // Check if the move is valid
            if (valid(i1, j1, GRID_SIZE))
            {
                // If the adjacent cell has the same color, continue the path
                if (data[i1][j1] == color)
                {
                    vis[i1][j1] = true;                     // Mark the adjacent cell as visited
                    pair<int, int> nextColor = get_color(); // Find the next color to fill
                    if (nextColor.first == -1 && nextColor.second == -1)
                    {
                        return true; // Base case: No more colors to fill, solution found
                    }
                    int c = data[nextColor.first][nextColor.second];
                    if (solve(nextColor.first, nextColor.second, c))
                    {
                        return true; // Recursively continue the path
                    }
                    vis[i1][j1] = false; // Backtrack
                }
                // If the adjacent cell is empty, try filling it with the current color
                else if (data[i1][j1] == 0)
                {
                    data[i1][j1] = color;
                    if (solve(i1, j1, color))
                    {
                        return true; // Recursively continue the path
                    }
                    data[i1][j1] = 0; // Backtrack
                }
            }
        }

        // If no valid move is found, mark the current cell as unvisited and backtrack
        vis[i][j] = false;
        return false;
    }

private:
    int data[MAX][MAX];
    bool vis[MAX][MAX];
    int dx[4] = {-1, 0, 0, 1};
    int dy[4] = {0, -1, 1, 0};
};



class FlowFree : public StressReliever
{
public:
    FlowFree() : StressReliever("Flow free", 800, 700), currentColor(1), level(1), Margin(20), selectedDot(-1, -1), drawingLine(false), numMoves(0), points(0)
    {
        initialize();
    }

    // Method to initialize the game
    void initialize()
    {
        font = TTF_OpenFont("fonts/Oswald-Bold.ttf", 40);
        dataFont = TTF_OpenFont("fonts/arial.ttf", 25);
        backgroundTexture = loadImage("images/FlowFreeBg.jpg");
        backgroundMusic = Mix_LoadMUS("sound/flowFree-sounds.mp3");
        success = Mix_LoadWAV("sound/success.mp3");
        if (!font || !dataFont || !backgroundTexture || !backgroundMusic || !success)
        {
            cerr << "Failed to load font, texture or background Music: " << TTF_GetError() << endl;
            return;
        }
        srand(time(0));
    }
    ~FlowFree()
    {
        cleanup();
    }
    void run()
    {
        level = 1;
        GRID_SIZE = 5;
        while (level <= LEVELS && event.type != SDL_QUIT && event.key.keysym.sym != SDLK_ESCAPE)
        {
            CELL_SIZE = 520 / GRID_SIZE;
            Mix_PlayMusic(backgroundMusic, -1);
            string filename = "textFiles/level" + to_string(level) + ".txt";
            solver.reset();
            solver.loadFromFile(filename);
            pair<int, int> p = solver.get_color();
            int color = solver.getValue(p.first, p.second);
            bool res = solver.solve(p.first, p.second, color);
            grid.reset();
            grid.loadFromFile(filename);
            drawGrid();
            handleEvents();
            level++;
            if (level >= 3)
            {
                GRID_SIZE++;
            }
        }
    }

private:
    int currentColor;
    int level;
    pair<int, int> selectedDot;
    bool drawingLine;
    SDL_Texture *imageTexture;
    Grid grid;
    Grid solver;
    int numMoves;
    int Margin;
    int points;
    int backendArray[MAX][MAX];
    void drawText(const string text, int x, int y, const SDL_Color color, TTF_Font *f)
    {
        SDL_Surface *surface = TTF_RenderText_Solid(f, text.c_str(), color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect textRect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
        SDL_RenderPresent(renderer);
    }
    SDL_Texture *loadImage(string filename)
    {
        SDL_Surface *surface = IMG_Load(filename.c_str());
        if (!surface)
        {
            cerr << "Failed to load image: " << IMG_GetError() << endl;
        }
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
    }
    void drawImage(SDL_Texture *texture, int x, int y, int w, int h)
    {
        SDL_Rect imageRect = {x, y, w, h};
        SDL_RenderCopy(renderer, texture, nullptr, &imageRect);
    }
    int calculateScore()
    {
        return numMoves * (-0.5) + level * 10;
    }
    void drawGrid()
    {
        drawImage(backgroundTexture, 0, 0, 560, Height);
        // Calculate the vertical center of the window
        int centerY = Height / 2 - (GRID_SIZE * CELL_SIZE) / 2;
        // Draw the grid lines on the left side with a vertical offset and a left margin
        for (int row = 0; row < GRID_SIZE; ++row)
        {
            for (int col = 0; col < GRID_SIZE; ++col)
            {
                SDL_Rect cellRect = {Margin + col * CELL_SIZE, centerY + row * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Grid color
                SDL_RenderDrawRect(renderer, &cellRect);
                ColorRGBA cellColor;
                int value = grid.getValue(row, col);
                if (value >= 1 && value < MAX - 1)
                {
                    cellColor = COLORS_ARRAY[value];
                }
                else
                {
                    cellColor = COLORS_ARRAY[0];
                }
                SDL_SetRenderDrawColor(renderer, cellColor.r, cellColor.g, cellColor.b, cellColor.a);
                int centerX = Margin + col * CELL_SIZE + CELL_SIZE / 2;
                int cellCenterY = centerY + row * CELL_SIZE + CELL_SIZE / 2;
                int radius = CELL_SIZE / 3;
                drawFilledCircle(centerX, cellCenterY, radius, cellColor);
            }
        }
        drawImage(backgroundTexture, 2 * Margin + GRID_SIZE * CELL_SIZE, 0, Width - (2 * Margin + GRID_SIZE * CELL_SIZE), Height);
        SDL_RenderPresent(renderer);
        // Add your code to render game details here
        drawText("Level: " + to_string(level), Margin + GRID_SIZE * CELL_SIZE + 70, 210, {95, 123, 217, 255}, dataFont);
        drawText("Moves: " + to_string(numMoves), Margin + GRID_SIZE * CELL_SIZE + 70, 250, {95, 123, 217, 255}, dataFont);
        drawText("Points: " + to_string(points), Margin + GRID_SIZE * CELL_SIZE + 70, 290, {95, 123, 217, 255}, dataFont);
        // Present the renderer
        imageTexture = loadImage("images/replay.png");
        drawImage(imageTexture, Margin + GRID_SIZE * CELL_SIZE + 82, 350, 75, 75);
        SDL_RenderPresent(renderer);
    }
    void drawFilledCircle(int centerX, int centerY, int radius, ColorRGBA color)
    {
        for (int y = -radius; y <= radius; ++y)
        {
            for (int x = -radius; x <= radius; ++x)
            {
                if (x * x + y * y <= radius * radius)
                {
                    SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
                }
            }
        }
    }

// Handle various SDL events during the game
void handleEvents()
{
    // Find and load fixed dots from the level text file
    findFixedDots();

    // Flag indicating whether the current level is completed
    bool levelComplete = false;

    // Continue handling events until the level is completed
    while (!levelComplete)
    {
        // Process all pending SDL events
        while (SDL_PollEvent(&event))
        {
            // Check the type of each SDL event
            if (event.type == SDL_KEYDOWN)
            {
                // Check if the user pressed the ESC key to exit the game
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    return;
                }
            }
            else if (event.type == SDL_QUIT)
            {
                // Check if the user closed the window
                return;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                // Handle mouse button down event
                handleMouseDown(event.button.x, event.button.y);
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                // Handle mouse button up event
                handleMouseUp(event.button.x, event.button.y);
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                // Handle mouse motion event (user dragging the mouse)
                handleMouseMotion(event.motion.x, event.motion.y);
            }
        }

        // Check if the current grid matches the solution grid, indicating level completion
        levelComplete = compareGrids(grid, solver);
    }

    // Reset drawing state and selected dot after completing the level
    drawingLine = false;
    selectedDot = make_pair(-1, -1);

    // Reset the number of moves
    numMoves = 0;

    // If the level is completed
    if (levelComplete)
    {
        // Play success sound
        Mix_PlayChannel(-1, success, 0);

        // Display the level completion message
        string levelCompletionMessage = "Level " + to_string(level) + " Completed!";
        drawText(levelCompletionMessage, Width / 4, 250, {95, 123, 217, 255}, font);

        // If the current level is less than 5, delay for 2000 milliseconds and clear the renderer for the next level
        if (level < 5)
        {
            SDL_Delay(2000);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
        }
        else
        {
            // If the current level is 5 or greater, the game is won
            string gameWonMessage = "Game Won!";
            // Display the "Game Won!" message
            drawText(gameWonMessage, 210, 320, {95, 123, 217, 255}, font);
            // Delay for 3000 milliseconds before returning from the game loop
            SDL_Delay(3000);
            return;
        }
    }
}


  // Handle mouse button down event
void handleMouseDown(int x, int y)
{
    // Calculate the vertical center of the grid
    int centerY = Height / 2 - (GRID_SIZE * CELL_SIZE) / 2;

    // Check if the mouse click is within the "Reset" button area
    if (x >= 630 && x <= 685 && y >= 350 && y <= 405)
    {
        // Generate the filename for the current level's text file
        string filename = "textFiles/level" + to_string(level) + ".txt";

        // Reset the grid by loading data from the level text file
        grid.reset();
        grid.loadFromFile(filename);

        // Redraw the grid to reflect the changes
        drawGrid();

        // Exit the function to avoid processing additional actions for this click
        return;
    }

    // Calculate the row and column corresponding to the mouse click within the grid
    int row = (y - centerY) / CELL_SIZE;
    int col = (x - Margin) / CELL_SIZE;

    // Retrieve the value of the dot at the clicked position
    int dotValue = grid.getValue(row, col);

    // Check if the clicked position contains a valid dot (dotValue > 0)
    if (dotValue > 0)
    {
        // Store the coordinates of the selected dot
        selectedDot = std::make_pair(row, col);

        // Set the drawingLine flag to indicate that the user is drawing a line
        drawingLine = true;

        // Store the color of the selected dot for line drawing
        currentColor = dotValue;
    }

    // Increment the number of moves made by the user
    numMoves++;
}


// Handle mouse button up event
void handleMouseUp(int x, int y)
{
    // Check if the user is currently drawing a line
    if (drawingLine)
    {
        // Calculate the vertical center of the grid
        int centerY = Height / 2 - (GRID_SIZE * CELL_SIZE) / 2;

        // Calculate the row and column corresponding to the mouse release position within the grid
        int row = (y - centerY) / CELL_SIZE;
        int col = (x - Margin) / CELL_SIZE;

        // Check if the released position contains a dot of the same color
        if (grid.getValue(row, col) == currentColor)
        {
            // Connect dots and draw the line
            grid.setValue(selectedDot.first, selectedDot.second, currentColor);
            grid.setValue(row, col, currentColor);

            // Redraw the grid to reflect the changes
            drawGrid();
        }

        // Reset drawing-related flags and variables
        drawingLine = false;
        selectedDot = make_pair(-1, -1);

        // Update the user's points based on the completed line
        points += calculateScore();
    }
}


    void handleMouseMotion(int x, int y)
    {
        if (drawingLine)
        {
            // Draw the grid to clear previous lines
            drawGrid();
            int centerY = Height / 2 - (GRID_SIZE * CELL_SIZE) / 2;
            int row = (y - centerY) / CELL_SIZE;
            int col = (x - Margin) / CELL_SIZE;
            // Calculate the absolute differences between the current and selected coordinates
            int rowDiff = abs(row - selectedDot.first);
            int colDiff = abs(col - selectedDot.second);
            // Check if the movement is predominantly horizontal or vertical
            if (rowDiff > colDiff)
            {
                // Update the grid vertically
                int startRow = min(selectedDot.first, row);
                int endRow = max(selectedDot.first, row);
                for (int r = startRow; r <= endRow; ++r)
                {
                    int c = selectedDot.second;
                    // Check if the current cell is an initial dot and a fixed point
                    if (!(grid.getValue(r, c) == currentColor && isFixedDot(r, c)) && !isFixedDot(r, c))
                    {
                        grid.setValue(r, c, currentColor);
                    }
                }
            }
            else
            {
                // Update the grid horizontally
                int startCol = min(selectedDot.second, col);
                int endCol = max(selectedDot.second, col);
                for (int c = startCol; c <= endCol; ++c)
                {
                    int r = selectedDot.first;
                    // Check if the current cell is an initial dot and a fixed point
                    if (!(grid.getValue(r, c) == currentColor && isFixedDot(r, c)) && !isFixedDot(r, c))
                    {
                        grid.setValue(r, c, currentColor);
                    }
                }
            }
            // Present the renderer
            SDL_RenderPresent(renderer);
        }
    }
    void cleanup()
    {
        SDL_DestroyTexture(imageTexture);
    }
    bool compareGrids(Grid &userGrid, Grid &solutionGrid)
    {
        for (int row = 0; row < GRID_SIZE; ++row)
        {
            for (int col = 0; col < GRID_SIZE; ++col)
            {
                if (userGrid.getValue(row, col) < 0 || userGrid.getValue(row, col) > MAX || userGrid.getValue(row, col) != solutionGrid.getValue(row, col))
                {
                    return false;
                }
            }
        }
        return true;
    }
    
    void findFixedDots()
    {
        string filename = "textFiles/level" + to_string(level) + ".txt";
        ifstream file(filename);
        for (int row = 0; row < GRID_SIZE; ++row)
        {
            string line;
            if (getline(file, line))
            {
                for (int col = 0; col < GRID_SIZE && col < line.size(); ++col)
                {
                    int val = line[col] - '0';
                    backendArray[row][col] = val;
                }
            }
        }
        file.close();
    }

    bool isFixedDot(int row, int col)
    {
        vector<pair<int, int>> fixedPoints;
        for (int r = 0; r < GRID_SIZE; ++r)
        {
            for (int c = 0; c < GRID_SIZE; ++c)
            {
                if (backendArray[r][c] != 0)
                {
                    fixedPoints.push_back({r, c});
                }
            }
        }
        for (const auto &point : fixedPoints)
        {
            if (row == point.first && col == point.second)
            {
                return true;
            }
        }
        return false;
    }
};