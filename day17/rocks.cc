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
	int64_t	wide;
	int64_t tall;
	int     tops[4];
	int     bots[4];
	int64_t lpos;
	int64_t rpos;
	int64_t bpos;
} rock_t;

typedef struct sh_r {
	int64_t sh[7];  // sh[0] = ch_top - ch_tops[0];
	int64_t r;
	int64_t height;
} sh_t;

using namespace std;

vector<sh_t> shapes;

void out(rock_t &r, const char *s = "")
{

	printf("\n%s\nrock: %s  wide: %ld   tall: %ld\n", s, r.id, r.wide, r.tall);
	printf("lpos: %ld   rpos: %ld    bpos: %ld\n", r.lpos, r.rpos, r.bpos);
	printf("tops: %d   %d   %d   %d \n", r.tops[0], r.tops[1], r.tops[2], r.tops[3]);
	printf("bots: %d   %d   %d   %d \n", r.bots[0], r.bots[1], r.bots[2], r.bots[3]);
}

rock_t	rocks[NUM_ROCK_TYPES];


#define CH_WIDE	7

int64_t		ch_top = 0;
int64_t		ch_tops[CH_WIDE];
int64_t		ch_gaps[CH_WIDE];

bool	jet[12000];
int64_t		jet_count;
int64_t		jet_index = 0;


void histogram()
{

	printf("  TOPS:  %6ld  %6ld  %6ld  %6ld  %6ld  %6ld  %6ld\n",
		ch_tops[0], ch_tops[1], ch_tops[2], ch_tops[3], ch_tops[4], ch_tops[5], ch_tops[6]);
	printf("  GAPS:  %6ld  %6ld  %6ld  %6ld  %6ld  %6ld  %6ld\n",
		ch_gaps[0], ch_gaps[1], ch_gaps[2], ch_gaps[3], ch_gaps[4], ch_gaps[5], ch_gaps[6]);
}
bool next_jet_right()
{
	bool rv = jet[jet_index % jet_count];
	jet_index++;
	return rv;
}

//
// need to add ch_gaps!
bool intersect(rock_t &r, bool debug = false)
{
	if (r.bpos < 0) return true;
	for (int64_t w = 0; w < 4; w++)
	{
		if (r.tops[w] == 0) continue;
		if (ch_tops[w + r.lpos] > r.bpos + r.bots[w])
		{
			if (debug) printf("Top intersects at %ld - height %ld - rock %ld\n",
				w + r.lpos, ch_tops[w + r.lpos], r.bpos + r.bots[w]);
			return true;
		}
	}
	return false;
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
	for (int i = 0; i < 3; i++)
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
	nr.bpos = ch_top;		// we will move 3 'puffs of jets'
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
	rocks[0].wide = 4;
	rocks[0].tall = 1;
	rocks[0].lpos = 2;
	rocks[0].rpos = 5;

	rocks[0].tops[0] = 1;
	rocks[0].tops[1] = 1;
	rocks[0].tops[2] = 1;
	rocks[0].tops[3] = 1;
	rocks[0].bots[0] = 0;
	rocks[0].bots[1] = 0;
	rocks[0].bots[2] = 0;
	rocks[0].bots[3] = 0;

	// .#.
	// ###
	// .#.
	rocks[1].id = "cross";
	rocks[1].wide = 3;
	rocks[1].tall = 3;
	rocks[1].lpos = 2;
	rocks[1].rpos = 4;

	//rocks[0].shape = {1,3,1];
	rocks[1].tops[0] = 2;
	rocks[1].tops[1] = 3;
	rocks[1].tops[2] = 2;
	rocks[1].tops[3] = 0;
	rocks[1].bots[0] = 1;
	rocks[1].bots[1] = 0;
	rocks[1].bots[2] = 1;
	rocks[1].bots[3] = 0;	

	// ..#
	// ..#
	// ###
	rocks[2].id = "bw L";
	rocks[2].wide = 3;
	rocks[2].tall = 3;
	rocks[2].lpos = 2;
	rocks[2].rpos = 4;
	
	rocks[2].tops[0] = 1;
	rocks[2].tops[1] = 1;
	rocks[2].tops[2] = 3;
	rocks[2].tops[3] = 0;
	rocks[2].bots[0] = 0;
	rocks[2].bots[1] = 0;
	rocks[2].bots[2] = 0;
	rocks[2].bots[3] = 0;
	// #
	// #
	// #
	// #
	rocks[3].id = "tall";
	rocks[3].wide = 1;
	rocks[3].tall = 4;
	rocks[3].lpos = 2;
	rocks[3].rpos = 2;
	
	rocks[3].tops[0] = 4;
	rocks[3].tops[1] = 0;
	rocks[3].tops[2] = 0;
	rocks[3].tops[3] = 0;
	rocks[3].bots[0] = 0;
	rocks[3].bots[1] = 0;
	rocks[3].bots[2] = 0;
	rocks[3].bots[3] = 0;	

	// ##
	// ##		
	rocks[4].id = "box";
	rocks[4].wide = 2;
	rocks[4].tall = 2;
	rocks[4].lpos = 2;
	rocks[4].rpos = 3;

	rocks[4].tops[0] = 2;
	rocks[4].tops[1] = 2;
	rocks[4].tops[2] = 0;
	rocks[4].tops[3] = 0;
	rocks[4].bots[0] = 0;
	rocks[4].bots[1] = 0;
	rocks[4].bots[2] = 0;
	rocks[4].bots[3] = 0;	
}



void add_to_chamber(rock_t &r, bool debug = false)
{
	for (int i = 0; i < r.wide; i++)
	{
		int ix = i + r.lpos;
		int old_top = ch_tops[ix];
		ch_tops[ix] = r.tops[i] + r.bpos;
		if (ch_top < ch_tops[ix]) ch_top = ch_tops[ix];
		ch_gaps[ix] = ch_tops[ix] - old_top + ch_bots[i];

	}
	if (debug) out(r);
	if (debug) histogram();
}

bool same_shape(int64_t *s, int64_t *t)
{
	for (int i = 0; i < 7; i++, s++, t++)
	    if( *s != *t) return false;
	return true;
}

int64_t hash_shapes(int64_t *sh)
{
	int64_t  hs = 0;
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
			if (debug) printf("sh: %ld  at  rock %ld - %ld tall"
					"  repeats at sh: %ld  at  rock %ld  - %ld tall\n",
						hash_shapes(s.sh), s.r, s.height, hash_shapes(shapes[i].sh), shapes[i].r, shapes[i].height);
			index = i;
			return true;
		}
	}
	shapes.push_back(s);
	return false;
}

bool shape(int64_t r, int64_t &index)
{
	sh_t 	s;

	for (int64_t w = 0; w < CH_WIDE; w++)
	{
		s.sh[w] =  ch_top - ch_tops[w] + 1;
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
		if (debug) out(r, "New rock");
		while (!done)
		{
			bool lr = next_jet_right();
			movelr(r, lr);
			if (debug)
			{
				std::string s = "Moving rock ";
				s += lr ? "Right" : "Left";
				out(r, s.c_str());
			}
			done = !movedown(r);
			if (debug) out(r, "Moving rock down");
		}
		add_to_chamber(r);
		//out_chamber(15);
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

void solvept2(const char *fn, int64_t num_rocks, int64_t answer)
{
	excercise(fn, num_rocks, est_answer, true);	// get est answers
	int64_t ex_answer;
	excercise(fn, excess_height, ex_answer, false); // get add-on answer
	printf("Rocks at excess: %ld   ex_answer: %ld\n", excess_height, ex_answer);
	est_answer += ex_answer;
	
	printf("Height: %ld\n", est_answer);

	if (est_answer == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %ld\n", answer);
}

void solvept1(const char *fn, int64_t num_rocks, int64_t answer)
{
	int64_t est_answer;
	excercise(fn, num_rocks, est_answer, false); // get add-on answer
	
	printf("Part 1 Height: %ld\n", est_answer);

	if (est_answer == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %ld\n", answer);
}

int main()
{
	solvept2("input.txt", 1000000000000, 1579411764703); // 1579411764703 is too low
	solvept2(   "ex.txt", 1000000000000, 1514285714288);
	return 0;
	solvept1(   "ex.txt", 2022, 3068);
	solvept1("input.txt", 2022, 3211);
	solvept2(   "ex.txt", 1000000000000, 1514285714288);
	return 0;
}
