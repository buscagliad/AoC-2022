#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <cstring>
#include <string>
#include <stack>
#include <iostream>

#ifdef PROBLEM
[ [ [ [2, [],5, [] ]]], [4,3, [6,8, [8], 3, [9,2,10,4,10] ] ], [4, [ [7], [9,6],10,1,8], [9, [] ,0] ,8] ]
[ [ [ [8,[10],10,[5,1]] ,8], [ [6,9,9,5], [2,9] ] ], [1], [], [1,1,5,1], [2] ]


[[2,[],5,[]],[4,3,[6,8,[8],3,[9,2,10,4,10]]],[4,[[7],[9,6],10,1,8],[9,[],0],8]]
[[[[8,10,10,5,1],8],[[6,9,9,5],[2,9]]],[1],[],[1,1,5,1],[2]]


#endif

using namespace std;

#define MAX_MSG 400

#define END_VALUE 2000

typedef struct msg_s {
	size_t     		vi;
	int      		li;
	vector<int>		v;		// if v[j] < 0, then list[-j - 1] is the list of lists
	vector<struct msg_s *> list;
	struct msg_s * prev;
} vlist;

bool isInt(vlist *a)
{
	if (a->v[a->vi] >= 0) return true;
	return false;
}

bool isList(vlist *a)
{
	if (a->v[a->vi] < 0) return true;
	return false;
}


	
void zero_indexes(vlist *v)
{
	if (v == NULL) return;
	v->vi = 0;
	v->li = 0;
	for (size_t i = 0; i < v->list.size(); i++)
	    zero_indexes(v->list[i]);
}

void clear(vlist *v)
{
	if (v == NULL) return;
	for (size_t i = 0; i < v->list.size(); i++)
	{
		int d = v->v[i];
		if (d < 0)
			clear(v->list[-d-1]);
	}
	free(v);
}


string to_ascii(vlist *v, bool display_ints = true)
{
	if (v == NULL) return "";
	bool comma = false;
    string s = "[";
	for (size_t i = 0; i < v->v.size(); i++)
	{
	    int d = v->v[i];
        if (d < 0) 
        {
			if (comma) {s += ","; }
			s += to_ascii(v->list[-d-1], false);
			comma = true;
		}
	    else
	    {
			if (comma) {s += ","; }
			s += to_string(d);
			comma = true;
		}
	}
    s += "]";
 
    return s;
}


void display(vlist *v)
{
	string s = to_ascii(v);
	cout << s;
}

typedef vector<vlist> vl_t;

typedef vector<int> *vi;

int parsex(char *m, vlist **v)
{
	vlist *top = new vlist;
	top->vi = 0;
	top->li = 0;
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
			nv->vi = 0;
			nv->li = 0;
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
	*m = 0;
	return 0;
}

int getmsg(FILE *f, char *msg1, char *msg2)
{
	char    dummy[MAX_MSG];
	fgets(msg1, MAX_MSG, f);
	fgets(msg2, MAX_MSG, f);
	fgets(dummy, MAX_MSG, f);
	if (feof(f)) return 0;
	return 1;
}


vlist *nextList(vlist *a)
{
	int d = a->vi++;
	return a->list[-d-1];
}

int num_items(vlist *c)
{
	return c->v.size();
}


int getn(char *a, int &n)
{
	n = 1;
	int s = 0;
	s = *a++ - '0';
	while (isdigit(*a))
	{
		s += 10 * s + *a++ - '0';
		n++;
	}
	return s;
}

int compare(char *a, char *b)
{
	if (*a == '\n') return -1;
	if (*b == '\n') return 1;
	// compare two lists
	if ((*a == '[') && (*b == '['))
	{
		printf("Two lists %c - %c\n", *(a+1), *(b+1));
	    return compare(a+1, b+1);
	}
	// compare two numbers
	else if (isdigit(*a) && isdigit(*b))
	{
		int lm, rm;
		int ln = getn(a, lm);
		int rn = getn(b, rm);
		if (ln > rn) return 1;
		else if (ln < rn) return -1;
		printf("%d == %d - getting next\n", ln, rn);
		return compare(a+lm, b+rm);
	}
	// left is a number
	else if (isdigit(*a))
	{
		printf("Left is a digit %d\n", *a-'0');
		return compare(a, b+1);
	}
	// right is a number
	else if (isdigit(*b))
	{
		printf("Left is a digit %d\n", *b-'0');
		return compare(a+1, b);
	}
	return 1;
}

int solve(const char *fn, int v)
{
	FILE *f = fopen(fn, "r");
	
	int num = 0;
	int good = 0;
	int bad = 0;
	int sum = 0;
	int pair = 1;
	char	msg1[MAX_MSG];
	char    msg2[MAX_MSG];
	while (getmsg(f, msg1, msg2))
	{
		if (compare(msg1, msg2) > 0)
		{
			good++; 
			sum += pair;
			printf("GOOD\n");
		}
		else 
		{
			bad++;
			printf("BAD\n");
		}
		num++;
		pair++;
		//printf("\n");
	}
	printf("Sum is %d\n", sum);
	return num;
}

int main()
{
	solve("ex.txt", 1);
	//solve("input.txt", 1);
}
