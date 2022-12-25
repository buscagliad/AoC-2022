#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <cstring>
#include <string>

using namespace std;
#define MAXX 2000
#define MAXY 2000

typedef enum {NORTH=0, SOUTH=1, WEST=2, EAST=3, STAY=4} dir_e;

typedef struct {
	bool    occupied;
	char	letter;
	dir_e	d;	// direction to move point
} gPoint;

int num_movements = 0;

const char *dirAsc(dir_e e)
{
	switch(e)
	{
		case  WEST: return "WEST";
		case  EAST: return "EAST";
		case SOUTH: return "SOUTH";
		case NORTH: return "NORTH";
		case STAY:  return "STAY";
	}
	return "ERROR";
}
const char dirChar(dir_e e)
{
	switch(e)
	{
		case  WEST: return 'W';
		case  EAST: return 'E';
		case SOUTH: return 'S';
		case NORTH: return 'N';
		case STAY:  return '.';
	}
	return 'X';
}

dir_e order[] = {NORTH, SOUTH, WEST, EAST};
int roundNumber = 0;

gPoint	gmap[MAXX];
gPoint	gridA[MAXX][MAXY];
int	colgrid[MAXX][MAXY];	// initially all zero - [x][y] is incremented everytime a elf is wanting to move there
						    // colgrid point > 1 indicate a collision, and movement there will not be allowed

int xpts = 0;
int ypts = 0;

int xmin = 0;
int xmax = 0;
int ymin = 0;
int ymax = 0;

#define has_neighbor(pts, x, y)   pts[x-1][y-1].occupied || pts[x][y-1].occupied || pts[x+1][y-1].occupied || \
						          pts[x-1][y].occupied   ||                         pts[x+1][y].occupied   || \
					              pts[x-1][y+1].occupied || pts[x][y+1].occupied || pts[x+1][y+1].occupied


void zerocolgrid()
{
	for (int i = 0; i < MAXX; i++)
	{
	    for (int j = 0; j < MAXY; j++)
	    {
	        colgrid[i][j] = 0;
	    }
	 }
}

void inccolgrid(gPoint g[MAXX][MAXY])
{
	for (int i = 0; i < MAXX; i++)
	{
	    for (int j = 0; j < MAXY; j++)
	    {
			switch(g[i][j].d)
			{
				case  WEST: colgrid[i-1][j]++;break;
				case  EAST: colgrid[i+1][j]++;break;
				case SOUTH: colgrid[i][j+1]++;break;
				case NORTH: colgrid[i][j-1]++;break;
				case STAY:  break;
			}
	    }
	 }
}
	

int	gridcount(gPoint g[MAXX][MAXY])
{
	int sum = 0;
	for (int i = 0; i < MAXX; i++)
	{
	    for (int j = 0; j < MAXY; j++)
		{
			if(g[i][j].occupied) sum++;
		}
	}
	return sum;
}



void setminmax(gPoint g[MAXX][MAXY])
{
	xmin = MAXX;
	xmax = 0;
	ymin = MAXY;
	ymax = 0;
	for (int i = 0; i < MAXX; i++)
	    for (int j = 0; j < MAXY; j++)
	    {
	        if (g[i][j].occupied)
	        {
				if (xmin > i) xmin = i;
				if (xmax < i) xmax = i;
				if (ymin > j) ymin = j;
				if (ymax < j) ymax = j;
			}
	    }
}

void outgrid(gPoint g[MAXX][MAXY], bool simple=true)
{

	printf("GRID  X: [%d .. %d]  Y: [%d .. %d]  -- %d elves\n", xmin, xmax, ymin, ymax, gridcount(g));
	for (int y = ymin; y <= ymax; y++)
	{
		for (int x = xmin; x <= xmax; x++)
		{
			if (simple) printf("%c", g[x][y].letter);
			else printf("%c", dirChar(g[x][y].d));
		}
		printf("\n");
	}
	printf("\n");
}


//  ----- x positive (EAST) ----->
//  <---- x negative (WEST) -----
//             /\       |
//             |		|
//  y (NORTH)  |		| y (SOUTH)
//   negative  |		| positive
//             |		|
//             |		|
//                      \/


//            NW     N     NE
//           
//           
//            W      *      E
//           
//           
//            SW     S     SE
//  

#ifdef XXXXX
Round: 1
GRID  X: [502 .. 510]  Y: [502 .. 510]  -- 22 elves
.....#...
...#...#.
.#..#.#..
.....#..#
..#.#.##.
#..#.#...
#.#.#.##.
.........
..#..#...

#endif
dir_e getDir(gPoint g[MAXX][MAXY], int x, int y)
{
	
	if (!(has_neighbor(g, x, y))) 
	{
		//printf("%c %d,%d has no neighbor\n", g[x][y].letter, x, y); 
		return STAY; 
	}
	
	for (int i = 0; i < 4; i++)
	{
		dir_e ord = order[(i + roundNumber) % 4];
		switch (ord) {
			case NORTH:  // NW  N  NE
				if (! (g[x-1][y-1].occupied || g[x][y-1].occupied || g[x+1][y-1].occupied) )
					return NORTH;
				break;
			case SOUTH:  // SW  S  SE
				if (! (g[x-1][y+1].occupied || g[x][y+1].occupied || g[x+1][y+1].occupied) ) 
					return SOUTH; 
				break;
			case WEST:   // NW  W  SW
				if (! (g[x-1][y-1].occupied || g[x-1][y].occupied || g[x-1][y+1].occupied) ) 
					return WEST; 
				break;
			case EAST:   // NE  E  SE
				if (! (g[x+1][y-1].occupied || g[x+1][y].occupied || g[x+1][y+1].occupied) ) 
					return EAST;
				break;
			case STAY:
				printf("WHAT %d,%d\n", x, y);
				return STAY;	// this is an ERROR!!!
		}
	}
	//printf("%c %d,%d no possible moves\n", g[x][y].letter, x, y); 
	return STAY;
}

void mod_pt(gPoint p[MAXX][MAXY], int x, int y, int dx, int dy);

bool move_pt(gPoint pts[MAXX][MAXY], int x, int y)
{
	switch(pts[x][y].d)
	{
		case STAY: return false;  // do nothing
		case EAST:
			mod_pt(pts, x, y, 1, 0);
			return true;
		case WEST:
			mod_pt(pts, x, y, -1, 0);
			return true;
		case NORTH:
			mod_pt(pts, x, y, 0, -1);
			return true;
		case SOUTH:
			mod_pt(pts, x, y, 0, 1);
			return true;
	}
	return false;
}

	


void mod_pt(gPoint p[MAXX][MAXY], int x, int y, int dx, int dy)
{
	int newx = x+dx;
	int newy = y+dy;
	if (colgrid[newx][newy] > 1)
	{
		p[x][y].d = STAY;
		//printf("%c - Avoiding collision (grid: %d) moving to %d,%d\n", p[x][y].letter, colgrid[newx][newy], newx, newy);
		return;
	}
	if (p[newx][newy].occupied)
	{
		//printf("%c  cannot move - space occupied %d,%d\n", p[x][y].letter, newx, newy);
		move_pt(p, newx, newy);
	}
	if (p[newx][newy].occupied)
	{
		//printf("WHAT THE F?? %d,%d\n", newx, newy);
	}
	//printf("%c %d,%d moving to %d,%d\n", p[x][y].letter, x, y, newx, newy); 
	p[x][y].occupied = false;
	p[x][y].d = STAY;
	p[newx][newy].occupied = true;
	p[newx][newy].d = STAY;
	p[newx][newy].letter = p[x][y].letter;
	p[x][y].letter = ' ';
	num_movements++;
	if (newx < xmin) xmin = newx;
	if (newx > xmax) xmax = newx;
	if (newy < ymin) ymin = newy;
	if (newy > ymax) ymax = newy;
}

void stay(gPoint g[MAXX][MAXY])
{
	for (int i = 0; i < MAXX; i++)
	{
		for (int j = 0; j < MAXY; j++)
		{
			g[i][j].d = STAY;
		}
	}
}
			

//
// update will go through the entire grid and
// determine the desired direction that the elve
// wants to move - it DOES NOT MOVE THEM
//
void update(gPoint g[MAXX][MAXY])
{
	int cxmin = xmin;
	int cxmax = xmax;
	int cymin = ymin;
	int cymax = ymax;
	zerocolgrid();
	stay(g);
	for (int i = cxmin; i <= cxmax; i++)
	{
		for (int j = cymin; j <= cymax; j++)
		{
			if (g[i][j].occupied)
			{
				g[i][j].d = getDir(g, i, j);
				//printf("%d,%d going %s\n", i, j, dirAsc(g[i][j].d));
			}
		}
	}
	inccolgrid(g);
}
//
// execute will go through the entire grid and
// move the elves as required by 'update'
//
void execute(gPoint pts[MAXX][MAXY])
{
	int cxmin = xmin;
	int cxmax = xmax;
	int cymin = ymin;
	int cymax = ymax;
	for (int i = cxmin; i <= cxmax; i++)
	{
		for (int j = cymin; j <= cymax; j++)
		{
			move_pt(pts, i, j);
		}
	}	
}

int  do_round(gPoint pts[MAXX][MAXY])
{
	num_movements = 0;
	update(pts);
	execute(pts);
	setminmax(pts);
	roundNumber++;
	return gridcount(pts);
}
	
	
	
#define BLANK '.'
#define MARK  '#'

int inf_value = BLANK;

#define MAXLINE 1000
///////////////////////////////////////////////////////////////////
// DEBUG outgrid outputs the grid to stdout
///////////////////////////////////////////////////////////////////



void clear(gPoint g[MAXX][MAXY])
{
	xmin = MAXX;
	xmax = 0;
	ymin = MAXY;
	ymax = 0;
	for (int i = 0; i < MAXX; i++)
	    for (int j = 0; j < MAXY; j++)
	    {
	        g[i][j].occupied = false;
	        g[i][j].letter = ' ';
	        g[i][j].d = STAY;
	    }
}
			


char letter = 'A';


bool parseline(FILE *f, gPoint g[MAXX][MAXY])
{
	char s[MAXLINE];

	fgets(s, MAXLINE, f);
	if (feof(f)) return false;
	ymax++;
	for (int i = 0; i < (int) strlen(s); i++)
	{
	    if (s[i] == '#')
	    {
			g[xmin + i][ymax].occupied = true;
			g[xmin + i][ymax].letter = letter;
			letter++; if (letter > 'Z') letter = 'A';
		}

	    if (xmin + i > xmax) xmax = xmin + i;
	}
	return true;
}
	
void init(const char *fn, gPoint g[MAXX][MAXY])
{
	//roundNumber = 0;
	clear(g);
	xmin = 500; // room to grow
	ymin = 500;	// room to grow
	xmax = xmin;
	ymax = ymin;
	FILE *f = fopen(fn, "r");
	while (parseline(f, g));
	fclose(f);
	setminmax(g);
}

void solvept1(const char *v, int true_spaces)
{
	init(v, gridA);

	printf("Input file: %s\n", v);
	//outgrid(gridA);
	int NUM_ROUNDS = 10;
	roundNumber = 0;
	for (int i = 0; i < NUM_ROUNDS; i++)
	{
		do_round(gridA);
//		printf("\n\nRound: %d\n", roundNumber);
//		outgrid(gridA);
	}
	int num_elves = gridcount(gridA);
	int num_spaces = (xmax-xmin+1)*(ymax-ymin+1) - num_elves;
	printf("Number of open spaces: %d\n", num_spaces);
	if (true_spaces > 0)
	{
		if (num_spaces != true_spaces)
			printf("ERROR - light count does not agree with truth: %d!\n", true_spaces);
		else
			printf("Successfully counted spaces!!\n");
	}

}

void solvept2(const char *v, int num_rounds)
{
	init(v, gridA);

	printf("Input file: %s\n", v);
	//outgrid(gridA);
	num_movements = 10;
	roundNumber = 0;
	while (num_movements)
	{
		do_round(gridA);
		//printf("Round: %d   Num Movements: %d\n", roundNumber, num_movements);
		//printf("\n\nRound: %d\n", roundNumber);
		//outgrid(gridA);
	}
	printf("Number of rounds: %d\n", roundNumber);
	if (num_rounds > 0)
	{
		if (num_rounds != roundNumber)
			printf("ERROR - light count does not agree with truth: %d!\n", num_rounds);
		else
			printf("Successfully counted spaces!!\n");
	}

}

int main(int argc, char **argv)
{
	solvept1("ex.txt", 110);
	solvept1("input.txt", 3906);
	solvept2("ex.txt", 20);
	solvept2("input.txt", 895);
	return 1;
}


