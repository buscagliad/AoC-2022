#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <cstring>
#include <string>

using namespace std;
#define MAXX 1000
#define MAXY 1000
#define MAX_RV 88888

int	s_grid[MAXX][MAXY];
char	g_grid[MAXX][MAXY];

int  g_nx = 0;
int  g_ny = 0;

int  xstart = 0;
int  ystart = 0;
int  xend = 0;
int  yend = 0;

int xpts = 0;
int ypts = 0;
#define NOT_SET -1
int save_first = 0;
#define MAXLINE 1000
///////////////////////////////////////////////////////////////////
// DEBUG outgrid outputs the grid to stdout
///////////////////////////////////////////////////////////////////
void outgrid(char g[MAXX][MAXY], int nx, int ny)
{
	printf("GRID  X: %d  Y: %d\n", nx, ny);
	for (int y = 0; y < ny; y++)
	{
		for (int x = 0; x < nx; x++)
		{
			if (x == xstart && y == ystart) printf("  A ");
			else if (x == xend && y == yend) printf("  Z ");
			else printf("%3c ", g[x][y]);
		}
		printf("\n");
	}
	printf("\n");
}

void outgrid(int g[MAXX][MAXY], int nx, int ny)
{
	printf("GRID  X: %d  Y: %d\n", nx, ny);
	for (int y = 0; y < ny; y++)
	{
		for (int x = 0; x < nx; x++)
		{
			printf("%3d ", g[x][y]);
		}
		printf("\n");
	}
	printf("\n");
}



bool mindval(char g[MAXX][MAXY], int s[MAXX][MAXY], int nx, int ny, int x, int y)
{
	bool	rv = false;
	int	sumv = 0;
	
#define PRINT(a,b) //printf("(%d,%d) %c  --> (%d,%d) %c\n", x, y, g[x][y], a, b, g[a][b])

	if (s[x][y] != NOT_SET)
	{
        if ( (x - 1 >= 0) && (g[x][y] + 1 >= g[x-1][y]) ) // LEFT
        {
            sumv = s[x][y] + 1;
            if ( (sumv < s[x-1][y]) || (s[x-1][y] == NOT_SET) )
            {
                s[x-1][y] = sumv;
                PRINT(x-1,y);
                rv = true;
            }
        }
        if ( (x + 1 < nx) && (g[x][y] + 1 >= g[x+1][y]) ) // RIGHT
        {
            sumv = s[x][y] + 1;
            if ( (sumv < s[x+1][y]) || (s[x+1][y] == NOT_SET) )
            {
                s[x+1][y] = sumv;
                 PRINT(x+1,y);
               rv = true;
            }
        }
        if ( (y - 1 >= 0)  && (g[x][y] + 1 >= g[x][y-1]) ) // ABOVE
        {
            sumv = s[x][y] + 1;
            if ( (sumv < s[x][y-1]) || (s[x][y-1] == NOT_SET) )
            {
                s[x][y-1] = sumv;
                 PRINT(x,y-1);
               rv = true;
            }
        }
        if ( (y + 1 < ny)  && (g[x][y] + 1 >= g[x][y+1]) ) // BELOW
        {
            sumv = s[x][y] + 1;
            if ( (sumv < s[x][y+1]) || (s[x][y+1] == NOT_SET) )
            {
                s[x][y+1] = sumv;
                 PRINT(x,y+1);
               rv = true;
            }
        }
	}
	return rv;
}

int route(char g[MAXX][MAXY], int s[MAXX][MAXY], int nx, int ny)
{
	save_first = g[0][0];
	s[xstart][ystart] = 0;
	bool done = false;
	while (!done)
	{
		done = true;
		for (int x = 0; x < nx; x++)
		{
			for (int y = 0; y < ny; y++)
			{
				if (mindval(g, s, nx, ny, x, y))
				    done = false;
			}
		}
		//outgrid(s, nx, ny);
	}
	return s[xend][yend];
}



void zerodata()  
{
	for(int i = 0; i < MAXX; i++) 
	    for(int j = 0; j < MAXY; j++)
	    {
			g_grid[i][j] = 0;
			s_grid[i][j] = NOT_SET;	// set the sum-path to not be set
		}
	xpts = 0;
	ypts = 0;
	
}

void zero_s_grid()
{
	for(int i = 0; i < MAXX; i++) 
	    for(int j = 0; j < MAXY; j++)
	    {
			s_grid[i][j] = NOT_SET;	// set the sum-path to not be set
		}
}

void parseline(char *c)
{
	int   x = 0;
	int   y = ypts;
	//printf("LINE: %s", c);
	while (*c != '\n')
	{
		g_grid[x][y] = *c;
		if (*c == 'S')
		{
			xstart = x;
			ystart = y;
			g_grid[x][y] = 'a';
		}
		else if (*c == 'E')
		{
			xend = x;
			yend = y;
			g_grid[x][y] = 'z';
		}
		c++;
		x++;
	}
	ypts++;
	xpts = x;
	//printf("xpts: %d  ypts: %d\n", xpts, ypts);
}

void init(const char *v)
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
	
	g_nx = xpts;
	g_ny = ypts;
	//outgrid(g_grid, g_nx, g_ny);
	fclose(f);
}

void solve2(const char *fn, int num_steps)
{
	init(fn);
	int	fewest_steps = g_nx * g_ny;
	int xfs = 0;
	int yfs = 0;
	int nsteps;
	for (int xns = 0; xns < g_nx; xns++)
	{
		for (int yns = 0; yns < g_ny; yns++)
		{
			zero_s_grid();
			if (g_grid[xns][yns] == 'a')
			{
				xstart = xns;
				ystart = yns;
				nsteps = route(g_grid, s_grid, g_nx, g_ny);
				if (nsteps < 1) continue;
				//printf("Starting from %d,%d  takes %d steps\n", xns, yns, nsteps);
				if (nsteps < fewest_steps)
				{
					xfs = xns;
					yfs = yns;
					fewest_steps = nsteps;
				}
			}
		}
	}


	printf("Part 2:: File: %s Number of steps: %d  starting from (%d,%d)\n", fn, fewest_steps, xfs, yfs);
	if (num_steps > 0)
	{
		if (fewest_steps != num_steps)
			printf("ERROR - number of setps does not agree with truth: %d!\n", num_steps);
		else
			printf("Successfully esimated least number of steps!!\n");
	}

}
void solve(int num_steps)
{
	int my_steps = 0;
	my_steps = route(g_grid, s_grid, g_nx, g_ny);
	//outgrid(s_grid, g_nx, g_ny);

	printf("Number of steps: %d\n", my_steps);
	if (num_steps > 0)
	{
		if (my_steps != num_steps)
			printf("ERROR - number of steps does not agree with truth: %d!\n", num_steps);
		else
			printf("Successfully esimated least number of steps!!\n");
	}

}

int main(int argc, char **argv)
{
	init("ex.txt");
	//outgrid(g_grid, xpts, ypts);
	printf("Example for part 1\n");
	solve(31);
	//outgrid(s_grid, xpts, ypts);
	printf("Acutal input for part 1\n");
	init("input.txt");
	solve(339);

	solve2("ex.txt", 29);
	solve2("input.txt", 332);

	return 0;
}
