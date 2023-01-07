#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <vector>
#include <string>


#define MAX_ROCK_WIDE 4
#define MAX_ROCK_TALL 4
#define NUM_ROCK_TYPES 5

typedef struct rock_s {
	const char *id;
	int64_t rows;
	int64_t	cols;
	bool	sh[4][4];
	int64_t lpos;
	int64_t rpos;
	int64_t bpos;
} rock_t;

typedef struct sh_r {
	uint64_t sh[7];  // sh[0] = ch_top - ch_tops[0];
	int64_t r;
	int64_t height;
} sh_t;

using namespace std;

vector<sh_t> shapes;

void out(rock_t &r, const char *s = "")
{
	printf("%s Rock: %s\n", s, r.id);
	for (int j = r.rows - 1; j >= 0; j--)
	{
		printf("|");
		for (int i = 0; i < r.cols; i++)
		{
			if (r.sh[j][i]) printf("#"); else printf(".");
		}
		printf("|\n");
	}
	//printf("\n%s\nrock: %s  cols: %ld   rows: %ld\n", s, r.id, r.cols, r.rows);
	printf("lpos: %ld   rpos: %ld    bpos: %ld\n", r.lpos, r.rpos, r.bpos);
}

rock_t	rocks[NUM_ROCK_TYPES];


#define CH_WIDE	7

int64_t		ch_top = 0;
int64_t		ch_tops[CH_WIDE];
int64_t		ch_gaps[CH_WIDE];

bool	jet[12000];
int64_t		jet_count;
int64_t		jet_index = 0;

const int64_t			ch_size = 50;

bool	chamber[ch_size][CH_WIDE];
int64_t	ch_index;	// index into the chamber (mod ch_size to get index)

void histogram();

// need to add ch_gaps!
void add_to_chamber(rock_t &rk, bool debug = false)
{
	
	// clear bottom rows to become 'top' rows
	for (int64_t r = ch_top; r <= max(ch_top, ch_top + rk.rows + rk.bpos); r++)
		for (int64_t c = 0; c < 7; c++)
			chamber[r % ch_size][c] = false;
	
	//if (ch_top + r.bpos < 0) return;
	for (int64_t r = 0; r < rk.rows; r++)
	{
		for (int64_t c = 0; c < rk.cols; c++)
		{
			if (rk.sh[r][c])
			    chamber[(r + rk.bpos + ch_top) % ch_size][c + rk.lpos] = rk.sh[r][c];
		}
	}
	ch_top += max(0l, rk.rows + rk.bpos);
	if (debug) histogram();
}

//
bool intersect(rock_t &rk, bool debug = false)
{
	if (rk.bpos >= 0) return false;
	if (rk.bpos < 0 && ch_top == 0) return true;
	for (int64_t c = 0; c < rk.cols; c++)
	{
		//for (int64_t t = 0; t < r.rows; t++)
		//for (int64_t r = rk.rows - 1; r >= 0; r--)
		for (int64_t rb = rk.bpos; rb <= min(0l, (rk.rows - 1 + rk.bpos)); rb++)  // just do the bottom rows
		{
			int64_t r = rb - rk.bpos;
			if (!rk.sh[r][c]) continue;
			//int64_t chr = (r + rk.bpos + ch_top) % ch_size;
			int64_t chr = (rb + ch_top) % ch_size;
			//int64_t chc = (t + ch_top) % ch_size;
			int64_t chc = c + rk.lpos;
			if (chamber[chr][chc] && rk.sh[r][c])
			{
				if (debug) printf ("Collision at chc,chr: %ld,%ld   w,t:%ld,%ld  bpos=%ld, ch_top=%ld\n", 
						chr, chc, r, c, rk.bpos, ch_top);
				return true;
			}
		}
	}
	return false;
}



void 	zarr(bool x[MAX_ROCK_WIDE][MAX_ROCK_TALL])
{
	for (int64_t w = 0; w < MAX_ROCK_WIDE; w++)
		for (int64_t t = 0; t < MAX_ROCK_TALL; t++)
			x[w][t] = false;
}



void histogram()
{
	printf("%6ld |.......|\n", ch_top);
	for (int r = ch_top-1; r >= max(ch_top - ch_size, 0l); r--)
	//for (int j = max(ch_size-1, ch_top); j >= max(0, ch_top); j--)
	{
		printf("%6d |", r);
		for (int c = 0; c < CH_WIDE; c++)
		{
			if (chamber[r % ch_size][c]) printf("#"); else printf(".");
		}
		printf("|\n");
	}
	printf("\n");
}

bool next_jet_right()
{
	bool rv = jet[jet_index % jet_count];
	jet_index++;
	return rv;
}


bool movelr(rock_t &r, bool right)
{
	rock_t nr = r;
	bool rv = false;
	if (right)
	{
		nr.rpos++;
		nr.lpos++;
		if (nr.rpos < CH_WIDE) rv = true;
	}
	else
	{
		nr.rpos--;
		nr.lpos--;
		if (nr.lpos >= 0) rv = true;
	}
	if (rv)
	{
		rv = !intersect(nr);
	}
	if (rv) r = nr;
	return rv;
}

//
// returns true if movement allowed,
// false if movement would intersect the chamber
bool movedown(rock_t &r)
{
	rock_t nr = r;
	nr.bpos--;
	if (intersect(nr)) return false;
	r = nr;
	return true;
}

void getthree(int64_t &left, int64_t &right)
{
	for (int i = 0; i < 4; i++)
	{
		if (next_jet_right())
		{
			if (right < 6) { right++; left++; }
		}
		else
		{
			if (left > 0) { right--; left--; }
		}
	}
}


rock_t new_rock(int64_t index)
{
	int64_t rtype = index % NUM_ROCK_TYPES;
	rock_t	nr = rocks[rtype];
	nr.bpos = 0;		// we will move 3 'puffs of jets'
	getthree(nr.lpos, nr.rpos);

	return nr;
}
	
void init(const char *fn)
{
	FILE *f = fopen(fn, "r");
	int c;
	jet_count = 0;
	while ((c = fgetc(f)))
	{
		if (c == '<') jet[jet_count++] = false;
		else if (c == '>') jet[jet_count++] = true;
		else break;
	}
	jet_index = 0;
	printf("jet_count is %ld\n", jet_count);
	fclose(f);
	
	shapes.clear();

	ch_top = 0;
	for (int64_t i = 0; i < CH_WIDE; i++) { ch_tops[i] = 0; ch_gaps[i] = 0; }

	// ####
	rocks[0].id = "flat";
	rocks[0].cols = 4;
	rocks[0].rows = 1;
	rocks[0].lpos = 2;
	rocks[0].rpos = 5;
	rocks[0].bpos = 0;

	zarr(rocks[0].sh);
	rocks[0].sh[0][0] = true;
	rocks[0].sh[0][1] = true;
	rocks[0].sh[0][2] = true;
	rocks[0].sh[0][3] = true;

	// .#.
	// ###
	// .#.
	rocks[1].id = "cross";
	rocks[1].cols = 3;
	rocks[1].rows = 3;
	rocks[1].lpos = 2;
	rocks[1].rpos = 4;
	rocks[1].bpos = 0;

	zarr(rocks[1].sh);
	rocks[1].sh[2][1] = true;
	rocks[1].sh[1][0] = true;
	rocks[1].sh[1][1] = true;
	rocks[1].sh[1][2] = true;
	rocks[1].sh[0][1] = true;

	// ..#
	// ..#
	// ###
	rocks[2].id = "bw L";
	rocks[2].cols = 3;
	rocks[2].rows = 3;
	rocks[2].lpos = 2;
	rocks[2].rpos = 4;
	rocks[2].bpos = 0;
	

	zarr(rocks[2].sh);
	rocks[2].sh[0][0] = true;
	rocks[2].sh[0][1] = true;
	rocks[2].sh[0][2] = true;
	rocks[2].sh[1][2] = true;
	rocks[2].sh[2][2] = true;
	// #
	// #
	// #
	// #
	rocks[3].id = "rows";
	rocks[3].cols = 1;
	rocks[3].rows = 4;
	rocks[3].lpos = 2;
	rocks[3].rpos = 2;
	
	zarr(rocks[3].sh);
	rocks[3].sh[3][0] = true;
	rocks[3].sh[2][0] = true;
	rocks[3].sh[1][0] = true;
	rocks[3].sh[0][0] = true;
	rocks[3].bpos = 0;

	// ##
	// ##		
	rocks[4].id = "box";
	rocks[4].cols = 2;
	rocks[4].rows = 2;
	rocks[4].lpos = 2;
	rocks[4].rpos = 3;
	rocks[4].bpos = 0;

	zarr(rocks[4].sh);
	rocks[4].sh[1][1] = true;
	rocks[4].sh[0][1] = true;
	rocks[4].sh[1][0] = true;
	rocks[4].sh[0][0] = true;
	
	
	for (int c = 0; c < CH_WIDE; c++)
		for (int r = 0; r < ch_size; r++)
			chamber[r][c] = false;
}



//
bool same_shape(uint64_t *s, uint64_t *t)
{
	for (int i = 0; i < 7; i++, s++, t++)
	    if( *s != *t) return false;
	return true;
}

uint64_t hash_shapes(uint64_t *sh)
{
	uint64_t  hs = 0;
	for (int i = 0; i < 7; i++)
	{
		hs <<= 8;
		hs += *sh++;
	}
	return hs;
}

bool add_shape(sh_t & s, int64_t r, int64_t &index, bool debug = true)
{
	s.r = r;
	s.height = ch_top;
	for (size_t i = 0; i < shapes.size(); i++)
	{
		if ( (same_shape(s.sh, shapes[i].sh)  && (r == 2 * shapes[i].r) ) )
		{
			if (debug) printf("sh: %lu  at  rock %ld - %ld rows"
					"  repeats at sh: %lu at  rock %ld  - %ld rows\n",
						hash_shapes(s.sh), s.r, s.height, hash_shapes(shapes[i].sh), shapes[i].r, shapes[i].height);
			index = i;
			return true;
		}
	}
	shapes.push_back(s);
	//printf("Adding::  sh: %lu  at  rock %ld - %ld rows\n",
	//					hash_shapes(s.sh), s.r, s.height);
	return false;
}

bool shape(int64_t r, int64_t &index)
{
	sh_t 	s;

	for (int64_t c = 0; c < CH_WIDE; c++)
	{
		s.sh[c] = 0;
		for (int64_t r = 0; r < ch_size; r++)
		{
			s.sh[c] <<= 1;
			s.sh[c] += (chamber[r][c] ? 1 : 0);
		}
	}
	return add_shape(s, r, index);
}


int64_t est_answer;
int64_t excess_height;

void excercise(const char *fn, int64_t num_rocks, int64_t &answer, bool trend, bool debug = false)
{
	init(fn);
	rock_t r;
	int64_t index;

	for (int64_t numr = 1; numr <= num_rocks; numr++)
	{
		r = new_rock(numr-1);
		bool done = false;
		if (debug) {
			printf("New rock %ld   Jet: %ld\n", numr, jet_index-4);
			out(r);
		}
		while (!done)
		{
			done = !movedown(r);
			if (debug && numr == 29) out(r, "Moving rock down");
			if (done) break;
			bool lr = next_jet_right();
			bool mlr = movelr(r, lr);
			if (debug && numr == 29)
			{
				std::string s = "Moving rock ";
				s += lr ? "Right" : "Left";
				if (mlr) s += "  SUCCESS"; else s+= "FAIL" ;
				out(r, s.c_str());
			}
		}
		add_to_chamber(r, debug);

		if (!trend) continue;
		if ((numr) % (5*jet_count) == 0)
		{
		    if (shape(numr, index))
		    {
				int64_t	repeats = num_rocks / shapes[index].r - 1;
				excess_height = num_rocks % shapes[index].r + shapes[index].r;
				answer = (ch_top - shapes[index].height) * repeats;
				// now just add height of excess_height
				printf("Number of rocks: %ld  num repeats: %ld\n", num_rocks, repeats);
				printf("Est answer: %ld  Excess Height: %ld\n", answer, excess_height);
				break;
			}
		}
	}
	if (trend) return;
	answer = ch_top;
}

void solvept2(const char *fn, int64_t num_rocks, int64_t answer, bool debug = false)
{
	excercise(fn, num_rocks, est_answer, true, debug);	// get est answers
	int64_t ex_answer;
	excercise(fn, excess_height, ex_answer, false, debug); // get add-on answer
	printf("Rocks at excess: %ld   ex_answer: %ld\n", excess_height, ex_answer);
	est_answer += ex_answer;
	
	printf("Height: %ld\n", est_answer);

	if (est_answer == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %ld\n", answer);
}

void solvept1(const char *fn, int64_t num_rocks, int64_t answer, bool debug = false)
{
	int64_t est_answer;
	excercise(fn, num_rocks, est_answer, false, debug); // get add-on answer
	
	printf("Part 1 Height: %ld\n", est_answer);

	if (est_answer == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %ld\n", answer);
}

int main()
{
	solvept1(   "ex.txt", 2022, 3068, false);
	solvept1("input.txt", 2022, 3211, false);
	solvept2(   "ex.txt", 1000000000000, 1514285714288);
	solvept2("input.txt", 1000000000000, 1589142857183); // 1579411764703 is too low
}
