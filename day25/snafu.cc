
#include <cstdio>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <ctype.h>
#include <cstring>
#include <string>
#include <cmath>

int getval(char s)
{
	switch(s)
	{
		case '1': return 1;
		case '2': return 2;
		case '0': return 0;
		case '-': return -1;
		case '=': return -2;
	}
	//printf("ERROR - unknown snafu char: %c\n", s);
	return -10;
}

bool ispwr5(char d)
{
	if ( (d == '0') || (d == '1') || (d == '2') || (d == '-') || (d == '=') ) return true;
	return false;
}

int64_t powerof5(const char *s)
{
	int64_t p5 = 1;
	s++;
	while (ispwr5(*s))
	{
	    p5 = p5 * 5;
	    s++;
	}
	return p5;
}

int64_t	s2d(const char *s)
{
	int64_t v = 0;
	const char *p = s;
	int64_t pwr5 = powerof5(s);
	//printf("snafu: %s  p5 = %ld\n", s, pwr5);
	int64_t gv;
	v = pwr5 * getval(*p);
	p++;
	while ((gv = getval(*p)) != -10)
	{
		pwr5 /= 5;
		v = v + pwr5 * gv;
		//printf("v = %ld   p: %c  dig: %ld\n", v, *p, gv);
		p++;
	}
	return v;
}

const char *d2s(int64_t n)
{
	const char chk[]="=-012";
	int64_t p5 = 1;
	std::string s = "";
	while (3 * p5 < n) p5 = 5 * p5;
	int k;
	int l;
	while (p5 > 0)// 2=-1=0
	{
		k = round((double)n / (double) p5 + 0.00001);
		s = s + chk[k+2];
		//printf("N: %ld  p5: %ld   k: %d  c: %c", n, p5, k, chk[k+2]);
		n -= k * p5;
		//printf("  New n: %ld\n", n);
		p5 = p5 / 5;
	}

	const char *rs = strdup(s.c_str());
	//printf("n: %ld   p5: %ld  s = %s\n", n, p5, s.c_str());
	return rs;
}

void test(const char *s, int64_t v)
{
	int64_t sv = s2d(s);
	const char *ss = d2s(v);
	printf("Snafu: %s    Value: %ld   %s\n", s, sv, (sv == v)? "SUCCESS" : "ERROR");
	printf("Value: %ld   Snafu: %s    %s\n", v, ss, (strcmp(s, ss) == 0) ? "SUCCESS" : "ERROR");
}

void test_snafu()
{
	struct snafu {
		int64_t		d;
		const char *s;
	} sn[] = {
			{1,              "1"},
			{2,              "2"},
			{3,             "1="},
			{4,             "1-"},
			{5,             "10"},
			{6,             "11"},
			{7,             "12"},
			{8,             "2="},
			{9,             "2-"},
			{10,             "20"},
			{15,            "1=0"},
			{20,            "1-0"},
			{2022,         "1=11-2"},
			{12345,        "1-0---0"},
			{314159265,  "1121-1110-1=0"},
			{0, ""}  };
	for (int i = 0; sn[i].d > 0; i++)
	{
		test(sn[i].s, sn[i].d);
	}
}


void solvept1(const char *fn, int64_t value, const char *snafu)
{
	FILE *f = fopen(fn, "r");
	char sn[100];
	int64_t v = 0;
	printf("Input file: %s\n", fn);
	while (!feof(f))
	{
		fgets(sn, 100, f);
		if (feof(f)) continue;
		v += s2d(sn);
	}
	const char *s = d2s(v);
	printf("Sum is %ld   Snafu: %s\n\n", v, s);
}



int main()
{
	
	//test_snafu();
	solvept1("ex.txt", 4890, "2=-1=0");
	solvept1("input.txt", 10000, "1111");
}

