
#include <stack>
#include <cstdio>
#include <cstdlib>

std::stack<char> stacks[10];

/*
[V]     [B]                     [C]
[C]     [N] [G]         [W]     [P]
[W]     [C] [Q] [S]     [C]     [M]
[L]     [W] [B] [Z]     [F] [S] [V]
[R]     [G] [H] [F] [P] [V] [M] [T]
[M] [L] [R] [D] [L] [N] [P] [D] [W]
[F] [Q] [S] [C] [G] [G] [Z] [P] [N]
[Q] [D] [P] [L] [V] [D] [D] [C] [Z]
 1   2   3   4   5   6   7   8   9 
*/

void init()
{
	stacks[9].push('Z');
	stacks[9].push('N');
	stacks[9].push('W');
	stacks[9].push('T');
	stacks[9].push('V');
	stacks[9].push('M');
	stacks[9].push('P');
	stacks[9].push('C');
	
	stacks[8].push('C');
	stacks[8].push('P');
	stacks[8].push('D');
	stacks[8].push('M');
	stacks[8].push('S');
	
	stacks[7].push('D');
	stacks[7].push('Z');
	stacks[7].push('P');
	stacks[7].push('V');
	stacks[7].push('F');
	stacks[7].push('C');
	stacks[7].push('W');
	
	stacks[6].push('D');
	stacks[6].push('G');
	stacks[6].push('N');
	stacks[6].push('P');
	
	stacks[5].push('V');
	stacks[5].push('G');
	stacks[5].push('L');
	stacks[5].push('F');
	stacks[5].push('Z');
	stacks[5].push('S');
	
	stacks[4].push('L');
	stacks[4].push('C');
	stacks[4].push('D');
	stacks[4].push('H');
	stacks[4].push('B');
	stacks[4].push('Q');
	stacks[4].push('G');
	
	stacks[3].push('P');	
	stacks[3].push('S');	
	stacks[3].push('R');
	stacks[3].push('G');
	stacks[3].push('W');
	stacks[3].push('C');
	stacks[3].push('N');
	stacks[3].push('B');
	
	stacks[2].push('D');	
	stacks[2].push('Q');	
	stacks[2].push('L');
		
	stacks[1].push('Q');
	stacks[1].push('F');
	stacks[1].push('M');
	stacks[1].push('R');
	stacks[1].push('L');
	stacks[1].push('W');
	stacks[1].push('C');
	stacks[1].push('V');
}

void display(const char *s)
{
	printf("%s: ", s);
	for (int i = 1; i <= 9; i++)
	{
		printf("%c", stacks[i].top());
	}
	printf("\n");
}

void parse(char *inl, int &n, int &from, int &to)
{
//move 2 from 3 to 1
	char *p = inl + 5; // first char after 'move'
	n = atoi(p);
	while (*p != 'm') p++;
	p++;
	from = atoi(p);
	while (*p != 'o') p++;
	p++;
	to = atoi(p);
}

void stack_move(int n, int f, int t)
{
	char	s;
	for (int i = 1; i <= n; i++)
	{
		s = stacks[f].top();
		stacks[f].pop();
		stacks[t].push(s);
	}
}
void stack_move_multi(int n, int f, int t)
{
	char	q[n];
	for (int i = 0; i < n; i++)
	{
		q[i] = stacks[f].top();
		stacks[f].pop();
		//printf("%lu  i: %d   q: %c\n", stacks[f].size(), i, q[i]);
	}
	for (int i = n-1; i >= 0; i--)
	{
		stacks[t].push(q[i]);
	}
}

void move_multi(const char *fn)
{
//move 2 from 3 to 1
	FILE *f = fopen(fn, "r");
	char inl[100];
	int from, how_many, to;
	
	while (!feof(f))
	{
		fgets(inl, 100, f);
		if (feof(f)) break;
		parse(inl, how_many, from, to);
		//printf("move %d from %d to %d\n", how_many, from, to);
		stack_move_multi(how_many, from, to);
	}
	fclose(f);
}

void move(const char *fn)
{
//move 2 from 3 to 1
	FILE *f = fopen(fn, "r");
	char inl[100];
	int from, how_many, to;
	
	while (!feof(f))
	{
		fgets(inl, 100, f);
		if (feof(f)) break;
		parse(inl, how_many, from, to);
		//printf("move %d from %d to %d\n", how_many, from, to);
		stack_move(how_many, from, to);
	}
	fclose(f);
}

int main()
{
	init();
	display("Original top of stacks");
	move("input.txt");
	display("Part 1: Moving one container at a time");
	init();
	move_multi("input.txt");
	display("Part 2: Moving multiple containers at a time");
}

	
	
