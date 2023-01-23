#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <cstring>
#include <string>

using namespace std;
#define MAXX 155
#define MAXY 23

typedef struct {
	bool check[MAXX][MAXY];
}bmap_t;

vector<bmap_t> bmap_time;	// bmap_time[t][x][y] is true, we have already been at x,y at time t - 
							// we don't have to do this again
							
void clear(bmap_t &b)
{
	for (int i = 0; i < MAXX; i++)
	    for (int j = 0; j < MAXY; j++)
			b.check[i][j] = false;
}

typedef enum {STAY=0, UP=1, RIGHT=2, DOWN=4, LEFT=8, WALL=64} dir_e;


int num_movements = 0;
int path_solution = 999999999;
bool debug = false;

const char *dirAsc(int e)
{
	switch(e)
	{
		case LEFT:  return "LEFT";
		case RIGHT: return "RIGHT";
		case DOWN:  return "DOWN";
		case UP:    return "UP";
		case STAY:  return "STAY";
		case WALL:  return "WALL";
	}
	return "ERROR";
}

char dirChar(int e)
{
	switch(e)
	{
		case LEFT:  return 'W';
		case RIGHT: return 'E';
		case DOWN:  return 'S';
		case UP:    return 'N';
		case STAY:  return '.';
		case WALL:  return '#';
	}
	return 'X';
}

char exChar(int e)
{
	if (e == LEFT)  return '<';
	if (e == RIGHT) return '>';
	if (e == DOWN)  return 'v';
	if (e == UP)    return '^';
	if (e == STAY)  return '.';
	if (e == WALL)  return '#';
	// we get here if more that one blizzard is present
	int c = 0;
	c += e & LEFT ? 1 : 0;
	c += e & RIGHT ? 1 : 0;
	c += e & DOWN ? 1 : 0;
	c += e & UP ? 1 : 0;
	return '0' + c;
	
}

dir_e order[] = {UP, DOWN, LEFT, RIGHT};

typedef struct {
	char b[MAXX][MAXY];
	int  xpts;
	int  ypts;
	int  minute;
} bstate;

vector<bstate> allbasins;
int		minutes[MAXX][MAXY];		// number of minutes to get to x,y		


//int xpts = 0;
//int ypts = 0;


void outgrid(bstate &b, int Ex = -5, int Ey =-5, bool EWNS = false)
{

	printf("Minute: %d  [%d,%d]\n", b.minute, b.xpts, b.ypts);
	for (int y = 0; y < b.ypts; y++)
	{
		for (int x = 0; x < b.xpts; x++)
		{
			if (Ex == x && Ey == y) printf("E");
			else if (EWNS) printf("%c", dirChar(b.b[x][y]));
			else printf("%c", exChar(b.b[x][y]));
		}
		printf("\n");
	}
	printf("\n");
}


char getbs(bstate &bs, int i, int j)
{
	if (i <= 0) i = bs.xpts - 2;
	else if (i >= bs.xpts - 1) i = 1;
	if (j <= 0) j = bs.ypts - 2;
	else if (j >= bs.ypts - 1) j = 1;
	return bs.b[i][j];
}

#define	GETBS(a,b,d)	if (getbs(bs, a, b) & d) {													\
							rs |= d;																\
							if (debug) printf("at (%d,%d) storm is heading %s - INCLUDED\n", a, b, dirAsc(getbs(bs, a, b))); \
						} 																			\
						else 																		\
						{																			\
							if (debug) printf("at (%d,%d) storm is heading %s - EXCLUDED\n", a, b, dirAsc(getbs(bs, a, b))); \
						}

char step(bstate &bs, int i, int j, bool debug = false)
{
	char rs = 0;
	if (debug) printf("Examining cell %d,%d\n", i, j);
	GETBS(i-1,j,RIGHT);
	GETBS(i+1,j,LEFT);
	GETBS(i,j-1,DOWN);
	GETBS(i,j+1,UP);
	if (debug) printf("\n");
	return rs;
}

void blizzard_update(bstate &bs, int minute)
{
	if (minute < (int) allbasins.size()) {
		bs = allbasins[minute];
		return;
	}
	if (minute > (int) allbasins.size())
	{
		printf("ERROR - minute: %d   allbasins: %d   bmap_time: %d\n", minute, (int)allbasins.size(), (int)bmap_time.size());
	}
	bstate nbs = bs;
	nbs.minute = minute;
	for (int i = 1; i < bs.xpts - 1; i++)
	{
		for (int j = 1; j < bs.ypts - 1; j++)
		{
			nbs.b[i][j] = step(bs, i, j);
		}
	}
	allbasins.push_back(nbs);
	bmap_t ps;
	clear(ps);
	bmap_time.push_back(ps);
	bs = nbs;
	//printf("Minute: %d   allbasins: %d   bmap_time: %d\n", minute, (int)allbasins.size(), (int)bmap_time.size());
}

bool update(bstate &bs, int minute)
{
	blizzard_update(bs, minute);
	return true;
}


void initminute()
{
	for (int i = 0; i < MAXX; i++)
	{
	    for (int j = 0; j < MAXY; j++)
	    {
	        minutes[i][j] = -1;
	    }
	 }
}




#define MAXLINE 1000
///////////////////////////////////////////////////////////////////
// DEBUG outgrid outputs the grid to stdout
///////////////////////////////////////////////////////////////////



void clear(bstate &b)
{
	b.minute = 0;
	b.xpts = 0;
	b.ypts = 0;
	for (int i = 0; i < MAXX; i++)
	    for (int j = 0; j < MAXY; j++)
	    {
	        b.b[i][j] = STAY;
	    }
}
			


char letter = 'A';


bool parseline(FILE *f, bstate &basin)
{
	char s[MAXLINE];

	fgets(s, MAXLINE, f);
	if (feof(f)) return false;
	int slen = strlen(s);
	if (basin.xpts < slen-1) { basin.xpts = slen-1; 
		//printf("xpts = %d\n", xpts); 
	}
	for (int i = 0; i < slen; i++)
	{
		switch(s[i])
		{
			case '#': basin.b[i][basin.ypts] = WALL;  break;
			case '>': basin.b[i][basin.ypts] = RIGHT; break;
			case '<': basin.b[i][basin.ypts] = LEFT;  break;
			case '^': basin.b[i][basin.ypts] = UP;    break;
			case 'v': basin.b[i][basin.ypts] = DOWN;  break;
			case '.': basin.b[i][basin.ypts] = 0;     break;
			case '\n': continue;
		}

	}
	basin.ypts++;
	return true;
}

void init(const char *fn, bstate &basin)
{
	clear(basin);

	initminute();
	//path_solution = 999999999;
	FILE *f = fopen(fn, "r");
	while (parseline(f, basin));
	fclose(f);
	allbasins.clear();
	allbasins.push_back(basin);
	bmap_t ps;
	clear(ps);
	bmap_time.clear();
	bmap_time.push_back(ps);
	
}

bool procede(bstate &bs, int x, int y, const char *s)
{
	if (x == 1 && y == 0) return true;
	if (x == bs.xpts - 2 && y == bs.ypts - 1) return true;
//	{
//		if (path_solution > bs.minute)
//		{
//			path_solution = bs.minute;
//		}
//		printf("SOLUTION::  Minute: %d -- at %d,%d\n", bs.minute, x, y);
//		fflush(stdout);
//		return true;
//	}

	if (debug)  printf("[%d] Proceding %s to %d,%d ", bs.minute, s, x, y);
	const char *freason;
	bool rv = false;
	if (x < 1) {rv = false; freason = "x < 1";}
	else if ( (y < 1) ) {rv = false; freason = "y < 1";}
	else if (x > bs.xpts - 2) {rv = false; freason = "x > xpts-2";}
	else if (y > bs.ypts - 2) {rv = false; freason = "y > ypts-2";}
	else if (bs.b[x][y] == STAY) {
		rv = true;

	}
	else
		freason = "NO STAY";
	if (debug)  {	
		if (!rv) printf("FAIL - %s\n", freason);
		else printf("SUCCESS\n");
	}
	return rv;
}

void traverse(bstate &bs, int minute, int x, int y, const char *s)
{
	//minute++;
	//outgrid(bs, x, y);
	if (minute >= path_solution) return;
	update(bs, minute);
	if (bmap_time[minute].check[x][y]) {
		//printf("%d,%d at minute %d - been here before\n", x,y,minute);
		return;	// been here before
	}
	bmap_time[minute].check[x][y] = true;

	if (x == bs.xpts - 2 && y == bs.ypts - 1)
	{
		if (path_solution > minute)
		{
			path_solution = minute;
			printf("SOLUTION::  Minute: %d -- at %d,%d\n", minute, x, y);
			fflush(stdout);
		}
		return;
	}
	//if (minutes[x][y] > 0 && bs.minute > minutes[x][y]) return;
	if (debug) printf("[%d] Traversing %s from %d,%d\n", bs.minute, s, x, y);
	//if (minutes[x][y] != -1 && bs.minute > minutes[x][y]) return;
	//update(bs);
	if (procede(bs, x+1, y, "RIGHT")) {traverse(bs, minute+1, x+1, y, "RIGHT"); } // move RIGHT
	if (procede(bs, x, y+1, "DOWN")) { traverse(bs, minute+1, x, y+1, "DOWN"); } // move DOWN
	if (procede(bs, x-1, y, "LEFT")) traverse(bs, minute+1, x-1, y, "LEFT"); // move LEFT
	//if ((x == 1 && y == 1) ) traverse(bs, minute, 1, 0, "UP"); // move UP
	if (procede(bs, x, y-1, "UP")) traverse(bs, minute+1, x, y-1, "UP"); // move UP
	if (procede(bs, x, y, "STAY")) traverse(bs, minute+1, x, y, "STAY");	// STAY
	
}

void solvept1(const char *v, int solution)
{
	bstate basin;
	init(v, basin);

	printf("Input file: %s\n", v);
	fflush(stdout);

	int	num_spaces = 0;
	//update(basin);
	//int save_solution = path_solution;
	//init(v, basin);
	//update(basin);
	//path_solution = save_solution;
	//printf("Trying STAY first\n");
	//traverse(basin, 0, 1, 0, "STAY");
	//printf("Trying DOWN first\n");
	//traverse(basin, 0, 1, 1, "DOWN");
	traverse(basin, 0, 1, 0, "DOWN");
	num_spaces = path_solution;
	update(basin, path_solution);
	outgrid(basin, basin.xpts-2, basin.ypts-1);
	printf("Number of minutes to traverse: %d\n", num_spaces);
	if (solution > 0)
	{
		if (num_spaces != solution)
			printf("ERROR - traverse time does not agree with truth: %d!\n", solution);
		else
			printf("Successfully found minimal traverse time!!\n");
	}
	printf("\n");

}


void solvept2(const char *v, int solution)
{
	bstate basin;
	init(v, basin);

	printf("Input file: %s\n", v);
	outgrid(basin);
	int num_minutes = 10;
	while (num_movements)
	{
		//outgrid(basin);
	}
	printf("Number of minutes to traverse: %d\n", num_minutes);
	if (solution > 0)
	{
		if (solution != num_minutes)
			printf("ERROR - traverse time does not agree with truth: %d!\n", solution);
		else
			printf("Successfully found minimal traverse time!!\n");
	}

}

int main(int argc, char **argv)
{
	//solvept1("ex2.txt", 18);
	path_solution = 50;
	solvept1("ex.txt", 18);
	//path_solution = 3000;
	path_solution = 500;

	solvept1("input.txt", 1000);  // 339 is too hight, 344 is too high, we are currently getting 345
	//solvept2("ex.txt", 20);
	//solvept2("input.txt", 895);
	return 1;
}


