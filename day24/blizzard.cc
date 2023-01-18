#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <cstring>
#include <string>

using namespace std;
#define MAXX 155
#define MAXY 23

typedef enum {STAY=0, UP=1, RIGHT=2, DOWN=4, LEFT=8, WALL=64} dir_e;


int num_movements = 0;

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
	int  x;
	int  y;
	int  minute;
} bstate;

bstate	basin;
int		minutes[MAXX][MAXY];		// number of minutes to get to x,y		


int xpts = 0;
int ypts = 0;

char getbs(bstate &bs, int i, int j)
{
	if (i <= 0) i = xpts - 2;
	else if (i >= xpts - 1) i = 1;
	if (j <= 0) j = ypts - 2;
	else if (j >= ypts - 1) j = 1;
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

void blizzard_update(bstate &bs)
{
	bstate nbs = bs;
	nbs.minute++;
	for (int i = 1; i < xpts - 1; i++)
	{
		for (int j = 1; j < ypts - 1; j++)
		{
			nbs.b[i][j] = step(bs, i, j);
		}
	}
	bs = nbs;
}

bool update(bstate &bs)
{
	blizzard_update(bs);
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



void outgrid(bstate &b, bool EWNS = false)
{

	printf("Minute: %d  [%d,%d]\n", b.minute, xpts, ypts);
	for (int y = 0; y < ypts; y++)
	{
		for (int x = 0; x < xpts; x++)
		{
			if (EWNS) printf("%c", dirChar(b.b[x][y]));
			else printf("%c", exChar(b.b[x][y]));
		}
		printf("\n");
	}
	printf("\n");
}


#define MAXLINE 1000
///////////////////////////////////////////////////////////////////
// DEBUG outgrid outputs the grid to stdout
///////////////////////////////////////////////////////////////////



void clear(bstate &b)
{
	for (int i = 0; i < MAXX; i++)
	    for (int j = 0; j < MAXY; j++)
	    {
	        b.b[i][j] = STAY;
	    }
}
			


char letter = 'A';


bool parseline(FILE *f)
{
	char s[MAXLINE];

	fgets(s, MAXLINE, f);
	if (feof(f)) return false;
	int slen = strlen(s);
	if (xpts < slen-1) { xpts = slen-1; printf("xpts = %d\n", xpts); }
	for (int i = 0; i < slen; i++)
	{
		switch(s[i])
		{
			case '#': basin.b[i][ypts] = WALL;  break;
			case '>': basin.b[i][ypts] = RIGHT; break;
			case '<': basin.b[i][ypts] = LEFT;  break;
			case '^': basin.b[i][ypts] = UP;    break;
			case 'v': basin.b[i][ypts] = DOWN;  break;
			case '.': basin.b[i][ypts] = 0;     break;
			case '\n': continue;
		}

	}
	ypts++;
	return true;
}
	
void init(const char *fn)
{
	clear(basin);
	xpts = 0; 
	ypts = 0;
	FILE *f = fopen(fn, "r");
	while (parseline(f));
	fclose(f);
}

bool procede(bstate &bs, int x, int y)
{
	printf("Proceding to %d,%d ", x, y);
	const char *freason;
	bool rv = false;
	if (x < 1) {rv = false; freason = "x < 1";}
	else if (y < 1) {rv = false; freason = "y < 1";}
	else if (x > xpts - 2) {rv = false; freason = "x > xpts-2";}
	else if (y > ypts - 2) {rv = false; freason = "y > ypts-2";}
	else if (bs.b[x][y] == STAY) {
		rv = true;
		if (minutes[x][y] == -1) minutes[x][y] = bs.minute;
		if (minutes[x][y] > bs.minute)
		{
			minutes[x][y] = bs.minute;
		}

	}
	else
		freason = "NO STAY";
		
	if (!rv) printf("FAIL - %s\n", freason);
	else printf("SUCCESS\n");
	return rv;
}

void traverse(bstate bs, int x, int y)
{
	printf("[%d] Traversing from %d,%d\n", bs.minute, x, y);
	//if (minutes[x][y] != -1 && bs.minute > minutes[x][y]) return;
	update(bs);
	if (procede(bs, x+1, y)) traverse(bs, x+1, y); // move RIGHT
	if (procede(bs, x-1, y)) traverse(bs, x-1, y); // move LEFT
	if (procede(bs, x, y+1)) traverse(bs, x, y+1); // move DOWN
	if (procede(bs, x, y-1)) traverse(bs, x, y-1); // move UP
	traverse(bs, x, y);	// STAY
}

void solvept1(const char *v, int solution)
{
	init(v);

	printf("Input file: %s\n", v);

	int	num_spaces = 0;
	update(basin);
	if (procede(basin, 1, 1))
		traverse(basin, 1, 1);
	else
		printf("ERROR initialization\n");
	num_spaces = minutes[xpts-1][ypts-2];
	printf("Number of minutes to traverse: %d\n", num_spaces);
	if (solution > 0)
	{
		if (num_spaces != solution)
			printf("ERROR - traverse time does not agree with truth: %d!\n", solution);
		else
			printf("Successfully found minimal traverse time!!\n");
	}

}


void solvept2(const char *v, int solution)
{
	init(v);

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
	solvept1("ex.txt", 18);
	//solvept1("input.txt", 1000);
	//solvept2("ex.txt", 20);
	//solvept2("input.txt", 895);
	return 1;
}


