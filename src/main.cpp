#include <raylib.h>

#include <iostream>
#include <random>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <cstdlib>

#include <vector>

#define SCREEN_WIDTH 1800
#define SCREEN_HEIGHT 900

#define BOARD_SIZE_X 10
#define BOARD_SIZE_Y 24

#define ENTIRE_BOARD_SIZE BOARD_SIZE_X *BOARD_SIZE_Y

#define SPAWN_COLOR Color{75, 75, 75, 255}
#define BOARD_COLOR Color{15, 15, 15, 255}
#define FILLED_COLOR Color{100, 100, 100, 255}

#define HOLD_PIECE_BACKGROUND Color{20, 25, 20, 255}

#define BACKGROUND_COLOR Color{25, 25, 75, 255}

#define OFFSET_X SCREEN_WIDTH / 2 - 250
#define OFFSET_Y SCREEN_HEIGHT / 2 - 300

#define CELL_SIZE 30

#define GRAVITY_TICKS 30

enum Cell_state
{
    EMPTY,
    SPAWN,
    ACTIVE,
    FILLED
} typedef Cell_state;

enum Piece_type
{
    I_PIECE,
    J_PIECE,
    L_PIECE,
    O_PIECE,
    S_PIECE,
    T_PIECE,
    Z_PIECE
} typedef Piece_type;

enum Movement_direction
{
    LEFT,
    RIGHT,
    DOWN,
    UP,
    TOP_RIGHT,
    TOP_LEFT,
    BOTTOM_RIGHT,
    BOTTOM_LEFT
} typedef Movement_direction;

enum Rotation
{
    NORMAL,
    CLOCKWISE,
    HALF,
    COUNTER_CLOCKWISE
} typedef Rotation;

struct Position
{
    int x;
    int y;
};

struct Tetromino
{
    struct Position positions[5];
    Rotation current_rotation = NORMAL;
};

// Respawns piece at the top
void set_piece(Tetromino *active_piece, Piece_type piece_type)
{

    switch (piece_type)
    {
    case I_PIECE:

        active_piece->positions[0] = {4, 0};
        active_piece->positions[1] = {5, 0};
        active_piece->positions[2] = {6, 0};
        active_piece->positions[3] = {7, 0};

        break;

    case J_PIECE:

        active_piece->positions[0] = {3, 0};
        active_piece->positions[1] = {3, 1};
        active_piece->positions[2] = {4, 1};
        active_piece->positions[3] = {5, 1};

        break;

    case L_PIECE:

        active_piece->positions[0] = {3, 1};
        active_piece->positions[1] = {4, 1};
        active_piece->positions[2] = {5, 1};
        active_piece->positions[3] = {5, 0};

        break;

    case O_PIECE:

        active_piece->positions[0] = {5, 0};
        active_piece->positions[1] = {6, 0};
        active_piece->positions[2] = {5, 1};
        active_piece->positions[3] = {6, 1};

        break;

    case S_PIECE:

        active_piece->positions[0] = {5, 0};
        active_piece->positions[1] = {6, 0};
        active_piece->positions[2] = {4, 1};
        active_piece->positions[3] = {5, 1};

        break;

    case T_PIECE:

        active_piece->positions[0] = {5, 0};
        active_piece->positions[1] = {4, 1};
        active_piece->positions[2] = {5, 1};
        active_piece->positions[3] = {6, 1};

        break;

    case Z_PIECE:

        active_piece->positions[0] = {4, 0};
        active_piece->positions[1] = {5, 0};
        active_piece->positions[2] = {5, 1};
        active_piece->positions[3] = {6, 1};

        break;

    default:
        break;
    }

    active_piece->current_rotation = NORMAL;
}

// Projects the movement of the active piece
Position *project_movement(Tetromino *active_piece, Movement_direction direction, int movement_distance_x = 1, int movement_distance_y = 1)
{

    Position *new_positions = (Position *)malloc(sizeof(Position) * 4);

    switch (direction)
    {
    case UP:
        for (int i = 0; i < 4; i++)
        {
            new_positions[i] = {active_piece->positions[i].x, active_piece->positions[i].y - 1 * movement_distance_y};
        }
        break;
    case DOWN:
        for (int i = 0; i < 4; i++)
        {
            new_positions[i] = {active_piece->positions[i].x, active_piece->positions[i].y + 1 * movement_distance_y};
        }
        break;
    case LEFT:
        for (int i = 0; i < 4; i++)
        {
            new_positions[i] = {active_piece->positions[i].x - 1 * movement_distance_x, active_piece->positions[i].y};
        }
        break;
    case RIGHT:
        for (int i = 0; i < 4; i++)
        {
            new_positions[i] = {active_piece->positions[i].x + 1 * movement_distance_x, active_piece->positions[i].y};
        }
        break;
    case TOP_RIGHT:
        for (int i = 0; i < 4; i++)
        {
            new_positions[i] = {active_piece->positions[i].x + 1 * movement_distance_x, active_piece->positions[i].y - 1 * movement_distance_y};
        }
        break;
    case TOP_LEFT:
        for (int i = 0; i < 4; i++)
        {
            new_positions[i] = {active_piece->positions[i].x - 1 * movement_distance_x, active_piece->positions[i].y - 1 * movement_distance_y};
        }
        break;
    case BOTTOM_RIGHT:
        for (int i = 0; i < 4; i++)
        {
            new_positions[i] = {active_piece->positions[i].x + 1 * movement_distance_x, active_piece->positions[i].y + 1 * movement_distance_y};
        }
        break;
    case BOTTOM_LEFT:
        for (int i = 0; i < 4; i++)
        {
            new_positions[i] = {active_piece->positions[i].x - 1 * movement_distance_x, active_piece->positions[i].y + 1 * movement_distance_y};
        }
        break;
    default:
        break;
    }

    return new_positions;
}

// Sets positions behind to empty and new ones to filled
void move_piece(Tetromino *active_piece, Cell_state board[BOARD_SIZE_X][BOARD_SIZE_Y], Movement_direction direction, int movement_distance_x = 1, int movement_distance_y = 1)
{

    Position *new_positions = project_movement(active_piece, direction, movement_distance_x, movement_distance_y);

    board[active_piece->positions[0].x][active_piece->positions[0].y] = EMPTY;
    board[active_piece->positions[1].x][active_piece->positions[1].y] = EMPTY;
    board[active_piece->positions[2].x][active_piece->positions[2].y] = EMPTY;
    board[active_piece->positions[3].x][active_piece->positions[3].y] = EMPTY;

    for (int i = 0; i < 4; i++)
    {
        active_piece->positions[i] = new_positions[i];
    }

    free(new_positions);

    board[active_piece->positions[0].x][active_piece->positions[0].y] = ACTIVE;
    board[active_piece->positions[1].x][active_piece->positions[1].y] = ACTIVE;
    board[active_piece->positions[2].x][active_piece->positions[2].y] = ACTIVE;
    board[active_piece->positions[3].x][active_piece->positions[3].y] = ACTIVE;
}

// Draws the board
void draw_board(Cell_state board[BOARD_SIZE_X][BOARD_SIZE_Y], Piece_type type)
{
    for (int x = 0; x < BOARD_SIZE_X; x++)
    {
        for (int y = 0; y < BOARD_SIZE_Y; y++)
        {
            Color cell_color;

            if (y < 4)
            {
                cell_color = SPAWN_COLOR;
            }
            else
            {
                cell_color = BOARD_COLOR;
            }

            if (board[x][y] == FILLED)
            {
                cell_color = FILLED_COLOR;
            }

            if (board[x][y] == ACTIVE)
            {
                switch (type)
                {
                case I_PIECE:
                    cell_color = SKYBLUE;
                    break;
                case J_PIECE:
                    cell_color = DARKBLUE;
                    break;
                case L_PIECE:
                    cell_color = ORANGE;
                    break;
                case O_PIECE:
                    cell_color = YELLOW;
                    break;
                case S_PIECE:
                    cell_color = GREEN;
                    break;
                case T_PIECE:
                    cell_color = PURPLE;
                    break;
                case Z_PIECE:
                    cell_color = RED;
                    break;
                default:
                    break;
                }
            }

            DrawRectangle(OFFSET_X + x * CELL_SIZE, OFFSET_Y + y * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1, cell_color);
        }
    }
}

// Sets the current rotation of the active piece
void rotate_clockwise(Tetromino *active_piece)
{
    switch (active_piece->current_rotation)
    {
    case NORMAL:
        active_piece->current_rotation = CLOCKWISE;
        break;

    case CLOCKWISE:
        active_piece->current_rotation = HALF;
        break;

    case HALF:
        active_piece->current_rotation = COUNTER_CLOCKWISE;
        break;

    case COUNTER_CLOCKWISE:
        active_piece->current_rotation = NORMAL;
        break;

    default:
        break;
    }
}

// Sets the current rotation of the active piece
void rotate_counterclockwise(Tetromino *active_piece)
{
    switch (active_piece->current_rotation)
    {
    case NORMAL:
        active_piece->current_rotation = COUNTER_CLOCKWISE;
        break;

    case CLOCKWISE:
        active_piece->current_rotation = NORMAL;
        break;

    case HALF:
        active_piece->current_rotation = CLOCKWISE;
        break;

    case COUNTER_CLOCKWISE:
        active_piece->current_rotation = HALF;
        break;

    default:
        break;
    }
}

// Sets the current rotation of the active peice
void rotate_mirror(Tetromino *active_piece)
{

    switch (active_piece->current_rotation)
    {
    case NORMAL:
        active_piece->current_rotation = HALF;
        break;

    case CLOCKWISE:
        active_piece->current_rotation = COUNTER_CLOCKWISE;
        break;

    case HALF:
        active_piece->current_rotation = NORMAL;
        break;

    case COUNTER_CLOCKWISE:
        active_piece->current_rotation = CLOCKWISE;
        break;

    default:
        break;
    }
}

// Projects the rotation
Position *project_rotation(Tetromino *active_piece, Piece_type piece_type, Rotation rotation_direction = NORMAL)
{

    Position *new_positions = (Position *)malloc(sizeof(Position) * 4);

    switch (piece_type)
    {
    case I_PIECE:

        switch (rotation_direction)
        {
        case NORMAL:

            new_positions[0] = {active_piece->positions[1].x - 2, active_piece->positions[1].y};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x - 1, active_piece->positions[1].y};
            new_positions[3] = {active_piece->positions[1].x + 1, active_piece->positions[1].y};
            break;

        case CLOCKWISE:
            new_positions[0] = {active_piece->positions[1].x, active_piece->positions[1].y - 2};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x, active_piece->positions[1].y - 1};
            new_positions[3] = {active_piece->positions[1].x, active_piece->positions[1].y + 1};
            break;

        case HALF:

            new_positions[0] = {active_piece->positions[1].x + 2, active_piece->positions[1].y};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x + 1, active_piece->positions[1].y};
            new_positions[3] = {active_piece->positions[1].x - 1, active_piece->positions[1].y};
            break;

        case COUNTER_CLOCKWISE:
            new_positions[0] = {active_piece->positions[1].x, active_piece->positions[1].y + 2};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x, active_piece->positions[1].y + 1};
            new_positions[3] = {active_piece->positions[1].x, active_piece->positions[1].y - 1};
            break;

        default:
            break;
        }

        break;

    case J_PIECE:

        switch (rotation_direction)
        {
        case NORMAL:
            new_positions[0] = {active_piece->positions[2].x - 1, active_piece->positions[2].y - 1};
            new_positions[1] = {active_piece->positions[2].x - 1, active_piece->positions[2].y};
            new_positions[2] = {active_piece->positions[2].x, active_piece->positions[2].y};
            new_positions[3] = {active_piece->positions[2].x + 1, active_piece->positions[2].y};
            break;

        case CLOCKWISE:
            new_positions[0] = {active_piece->positions[2].x + 1, active_piece->positions[2].y - 1};
            new_positions[1] = {active_piece->positions[2].x, active_piece->positions[2].y - 1};
            new_positions[2] = {active_piece->positions[2].x, active_piece->positions[2].y};
            new_positions[3] = {active_piece->positions[2].x, active_piece->positions[2].y + 1};
            break;

        case HALF:
            new_positions[0] = {active_piece->positions[2].x - 1, active_piece->positions[2].y};
            new_positions[1] = {active_piece->positions[2].x + 1, active_piece->positions[2].y};
            new_positions[2] = {active_piece->positions[2].x, active_piece->positions[2].y};
            new_positions[3] = {active_piece->positions[2].x + 1, active_piece->positions[2].y + 1};
            break;

        case COUNTER_CLOCKWISE:
            new_positions[0] = {active_piece->positions[2].x, active_piece->positions[2].y - 1};
            new_positions[1] = {active_piece->positions[2].x, active_piece->positions[2].y + 1};
            new_positions[2] = {active_piece->positions[2].x, active_piece->positions[2].y};
            new_positions[3] = {active_piece->positions[2].x - 1, active_piece->positions[2].y + 1};
            break;

        default:
            break;
        }

        break;

    case L_PIECE:

        switch (rotation_direction)
        {
        case NORMAL:
            new_positions[0] = {active_piece->positions[1].x - 1, active_piece->positions[1].y};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x + 1, active_piece->positions[1].y};
            new_positions[3] = {active_piece->positions[1].x + 1, active_piece->positions[1].y - 1};
            break;

        case CLOCKWISE:
            new_positions[0] = {active_piece->positions[1].x, active_piece->positions[1].y - 1};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x, active_piece->positions[1].y + 1};
            new_positions[3] = {active_piece->positions[1].x + 1, active_piece->positions[1].y + 1};
            break;

        case HALF:
            new_positions[0] = {active_piece->positions[1].x - 1, active_piece->positions[1].y};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x + 1, active_piece->positions[1].y};
            new_positions[3] = {active_piece->positions[1].x - 1, active_piece->positions[1].y + 1};
            break;

        case COUNTER_CLOCKWISE:
            new_positions[0] = {active_piece->positions[1].x - 1, active_piece->positions[1].y - 1};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x, active_piece->positions[1].y - 1};
            new_positions[3] = {active_piece->positions[1].x, active_piece->positions[1].y + 1};
            break;

        default:
            break;
        }

        break;

    case O_PIECE:

        switch (rotation_direction)
        {
        case NORMAL:
            new_positions[0] = {active_piece->positions[1].x + 1, active_piece->positions[1].y - 1};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x + 1, active_piece->positions[1].y};
            new_positions[3] = {active_piece->positions[1].x, active_piece->positions[1].y - 1};
            break;
        case CLOCKWISE:
            new_positions[0] = {active_piece->positions[1].x + 1, active_piece->positions[1].y};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x + 1, active_piece->positions[1].y + 1};
            new_positions[3] = {active_piece->positions[1].x, active_piece->positions[1].y + 1};
            break;

        case HALF:
            new_positions[0] = {active_piece->positions[1].x - 1, active_piece->positions[1].y};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x - 1, active_piece->positions[1].y + 1};
            new_positions[3] = {active_piece->positions[1].x, active_piece->positions[1].y + 1};
            break;

        case COUNTER_CLOCKWISE:
            new_positions[0] = {active_piece->positions[1].x - 1, active_piece->positions[1].y - 1};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x, active_piece->positions[1].y - 1};
            new_positions[3] = {active_piece->positions[1].x - 1, active_piece->positions[1].y};
            break;

        default:
            break;
        }
        break;

    case S_PIECE:

        switch (rotation_direction)
        {
        case NORMAL:
            new_positions[0] = {active_piece->positions[1].x + 1, active_piece->positions[1].y - 1};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x - 1, active_piece->positions[1].y};
            new_positions[3] = {active_piece->positions[1].x, active_piece->positions[1].y - 1};
            break;
        case CLOCKWISE:
            new_positions[0] = {active_piece->positions[1].x + 1, active_piece->positions[1].y + 1};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x, active_piece->positions[1].y - 1};
            new_positions[3] = {active_piece->positions[1].x + 1, active_piece->positions[1].y};
            break;

        case HALF:
            new_positions[0] = {active_piece->positions[1].x - 1, active_piece->positions[1].y + 1};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x + 1, active_piece->positions[1].y};
            new_positions[3] = {active_piece->positions[1].x, active_piece->positions[1].y + 1};
            break;

        case COUNTER_CLOCKWISE:
            new_positions[0] = {active_piece->positions[1].x - 1, active_piece->positions[1].y - 1};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x, active_piece->positions[1].y + 1};
            new_positions[3] = {active_piece->positions[1].x - 1, active_piece->positions[1].y};
            break;

        default:
            break;
        }
        break;

    case T_PIECE:

        switch (rotation_direction)
        {
        case NORMAL:
            new_positions[0] = {active_piece->positions[2].x, active_piece->positions[2].y - 1};
            new_positions[1] = {active_piece->positions[2].x - 1, active_piece->positions[2].y};
            new_positions[2] = {active_piece->positions[2].x, active_piece->positions[2].y};
            new_positions[3] = {active_piece->positions[2].x + 1, active_piece->positions[2].y};
            break;
        case CLOCKWISE:
            new_positions[0] = {active_piece->positions[2].x + 1, active_piece->positions[2].y};
            new_positions[1] = {active_piece->positions[2].x, active_piece->positions[2].y - 1};
            new_positions[2] = {active_piece->positions[2].x, active_piece->positions[2].y};
            new_positions[3] = {active_piece->positions[2].x, active_piece->positions[2].y + 1};
            break;
        case HALF:
            new_positions[0] = {active_piece->positions[2].x, active_piece->positions[2].y + 1};
            new_positions[1] = {active_piece->positions[2].x + 1, active_piece->positions[2].y};
            new_positions[2] = {active_piece->positions[2].x, active_piece->positions[2].y};
            new_positions[3] = {active_piece->positions[2].x - 1, active_piece->positions[2].y};
            break;
        case COUNTER_CLOCKWISE:
            new_positions[0] = {active_piece->positions[2].x - 1, active_piece->positions[2].y};
            new_positions[1] = {active_piece->positions[2].x, active_piece->positions[2].y + 1};
            new_positions[2] = {active_piece->positions[2].x, active_piece->positions[2].y};
            new_positions[3] = {active_piece->positions[2].x, active_piece->positions[2].y - 1};
            break;
        default:
            break;
        }

        break;

    case Z_PIECE:

        switch (rotation_direction)
        {
        case NORMAL:
            new_positions[0] = {active_piece->positions[1].x - 1, active_piece->positions[1].y - 1};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x, active_piece->positions[1].y - 1};
            new_positions[3] = {active_piece->positions[1].x + 1, active_piece->positions[1].y};
            break;
        case CLOCKWISE:
            new_positions[0] = {active_piece->positions[1].x + 1, active_piece->positions[1].y - 1};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x + 1, active_piece->positions[1].y};
            new_positions[3] = {active_piece->positions[1].x, active_piece->positions[1].y + 1};
            break;
        case HALF:
            new_positions[0] = {active_piece->positions[1].x - 1, active_piece->positions[1].y};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x, active_piece->positions[1].y + 1};
            new_positions[3] = {active_piece->positions[1].x + 1, active_piece->positions[1].y + 1};
            break;
        case COUNTER_CLOCKWISE:
            new_positions[0] = {active_piece->positions[1].x, active_piece->positions[1].y - 1};
            new_positions[1] = {active_piece->positions[1].x, active_piece->positions[1].y};
            new_positions[2] = {active_piece->positions[1].x - 1, active_piece->positions[1].y};
            new_positions[3] = {active_piece->positions[1].x - 1, active_piece->positions[1].y + 1};
            break;
        default:
            break;
        }
    default:
        break;
    }

    return new_positions;
}

// Sets positions behind to empty and new ones to filled
void rotate_piece(Tetromino *active_piece, Cell_state board[BOARD_SIZE_X][BOARD_SIZE_Y], Piece_type piece_type, Rotation rotation_direction = NORMAL)
{
    Position *new_positions = project_rotation(active_piece, piece_type, rotation_direction);
    board[active_piece->positions[0].x][active_piece->positions[0].y] = EMPTY;
    board[active_piece->positions[1].x][active_piece->positions[1].y] = EMPTY;
    board[active_piece->positions[2].x][active_piece->positions[2].y] = EMPTY;
    board[active_piece->positions[3].x][active_piece->positions[3].y] = EMPTY;

    for (int i = 0; i < 4; i++)
    {
        active_piece->positions[i] = new_positions[i];
    }

    free(new_positions);
    new_positions = NULL;

    board[active_piece->positions[0].x][active_piece->positions[0].y] = ACTIVE;
    board[active_piece->positions[1].x][active_piece->positions[1].y] = ACTIVE;
    board[active_piece->positions[2].x][active_piece->positions[2].y] = ACTIVE;
    board[active_piece->positions[3].x][active_piece->positions[3].y] = ACTIVE;
}

// Checks collision
bool check_collision_movement(Tetromino *active_piece, Cell_state board[BOARD_SIZE_X][BOARD_SIZE_Y], Movement_direction direction, int movement_distance_x = 1, int movement_distance_y = 1)
{
    Position *new_positions = project_movement(active_piece, direction, movement_distance_x, movement_distance_y);

    for (int i = 0; i < 4; i++)
    {
        if (new_positions[i].x < 0 || new_positions[i].x >= BOARD_SIZE_X || new_positions[i].y < 0 || new_positions[i].y >= BOARD_SIZE_Y)
        {
            return true;
        }

        if (board[new_positions[i].x][new_positions[i].y] == FILLED)
        {
            return true;
        }
    }

    return false;
}

bool check_collision_rotation(Tetromino *active_piece, Cell_state board[BOARD_SIZE_X][BOARD_SIZE_Y], Piece_type piece_type, Rotation rotation_direction = NORMAL)
{
    Position *new_positions = project_rotation(active_piece, piece_type, rotation_direction);

    for (int i = 0; i < 4; i++)
    {
        if (new_positions[i].x < 0 || new_positions[i].x >= BOARD_SIZE_X || new_positions[i].y < 0 || new_positions[i].y >= BOARD_SIZE_Y)
        {
            return true;
        }

        if (board[new_positions[i].x][new_positions[i].y] == FILLED)
        {
            return true;
        }
    }

    return false;
}

// Gets a random queue of peices
int *get_random_piece_queue()
{

    std::vector<int> v{0, 1, 2, 3, 4, 5, 6};

    std::random_device rd;

    std::mt19937 g(rd());

    std::shuffle(v.begin(), v.end(), g);

    int *shuffled_list = (int *)malloc(sizeof(int) * 7);

    for (int i = 0; i < 7; i++)
    {
        shuffled_list[i] = v[i];
    }

    return shuffled_list;
}

// Converts piece type to a char for displaying
char int_to_piece_type(int i)
{

    switch (i)
    {
    case I_PIECE:
        return 'I';
        break;
    case J_PIECE:
        return 'J';
        break;
    case L_PIECE:
        return 'L';
        break;
    case O_PIECE:
        return 'O';
        break;
    case S_PIECE:
        return 'S';
        break;
    case T_PIECE:
        return 'T';
        break;
    case Z_PIECE:
        return 'Z';
        break;
    default:
        break;
    }
}

Piece_type int_to_piece_type_enum(int i)
{
    switch (i)
    {
    case I_PIECE:
        return I_PIECE;
        break;
    case J_PIECE:
        return J_PIECE;
        break;
    case L_PIECE:
        return L_PIECE;
        break;
    case O_PIECE:
        return O_PIECE;
        break;
    case S_PIECE:
        return S_PIECE;
        break;
    case T_PIECE:
        return T_PIECE;
        break;
    case Z_PIECE:
        return Z_PIECE;
        break;
    default:
        break;
    }
}

void hard_drop(Tetromino *active_piece, Cell_state board[BOARD_SIZE_X][BOARD_SIZE_Y], Piece_type current_piece_type)
{
    while (check_collision_movement(active_piece, board, DOWN, 1) == false)
    {
        move_piece(active_piece, board, DOWN, 1);
    }
}

void clean_board(Cell_state board[BOARD_SIZE_X][BOARD_SIZE_Y])
{

    for (int x = 0; x < BOARD_SIZE_X; x++)
    {
        for (int y = 0; y < BOARD_SIZE_Y; y++)
        {
            board[x][y] = EMPTY;
        }
    }
}

int main()
{

    SetTargetFPS(60);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GEORGE TETRIS");
    InitAudioDevice();

    // GEORGE
    Cell_state board_cell_positions[BOARD_SIZE_X][BOARD_SIZE_Y];
    Tetromino active_piece;
    int current_hold_piece = -1;
    int known_queue[5];

    int queue_position = 0;
    int *piece_queue_1 = get_random_piece_queue();
    int *piece_queue_2 = get_random_piece_queue();

    bool generated_queue_1 = false;

    int whole_queue[14];

    for (int i = 0; i < 8; i++)
    {
        whole_queue[i] = piece_queue_1[i];
    }
    for (int j = 7; j < 14; j++)
    {
        whole_queue[j] = piece_queue_2[j - 7];
    }

    for (int i = 0; i < 14; i++)
    {
        std::cout << whole_queue[i] << std::endl;
    }

    int score = 0;

    Piece_type current_piece_type = int_to_piece_type_enum(piece_queue_1[0]);

    set_piece(&active_piece, current_piece_type);

    board_cell_positions[active_piece.positions[0].x][active_piece.positions[0].y] = ACTIVE;
    board_cell_positions[active_piece.positions[1].x][active_piece.positions[1].y] = ACTIVE;
    board_cell_positions[active_piece.positions[2].x][active_piece.positions[2].y] = ACTIVE;
    board_cell_positions[active_piece.positions[3].x][active_piece.positions[3].y] = ACTIVE;

    int gravity_counter = 0;

    srand(time(0));

    while (!WindowShouldClose())
    {

        BeginDrawing();

        if (gravity_counter >= GRAVITY_TICKS)
        {

            if (check_collision_movement(&active_piece, board_cell_positions, DOWN, 1) == false)
            {
                move_piece(&active_piece, board_cell_positions, DOWN, 1);
            }
            else
            {
                board_cell_positions[active_piece.positions[0].x][active_piece.positions[0].y] = FILLED;
                board_cell_positions[active_piece.positions[1].x][active_piece.positions[1].y] = FILLED;
                board_cell_positions[active_piece.positions[2].x][active_piece.positions[2].y] = FILLED;
                board_cell_positions[active_piece.positions[3].x][active_piece.positions[3].y] = FILLED;

                queue_position++;
                if (queue_position < 7)
                {
                    current_piece_type = int_to_piece_type_enum(whole_queue[queue_position]);
                }
                else
                {
                    if (queue_position < 14)
                    {

                        if (generated_queue_1 == false)
                        {
                            piece_queue_1 = get_random_piece_queue();

                            for (int i = 0; i < 7; i++)
                            {
                                whole_queue[i] = piece_queue_1[i];
                            }
                            free(piece_queue_1);
                            generated_queue_1 = true;
                        }

                        current_piece_type = int_to_piece_type_enum(whole_queue[queue_position]);
                    }
                    else
                    {
                        queue_position = 0;
                        piece_queue_2 = get_random_piece_queue();

                        for (int i = 7; i < 14; i++)
                        {
                            whole_queue[i] = piece_queue_2[i - 7];
                        }

                        current_piece_type = int_to_piece_type_enum(whole_queue[queue_position]);
                        free(piece_queue_2);
                        generated_queue_1 = false;
                    }
                }

                set_piece(&active_piece, current_piece_type);

                board_cell_positions[active_piece.positions[0].x][active_piece.positions[0].y] = ACTIVE;
                board_cell_positions[active_piece.positions[1].x][active_piece.positions[1].y] = ACTIVE;
                board_cell_positions[active_piece.positions[2].x][active_piece.positions[2].y] = ACTIVE;
                board_cell_positions[active_piece.positions[3].x][active_piece.positions[3].y] = ACTIVE;
            }

            gravity_counter = 0;
        }

        gravity_counter++;

        if (IsKeyPressed(KEY_LEFT))
        {
            if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 1) == false)
            {
                move_piece(&active_piece, board_cell_positions, LEFT, 1);
            }
        }
        else if (IsKeyPressed(KEY_RIGHT))
        {
            if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 1) == false)
            {
                move_piece(&active_piece, board_cell_positions, RIGHT, 1);
            }
        }
        else if (IsKeyPressed(KEY_DOWN))
        {
            if (check_collision_movement(&active_piece, board_cell_positions, DOWN, 1) == false)
            {
                move_piece(&active_piece, board_cell_positions, DOWN, 1);
            }
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            hard_drop(&active_piece, board_cell_positions, current_piece_type);
            board_cell_positions[active_piece.positions[0].x][active_piece.positions[0].y] = FILLED;
            board_cell_positions[active_piece.positions[1].x][active_piece.positions[1].y] = FILLED;
            board_cell_positions[active_piece.positions[2].x][active_piece.positions[2].y] = FILLED;
            board_cell_positions[active_piece.positions[3].x][active_piece.positions[3].y] = FILLED;

            queue_position++;
            if (queue_position < 7)
            {
                current_piece_type = int_to_piece_type_enum(whole_queue[queue_position]);
            }
            else
            {
                if (queue_position < 14)
                {

                    if (generated_queue_1 == false)
                    {
                        piece_queue_1 = get_random_piece_queue();

                        for (int i = 0; i < 7; i++)
                        {
                            whole_queue[i] = piece_queue_1[i];
                        }
                        free(piece_queue_1);
                        generated_queue_1 = true;
                    }

                    current_piece_type = int_to_piece_type_enum(whole_queue[queue_position]);
                }
                else
                {
                    queue_position = 0;
                    piece_queue_2 = get_random_piece_queue();

                    for (int i = 7; i < 14; i++)
                    {
                        whole_queue[i] = piece_queue_2[i - 7];
                    }

                    current_piece_type = int_to_piece_type_enum(whole_queue[queue_position]);
                    free(piece_queue_2);
                    generated_queue_1 = false;
                }
            }

            set_piece(&active_piece, current_piece_type);

            board_cell_positions[active_piece.positions[0].x][active_piece.positions[0].y] = ACTIVE;
            board_cell_positions[active_piece.positions[1].x][active_piece.positions[1].y] = ACTIVE;
            board_cell_positions[active_piece.positions[2].x][active_piece.positions[2].y] = ACTIVE;
            board_cell_positions[active_piece.positions[3].x][active_piece.positions[3].y] = ACTIVE;

            if (check_collision_movement(&active_piece, board_cell_positions, DOWN, 1, 1) == true)
            {
                clean_board(board_cell_positions);
                score = 0;
            }
        }

        if (IsKeyPressed(KEY_Z))
        {
            Rotation previous_rotation = active_piece.current_rotation;

            rotate_counterclockwise(&active_piece);
            if (check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
            {

                rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
            }
            else
            {

                switch (previous_rotation)
                {
                // De normal para counter-clockwise
                case NORMAL:

                    switch (current_piece_type)
                    {
                    case J_PIECE || L_PIECE || S_PIECE || T_PIECE || Z_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, DOWN, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, DOWN, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_RIGHT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_RIGHT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_clockwise(&active_piece);
                        }

                    case I_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_LEFT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_LEFT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_RIGHT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_RIGHT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_clockwise(&active_piece);
                        }

                    case O_PIECE:

                        if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else
                        {
                            rotate_clockwise(&active_piece);
                        }

                    default:
                        break;
                    }

                    break;
                // De conter-clockwise para Half
                case COUNTER_CLOCKWISE:

                    switch (current_piece_type)
                    {
                    case J_PIECE || L_PIECE || S_PIECE || T_PIECE || Z_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_LEFT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_LEFT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_clockwise(&active_piece);
                        }

                    case I_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_RIGHT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_RIGHT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_RIGHT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_RIGHT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_clockwise(&active_piece);
                        }

                    case O_PIECE:

                        if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else
                        {
                            rotate_clockwise(&active_piece);
                        }

                    default:
                        break;
                    }

                    break;

                // De half para clockwise
                case HALF:

                    switch (current_piece_type)
                    {
                    case J_PIECE || L_PIECE || S_PIECE || T_PIECE || Z_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, DOWN, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, DOWN, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_LEFT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_clockwise(&active_piece);
                        }

                    case I_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_RIGHT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_RIGHT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_LEFT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_LEFT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_clockwise(&active_piece);
                        }

                    case O_PIECE:

                        if (check_collision_movement(&active_piece, board_cell_positions, DOWN, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, DOWN, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else
                        {
                            rotate_clockwise(&active_piece);
                        }

                    default:
                        break;
                    }

                    break;
                // De clockwise para normal
                case CLOCKWISE:

                    switch (current_piece_type)
                    {
                    case J_PIECE || L_PIECE || S_PIECE || T_PIECE || Z_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_LEFT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_RIGHT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_clockwise(&active_piece);
                        }

                    case I_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_RIGHT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_RIGHT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_clockwise(&active_piece);
                        }

                    case O_PIECE:

                        if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else
                        {
                            rotate_clockwise(&active_piece);
                        }

                    default:
                        break;
                    }

                    break;

                default:
                    break;
                }
            }
        }
        else if (IsKeyPressed(KEY_X))
        {
            Rotation previous_rotation = active_piece.current_rotation;

            rotate_clockwise(&active_piece);
            if (check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
            {

                rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
            }
            else
            {

                switch (previous_rotation)
                {
                // De normal para clockwise
                case NORMAL:

                    switch (current_piece_type)
                    {
                    case J_PIECE || L_PIECE || S_PIECE || T_PIECE || Z_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_counterclockwise(&active_piece);
                        }

                    case I_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_LEFT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_LEFT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_RIGHT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_RIGHT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_counterclockwise(&active_piece);
                        }

                    case O_PIECE:

                        if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else
                        {
                            rotate_counterclockwise(&active_piece);
                        }

                    default:
                        break;
                    }

                    break;
                // De conter-clockwise para Half
                case COUNTER_CLOCKWISE:

                    switch (current_piece_type)
                    {
                    case J_PIECE || L_PIECE || S_PIECE || T_PIECE || Z_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_RIGHT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_RIGHT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_counterclockwise(&active_piece);
                        }

                    case I_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_LEFT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_LEFT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_RIGHT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_RIGHT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_counterclockwise(&active_piece);
                        }

                    case O_PIECE:

                        if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else
                        {
                            rotate_counterclockwise(&active_piece);
                        }

                    default:
                        break;
                    }

                    break;

                // De half para clockwise
                case HALF:

                    switch (current_piece_type)
                    {
                    case J_PIECE || L_PIECE || S_PIECE || T_PIECE || Z_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, DOWN, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, DOWN, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_RIGHT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_RIGHT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_counterclockwise(&active_piece);
                        }

                    case I_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_RIGHT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_RIGHT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_counterclockwise(&active_piece);
                        }

                    case O_PIECE:

                        if (check_collision_movement(&active_piece, board_cell_positions, DOWN, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, DOWN, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else
                        {
                            rotate_counterclockwise(&active_piece);
                        }

                    default:
                        break;
                    }

                    break;
                // De clockwise para normal
                case CLOCKWISE:

                    switch (current_piece_type)
                    {
                    case J_PIECE || L_PIECE || S_PIECE || T_PIECE || Z_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_LEFT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, UP, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, UP, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_LEFT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_RIGHT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_counterclockwise(&active_piece);
                        }

                    case I_PIECE:

                        // Teste 2
                        if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 3
                        else if (check_collision_movement(&active_piece, board_cell_positions, LEFT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, LEFT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 4
                        else if (check_collision_movement(&active_piece, board_cell_positions, BOTTOM_RIGHT, 1, 2) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, BOTTOM_RIGHT, 1, 2);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        } // Teste 5
                        else if (check_collision_movement(&active_piece, board_cell_positions, TOP_LEFT, 2, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, TOP_LEFT, 2, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else // Todos os testes falharam, volta a rotacao
                        {
                            rotate_counterclockwise(&active_piece);
                        }

                    case O_PIECE:

                        if (check_collision_movement(&active_piece, board_cell_positions, RIGHT, 1, 1) == false && check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
                        {
                            move_piece(&active_piece, board_cell_positions, RIGHT, 1, 1);
                            rotate_piece(&active_piece, board_cell_positions, current_piece_type);
                        }
                        else
                        {
                            rotate_counterclockwise(&active_piece);
                        }

                    default:
                        break;
                    }

                    break;

                default:
                    break;
                }
            }
        }
        else if (IsKeyPressed(KEY_A))
        {

            rotate_mirror(&active_piece);
            if (check_collision_rotation(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation) == false)
            {

                rotate_piece(&active_piece, board_cell_positions, current_piece_type, active_piece.current_rotation);
            }
            else
            {
                rotate_mirror(&active_piece);
            }
        }

        if (IsKeyPressed(KEY_C))
        {
            if (current_hold_piece == -1)
            {

                board_cell_positions[active_piece.positions[0].x][active_piece.positions[0].y] = EMPTY;
                board_cell_positions[active_piece.positions[1].x][active_piece.positions[1].y] = EMPTY;
                board_cell_positions[active_piece.positions[2].x][active_piece.positions[2].y] = EMPTY;
                board_cell_positions[active_piece.positions[3].x][active_piece.positions[3].y] = EMPTY;

                current_hold_piece = current_piece_type;

                queue_position++;
                current_piece_type = int_to_piece_type_enum(piece_queue_1[queue_position]);
                set_piece(&active_piece, current_piece_type);

                board_cell_positions[active_piece.positions[0].x][active_piece.positions[0].y] = ACTIVE;
                board_cell_positions[active_piece.positions[1].x][active_piece.positions[1].y] = ACTIVE;
                board_cell_positions[active_piece.positions[2].x][active_piece.positions[2].y] = ACTIVE;
                board_cell_positions[active_piece.positions[3].x][active_piece.positions[3].y] = ACTIVE;
            }
            else
            {
                board_cell_positions[active_piece.positions[0].x][active_piece.positions[0].y] = EMPTY;
                board_cell_positions[active_piece.positions[1].x][active_piece.positions[1].y] = EMPTY;
                board_cell_positions[active_piece.positions[2].x][active_piece.positions[2].y] = EMPTY;
                board_cell_positions[active_piece.positions[3].x][active_piece.positions[3].y] = EMPTY;

                int temp = current_hold_piece;
                current_hold_piece = current_piece_type;
                current_piece_type = int_to_piece_type_enum(temp);
                set_piece(&active_piece, current_piece_type);

                board_cell_positions[active_piece.positions[0].x][active_piece.positions[0].y] = ACTIVE;
                board_cell_positions[active_piece.positions[1].x][active_piece.positions[1].y] = ACTIVE;
                board_cell_positions[active_piece.positions[2].x][active_piece.positions[2].y] = ACTIVE;
                board_cell_positions[active_piece.positions[3].x][active_piece.positions[3].y] = ACTIVE;
            }
        }

        ClearBackground(BACKGROUND_COLOR);

        // Escreve rotação atual
        switch (active_piece.current_rotation)
        {
        case NORMAL:
            DrawText(TextFormat("Current rotation = NORMAL"), 30, 20, 20, LIGHTGRAY);
            break;

        case CLOCKWISE:
            DrawText(TextFormat("Current rotation = CLOCKWISE"), 30, 20, 20, LIGHTGRAY);
            break;

        case HALF:
            DrawText(TextFormat("Current rotation = HALF"), 30, 20, 20, LIGHTGRAY);
            break;
        case COUNTER_CLOCKWISE:
            DrawText(TextFormat("Current rotation = COUNTER_CLOCKWISE"), 30, 20, 20, LIGHTGRAY);
            break;

        default:
            break;
        }

        // Hold piece
        switch (current_hold_piece)
        {
        case I_PIECE:
            DrawText(TextFormat("HOLD = I"), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 400, 30, RED);
            break;
        case J_PIECE:
            DrawText(TextFormat("HOLD = J"), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 400, 30, RED);
            break;
        case L_PIECE:
            DrawText(TextFormat("HOLD = L"), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 400, 30, RED);
            break;
        case O_PIECE:
            DrawText(TextFormat("HOLD = O"), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 400, 30, RED);
            break;
        case S_PIECE:
            DrawText(TextFormat("HOLD = S"), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 400, 30, RED);
            break;
        case T_PIECE:
            DrawText(TextFormat("HOLD = T"), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 400, 30, RED);
            break;
        case Z_PIECE:
            DrawText(TextFormat("HOLD = Z"), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 400, 30, RED);
            break;
        default:
            break;
        }

        // Queue

        if (queue_position + 5 < 14)
        {
            known_queue[0] = whole_queue[queue_position + 1];
            known_queue[1] = whole_queue[queue_position + 2];
            known_queue[2] = whole_queue[queue_position + 3];
            known_queue[3] = whole_queue[queue_position + 4];
            known_queue[4] = whole_queue[queue_position + 5];
        }
        else if (queue_position + 5 == 14)
        {
            known_queue[0] = whole_queue[queue_position + 1];
            known_queue[1] = whole_queue[queue_position + 2];
            known_queue[2] = whole_queue[queue_position + 3];
            known_queue[3] = whole_queue[queue_position + 4];
            known_queue[4] = whole_queue[0];
        }
        else if (queue_position + 5 == 15)
        {
            known_queue[0] = whole_queue[queue_position + 1];
            known_queue[1] = whole_queue[queue_position + 2];
            known_queue[2] = whole_queue[queue_position + 3];
            known_queue[3] = whole_queue[0];
            known_queue[4] = whole_queue[1];
        }
        else if (queue_position + 5 == 16)
        {
            known_queue[0] = whole_queue[queue_position + 1];
            known_queue[1] = whole_queue[queue_position + 2];
            known_queue[2] = whole_queue[0];
            known_queue[3] = whole_queue[1];
            known_queue[4] = whole_queue[2];
        }
        else if (queue_position + 5 == 17)
        {
            known_queue[0] = whole_queue[queue_position + 1];
            known_queue[1] = whole_queue[0];
            known_queue[2] = whole_queue[1];
            known_queue[3] = whole_queue[2];
            known_queue[4] = whole_queue[3];
        }

        if (queue_position <= 14)
        {
            DrawText(TextFormat("QUEUE = %d", queue_position), SCREEN_WIDTH / 2 + 400, SCREEN_HEIGHT / 2 - 400, 30, RED);
            DrawText(TextFormat("CURRENT = %c", int_to_piece_type(whole_queue[queue_position])), SCREEN_WIDTH / 2 + 400, SCREEN_HEIGHT / 2 - 360, 30, RED);
            DrawText(TextFormat("QUEUE = %c %c %c %c %c", int_to_piece_type(known_queue[0]),
                                int_to_piece_type(known_queue[1]),
                                int_to_piece_type(known_queue[2]),
                                int_to_piece_type(known_queue[3]),
                                int_to_piece_type(known_queue[4])),
                     SCREEN_WIDTH / 2 + 400, SCREEN_HEIGHT / 2 - 320, 30, RED);
        }

        int cleared_lines = 0;

        // Loopa por todas as linhas
        for (int line = 0; line < BOARD_SIZE_Y; line++)
        {
            int filled_count = 0;

            // Checa se a linha esta cheia
            for (int x = 0; x < BOARD_SIZE_X; x++)
            {

                if (board_cell_positions[x][line] == FILLED)
                {
                    filled_count++;
                }
            }

            // Se estiver cheia limpa ela e puxa tudo pra baixo
            if (filled_count == BOARD_SIZE_X)
            {
                cleared_lines++;
                int cleared_line = line;

                for (int line_above = line - 1; line_above > 0; line_above--)
                {
                    for (int cell = 0; cell < BOARD_SIZE_X; cell++)
                    {

                        if (board_cell_positions[cell][line_above] != ACTIVE)
                        {
                            board_cell_positions[cell][cleared_line] = board_cell_positions[cell][line_above];
                        }
                    }
                    cleared_line = line_above;
                }
            }
        }

        switch (cleared_lines)
        {
        case 1:
            score += 100;
            break;
        case 2:
            score += 300;
            break;
        case 3:
            score += 500;
            break;
        case 4:
            score += 1000;
            break;
        default:
            break;
        }

        DrawText(TextFormat("SCORE = %d", score), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT / 2 - 375, 30, WHITE);

        draw_board(board_cell_positions, current_piece_type);
        EndDrawing();
    }

    CloseAudioDevice();

    CloseWindow();
    return 0;
}