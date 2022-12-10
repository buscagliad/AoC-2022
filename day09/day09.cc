#ifdef PROBLEM




#endif

#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <cstdlib>

using namespace std;

int Hx = 0;
int Hy = 0;
int Tx = 0;
int Ty = 0;

typedef struct {
	int	x;
	int y;
} xy_pair;

xy_pair R[10];

void display()
{
	for(int i = 0; i < 10; i++)
	{
	    printf("%d:(%d,%d) ", i, R[i].x, R[i].y);
	}
	printf("\n");
}

vector<xy_pair> xyp;

bool addpos(int x, int y)
{
	for (size_t i = 0; i < xyp.size(); i++)
	{
		if ( (x == xyp[i].x) && (y == xyp[i].y) ) return false;
	}
	xy_pair p={x, y};
	xyp.push_back(p);
	//printf("%lu adding %d,%d\n", xyp.size(), x, y);
	return true;
}

#define MAXLINE 1000
///////////////////////////////////////////////////////////////////
// DEBUG outgrid outputs the grid to stdout
///////////////////////////////////////////////////////////////////



void parseline(char *c, char &m, int &p)
{
	m = *c;
	c+=2;
	p = atoi(c);
	//printf("----  %c   %d\n", m, p);
}

int block(int x1, int y1, int x2, int y2)
{
	return (abs(x1 - x2) + abs(y1 - y2));
}

bool touching(int x1, int y1, int x2, int y2, int &dx, int &dy)
{
	dx = x2 - x1;
	dy = y2 - y1;
	if ( abs(x1-x2) == 1 && abs(y1-y2) == 1) return true;
	if ( (x1 == x2) && abs(y1 - y2) < 2) return true;
	if ( (y1 == y2) && abs(x1 - x2) < 2) return true;
	return false;
}

bool touchingx(int x1, int y1, int &x2, int &y2)
{
	// if (x1,y1) and (x2, y2) are 'touching' do nothing
	if ( abs(x1-x2) <= 1 && abs(y1 - y2) <= 1) return false;
	// do i need to move diagonally?
	if (abs(x1 - x2) == 2 && abs(y1 - y2) == 2)
	{
		if (x2 > x1) x2 = x1 + 1;
		else x2 = x1 - 1;
		if (y2 > y1) y2 = y1 + 1;
		else y2 = y1 - 1;
		return true;
	}
	if ( abs(x1 - x2) > abs(y1 - y2) )	// will move to left or right of knot
	{
		if (x2 > x1) x2 = x1 + 1;
		else x2 = x1 - 1;
		y2 = y1;
		return true;
	}
	if ( abs(y1 - y2) > abs(x1 - x2) )	// will move to up or down of knot
	{
		if (y2 > y1) y2 = y1 + 1;
		else y2 = y1 - 1;
		x2 = x1;
		return true;
	}

	return false;
}
void planck(int dx, int dy)
{
	Hx += dx;
	Hy += dy;
	bool do_add = false;
	int a, b;
	if (!touching(Tx, Ty, Hx, Hy, a, b))
	{
		Tx = Hx - dx;
		Ty = Hy - dy;
		do_add = true;
	}


	if (do_add)
	{
		addpos(Tx, Ty);
		//printf("H: %d,%d  T: %d,%d  %d,%d\n", Hx, Hy, Tx, Ty, dx, dy);
	}
}

void planckx(int dx, int dy)
{
	R[0].x += dx;
	R[0].y += dy;
	int hi = 0;
	for (; hi < 9; hi++)
	{
		touchingx(R[hi].x, R[hi].y, R[hi+1].x, R[hi+1].y);

	}
	addpos(R[9].x, R[9].y);

	//display();
}


void move(char m, int p)
{
	int dx=0, dy=0;
	switch(m)
	{
		case 'U': dx = 0;  dy = 1; break;
		case 'D': dx = 0;  dy = -1; break;
		case 'L': dx = -1; dy = 0; break;
		case 'R': dx = 1;  dy = 0; break;
		default: printf("Error!!!\n"); exit(1);
	}
	for (int i = 0; i < p; i++)
	{
		planck(dx, dy);
	}
}


void movex(char m, int p)
{
	int dx=0, dy=0;
	switch(m)
	{
		case 'U': dx = 0;  dy = 1; break;
		case 'D': dx = 0;  dy = -1; break;
		case 'L': dx = -1; dy = 0; break;
		case 'R': dx = 1;  dy = 0; break;
		default: printf("Error!!!  m: %c  p: %d\n", m, p); exit(1);
	}
	for (int i = 0; i < p; i++)
	{
		planckx(dx, dy);
	}
}


void solve(const char *v, int head_moves)
{
	FILE *f = fopen(v, "r");
	int	p;
	char m;
	char c[MAXLINE];
	fgets(c, MAXLINE, f);
	Tx = 0;
	Ty = 0;
	Hx = 0;
	Hy = 0;
	xyp.clear();
	addpos(Tx, Ty);
	while (!feof(f))
	{
		parseline(c, m, p);
		move(m, p);
		fgets(c, MAXLINE, f);
	}
	int num_head_moves = xyp.size();
	printf("Input file: %s\n", v);
	printf("Num head moves: %d\n", num_head_moves);
	if (head_moves > 0)
	{
		if (num_head_moves != head_moves)
			printf("ERROR - number of tail spots does not agree with truth: %d!\n", head_moves);
		else
			printf("Successfully counted unique postions of the tail!!\n");
	}


	fclose(f);
}

void solvex(const char *v, int head_moves)
{
	FILE *f = fopen(v, "r");
	int	p;
	char m;
	char c[MAXLINE];
	fgets(c, MAXLINE, f);
	for (int i = 0; i <= 9; i++)
	{
		R[i].x = 0;
		R[i].y = 0;
	}
	xyp.clear();
	addpos(0, 0);
	while (!feof(f))
	{
		parseline(c, m, p);
		movex(m, p);
		//display();
		fgets(c, MAXLINE, f);
	}
	int num_head_moves = xyp.size();
	printf("Input file: %s\n", v);
	printf("Num head moves: %d\n", num_head_moves);
	if (head_moves > 0)
	{
		if (num_head_moves != head_moves)
			printf("ERROR - number of tail spots does not agree with truth: %d!\n", head_moves);
		else
			printf("Successfully counted unique postions of the tail!!\n");
	}


	fclose(f);
}
int main(int argc, char **argv)
{
	solve("test.txt", 13);
	solve("input.txt", 6367);
	solvex("test2.txt", 36);
	solvex("input.txt", 2536);
	return 0;
}
