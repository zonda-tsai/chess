#ifndef TERMINAL_PAINTER_FROM_ZONDA
#define TERMINAL_PAINTER_FROM_ZONDA

#include <stdio.h>
#include <stdbool.h>
#include "chess_tool.h"

#define bg_black _b_RGB(stdout, 59, 61, 94)
#define bg_white _b_RGB(stdout, 148, 150, 181)
#define selection_black _b_RGB(stdout, 217, 188, 108)
#define selection_white _b_RGB(stdout, 249, 213, 120)
#define threat_bg_black _b_RGB(stdout, 125, 62, 62)
#define threat_bg_white _b_RGB(stdout, 214, 71, 71)
#define white_pieces _RGB(stdout, 255, 255, 255)
#define black_pieces _RGB(stdout, 0, 0, 0)
#define info_input(x) printf("\x1b[%d;48H\x1b[K\x1b[%d;48H", x, x)

extern struct termios orig_termios;
extern bool chess_piece;

void clear();
void all_clear();
char get_key();
void disableGamingMode();
void enableGamingMode();
void draw_pieces(char);
bool draw_errors(Moves_and_Functions);
void draw_chess(char, bool);
void draw_background(draw_state, int, int);
void draw_infos(bool, rec*);
void draw_help();
void draw_promotion(int);
void draw(chess, int, draw_state, rec*);
Moves_and_Functions promotion(chess*, locat);
void draw_all_history(rec*, int);

#endif
