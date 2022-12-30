
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

typedef enum {ACE, TWO, THREE, FOUR, FIVE, SIX} die_t;

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
	bool	cw;
	int		spaces;
} path_t;


dir_e	next_facing(dir_e f, bool cw)
{
	dir_e rv;
	switch(f)
	{
		case RIGHT:
			if (cw) rv = DOWN;
			else rv = UP;
			break;
		case LEFT:
			if (cw) rv = UP;
			else rv = DOWN;
			break;
		case UP:
			if (cw) rv = RIGHT;
			else rv = LEFT;
			break;
		case DOWN:
			if (cw) rv = LEFT;
			else rv = RIGHT;
			break;
	}
	return rv;
}


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
bool  last_cw;

void parsepath(char *c, vector<path_t> &p)
{
	path_t	go;

	if ( !(*c) || *c == '\n') return;
	if (p.size() == 0)
	{
		go.d = UP;
		go.cw = true;
		last = RIGHT;
		go.spaces = getnum(&c);
		p.push_back(go);
		//printf("D: %s  N: %d  %s\n", dirAsc(go.d), go.spaces, c);	
		//parsepath(c, p);
	}
	bool cw = *c == 'R';
	if ( (*c != 'L') && (*c != 'R') ) printf("ERROR!!!\n");
	c++;
	go.cw = cw;
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
	//printf("D: %s  N: %d -  cw: %d  %s", dirAsc(go.d), go.spaces, cw, c);	
	
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


void transition50 (loc_t &cpos, dir_e d, int cubeside)
{
	int row = cpos.row;
	int col = cpos.col;
	

	switch(d)
	{
		case RIGHT:
			if (row < cubeside)		// 2 -> 5 Orange
			{
				cpos.col = 2 * cubeside - 1;
				cpos.row = 3 * cubeside - row - 1;
				cpos.facing = LEFT;
			}
			else if (row < 2 * cubeside) // 3 -> 2 Blue
			{
				cpos.col = cubeside + row;
				cpos.row = cubeside - 1;
				cpos.facing = UP;
			}			
			else if (row < 3 * cubeside) // 5 -> 2 ORANGE
			{
				cpos.col = 3 * cubeside - 1;
				cpos.row = 3 * cubeside - row - 1;
				cpos.facing = LEFT;
			}
			else if (row < 4 * cubeside) // 6 --> 5 Yellow
			{
				cpos.col = row - 2 * cubeside;
				cpos.row = 3 * cubeside - 1;
				cpos.facing = UP;
			}
			break;
		case LEFT:
			if (row < cubeside)		// 1 -> 4  purple
			{
				cpos.col = 0;
				cpos.row = 3 * cubeside - row - 1;
				cpos.facing = RIGHT;
			}
			else if (row < 2 * cubeside) // 3 --> 4 green
			{
				cpos.col = row - cubeside;
				cpos.row = 2 * cubeside;
				cpos.facing = DOWN;
			}			
			else if (row < 3 * cubeside) // 4 --> 1  purple
			{
				cpos.col = cubeside;
				cpos.row = 3 * cubeside - row - 1;
				cpos.facing = RIGHT;
			}
			else if (row < 4 * cubeside) // 6 --> 1  light blue
			{
				cpos.col = row - 2 * cubeside;
				cpos.row = 0;
				cpos.facing = DOWN;
			}
			break;
		case UP:
			if (col < cubeside)		// 4 -> 3 green
			{
				cpos.col = cubeside;
				cpos.row = cubeside + col;
				cpos.facing = RIGHT;
			}
			else if (col < 2 * cubeside) // 1 -> 6 light blue
			{
				cpos.col = 0;
				cpos.row = 2 * cubeside + col;
				cpos.facing = RIGHT;
			}			
			else if (col < 3 * cubeside) // 2 -> 6 pink
			{
				cpos.col = col - 2 * cubeside;
				cpos.row = 4 * cubeside - 1;
				cpos.facing = UP;
			}
			break;
		case DOWN:
			if (col < cubeside)		// 6 -> 2 Pink
			{
				cpos.col = 2 * cubeside + col;
				cpos.row = 0;
				cpos.facing = DOWN;
			}
			else if (col < 2 * cubeside) // 5 --> 6  Yellow
			{
				cpos.col = cubeside - 1;
				cpos.row = 2 * cubeside + col;
				cpos.facing = LEFT;
			}			
			else if (col < 3 * cubeside) // 2 --> 3 blue
			{
				cpos.col = 2 * cubeside - 1;
				cpos.row = col - cubeside;
				cpos.facing = LEFT;
			}
			break;
	}
}	

void transition4 (loc_t &cpos, dir_e d, int cubeside)
{
	int row = cpos.row;
	int col = cpos.col;
	

	switch(d)
	{
		case RIGHT:
			if (row < cubeside)		// 1 -> 6 orange
			{
				cpos.col = 4 * cubeside - 1;
				cpos.row = 3 * cubeside - row - 1;
				cpos.facing = LEFT;
			}
			else if (row < 2 * cubeside) // 4 -> 6 yellow
			{
				cpos.col = 5 * cubeside - row - 1;
				cpos.row = 2 * cubeside;
				cpos.facing = DOWN;
			}			
			else if (row < 3 * cubeside) // 6 -> 1 ORANGE
			{
				cpos.col = 3 * cubeside - 1;
				cpos.row = 3 * cubeside - row - 1;
				cpos.facing = LEFT;
			}
			break;
		case LEFT:
			if (row < cubeside)		// 1 -> 3  blue
			{
				cpos.col = row + cubeside;
				cpos.row = cubeside;
				cpos.facing = DOWN;
			}
			else if (row < 2 * cubeside) // 2 -> 6 purple
			{
				cpos.col = 5 * cubeside - row - 1;
				cpos.row = 3 * cubeside - 1;
				cpos.facing = UP;
			}			
			else if (row < 3 * cubeside) // 5 -> 3 green
			{
				cpos.col = 4 * cubeside - row - 1;
				cpos.row = 2 * cubeside - 1;
				cpos.facing = UP;
			}
			break;
		case UP:
			if (col < cubeside)		// 2 -> 1 pink
			{
				cpos.col = 2 * cubeside + col;
				cpos.row = 0;
				cpos.facing = DOWN;
			}
			else if (col < 2 * cubeside) // 3 -> 1 blue
			{
				cpos.col = 2 * cubeside;
				cpos.row = col - cubeside;
				cpos.facing = RIGHT;
			}			
			else if (col < 3 * cubeside) // 1 -> 2 pink
			{
				cpos.col = col - 2 * cubeside;
				cpos.row = cubeside;
				cpos.facing = DOWN;
			}
			else if (col < 4 * cubeside) // 6 -> 4 yellow
			{
				cpos.col = 3 * cubeside - 1;
				cpos.row = 5 * cubeside - col - 1;
				cpos.facing = LEFT;
			}
			break;
		case DOWN:
			if (col < cubeside)		// 2 -> 5 light blue
			{
				cpos.col = 3 * cubeside - col - 1;
				cpos.row = 3 * cubeside - 1;
				cpos.facing = UP;
			}
			else if (col < 2 * cubeside) // 3 -> 5 green
			{
				cpos.col = 2 * cubeside;
				cpos.row = 4 * cubeside - col - 1;
				cpos.facing = RIGHT;
			}			
			else if (col < 3 * cubeside) // 5 -> 2 light blue
			{
				cpos.col = 3 * cubeside - col - 1;
				cpos.row = 2 * cubeside - 1;
				cpos.facing = UP;
			}
			else if (col < 4 * cubeside) // 6 -> 2 purple
			{
				cpos.col = 0;
				cpos.row = 5 * cubeside - col - 1;
				cpos.facing = RIGHT;
			}
			break;
	}
}
	
void transition(loc_t &cpos, dir_e d, int cubeside)
{
	if (cubeside == 4)
	    transition4 (cpos, d, cubeside);
	if (cubeside == 50)
	    transition50 (cpos, d, cubeside);
}

bool next_pos(rowcol_t r[MAXN], rowcol_t c[MAXN], loc_t &cpos, 
		int cubeside, bool part2)
{
	loc_t  pos = cpos;
	switch(cpos.facing)
	{
		case RIGHT:
			if (pos.col == r[pos.row].end) // need to transition
			{
				if (part2) transition(pos, RIGHT, cubeside);
				else pos.col = r[pos.row].start;
			}
			else
				pos.col++;
			break;
		case LEFT:
			if (pos.col == r[pos.row].start) // need to transition
			{
				if (part2) transition(pos, LEFT, cubeside);
				else pos.col = r[pos.row].end;
			}
			else
				pos.col--;
			break;
		case UP:
			if (pos.row == c[pos.col].start) // need to transition
			{
				if (part2) transition(pos, UP, cubeside);
				else pos.row = c[pos.col].end;
			}
			else
				pos.row--;
			break;
		case DOWN:
			if (pos.row == c[pos.col].end) // need to transition
			{
				if (part2) transition(pos, DOWN, cubeside);
				else pos.row = c[pos.col].start;
			}
			else
				pos.row++;
			break;
	}
	if ( (pos.row < 0) || (pos.col < 0) )
		printf("ERROR - pos.row = %d  pos.col = %d\n", pos.row, pos.col);
	if (r[pos.row].walls[pos.col]) return false;
	cpos = pos;
	return true;
}

void do_movept2(rowcol_t r[MAXN], rowcol_t c[MAXN], bool cw, int num, 
							loc_t &pos, int cubeside, bool part2, bool debug)
{
	
	if (debug) printf("From %d,%d -- %s  %d spaces  ", pos.row, pos.col, 
		dirAsc(pos.facing), num);
	while ( (num > 0) && next_pos(r, c, pos, cubeside, part2)) 
	{
		num--;
	}
	if (debug) printf(" move to %d,%d  facing %s\n", pos.row, pos.col, dirAsc(pos.facing));
}

int movept2(rowcol_t r[MAXN], rowcol_t c[MAXN], vector<path_t> &p, loc_t &pos, int cubeside, bool part2, bool debug)
{
	pos.facing = UP;
	for (size_t i = 0; i < p.size(); i++)
	{
		pos.facing = next_facing(pos.facing, p[i].cw);
		if (debug) printf("From %d,%d  facing %s  with CW: %d for %d steps\n",
			pos.row, pos.col, 
			dirAsc(pos.facing), p[i].cw, p[i].spaces);
		do_movept2(r, c, p[i].cw, p[i].spaces, pos, cubeside, part2, debug);
	}
	return 0;
}

void dump(vector<path_t> &p)
{
	dir_e    d = UP;
	for (int i = 0; i < (int) p.size(); i++)
	{
		d = next_facing(d, p[i].cw);
		printf("%6s %6d spaces %s\n", dirAsc(d), p[i].spaces,
			p[i].cw ? "CW" : "CCW");
	}
}




void solve(const char *v, int cubeside, int true_password, 
					bool part2, bool debug = false)
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
	
	if (0)
	{
		loc_t cpos;
		loc_t npos;
		for (int r = 0; r < num_rows; r++)
		{
			cpos.row = r;
			cpos.col = rows[r].start;
			npos = cpos;
			transition50 (npos, LEFT, cubeside);

		    printf("%s  %d,%d  -->  %d,%d\n", dirAsc(LEFT), cpos.row, cpos.col, npos.row, npos.col);
		}
		printf("\n");
		for (int r = 0; r < num_rows; r++)
		{

			cpos.row = r;
			cpos.col = rows[r].end;
			npos = cpos;
			transition50 (npos, RIGHT, cubeside);

		    printf("%s  %d,%d  -->  %d,%d\n", dirAsc(RIGHT), cpos.row, cpos.col, npos.row, npos.col);
		}
		printf("\n");
		for (int c = 0; c < num_cols; c++)
		{
			cpos.row = cols[c].end;
			cpos.col = c;
			npos = cpos;
			transition50 (npos, DOWN, cubeside);

		    printf("%s  %d,%d  -->  %d,%d\n", dirAsc(DOWN), cpos.row, cpos.col, npos.row, npos.col);
		}
		printf("\n");
		for (int c = 0; c < num_cols; c++)
		{

			cpos.row = cols[c].start;
			cpos.col = c;
			npos = cpos;
			transition50 (npos, UP, cubeside);

		    printf("%s  %d,%d  -->  %d,%d\n", dirAsc(UP), cpos.row, cpos.col, npos.row, npos.col);
		}
		printf("\n");
		exit(1);
	}
	
	movept2(rows, cols, path, mloc, cubeside, part2, debug);

	int password = (mloc.row + 1) * 1000 + 4 * (mloc.col + 1) + mloc.facing;
	if (part2) printf("Part II GPS : %d\n", password);
	else printf("Part I GPS : %d\n", password);
	if (true_password > 0)
	{
		if (password != true_password)
			printf("ERROR - password does not agree with truth: %d!\n\n", true_password);
		else
			printf("Successfully found password!!\n\n");
	}

}
int main(int argc, char **argv)
{
	solve("ex.txt", 4, 6032, false, false);
	solve("ex.txt", 4, 5031, true, false);
	solve("input.txt", 50, 75388, false); 
	solve("input.txt", 50, 182170, true, false);
	return 1;
}



