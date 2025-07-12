#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "customise.h"
#include "chess_tool.h"
#include "painter.h"

#ifdef _WIN32
#include <conio.h>
HANDLE hStdin_win;
DWORD dwOriginalMode;
#else
#include <termios.h>
struct termios orig_termios;
#endif

bool chess_piece = 1;

void clear(){
	printf("\x1b[2j\x1b[H");
	fflush(stdout);
}

void all_clear(){
int n;
#ifdef _WIN32
	n = system("cls");
#else
	n = system("clear");
#endif
	if(n) exit(1);
}

char get_key(){
    char c;
#ifdef _WIN32
	c = _getch();
#else
	ssize_t n = read(STDIN_FILENO, &c, 1);
	if(n <= 0) exit(1);
#endif
    return c;
}

void disableGamingMode(){
#ifdef _WIN32
	SetConsoleMode(hStdin_win, dwOriginalMode);
#else
	printf(_cursor_show);
	fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
#endif
}

void enableGamingMode(){
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
	win_term_init();
    hStdin_win = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin_win == INVALID_HANDLE_VALUE) exit(1);
    if (!GetConsoleMode(hStdin_win, &dwOriginalMode)) exit(1);
    DWORD dwNewMode = dwOriginalMode;
	dwNewMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    if (!SetConsoleMode(hStdin_win, dwNewMode)) exit(1);
#else
    // 取得當前的終端機設定
    tcgetattr(STDIN_FILENO, &orig_termios);
    // 複製一份設定，我們將在此基礎上修改
    struct termios raw = orig_termios;
    // c_lflag (local flags) 是用來控制終端機的本地模式
    // &= ~ICANON : 關閉標準模式 (canonical mode)，不再需要按 Enter 觸發
    // &= ~ECHO   : 關閉回顯 (echo)，您按下的鍵不會自動顯示在螢幕上
    raw.c_lflag &= ~(ICANON | ECHO);
    // 設定 read() 的行為
    raw.c_cc[VMIN] = 1;  // read() 至少要讀到 1 個位元組後才會返回
    raw.c_cc[VTIME] = 0; // 不設定讀取超時
    // 將修改後的新設定套用至終端機
    // TCSAFLUSH 表示在套用新設定前，會先清空終端機的輸入緩衝區
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
#endif
    // 註冊還原函式，確保程式在任何情況下退出時都會執行它
	atexit(disableGamingMode);
}

bool draw_errors(Moves_and_Functions a){
	if(!isError(a)) return 0;
	info_input(22);
	switch(a){
	case ILLEGAL_INPUT:
		printf("Invalid input...");
		break;
	case ILLEGAL:
	case ILLEGAL_ROUTE:
		printf("Invalid move...");
		break;
	case POSITION:
		printf("Invalid position...");
		break;
	case COLOUR:
		printf("You're moving your opponent's pieces...");
		break;
	case EMPTY_POSITION:
		printf("There's empty at the position...");
		break;
	case BLOCKED:
		printf("Route has been blocked...");
		break;
	case NOT_EN_PASSANT:
		printf("Invalid eat since it's not en passant...");
		break;
	case ILLEGAL_PROMOTION:
		printf("Illegal promotion...");
		break;
	case THREATENED_CASTLE:
		printf("King is threatened on the route of castling...");
		break;
	case THREATENED_MOVE:
		printf("King will be threatened if you move it...");
		break;
	case THREATENED_KING:
		printf("King is under threat...");
		break;
	case MOVED_KING:
		printf("Unable to Castle since king has moved...");
		break;
	case MOVED_ROOK:
		printf("Unable to Castle since rook has moved...");
		break;
	default:
		return 0;
	}
	fflush(stdout);
	return 1;
}

void draw_chess(char a, bool color){
	printf("  ");
	if(isBlack(a) && color)
		black_pieces;
	else if(isWhite(a) && color)
		white_pieces;
	if(chess_piece){
		if(isRook(a))
			printf(_black_rook);
		else if(isKnight(a))
			printf(_black_knight);
		else if(isBishop(a))
			printf(_black_bishop);
		else if(isQueen(a))
			printf(_black_queen);
		else if(isKing(a))
			printf(_black_king);
		else if(isPawn(a))
			printf(_black_pawn);
		else
			printf(" ");
	}
	else{
		if(isRook(a))
			printf("R");
		else if(isKnight(a))
			printf("N");
		else if(isBishop(a))
			printf("B");
		else if(isQueen(a))
			printf("Q");
		else if(isKing(a))
			printf("K");
		else if(isPawn(a))
			printf("P");
		else
			printf(" ");

	}
	printf("  ");
}

void draw_background(draw_state type, int i, int j){
	if(type.state == 0){
		if((i + j) & 1)
			bg_black;
		else
			bg_white;
		return;
	}
	bool a = type.from >= 1 && type.from <= 8 && j == (int)type.from - 1;
	bool b = type.from >= 9 && type.from <= 16 && i == (int)type.from - 9;
	bool c = type.from >= 17 && type.from <= 80 && i * 8 + j == type.from - 17;
	bool d = type.to >= 1 && type.to <= 8 && j == (int)type.to - 1;
	bool e = type.to >= 9 && type.to <= 16 && i == (int)type.to - 9;
	bool f = type.to >= 17 && type.to <= 80 && i * 8 + j == type.to - 17;
	if(!a && !b && !c && !d && !e && !f){
		if((i + j) & 1)
			bg_black;
		else
			bg_white;
	}
	else if(type.state == 1){
		if((i + j) & 1)
			selection_black;
		else
			selection_white;
	}
	else if(type.state == 2){
		if((i + j) & 1)
			threat_bg_black;
		else
			threat_bg_white;
	}
}

void draw_help(){
	info_input(1);
	printf("- setting chess position by 2 characters:");
	info_input(2);
	printf("    e.g. the location e2 needs 'e' and '2'");
	info_input(3);
	printf("  confirming by [Enter]");
	info_input(4);
	printf("- '" _yellow "C" _end "' for reset anchor point");
	info_input(5);
	printf("- '" _yellow "T" _end "' is for text mode chess");
	info_input(6);
	printf("- '" _yellow "R" _end "' for resign, '" _yellow "D" _end "' for draw");
	info_input(7);
	printf("- '" _yellow "u" _end "' for undo, '" _yellow "r" _end "' for redo");
	info_input(8);
	printf("- " _yellow "[Space]" _end " is for refreshing screen");
	info_input(9);
	printf("- '" _yellow "q" _end "' to quit the game, " _red "DON'T USE [CTRL][C]" _end);
	fflush(stdout);
}

void draw_infos(bool color, rec *current){
	fflush(stdout);
	info_input(4);
	printf("%s" "'s TURN" _end, (color) ? "\x1b[37m[WHITE]\x1b[0m" : "\x1b[90m[BLACK]\x1b[0m");
	int i, j = 6;
	rec *record = current;
	for(i = 0 ; i < 10 ; i++)
			info_input(j + i);
	if(record != NULL && record->prev != NULL){
		for(i = 0 ; i < 18 - color ; i++){
			if(record->prev->prev == NULL)
				break;
			record = record->prev;
		}
		while(record != current->next){
			info_input(j);
			printf("%-3d", (step - (i -= 2)) / 2);
			white_pieces;
			printf("\x1b[7m %-10s", record->not);
			printf(_end);
			fflush(stdout);
			record = record->next;
			if(record == current->next)
				break;
			printf("\x1b[%d;62H\x1b[K\x1b[%d;62H", j, j);
			j++;
			_RGB(stdout, 204, 204, 204);
			printf("\x1b[7m %-10s", record->not);
			printf(_end);
			record = record->next;
		}
	}
	info_input(22);
}

void draw_promotion(int type){
	int i;
	char *temp[4] = {_black_queen, _black_rook, _black_knight, _black_bishop};

	info_input(18);
	printf( _lu_rcorner _h_line _h_line _h_line _T _h_line _h_line _h_line _T _h_line _h_line _h_line _T _h_line _h_line _h_line _ru_rcorner);
	info_input(19);
	printf(_v_line);
	for(i = 0 ; i < 4 ; i++){
		if(type - 1 == i)
			printf(_yellow);
		printf(" %d ", i + 1);
		printf(_end);
		printf(_v_line);
	}
	info_input(20);
	printf(_v_line);
	for(i = 0 ; i < 4 ; i++){
		if(type - 1 == i)
			printf(_yellow);
		printf(" %s ", temp[i]);
		printf(_end);
		printf(_v_line);
	}
	info_input(21);
	printf(_ld_rcorner _h_line _h_line _h_line _rT _h_line _h_line _h_line _rT _h_line _h_line _h_line _rT _h_line _h_line _h_line _rd_rcorner);
	info_input(22);
	printf("\x1b[K");
	fflush(stdout);
}

void draw(chess temp, int a, draw_state type, rec *current){
	int i, j, i_st = a ? 0 : 7, i_en = a ? 8 : -1, delta = a ? 1 : -1;
	
	// top
	printf("  ");
	bg_black;
	if(!a)
		for(i = 0 ; i < 16 ; i++){
			if(i == 8){
				printf("\n" _end);
				printf("  ");
				bg_black;
			}
			draw_chess(temp.b_die[i], 0);
		}
	else
		for(i = 0 ; i < 16 ; i++){
			if(i == 8){
				printf("\n" _end);
				printf("  ");
				bg_black;
			}
			draw_chess(temp.w_die[i], 0);
		}
	printf(_end "\n");
	
	for(i = i_st ; i != i_en ; i += delta)
		printf("%5c", i + 'a');
	printf("\n");
	
	// body
	for(i = i_st ; i != i_en ; i += delta){
		printf("  ");
		for(j = i_st ; j != i_en ; j += delta){
			draw_background(type, i, j);
			printf("%5c", ' ');
			printf(_end);
		}
		printf("\n%d ", 8 - i);
		for(j = i_st ; j != i_en ; j += delta){
			draw_background(type, i, j);
			draw_chess(temp.board[i][j], 1);
			printf(_end);
		}
		printf("\n  ");
		for(j = i_st ; j != i_en ; j += delta){
			draw_background(type, i, j);
			printf("%5c", ' ');
			printf(_end);
		}
		printf("\n");
	}

	// bottom
	printf("\n  ");
	bg_black;
	if(!a)
		for(i = 0 ; i < 16 ; i++){
			if(i == 8){
				printf("\n" _end);
				printf("  ");
				bg_black;
			}
			draw_chess(temp.w_die[i], 0);
		}
	else
		for(i = 0 ; i < 16 ; i++){
			if(i == 8){
				printf("\n" _end);
				printf("  ");
				bg_black;
			}
			draw_chess(temp.b_die[i], 0);
		}

	
	printf(_end "\n");
	draw_infos(a, current);
	fflush(stdout);
}

Moves_and_Functions promotion(chess *temp, locat pos){
	if(!isLegal_locat(pos)) return POSITION;
	char chess = fetch(temp, pos);
	if(!isPawn(chess))
		return ILLEGAL_ROUTE;
	
	char answer = 0, c;
	draw_promotion(0);
	while((c = get_key()) != '\n' && c != '\r'){
		if(c < '1' || c > '4'){
			printf("Invalid input");
			draw_promotion(0);
			continue;
		}
		draw_promotion(c - '0');
		answer = c;
	}
	info_input(18);
	info_input(19);
	info_input(20);
	info_input(21);
	info_input(22);
	fflush(stdout);
	switch(answer){
	case '1':
		return PROMOTION_Q;
	case '2':
		return PROMOTION_R;
	case '3':
		return PROMOTION_N;
	case '4':
		return PROMOTION_B;
	}
	return ILLEGAL_PROMOTION;
}

void draw_all_history(rec *current, int n){
	if(current == NULL)
		return;
	if(n == 0){
		fflush(stdout);
		printf("\n\n[HISTORY]\n\n");
		return;
	}
	draw_all_history(current->prev, n - 1);
	if(n & 1)
		printf("%-3d", (n + 1) / 2);
	if(n & 1)
		white_pieces;
	else
		_RGB(stdout, 204, 204, 204);
	printf("\x1b[7m %-10s" _end, current->not);
	if(!(n & 1))
		printf("\n");
	if(n == 0)
		printf("\n");
	if(n == step)
		printf("\n\n");
}
