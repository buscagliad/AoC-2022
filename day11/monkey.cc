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
bool part1 = true;


long m0(long n) { return n * 19; };
long m1(long n) { return n * 11; };
long m2(long n) { return n + 6; };
long m3(long n) { return n + 5; };
long m4(long n) { return n + 7; };
long m5(long n) { return n * n; };
long m6(long n) { return n + 2; };
long m7(long n) { return n + 3; };

typedef struct monk_s
{
	vector<long>	item;
	long 			divis;
	mop*		m;
	int			divtrue;
	int			divfalse;
	int			insp;
} monk_t;

monk_t mnk[8];
int num_monkey = 8;
int Round = 0;

const bool debug = false;
const long divx = 17l * 3l * 19l * 7l * 2l * 5l * 11l * 13l;

void init()
{
	int i = 0;
	Round = 0;
	for (i = 0; i < num_monkey; i++)
	{
		mnk[i].item.clear();
	}
	i = 0;
/*
Monkey 0:
  Starting items: 72, 64, 51, 57, 93, 97, 68
  Operation: new = old * 19
  Test: divisible by 17
    If true: throw to monkey 4
    If false: throw to monkey 7
*/	
	mnk[i].item.push_back(72);
	mnk[i].item.push_back(64);
	mnk[i].item.push_back(51);
	mnk[i].item.push_back(57);
	mnk[i].item.push_back(93);
	mnk[i].item.push_back(97);
	mnk[i].item.push_back(68);
	mnk[i].divis = 17;
	mnk[i].m = m0;
	mnk[i].divtrue = 4;
	mnk[i].divfalse = 7;
	mnk[i].insp = 0;
	i++;
/*
Monkey 1:
  Starting items: 62
  Operation: new = old * 11
  Test: divisible by 3
    If true: throw to monkey 3
    If false: throw to monkey 2
*/	
	mnk[i].item.push_back(62);
	mnk[i].divis = 3;
	mnk[i].m = m1;
	mnk[i].divtrue = 3;
	mnk[i].divfalse = 2;
	mnk[i].insp = 0;
	i++;
/*
Monkey 2:
  Starting items: 57, 94, 69, 79, 72
  Operation: new = old + 6
  Test: divisible by 19
    If true: throw to monkey 0
    If false: throw to monkey 4
*/	

	mnk[i].item.push_back(57);
	mnk[i].item.push_back(94);
	mnk[i].item.push_back(69);
	mnk[i].item.push_back(79);
	mnk[i].item.push_back(72);
	mnk[i].divis = 19;
	mnk[i].m = m2;
	mnk[i].divtrue = 0;
	mnk[i].divfalse = 4;
	mnk[i].insp = 0;
	i++;
/*
Monkey 3:
  Starting items: 80, 64, 92, 93, 64, 56
  Operation: new = old + 5
  Test: divisible by 7
    If true: throw to monkey 2
    If false: throw to monkey 0
*/	
	mnk[i].item.push_back(80);
	mnk[i].item.push_back(64);
	mnk[i].item.push_back(92);
	mnk[i].item.push_back(93);
	mnk[i].item.push_back(64);
	mnk[i].item.push_back(56);
	mnk[i].divis = 7;
	mnk[i].m = m3;
	mnk[i].divtrue = 2;
	mnk[i].divfalse = 0;
	mnk[i].insp = 0;
	i++;
/*
Monkey 4:
  Starting items: 70, 88, 95, 99, 78, 72, 65, 94
  Operation: new = old + 7
  Test: divisible by 2
    If true: throw to monkey 7
    If false: throw to monkey 5
*/	

	mnk[i].item.push_back(70);
	mnk[i].item.push_back(88);
	mnk[i].item.push_back(95);
	mnk[i].item.push_back(99);
	mnk[i].item.push_back(78);
	mnk[i].item.push_back(72);
	mnk[i].item.push_back(65);
	mnk[i].item.push_back(94);
	mnk[i].divis = 2;
	mnk[i].m = m4;
	mnk[i].divtrue = 7;
	mnk[i].divfalse = 5;
	mnk[i].insp = 0;
	i++;
/*
Monkey 5:
  Starting items: 57, 95, 81, 61
  Operation: new = old * old
  Test: divisible by 5
    If true: throw to monkey 1
    If false: throw to monkey 6
*/	

	mnk[i].item.push_back(57);
	mnk[i].item.push_back(95);
	mnk[i].item.push_back(81);
	mnk[i].item.push_back(61);
	mnk[i].divis = 5;
	mnk[i].m = m5;
	mnk[i].divtrue = 1;
	mnk[i].divfalse = 6;
	mnk[i].insp = 0;
	i++;
/*
Monkey 6:
  Starting items: 79, 99
  Operation: new = old + 2
  Test: divisible by 11
    If true: throw to monkey 3
    If false: throw to monkey 1
*/	

	mnk[i].item.push_back(79);
	mnk[i].item.push_back(99);
	mnk[i].divis = 11;
	mnk[i].m = m6;
	mnk[i].divtrue = 3;
	mnk[i].divfalse = 1;
	mnk[i].insp = 0;
	i++;
/*
Monkey 7:
  Starting items: 68, 98, 62
  Operation: new = old + 3
  Test: divisible by 13
    If true: throw to monkey 5
    If false: throw to monkey 6
*/	

	mnk[i].item.push_back(68);
	mnk[i].item.push_back(98);
	mnk[i].item.push_back(62);
	mnk[i].divis = 13;
	mnk[i].m = m7;
	mnk[i].divtrue = 5;
	mnk[i].divfalse = 6;
	mnk[i].insp = 0;
	i++;
/*
*/
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
		if (part1)
			worry /= 3;
		else
		    worry %= divx;
		if (debug) printf("  Worry level divided by 3 %ld\n", worry);
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

void display()
{
	printf("ROUND %d\n", Round);
	for (int i = 0; i < num_monkey; i++)
	{
		printf("Monkey %d: ", i);
		if (mnk[i].item.size() == 0) printf(" EMPTY ");
		else printf("%ld", mnk[i].item[0]);
		for (long j = 1; j < (int) mnk[i].item.size(); j++)
		    printf(", %ld", mnk[i].item[j]);
		printf("\n");
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
	Round++;
}

int main()
{
	init();
	//display();
	part1 = true;
	printf("Part 1:\n");
	for (int i = 1; i <= 20; i++) { round(); }
	//display();
	answer();
	init();
	part1 = false;
	printf("\nPart 2:\n");
	for (int i = 1; i <= 10000; i++) { round(); }
	//display();
	answer();
	//printf("DIV: %ld\n", divx);
}
