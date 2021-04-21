#include "terminal.h"

void Init_Terminal() {
    //enable ncurses to work, disable input buffering and don't echo input onto
    //terminal
    initscr();
    cbreak();
    noecho();

    //clear the terminal
    //keypad(stdscr,true);
    clear();
}

WINDOW *Init_Local_Window(WIN *win, float height, float width, float startx, float starty) {
    //Initialise the window attributes. Accessible via the window itself but they
    //are not meant to be accessed (modified mainly)
    win->width = (width) * COLS;
    win->height = (height) * LINES > 3 ? (height) * LINES : 3;

    if (startx == -1 && starty == -1) {
        win->startx = (COLS - win->width)/2;
        win->starty = (LINES - win->height)/2;
    } else {
        win->startx = COLS * startx;
        win->starty = LINES * starty;
    }

    win->border.bb = win->border.tt = '-';
    win->border.ll = win->border.rr = '|';
    win->border.tl = win->border.tr = '+';
    win->border.bl = win->border.br = '+';

    WINDOW *local_win = newwin(win->height,win->width,win->starty,win->startx);

    //Allow for detecting function key presses when getting input in the window
    keypad(local_win,true);
    return local_win;
}

void Display_Box(WINDOW *win, WIN *win_props, bool visible) {
    Border border = win_props->border;

    if (visible) {
        wborder(win, border.ll,border.rr,border.tt,border.bb,
            border.tl,border.tr,border.bl,border.br);
    }
    else {
        wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    }
    wrefresh(win);
}

void Determine_Line_No(Queue *q, WIN *win_props) {
    int curr_x = 1;
    int curr_y = 1;

    //Initialise boundries upto which characters may be printed
    int x_max = win_props->width - 1;
    int y_max = win_props->height - 1;

    //Determine line numbers
    size_t i=q->start;
    while (i<q->size) {
        if (curr_x+q->words[i].len - 1 < x_max)
            curr_x+=q->words[i].len;
        else {
            curr_y++;
            curr_x = 1 + q->words[i].len;
        }
        q->words[i].line = curr_y;
        i++;
    }
}

void Display_Text(WINDOW *win,WIN *win_props, Queue *q, int lines_done) {
    //Clear the window and show changes.
    wmove(win,1,1);
    for (size_t y=1; y<win_props->height-1; y++){
        for (size_t x=1; x<win_props->width - 1; x++)
            waddch(win, ' ');
        wmove(win,y+1,1);
    }

    //Move to start position
    wmove(win,1,1);

    //Print the words and initialise the line numbers for each word
    int curr_line = q->words[q->start].line;
    //int i = q->start;
    //while (i<q->size && q->words[i].line - lines_done != curr_line+1) i++;
    for (size_t i = q->start; i<q->size; i++) {
        //handle when switching back to text from high scores
        if (q->words[i].w[q->words[i].len - 1] == '_')
            q->words[i].w[q->words[i].len - 1] = ' ';
        if (q->words[i].line == curr_line) {
            wprintw(win,q->words[i].w);
        }
        else {
            curr_line++;
            if (curr_line >= win_props->height - 1)
                break;
            mvwprintw(win,curr_line - lines_done ,1,q->words[i].w);
        }
    }

    //Move back to start
    wmove(win,1,1);

    //Show changes
    wrefresh(win);
}

void Display_Current_Score (WINDOW* win, WIN* win_props, struct Score* score) {
    /*wclear (win);*/
    wmove(win,1,1);
    for (size_t y=1; y<win_props->height-1; y++){
        for (size_t x=1; x<win_props->width - 1; x++)
            waddch(win, ' ');
        wmove(win,y+1,1);
    }

    /* wprintw(win, "WPM = "); */
    char s[100];
    snprintf(s, 100, "WPM = %d, accuracy = %d", score->net_WPM, score->accuracy);

    wmove (win, win_props->height/2 - (1-win_props->height%2), (win_props->width - strlen(s))/2);
    /* printf("%d ", score->all_typed_entries); */
    wprintw(win, s);
    wrefresh(win);
}

//Queue's pop functionality
void Delete_Line(WINDOW *win, WIN *win_props, Queue *q, int word_no, int lines_done) {
    q->start = word_no;
    Display_Text(win,win_props,q,lines_done);
}

void Destroy_Window(WINDOW* win) {
    wclear(win);
    wrefresh(win);
    delwin(win);

}

void Exit_Terminal() {
    endwin();
}
