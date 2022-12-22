#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>
#define MAXN 10000


typedef struct llist {
	int64_t		value;
	int64_t		orig_index;
	struct llist *next;
	struct llist *prev;
} llist;



llist	*orig[MAXN];	// this will point to each of the original entries
						//  orig[K]->value  is the value of the Kth term
						// of the original list
llist	*begList = NULL; // top of the current list

bool DEBUG = false;

int64_t lSize;


llist	*getOrigIndex(int64_t ix)
{
	llist *n = begList;
	for (int64_t i = 0; i < lSize; i++)
	{
		if (n->orig_index == ix) return n;
	}
	return NULL;
}

llist	*getValueIndex(int64_t value)
{
	llist *n = begList;
	for (int64_t i = 0; i < lSize; i++)
	{
		if (n->value == value) return n;
		n = n->next;
	}
	return NULL;
}



bool valid(llist *)
{
	llist *t = begList;
	for (int64_t i = 0; i < lSize; i++)
	{
		llist *n = t->next;
		if (n->prev != t) { printf("Address error at i = %ld\n", i); return false; }
	}
	t = begList;
	for (int64_t i = 0; i < lSize; i++)
	{
		llist *n = t->prev;
		if (n->next != t) { printf("Address error at i = %ld\n", i); return false; }
	}
	return true;
}

void out(llist *l, bool spr = true)
{
	if (!valid(l)) printf("BAD list\n");
	l = getValueIndex(0);
	for (int64_t n = 0; n < lSize; n++)
	{
		if (spr) printf("%5ld ", l->value);
		else printf("%5ld  O: %5ld  V: %5ld\n", n, l->orig_index, l->value);
		l = l->next;
	}
	l = l->prev;
	printf("\n");
	for (int64_t n = 0; n < lSize; n++)
	{
		if (spr) printf("%5ld ", l->value);
		else printf("%5ld  O: %5ld  V: %5ld\n", n, l->orig_index, l->value);
		l = l->prev;
	}
	printf("\n\n");
}

void out(llist *l[], int64_t n)
{
	for (int64_t i = 0; i < n; i++)
	{
		printf("%5ld ", l[i]->value);
	}
	printf("\n");
}


#ifdef CURRENT_RESULTS
Get: 1000   Val: 4
Get: 2000   Val: -3
Get: 3000   Val: 2
Grove coordinate is: 3
The answer is correct!!
Get: 1000   Val: -7021
Get: 2000   Val: -8947
Get: 3000   Val: 4682
Grove coordinate is: -11286
INCORRECT - answer should be 1
#endif





void init(const char *fn, int64_t enc_mult = 1)
{
	FILE *f = fopen(fn, "r");
	lSize = 0;
	char s[100];
	llist *prev = NULL;
	llist *nl = new llist;
	llist *n;
	begList = nl;
	while (!feof(f))
	{
		fgets(s, 100, f);
		if (feof(f)) break;
		int64_t orig_index = lSize;
		int64_t value = atoll(s);
		n = new llist;
		n->next = begList;
		n->prev = nl;
		orig[lSize] = nl;
		nl->value = value * enc_mult;
		nl->orig_index = orig_index;
		nl->prev = prev;
		nl->next = n;
		prev = nl;
		nl = n;
		
		lSize++;
	}
	orig[lSize-1]->next = begList;
	begList->prev = orig[lSize-1];

	fclose(f);
}

//
// 4  6  1 -4 -3  3  2  5
void move(int64_t i)
{
	//int64_t n = *p;
	//int64_t to_n = (n + lSize) % lSize;
}

int64_t getval(int64_t nPlaces)
{
	llist *n = getValueIndex(0);
	for(int64_t i = 0; i < nPlaces; i++) n = n->next;
	//printf("%6ld  %ld\n", nPlaces, n->value);
	
	return n->value;
}

void vout()
{
	for (int64_t i = 0; i < lSize; i++)
	{
		// always sort list to be in 'current index' order
		llist *p = orig[i];
		printf("I: %ld  V: %ld  I: %ld\n", i, p->value, p->orig_index);
	}
}

void mix(bool debug = false)
{
	for (int64_t k = 0; k < lSize; k++)
	{
		// always sort list to be in 'current index' order
		llist *p = orig[k];
		int64_t num_moves = (p->value ) % (lSize - 1);	// Had to subtract since i will remove
													// p from the list
													
		if (num_moves == 0) continue;
		llist *r;								// r will be moved till it moves num_moves
		// p is pointing to the object we need to move, we 
		// first remove it and relink its neighbors
		p->prev->next = p->next;
		p->next->prev = p->prev;
		r = p->prev;   // need to reset where r is starting at ****


		if (num_moves > 0)
		{
			for (int64_t i = 0; i < abs(num_moves); i++) { 
				r = r->next; 
				if (!r) printf("ERROR null\n"); 
			}
			if(debug)printf("Moving %ld to after %ld\n", num_moves, r->value);
			// r points to the element for which p will move to the 'right' of
		}
		else if (num_moves < 0)
		{
			for (int64_t i = 0; i < abs(num_moves); i++){ 
				r = r->prev;
				if (!r) printf("ERROR null\n"); 
			}
		}
		// Insert p before r
		p->prev = r;
		p->next = r->next;
		r->next->prev = p;
		r->next = p;


		if (debug) out(begList);
		//if (!eval()) exit(1);
		
	}
}
void solvept1(const char *fn, int64_t answer, bool debug = false)
{
	init(fn, 1);
	mix();
	if (debug) out(begList);
	int64_t res = getval(1000) + getval(2000) + getval(3000);
	printf("Grove coordinate is: %ld\n", res);
	if (res == answer) printf("The answer is correct!!\n");
	else printf("INCORRECT - answer should be %ld\n", answer);
}

void solvept2(const char *fn, int64_t answer, bool debug = false)
{
	init(fn, 811589153);
	for (int i = 0; i < 10; i++) mix();
	if (debug) out(begList);
	int64_t res = getval(1000) + getval(2000) + getval(3000);
	printf("Grove coordinate is: %ld\n", res);
	if (res == answer) printf("The answer is correct!!\n");
	else printf("INCORRECT - answer should be %ld\n", answer);
}
	
int main()
{
	solvept1("ex.txt", 3, false);
	solvept1("input.txt", 11037, false);
	solvept2("ex.txt", 1623178306l, false);
	solvept2("input.txt", 3033720253914l, false);

}
