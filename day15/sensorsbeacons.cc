#include <cstring>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cctype>

using namespace std;

typedef struct {
	int	sx;
	int sy;
	int bx;
	int by;
	bool used;
} senbcn;

 bool debug = false;



#define MAX_SR 10000000
int OFFSET = 2000000;
int MAX_XY = 20;	// only used for part 2
char senrow[MAX_SR];

bool part2 = false;

int mandist(int x1, int y1, int x2, int y2)
{
	return abs(x1 - x2) + abs(y1 - y2);
}

#define empty(x) ( ((x) != 'B') && ((x) != 'S') )
bool find(int sx, int sy, int bx, int by, int row)
{
	// y = row
	if (by == row) senrow[OFFSET+bx] = 'B';
	if (sy == row) senrow[OFFSET+sx] = 'S';
	int dist = abs(sx - bx) + abs(sy - by);
	int rowdist = abs(sy - row);
	if (rowdist > dist) return false;
	// otherwise, xa and xb exist
	// abs(x - sx) = dist - rowdist
	// xa = sx - rowdist  
	// xb = sx + rowdist   
	int xa = sx - (dist - rowdist);
	int xb = sx + (dist - rowdist);
	//printf("S:(%d,%d)  B:(%d,%d): %d  xrange: %d - %d\n", sx, sy, bx, by, mandist(sx,sy, bx, by),
	//					xa, xb);
	//printf("D: (%d,%d) -> S: %d  D: (%d,%d) -> S: %d\n",
	//	xa, row, mandist(xa, row, sx, sy), xb, row, mandist(xb, row, sx, sy));
	if (part2) xa = max(xa, 0);
	if (part2) xb = min(xb, MAX_XY);
	for (int i = xa; i <= xb; i++)
	{
		if (empty(senrow[OFFSET+i])) senrow[OFFSET+i] = '#';
	}
	
	return true;
}

void init()
{
	memset(senrow, ' ', MAX_SR);
}

int intv(char **c)
{
	char *eq = strchr(*c, '=');
	eq++;
	int v = atoi(eq);
	while (*eq != ' ' && *eq != '\n')
	{
		eq++;
	}
	*c = eq;
	return v;
}

int find_char(char *a, char c)
{
	for (int i = 0; i <= MAX_XY; i++)
	{
	    if (senrow[i] == c)
	    {
			return i;
		}
	}
	//printf("First x: %d   Last x: %d\n", firstx, lastx);

	return -1;
}

int count_char(char *a, char c)
{
	int count = 0;
	int firstx, lastx;
	for (int i = 0; i < MAX_SR; i++)
	{
	    if (senrow[i] == c)
	    {
			if (count == 0) { firstx = i - OFFSET; lastx = firstx-1; }
	        count++;
	        lastx++;
		}
	}
	//printf("First x: %d   Last x: %d\n", firstx, lastx);

	return count;
}
void parse(char *s, int nrow)
{
//	Sensor at x=17, y=20: closest beacon is at x=21, y=22
	int sx = intv(&s);
	int sy = intv(&s);
	int bx = intv(&s);
	int by = intv(&s);
	//printf("Sensor: (%d,%d)   Beacon: (%d,%d)\n", sx, sy, bx, by);
	find(sx, sy, bx, by, nrow);
}

void solve(const char *fn, int nrow, int exval, bool output = true)
{
	FILE *f = fopen(fn, "r");
	char inp[100];
	init();
	while (!feof(f))
	{
		fgets(inp, 100, f);
		if (feof(f)) break;
		parse(inp, nrow);
	}
	int notbcn = count_char(senrow, '#');
	fclose(f);
	if (output) printf("Not beacon at row %d is %d postions\n", nrow, notbcn);
}

bool findbeacon(const char *fn, int row)
{ return true; }


void initbcn(const char *fn, vector<senbcn> &s)
{
		FILE *f = fopen(fn, "r");
	char inl[100];
	char *inp = inl;
	init();
	while (!feof(f))
	{
		fgets(inl, 100, f);
		inp = inl;
		if (feof(f)) break;
		int sx = intv(&inp);
		int sy = intv(&inp);
		int bx = intv(&inp);
		int by = intv(&inp);
		senbcn sb = {sx, sy, bx, by};
		//printf("Adding sensor %d,%d  beacon %d,%d\n", sx, sy, bx, by);
		s.push_back(sb);
	}
	fclose(f);
}
#define empty(x) ( ((x) != 'B') && ((x) != 'S') )
bool findnext(int sx, int sy, int bx, int by, int &x, int row)
{
	// y = row
	int dist = abs(sx - bx) + abs(sy - by);
	int rowdist = abs(sy - row);
	if (rowdist > dist) return false;
	// otherwise, xa and xb exist
	// abs(x - sx) = dist - rowdist
	// xa = sx - rowdist  
	// xb = sx + rowdist   
	int xa = sx - (dist - rowdist);
	int xb = sx + (dist - rowdist);
	if (x >= xa  &&  x <= xb) { x = xb; return true; }

	return false;
}


bool solvebcn(vector<senbcn> &s, int maxx,  int &x, int y)
{
	for (size_t i = 0; i < s.size(); i++)
	    s[i].used = false;
	bool f;
	bool done = false;
	while (!done)
	{
		done = true;
		for (size_t t = 0; t < s.size(); t++)
		{
			if (s[t].used) { continue; }
			f = findnext(s[t].sx, s[t].sy, s[t].bx, s[t].by, x, y);
			if (f) {
				//printf("x=%d  y=%d\n", x, y);
				s[t].used = false;
				x++;	// go to next square
				done = false;
			}
		}
	}
	return x < maxx;
}	

int main()
{
	solve("ex.txt", 10, 26);
	solve("input.txt", 2000000, 26);
	part2 = true;
	OFFSET = 0;
	MAX_XY = 20;
	vector<senbcn> sbxy;
	initbcn("ex.txt", sbxy);
	int ncol = 0;
	for (int i = 0; i <= MAX_XY; i++)
	{
		ncol = 0;
		if (solvebcn(sbxy, MAX_XY, ncol, i))
		{
		    long ans = (long) ncol * MAX_XY + i;
		    printf("At row: %d   (%d,%d) is the beacon - ans: %ld\n", i, ncol, i, ans);
		    break;
		}
		//printf("i = %d\n", i); fflush(stdout);
	}
	part2 = true;
	OFFSET = 0;
	MAX_XY = 4000000;
	sbxy.clear();
	initbcn("input.txt", sbxy);
	ncol = 0;
	for (int i = 0; i <= MAX_XY; i++)
	{
		ncol = 0;
		if (solvebcn(sbxy, MAX_XY, ncol, i))
		{
		    long ans = (long) ncol * MAX_XY + i;
		    printf("\nAt row: %d   (%d,%d) is the beacon - ans: %ld\n", i, ncol, i, ans);
		    break;
		}
		//printf("i = %d\n", i); fflush(stdout);
		if (i % 50000 == 0) { printf("."); fflush(stdout); }
	}
}
		
