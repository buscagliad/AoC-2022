#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <cstring>
#include <string>
#include <queue>

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

void clearfull(vector<bmap_t> &bm)
{
	for (size_t i = 0; i < bm.size(); i++)
		clear(bm[i]);
}
							
int count(bmap_t &b)
{
	int cnt = 0;
	for (int i = 0; i < MAXX; i++)
	    for (int j = 0; j < MAXY; j++)
			if (b.check[i][j])cnt++;
	return cnt;
}
struct pos_t {
	int	x;	// x position
	int y;	// y position
	int t;	// time in minutes
	pos_t(int  _x, int  _y, int  _t) : x(_x), y(_y), t(_t){};
	pos_t incx() { return pos_t (x+1,y,t+1); };
	pos_t decx() { return pos_t (x-1,y,t+1); };
	pos_t incy() { return pos_t (x,y+1,t+1); };
	pos_t decy() { return pos_t (x,y-1,t+1); };
	pos_t stay() { return pos_t (x,y,t+1); };
	bool operator == (pos_t &rhs) { return x == rhs.x && y == rhs.y; };
	void out() { printf("(%d,%d) @ %d minute\n", x, y, t);  };
};

typedef enum {STAY=0, UP=1, RIGHT=2, DOWN=4, LEFT=8, WALL=64} dir_e;


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

pos_t g_goal(0,0,0);

void init(const char *fn, bstate &basin)
{
	clear(basin);

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
	pos_t	gg = {basin.xpts - 2, basin.ypts - 1, 0};
	g_goal = gg;
}

bool procede(bstate &bs, pos_t &p, const char *s)
{
	if (p.x == 1 && p.y == 0) return true;
	if (p.x == bs.xpts - 2 && p.y == bs.ypts - 1) return true;

	if (debug)  printf("[%d] Proceding %s to %d,%d ", bs.minute, s, p.x, p.y);
	const char *freason;
	bool rv = false;
	if (p.x < 1) {rv = false; freason = "x < 1";}
	else if ( (p.y < 1) ) {rv = false; freason = "y < 1";}
	else if (p.x > bs.xpts - 2) {rv = false; freason = "x > xpts-2";}
	else if (p.y > bs.ypts - 2) {rv = false; freason = "y > ypts-2";}
	else if (bs.b[p.x][p.y] == STAY) {
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


bool explored(pos_t p)
{
	return bmap_time[p.t].check[p.x][p.y];
}


//
// https://en.wikipedia.org/wiki/Breadth-first_search
//    tried depth first search, but in drilled down too fast and did not
//    seem to come back to find better solutions
//  procedure BFS(G, root) is
//   2      let Q be a queue
//   3      label root as explored
//   4      Q.enqueue(root)
//   5      while Q is not empty do
//   6          v := Q.dequeue()
//   7          if v is the goal then
//   8              return v
//   9          for all edges from v to w in G.adjacentEdges(v) do
//  10              if w is not labeled as explored then
//  11                  label w as explored
//  12                  w.parent := v
//  13                  Q.enqueue(w)
pos_t bfsearch(bstate &bs, pos_t p, pos_t goal)
{
	queue<pos_t> q;		// (2) q will be our q for searching
	// max_time too large x*y - try 4 * max(x,y)
	const int max_time = 4 * max(bs.xpts, bs.ypts) + p.t;
	update(bs, p.t);	//     updates state field and zeros bmap_time for this time
// (3) 
// (4) add root to q 
	q.push(p); 
	while (!q.empty())	// (5) while Q is not empty do
	{
		pos_t	v = q.front();		//   (6)  v := Q.dequeue()
		q.pop();
		// Check if pos_t has been 'explored', if so, continue
		if (explored(v)) {
			continue;
		}
		bmap_time[v.t].check[v.x][v.y] = true;
		if (v == goal) return  v;	//   (7) if v is the goal then return v (8)
		// NEED TO LIMIT TIME - say bs.xpts * bs.ypts maximum
		if (v.t > max_time) continue;
//
//   9          for all edges from v to w in G.adjacentEdges(v) do
//
		// it will add to the Q and mark explored
//  10              if w is not labeled as explored then
//  11                  label w as explored
//  12                  w.parent := v	// NOT NEEDED
//  13                  Q.enqueue(w)
		update(bs, v.t + 1);
		pos_t pyp1 = v.incy();
		pos_t pym1 = v.decy();
		pos_t pxp1 = v.incx();
		pos_t pxm1 = v.decx();
		pos_t pstay = v.stay();

		if (procede(bs, pyp1, "DOWN"))  q.push(pyp1);  	// move DOWN
		if (procede(bs, pxp1, "RIGHT")) q.push(pxp1); 	// move RIGHT
		if (procede(bs, pym1, "UP"))    q.push(pym1);	// move UP
		if (procede(bs, pxm1, "LEFT"))  q.push(pxm1);	// move LEFT
		if (procede(bs, pstay, "STAY")) q.push(pstay); 	// STAY
	}
	return {-1,-1,-1};
}



void solvept1(const char *v, int solution)
{
	bstate basin;
	init(v, basin);

	printf("Input file: %s\n", v);
	fflush(stdout);
	
	pos_t bfs_solution = bfsearch(basin, pos_t(1,0,0), g_goal);
	printf("x,y,t::  %d,%d,%d\n", bfs_solution.x, bfs_solution.y, bfs_solution.t);
	printf("Number of minutes to traverse: %d\n", bfs_solution.t);
	if (solution > 0)
	{
		if (bfs_solution.t != solution)
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
	fflush(stdout);
	pos_t bfs_solution = bfsearch(basin, pos_t(1,0,0), g_goal);
	bfs_solution.out();
	// when bfsearch returns, the state of the bf is good, just need to zero the 
	// bmap_time, then call bfsearch again, with the 
	clearfull(bmap_time);
	bfs_solution = bfsearch(basin, bfs_solution, pos_t(1,0,0));
	clearfull(bmap_time);
	bfs_solution = bfsearch(basin, pos_t(1,0,bfs_solution.t), g_goal);
	int num_minutes = bfs_solution.t;

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
	solvept1("ex.txt", 18);
	solvept1("input.txt", 326);
	

	solvept2("ex.txt", 54);
	solvept2("input.txt", 976);
	return 1;
}


