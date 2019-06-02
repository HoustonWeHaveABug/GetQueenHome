#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define SIDES_N 2
#define ON_MOVE_KING 0
#define ON_MOVE_QUEEN 1
#define WINNER_KING -1
#define WINNER_NONE 0
#define WINNER_QUEEN 1

typedef struct {
	int row;
	int col;
	int diag_b;
	int diag_u;
	int idx;
}
square_t;

typedef struct position_s position_t;

struct position_s {
	square_t *king_square;
	square_t *queen_square;
	int on_move;
	int is_legal;
	int winner;
	int visited;
	int moves_n;
	position_t **moves;
};

void set_square(square_t *, int, int);
square_t *read_square(void);
void set_position(position_t *, square_t *, square_t *, int);
int king_in_check(square_t *, square_t *);
int get_queen_home(position_t *, int, int, int);
void add_king_moves(position_t *);
int compare_king_moves(const void *, const void *);
void add_queen_moves(position_t *);
int compare_queen_moves(const void *, const void *);
int add_move(position_t *, position_t *);
position_t *get_position(square_t *, square_t *, int);

int board_size, squares_n, depth_max;
square_t *squares, *queen_home;
position_t *positions;

int main(void) {
	int on_move_start, positions_n, r, i;
	square_t *king_square_start, *queen_square_start;
	position_t *position_start;
	if (scanf("%d", &board_size) != 1 || board_size < 1 || board_size > INT_MAX/board_size) {
		fprintf(stderr, "Invalid board size\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	squares_n = board_size*board_size;
	if (squares_n > INT_MAX/SIDES_N || squares_n > INT_MAX/squares_n/SIDES_N) {
		fprintf(stderr, "Invalid board size\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	squares = malloc(sizeof(square_t)*(size_t)squares_n);
	if (!squares) {
		fprintf(stderr, "Could not allocate memory for squares\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	for (i = 0; i < board_size; i++) {
		int j;
		for (j = 0; j < board_size; j++) {
			set_square(squares+i*board_size+j, i, j);
		}
	}
	queen_home = read_square();
	if (!queen_home) {
		free(squares);
		return EXIT_FAILURE;
	}
	king_square_start = read_square();
	if (!king_square_start) {
		free(squares);
		return EXIT_FAILURE;
	}
	queen_square_start = read_square();
	if (!queen_square_start) {
		free(squares);
		return EXIT_FAILURE;
	}
	if (scanf("%d", &on_move_start) != 1 || on_move_start < ON_MOVE_KING || on_move_start > ON_MOVE_QUEEN) {
		fprintf(stderr, "Invalid starting on-move\n");
		fflush(stderr);
		free(squares);
		return EXIT_FAILURE;
	}
	squares_n = board_size*board_size;
	positions_n = squares_n*squares_n*2;
	positions = malloc(sizeof(position_t)*(size_t)positions_n);
	if (!positions) {
		fprintf(stderr, "Could not allocate memory for positions\n");
		fflush(stderr);
		free(squares);
		return EXIT_FAILURE;
	}
	for (i = 0; i < squares_n; i++) {
		int j;
		for (j = 0; j < squares_n; j++) {
			set_position(positions+i*squares_n*SIDES_N+j*SIDES_N+ON_MOVE_KING, squares+i, squares+j, ON_MOVE_KING);
			set_position(positions+i*squares_n*SIDES_N+j*SIDES_N+ON_MOVE_QUEEN, squares+i, squares+j, ON_MOVE_QUEEN);
		}
	}
	position_start = get_position(king_square_start, queen_square_start, on_move_start);
	if (!position_start->is_legal) {
		fprintf(stderr, "Invalid starting position\n");
		fflush(stderr);
		free(positions);
		free(squares);
		return EXIT_FAILURE;
	}
	depth_max = 0;
	do {
		depth_max++;
		printf("Depth %d\n", depth_max);
		fflush(stdout);
		r = get_queen_home(position_start, 1, WINNER_KING, WINNER_QUEEN);
	}
	while (depth_max <= squares_n && r == WINNER_NONE);
	if (r == WINNER_KING) {
		puts("The king is the winner");
	}
	else if (r == WINNER_QUEEN) {
		puts("The queen is the winner");
	}
	else {
		puts("There is no winner");
	}
	fflush(stdout);
	for (i = 0; i < positions_n; i++) {
		if (positions[i].moves) {
			free(positions[i].moves);
		}
	}
	free(positions);
	free(squares);
	return EXIT_SUCCESS;
}

void set_square(square_t *square, int row, int col) {
	square->row = row;
	square->col = col;
	square->diag_b = row-col;
	square->diag_u = row+col;
	square->idx = row*board_size+col;
}

square_t *read_square(void) {
	int row, col;
	if (scanf("%d%d", &row, &col) != 2 || row < 0 || row >= board_size || col < 0 || col >= board_size) {
		fprintf(stderr, "Invalid square\n");
		fflush(stderr);
		return NULL;
	}
	return squares+row*board_size+col;
}

void set_position(position_t *position, square_t *king_square, square_t *queen_square, int on_move) {
	position->king_square = king_square;
	position->queen_square = queen_square;
	position->on_move = on_move;
	if (on_move == ON_MOVE_KING) {
		if (king_square == queen_square) {
			position->is_legal = 0;
		}
		else {
			position->is_legal = queen_square == queen_home || king_in_check(king_square, queen_square);
		}
	}
	else if (on_move == ON_MOVE_QUEEN) {
		if (king_square == queen_square) {
			position->is_legal = queen_square != queen_home;
		}
		else {
			position->is_legal = queen_square != queen_home && !king_in_check(king_square, queen_square);
		}
	}
	else {
		position->is_legal = 0;
	}
	if (position->is_legal) {
		if (on_move == ON_MOVE_KING) {
			if (queen_square == queen_home) {
				position->winner = WINNER_QUEEN;
			}
			else {
				position->winner = WINNER_NONE;
			}
		}
		else if (on_move == ON_MOVE_QUEEN) {
			if (king_square == queen_square) {
				position->winner = WINNER_KING;
			}
			else {
				position->winner = WINNER_NONE;
			}
		}
		else {
			position->winner = WINNER_NONE;
		}
	}
	else {
		position->winner = WINNER_NONE;
	}
	position->visited = 0;
	position->moves_n = 0;
	position->moves = NULL;
}

int king_in_check(square_t *king_square, square_t *queen_square) {
	return king_square->row == queen_square->row || king_square->col == queen_square->col || king_square->diag_b == queen_square->diag_b || king_square->diag_u == queen_square->diag_u;
}

int get_queen_home(position_t *position, int depth, int alpha, int beta) {
	int r_best, i;
	if (position->winner != WINNER_NONE || depth == depth_max) {
		return position->winner;
	}
	if (position->visited) {
		return WINNER_NONE;
	}
	if (!position->moves) {
		if (position->on_move == ON_MOVE_KING) {
			add_king_moves(position);
		}
		else if (position->on_move == ON_MOVE_QUEEN) {
			add_queen_moves(position);
		}
		if (!position->moves) {
			return WINNER_NONE;
		}
	}
	position->visited = 1;
	if (position->on_move == ON_MOVE_KING) {
		r_best = WINNER_QUEEN;
		for (i = 0; i < position->moves_n; i++) {
			int r = get_queen_home(position->moves[i], depth+1, alpha, beta);
			if (r < r_best) {
				r_best = r;
			}
			if (r_best <= alpha) {
				break;
			}
			if (r_best < beta) {
				beta = r_best;
			}
		}
	}
	else if (position->on_move == ON_MOVE_QUEEN) {
		r_best = WINNER_KING;
		for (i = 0; i < position->moves_n; i++) {
			int r = get_queen_home(position->moves[i], depth+1, alpha, beta);
			if (r > r_best) {
				r_best = r;
			}
			if (r_best >= beta) {
				break;
			}
			if (r_best > alpha) {
				alpha = r_best;
			}
		}
	}
	else {
		r_best = WINNER_NONE;
	}
	position->visited = 0;
	return r_best;
}

void add_king_moves(position_t *position) {
	if (position->king_square->row > 0 && position->king_square->col > 0 && !add_move(position, get_position(position->king_square-board_size-1, position->queen_square, ON_MOVE_QUEEN))) {
		return;
	}
	if (position->king_square->row > 0 && !add_move(position, get_position(position->king_square-board_size, position->queen_square, ON_MOVE_QUEEN))) {
		return;
	}
	if (position->king_square->row > 0 && position->king_square->col < board_size-1 && !add_move(position, get_position(position->king_square-board_size+1, position->queen_square, ON_MOVE_QUEEN))) {
		return;
	}
	if (position->king_square->col < board_size-1 && !add_move(position, get_position(position->king_square+1, position->queen_square, ON_MOVE_QUEEN))) {
		return;
	}
	if (position->king_square->row < board_size-1 && position->king_square->col < board_size-1 && !add_move(position, get_position(position->king_square+board_size+1, position->queen_square, ON_MOVE_QUEEN))) {
		return;
	}
	if (position->king_square->row < board_size-1 && !add_move(position, get_position(position->king_square+board_size, position->queen_square, ON_MOVE_QUEEN))) {
		return;
	}
	if (position->king_square->row < board_size-1 && position->king_square->col > 0 && !add_move(position, get_position(position->king_square+board_size-1, position->queen_square, ON_MOVE_QUEEN))) {
		return;
	}
	if (position->king_square->col > 0 && !add_move(position, get_position(position->king_square-1, position->queen_square, ON_MOVE_QUEEN))) {
		return;
	}
	qsort(position->moves, (size_t)position->moves_n, sizeof(position_t *), compare_king_moves);
}

int compare_king_moves(const void *a, const void *b) {
	position_t *position_a = *(position_t * const *)a, *position_b = *(position_t * const *)b;
	return position_a->winner-position_b->winner;
}

void add_queen_moves(position_t *position) {
	square_t *queen_square = position->queen_square;
	while (queen_square->row > 0 && queen_square->col > 0) {
		if (!add_move(position, get_position(position->king_square, queen_square-board_size-1, ON_MOVE_KING))) {
			return;
		}
		queen_square -= board_size+1;
	}
	queen_square = position->queen_square;
	while (queen_square->row > 0) {
		if (!add_move(position, get_position(position->king_square, queen_square-board_size, ON_MOVE_KING))) {
			return;
		}
		queen_square -= board_size;
	}
	queen_square = position->queen_square;
	while (queen_square->row > 0 && queen_square->col < board_size-1) {
		if (!add_move(position, get_position(position->king_square, queen_square-board_size+1, ON_MOVE_KING))) {
			return;
		}
		queen_square -= board_size-1;
	}
	queen_square = position->queen_square;
	while (queen_square->col < board_size-1) {
		if (!add_move(position, get_position(position->king_square, queen_square+1, ON_MOVE_KING))) {
			return;
		}
		queen_square++;
	}
	queen_square = position->queen_square;
	while (queen_square->row < board_size-1 && queen_square->col < board_size-1) {
		if (!add_move(position, get_position(position->king_square, queen_square+board_size+1, ON_MOVE_KING))) {
			return;
		}
		queen_square += board_size+1;
	}
	queen_square = position->queen_square;
	while (queen_square->row < board_size-1) {
		if (!add_move(position, get_position(position->king_square, queen_square+board_size, ON_MOVE_KING))) {
			return;
		}
		queen_square += board_size;
	}
	queen_square = position->queen_square;
	while (queen_square->row < board_size-1 && queen_square->col > 0) {
		if (!add_move(position, get_position(position->king_square, queen_square+board_size-1, ON_MOVE_KING))) {
			return;
		}
		queen_square += board_size-1;
	}
	queen_square = position->queen_square;
	while (queen_square->col > 0) {
		if (!add_move(position, get_position(position->king_square, queen_square-1, ON_MOVE_KING))) {
			return;
		}
		queen_square--;
	}
	qsort(position->moves, (size_t)position->moves_n, sizeof(position_t *), compare_queen_moves);
}

int compare_queen_moves(const void *a, const void *b) {
	position_t *position_a = *(position_t * const *)a, *position_b = *(position_t * const *)b;
	return position_b->winner-position_a->winner;
}

int add_move(position_t *position, position_t *move) {
	if (move->is_legal) {
		if (position->moves_n == 0) {
			position->moves = malloc(sizeof(position_t *));
			if (!position->moves) {
				fprintf(stderr, "Could not allocate memory for positions->moves\n");
				fflush(stderr);
				return 0;
			}
		}
		else {
			position_t **moves = realloc(position->moves, sizeof(position_t *)*(size_t)(position->moves_n+1));
			if (!moves) {
				fprintf(stderr, "Could not reallocate memory for position->moves\n");
				fflush(stderr);
				free(position->moves);
				position->moves = NULL;
				return 0;
			}
			position->moves = moves;
		}
		position->moves[position->moves_n++] = move;
	}
	return 1;
}

position_t *get_position(square_t *king_square, square_t *queen_square, int on_move) {
	return positions+king_square->idx*squares_n*SIDES_N+queen_square->idx*SIDES_N+on_move;
}
