int dungeon(int rows, int cols){   // generate  dungeon map
        for (int yy = 0; yy <= rows; yy++)
        {
            for(int xx = 0; xx <= cols; xx++){

                map[yy][xx] = '#';
                mvaddch(yy,xx, '#');
            }
        }


        for (int yy = 11; yy <= rows / 2; yy++)
        {
            for(int xx = 11; xx <= cols / 2 ; xx++){
                
                map[yy][xx] = ' ';
                mvaddch(yy,xx, ' ');
            }
        }
        return 0;

}
