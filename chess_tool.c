#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "chess_tool.h"

int step = 0;

// BASIC CHESSs & LOCATEs & RECORDs
chess init(){
	chess a = {0};
	int i;
	char temp[2][8] = {{bR, bN, bB, bQ, bK, bB, bN, bR},
					   {wR, wN, wB, wQ, wK, wB, wN, wR}};
	for(i = 0 ; i < 8 ; i++){
		a.board[0][i] = temp[0][i];
		a.board[7][i] = temp[1][i];
		a.board[1][i] = bP;
		a.board[6][i] = wP;
		a.board[2][i] = a.board[3][i] = a.board[4][i] = a.board[5][i] = X;
	}
	memset(a.w_die, X, 17);
	memset(a.b_die, X, 17);

	a.w[0] = a.w[1] = a.w[2] = 0;
	a.b[0] = a.b[1] = a.b[2] = 0;
	return a;
}

char fetch(chess *temp, locat p){
	return temp->board[p.x][p.y];
}

locat get(int x, int y){
	locat temp = {x, y};
	return temp;
}

bool isLegal_locat(locat temp){
	return (temp.x >= 0 && temp.x <= 7 && temp.y >= 0 && temp.y <= 7);
}

void init_record(rec **current, chess *temp, locat pos, locat des, Moves_and_Functions type){
	*current = malloc(sizeof(rec));
	(*current)->prev = NULL;
	rec *new = NULL;
	new = malloc(sizeof(rec));
	new->prev = *current;
	new->from = pos;
	new->to = des;
	new->next = NULL;
	new->moving = type;
	new->eaten = fetch(temp, des);
	(*current)->next = new;
	*current = new;
	step++;
}

void erase_record(rec **loc, bool dir){
	if(*loc == NULL)
		return;
	if(!dir && (*loc)->prev != NULL)
		erase_record(&((*loc)->prev), dir);
	if(dir && (*loc)->next != NULL)
		erase_record(&((*loc)->next), dir);
	free(*loc);
	*loc = NULL;
}

void clean_record(rec **current){
	if(*current == NULL)
		return;
	if((*current)->prev != NULL)
		erase_record(&((*current)->prev), 0);
	if((*current)->next != NULL)
		erase_record(&((*current)->next), 1);
	free(*current);
	*current = NULL;
}

void push_record(rec **current, chess *temp, locat pos, locat des, Moves_and_Functions type){
	if(!isLegal_locat(pos) || !isLegal_locat(des)) return;
	if(*current == NULL){
		init_record(current, temp, pos, des, type);
		return;
	}
	rec *new;
	if((*current)->next != NULL)
		erase_record(&((*current)->next), 1);

	new = malloc(sizeof(rec));

	memcpy(new->w, temp->w, sizeof(temp->w));
	memcpy(new->b, temp->b, sizeof(temp->b));
	
	new->prev = *current;
	new->from = pos;
	new->to = des;
	new->next = NULL;
	new->moving = type;
	new->eaten = fetch(temp, des);
	(*current)->next = new;
	*current = new;
	step++;
}

// BOOLEANs
bool isError(Moves_and_Functions a){
	return a == ILLEGAL || a == ILLEGAL_INPUT || a == ILLEGAL_ROUTE || a == POSITION || a == COLOUR || a == EMPTY_POSITION || a == BLOCKED || a == NOT_EN_PASSANT || a == ILLEGAL_PROMOTION || a == THREATENED_CASTLE	|| a == THREATENED_MOVE || a == THREATENED_KING || a == MOVED_KING || a == MOVED_ROOK;
}

bool isBlack(char a){
	return (a == bR || a == bN || a == bB || a == bQ || a == bK || a == bP);
}

bool isWhite(char a){
	return (a == wR || a == wN || a == wB || a == wQ || a == wK || a == wP);
}

bool isPawn(char a){
	return a == bP || a == wP;
}

bool isKnight(char a){
	return a == bN || a == wN;
}

bool isBishop(char a){
	return a == bB || a == wB;
}

bool isRook(char a){
	return a == bR || a == wR;
}

bool isKing(char a){
	return a == bK || a == wK;
}

bool isQueen(char a){
	return a == bQ || a == wQ;
}

bool isEmpty(char a){
	return a == X;
}

bool is_same_loc(locat a, locat b){
	return (a.x == b.x && a.y == b.y);
}


// FUNCTIONs
bool push_die(chess *temp, char chess){
	if(!isWhite(chess) && !isBlack(chess)) return 0;
	int i, n = isWhite(chess);
	if(n){
		for(i = 0 ; i < 16 && !isEmpty(temp->w_die[i]) ; i++);
		temp->w_die[i] = chess;
	}
	else{
		for(i = 0 ; i < 16 && !isEmpty(temp->b_die[i]); i++);
		temp->b_die[i] = chess;
	}
	return 1;
}

bool find(chess *temp, char piece, locat *loc){
	int i, j;
	for(i = 0 ; i < 8 ; i++)
		for(j = 0 ; j < 8 ; j++)
			if(temp->board[i][j] == piece){
				*loc = get(i, j);
				return 1;
			}
	return 0;
}

bool coordinate_convert(const char* a, locat *l){
	l->x = 7 - (int)(a[1] - '1');
	l->y = (int)(a[0] - 'a');
	if(l->x < 0 || l->x > 7 || l->y < 0 || l->y > 7)
		return 0;
	return 1;
}

// DRAWING
void draw_history(chess *temp, rec *current){
	if(current->moving == CASTLE_K){
		strcpy(current->not, "O-O");
		return;
	}
	if(current->moving == CASTLE_Q){
		strcpy(current->not, "O-O-O");
		return;
	}
	strcpy(current->not, "");
	char piece = fetch(temp, current->to), location[3] = {0};
	bool colour = isWhite(piece);
	int i, j, k, l, x = current->to.x, y = current->to.y;
	bool a = 0, b = 0, c = 0, eat = !isEmpty(current->eaten), check = isChecked(temp, !colour), checkmate = isCheckmated(current, temp, !colour);

	switch(current->moving){
	case PROMOTION_Q:
	case PROMOTION_R:
	case PROMOTION_B:
	case PROMOTION_N:
	case EN_PASSANT:
		if(eat)
			a = 1;
		break;
	case NORMAL:
		if(isKing(piece)){
			strcat(current->not, "K");
			break;
		}
		else if(isPawn(piece)){
			if(eat)
				a = 1;
			break;
		}
		else if(isQueen(piece)){
			strcat(current->not, "Q");
			for(i = -1 ; i <= 1 ; i += 2)
				for(j = -1 ; j <= 1 ; j += 2)
					for(k = x + i, l = y + j ; isLegal_locat(get(k, l)) ; k += i, l += j)
						if(!isEmpty(fetch(temp, get(k, l)))){
							if(fetch(temp, get(k, l)) == piece){
								if(k == current->from.x) a = 1;
								else if(l == current->from.y) b = 1;
								else c = 1;
							}
							break;
						}
			for(i = -1 ; i <= 1 ; i += 2){
				for(j = x + i ; j >= 0 && j <= 7 ; j += i)
					if(!isEmpty(fetch(temp, get(j, y)))){
						if(fetch(temp, get(j, y)) == piece){
							if(j == current->from.x) a = 1;
							else if(y == current->from.y) b = 1;
						}
						break;
					}
				for(j = y + i ; j >= 0 && j <= 7 && !a ; j += i)
					if(!isEmpty(fetch(temp, get(x, j)))){
						if(fetch(temp, get(x, j)) == piece){
							if(x == current->from.x) a = 1;
							else if(j == current->from.y) b = 1;
						}
						break;
					}
			}
		}
		else if(isBishop(piece)){
			strcat(current->not, "B");
			for(i = -1 ; i <= 1 ; i += 2)
				for(j = -1 ; j <= 1 ; j += 2)
					for(k = x + i, l = y + j ; isLegal_locat(get(k, l)) ; k += i, l += j)
						if(!isEmpty(fetch(temp, get(k, l)))){
							if(fetch(temp, get(k, l)) == piece){
								if(k == current->from.x) a = 1;
								else if(l == current->from.y) b = 1;
								else c = 1;
							}
							break;
						}
		}
		else if(isRook(piece)){
			strcat(current->not, "R");
			for(i = -1 ; i <= 1 ; i += 2){
				for(j = x + i ; j >= 0 && j <= 7 ; j += i)
					if(!isEmpty(fetch(temp, get(j, y)))){
						if(fetch(temp, get(j, y)) == piece){
							if(j == current->from.x) a = 1;
							else if(y == current->from.y) b = 1;
						}
						break;
					}
				for(j = y + i ; j >= 0 && j <= 7 ; j += i)
					if(!isEmpty(fetch(temp, get(x, j)))){
						if(fetch(temp, get(x, j)) == piece){
							if(x == current->from.x) a = 1;
							else if(j == current->from.y) b = 1;
						}
						break;
					}
			}
		}
		else if(isKnight(piece)){
			strcat(current->not, "N");
			for(i = -2 ; i <= 2 ; i++)
				for(j = -2 ; j <= 2 ; j++){
					if(!isLegal_locat(get(x + i, y + j)) || i * i + j * j != 5 || isEmpty(fetch(temp, get(x + i, y + j))))
						continue;
					if(fetch(temp, get(x + i, y + j)) == piece){
						if(x + i == current->from.x) a = 1;
						if(y + j == current->from.y) b = 1;
						else c = 1;
					}
				}
		}
	default:
		break;
	}
	
	x = current->from.x;
	y = current->from.y;
	
	if(!a && b)
		location[0] = '1' + (7 - x);
	else if((a && !b) || (!a && !b && c))
		location[0] = 'a' + y;
	else if(a && b){
		location[0] = 'a' + y;
		location[1] = '1' + (7 - x);
	}
	
	strcat(current->not, location);
	
	if(eat)
		strcat(current->not, "x");
	sprintf(location, "%c%c", current->to.y + 'a', (7 - current->to.x) + '1');
	strcat(current->not, location);
	switch(current->moving){
	case PROMOTION_Q:
		strcat(current->not, "=Q");
		break;
	case PROMOTION_R:
		strcat(current->not, "=R");
		break;
	case PROMOTION_B:
		strcat(current->not, "=B");
		break;
	case PROMOTION_N:
		strcat(current->not, "=N");
	default:
		break;
	}
	if(!checkmate && check)
		strcat(current->not, "+");
	if(checkmate)
		strcat(current->not, "#");
}

// MOVES
bool move(chess *temp, locat pos, locat des){
	if(isEmpty(fetch(temp, pos)) || is_same_loc(pos, des)) return 0;
	if(isEmpty(fetch(temp, des))){
		temp->board[des.x][des.y] = temp->board[pos.x][pos.y];
		temp->board[pos.x][pos.y] = X;
		return 1;
	}
	return 0;
}

bool eat(chess *temp, locat pos){
	if(isEmpty(fetch(temp, pos))) return 0;
	push_die(temp, temp->board[pos.x][pos.y]);
	temp->board[pos.x][pos.y] = X;
	return 1;
}

Moves_and_Functions castle(chess *temp, bool color, bool type){
	if((color && temp->w[1]) || (!color && temp->b[1])) return MOVED_KING;
	if(!isKing(fetch(temp, get((color ? 7 : 0), 4)))) return MOVED_KING;
	if(threat(temp, get((color ? 7 : 0), 4), color))
		return THREATENED_KING;
	if((color && (type * temp->w[2] + !type * temp->w[0])) || (!color && (type * temp->b[2] + !type * temp->b[0])))
		return MOVED_ROOK;
	int i;
	locat p[10] = {{7, 5}, {7, 6},
				   {7, 1}, {7, 2}, {7, 3},
				   {0, 5}, {0, 6},
				   {0, 1}, {0, 2}, {0, 3}};
	if(!type && color && !temp->w[1] && !temp->w[2] && isEmpty(fetch(temp, p[0])) && isEmpty(fetch(temp, p[1]))){
		for(i = 4 ; i <= 6 ; i++)
			if(threat(temp, get(7, i), color))
				return THREATENED_CASTLE;
		return CASTLE_K;
	}
	else if(type && color && !temp->w[0] && !temp->w[1] && isEmpty(fetch(temp, p[2])) && isEmpty(fetch(temp, p[3])) && isEmpty(fetch(temp, p[4]))){
		for(i = 2 ; i <= 4 ; i++)
			if(threat(temp, get(7, i), color))
				return THREATENED_CASTLE;
		return CASTLE_Q;
	}
	else if(!type && !color && !temp->b[1] && !temp->b[2] && isEmpty(fetch(temp, p[5])) && isEmpty(fetch(temp, p[6]))){
		for(i = 4 ; i <= 6 ; i++)
			if(threat(temp, get(0, i), color))
				return THREATENED_CASTLE;
		return CASTLE_K;
	}
	else if(type && !color && !temp->b[0] && !temp->b[1] && isEmpty(fetch(temp, p[7])) && isEmpty(fetch(temp, p[8])) && isEmpty(fetch(temp, p[9]))){
		for(i = 2 ; i <= 4 ; i++)
			if(threat(temp, get(0, i), color))
				return THREATENED_CASTLE;
		return CASTLE_Q;
	}
	return BLOCKED;
}

bool threat(chess *temp, locat pos, bool color){
	int i, j, k, l;
	char piece;

	// pawn
	if(color){
		locat pawn1 = get(pos.x - 1, pos.y - 1), pawn2 = get(pos.x - 1, pos.y + 1);
		if(isLegal_locat(pawn1) && isBlack(fetch(temp, pawn1)) && isPawn(fetch(temp, pawn1)))
			return 1;
		if(isLegal_locat(pawn2) && isBlack(fetch(temp, pawn2)) && isPawn(fetch(temp, pawn2)))
			return 1;
	}
	else{
		locat pawn1 = get(pos.x + 1, pos.y - 1), pawn2 = get(pos.x + 1, pos.y + 1);
		if(isLegal_locat(pawn1) && isWhite(fetch(temp, pawn1)) && isPawn(fetch(temp, pawn1)))
			return 1;
		if(isLegal_locat(pawn2) && isWhite(fetch(temp, pawn2)) && isPawn(fetch(temp, pawn2)))
			return 1;
	}

	// King
	for(i = -1 ; i <= 1 ; i++)
		for(j = -1 ; j <= 1 ; j++)
			if(isLegal_locat(get(pos.x + i, pos.y + j))){
				piece = fetch(temp, get(pos.x + i, pos.y + j));
				if(!isEmpty(piece) && (color ^ isWhite(piece)) && isKing(piece))
					return 1;
			}

	// Knight
	for(i = -2 ; i <= 2 ; i++)
		for(j = -2 ; j <= 2 ; j++)
			if(i * i + j * j == 5 && isLegal_locat(get(pos.x + i, pos.y + j))){
				piece = fetch(temp, get(pos.x + i, pos.y + j));
				if(!isEmpty(piece) && (color ^ isWhite(piece)) && isKnight(piece))
					return 1;
			}
	
	// x-way (Queen or Bishop)
	for(k = -1 ; k <= 1 ; k += 2)
		for(l = -1 ; l <= 1 ; l += 2)
			for(i = pos.x + k, j = pos.y + l ; isLegal_locat(get(i, j)) ; i += k, j += l){
				piece = fetch(temp, get(i, j));
				if(isEmpty(piece))
					continue;
				if(color ^ isBlack(piece))
					break;
				if((color ^ isWhite(piece)) && (isBishop(piece) || isQueen(piece)))
					return 1;
				break;
			}

	// +-way (Queen or Rook)
	for(i = -1 ; i <= 1 ; i += 2){
		for(j = pos.x + i ; j >= 0 && j <= 7 ; j += i){
			piece = fetch(temp, get(j, pos.y));
			if(isEmpty(piece))
				continue;
			if(color ^ isBlack(piece))
				break;
			if((color ^ isWhite(piece)) && (isRook(piece) || isQueen(piece)))
				return 1;
			else break;
		}
		for(j = pos.y + i ; j >= 0 && j <= 7 ; j += i){
			piece = fetch(temp, get(pos.x, j));
			if(isEmpty(piece))
				continue;
			if(color ^ isBlack(piece))
				break;
			if((color ^ isWhite(piece)) && (isRook(piece) || isQueen(piece)))
				return 1;
			else break;
		}
	}
	return 0;
}

bool isThreat_step(chess *temp, locat pos, locat des, bool color, Moves_and_Functions a){
	locat k;
	chess tmp = *temp;
	if(!find(temp, color * wK + !color * bK, &k)) return 0;
	execute_action(&tmp, pos, des, a);
	if(!find(&tmp, color * wK + !color * bK, &k)) return 1;
	return threat(&tmp, k, color);
}

int available_step(rec *current, chess *temp, bool color){
	locat pos, des;
	char piece;
	int i, j, k, l, n = 0;
	for(pos.x = 0 ; pos.x < 8 ; pos.x++){
		for(pos.y = 0 ; pos.y < 8 ; pos.y++){
			piece = fetch(temp, pos);
			if(!(color ^ isBlack(piece))) continue;
			if(isPawn(piece)){
				
				des = get(pos.x + 2 * isBlack(piece) - 1, pos.y);
				if(isLegal_locat(des) && !isError(pawn(temp, pos, des, current)))
					n += !isThreat_step(temp, pos, des, color, NORMAL);
				
				des = get(pos.x + 4 * isBlack(piece) - 2, pos.y);
				if(isLegal_locat(des) && !isError(pawn(temp, pos, des, current)))
					n += !isThreat_step(temp, pos, des, color, NORMAL);

				des = get(pos.x + 2 * isBlack(piece) - 1, pos.y - 1);
				if(isLegal_locat(des) && !isError(pawn(temp, pos, des, current)))
					n += !isThreat_step(temp, pos, des, color, NORMAL);

				des = get(pos.x + 2 * isBlack(piece) - 1, pos.y + 1);
				if(isLegal_locat(des) && !isError(pawn(temp, pos, des, current)))
					n += !isThreat_step(temp, pos, des, color, NORMAL);
			}
			else if(isBishop(piece)){
				for(i = -1 ; i <= 1 ; i += 2)
					for(j = -1 ; j <= 1 ; j += 2)
						for(k = pos.x, l = pos.y ; isLegal_locat(get(k, l)) ; k += i, l += j)
							if(!isError(bishop(temp, pos, get(k, l))))
								n += !isThreat_step(temp, pos, get(k, l), color, NORMAL);
			}
			else if(isKnight(piece)){
				for(i = -2 ; i <= 2 ; i++)
					for(j = -2 ; j <= 2; j++){
						if(i * i + j * j != 5) continue;
						des = get(pos.x + i, pos.y + j);
						if(!isError(knight(temp, pos, des)))
							n += !isThreat_step(temp, pos, des, color, NORMAL);
					}
			}
			else if(isRook(piece)){
				for(i = 0 ; i < 8 ; i++){
					if(!isError(rook(temp, pos, get(i, pos.y))))
						n += !isThreat_step(temp, pos, get(i, pos.y), color, NORMAL);
					if(!isError(rook(temp, pos, get(pos.x, i))))
						n += !isThreat_step(temp, pos, get(pos.x, i), color, NORMAL);
				}
			}
			else if(isQueen(piece)){
				for(i = -1 ; i <= 1 ; i += 2)
					for(j = -1 ; j <= 1 ; j += 2)
						for(k = pos.x, l = pos.y ; isLegal_locat(get(k, l)) ; k += i, l += j)
							if(!isError(queen(temp, pos, get(k, l))))
								n += !isThreat_step(temp, pos, get(k, l), color, NORMAL);
				for(i = 0 ; i < 8 ; i++){
					if(!isError(queen(temp, pos, get(i, pos.y))))
						n += !isThreat_step(temp, pos, get(i, pos.y), color, NORMAL);
					if(!isError(queen(temp, pos, get(pos.x, i))))
						n += !isThreat_step(temp, pos, get(pos.x, i), color, NORMAL);
				}
			}
			else if(isKing(piece)){
				for(i = -1 ; i <= 1 ; i++)
					for(j = -1; j <= 1 ; j++)
						if(isLegal_locat(get(pos.x + i, pos.y + j)) && !isError(king(temp, pos, get(pos.x + i, pos.y + j))))
							n += !isThreat_step(temp, pos, get(pos.x + i, pos.y + j), color, NORMAL);
            }
        }
    }
	return n;
}

Moves_and_Functions pawn(chess *temp, locat pos, locat des, const rec* current){
	if(!isLegal_locat(pos) || !isLegal_locat(des) || is_same_loc(pos, des)) return POSITION;
	char piece = fetch(temp, pos);
	char enemy = fetch(temp, des);
		
	if(!isPawn(piece)) return EMPTY_POSITION;
	
	int delta = isWhite(piece) * -1 + isBlack(piece);
	int initial_position = isWhite(piece) * 6 + isBlack(piece) * 1;
	bool dif = isBlack(piece) ^ isBlack(enemy) && !isEmpty(enemy);

	// en passant
	if(current != NULL && isEnpassant(temp, current, pos, des))
		return EN_PASSANT;
	
	// eat
	if(des.x == pos.x + delta && (des.y - pos.y) * (des.y - pos.y) == 1 && dif)
		return NORMAL;
	
	// move
	if(pos.y == des.y && (des.x - pos.x) * delta > 0){
		switch((des.x - pos.x) * (des.x - pos.x)){
		case 1:
			if(!isEmpty(fetch(temp, des)))
				return BLOCKED;
			return NORMAL;
		case 4:
			if(!isEmpty(fetch(temp, des)) || !isEmpty(fetch(temp, get((pos.x + des.x) / 2, pos.y))))
				return BLOCKED;
			if(des.x - pos.x == 2 * delta && pos.x == initial_position)
				return NORMAL;
		}
	}
	return ILLEGAL_ROUTE;
}

Moves_and_Functions bishop(chess *temp, locat pos, locat des){
	if(!isLegal_locat(pos) || !isLegal_locat(des) || !isBishop(fetch(temp, pos)) || is_same_loc(pos, des)) return POSITION;
	if((pos.x - des.x) * (pos.x - des.x) != (pos.y - des.y) * (pos.y - des.y)) return ILLEGAL_ROUTE;
	locat vec = {2 * (des.x - pos.x > 0) - 1, 2 * (des.y - pos.y > 0) - 1};
	int i, j;
	for(i = pos.x + vec.x, j = pos.y + vec.y ; !is_same_loc(get(i, j), des); i += vec.x, j += vec.y)
		if(!isEmpty(fetch(temp, get(i, j))) && (isWhite(fetch(temp, pos)) ^ isBlack(fetch(temp, get(i, j)))))
			return BLOCKED;
	
	return NORMAL;
}

Moves_and_Functions knight(chess *temp, locat pos, locat des){
	if(!isLegal_locat(pos) || !isLegal_locat(des) || !isKnight(fetch(temp, pos)) || is_same_loc(pos, des)) return POSITION;
	locat delta = {des.x - pos.x, des.y - pos.y};
	if(delta.x * delta.x + delta.y * delta.y != 5) return ILLEGAL_ROUTE;
	return NORMAL;
}

Moves_and_Functions rook(chess *temp, locat pos, locat des){
	if(!isLegal_locat(pos) || !isLegal_locat(des) || !isRook(fetch(temp, pos)) || is_same_loc(pos, des)) return POSITION;

	if((des.x - pos.x) * (des.y - pos.y) != 0) return ILLEGAL_ROUTE;
	int i, delta = 2 * ((des.x - pos.x) + (des.y - pos.y) > 0) - 1;
	if(des.x == pos.x){
		for(i = pos.y + delta ; i != des.y ; i += delta)
			if(!isEmpty(fetch(temp, get(des.x, i))))
				return BLOCKED;
	}
	else if(des.y == pos.y){
		for(i = pos.x + delta ; i != des.x ; i += delta)
			if(!isEmpty(fetch(temp, get(i, des.y))))
				return BLOCKED;
	}
	
	return NORMAL;
}

Moves_and_Functions queen(chess *temp, locat pos, locat des){
	if(!isLegal_locat(pos) || !isLegal_locat(des) || !isQueen(fetch(temp, pos)) || is_same_loc(pos, des)) return POSITION;
	bool Bishop = (pos.x - des.x) * (pos.x - des.x) == (pos.y - des.y) * (pos.y - des.y);
	bool Rook = (des.x - pos.x) * (des.y - pos.y) == 0;
	int i, j;
	if(Bishop && !Rook){
		locat vec = {2 * (des.x - pos.x > 0) - 1, 2 * (des.y - pos.y > 0) - 1};
		for(i = pos.x + vec.x, j = pos.y + vec.y ; !is_same_loc(get(i, j), des); i += vec.x, j += vec.y)
			if(!isEmpty(fetch(temp, get(i, j)))) return BLOCKED;
		
		return NORMAL;
	}
	else if(Rook && !Bishop){
		int delta = 2 * ((des.x - pos.x) + (des.y - pos.y) > 0) - 1;
		if(des.x == pos.x){
			for(i = pos.y + delta ; i != des.y ; i += delta)
				if(!isEmpty(fetch(temp, get(des.x, i))))
					return BLOCKED;
		}
		else if(des.y == pos.y){
			for(i = pos.x + delta ; i != des.x ; i += delta)
				if(!isEmpty(fetch(temp, get(i, des.y))))
					return BLOCKED;
		}
		
		return NORMAL;
	}
	return ILLEGAL_ROUTE;
}

Moves_and_Functions king(chess *temp, locat pos, locat des){
	if(!isLegal_locat(pos) || !isLegal_locat(des) || !isKing(fetch(temp, pos)) || is_same_loc(pos, des)) return POSITION;
	if((des.y - pos.y) * (des.y - pos.y) + (des.x - pos.x) * (des.x - pos.x) > 2)
		return ILLEGAL_ROUTE;
	if(!isEmpty(fetch(temp, des)) && (isBlack(fetch(temp, pos)) ^ isWhite(fetch(temp, des))))
		return ILLEGAL;
	if(threat(temp, des, isWhite(fetch(temp, pos))))
		return THREATENED_KING;
	return NORMAL;
}

bool isEnpassant(chess *temp, const rec *current, locat pos, locat des){
	if(!isLegal_locat(pos) || !isLegal_locat(des) || current == NULL) return 0;
	bool a = (current->to.x - current->from.x) * (current->to.x - current->from.x) == 4;
	bool b = isPawn(fetch(temp, current->to)) && isPawn(fetch(temp, pos));
	bool c = isEmpty(fetch(temp, des));
	bool d = isBlack(fetch(temp, pos)) ^ isBlack(fetch(temp, get(pos.x, des.y)));
	bool e = current->to.x == pos.x && current->to.y == des.y;
	bool f = (des.y - pos.y) * (des.y - pos.y) == 1 && des.x - pos.x == -2 * isWhite(fetch(temp, pos)) + 1;
	if(a && b && c && d && e && f)
		return 1;
	return 0;
}

bool fifty_move_rule(rec *current){
	if(step < 100)
		return 0;
	int i;
	rec *temp = current;
	for(i = 0 ; i < 100 && temp != NULL && temp->prev != NULL ; i++, temp = temp->prev)
		if(!isEmpty(temp->eaten) || (temp->not[0] >= 'a' && temp->not[0] <= 'h'))
			return 0;
	return (i == 100);
}

bool is_same_content(rec *a, rec *b){
	return (a->moving == b->moving) && is_same_loc(a->from, b->from) && is_same_loc(a->to, b->to) && (a->eaten == b->eaten) && !strcmp(a->not, b->not) && !memcmp(a->w, b->w, 3) && !memcmp(a->b, b->b, 3);
}

bool threefold_rule(rec *current, chess *temp){
	int n = step, tfr = 0;
	rec *tmp = current;
	chess chessboard = *temp;
	while(tmp != NULL && tmp->prev != NULL && tfr < 2){
		Undo(&chessboard, &tmp);
		if(memcmp(&chessboard, temp, sizeof(chess)) == 0 && is_same_content(tmp, current))
			tfr++;
		else if(!isEmpty(tmp->eaten) || isPawn(fetch(&chessboard, tmp->to)) || tmp->moving == CASTLE_Q || tmp->moving == CASTLE_K || tmp->moving == PROMOTION_Q || tmp->moving == PROMOTION_R || tmp->moving == PROMOTION_B || tmp->moving == PROMOTION_N)
			break;
	}
	step = n;
	return tfr == 2;
}

bool insufficient_piece(chess *temp){
	int i, j, bk = 0, wk = 0, bn = 0, wn = 0, bb = 0, wb = 0, a, b;
	locat pos = {-1, -1};
	char piece;
	for(i = 0 ; i < 8 ; i++)
		for(j = 0 ; j < 8 ; j++){
			piece = fetch(temp, get(i, j));
			if(isEmpty(piece))
				continue;
			else if(isKing(piece))
				isWhite(piece) ? wk++ : bk++;
			else if(isKnight(piece))
				isWhite(piece) ? wn++ : bn++;
			else if(isBishop(piece))
				isWhite(piece) ? wb++ : bb++;
			else return 0;
		}
	if(wk > 1 || bk > 1 || wn > 1 || bn > 1 || wb > 1 || bb > 1) return 0;
	if(!wk || !bk) return 0;
	if(!wn && !bn && !wb && !bb)
		return 1;
	else if(((wn && !bn) || (bn && !wn)) && !wb && !bb)
		return 1;
	else if(((wb && !bb) || (bb && !wb)) && !wn && !bn)
		return 1;
	else if(!wn && !bn && wb && bb){
		find(temp, wB, &pos);
		a = (pos.x + pos.y) & 1;
		find(temp, bB, &pos);
		b = (pos.x + pos.y) & 1;
		return !(a ^ b);
	}
	return 0;
}

Moves_and_Functions isLegal_move(rec *current, chess *temp, locat pos, locat des, Moves_and_Functions a, bool c){
	if(!isLegal_locat(pos) || !isLegal_locat(des)) return POSITION;
	char piece = fetch(temp, pos);
	bool color = isWhite(piece);
	if(isEmpty(piece))
		return EMPTY_POSITION;
	if((c ^ color))
		return COLOUR;
	if(!isEmpty(fetch(temp, des)) && (isWhite(piece) ^ isBlack(fetch(temp, des))))
		return ILLEGAL;
	switch(a){
	case NORMAL:
		if(isPawn(piece) && isEmpty(fetch(temp, des)) && isEnpassant(temp, current, pos, des))
			a = EN_PASSANT;
		else if(isPawn(piece))
			a = pawn(temp, pos, des, current);
		else if(isBishop(piece))
			a = bishop(temp, pos, des);
		else if(isKnight(piece))
			a =  knight(temp, pos, des);
		else if(isRook(piece))
			a = rook(temp, pos, des);
		else if(isQueen(piece))
			a = queen(temp, pos, des);
		else if(isKing(piece))
			a = king(temp, pos, des);
		else
			a = ILLEGAL;
		break;
	case EN_PASSANT:
		a = (isPawn(piece) && isEmpty(fetch(temp, des)) && isEnpassant(temp, current, pos, des)) ? EN_PASSANT : NOT_EN_PASSANT;
		break;
	case CASTLE_K:
		a = castle(temp, color, 0);
		break;
	case CASTLE_Q:
		a = castle(temp, color, 1);
		break;
	case PROMOTION_Q:
	case PROMOTION_R:
	case PROMOTION_B:
	case PROMOTION_N:
		if(!(isPawn(piece) && !isError(pawn(temp, pos, des, current))))
			a = ILLEGAL_PROMOTION;
		if(!(isWhite(fetch(temp, pos)) && isPawn(fetch(temp, pos)) && des.x == 0) && !(isBlack(fetch(temp, pos)) && isPawn(fetch(temp, pos)) && des.x == 7))
			a = ILLEGAL_PROMOTION;
		break;
	default:
		a = ILLEGAL;
	}

	if(isError(a))
		return a;
	if(isThreat_step(temp, pos, des, isWhite(fetch(temp, pos)), a))
		return THREATENED_MOVE;
	return a;
}

bool execute_action(chess *temp, locat pos, locat des, Moves_and_Functions a){
	char piece = fetch(temp, pos);
	char final = fetch(temp, des);
	int x = pos.x;
	int y = pos.y;
	
	switch(a){
	case NORMAL:
		if(isPawn(piece)){
			if(!isEmpty(final))
				eat(temp, des);
			move(temp, pos, des);
			return 1;
		}
		if(isRook(piece)){
			if(isWhite(piece)){
				if(is_same_loc(get(7, 0), pos))
					temp->w[0] = 1;
				else if(is_same_loc(get(7, 7), pos))
					temp->w[2] = 1;
			}
			else if(isBlack(piece)){
				if(is_same_loc(get(0, 0), pos))
					temp->b[0] = 1;
				else if(is_same_loc(get(0, 7), pos))
					temp->b[2] = 1;
			}
		}
		if(isKing(piece)){
			if(isWhite(piece))
				temp->w[1] = 1;
			else
				temp->b[1] = 1;
		}
		if(!isEmpty(final))
			eat(temp, des);
		move(temp, pos, des);
		break;
	case EN_PASSANT:
			eat(temp, get(pos.x, des.y));
			move(temp, pos, des);
			return 1;
	case CASTLE_K:
		if(isWhite(piece))
			temp->w[1] = temp->w[2] = 1;
		else if(isBlack(piece))
			temp->b[1] = temp->b[2] = 1;
		temp->board[x][5] = temp->board[x][7];
		temp->board[x][7] = X;
		temp->board[x][6] = temp->board[x][4];
		temp->board[x][4] = X;
		break;
	case CASTLE_Q:
		if(isWhite(piece))
			temp->w[0] = temp->w[1] = 1;
		else if(isBlack(piece))
			temp->b[0] = temp->b[1] = 1;
		temp->board[x][3] = temp->board[x][0];
		temp->board[x][0] = X;
		temp->board[x][2] = temp->board[x][4];
		temp->board[x][4] = X;
		break;
	case PROMOTION_Q:
		if(!isEmpty(fetch(temp, des)))
			push_die(temp, fetch(temp, des));
		temp->board[des.x][des.y] = (des.x == 7) ? bQ : wQ;
		temp->board[x][y] = X;
		break;
	case PROMOTION_R:
		if(!isEmpty(fetch(temp, des)))
			push_die(temp, fetch(temp, des));
		temp->board[des.x][des.y] = (des.x == 7) ? bR : wR;
		temp->board[x][y] = X;
		break;
	case PROMOTION_B:
		if(!isEmpty(fetch(temp, des)))
			push_die(temp, fetch(temp, des));
		temp->board[des.x][des.y] = (des.x == 7) ? bB : wB;
		temp->board[x][y] = X;
		break;
	case PROMOTION_N:
		if(!isEmpty(fetch(temp, des)))
			push_die(temp, fetch(temp, des));
		temp->board[des.x][des.y] = (des.x == 7) ? bN : wN;
		temp->board[x][y] = X;
		break;
	default:
		return 0;
	}
	return 1;
}

Moves_and_Functions movement(rec **current, chess *temp, locat pos, locat des, Moves_and_Functions a, bool color){
	a = isLegal_move(*current, temp, pos, des, a, color);
	if(isError(a)) return a;
	if(*current == NULL)
		init_record(current, temp, pos, des, a);
	else
		push_record(current, temp, pos, des, a);
	if(!execute_action(temp, pos, des, a))
		return ILLEGAL;
	draw_history(temp, *current);
	return a;
}

bool isChecked(chess *temp, bool color){
	locat king;
	if(color && !find(temp, wK, &king))
		return 0;
	if(!color && !find(temp, bK, &king))
		return 0;
	return threat(temp, king, color);
}

bool isCheckmated(rec *current, chess *temp, bool color){
	locat king;
	if(color && !find(temp, wK, &king))
		return 0;
	if(!color && !find(temp, bK, &king))
		return 0;
	if(!threat(temp, king, color))
		return 0;
	if(available_step(current, temp, color) == 0)
		return 1;
	return 0;
}

bool isStalemate(rec *current, chess *temp, bool color){

	locat king;
	if(color && !find(temp, wK, &king))
		return 0;
	if(!color && !find(temp, bK, &king))
		return 0;
	if(threat(temp, king, color))
		return 0;
	if(available_step(current, temp, color) == 0 || available_step(current, temp, !color) == 0)
		return 1;
	else if(fifty_move_rule(current))
		return 1;
	else if(threefold_rule(current, temp))
		return 1;
	else if(insufficient_piece(temp))
		return 1;
	return 0;

}

// FUNCTIONS
bool Undo(chess *temp, rec **current){
	if((*current)->prev == NULL)
		return 0;
	int i;
	int x = (*current)->to.x;
	int y = (*current)->to.y;
	
	switch((*current)->moving){
	case NORMAL:
		temp->board[(*current)->from.x][(*current)->from.y] = temp->board[x][y];
		temp->board[x][y] = (*current)->eaten;
		if(isWhite((*current)->eaten)){
			for(i = 0 ; i < 16 && !isEmpty(temp->w_die[i]) ; i++);
			temp->w_die[i - 1] = X;
		}
		else if(isBlack((*current)->eaten)){
			for(i = 0 ; i < 16 && !isEmpty(temp->b_die[i]) ; i++);
			temp->b_die[i - 1] = X;
		}
		break;
	case EN_PASSANT:
		temp->board[(*current)->from.x][(*current)->from.y] = temp->board[x][y];
		temp->board[(*current)->from.x][y] = (*current)->eaten;
		temp->board[x][y] = X;
		if(isWhite((*current)->eaten)){
			for(i = 0 ; i < 16 && !isEmpty(temp->w_die[i]) ; i++);
			temp->w_die[i - 1] = X;
		}
		else if(isBlack((*current)->eaten)){
			for(i = 0 ; i < 16 && !isEmpty(temp->b_die[i]) ; i++);
			temp->b_die[i - 1] = X;
		}
		break;
	case CASTLE_K:
		temp->board[x][7] = temp->board[x][5];
		temp->board[x][5] = X;
		temp->board[x][4] = temp->board[x][6];
		temp->board[x][6] = X;
		break;
	case CASTLE_Q:
		temp->board[x][0] = temp->board[x][3];
		temp->board[x][3] = X;
		temp->board[x][4] = temp->board[x][2];
		temp->board[x][2] = X;
		break;
	case PROMOTION_Q:
	case PROMOTION_R:
	case PROMOTION_B:
	case PROMOTION_N:
		temp->board[x][y] = X;
		temp->board[(*current)->from.x][(*current)->from.y] = (x == 7) ? bP : wP;
		if(!isEmpty((*current)->eaten)){
			temp->board[x][y] = (*current)->eaten;
			if(isWhite((*current)->eaten)){
				for(i = 0 ; i < 16 && !isEmpty(temp->w_die[i]) ; i++);
				temp->w_die[i - 1] = X;
			}
			else if(isBlack((*current)->eaten)){
				for(i = 0 ; i < 16 && !isEmpty(temp->b_die[i]) ; i++);
				temp->b_die[i - 1] = X;
			}
		}
		break;
	default:
		return 0;
	}
	
	for(i = 0 ; i < 3 ; i++){
		temp->w[i] = (*current)->w[i];
		temp->b[i] = (*current)->b[i];
	}
	*current = (*current)->prev;
	step--;
	return 1;
}

bool Redo(chess *temp, rec **current){
	if((*current)->next == NULL)
		return 0;

	*current = (*current)->next;

	execute_action(temp, (*current)->from, (*current)->to, (*current)->moving);

	step++;
	return 1;
}
