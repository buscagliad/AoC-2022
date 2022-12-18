#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>

#ifdef PROBLEM
[ [ [ [2, [],5, [] ]]], [4,3, [6,8, [8], 3, [9,2,10,4,10] ] ], [4, [ [7], [9,6],10,1,8], [9, [] ,0] ,8] ]
[ [ [ [8,[10],10,[5,1]] ,8], [ [6,9,9,5], [2,9] ] ], [1], [], [1,1,5,1], [2] ]


[[2,[],5,[]],[4,3,[6,8,[8],3,[9,2,10,4,10]]],[4,[[7],[9,6],10,1,8],[9,[],0],8]]
[[[[8,10,10,5,1],8],[[6,9,9,5],[2,9]]],[1],[],[1,1,5,1],[2]]


#endif

using namespace std;

#define MAX_MSG 400


bool debug = false;

int getmsg(FILE *f, char *msg1, char *msg2)
{
	char    dummy[MAX_MSG];
	fgets(msg1, MAX_MSG, f);
	fgets(msg2, MAX_MSG, f);
	fgets(dummy, MAX_MSG, f);
	if (feof(f)) return 0;
	return 1;
}




int getn(char *a, int &n)
{
	n = 1;
	int s = 0;
	if (!isdigit(*a))
	{
		printf("ERROR - a: %s\n", a);
		return 0;
	}
	s = *a++ - '0';
	while (isdigit(*a))
	{
		s = 10 * s + *a++ - '0';
		n++;
	}
	return s;
}

char *makelist(char *s)
{
	char *k = (char *)malloc(strlen(s) + 5);
	char *sk = k;
	*k++ = '[';
	while (isdigit(*s)) { *k++ = *s++; }
	*k++ = ']';
	
	strcpy(k, s);
	return sk;
}
	
bool compare(char *a, char *b)
{
	if (*a == '\n') {
		if (debug) printf("Left side ran out - GOOD\n");
		return true;
	}
	if (*b == '\n') {
		if (debug) printf("Left side ran out - GOOD\n");
		return false;
	}
	if ( (*a == ']') && (*b != ']')){
		if (debug) printf("Left side ran out - GOOD\n");
		return true;
	}
	if ( (*a != ']') && (*b == ']')){
		if (debug) printf("Right side ran out - BAD\n");
		return false;
	}
	// compare two lists
	if ((*a == '[') && (*b == '['))
	{
		if (debug) printf("Two lists %c - %c\n", *(a+1), *(b+1));
	    return compare(a+1, b+1);
	}
	else if ((*a == ',') && (*b == ','))
	{
		if (debug) printf("Two commas\n");
	    return compare(a+1, b+1);
	}
	else if ((*a == ']') && (*b == ']'))
	{
		if (debug) printf("Two end brackets\n");
	    return compare(a+1, b+1);
	}
	// compare two numbers
	// compare two numbers
	else if (isdigit(*a) && isdigit(*b))
	{
		int lm, rm;
		int ln = getn(a, lm);
		int rn = getn(b, rm);
		if (ln < rn) {
			if (debug) printf("Good Order:: %d < %d\n", ln, rn);
			return true;
		}
		else if (ln > rn) {
			if (debug) printf("Bad Order:: %d > %d\n", ln, rn);
			return false;
		}
		if (debug) printf("%d(%d) == %d(%d) - getting next\n", ln, lm, rn, rm);
		return compare(a+lm, b+rm);
	}
	// left is a number
	else if (isdigit(*a))
	{
		if (debug) printf("Left is a digit %d\n", *a-'0');
		a = makelist(a);
		return compare(a, b);
	}
	// right is a number
	else if (isdigit(*b))
	{
		if (debug) printf("Right is a digit %d\n", *b-'0');
		b = makelist(b);
		return compare(a, b);
	}
	return true;
}

int solvepart1(const char *fn, int v)
{
	FILE *f = fopen(fn, "r");
	
	int num = 0;
	int good = 0;
	int bad = 0;
	int sum = 0;
	int pair = 1;
	char	msg1[MAX_MSG];
	char    msg2[MAX_MSG];
	string  add;
	while (getmsg(f, msg1, msg2))
	{
		if (debug) printf("\n\nPair: %d\nMsg 1: %s\nMsg 2: %s\n", pair, msg1, msg2);
		if (compare(msg1, msg2))
		{
			good++; 
			sum += pair;
			add += to_string(pair) + " + ";
			if (debug) printf("GOOD\n");
		}
		else 
		{
			bad++;
			if (debug) printf("BAD\n");
		}
		num++;
		pair++;
		//printf("\n");
	}
	fclose(f);
	printf("Sum is %d\n", sum);
	if (sum == v) printf("Correct answer!!\n");
	else printf("Incorrect answer.\n");
	if (debug) cout << add << endl;
	if (debug) printf("Num Good: %d   Num Bad: %d\n", good, bad);
	return num;
}

int findsig(vector<char *> &s, const char *sig)
{
	for (size_t i = 0; i < s.size(); i++)
		if (strncmp(s[i], sig, strlen(sig)) == 0) return i+1;
	return -1;
}

void trim(char *m)
{
	int i = strlen(m) - 1;
	if (m[i] == '\n') m[i] = 0;
}

void solvepart2(const char *fn, int v)
{
	FILE *f = fopen(fn, "r");
	
	vector<char *>  siglist;
	char	msg1[MAX_MSG];
	char    msg2[MAX_MSG];
	const char *sig1 = "[[2]]\n";
	const char *sig2 = "[[6]]\n";
	char *m1 = (char *)malloc(MAX_MSG);
	char *m2 = (char *)malloc(MAX_MSG);
	strcpy(m1, sig1);
	strcpy(m2, sig2);
	siglist.push_back(m1);
	siglist.push_back(m2);
	while (getmsg(f, msg1, msg2))
	{
		
		m1 = (char *)malloc(MAX_MSG);
		m2 = (char *)malloc(MAX_MSG);
		if (debug) printf("Msg 1: %s\nMsg 2: %s\n", msg1, msg2);
		//trim(msg1);
		//trim(msg2);
		strcpy(m1, msg1);
		strcpy(m2, msg2);
		siglist.push_back(m1);
		siglist.push_back(m2);
	}
	fclose(f);



	if (0)
	{
		for (size_t i = 0; i < siglist.size(); i++)
			printf("%s\n", siglist[i]);
		for (size_t i = 0; i < siglist.size(); i++)
			printf("%16.16lx\n", (unsigned long) siglist[i]);
	}
	std::sort(siglist.begin(), siglist.end(), compare);
	if (0)
	{
		for (size_t i = 0; i < siglist.size(); i++)
			printf("%s\n", siglist[i]);
	}
	int ix1 = findsig(siglist, sig1);
	int ix2 = findsig(siglist, sig2);
	int key = ix1 * ix2;
	printf("Key is %d\n", key);
	if (key == v) printf("Correct answer!!\n");
	else printf("Incorrect answer.\n");
}

int main()
{
	solvepart1("ex.txt", 13);
	solvepart1("input.txt", 5605);
	solvepart2("ex.txt", 140);
	solvepart2("input.txt", 24969);
}
