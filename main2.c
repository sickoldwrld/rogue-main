#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

// Размеры карты
#define MAP_WIDTH 80
#define MAP_HEIGHT 25

// Константы для клеточного автомата
#define WALL_THRESHOLD 4
#define OPEN_THRESHOLD 3
#define GENERATIONS 4

// Константы для генерации воды
#define WATER_THRESHOLD 0.45
#define WATER_SMOOTHNESS 4
#define MAX_WATER_HEIGHT 5

// Структура для хранения ячеек карты
typedef struct {
  char type;
  int waterHeight;
} Cell;

// Функция, которая инициализирует карту
void initMap(Cell map[MAP_WIDTH][MAP_HEIGHT]) {
  int x, y;

  // Заполняем карту пустыми ячейками
  for (y = 0; y < MAP_HEIGHT; y++) {
    for (x = 0; x < MAP_WIDTH; x++) {
      map[x][y].type = ' ';
      map[x][y].waterHeight = 0;
    }
  }
}

// Функция, которая выводит карту на экран
void drawMap(Cell map[MAP_WIDTH][MAP_HEIGHT]) {
  int x, y;

  // Очищаем экран перед отрисовкой карты
  clear();

  // Отрисовываем карту
  for (y = 0; y < MAP_HEIGHT; y++) {
    for (x = 0; x < MAP_WIDTH; x++) {
      // Если ячейка имеет высоту воды, то рисуем ее синим цветом
      if (map[x][y].waterHeight > 0) {
        attron(COLOR_PAIR(1));
        mvprintw(y, x, "%c", '~');
        attroff(COLOR_PAIR(1));
      } else {
        // Если ячейка является стеной, то рисуем ее белым цветом
        if (map[x][y].type == '#') {
          attron(COLOR_PAIR(2));
          mvprintw(y, x, "%c", '#');
          attroff(COLOR_PAIR(2));
        } else {
          // Иначе рисуем открытое пространств
          mvprintw(y, x, "%c", '.');
    }
  }
}
}

// Обновляем экран
refresh();
}

// Функция, которая считает количество стен вокруг ячейки
int countWalls(Cell map[MAP_WIDTH][MAP_HEIGHT], int x, int y) {
int count = 0;
int dx, dy;

// Проходим по всем соседним ячейкам и считаем количество стен
for (dy = -1; dy <= 1; dy++) {
for (dx = -1; dx <= 1; dx++) {
// Если это текущая ячейка, то пропускаем ее
if (dx == 0 && dy == 0) {
continue;
}
 // Если ячейка находится за границей карты, то пропускаем ее
  if (x + dx < 0 || x + dx >= MAP_WIDTH || y + dy < 0 || y + dy >= MAP_HEIGHT) {
    continue;
  }

  // Если это стена, то увеличиваем счетчик
  if (map[x + dx][y + dy].type == '#') {
    count++;
  }
}
}

return count;
}
// Функция, которая выполняет один шаг клеточного автомата
void performCAStep(Cell map[MAP_WIDTH][MAP_HEIGHT]) {
int x, y;
int wallCount;

// Создаем временную карту, чтобы избежать ошибок в вычислениях
Cell tempMap[MAP_WIDTH][MAP_HEIGHT];

// Заполняем временную карту пустыми ячейками
for (y = 0; y < MAP_HEIGHT; y++) {
for (x = 0; x < MAP_WIDTH; x++) {
tempMap[x][y].type = ' ';
tempMap[x][y].waterHeight = 0;
}
}

// Проходим по всем ячейкам карты и применяем правила клеточного автомата
for (y = 0; y < MAP_HEIGHT; y++) {
for (x = 0; x < MAP_WIDTH; x++) {
wallCount = countWalls(map, x, y);
 if (map[x][y].type == '#') {
    if (wallCount >= WALL_THRESHOLD) {
      tempMap[x][y].type = '#';
    } else {
      tempMap[x][y].type = ' ';
    }
  } else {
    if (wallCount >= OPEN_THRESHOLD) {
      tempMap[x][y].type = '#';
    } else {
      tempMap[x][y].type = ' ';
    }
  }
}
}

// Копируем данные из временной карты в основную карту
for (y = 0; y < MAP_HEIGHT; y++) {
for (x = 0; x < MAP_WIDTH; x++) {
map[x][y].type = tempMap[x][y].type;
}
}
}

// Функция, которая выполняет генерацию карты с использованием бинарного пространственного разбиения
void generateMap(Cell map[MAP_WIDTH][MAP_HEIGHT]) {
int i, x, y
int width, height;
int centerX, centerY;
int maxDim;

// Заполняем карту пустыми ячейками
for (y = 0; y < MAP_HEIGHT; y++) {
for (x = 0; x < MAP_WIDTH; x++) {
map[x][y].type = ' ';
map[x][y].waterHeight = 0;
}
}

// Создаем корневую ячейку
width = MAP_WIDTH - 2;
height = MAP_HEIGHT - 2;
centerX = MAP_WIDTH / 2;
centerY = MAP_HEIGHT / 2;
maxDim = (width > height) ? width : height;

// Создаем корневую ячейку
map[centerX][centerY].type = '#';

// Создаем дочерние ячейки с помощью рекурсивного алгоритма
splitRoom(map, centerX, centerY, maxDim);

// Применяем клеточный автомат для удаления избыточных стен
for (i = 0; i < CA_STEPS; i++) {
performCAStep(map);
}

// Добавляем воду на карту с помощью фрактального генератора
generateWater(map);

// Расставляем предметы на карте
placeItems(map);
}

// Основная функция программы
int main() {
Cell map[MAP_WIDTH][MAP_HEIGHT];

// Инициализируем библиотеку curses
initscr();
noecho();
curs_set(FALSE);

// Генерируем карту
generateMap(map);

// Отображаем карту на экране
drawMap(map);

// Ожидаем, пока пользователь не нажмет клавишу
getch();

// Выходим из библиотеки curses
endwin();

return 0;
}