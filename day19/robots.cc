#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <string>

using namespace std;


int MAX_MINUTE;

// ::5(3,6,2,0)BO

typedef enum {NONE, ORE, CLAY, OBSIDIAN, GEODE} robo_e;

bool debug = false;



uint64_t master_id = 1;

class robo_state {
	public:
		void clear()
		{
			ore_r = 1;
			ore_c = 0;
			clay_r = 0;
			clay_c = 0;
			obs_r = 0;
			obs_c = 0;
			geo_r = 0;
			geo_c = 0;

			minute = 0;
		}
		

		int64_t getnum(char **s)
		{
			char *p = *s;
			while (!isdigit(*p)) p++;
			int64_t n = *p - '0';
			p++;
			while (isdigit(*p))
			{
				n = 10 * n + *p - '0';
				p++;
			}
			*s = p;
			return n;
		}
		
		robo_state()
		{
			clear();
		}
		
		robo_state(char *s)
		{
			blueprint = getnum(&s);
			ore_robot_ore = getnum(&s);
			clay_robot_ore = getnum(&s);
			obs_robot_ore = getnum(&s);
			obs_robot_clay = getnum(&s);
			geo_robot_ore = getnum(&s);
			geo_robot_obs = getnum(&s);	
			
			max_ore =  std::max(std::max(ore_robot_ore, clay_robot_ore), std::max(obs_robot_ore, geo_robot_ore));
			max_clay =  obs_robot_clay;
			max_obs =  geo_robot_obs;
			clear();		
		};	

		void bpout()
		{
			printf(	"Blueprint %ld:\n"
					"  Each ore robot costs %ld ore. \n"
					"  Each clay robot costs %ld ore. \n"
					"  Each obsidian robot costs %ld ore and %ld clay. \n"
					"  Each geode robot costs %ld ore and %ld obsidian.\n",
					blueprint, 
					ore_robot_ore, clay_robot_ore, 
					obs_robot_ore, obs_robot_clay, 
					geo_robot_ore, geo_robot_obs);
			printf("    Max Ore: %ld    Max Clay: %ld   Max Obs: %ld\n", max_ore, max_clay, max_obs);
		}
		void out(const char *s = "", bool bp = false) const
		{

			printf("== Minute %ld ==  %s\n", minute, s);
			if (ore_r)  printf("%ld ore-collecting robot collects %ld ore; you now have %ld ore.\n", ore_r, ore_r, ore_c);
			if (clay_r) printf("%ld clay-collecting robots collect %ld clay; you now have %ld clay.\n", clay_r, clay_r, clay_c);
			if (obs_r)  printf("%ld obsidian-collecting robots collect %ld obsidian; you now have %ld obsidian.\n", obs_r, obs_r, obs_c);
			if (geo_r)  printf("%ld geode-cracking robots crack %ld geodes; you now have %ld open geodes.\n", geo_r, geo_r, geo_c);
		};

		void update()
		{
			minute++;
			ore_c += ore_r;
			clay_c += clay_r;
			obs_c += obs_r;
			geo_c += geo_r;
		};

		bool	buy_ore()
		{
			if (ore_c >= ore_robot_ore)
			{
				ore_c -= ore_robot_ore;
				ore_r++;
				return true;
			}
			printf("buy ore failure\n");
			return false;
		}

		bool	buy_clay()
		{
			if (ore_c >= clay_robot_ore)
			{
				ore_c -= clay_robot_ore;
				clay_r++;
				return true;
			}
			printf("buy clay failure\n");
			return false;
		}

		bool	buy_obs()
		{
			if ( (ore_c >= obs_robot_ore) && (clay_c >= obs_robot_clay) )
			{
				ore_c -= obs_robot_ore;
				clay_c -= obs_robot_clay;
				obs_r++;
				return true;
			}
			printf("buy obs failure\n");
			return false;
		}

		bool	buy_geo()
		{
			if ( (ore_c >= geo_robot_ore) && (obs_c >= geo_robot_obs) )
			{
				ore_c -= geo_robot_ore;
				obs_c -= geo_robot_obs;
				geo_r++;
				return true;
			}
			printf("buy geo failure\n");
			return false;
		}	

		bool	should_buy_ore()
		{
			if (ore_r >= max_ore) return false;
			return true;
		}
		bool	can_buy_ore()
		{
			if (ore_c >= ore_robot_ore)
			{
				return true;
			}
			return false;
		}

		bool	should_buy_clay()
		{			
			if (clay_r >= max_clay) return false;
			return true;
		}
		bool	can_buy_clay()
		{			
			if (ore_c >= clay_robot_ore)
			{
				return true;
			}
			return false;
		}

		bool	should_buy_obs()
		{
			if (obs_r >= max_obs) return false;
			return true;
		}
		bool	can_buy_obs()
		{
			if ( (ore_c >= obs_robot_ore) && (clay_c >= obs_robot_clay) )
			{
				return true;
			}
			return false;
		}

		bool	should_buy_geo()
		{
			return true;
		}
		bool	can_buy_geo()
		{
			if ( (ore_c >= geo_robot_ore) && (obs_c >= geo_robot_obs) )
			{
				return true;
			}
			return false;
		}


		int64_t	ore_r;
		int64_t ore_c;
		int64_t	clay_r;
		int64_t clay_c;
		int64_t obs_r;
		int64_t obs_c;
		int64_t geo_r;
		int64_t geo_c;
		
		int64_t minute;
		int64_t	ore_robot_ore = 0;	// ore required to build an ore robot
		int64_t clay_robot_ore = 0;	// ore required to build a clay robot
		int64_t obs_robot_ore = 0;	// ore required to build an obsidian robot
		int64_t obs_robot_clay = 0;	// clay required to build an obsidian robot
		int64_t geo_robot_ore = 0;	// ore required to build a geo smashing robot
		int64_t geo_robot_obs = 0;	// obsidian required to build geo smashing robot
		int64_t blueprint = 0;		// current blueprint ID

		int64_t max_ore;
		int64_t max_clay;
		int64_t max_obs;
};


robo_state rmax;



robo_state init(char *l, bool debug)
{
//	blueprint = getnum(&l);
//	ore_robot_ore = getnum(&l);
//	clay_robot_ore = getnum(&l);
//	obs_robot_ore = getnum(&l);
//	obs_robot_clay = getnum(&l);
//	geo_robot_ore = getnum(&l);
//	geo_robot_obs = getnum(&l);	
	rmax.clear();
	robo_state r(l);
//	max_ore =  std::max(std::max(ore_robot_ore, clay_robot_ore), std::max(obs_robot_ore, geo_robot_ore));
//	max_clay =  obs_robot_clay;
//	max_obs =  geo_robot_obs;
	if (false) r.out();
	return r;
}

//
// Optimizations:
//	For ORE, OBS and CLAY, we only need N ore bots, where N is the largest number of ore required to
//  build one of these bots, M obs bots, where M is the number of obsidian that we need to build a GEO
//  and only need at most P clay bots, where P is the number of clay required for an obsidian bot
//
//  Also, say you have R robots mining resource a non-geode resource, K, with M minutes left, and currently
//  have Z units in stocke, then if Q is the most a bot needs for for this resource, once R * M + Z >= M * Q
//  we no longer need to consider building robots of resource type K



void	get_max_geodes(robo_state r, robo_e action)
{
	// check if it is possible to beat the high score
	// 
	switch(action)
	{
		case ORE: 		if (!r.should_buy_ore()) return; break;
		case CLAY:		if (!r.should_buy_clay()) return; break;
		case OBSIDIAN:	if (!r.should_buy_obs()) return; break;
		case GEODE:		if (!r.should_buy_geo()) return; break;
		case NONE:		break;
	}
	while (r.minute < MAX_MINUTE)
	{
		bool bought = false;
		robo_state rs = r;
		if (rs.can_buy_geo() && action == GEODE) 
		{
			rs.update();
			rs.buy_geo();
			bought = true;
		}
		else if (rs.can_buy_obs() && action == OBSIDIAN) 
		{
			rs.update();
			rs.buy_obs();
			bought = true;
		}
		else if (rs.can_buy_clay() && action == CLAY) 
		{
			rs.update();
			rs.buy_clay();
			bought = true;
		}
		else if (rs.can_buy_ore() && action == ORE)
		{
			rs.update();
			rs.buy_ore();
			bought = true;
		}
		if (bought)
		{
			for (int e = (int) ORE; e <= (int) GEODE; e++)
				get_max_geodes(rs, (robo_e) e);
			return;
		}
		else r.update();
	}

	if (r.geo_c > rmax.geo_c) 
	{ 
		rmax = r; 
		if (debug) rmax.out("GET_MAX_GEO", false); 
		fflush(stdout); 
	}
}


#define MAXLINE 200

bool solvept1(const char *fn, int64_t answer)
{
	FILE *f = fopen(fn, "r");
	int64_t qual_level = 0;
	char robline[MAXLINE];
	MAX_MINUTE = 24;
	while (!feof(f))
	{
		fgets(robline, MAXLINE, f);
		if (feof(f)) break;
		robo_state r = init(robline, false);
		//r.bpout();
		// added this loop at start as it seeds which robot is bought first
		for (int e = (int) ORE; e <= (int) GEODE; e++)
			get_max_geodes(r, (robo_e) e);
		int64_t mg = rmax.geo_c;
		if (debug) printf("\n\nGEO Objects for blueprint %ld\n", r.blueprint);

		qual_level += mg * r.blueprint;
		if (debug) printf("\n\n");
	}
	fclose(f);
	printf("File: %s\nPart 1 Quality level: %ld\n", fn, qual_level);

	if (qual_level == answer)
	{
		printf("Answer is correct!!!\n\n");
		return true;
	}
	else
		printf("Answer is not correct - should be %ld\n\n", answer);
	return false;
}

bool solvept2(const char *fn, int64_t answer)
{
	FILE *f = fopen(fn, "r");
	int64_t qual_level = 1;
	char robline[MAXLINE];
	MAX_MINUTE = 32;
	bool done = false;
	while (!done)
	{
		fgets(robline, MAXLINE, f);
		if (feof(f)) break;
		robo_state r = init(robline, false);
		//r.bpout();
		// added this loop at start as it seeds which robot is bought first
		for (int e = (int) ORE; e <= (int) GEODE; e++)
			get_max_geodes(r, (robo_e) e);
		int64_t mg = rmax.geo_c;
		if (debug) printf("\n\nGEO Objects for blueprint %ld\n", r.blueprint);

		qual_level *= mg;
		if (debug) printf("\n\n");
		if (feof(f) || (r.blueprint == 3)) done = true;
	}
	fclose(f);
	printf("File: %s\nPart 1 Quality level: %ld\n", fn, qual_level);

	if (qual_level == answer)
	{
		printf("Answer is correct!!!\n\n");
		return true;
	}
	else
		printf("Answer is not correct - should be %ld\n\n", answer);
	return false;
}

int main()
{
	solvept1("ex.txt", 33);
	solvept1("input.txt", 1599); 
	solvept2("input.txt", 14112); 
}

