#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "chess_tool.h"
#include "painter.h"

Moves_and_Functions get_state(rec** recording, chess* temp, bool color, locat* from, locat* to, Moves_and_Functions f, bool undo, bool redo){
	char c;
	draw_state a = {0, -1, -1};
	rec *record = *recording;
	int i;
	from->x = from->y = to->x = to->y = -1;
	clear();
	draw(*temp, color, a, record);
	if(f == DRAW){
		clear();
		draw(*temp, color, a, record);
		printf("Opponent wanna draw");
		fflush(stdout);
		while((c = get_key()) != '\n' && c != '\r'){
			if(c == 'D'){
				clear();
				draw(*temp, !color, a, record);
				printf("Draw!");
				printf("\x1b[32;0H");
				fflush(stdout);
				draw_all_history(*recording, step);
				clean_record(recording);
				exit(0);
			}
			else
				return NORMAL;
		}
	}
	else if(isError(f))
		draw_errors(f);
	else if(undo){
		clear();
		draw(*temp, color, a, record);
		printf("It's the beginning of game...");
		fflush(stdout);
	}
	else if(redo){
		clear();
		draw(*temp, color, a, record);
		printf("It's the end of stack...");
		fflush(stdout);
	}
	a.state = 1;
	while((c = get_key()) != '\n' && c != '\r'){
		if(c >= 'a' && c <= 'h')
			from->y = (int)(c - 'a');
		else if(c >= '1' && c <= '8')
			from->x = 7 - (int)(c - '1');
		else if(c == 'T'){
			chess_piece = !chess_piece;
			return get_state(recording, temp, color, from, to, f, undo, redo);
		}
		else if(c == '?'){
			for(i = 0 ; i < 21 ; i++)
				info_input(i + 1);
			draw_help();
			c = getchar();
			for(i = 0 ; i < 9 ; i++)
				info_input(i + 1);
			return get_state(recording, temp, color, from, to, f, undo, redo);
		}
		else if(c == ' '){
			all_clear();
			return get_state(recording, temp, color, from, to, f, undo, redo);
		}
		else if(c == 'u')
			return UNDO;
		else if(c == 'r')
			return REDO;
		else if(c == 'q'){
			printf("\x1b[32;0H");
			fflush(stdout);
			draw_all_history(*recording, step);
			clean_record(recording);
			exit(0);
		}
		else if(c == 'R'){
			clear();
			draw(*temp, !color, a, record);
			info_input(22);
			printf("%s", (color ? "White" : "Black"));
			printf(" Resigned!");
			info_input(23);
			printf("[%s] wins!", (color ? "Black" : "White"));
			printf("\x1b[32;0H");
			fflush(stdout);
			draw_all_history(*recording, step);
			clean_record(recording);
			exit(0);
		}
		else if(c == 'D'){
			info_input(22);
			return DRAW;
		}
		else{
			clear();
			draw(*temp, color, a, record);
			printf("Invalid input!");
			fflush(stdout);
			continue;
		}
		if(from->x == -1)
			a.from = 1 + from->y;
		else if(from->y == -1)
			a.from = 9 + from->x;
		else
			a.from = 17 + from->x * 8 + from->y;
		clear();
		draw(*temp, color, a, record);
	}
	
	clear();
	draw(*temp, color, a, record);
	while((c = get_key()) != '\n' && c != '\r'){
		if(c >= 'a' && c <= 'h')
			to->y = (int)(c - 'a');
		else if(c >= '1' && c <= '8')
			to->x = 7 - (int)(c - '1');
		else if(c == 'C')
			return get_state(recording, temp, color, from, to, f, undo, redo);
		else if(c == 'u')
			return UNDO;
		else if(c == 'r')
			return REDO;
		else if(c == ' '){
			all_clear();
			return get_state(recording, temp, color, from, to, f, undo, redo);
		}
		else if(c == 'T'){
			all_clear();
			chess_piece = !chess_piece;
			return get_state(recording, temp, color, from, to, f, undo, redo);
		}
		else if(c == '?'){
			for(i = 0 ; i < 21 ; i++)
				info_input(i + 1);
			draw_help();
			c = getchar();
			for(i = 0 ; i < 9 ; i++)
				info_input(i + 1);
			return get_state(recording, temp, color, from, to, f, undo, redo);
		}
		else if(c == 'q'){
			printf("\x1b[32;0H");
			fflush(stdout);
			draw_all_history(*recording, step);
			clean_record(recording);
			exit(0);
		}
		else if(c == 'R'){
			clear();
			draw(*temp, !color, a, record);
			info_input(22);
			printf("%s", (color ? "White" : "Black"));
			printf(" Resigned!");
			info_input(23);
			printf("[%s] wins!", (color ? "Black" : "White"));
			printf("\x1b[32;0H");
			fflush(stdout);
			draw_all_history(*recording, step);
			clean_record(recording);
			exit(0);
		}
		else if(c == 'D'){
			info_input(22);
			return DRAW;
		}
		else{
			clear();
			draw(*temp, color, a, record);
			printf("Invalid input!");
			fflush(stdout);
			continue;
		}
		if(to->x == -1)
			a.to = 1 + to->y;
		else if(to->y == -1)
			a.to = 9 + to->x;
		else
			a.to = 17 + to->x * 8 + to->y;
		clear();
		draw(*temp, color, a, record);
	}

	if(isKing(fetch(temp, *from)) && (color ^ isBlack(fetch(temp, *from))) && from->y == 4 && from->x == to->x && (from->x == ((color) ? 7 : 0))){
		if(to->y == 6)
			return CASTLE_K;
		else if(to->y == 2)
			return CASTLE_Q;
	}
	return NORMAL;
}

void game(chess *a){
	rec *current = NULL;
	Moves_and_Functions type = NORMAL;
	bool color = white, promote = 0, undo = 0, redo = 0, Draw = 0;
	draw_state tmp = {0, -1, -1};
	locat pos, des;
	char piece;
	while(1){
		clear();
		
		type = get_state(&current, a, color, &pos, &des, type, undo, redo);
		if(Draw && type != DRAW){
			color = !color;
			Draw = 0;
			continue;
		}
		undo = redo = 0;
		switch(type){
		case DRAW:
			if(!Draw){
				color = !color;
				Draw = 1;
			}
			else{
				printf("Draw!");
			}
			continue;
		case UNDO:
			if(!Undo(a, &current))
				undo = 1;
			else
				color = !color;
			continue;
		case REDO:
			if(!Redo(a, &current))
				redo = 1;
			else
				color = !color;
			continue;
		case CASTLE_K:
		case CASTLE_Q:
			pos = (color) ? get(7, 4) : get(0, 4);
			des = get(((color) ? 7 : 0), ((type == CASTLE_K) ? 6 : 2));
			break;
		default:
			break;
		}
		
		piece = fetch(a, pos);
		promote = 0;
		if(isPawn(piece) && ((isWhite(piece) && pos.x == 1 && des.x == 0) || (isBlack(piece) && pos.x == 6 && des.x == 7)))
			promote = 1;
		if(promote)
			type = promotion(a, pos);

		type = movement(&current, a, pos, des, type, color);

		if(isError(type))
			continue;

		if(current != NULL && isCheckmated(current, a, !color)){
			clear();
			draw(*a, color, tmp, current);
			info_input(22);
			printf("[CHECKMATED]!");
			info_input(23);
			if(color)
				printf("White wins!");
			else
				printf("Black wins!");
			printf("\x1b[32;0H");
			fflush(stdout);
			draw_all_history(current, step);
			clean_record(&current);
			return;
		}
		else if(current != NULL && isStalemate(current, a, !color)){
			clear();
			draw(*a, color, tmp, current);
			info_input(22);
			printf("[STALEMATE]");
			printf("\x1b[32;0H");
			fflush(stdout);
			draw_all_history(current, step);
			clean_record(&current);
			return;
		}
		color = !color;
	}
}

int main(){
	enableGamingMode();
	chess test = init();
	all_clear();
	printf(_cursor_hide);
	game(&test);
	printf("\x1b[32;0H");
	return 0;
}
