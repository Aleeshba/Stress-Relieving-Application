#include <cmath>
#include <sstream>
#include "baseClass.hpp"
const int screenWidth = 800;
const int screenHeight = 700;
class PaintApp : public StressReliever{
public:
    PaintApp() :StressReliever("Paint App", 800, 700), brushSize(5), toolType(ToolType::PENCIL), selectedColor({0, 0, 0}) {
        x1 = y1 = x2 = y2 = x3 = y3 = length = width = 0; 
        flag = pickingColor = drawing = drawingShape = false;
        initialize();
    }
    ~PaintApp() {
    }
    void run() {
        while (event.type != SDL_QUIT && event.key.keysym.sym != SDLK_ESCAPE) {
            handleEvents();               
        }
    }
    enum class ToolType {
        PENCIL,
        BRUSH,
        ERASER,
        BUCKET,
        NONE
    };
    enum class ShapeType {
        NONE,
        SQUARE,
        CIRCLE,
        TRIANGLE,
        LINE,
    };
    enum class ActionType {
        INCREASE_BRUSH,
        DECREASE_BRUSH,
        UNDO,
        REDO,
        SAVE,
        NONE
    };
    struct ImageButton {
        SDL_Texture* texture;
        SDL_Rect rect;
        ToolType toolType;
        ShapeType shapeType;
        ActionType actionType;
    };
    struct Point {
        int x;
        int y;
    };
    struct ColoredSquare {
        SDL_Rect rect;
        SDL_Color color;
    };
private:
    int brushSize, x1 ,y1 ,x2 ,y2 ,x3 ,y3, length, width, paletteX, paletteY, paletteCellSize;
    ToolType toolType;
    ShapeType shapeType;
    bool drawingShape, drawing, flag, pickingColor;
    stack<vector<vector<SDL_Color>>> undoStack;
    stack<vector<vector<SDL_Color>>> redoStack;
    vector<vector<SDL_Color>> canvas;
    SDL_Color selectedColor;
    vector<SDL_Color> colorPalette;
    Point initialShapePoint;
    vector<ImageButton> toolButtons;
    void initialize(){
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        font = TTF_OpenFont("fonts/arial.ttf", 24); 
        backgroundTexture = loadImage("images/PaintBg.jpeg");
        if (!font || !backgroundTexture) {
            cerr << "Failed to load font or texture: " << TTF_GetError() << endl;
        }
        drawImage(backgroundTexture, 0,0,screenWidth, screenHeight/6);
        initializeCanvas();
        createColorPalette();
        drawColorPalette();
        loadToolButtons();
    }
    SDL_Texture* loadImage(string filename) {
        SDL_Surface* surface = IMG_Load(filename.c_str());
        if (!surface) {
            cerr << "Failed to load image: " << IMG_GetError() << endl;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        return texture;
    }
    void drawImage(SDL_Texture* texture, int x, int y, int w, int h) {
        SDL_Rect imageRect = {x, y, w, h};  // Adjust the size and position as needed
        SDL_RenderCopy(renderer, texture, nullptr, &imageRect);
        SDL_RenderPresent(renderer);
    }
    void initializeCanvas() {
        canvas.resize(screenHeight, std::vector<SDL_Color>(screenWidth, {255, 255, 255}));
    }
    void loadToolButtons() {
        addButton("images/square.png", 10, 10, ToolType::NONE, ActionType::NONE, ShapeType::SQUARE);
        addButton("images/circle.png", 97, 10, ToolType::NONE, ActionType::NONE, ShapeType::CIRCLE);
        addButton("images/triangle.png", 184, 10, ToolType::NONE, ActionType::NONE, ShapeType::TRIANGLE);
        addButton("images/line.png", 271, 10, ToolType::NONE, ActionType::NONE, ShapeType::LINE);
        addButton("images/increase.png", 358, 10, ToolType::NONE, ActionType::INCREASE_BRUSH, ShapeType:: NONE);
        addButton("images/decrease.png", 445, 10, ToolType::NONE, ActionType::DECREASE_BRUSH, ShapeType:: NONE);
        addButton("images/pencil.png", 10, 63, ToolType::PENCIL, ActionType::NONE, ShapeType:: NONE);
        addButton("images/brush.png", 97, 63, ToolType::BRUSH, ActionType::NONE, ShapeType:: NONE);
        addButton("images/eraser.png", 184, 63, ToolType::ERASER, ActionType::NONE, ShapeType:: NONE);
        addButton("images/bucket.png", 271, 63, ToolType::BUCKET, ActionType::NONE, ShapeType:: NONE);
        addButton("images/undo.png", 358, 63, ToolType::NONE, ActionType::UNDO, ShapeType:: NONE);
        addButton("images/redo.png", 445, 63, ToolType::NONE, ActionType::REDO, ShapeType:: NONE);
    }
    void addButton(const string& imagePath, int x, int y, ToolType toolType, ActionType actionType, ShapeType shapeType) {
        SDL_Surface* surface = IMG_Load(imagePath.c_str());
        if (!surface) {
            cerr << "Failed to load image " << imagePath << ": " << IMG_GetError() << endl;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) {
            cerr << "Failed to create texture from surface: " << SDL_GetError() << endl;
        }
        SDL_Rect rect = {x, y, 80, 43}; // Adjust the size of the button as needed
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        // Present the renderer
        SDL_RenderPresent(renderer);
        ImageButton button;
        button.texture = texture;
        button.rect = rect;
        button.toolType = toolType;
        button.shapeType = shapeType;
        button.actionType = actionType;
        toolButtons.push_back(button);
    }
    void handleToolButtonClick(const SDL_MouseButtonEvent& button) {
        for (const auto& toolButton : toolButtons) {
            if (button.x >= toolButton.rect.x && button.x <= toolButton.rect.x + toolButton.rect.w &&
                button.y >= toolButton.rect.y && button.y <= toolButton.rect.y + toolButton.rect.h) {
                // User clicked on a tool button
                if (toolButton.toolType == ToolType::PENCIL || toolButton.toolType == ToolType::BRUSH ||
                    toolButton.toolType == ToolType::ERASER || toolButton.toolType == ToolType::BUCKET) {
                    toolType = toolButton.toolType;
                    shapeType = ShapeType::NONE;  // Reset shape type when selecting pencil, brush, eraser, or bucket
                } else if (toolButton.actionType == ActionType::INCREASE_BRUSH) {
                    // Handle the increase brush size action
                    brushSize = std::min(brushSize + 1, 50);
                } else if (toolButton.actionType == ActionType::DECREASE_BRUSH) {
                    // Handle the decrease brush size action
                    brushSize = std::max(brushSize - 1, 1);
                } else if (toolButton.actionType == ActionType::UNDO) {
                    // Handle the undo action
                    undo();
                } else if (toolButton.actionType == ActionType::REDO) {
                    // Handle the redo action
                    redo();
                } else if (toolButton.actionType == ActionType::SAVE) {
                    // Handle the save action
                    saveCanvas();
                } else {
                    shapeType = toolButton.shapeType;  // Set shape type based on the tool button
                    toolType = ToolType::PENCIL;  // Reset tool type when selecting a shape
                }
                return;
            }
        }
    }
    void createColorPalette() {
        paletteX = screenWidth - 100;
        paletteY = 25;
        paletteCellSize = 30;
        colorPalette = {
            {0, 0, 0},    // Black
            {255, 0, 0},    // Red
            {0, 255, 0},    // Green
            {0, 0, 255},    // Blue
            {255, 255, 0},  // Yellow
            {128, 0, 128},   // Purple
            {155, 131, 23},    //Brown
            {0, 255, 255},  // Cyan
            {255, 0, 255},  // Magenta
            {128, 128, 128}, // Gray
            {255, 165, 0},   // Orange
            {0, 128, 128},   // Teal
            {128, 0, 0},     // Maroon
            {0, 128, 0}      // Olive
        };
    }
    // Handle various SDL events during the program execution
void handleEvents() {
    // Poll SDL events in the event queue
    while (SDL_PollEvent(&event)) {
        // Check for keydown events
        if (event.type == SDL_KEYDOWN) {
            // Check if the ESC key is pressed, indicating a request to exit
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                return;
            }
        }
        // Check for quit events (window close button)
        else if (event.type == SDL_QUIT) {
            return;
        }
        // Check for mouse button down events
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            // Handle mouse down event using the details provided in the event.button structure
            handleMouseDown(event.button);
        }
        // Check for mouse button up events
        else if (event.type == SDL_MOUSEBUTTONUP) {
            // Reset the drawing flag
            drawing = false;

            // Check if the mouse release position is within the canvas area
            if (event.button.y >= screenHeight / 6 && event.button.y < screenHeight) {
                // Check if the user was drawing a shape
                if (drawingShape) {
                    // Perform actions based on the selected shape type
                    switch (shapeType) {
                        case ShapeType::SQUARE:
                            drawSquareLines();
                            break;
                        case ShapeType::TRIANGLE:
                            drawTriangleLines();
                            break;
                        case ShapeType::CIRCLE:
                            drawCircleOnCanvas();
                            break;
                        case ShapeType::LINE:
                            drawLineOnCanvas(x1, y1, x2, y2);
                            break;
                    }

                    // Reset variables related to drawing shapes
                    x1 = y1 = x2 = y2 = x3 = y3 = length = width = 0;
                }

                // Save the canvas after handling the drawing events
                saveCanvas();
            }
        }
        // Check for mouse motion events
        else if (event.type == SDL_MOUSEMOTION) {
            // Handle mouse motion event using the details provided in the event.motion structure
            handleMouseMotion(event.motion);
        }
    }
}
    void handleMouseDown(const SDL_MouseButtonEvent& button) {
        // Check for tool button clicks
        handleToolButtonClick(button);
        const int colorsPerRow = colorPalette.size() / 2;
        const int spacing = 5; // Adjust the spacing value as needed
        const int xOffset = 160;
        // Check if the user clicked on the color palette
        for (size_t i = 0; i < colorPalette.size(); ++i) {
            int row = i / colorsPerRow;
            int col = i % colorsPerRow;
            int x = paletteX - xOffset + col * (paletteCellSize + spacing);
            int y = paletteY + row * (paletteCellSize + spacing);
            if (button.x >= x && button.x <= x + paletteCellSize &&
                button.y >= y && button.y <= y + paletteCellSize) {
                // User clicked on a color in the palette
                if (pickingColor) {
                    // If pickingColor is true, set the selected color
                    selectedColor = colorPalette[i];
                    pickingColor = false;
                } else {
                    // If pickingColor is false, toggle pickingColor
                    pickingColor = true;
                }
                // Redraw the color palette to reflect the selection
                drawColorPalette();
                return;
            }
        }
        // If not in the color palette, handle drawing actions
        drawing = true;
        switch (button.button) {
            case SDL_BUTTON_LEFT:
                switch (shapeType) {
                    case ShapeType::SQUARE:
                    case ShapeType::CIRCLE:
                    case ShapeType::TRIANGLE:
                    case ShapeType::LINE:
                        startDrawing(button.x, button.y);
                        break;
                    default:
                        draw(button.x, button.y);
                        pickingColor = false;
                        drawing = true;
                        break;
                }
                break;
            case SDL_BUTTON_RIGHT:
                selectedColor = getPixelColor(button.x, button.y);
                break;
        }
    }
    bool handleColorPaletteClick(int x, int y) {
        const int colorsPerRow = colorPalette.size() / 2;
        const int spacing = 5;
        const int xOffset = 160;
        for (size_t i = 0; i < colorPalette.size(); ++i) {
            int row = i / colorsPerRow;
            int col = i % colorsPerRow;
            int paletteX = screenWidth - xOffset + col * (paletteCellSize + spacing);
            int paletteY = paletteY + row * (paletteCellSize + spacing);
            if (x >= paletteX && x <= paletteX + paletteCellSize &&
                y >= paletteY && y <= paletteY + paletteCellSize) {
                // User clicked on a color in the palette
                if (pickingColor) {
                    // If pickingColor is true, set the selected color
                    selectedColor = colorPalette[i];
                    pickingColor = false;
                } else {
                    // If pickingColor is false, toggle pickingColor
                    pickingColor = true;
                }
                // Redraw the color palette to reflect the selection
                drawColorPalette();
                return true; // Indicate that the click was on the color palette
            }
        }
        return false; // Indicate that the click was not on the color palette
    }
    void drawColorPalette() {
        const int colorsPerRow = colorPalette.size() / 2;
        const int spacing = 5; // Adjust the spacing value as needed
        const int xOffset = 160; // Adjust the offset value as needed
        for (size_t i = 0; i < colorPalette.size(); ++i) {
            int row = i / colorsPerRow;
            int col = i % colorsPerRow;
            int x = paletteX - xOffset + col * (paletteCellSize + spacing);
            int y = paletteY + row * (paletteCellSize + spacing);
            SDL_Color color = colorPalette[i];
            SDL_Rect colorRect = {x, y, paletteCellSize, paletteCellSize};
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
            // Highlight the selected color
            if (color.r == selectedColor.r && color.g == selectedColor.g && color.b == selectedColor.b && !pickingColor) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawRect(renderer, &colorRect);
            }
            SDL_RenderFillRect(renderer, &colorRect);
        }
        SDL_RenderPresent(renderer);
    }
    void handleMouseMotion(SDL_MouseMotionEvent motion) {
        if (drawing && motion.x >=0 && motion.y > screenHeight/6) {
            switch (shapeType) {
                case ShapeType::SQUARE:
                    drawSquare(motion.x, motion.y);
                    break;
                case ShapeType::CIRCLE:
                    drawCircle(motion.x, motion.y);
                    break;
                case ShapeType::TRIANGLE:
                    drawTriangle(motion.x, motion.y);
                    break;
                case ShapeType::LINE:
                    drawLine(motion.x, motion.y);
                    break;
                default:
                    draw(motion.x, motion.y);
                    break;
            }
        }
    }
    void draw(int x, int y) {
        // Check if the coordinates are within the canvas area (excluding the image)
        if (y >= screenHeight / 6 && y < screenHeight) {
            switch (toolType) {
                case ToolType::PENCIL:
                    brushSize = max(brushSize - 3, 1);
                    drawPoint(x, y);
                    brushSize = max(brushSize + 3, 1);
                    break;
                case ToolType::BRUSH:{
                    drawPoint(x, y);
                    break;
                }
                case ToolType::ERASER:
                    erasePoint(x, y);
                    break;
                case ToolType::BUCKET:
                    fillBucket(x, y, selectedColor);
                    break;
            }
            flag = false;
        }
    }
    void drawPoint(int x, int y) {
        // Check if the point is within the canvas area (excluding the image)
        if (y >= screenHeight / 6 && y < screenHeight) {
            for (int i = -brushSize; i <= brushSize; ++i) {
                for (int j = -brushSize; j <= brushSize; ++j) {
                    int newX = x + i;
                    int newY = y + j;
                    // Check if the new coordinates are within the canvas area
                    if (newX >= 0 && newX < screenWidth && newY >= screenHeight / 6 && newY < screenHeight) {
                        canvas[newY][newX] = selectedColor;
                        SDL_SetRenderDrawColor(renderer, selectedColor.r, selectedColor.g, selectedColor.b, SDL_ALPHA_OPAQUE);
                        SDL_RenderDrawPoint(renderer, newX, newY);
                    }
                }
            }
            SDL_RenderPresent(renderer);
        }
    }
    void erasePoint(int x, int y) {
        // Check if the point is within the canvas area (excluding the image)
        if (y >= screenHeight / 6 && y < screenHeight) {
            for (int i = -brushSize; i <= brushSize; ++i) {
                for (int j = -brushSize; j <= brushSize; ++j) {
                    int newX = x + i;
                    int newY = y + j;
                    // Check if the new coordinates are within the canvas area
                    if (newX >= 0 && newX < screenWidth && newY >= screenHeight / 6 && newY < screenHeight) {
                        canvas[newY][newX] = {255, 255, 255};
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                        SDL_RenderDrawPoint(renderer, newX, newY);
                    }
                }
            }
            SDL_RenderPresent(renderer);
        }
    }
    void fillBucket(int x, int y, SDL_Color targetColor) {
        // Check if the starting point is within the canvas area (excluding the image)
        if (y < screenHeight / 6 || y >= screenHeight) {
            return;
        }
        SDL_Color currentPixelColor = canvas[y][x];
        if (currentPixelColor.r == targetColor.r && currentPixelColor.g == targetColor.g && currentPixelColor.b == targetColor.b) {
            return; // Already filled with the target color
        }
        queue<pair<int, int>> pixelsQueue;
        pixelsQueue.push({x, y});
        while (!pixelsQueue.empty()) {
            auto [currentX, currentY] = pixelsQueue.peek();
            pixelsQueue.pop();
            if (currentX < 0 || currentX >= screenWidth || currentY < screenHeight / 6 || currentY >= screenHeight) {
                continue;
            }
            SDL_Color& currentPixel = canvas[currentY][currentX];
            if (currentPixel.r == currentPixelColor.r && currentPixel.g == currentPixelColor.g && currentPixel.b == currentPixelColor.b) {
                currentPixel = targetColor;
                SDL_SetRenderDrawColor(renderer, targetColor.r, targetColor.g, targetColor.b, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawPoint(renderer, currentX, currentY);
                pixelsQueue.push({currentX - 1, currentY});
                pixelsQueue.push({currentX + 1, currentY});
                pixelsQueue.push({currentX, currentY - 1});
                pixelsQueue.push({currentX, currentY + 1});
            }
        }
        SDL_RenderPresent(renderer);
    }
    SDL_Color getPixelColor(int x, int y) {
        if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
            return canvas[y][x];
        }
        return {0, 0, 0}; // Default color if out of bounds
    }
    void undo() {
        if (!undoStack.empty()) {
            redoStack.push(canvas);
            undoStack.pop();
            if(undoStack.empty()){
                clearCanvas();
            } else {
                canvas = undoStack.peek();
                renderCanvas();
            }
        }
    }
    void saveCanvas() {
        undoStack.push(canvas); // Save current state for potential undo
    }
    void redo() {
        if (!redoStack.empty()) {
            undoStack.push(redoStack.peek());
            redoStack.pop();
            canvas = undoStack.peek();
            renderCanvas();
        }
    }
    void renderCanvas() {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        drawImage(backgroundTexture, 0, 0 , screenWidth, screenHeight/6);
        drawColorPalette();
        loadToolButtons();
            for (int y = 0; y < screenHeight; ++y) {
                for (int x = 0; x < screenWidth; ++x) {
                    SDL_Color pixelColor = canvas[y][x];
                    SDL_SetRenderDrawColor(renderer, pixelColor.r, pixelColor.g, pixelColor.b, SDL_ALPHA_OPAQUE);
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        drawImage(backgroundTexture, 0, 0 , screenWidth, screenHeight/6);
        drawColorPalette();
        loadToolButtons();
        // Reset shape drawing state
        drawingShape = false;
        initialShapePoint = {0, 0};
        SDL_RenderPresent(renderer);
    }
    void clearCanvas(int x = 0, int y = 116, int w = 800, int h = 584) {
        SDL_Rect canvasRect = {x, y, w, h};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(renderer, &canvasRect);
        drawColorPalette();
        loadToolButtons();
        SDL_RenderPresent(renderer);
    }
    double calculateDistance(int x1, int y1, int x2, int y2) {
        return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    }
    void startDrawing(int x, int y) {
        drawingShape = true;
        initialShapePoint = { x, y };
    }
 // Draw a line on the canvas using Bresenham's line algorithm
void drawLineOnCanvas(int x1, int y1, int x2, int y2) {
    // Calculate the absolute differences between the two end points
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    // Determine the direction of movement along the x and y axes
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    // Initialize the error term
    int err = dx - dy;
    // Loop through the points along the line using Bresenham's algorithm
    while (true) {
        // Check if the current point is within the canvas boundaries
        if (x1 >= 0 && x1 < screenWidth && y1 >= screenHeight / 6 && y1 < screenHeight) {
            // Set the color of the canvas at the current point to the selected color
            canvas[y1][x1] = selectedColor;
        }
        // Check if the end point of the line is reached
        if (x1 == x2 && y1 == y2) break;
        // Calculate the error term for the next iteration
        int e2 = 2 * err;
        // Update the x-coordinate if the error term is greater than the negative of the y difference
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        // Update the y-coordinate if the error term is less than the x difference
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

    // Draw a square on the canvas based on the provided end coordinates (x, y)
void drawSquare(int x, int y) {
    // Check if the initial shape point is set and it is within the canvas area, and drawing of a shape is in progress
    if (initialShapePoint.x != 0 && initialShapePoint.y >= screenHeight / 6 && drawingShape) {
        // Calculate the length and width of the square based on the provided end coordinates
        length = abs(x - initialShapePoint.x);
        width = abs(y - initialShapePoint.y);
        // Determine the top-left corner (x1, y1) of the square
        x1 = min(initialShapePoint.x, x);
        y1 = min(initialShapePoint.y, y);
        // Clear the canvas within the bounding rectangle of the square
        clearCanvas(x1, y1, length, width);
        // Check if the square can fit within the canvas area
        if (x >= 0 && x + length < screenWidth && y >= screenHeight / 6 && y + width < screenHeight) {
            // Set the render draw color to the selected color with full opacity
            SDL_SetRenderDrawColor(renderer, selectedColor.r, selectedColor.g, selectedColor.b, SDL_ALPHA_OPAQUE);
            // Draw the four sides of the square
            SDL_RenderDrawLine(renderer, x1, y1, x1 + length, y1);
            SDL_RenderDrawLine(renderer, x1, y1 + width, x1 + length, y1 + width);
            SDL_RenderDrawLine(renderer, x1, y1, x1, y1 + width);
            SDL_RenderDrawLine(renderer, x1 + length, y1, x1 + length, y1 + width);
            // Present the renderer to update the display
            SDL_RenderPresent(renderer);
            // Set the flag to indicate that drawing is successful
            flag = true;
        }
    }
}
    // Draw a circle on the canvas using the midpoint circle drawing algorithm
void drawCircle(int x, int y) {
    // Check if the initial point is set and the shape drawing is in progress
    if (initialShapePoint.x != 0 && initialShapePoint.y >= screenHeight / 6 && drawingShape) {
        // Clear the area where the circle was previously drawn
        clearCanvas(initialShapePoint.x - length, initialShapePoint.y - length, initialShapePoint.x + length, initialShapePoint.y + length);
        // Set the center coordinates of the circle
        x1 = initialShapePoint.x;
        y1 = initialShapePoint.y;
        // Calculate the radius of the circle based on the distance from the center to the current mouse position
        length = static_cast<int>(calculateDistance(x1, y1, x, y));
        // Check if the entire circle is within the canvas boundaries
        if (x + length >= 0 && x + length < screenWidth && y + length >= screenHeight / 6 && y + length < screenHeight) {
            // Define the number of points to approximate the circle
            const int points = 100;
            // Define the step size between points along the circle circumference
            const double stepSize = 0.005;
            // Iterate through the points along the circle using polar coordinates
            for (int i = 0; i < points; ++i) {
                // Calculate the angle for the current point
                double angle = i * stepSize;
                int drawX, drawY;
                // Iterate through the circumference of the circle
                for (double t = 0; t < 2 * M_PI; t += stepSize) {
                    // Calculate the coordinates of the point along the circumference
                    drawX = static_cast<int>(x1 + length * cos(angle + t));
                    drawY = static_cast<int>(y1 + length * sin(angle + t));
                    // Check if the calculated point is within the canvas boundaries
                    if (drawX >= 0 && drawX < screenWidth && drawY >= screenHeight / 6 && drawY < screenHeight) {
                        // Set the color of the canvas at the calculated point to the selected color
                        SDL_SetRenderDrawColor(renderer, selectedColor.r, selectedColor.g, selectedColor.b, SDL_ALPHA_OPAQUE);
                        // Draw a point on the canvas at the calculated coordinates
                        SDL_RenderDrawPoint(renderer, drawX, drawY);
                    }
                }
            }
            // Present the renderer to display the updated canvas
            SDL_RenderPresent(renderer);
            // Set the flag to indicate that the drawing operation is complete
            flag = true;
        }
    }
}
// Draw an equilateral triangle on the canvas
void drawTriangle(int x, int y) {
    // Check if the initial point is set and the shape drawing is in progress
    if (initialShapePoint.x != 0 && initialShapePoint.y >= screenHeight / 6 && drawingShape) {
        // Clear the area within the bounding box of the current triangle, considering the brush size
        int baseX = initialShapePoint.x;
        int baseY = initialShapePoint.y;
        int sideLength = min(abs(x - initialShapePoint.x), abs(y - initialShapePoint.y));
        clearCanvas(baseX - sideLength - brushSize, baseY - brushSize, baseX + sideLength + brushSize, baseY + sideLength + brushSize);
        // Calculate the vertices of the equilateral triangle
        x1 = baseX - sideLength / 2;
        y1 = baseY + sideLength;
        x2 = baseX;
        y2 = baseY;
        x3 = baseX + sideLength / 2;
        y3 = baseY + sideLength;
        // Draw the three sides of the equilateral triangle
        SDL_SetRenderDrawColor(renderer, selectedColor.r, selectedColor.g, selectedColor.b, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
        SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
        // Present the renderer to display the updated canvas
        SDL_RenderPresent(renderer);
        // Set the flag to indicate that the drawing operation is complete
        flag = true;
    }
}
// Draw a line on the canvas
void drawLine(int x, int y) {
    // Check if the initial point is set and the drawing is within the canvas area
    if (initialShapePoint.x != 0 && initialShapePoint.y >= screenHeight / 6) {
        // Set the starting and ending points of the line
        x1 = initialShapePoint.x;
        y1 = initialShapePoint.y;
        x2 = x;
        y2 = y;
        // Clear the area within the bounding box of the line, considering the brush size
        clearCanvas(x1 - brushSize, y1 - brushSize, x2 + brushSize, y2 + brushSize);
        // Set the render draw color and draw the line
        SDL_SetRenderDrawColor(renderer, selectedColor.r, selectedColor.g, selectedColor.b, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        // Present the renderer to display the updated canvas
        SDL_RenderPresent(renderer);
        // Set the flag to indicate that the drawing operation is complete
        flag = true;
    }
}
// Draw a circle on the canvas
void drawCircleOnCanvas() {
    int points = 100;
    double stepSize = 0.005;
    // Iterate through points to draw the circle using parametric equations
    for (int i = 0; i < points; ++i) {
        double angle = i * stepSize;
        int drawX, drawY;       
        // Iterate through the circumference of the circle
        for (double t = 0; t < 2 * M_PI; t += stepSize) {
            // Calculate the coordinates of points on the circle
            drawX = static_cast<int>(x1 + length * cos(angle + t));
            drawY = static_cast<int>(y1 + length * sin(angle + t));
            // Update the canvas if the point is within the canvas area
            if (drawX >= 0 && drawX < screenWidth && drawY >= screenHeight / 6 && drawY < screenHeight) {
                canvas[drawY][drawX] = selectedColor;
            }
        }
    }
}
    void drawSquareLines(){
        drawLineOnCanvas(x1, y1, x1 + length, y1);                  // Top
        drawLineOnCanvas(x1, y1, x1, y1 + width);                   // Left
        drawLineOnCanvas(x1 + length, y1, x1 + length, y1 + width); // Right
        drawLineOnCanvas(x1, y1 + width, x1 + length, y1 + width);  // Down
    }
    void drawTriangleLines(){
        drawLineOnCanvas(x1,y1,x2,y2);
        drawLineOnCanvas(x2,y2,x3,y3);
        drawLineOnCanvas(x3,y3,x1,y1);
    }
};