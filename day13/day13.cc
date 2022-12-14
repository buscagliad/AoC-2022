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


typedef struct msg_s {
	int	     		vi;
	int      		li;
	vector<int>		v;		// if v[j] < 0, then list[-j - 1] is the list of lists
	vector<struct msg_s *> list;
	struct msg_s * prev;
} vlist;

void zero_indexes(vlist *v)
{
	if (v == NULL) return;
	v->vi = 0;
	v->li = 0;
	for (size_t i = 0; i < v->list.size(); i++)
	    zero_indexes(v->list[i]);
}


void display(vlist *v, bool first_call = true)
{
	if (v == NULL) return;
	bool comma = false;
    printf("[");
	for (size_t i = 0; i < v->v.size(); i++)
	{
	    int d = v->v[i];
        if (d < 0) 
        {
			if (comma) printf(",");
			display(v->list[-d-1], false);
		}
	    else
	    {
			if (comma) printf(",");
			printf("%d", d);
			comma = true;
		}
	}
	printf("]");
}

typedef vector<vlist> vl_t;

typedef vector<int> *vi;

int parsex(char *m, vlist **v)
{
	vlist *top = new vlist;
	vlist *curr = top;
	*v = NULL;
	while (*m && *m != '\n')
	{
		//printf("%s", m);
		if (*m == '[')	
		{
			int  d = curr->list.size();
			d = -d - 1;
			curr->v.push_back(d);
			//curr->prev = prev;
			vlist *nv = new vlist;
			if (*v == NULL) *v = nv;
			nv->prev = curr;
			curr->list.push_back(nv);
			curr = nv;
			//printf("PUSH:: curr vector address %16.16lx\n", curr);
		}
		else if (*m == ']') 
		{
			if (!curr) printf("NULL curr pointer\n");
			else curr = curr->prev;
			//printf("POP:: curr vector address %16.16lx\n", curr);
		}
		else if (isdigit(*m))
		{
			int vx = *m - '0';
			m++;
			while (isdigit(*m))
			{
				vx = 10 * vx + *m - '0';
				m++;
			}
			curr->v.push_back(vx);
			m--;
		}
		else if (*m == ',') // ignore
		{
		}
		else
		{
			printf("PARSE ERROR\n");
		}
		m++;

	}
	return 0;
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
	vlist *v;
	parsex(msg1, &v);
	printf("PARSEX:: curr vector address %16.16lx\n", (unsigned long)v);
	display(v);
}
