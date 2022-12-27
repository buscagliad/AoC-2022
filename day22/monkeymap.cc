
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <cstring>
#include <string>

using namespace std;
#define MAXN 2000

typedef enum {EMPTY=0, SPACE, WALL} grid_e;

typedef enum {RIGHT=0, DOWN=1, LEFT=2, UP=3} dir_e;

typedef struct {
	int		row;
	int		col;
	dir_e	facing;
}loc_t;


typedef struct {
	int		start;	// start row/column
	int		end;	// end row/column
	bool	walls[MAXN];	// true if row,col has a wall
} rowcol_t;

loc_t my_position;

typedef struct {
	dir_e	d;
	int		spaces;
} path_t;



void clearWalls(bool w[MAXN])
{
	for (int i = 0; i < MAXN; i++) w[i] = false;
}

void clearGrid(grid_e g[MAXN][MAXN])
{
	for (int i = 0; i < MAXN; i++) 
	    for (int j = 0; j < MAXN; j++)
			g[i][j] = EMPTY;
}

void dumprow(rowcol_t &r)
{
	int i;
	for (i = 0; i < r.start; i++) printf(" ");
	for (i = r.start ; i <= r.end; i++) if (r.walls[i]) printf("#"); else printf(".");
	printf("\n");
}

void	row2col(grid_e g[MAXN][MAXN], rowcol_t cols[MAXN], int num_cols)
{

#define CLEAR_COL(c, n)	 c[n].start = -1; c[n].end = -1; clearWalls(c[n].walls);
	int r = 0;

	for (int ncol = 0 ; ncol < num_cols; ncol++)
	{
		bool foundSpace = false;
		CLEAR_COL(cols, ncol);
		for (r = 0; r < MAXN; r++)
		{
			if (!foundSpace && g[r][ncol] == EMPTY) continue;
			if (!foundSpace && g[r][ncol] == EMPTY) foundSpace = true;

			if (cols[ncol].start == -1) cols[ncol].start = r;
			cols[ncol].end = r;
			if (g[r][ncol] == WALL) cols[ncol].walls[r] = true;
		}
		//printf("ncol: %d  r: %d\n", ncol, r);
	}
}

const char *dirAsc(dir_e e)
{
	switch(e)
	{
		case UP: 	return "UP";
		case DOWN: 	return "DOWN";
		case LEFT: 	return "LEFT";
		case RIGHT: return "RIGHT";
	}
	return "ERROR";
}

char letter = 'A';

int getnum(char **s)
{
	int	val = 0;
	while (isdigit(**s))
	{
		val = 10 * val + (**s) - '0';
		(*s)++;
	}
	return val;
}

dir_e last;

void parsepath(char *c, vector<path_t> &p)
{
	path_t	go;
	if ( !(*c) || *c == '\n') return;
	if (p.size() == 0)
	{
		go.d = RIGHT;
		last = RIGHT;
		go.spaces = getnum(&c);
		p.push_back(go);
		//printf("D: %s  N: %d  %s\n", dirAsc(go.d), go.spaces, c);	
		//parsepath(c, p);
	}
	bool cw = *c == 'R';
	if ( (*c != 'L') && (*c != 'R') ) printf("ERROR!!!\n");
	c++;
	if (cw)
	{
		switch(last)
		{
			case UP: go.d = RIGHT; break;
			case RIGHT: go.d = DOWN; break;
			case DOWN: go.d = LEFT; break;
			case LEFT: go.d = UP; break;
		}
	}
	else
	{
		switch(last)
		{
			case UP: go.d = LEFT; break;
			case RIGHT: go.d = UP; break;
			case DOWN: go.d = RIGHT; break;
			case LEFT: go.d = DOWN; break;
		}
	}
	go.spaces = getnum(&c);
	last = go.d;
	p.push_back(go);	
	//printf("D: %s  N: %d  %s\n", dirAsc(go.d), go.spaces, c);	
	
	parsepath(c, p);
}

#define MAXLINE 10000

//  g [row] [col]   g [y] [x]

bool parseboard(FILE *f, grid_e gr[MAXN][MAXN], rowcol_t board[MAXN], 
					int &num_rows, int &num_cols, vector<path_t> &p)
{
	char s[MAXLINE];

	fgets(s, MAXLINE, f);
	rowcol_t	r;
	clearWalls(r.walls);
	r.start = -1;
	if (strlen(s) < 2 && !feof(f))
	{
		fgets(s, MAXLINE, f);
		parsepath(s, p);
		return false;
	}
	for (int i = 0; i < (int) strlen(s); i++)
	{
	    if (s[i] == '#')
	    {
			r.walls[i] = true;
			gr[num_rows][i] = WALL;
			if (r.start == -1) r.start = i;
			else r.end = i;
		}
		else if (s[i] == '.')
		{
			gr[num_rows][i] = SPACE;
			if (r.start == -1) r.start = i;
			else r.end = i;
		}
	}
	if (r.end > num_cols) { num_cols = r.end + 1; }
	board[num_rows++] = r;
	return true;
}

	
void init(const char *fn, grid_e ge[MAXN][MAXN], rowcol_t board[MAXN], 
		int &num_rows, int &num_cols, vector<path_t> &p)
{
	FILE *f = fopen(fn, "r");
	num_rows = 0;
	num_cols = 0;
	clearGrid(ge);
	while (parseboard(f, ge, board, num_rows, num_cols, p));
	fclose(f);
}

void dump(rowcol_t board[MAXN], int bsize)
{
	//for (int i = 0; i < bsize; i++)
		//printf("Start: %d  End: %d\n", board[i].start, board[i].end);
	for (int i = 0; i < bsize; i++)
	    dumprow(board[i]);
}


void dump(grid_e g[MAXN][MAXN], int num_rows)
{
	bool nexti = false;

	for (int i = 0; i < num_rows; i++)
	{
		bool brkline = false;
		nexti = false;
		for (int j = 0; j < MAXN  && !nexti; j++)
		{
			switch (g[i][j])
			{
				case EMPTY: 
					if (brkline) 
					{
						printf("\n");
						nexti = true;
						continue;
					}
					else printf(" "); 
					break;
				case WALL:  printf("#"); brkline = true; 
					break;
				case SPACE: printf("."); brkline = true; 
					break;
			}
		}
	}
}

//[MAXN][MAXN]
rowcol_t rows[MAXN];
rowcol_t cols[MAXN];
grid_e gr[MAXN][MAXN];


int ROW(rowcol_t &col, int to_row)
{
	int width = col.end - col.start + 1;
	int delta = to_row - col.start;
	delta = (100 * width + delta) % width;
	
	return col.start + delta;
}

//
// row_move takes in a column with the starting spot (row) and number
//  of spaces to move, and returns the ending row
int row_move(rowcol_t &column, int col_index, int spaces, int from_row, int &to_row, bool debug)
{
	int width = column.end - column.start + 1;
//#define ROW(n)	(column.start + ( ( (n) + 10 * width) % (width) ) )
	int sgn = spaces > 0 ? 1 : -1;
	
	if (debug) printf("row_move: %d  [%d - %d] moving %d   width = %d\n", from_row, column.start, 
				column.end, spaces, width);
	for (int i = 1; i <= abs(spaces); i++)
	{
		if (column.walls[ROW(column, from_row + i * sgn)])
		{
			to_row = ROW(column, from_row + (i - 1) * sgn);
			if (debug) printf("Hitting wall at %d,%d  stopping at %d,%d\n", ROW(column, from_row + i * sgn), col_index, to_row, col_index);
			return to_row;
		}
	}
	to_row = ROW(column, from_row + spaces);
	return to_row;
}



int COL(rowcol_t &row, int to_col)
{
	int width = row.end - row.start + 1;
	int delta = to_col - row.start;
	delta = (100 * width + delta) % width;
	
	//if (to_col > row.end) delta = (to_col - row.end) % width;
	//if (to_col < row.start) delta = (row.start - to_col) % width;
	
	return row.start + delta;
}

//
// col_move takes in a row with the starting spot (col) and number
//  of spaces to move, and returns the ending column
int col_move(rowcol_t &row, int row_index, int spaces, int from_col, int &to_col, bool debug)
{
	int width = row.end - row.start + 1;
//#define COL(n)	((n) > row.end ? row.start + (n) - row.end : (n) < row.start ? row.end + ((n) - row.start) : (n) : (n) 
	int sgn = spaces > 0 ? 1 : -1;
	
	if (debug) printf("col_move: %d  [%d - %d] moving %d   width = %d\n", from_col, 
				row.start, row.end, spaces, width);
	for (int i = 1; i <= abs(spaces); i++)
	{
		if (row.walls[COL(row, from_col + i * sgn)])
		{
			to_col = COL(row, from_col + (i - 1 ) * sgn);
			if (debug) printf("Hitting wall at %d,%d  i=%d stopping at %d,%d\n", row_index, COL(row, from_col + i * sgn), i, row_index, to_col);
			return to_col;
		}
	}
	to_col = COL(row, from_col + spaces);
	return to_col;
}

void do_move(rowcol_t r[MAXN], rowcol_t c[MAXN], path_t &p, loc_t &pos, bool debug)
{
	int	num = p.spaces;
	int row = pos.row;
	int col = pos.col;
	int to_row;
	int to_col;
	
	if (debug) printf("From %d,%d -- %s  %d spaces  ", pos.row, pos.col, dirAsc(p.d), num);

	switch(p.d)
	{
		case RIGHT:
			pos.col = col_move(r[row], row,  num, col, to_col, debug);
			break;
		case LEFT:
			pos.col = col_move(r[row], row, -num, col, to_col, debug);
			break;
		case UP:
			pos.row = row_move(c[col], col, -num, row, to_row, debug);
			break;
		case DOWN:
			pos.row = row_move(c[col], col, num, row, to_row, debug);
			break;
	}
	if (debug) printf(" move to %d,%d\n", pos.row, pos.col);
}

int move(rowcol_t r[MAXN], rowcol_t c[MAXN], vector<path_t> &p, loc_t &pos, bool debug)
{
	for (size_t i = 0; i < p.size(); i++)
	{
		do_move(r, c, p[i], pos, debug);
		if (debug) if (r[pos.row].walls[pos.col]) printf("ERROR - bad location AA\n");
		if (debug) if (c[pos.col].walls[pos.row]) printf("ERROR - bad location BB\n");
	}
	return 0;
}

void dump(vector<path_t> &p)
{
	for (int i = 0; i < (int) p.size(); i++)
	{
		printf("%6s %6d spaces\n", dirAsc(p[i].d), p[i].spaces);
	}
}

void solvept1(const char *v, int true_spaces, bool debug = false)
{
	int	num_rows = 0;
	int num_cols = 0;
	vector<path_t> path;
	init(v, gr, rows, num_rows, num_cols, path);
	row2col(gr, cols, num_cols);

	printf("Input file: %s\n", v);

	if (debug) dump(rows, num_rows);
	if (debug) dump(path);
	loc_t mloc = {0, rows[0].start, RIGHT};
	
	move(rows, cols, path, mloc, debug);

	int password = (mloc.row + 1) * 1000 + 4 * (mloc.col + 1) + mloc.facing;
	printf("Part I GPS : %d\n", password);
	if (true_spaces > 0)
	{
		if (password != true_spaces)
			printf("ERROR - light count does not agree with truth: %d!\n\n", true_spaces);
		else
			printf("Successfully found password!!\n\n");
	}

}


void solvept2(const char *v, int true_spaces, bool debug = false)
{
	int	num_rows = 0;
	int num_cols = 0;
	vector<path_t> path;
	init(v, gr, rows, num_rows, num_cols, path);
	row2col(gr, cols, num_cols);

	printf("Input file: %s\n", v);

	if (debug) dump(rows, num_rows);
	if (debug) dump(path);
	loc_t mloc = {0, rows[0].start, RIGHT};
	
	move(rows, cols, path, mloc, debug);

	int password = (mloc.row + 1) * 1000 + 4 * (mloc.col + 1) + mloc.facing;
	printf("Part II GPS : %d\n", password);
	if (true_spaces > 0)
	{
		if (password != true_spaces)
			printf("ERROR - light count does not agree with truth: %d!\n\n", true_spaces);
		else
			printf("Successfully found password!!\n\n");
	}

}
int main(int argc, char **argv)
{
	solvept1("ex.txt", 6032, false);
	solvept1("input.txt", 75388, false); // 139172 too high  29292 too low 67244 is too low
	return 1;
}



