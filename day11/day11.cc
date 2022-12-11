#ifdef PROBLEM

Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3



#endif

#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <algorithm>

using namespace std;

typedef  long (mop)(long);

typedef struct monk_s
{
	vector<long>	item;
	long 			divis;
	mop*		m;
	int			divtrue;
	int			divfalse;
	int			insp;
} monk_t;

monk_t mnk[4];
int num_monkey = 4;

const bool debug = false;

/*
Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3

Monkey 1:
  Starting items: 54, 65, 75, 74
  Operation: new = old + 6
  Test: divisible by 19
    If true: throw to monkey 2
    If false: throw to monkey 0

Monkey 2:
  Starting items: 79, 60, 97
  Operation: new = old * old
  Test: divisible by 13
    If true: throw to monkey 1
    If false: throw to monkey 3

Monkey 3:
  Starting items: 74
  Operation: new = old + 3
  Test: divisible by 17
    If true: throw to monkey 0
    If false: throw to monkey 1
*/

long m0(long n) { return n * 19; };
long m1(long n) { return n + 6; };
long m2(long n) { return n * n; };
long m3(long n) { return n + 3; };

void init()
{

	int i = 0;
/*
Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3
*/
	mnk[i].item.push_back(79);
	mnk[i].item.push_back(98);
	mnk[i].divis = 23;
	mnk[i].m = m0;
	mnk[i].divtrue = 2;
	mnk[i].divfalse = 3;
	mnk[i].insp = 0;
	i++;

/*
Monkey 1:
  Starting items: 54, 65, 75, 74
  Operation: new = old + 6
  Test: divisible by 19
    If true: throw to monkey 2
    If false: throw to monkey 0
*/	
	mnk[i].item.push_back(54);
	mnk[i].item.push_back(65);
	mnk[i].item.push_back(75);
	mnk[i].item.push_back(74);
	mnk[i].divis = 19;
	mnk[i].m = m1;
	mnk[i].divtrue = 2;
	mnk[i].divfalse = 0;
	mnk[i].insp = 0;
	i++;

/*
Monkey 2:
  Starting items: 79, 60, 97
  Operation: new = old * old
  Test: divisible by 13
    If true: throw to monkey 1
    If false: throw to monkey 3
*/	
	mnk[i].item.push_back(79);
	mnk[i].item.push_back(60);
	mnk[i].item.push_back(97);
	mnk[i].divis = 13;
	mnk[i].m = m2;
	mnk[i].divtrue = 1;
	mnk[i].divfalse = 3;
	mnk[i].insp = 0;
	i++;

/*
Monkey 3:
  Starting items: 74
  Operation: new = old + 3
  Test: divisible by 17
    If true: throw to monkey 0
    If false: throw to monkey 1
*/	
	mnk[i].item.push_back(74);
	mnk[i].divis = 17;
	mnk[i].m = m3;
	mnk[i].divtrue = 0;
	mnk[i].divfalse = 1;
	mnk[i].insp = 0;
	i++;
}


void inspect(int m)
{
	if (debug) printf("Monkey %d\n", m);
	for (size_t i = 0; i < mnk[m].item.size(); i++)
	{
		if (debug) printf("  Inspect item with worry level %ld\n", mnk[m].item[i]);
		long  worry = mnk[m].m(mnk[m].item[i]);
		if (debug) printf("  Worry level modified to %ld\n", worry);
		mnk[m].insp++;
		worry %= (17*13*19*23);
		if (debug) printf("  Worry level divided by %ld -> %ld\n",  3l, worry);
		if (worry % mnk[m].divis == 0)
		{
			if (debug) printf("  Worry level divisible by  %ld moved to %d\n", 
					mnk[m].divis, mnk[m].divtrue);
		    mnk[mnk[m].divtrue].item.push_back(worry);
		}
		else 
		{
			if (debug) printf("  Worry level NOT divisible by  %ld moved to %d\n", 
					mnk[m].divis, mnk[m].divfalse);
		    mnk[mnk[m].divfalse].item.push_back(worry);
		}
	}
	mnk[m].item.clear();
}

void display(int d)
{
	printf("ROUND %d\n", d);
	for (int i = 0; i < num_monkey; i++)
	{
		printf("Monkey %d: ", i);
		if (mnk[i].item.size() == 0) printf(" EMPTY ");
		else printf("%ld", mnk[i].item[0]);
		for (long j = 1; j < (int) mnk[i].item.size(); j++)
		    printf(", %ld", mnk[i].item[j]);
		printf("... Inspected %d times\n", mnk[i].insp);
	}
}

void answer()
{
	vector<long> insp;
	for (long i = 0; i < num_monkey; i++)
	{
		insp.push_back(mnk[i].insp);
	}
	sort(insp.begin(), insp.end());
	long m1 = insp[num_monkey-2];
	long m2 = insp[num_monkey-1];
	printf("Answer is %ld * %ld = %ld\n", m1, m2, m1*m2);
}

void round()
{
	for (long i = 0; i < num_monkey; i++)
	{
		inspect(i);
	}
}

int main()
{
	init();
	display(0);
	for (int i = 1; i <= 10000; i++) { round(); if (i == 1 || i == 20 || i %1000 == 0) display(i); }
	answer();
}
