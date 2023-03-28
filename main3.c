#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#define WIDTH 80
#define HEIGHT 24

typedef struct {
    int x;
    int y;
} point;

enum direction {
    NORTH,
    SOUTH,
    EAST,
    WEST
};

enum cell_state {
    WALL,
    ROOM,
    CORRIDOR
};

enum room_type {
    START_ROOM,
    END_ROOM,
    NORMAL_ROOM
};

void generate_map(int dungeon[WIDTH][HEIGHT], int min_size);
void binary_space_partitioning(int dungeon[WIDTH][HEIGHT], int x, int y, int width, int height, int min_size);
void carve_room(int dungeon[WIDTH][HEIGHT], point top_left, point bottom_right, enum room_type type);
void connect_rooms(int dungeon[WIDTH][HEIGHT], point room1, point room2);
point random_point_in_room(int dungeon[WIDTH][HEIGHT], point top_left, point bottom_right);
enum direction random_direction();
point move_point(point p, enum direction dir);
void carve_corridor(int dungeon[WIDTH][HEIGHT], point start, point end);
void display_map(int dungeon[WIDTH][HEIGHT], point player_pos);

int main() {
    srand(time(NULL));
    int dungeon[WIDTH][HEIGHT];
    generate_map(dungeon, 5);
    point player_pos = random_point_in_room(dungeon, (point){0,0}, (point){WIDTH-1,HEIGHT-1});
    initscr();
    noecho();
    curs_set(0);
    display_map(dungeon, player_pos);
    while(1) {
        int ch = getch();
        point new_pos;
        switch(ch) {
            case 'w':
                new_pos = move_point(player_pos, NORTH);
                if (dungeon[new_pos.x][new_pos.y] != WALL) {
                    player_pos = new_pos;
                }
                break;
            case 's':
                new_pos = move_point(player_pos, SOUTH);
                if (dungeon[new_pos.x][new_pos.y] != WALL) {
                    player_pos = new_pos;
                }
                break;
            case 'a':
                new_pos = move_point(player_pos, WEST);
                if (dungeon[new_pos.x][new_pos.y] != WALL) {
                    player_pos = new_pos;
                }
                break;
            case 'd':
                new_pos = move_point(player_pos, EAST);
                if (dungeon[new_pos.x][new_pos.y] != WALL) {
                    player_pos = new_pos;
                }
                break;
            case 'q':
                endwin();
                return 0;
            default:
                break;
        }
        display_map(dungeon, player_pos);
    }
}

void generate_map(int dungeon[WIDTH][HEIGHT], int min_size) {
    // Initialize entire dungeon as walls
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            dungeon[x][y] = WALL;
        }
    }
    // Carve out initial room for start location
    point start_room_top_left = (point){1, 1};
    point start_room_bottom_right = (point){10, 10};
    carve_room(dungeon, start_room_top_left, start_room_bottom_right, START_ROOM);
    // Generate rest of dungeon using binary space partitioning
    binary_space_partitioning(dungeon, 0, 0, WIDTH-1, HEIGHT-1, min_size);
    // Carve out room for end location
    point end_room_top_left = random_point_in_room(dungeon, (point){0,0}, (point){WIDTH-1,HEIGHT-1});
    point end_room_bottom_right = (point){end_room_top_left.x + 9, end_room_top_left.y + 9};
    carve_room(dungeon, end_room_top_left, end_room_bottom_right, END_ROOM);
    // Connect start and end locations
    connect_rooms(dungeon, start_room_top_left, end_room_top_left);
}
    void binary_space_partitioning(int dungeon[WIDTH][HEIGHT], int x, int y, int width, int height, int min_size) {
    if (width <= 2 * min_size || height <= 2 * min_size) {
    return;
    }
    if (width > height) {
    int split_x = x + min_size + rand() % (width - 2 * min_size);
    for (int y2 = y; y2 <= y + height; y2++) {
    dungeon[split_x][y2] = WALL;
    }
    binary_space_partitioning(dungeon, x, y, split_x - x, height, min_size);
    binary_space_partitioning(dungeon, split_x, y, x + width - split_x, height, min_size);
    } else {
    int split_y = y + min_size + rand() % (height - 2 * min_size);
    for (int x2 = x; x2 <= x + width; x2++) {
    dungeon[x2][split_y] = WALL;
    }
    binary_space_partitioning(dungeon, x, y, width, split_y - y, min_size);
    binary_space_partitioning(dungeon, x, split_y, width, y + height - split_y, min_size);
    }
    }

    void carve_room(int dungeon[WIDTH][HEIGHT], point top_left, point bottom_right, enum room_type type) {
    for (int x = top_left.x; x <= bottom_right.x; x++) {
    for (int y = top_left.y; y <= bottom_right.y; y++) {
    if (type == START_ROOM && x == top_left.x + 1 && y == top_left.y + 1) {
    // Carve out entrance to start room
    dungeon[x][y] = ROOM;
    } else if (type == END_ROOM && x == bottom_right.x - 1 && y == bottom_right.y - 1) {
    // Carve out exit from end room
    dungeon[x][y] = ROOM;
    } else if (x == top_left.x || x == bottom_right.x || y == top_left.y || y == bottom_right.y) {
    // Carve out walls around room
    dungeon[x][y] = WALL;
    } else {
    // Carve out floor of room
    dungeon[x][y] = ROOM;
    }
    }
    }
    }

    void connect_rooms(int dungeon[WIDTH][HEIGHT], point room1, point room2) {
    point current_pos = room1;
    while (current_pos.x != room2.x || current_pos.y != room2.y) {
    if (current_pos.x < room2.x) {
    current_pos.x++;
    } else if (current_pos.x > room2.x) {
    current_pos.x--;
    } else if (current_pos.y < room2.y) {
    current_pos.y++
    } else if (current_pos.y > room2.y) {
current_pos.y--;
}
dungeon[current_pos.x][current_pos.y] = ROOM;
}
}

void print_dungeon(int dungeon[WIDTH][HEIGHT], point player_pos) {
// Clear screen
clear();
// Print dungeon to screen
for (int y = 0; y < HEIGHT; y++) {
for (int x = 0; x < WIDTH; x++) {
if (x == player_pos.x && y == player_pos.y) {
// Print player character
addch('@');
} else if (dungeon[x][y] == WALL) {
// Print wall
addch('#');
} else {
// Print floor
addch('.');
}
}
addch('\n');
}
// Refresh screen
refresh();
}

int main() {
// Seed random number generator
srand(time(NULL));
// Initialize ncurses
initscr();
// Hide cursor
curs_set(FALSE);
// Generate dungeon
int dungeon[WIDTH][HEIGHT];
generate_dungeon(dungeon, 10);
// Initialize player position
point player_pos = random_point_in_room(dungeon, (point){0,0}, (point){WIDTH-1,HEIGHT-1});
// Print initial dungeon
print_dungeon(dungeon, player_pos);
// Main game loop
while (true) {
// Get user input
int input = getch();
switch (input) {
case KEY_LEFT:
if (player_pos.x > 0 && dungeon[player_pos.x-1][player_pos.y] != WALL) {
player_pos.x--;
}
break;
case KEY_RIGHT:
if (player_pos.x < WIDTH-1 && dungeon[player_pos.x+1][player_pos.y] != WALL) {
player_pos.x++;
}
break;
case KEY_UP:
if (player_pos.y > 0 && dungeon[player_pos.x][player_pos.y-1] != WALL) {
player_pos.y--;
}
break;
case KEY_DOWN:
if (player_pos.y < HEIGHT-1 && dungeon[player_pos.x][player_pos.y+1] != WALL) {
player_pos.y++;
}
break;
case 'q':
// Quit game
endwin();
return 0;
default:
break;
}
// Redraw dungeon with updated player position
print_dungeon(dungeon, player_pos);
}
return 0;
}
