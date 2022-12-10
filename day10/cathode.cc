#ifdef PROBLEM




#endif

#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <cstdlib>

using namespace std;

int Cycle = 0;

#define MAXLINE 1000

char crt[6][40];
void init()
{
	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 40; j++)
		   crt[i][j] = ' ';
//	crt[0][0] = '#';
//	crt[0][1] = '#';
//	crt[0][2] = '#';

}

///////////////////////////////////////////////////////////////////
// DEBUG outgrid outputs the grid to stdout
///////////////////////////////////////////////////////////////////



void parseline(char *c, bool &noop, int &v)
{
	if (strncmp(c, "noop", 4) == 0) { noop = true; return; }
	noop = false;
	c += 5;
	v = atoi(c);
}

void draw(int c, int signal)
{
	c--;
	int col = c % 40;
	//printf("cycle: %d   col: %d  signal: %d\n", c, col, signal);
	if (abs(col-signal) < 2)
	{
		int row = c / 40;
		crt[row][col] = '#';
	}
}

void printcrt()
{
	for (int row = 0; row < 6; row++)
	{
		for (int col = 0; col < 40; col++)
		{
			printf("%c", crt[row][col]);
		}
		printf("\n");
	}
}
	

void solve(const char *v, int true_signal)
{
	FILE *f = fopen(v, "r");
	int	p;
	bool noop;
	char c[MAXLINE];
	fgets(c, MAXLINE, f);
	Cycle = 0;
	int signal = 1;
	int icy = 0;
	int Report = 20;
	int signalStrength = 0;
	init();
	while (!feof(f))
	{
		parseline(c, noop, p);
		//if (!noop) printf("C: %d p:%d Signal: %d\n", Cycle, p, signal);
		Cycle++;
		draw(Cycle, signal);
		if (noop) 
		{
			icy = 0; 
		}
		else
		{
			icy ++;
			if (icy > 1) { signal += p; icy = 0; }
			icy ++;
			Cycle++;
			draw(Cycle, signal);
		}
		if (Cycle >=  Report) 
		{ 
			//printf("%3d  Signal x: %d\n", Cycle, signal); 
			signalStrength += signal * Report; 
			Report += 40; 
		}
		if (icy > 1) { signal += p; icy = 0; }
		fgets(c, MAXLINE, f);
	}

	printcrt();
	printf("Input file: %s\n", v);
	printf("Total signal: %d\n", signalStrength);
	if (true_signal > 0)
	{
		if (true_signal != signalStrength)
			printf("ERROR - signal does not agree with truth: %d!\n", true_signal);
		else
			printf("Successfully estimated the signal!!\n");
	}


	fclose(f);
}


int main(int argc, char **argv)
{
	solve("test.txt", 13140);
	solve("input.txt", 16880);
	return 0;
}
