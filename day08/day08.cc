#ifdef PROBLEM




#endif

#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <cstdlib>

using namespace std;

#define MAXX 200
#define MAXY 200
#define MAXBASINS 1000
int	grid[MAXX][MAXY];
int	scene[MAXX][MAXY];
int xpts = 0;
int ypts = 0;
bool is_vis(int x, int y);

#define MAXLINE 1000
///////////////////////////////////////////////////////////////////
// DEBUG outgrid outputs the grid to stdout
///////////////////////////////////////////////////////////////////
void outgrid()
{
	printf("GRID  X: %d  Y: %d\n", xpts, ypts);
	for (int y = 0; y < ypts; y++)
	{
		for (int x = 0; x < xpts; x++)
		{
			printf("%d%c ", grid[x][y], is_vis(x, y)?'*': ' ');
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
			grid[i][j] = 0;
			scene[i][j] = 0;
		}
	xpts = 0;
	ypts = 0;
	
}

void parseline(char *c)
{
	int   x = 0;
	int   y = ypts;
	while (*c != '\n')
	{
		grid[x++][y] = *c - '0';
		c++;
	}
	ypts++;
	xpts = x;
}



bool is_vis(int x, int y)
{
	bool left = true, right = true, up = true, down = true;
	for (int i = 0; i < x; i++)	// left
	    if (grid[x][y] <=  grid[i][y]) left = false;
	for (int i = x; i < xpts - 1; i++)	// right
	    if (grid[x][y] <=  grid[i+1][y]) right = false;
	for (int i = 0; i < y; i++)	// up
	    if (grid[x][y] <= grid[x][i]) up = false;
	for (int i = y; i < ypts - 1; i++)	// down
	    if (grid[x][y] <= grid[x][i+1]) down = false;
	bool cansee = down || up || left || right;
	//printf("x: %d y: %d  v: %d  vis: %s  l:%d r:%d r:%d d:%d\n", x, y, grid[x][y], cansee?"YES":"NO", left, right, up, down);
	return cansee;
}

// markvistrees will mark a tree as -v if it is visible
// we use abs(v) for all tests
int markvistrees()
{
	int sum = 0;
	for (int y = 0; y < ypts; y++)
	{
		for (int x = 0; x < xpts; x++)
		{
			if (is_vis(x, y)) sum++;
		}
	}
	return sum;
}

int comp_scene(int x, int y)
{
	int left = 0, right = 0, up = 0, down = 0;
	for (int i = x-1; i >= 0; i--)	// left
	    if (grid[x][y] > grid[i][y]) left++;
	    else if (grid[x][y] <= grid[i][y]) { left++; break; }
	    else break;
	for (int i = x; i < xpts - 1; i++)	// right
	    if (grid[x][y] >  grid[i+1][y]) right++;
	    else if (grid[x][y] <= grid[i+1][y]) { right++; break; }
	    else break;
	for (int i = y-1; i >= 0; i--)	// up
	    if (grid[x][y] > grid[x][i]) up++;
	    else if (grid[x][y] <= grid[x][i]) { up++; break; }
	    else break;
	for (int i = y; i < ypts - 1; i++)	// down
	    if (grid[x][y] > grid[x][i+1]) down++;
	    else if (grid[x][y] <= grid[x][i+1]) { down++; break; }
	    else break;
	int score = down * up * left * right;
	//printf("x: %d y: %d  v: %d    l:%d r:%d r:%d d:%d\n", x, y, grid[x][y], left, right, up, down);
	return score;
}

// markvistrees will mark a tree as -v if it is visible
// we use abs(v) for all tests
int markescene()
{
	int max = 0;
	for (int y = 0; y < ypts; y++)
	{
		for (int x = 0; x < xpts; x++)
		{
			int cs = comp_scene(x, y);
			scene[x][y] = comp_scene(x, y);
			if (cs > max) max = cs;
		}
	}
	return max;
}


void solve(const char *v, int xtrees, int xscore)
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
	int vtrees = markvistrees();
	printf("Input file: %s\n", v);
	printf("Num visable trees: %d\n", vtrees);
	if (xtrees > 0)
	{
		if (xtrees != vtrees)
			printf("ERROR - number of trees does not agree with truth: %d!\n", xtrees);
		else
			printf("Successfully esimated number of trees!!\n");\
	}
	int bscore = markescene();
	printf("Biggest score: %d\n", bscore);
	if (xscore > 0)
	{
		if (xscore != bscore)
			printf("ERROR - best viewing score does not agree with truth: %d!\n", xscore);
		else
			printf("Successfully esimated number of trees!!\n");\
	}	

	fclose(f);
}

int main(int argc, char **argv)
{
	solve("test.txt", 21, 8);
	solve("input.txt", 1829, 291840);
	return 0;
}
