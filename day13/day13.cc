#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <cstring>
#include <string>
#include <stack>

#ifdef PROBLEM
[ [ [ [2, [],5, [] ]]], [4,3, [6,8, [8], 3, [9,2,10,4,10] ] ], [4, [ [7], [9,6],10,1,8], [9, [] ,0] ,8] ]
[ [ [ [8,[10],10,[5,1]] ,8], [ [6,9,9,5], [2,9] ] ], [1], [], [1,1,5,1], [2] ]


[[2,[],5,[]],[4,3,[6,8,[8],3,[9,2,10,4,10]]],[4,[[7],[9,6],10,1,8],[9,[],0],8]]
[[[[8,10,10,5,1],8],[[6,9,9,5],[2,9]]],[1],[],[1,1,5,1],[2]]


#endif

using namespace std;

#define MAX_MSG 100
char	msg1[MAX_MSG];
char    msg2[MAX_MSG];
char    dummy[MAX_MSG];

typedef struct msg_s {
	vector<int>  v;		// if v[j] < 0, then list[-j - 1] is the list of lists
	vector<struct msg_s *> list;
} vlist;

typedef vector<vlist> vl_t;

typedef vector<int> *vi;

stack<vector<int>>  m1;
stack<vector<int>>  m2;

//
// if >= 0, its the number
// if == -1 v is returned
// if < -1 stop
int parse(char *m, char *v, int &in)
{
	int i = 0;
	int depth = -1;
	int rv = -2;
	while (m[in] && m[in] != '\n' && depth != 0)
	{
		if (m[in] == '[')	
		{
			if (depth < 0) depth = 0;
			depth++;
			v[i++] = m[in++];
			rv = -1;
		}
		else if (m[in] == ']') 
		{
			depth--;
			v[i++] = m[in++];
			rv = -1;
		}
		else if (isdigit(m[in]))
		{
			int vx = m[in] - '0';
			in++;
			while (isdigit(m[in]))
			{
				vx = 10 * vx + m[in] - '0';
				in++;
			}
			return vx;
		}
		else
		{
			v[i++] = m[in++];
			rv = -1;
		}
	}
	v[i] = 0;
	return rv;
}



int getmsg(FILE *f)
{
	fgets(msg1, MAX_MSG, f);
	fgets(msg2, MAX_MSG, f);
	fgets(dummy, MAX_MSG, f);
	if (feof(f)) return 0;
	return 1;
}

int compare(char *a, char *b)
{
	return 1;
}

int solve(const char *fn, int v)
{
	FILE *f = fopen(fn, "r");
	int num = 0;
	int good = 0;
	int bad = 0;
	while (getmsg(f))
	{
		printf("MSG1: %s", msg1);
		printf("MSG2: %s", msg2);
		if (compare(msg1, msg2) > 0)
		{
			good++; 
			printf("GOOD\n");
		}
		else 
		{
			bad++;
			printf("BAD\n");
		}
		num++;
		printf("\n");
	}
	return num;
}

int main()
{
	solve("ex.txt", 1);
	int i = 0;
	char v[1000];
	int n;
	bool done = false;
	while(!done)
	{
		n = parse(msg1, v, i);
		if (n >= 0)
			printf("P: %s\n", v);
		else if (n == -1)
		    printf("N: %d\n", n);
		else done = true;
		
	}
}
