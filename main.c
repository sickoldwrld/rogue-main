#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int py, px;                                                              // character position
int ey, ex;                                                              // entity position
int player_credits = 0;                                                  // player gold-credits-money


bool entity_placed = 0; 
bool player_placed = 0;
int  room_placed   = 0;



int dungeon(int rows, int cols,  char (*map)[cols],int player){             // generate  dungeon map

    bool collision;

    
    srand(time(NULL));

    

    if(!room_placed)    // random generate room
    {                                                                       
        int ry,rx; 
        int r_size_y, r_size_x;
        int room_num = rand() % 5 + 10;
        bool collision;                                  


        for (int yy = 0; yy <= rows; yy++){                                  // fill dungeon and borders database  

            for(int xx = 0; xx <= cols; xx++)
            {
                // border
                if(yy == 0 || yy == rows - 1  || xx == 0 || xx == cols || yy == rows){
                    map[yy][xx] = '%';
                }
                // walls
                else
                    map[yy][xx] = '#';
            
            }
        }

        while (room_placed < room_num){   
            int try_counter =0;// prototyping room until collision
            do {
                collision =0;
                try_counter++;
                // generate room coords
                ry = rand() % (rows -3);
                rx = rand() % (cols -3);
                // room size
                r_size_y = rand() % 5 + 4;
                r_size_x = rand() % 10 + 8;


                for (int yy = ry; yy <= ry + r_size_y; yy++)                             // generate room
                {
                    
                    for(int xx = rx; xx <= rx + r_size_x ; xx++){
                        
                        
                        if(map[yy][xx] == '%' || map[yy][xx] == ' ' 
                        || map[yy + 2][xx] == ' ' || map[yy - 2][xx] == ' ' 
                        || map[yy][xx+2] == ' ' || map[yy][xx-2] == ' ' )
                        {   
                            collision = 1;
                            yy = ry + r_size_y;                                           // exit upper loop
                            break;                                                        // ... exit from current loop
                        }
                    }
                }
                if (try_counter > 100){
                    ry = rx = 3;
                    r_size_y = r_size_x = 3;
                    break;

                }

            } while (collision == 1);
        


            for (int yy = ry; yy <= ry + r_size_y; yy++){                             // generate room  // fill DB
                for(int xx = rx; xx <= rx + r_size_x ; xx++){            
                    if(map[yy][xx] == '%')
                    {   
                        yy = ry + r_size_y;                                           // exit upper loop
                        break;                                                        // ... exit from current loop
                    }
                    {
                        map[yy][xx] = ' ';
                        mvaddch(yy,xx, ' ');
                    }
                }
            }
            room_placed++;
        } 
    }


  



    for (int yy = 0; yy <= rows; yy++){                                       // draw location
        for(int xx = 0; xx <= cols; xx++){
            if(yy == rows ){

                mvaddch(yy,xx, ' ');
            }
            else if(map[yy][xx] == '%'){                                           //  draw border
               
               mvaddch(yy,xx, '%');
            }
            else if (map[yy][xx] == ' '){
            
                mvaddch(yy,xx, ' ');
            }
            else
               mvaddch(yy,xx, '#');
            
        }
    }


    if(!entity_placed){ 
        do {

            ey = rand() % rows + 1;
            ex = rand() % cols + 1;
        }
        while(map[ey][ex] == '#' || map[ey][ex] == '%');
        entity_placed = 1;
    }

    if(!player_placed){ 

        do {

            py = rand() % rows;
            px = rand() % cols;
        }
        while(map[py][px] == '#' || map[py][px] == '%');
        player_placed = 1;
    }


    if      ((player == KEY_UP)      && (map[py-1][px] == ' ')) py--;        //
    else if ((player == KEY_DOWN)    && (map[py+1][px] == ' ')) py++;        //         move key character 
    else if ((player == KEY_LEFT)    && (map[py][px-1] == ' ')) px--;        //
    else if ((player == KEY_RIGHT)   && (map[py][px+1] == ' ')) px++;        // 
    



    if (py == ey && px == ex){

        entity_placed = 0;
        player_credits += rand() % 10 + 1;
        room_placed = 0;
        player_placed = 0;


    }



    mvaddch(py,px,'@'); // print-move player
    mvaddch(ey,ex,'g'); // print-move entity
    mvprintw(rows, 0, "Credits %d", player_credits);


    return 0;

}




int main(void){
    int player = 0; 																 // input
    int cols, rows;                                                                  // Variable of cols rows     
    initscr();
    keypad(stdscr,TRUE); 															 // Allow F1-F12, ARROW //
    noecho();                                                                        // Disable input 
    curs_set(0);                                                                     // Disable cursor
    getmaxyx(stdscr,rows,cols);                                                      // Set window size
    char map[rows][cols];                                                            // array map symbols



    do
    { 
        dungeon(rows -1,cols -1,map,player);
        


        
    }
    while ((player = getch()) != 27);

    


    
    getch();
    endwin();
    return 0;

}
