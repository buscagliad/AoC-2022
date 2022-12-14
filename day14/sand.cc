#include <cstring>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cctype>

using namespace std;

#define MAXX 1050
#define MAXY 1050
#define MAXBASINS 1000
char	grid[MAXX][MAXY];
int	scene[MAXX][MAXY];
int xpts = 0;
int ypts = 0;

int xmin = 100000000;
int xmax = 0;
int ymin = 0;
int ymax = 0;
bool first = true;

bool part2 = false;

#define MAXLINE 1000
///////////////////////////////////////////////////////////////////
// DEBUG outgrid outputs the grid to stdout
///////////////////////////////////////////////////////////////////
void outgrid()
{
	printf("X: %d->%d  Y: %d->%d\n", xmin, xmax, ymin, ymax);
	for (int y = ymin; y <= ymax; y++)
	{
		for (int x = xmin; x <= xmax; x++)
		{
			printf("%c", grid[x][y]);
		}
		printf("\n");
	}
	printf("\n");
}


void zerodata()  
{
	for(int i = 0; i < MAXX; i++) 
	    for(int j = 0; j < MAXY; j++)
	    {
			grid[i][j] = '.';
			//scene[i][j] = 0;
		}
	xmin = 100000000;
	xmax = 0;
	ymin = 0;
	ymax = 0;
	xpts = 0;
	ypts = 0;
	
}

#define getnum(c, x)			\
	x = 0;						\
	while (isdigit(*c))			\
	{							\
		x = 10 * x + *c - '0';	\
		c++;					\
	}							\
	c++;

void getxy(char *c, vector<int> &xv, vector<int> &yv)
{
	//printf("LINE: %s", c);
//498,4 -> 498,6 -> 496,6
	int x, y;
	xv.clear();
	yv.clear();
	while (*c && *c != '\n')
	{
		getnum(c, x);
		getnum(c, y);
		xv.push_back(x);
		yv.push_back(y);
		if (strncmp(c, "-> ", 3) == 0) c += 3;
		else return;
	}
	printf("%d,%d", xv[0], yv[0]);
	for (size_t i = 1; i < xv.size(); i++)
	    printf(" -> %d,%d", xv[i], yv[i]);
	printf("\n");
}

void draw(int x1, int y1, int x2, int y2)
{
	if (x1 == x2)
	{
		for (int i = min(y1,y2); i <= max(y1,y2); i++) grid[x1][i] = '#';
	}		
	else if (y1 == y2)
	{
		for (int i = min(x1, x2); i <= max(x1, x2); i++) grid[i][y1] = '#';
	}
	else
	{
		printf("LINE Error (%d,%d) -> (	%d,%d)\n", x1, y1, x2, y2);
	}
}
		
void parseline(char *c)
{
	vector<int> x;
	vector<int> y;
	getxy(c, x, y);
	int xv_max = *max_element(x.begin(), x.end());
	int xv_min = *min_element(x.begin(), x.end());
	int yv_max = *max_element(y.begin(), y.end());
	int yv_min = *min_element(y.begin(), y.end());
	xmax = max(xv_max, xmax);
	ymax = max(yv_max, ymax);
	xmin = min(xv_min, xmin);
	ymin = min(yv_min, ymin);

	for (size_t i = 0; i < x.size() - 1; i++)
	{
		draw(x[i], y[i], x[i+1], y[i+1]);
	}
}

bool drop_sand()
{
	int xs = 500;
	int ys = 0;
	bool done = false;
	if (grid[xs][ys] == 'o') return false;
	while (!done)
	{
		//printf("(xs,ys): %d,%d\n", xs, ys);
		if (ys > ymax) return false;
		if (grid[xs][ys+1] == '.')
		{
			ys++;
		}
		else if (grid[xs-1][ys+1] == '.')
		{
			ys++;
			xs--;
		}
		else if (grid[xs+1][ys+1] == '.')
		{
			ys++;
			xs++;
		}
		else
		{
			grid[xs][ys] = 'o';
			done = true;
		}
	}
	//outgrid();
	if ( (xs < xmin) && (ys > ymax) )
	    return false;
	if ( (xs > xmin) && (ys > ymax) )
	    return false;
	return true;
}

int count_sand()
{
	int count = 0;
	while (drop_sand()) count++;
	return count;
}

void infline()
{
	xmin = 500 - ymax - 4;
	xmax = 500 + ymax + 4;
	for (int i = xmin; i <= xmax; i++)
	    grid[i][ymax + 2] = '#';
	ymax+=2;
}

int count_char(char c)
{
	int count = 0;
	for (int i = xmin; i <= xmax; i++)
	    for (int j = ymin; j <= ymax; j++)
	        if (grid[i][j] == c) count++;

	return count;
}

void solve(const char *v, int true_sand)
{
	FILE *f = fopen(v, "r");

	char c[MAXLINE];
	fgets(c, MAXLINE, f);
	zerodata();
	while (!feof(f))
	{
		parseline(c);
		fgets(c, MAXLINE, f);
	}
	if (part2) infline();
	int num_sand = count_sand();
	printf("Number of 'o': %d\n", count_char('o'));
	//outgrid();
	printf("Input file: %s\n", v);
	printf("Number of units of sand coming to rest : %d\n", num_sand);
	if (true_sand > 0)
	{
		if (true_sand != num_sand)
			printf("ERROR - number does not agree with truth: %d!\n", true_sand);
		else
			printf("Successfully esimated number of trees!!\n");\
	}

	fclose(f);
}

int main(int argc, char **argv)
{
	solve("ex.txt", 24);
	solve("input.txt", 825);
	part2 = true;
	solve("ex.txt", 93);
	solve("input.txt", 26729);
	return 0;
}
