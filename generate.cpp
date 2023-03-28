#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pdcurses/curses.h>

#define WIDTH 80
#define HEIGHT 24
#define MAX_POLYGONS 100
#define MAX_DEPTH 10
#define MIN_POLYGONS_PER_NODE 5

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point position;
    int type;
} Cell;

typedef struct {
    Point start;
    Point end;
} Line;

typedef struct {
    Point vertices[4];
} Polygon;

typedef struct bsp_node_t {
    Polygon *polygons[MAX_POLYGONS];
    int num_polygons;
    struct bsp_node_t *front;
    struct bsp_node_t *back;
} BSPNode;

int map[HEIGHT][WIDTH];
int next_map[HEIGHT][WIDTH];
Cell cells[HEIGHT][WIDTH];
Line lines[MAX_POLYGONS];
int num_lines = 0;

BSPNode* create_bsp_node() {
    BSPNode *node = malloc(sizeof(BSPNode));
    node->num_polygons = 0;
    node->front = NULL;
    node->back = NULL;
    return node;
}

Polygon* create_polygon(Point p1, Point p2, Point p3, Point p4) {
    Polygon *polygon = malloc(sizeof(Polygon));
    polygon->vertices[0] = p1;
    polygon->vertices[1] = p2;
    polygon->vertices[2] = p3;
    polygon->vertices[3] = p4;
    return polygon;
}

void generate_cells() {
    srand(time(NULL));
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (rand() % 100 < 50) {
                cells[y][x].type = 1;
            } else {
                cells[y][x].type = 0;
            }
            cells[y][x].position.x = x;
            cells[y][x].position.y = y;
        }
    }
}

void draw_cells() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (cells[y][x].type == 1) {
                mvprintw(cells[y][x].position.y, cells[y][x].position.x, "#");
            } else {
                mvprintw(cells[y][x].position.y, cells[y][x].position.x, ".");
            }
        }
    }
}

void generate_lines() {
    for (int y = 0; y < HEIGHT - 1; y++) {
        for (int x = 0; x < WIDTH - 1; x++) {
            if (cells[y][x].type != cells[y+1][x].type) {
                lines[num_lines].start.x = x;
                lines[num_lines].start.y = y + 1;
                lines[num_lines].end.x = x + 1;
                lines[num_lines].end.y = y + 1;
                num_lines++;
            }
            if (cells[y][x].type !=     cells[y][x].type != cells[y][x+1].type) {
                lines[num_lines].start.x = x + 1;
                lines[num_lines].start.y = y;
                lines[num_lines].end.x = x + 1;
                lines[num_lines].end.y = y + 1;
                num_lines++;
            }
        }
    }
}

void draw_lines() {
    for (int i = 0; i < num_lines; i++) {
        Line line = lines[i];
        int dx = line.end.x - line.start.x;
        int dy = line.end.y - line.start.y;
        if (dx == 0) {
            for (int y = line.start.y; y < line.end.y; y++) {
                mvprintw(y, line.start.x, "#");
            }
        } else {
            for (int x = line.start.x; x < line.end.x; x++) {
                mvprintw(line.start.y, x, "#");
            }
        }
    }
}

void init_map() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            map[y][x] = cells[y][x].type;
        }
    }
}

void generate_dungeon(int depth, BSPNode *node) {
    if (depth == 0 || node->num_polygons <= MIN_POLYGONS_PER_NODE) {
        for (int y = node->polygons[0]->vertices[0].y; y <= node->polygons[0]->vertices[2].y; y++) {
            for (int x = node->polygons[0]->vertices[0].x; x <= node->polygons[0]->vertices[2].x; x++) {
                next_map[y][x] = 0;
            }
        }
        return;
    }

    BSPNode *front = create_bsp_node();
    BSPNode *back = create_bsp_node();
    node->front = front;
    node->back = back;

    int line_index = rand() % num_lines;
    Line split_line = lines[line_index];
    num_lines--;
    lines[line_index] = lines[num_lines];

    Polygon *front_polygon1 = create_polygon(
        split_line.start,
        node->polygons[0]->vertices[1],
        node->polygons[0]->vertices[2],
        split_line.end
    );
    front->polygons[front->num_polygons++] = front_polygon1;

    Polygon *back_polygon1 = create_polygon(
        node->polygons[0]->vertices[0],
        node->polygons[0]->vertices[1],
        split_line.end,
        split_line.start
    );
    back->polygons[back->num_polygons++] = back_polygon1;

    for (int i = 1; i < node->num_polygons; i++) {
        Polygon *polygon = node->polygons[i];
        if (polygon->vertices[0].y <= split_line.start.y && polygon->vertices[2].y >= split_line.end.y) {
            Polygon *front_polygon2 = create_polygon(
                polygon->vertices[0],
                polygon->vertices[1],
                polygon->vertices[2],
                polygon->vertices[3]
            );
            front->polygons[front->num_polygons++] = front_polygon2;
        } else if (polygon->vertices[2].y < split_line.start.y) {
            front->polygons[front->num_polygons++] = polygon;
        } else {
            back->polygons[back-> polygons[back->num_polygons++] = polygon;
        }
    }

    generate_dungeon(depth - 1, front);
    generate_dungeon(depth - 1, back);

    for (int y = node->polygons[0]->vertices[0].y; y <= node->polygons[0]->vertices[2].y; y++) {
        for (int x = node->polygons[0]->vertices[0].x; x <= node->polygons[0]->vertices[2].x; x++) {
            int is_inside_front = is_point_inside_polygons(create_point(x, y), front->polygons, front->num_polygons);
            int is_inside_back = is_point_inside_polygons(create_point(x, y), back->polygons, back->num_polygons);
            if (is_inside_front && is_inside_back) {
                next_map[y][x] = 1;
            } else {
                next_map[y][x] = is_inside_front;
            }
        }
    }

    destroy_bsp_node(front);
    destroy_bsp_node(back);
}

int main() {
srand(time(NULL));
init_curses();
generate_cells();
create_lines_from_cells();
create_bsp_tree();

generate_dungeon(6, bsp_tree);
init_map();

while (1) {
    draw_map();
    int key = getch();
    if (key == KEY_ENTER || key == 27) {
        break;
    }
}

end_curses();
return 0;
}