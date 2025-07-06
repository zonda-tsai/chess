#ifndef CHESS_TOOL_BY_ZONDA
#define CHESS_TOOL_BY_ZONDA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "customise.h"

#define black 0
#define white 1

typedef enum{X, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK} Pieces;

typedef enum{
	// errors
	ILLEGAL, ILLEGAL_INPUT, ILLEGAL_ROUTE, POSITION, COLOUR, EMPTY_POSITION, BLOCKED, NOT_EN_PASSANT, ILLEGAL_PROMOTION, THREATENED_CASTLE, THREATENED_MOVE, THREATENED_KING, MOVED_KING, MOVED_ROOK,
	// movements
	NORMAL, CASTLE_K, CASTLE_Q, PROMOTION_Q, PROMOTION_R, PROMOTION_B, PROMOTION_N, EN_PASSANT, UNDO, REDO, DRAW
} Moves_and_Functions;

typedef enum{NORM, CHECK, CHECKMATE, STALEMATE} state;

typedef struct{
	char board[8][8];
	char w_die[17];
	char b_die[17];
	bool w[3];
	bool b[3];
}chess;

typedef struct{
	int x, y;
}locat;

typedef struct rec{
	Moves_and_Functions moving;
	locat from, to;
	char eaten, not[11];
	bool w[3], b[3];
	struct rec *prev, *next;
}rec;

typedef struct{
	char state; // normal = 0, selection = 1, threat = 2, 
	char from;
	char to;
}draw_state;

extern int step;

// BASIC CHESSs & LOCATEs & RECORDs
chess init();
char fetch(chess*, locat);
locat get(int, int);
void init_record(rec**, chess*, locat, locat, Moves_and_Functions);
void erase_record(rec**, bool);
void clean_record(rec**);
void push_record(rec**, chess*, locat, locat, Moves_and_Functions);

// BOOLEANs
bool isError(Moves_and_Functions);
bool isLegal_locat(locat);
bool isBlack(char);
bool isWhite(char);
bool isPawn(char);
bool isKnight(char);
bool isBishop(char);
bool isRook(char);
bool isKing(char);
bool isQueen(char);
bool isEmpty(char);
bool is_same_loc(locat, locat);

// FUNCTIONs
bool find(chess*, char, locat*);
bool push_die(chess*, char);
bool coordinate_convert(const char*, locat*);

// DRAWING
void draw_history(chess*, rec*);

// MOVEs
bool move(chess*, locat, locat);
bool eat(chess*, locat);
Moves_and_Functions castle(chess*, bool, bool);
bool threat(chess*, locat, bool);
bool isThreat_step(chess*, locat, locat, bool, Moves_and_Functions);
int available_step(rec*, chess*, bool);
Moves_and_Functions pawn(chess*, locat, locat, const rec*);
Moves_and_Functions bishop(chess*, locat, locat);
Moves_and_Functions knight(chess*, locat, locat);
Moves_and_Functions rook(chess*, locat, locat);
Moves_and_Functions queen(chess*, locat, locat);
Moves_and_Functions king(chess*, locat, locat);
bool isEnpassant(chess*, const rec*, locat, locat);
Moves_and_Functions isLegal_move(rec*, chess*, locat, locat, Moves_and_Functions, bool);
bool execute_action(chess*, locat, locat, Moves_and_Functions);
Moves_and_Functions movement(rec**, chess*, locat, locat, Moves_and_Functions, bool);
bool isChecked(chess*, bool);
bool isCheckmated(rec*, chess*, bool);
bool isStalemate(rec*, chess*, bool);


// FUNCTIONs
bool Undo(chess*, rec**);
bool Redo(chess*, rec**);

// COMMANDs
Moves_and_Functions command(const char*, locat*, locat*);

#endif
