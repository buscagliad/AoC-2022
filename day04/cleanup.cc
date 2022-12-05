#ifdef PROBLEM
--- Day 4: Camp Cleanup ---

Space needs to be cleared before the last supplies can be unloaded from the ships, and so several Elves have been assigned the job of cleaning up sections of the camp. Every section has a unique ID number, and each Elf is assigned a range of section IDs.

However, as some of the Elves compare their section assignments with each other, theyve noticed that many of the assignments overlap. To try to quickly find overlaps and reduce duplicated effort, the Elves pair up and make a big list of the section assignments for each pair (your puzzle input).

For example, consider the following list of section assignment pairs:

2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8

For the first few pairs, this list means:

    Within the first pair of Elves, the first Elf was assigned sections 2-4 (sections 2, 3, and 4), while the second Elf was assigned sections 6-8 (sections 6, 7, 8).
    The Elves in the second pair were each assigned two sections.
    The Elves in the third pair were each assigned three sections: one got sections 5, 6, and 7, while the other also got 7, plus 8 and 9.

This example list uses single-digit section IDs to make it easier to draw; your actual list might contain larger numbers. Visually, these pairs of section assignments look like this:

.234.....  2-4
.....678.  6-8

.23......  2-3
...45....  4-5

....567..  5-7
......789  7-9

.2345678.  2-8
..34567..  3-7

.....6...  6-6
...456...  4-6

.23456...  2-6
...45678.  4-8

Some of the pairs have noticed that one of their assignments fully contains the other. For example, 2-8 fully contains 3-7, and 6-6 is fully contained by 4-6. In pairs where one assignment fully contains the other, one Elf in the pair would be exclusively cleaning sections their partner will already be cleaning, so these seem like the most in need of reconsideration. In this example, there are 2 such pairs.

In how many assignment pairs does one range fully contain the other?

#endif

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>

typedef struct {
	int start;
	int end;
} rooms;

std::vector<rooms> er;

void parse(char *l, int &l1, int &h1, int &l2, int &h2)
{
	//printf("parse: %s", l); fflush(stdout);
	
	const char *p = strtok(l, "-,\n");
	l1 = atoi(p);
	//printf("l1: %d  s: %s\n", l1, p);  fflush(stdout);
	p = strtok(NULL, "-,\n");
	h1 = atoi(p);
	//printf("h1: %d  s: %s\n", h1, p);  fflush(stdout);
	p = strtok(NULL, "-,\n");
	l2 = atoi(p);
	p = strtok(NULL, "-,\n");
	h2 = atoi(p);
	//printf("[%d,%d] [%d,%d] \n", l1, h1, l2, h2);
}

int fill(const char *fn)
{
	FILE *f = fopen(fn, "r");
	if (f) printf("Opened file %s\n", fn);
	char ln[100];
	//42-76,3-76
	int l1, h1, l2, h2;
	int contain = 0;
	int overlap = 0;
	while (!feof(f))
	{
		fgets(ln, 100, f);
		if (feof(f)) break;
		parse(ln, l1, h1, l2, h2);
		if ( (l1 <= l2) && (h1 >= h2) )  contain++;
		else if ( (l1 >= l2) && (h1 <= h2) )  contain++;
		
		if (l1 <= h2  &&  h2 <= h1) overlap++;
		else if (l1 <= l2  &&  l2 <= h1) overlap++;
		else if (l2 <= h1  &&  h1 <= h2) overlap++;
		else if (l2 <= l1  &&  l1 <= h2) overlap++;
	}
	printf("  Num containing pairs are: %d\n", contain);
	printf("  Num overlapping pairs are: %d\n", overlap);
	return 1;
}

int main()
{
	fill("test.txt");
	fill("day04.txt");
}

	
