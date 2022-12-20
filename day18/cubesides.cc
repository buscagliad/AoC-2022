
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <algorithm>

using namespace  std;

#define MAXN 50

bool boulders[MAXN][MAXN][MAXN];
int interior[MAXN][MAXN][MAXN];

bool debug = false;

int	xstartyz[MAXN][MAXN];
int	xendyz[MAXN][MAXN];
int	ystartxz[MAXN][MAXN];
int	yendxz[MAXN][MAXN];
int	zstartxy[MAXN][MAXN];
int	zendxy[MAXN][MAXN];

typedef struct { int x,y,z; } xyz;
int xmin, xmax, ymin, ymax, zmin, zmax;
typedef enum { ANY, UP, DOWN, LEFT, RIGHT, FORWARD, BACK } dir_e;



#define ROCK(x,y,z) (boulders[x][y][z] ? 1 : 0)

int airneighbor(int x, int y, int z)
{
	if (ROCK(x,y,z)) return 0;
	return  ROCK(x,y,z-1) + ROCK(x,y,z+1) + 
			ROCK(x-1,y,z) + ROCK(x+1,y,z) + 
			ROCK(x,y+1,z) + ROCK(x,y-1,z);
}

void init()
{
	for (int i = 0; i < MAXN; i++)
		for (int j = 0; j < MAXN; j++)
		{
			zstartxy[i][j] = MAXN;
			zendxy[i][j] = 0;
			ystartxz[i][j] = MAXN;
			yendxz[i][j] = 0;
			xstartyz[i][j] = MAXN;
			xendyz[i][j] = 0;
			for (int k = 0; k < MAXN; k++)
			{
			    boulders[i][j][k] = false;
			    interior[i][j][k] = 0;
			}
		}
}

bool MK(int X[MAXN][MAXN][MAXN], int i, int j, int k, int M, int &cnt)
{
	if ( !boulders[i][j][k] && (X[i][j][k] >= 0) && (X[i][j][k] < M) )
	{
		X[i][j][k] += M;
		cnt++;
		return true;
	}
	return false;
}

int mark(int in[MAXN][MAXN][MAXN], int i, int j, int k, int M)
{
	int cnt = 0;
	if (in[i][j][k] >= 0)
	{
		MK(in, i, j, k, M, cnt);
		if (MK(in, i+1, j, k, M, cnt)) MK(in, i+1, j, k, M, cnt);
		if (MK(in, i-1, j, k, M, cnt)) MK(in, i-1, j, k, M, cnt);
		if (MK(in, i, j+1, k, M, cnt)) MK(in, i, j+1, k, M, cnt);
		if (MK(in, i, j-1, k, M, cnt)) MK(in, i, j-1, k, M, cnt);
		if (MK(in, i, j, k+1, M, cnt)) MK(in, i, j, k+1, M, cnt);
		if (MK(in, i, j, k-1, M, cnt)) MK(in, i, j, k-1, M, cnt);
	}
	return cnt;
}

int been_here[MAXN][MAXN][MAXN];
bool escaped = false;

void clear_bh()
{
	for (int i = 0; i < MAXN; i++)
	{
		for (int j = 0; j < MAXN; j++)
		{
			for (int k = 0; k < MAXN; k++)
			{
				been_here[i][j][k] = false;
			}
		}
	}
	escaped = false;
}

bool outside( int i, int j, int k )
{
	if ( (k < zstartxy[i][j]) ||
		 (k > zendxy[i][j])   ||
		 (j < ystartxz[i][k]) ||
		 (j > yendxz[i][k])   ||
		 (i < xstartyz[j][k]) ||
		 (i > xendyz[j][k])  ) return true;
	return false;
}


bool MOVE(int i, int j, int k)
{
	if (escaped) return false;
	if ( !boulders[i][j][k] && !been_here[i][j][k] )
	{
		been_here[i][j][k] = true;
		if (outside(i,j,k)) escaped = true;
		return true;
	}
	return false;	
}

bool towater(int i, int j, int k, bool clr = false)
{
	if (clr) clear_bh();
	if (boulders[i][j][k]) return false;
	{
		if (MOVE(i+1, j, k)) towater(i+1, j, k);
		if (MOVE(i-1, j, k)) towater(i-1, j, k);
		if (MOVE(i, j+1, k)) towater(i, j+1, k);
		if (MOVE(i, j-1, k)) towater(i, j-1, k);
		if (MOVE(i, j, k+1)) towater(i, j, k+1);
		if (MOVE(i, j, k-1)) towater(i, j, k-1);
	}
	return escaped;
}
int cntgth(int in[MAXN][MAXN][MAXN], int m, int &count, int &sum)
{
	count = 0;
	sum = 0;
	for (int i = 0; i < MAXN; i++)
	{
		for (int j = 0; j < MAXN; j++)
		{
			for (int k = 0; k < MAXN; k++)
			{
				if ( (in[i][j][k] >= m) && in[i][j][k] < m + 10)
				{
					count++;
					sum += in[i][j][k] - m;
					if (debug) printf("Mark: %d   (%d,%d,%d) @ %d\n", m, i, j, k, in[i][j][k]);
				}
			}
		}
	}
	return count;
}

void fillwater(int in[MAXN][MAXN][MAXN])
{
	for (int i = 0; i < MAXN; i++)
	{
	    for (int j = 0; j < MAXN; j++)
		{
			for (int k = 0; k < MAXN; k++)
			{
				// is i,j,k an interior point?
				if ( ( (xstartyz[j][k] < i) && (i < xendyz[j][k]) ) &&
					 ( (ystartxz[i][k] < j) && (j < yendxz[i][k]) ) &&
					 ( (zstartxy[i][j] < k) && (k < zendxy[i][j]) ) )
				{
							
					if (!boulders[i][j][k])
					{
						int	 n = airneighbor(i,j,k);
						in[i][j][k] = n;
					}
					else
					{
						in[i][j][k] = -1;
					}
				}
				else in[i][j][k] = -2;
			}
		}
	}
	//
	// we will fill in in[i][j][k] with MARKER (10) 
	// to see if it 'fills'
	//doagain:
	for (int i = 0; i < MAXN; i++)
	{
	    for (int j = 0; j < MAXN; j++)
		{
			for (int k = 0; k < MAXN; k++)
			{
				if (in[i][j][k] >= 0)
			    {
					if (towater(i, j, k, true))
					{
						in[i][j][k] = -1;
			            if (debug) printf("%d,%d,%d  is connected to water\n", i, j, k);
					}
			    }
			}
		}
	}

	int MARK = 10;
	for (int sides = 6; sides >= 0; sides--)
	{
		for (int i = 0; i < MAXN; i++)
		{
			for (int j = 0; j < MAXN; j++)
			{
				for (int k = 0; k < MAXN; k++)
				{
					if ( (in[i][j][k] == sides) )
					{
						int count, sum;
						if (debug) printf("%d,%d,%d checking mark\n", i, j, k);
						int nm = mark(in, i, j, k, MARK);
						if (debug) printf("   -- %d changed\n", nm);
						cntgth(in, MARK, count, sum);
						if (debug) printf("MARK: %d  count: %d  sum: %d\n", MARK, count, sum);
						MARK += 10;
					}
				}
			}
		}
	}
	for (int i = 0; i < MAXN; i++)
	{
	    for (int j = 0; j < MAXN; j++)
		{
			for (int k = 0; k < MAXN; k++)
			{
				if (in[i][j][k] >= 10)
				{
				    if (debug) printf("%d,%d,%d SETTING boulder mark\n", i, j, k);
				    boulders[i][j][k] = true;;
			    }
		    }
		}
	}
	
}


#define getnum(c, x, m, M)		\
	x = 0;						\
	while (isdigit(*c))			\
	{							\
		x = 10 * x + *c - '0';	\
		c++;					\
	}							\
	m = min(x, m);				\
	M = max(x, M);				\
	c++;




#define TRUE(s) { if (debug) printf("%s\n", s); return true; }
#define FALSE(s)   { if (debug) printf("%s\n", s); return false; }




void startstop(int x, int y, int z)
{
	if (zstartxy[x][y] > z) zstartxy[x][y] = z;
	if (zendxy[x][y]   < z) zendxy[x][y] = z;
	if (ystartxz[x][z] > y) ystartxz[x][z] = y;
	if (yendxz[x][z]   < y) yendxz[x][z] = y;
	if (xstartyz[y][z] > x) xstartyz[y][z] = x;
	if (xendyz[y][z]   < x) xendyz[y][z] = x;
}
	
bool getxyz(FILE *f, int &x, int &y, int &z)
{
	char st[200];
	char *c = st;
	fgets(c, 200, f);
	if (feof(f)) return false;
	while (*c && *c != '\n')
	{
		getnum(c, x, xmin, xmax);
		getnum(c, y, ymin, ymax);
		getnum(c, z, zmin, zmax);
		startstop(x,y,z);

	}
	return true;
}

void set(const char *fn)
{
	FILE *f = fopen(fn, "r");
	int x;
	int y;
	int z;
	bool first = true;
	
	while (getxyz(f,x,y,z))
	{
		if (first)
		{
			xmin = xmax = x;
			ymin = ymax = y;
			zmin = zmax = z;
			first = false;
		}
	    boulders[x][y][z] = true;
		//printf("%d,%d,%d\n", x, y, z);
	    
	}
	
	fclose(f);
	fflush(stdout);
}

//
// for every 
int count(bool b[MAXN][MAXN][MAXN])
{
	int transitions = 0;
	int	x, y, z;
	// for each (x, y) from (0..MAXN,0..MAXN) Count number of boolean transitions
	// for z = 0 to to MAX 
	for (x = xmin; x <= xmax; x++)
	{
		for (y = ymin; y <= ymax; y++)
		{
			bool tx = false;
			for (z = zstartxy[x][y]; z <= zendxy[x][y]; z++)
			{
				if (b[x][y][z] ^ tx) transitions++;
				tx = b[x][y][z];
			}
			if (tx)  transitions++;
		}
	}
	// for each (y, z) from (0..MAXN,0..MAXN) Count number of boolean transitions
	// for x = 0 to to MAX 
	for (z = zmin; z <= zmax; z++)
	{
		for (y = ymin; y <= ymax; y++)
		{
			bool tx = false;
			for (x = xstartyz[y][z]; x <= xendyz[y][z]; x++)
			{
				if (b[x][y][z] ^ tx) transitions++;
				tx = b[x][y][z];
			}
			if (tx)  transitions++;
		}
	}
	// for each (x, z) from (0..MAXN,0..MAXN) Count number of boolean transitions
	// for y = 0 to to MAX 
	for (x = xmin; x <= xmax; x++)
	{
		for (z = zmin; z <= zmax; z++)
		{
			bool tx = false;
			for (y = ystartxz[x][z]; y <= yendxz[x][z]; y++)
			{
				if (b[x][y][z] ^ tx) transitions++;
				tx = b[x][y][z];
			}
			if (tx)  transitions++;
		}
	}
	return transitions;
}


int dist(xyz a, xyz b)
{
	return abs(a.x-b.x)+abs(a.y-b.y)+abs(a.z-b.z);
}



int solvept1(const char *fn, int answer)
{
	init();
	set(fn);
	int cnt = count(boulders);
	printf("Number of open sides is %d\n", cnt);
	if (cnt == answer) printf("Solution is correct!!!\n");
	else printf("ERROR - solution is not correct\n");
	return 1;
}

int findi(int a[MAXN][MAXN][MAXN], int n)
{
	int count = 0;
	for (int i = 0; i < MAXN; i++)
	{
	    for (int j = 0; j < MAXN; j++)
		{
			for (int k = 0; k < MAXN; k++)
			{
				if (a[i][j][k] == n) count++;
			}
		}
	}
	return count;
}	

int solvept2(const char *fn, int answer)
{
	init();
	set(fn);
	//srwater(0,0,0);
	int total_surface_before_fillwater = count(boulders);
	fillwater(interior);

	int total_surface = count(boulders);
	int int_surface = 0;//count(interior);


	int cnt = total_surface - int_surface;
	printf("Total surface area before fillwater %d\n", total_surface_before_fillwater);
	printf("Total surface area: %d  Internal surface area is %d\n",
		total_surface, int_surface);
	printf("Actual surface area is: %d\n", cnt);
	if (cnt == answer) printf("Solution is correct!!!\n");
	else printf("ERROR - solution is not correct\n");
	fflush(stdout);
	return 1;
}

int main()
{
	solvept1("ex.txt", 64);
	solvept1("input.txt", 4300);
	
	solvept2("ex.txt", 58);
	solvept2("input.txt", 2490);
	
}
