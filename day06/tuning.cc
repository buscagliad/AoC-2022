#include <cstdio>


bool marker(const char *m, int N)
{
	for (int i = 0; i < N-1; i++)
	    for (int j = i+1; j < N; j++)
	        if (m[i] == m[j]) return false;
	return true;
}

void process(const char *fn, int N)
{
	FILE *f = fopen(fn, "r");
	char p[N];
	for (int i = 0; i < N; i++) p[i] = fgetc(f);
	int	n = N;
	while (!marker(p, N))
	{
		p[n%N] = fgetc(f);
		n++;
		if (feof(f)) {
			printf("ERROR\n");
			return;
		}
	}
	printf("marker for %d at %d\n", N, n);
}

int main()
{
	process("input.txt", 4);
	process("input.txt", 14);
}
