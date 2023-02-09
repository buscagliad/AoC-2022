
#include <cstring>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cctype>
#include <string>
#include <queue>
#include <stack>
#include <set>
#include <map>




using namespace std;

#define DEBUG 0
int debug = DEBUG;
#define MAX_MINUTE 30
#define MAX_FLOW 15	// cheating - counting number of non-zero valves in input.txt

typedef struct valve_s {
	int index;
	string nm;
	long flow;
	int minute;
	//int times_visited;
	vector<string> tunnels;
	vector<int>  tunix;
} valve;

vector<valve> volcano;



#define GRAPH_SIZE 17

typedef struct {
	int    size;
	string nm[GRAPH_SIZE];
	int    flow[GRAPH_SIZE];
	int    minutes[GRAPH_SIZE][GRAPH_SIZE];
} graph_t;	// this compressed graph will take vector<valve> --> vector<graph_t>


int nm_index(graph_t g, string nm)
{
	for (int i = 0; i <  g.size; i++)
	    if (g.nm[i] == nm) return i;
	if (debug) printf("nm_index::  |%s| not found\n", nm.c_str());
	return -1;
}

void dump(graph_t &g)
{
	printf("    ");
	for (int i = 0; i < g.size; i++)
	{
		printf("  %2.2s", g.nm[i].c_str());
	}
	printf("\n");
	for (int i = 0; i < g.size; i++)
	{
		printf("  %2.2s", g.nm[i].c_str());
		for (int j = 0; j <  g.size; j++)
		{
			int	f2 = g.minutes[i][j];
			if (f2 >= 0) printf("%4d", f2);
			else printf("    ");
		}
		printf("\n");
	}
	fflush(stdout);
}

void zerog(graph_t &g)
{
	g.size = 0;
	for (int i = 0; i < GRAPH_SIZE; i++)
	{
		g.nm[i] ="";
		g.flow[i] = -1;
		for (int j = 0; j < GRAPH_SIZE; j++)
		{
			g.minutes[i][j] = -1;
		}
	}
}

#define SET_BIT(u, n)	u |= 1UL << n
#define IS_BIT(u, n)	(u & (1UL << n))

typedef struct {
	string		path;
	int			index;
	int			minutes;
	uint64_t 	visited;
} search_t;

void out(search_t &t)
{
	printf("Path %s   Min: %d\n", t.path.c_str(), t.minutes);
}

int	time2travel(vector<valve> &vv, int ofrom, int to, bool debug = false)
{
	queue<search_t> st;
	if (debug) printf("\n\nTime 2 Travel from %s to %s\n", vv[ofrom].nm.c_str(),  vv[to].nm.c_str());
	
	for (int i = 0; i < (int) vv[ofrom].tunix.size(); i++)
	{
		search_t tt;
		tt.visited = 0;
		tt.index = vv[ofrom].tunix[i];
		tt.minutes = 1;
		if (tt.index == to) return tt.minutes;
		SET_BIT(tt.visited, ofrom);
		SET_BIT(tt.visited, tt.index);
		tt.path = vv[ofrom].nm;
		tt.path += " " + vv[tt.index].nm;
		if (debug) out(tt);
		st.push(tt);
	}
	while (!st.empty())
	{
		search_t tt = st.front();
		int from = tt.index;
		st.pop();
		if (tt.index == to)
		{
			if (debug) printf("Found %s to %s in %d minutes - Q: %d.\n", vv[ofrom].nm.c_str(), vv[to].nm.c_str(), tt.minutes, (int) st.size());
			return tt.minutes;
		}			

		for (int i = 0; i < (int) vv[from].tunix.size(); i++)
		{
			search_t xt = tt;
			xt.minutes++;
			xt.path += " " + vv[vv[from].tunix[i]].nm;
			xt.index = vv[from].tunix[i];
			if (IS_BIT(xt.visited, xt.index)) continue;
			SET_BIT(xt.visited, xt.index);
			if (debug) out(xt);
			
			st.push(xt);
		}
	}
	return -1;
}

bool compress(vector<valve> &vv, graph_t &cg, bool debug = false)
{
	//
	// fill in flow and 'AA'
	//
	int	gf = 0;
	int gt = 0;
	cg.size = 0;
	for (int from = 0; from < (int) vv.size(); from++)
	{
		if (vv[from].nm != "AA"  &&  vv[from].flow == 0) continue;
		cg.nm[cg.size] = vv[from].nm;
		cg.flow[cg.size] = vv[from].flow;
		cg.size++;
	}
	//
	// sort from high flow to low flow - AA always at 0
	//
	
	for (int i =  0; i < cg.size; i++)
	{
		for (int j = i + 1; j < cg.size; j++)
		{
			if (cg.flow[i] < cg.flow[j])
			{
				std::swap(cg.flow[i], cg.flow[j]);
				std::swap(cg.nm[i], cg.nm[j]);
			}
		}
	}	
	

	//
	// go from each g.nm to each other g.nm within v
	// to determine the shortest path
	//
	for (int from = 0; from < (int) vv.size(); from++)
	{
		for (int to = 0; to < (int) vv.size(); to++)
		{
			if (to == from) continue;
			if (vv[to].flow == 0) continue;
			gf = nm_index(cg, vv[from].nm);
			gt = nm_index(cg, vv[to].nm);
			if (gf < 0 || gt < 0) { if (debug) printf("gf: %d  gt: %d  from: %d  to: %d\n", gf, gt, from, to);
				continue; }
			cg.minutes[gf][gt] = time2travel(vv, from, to, debug); //6; //
			if (debug) printf("gf: %d  gfn: %s  gt: %d  gtn: %s  min: %d\n",
				gf, cg.nm[gf].c_str(), gt, cg.nm[gt].c_str(), cg.minutes[gf][gt]);
		}
	}


	return true;
}

typedef struct {	
	int	index;
	int minutes;
} path_state;

typedef struct {
	string path;
	int	flow;
	uint64_t visited;
	path_state ps;
	path_state e;	// elephant
} maxflow_t;

//
// note:: g is ordered from highest flow to lowest flow
// we assumed only 2 minuts from now to open valve
int max_remaining(graph_t &g, maxflow_t m, int MinutesToRun)
{
	int maxP = m.flow;
	for (int s = 0; s < g.size; s++)
	{
		if (!IS_BIT(m.visited, s) ){
			if (m.e.minutes + 2 < MinutesToRun)
			{
				m.e.minutes += 2;
				maxP += g.flow[s] * (MinutesToRun - m.e.minutes);
				SET_BIT(m.visited, s);
			}
			if (m.ps.minutes + 2 < MinutesToRun)
			{
				m.ps.minutes += 2;
				maxP += g.flow[s] * (MinutesToRun - m.ps.minutes);
				SET_BIT(m.visited, s);
			}
		}
	}
	//printf("Minute: %d  valves open: %d  Total: %d  Pressure: %d   Computed Max: %d  Current Max: %d\n",
	//	p.minute, p.valves_left_to_open, p.total, p.pressure, maxP, maxps.total);
	return maxP;
}


void out(maxflow_t &t)
{
	printf("path: %s   flow: %d   minute: %d\n", t.path.c_str(), t.flow, t.ps.minutes);
}


#define ADD_PATH(V, n)	V.path += g.nm[n] + "[" + to_string(V.ps.minutes) + "] "


bool add_route(graph_t &g, maxflow_t &mt, bool el, int from, int to, int max_minutes, bool debug = false)
{
	if ( IS_BIT(mt.visited, to)) return false;
	
	if (el)
	{
		mt.e.minutes += g.minutes[from][to] + 1;
		mt.flow += (max_minutes - mt.e.minutes) * g.flow[to];
		mt.e.index = to;
		if (debug) mt.path += g.nm[to] + "<" + to_string(mt.e.minutes) + "> ";	
	}
	else
	{
		mt.ps.minutes += g.minutes[from][to] + 1;
		mt.flow += (max_minutes - mt.ps.minutes) * g.flow[to];
		mt.ps.index = to;
		if (debug) mt.path += g.nm[to] + "[" + to_string(mt.ps.minutes) + "] ";	
	}
	SET_BIT(mt.visited, to);
	
	if (debug) out(mt);
	return true;
}

int max_product(graph_t &g, int &from, uint64_t &visited, int &now, int maxtime)
{
	int	maxi = -1;
	int maxv = 0;
	int maxt = 0;
	
	for (int i = 0; i < g.size; i++)
	{
		if (!IS_BIT(visited, i))
		{
			int mnow = g.minutes[from][i] + now + 1;
			if (mnow > maxtime) continue;
			int flow = (g.flow[i] * (maxtime - mnow) );
			if (flow > maxv)
			{
				maxi = i;
				maxv = flow;
				maxt = mnow;
			}
		}
	}
	if (maxi > -1)
	{
		SET_BIT(visited, maxi);
		now = maxt;
		from = maxi;
	}
	else
	{
		now = maxtime;
	}
	return maxv;
}

maxflow_t good_solution(graph_t &g, int max_minutes, bool elephant = false, bool debug = false)
{
	int	aa = g.size - 1; 
	maxflow_t tt;
	int flow = 0;
	tt.flow = 0;
	tt.ps.index = aa;
	tt.ps.minutes = 0;
	tt.e.index = aa;
	if (elephant)
		tt.e.minutes = 0;
	else
		tt.e.minutes = max_minutes;
	tt.visited = 0;
	if (debug) ADD_PATH(tt, 0);
	SET_BIT(tt.visited, aa);
	bool done = false;
	while(!done)
	{
		int	ei = tt.e.index;
		int hi = tt.ps.index;
		done = true;
		if (tt.e.minutes < max_minutes)
		{
			flow = max_product(g, ei, tt.visited, tt.e.minutes, max_minutes);
			if (flow > 0)
			{
				done = false;
				tt.flow += flow;
				if (debug) printf("{E} From: %d  To: %d   Now: %d   Flow: %d  Total flow: %d\n", tt.e.index, ei, tt.e.minutes, g.flow[ei], tt.flow);
				tt.e.index = ei;
			}
		}
			
		if (tt.ps.minutes < max_minutes)
		{
			flow = max_product(g, hi, tt.visited, tt.ps.minutes, max_minutes);
			if (flow > 0)
			{
				done = false;
				tt.flow += flow;
				if (debug) printf("{E} From: %d  To: %d   Now: %d   Flow: %d  Total flow: %d\n", tt.ps.index, hi, tt.ps.minutes, g.flow[hi], tt.flow);
				tt.ps.index = hi;
			}
		}
	}
	return tt;
}

std::map<uint64_t, bool> vmap;

uint64_t maxhash(maxflow_t &tt)
{
	return ( ( ((uint64_t)tt.flow        & 0xFFFF)        ) + 
			 ( ((uint64_t)tt.visited     & 0xFFFF)  << 16 ) +
	         ( ((uint64_t)tt.ps.minutes  & 0xFFFF)  << 32 ) + 
	         ( ((uint64_t)tt.e.minutes   & 0xFFFF)  << 48 ) );
}

bool alreadyVisited(maxflow_t &tt)
{
	uint64_t	hash = maxhash(tt);
	bool rv = (vmap.find(hash) != vmap.end());
	vmap[hash] = true;
	return rv;
}
	
#define DFS
	

int maxflow(graph_t &g, int max_minutes, bool elephant = false, bool debug = false)
{
	if (debug) dump(g);
	int	aa = nm_index(g, "AA");
	#ifdef BFS
	queue<maxflow_t> st;
	#else
	stack<maxflow_t> st;
	#endif
	int max_flow = 0;
	uint64_t	all_visited = (1 << g.size) - 1;
	maxflow_t tt;
	tt.flow = 0;
	tt.ps.index = aa;
	tt.ps.minutes = 0;
	tt.e.index = aa;
	if (elephant)
		tt.e.minutes = 0;
	else
		tt.e.minutes = max_minutes;
	tt.visited = 0;
	if (debug) ADD_PATH(tt, 0);
	SET_BIT(tt.visited, aa);
	st.push(tt);
	
	//
	// find a 'good' solution
	//
	tt = good_solution(g, max_minutes, elephant);
	st.push(tt);

	while (!st.empty())
	{
		#ifdef BFS
		maxflow_t tt = st.front();
		#else
		maxflow_t tt = st.top();
		#endif
		st.pop();
		if (alreadyVisited(tt)) continue;
		if (max_remaining(g, tt, max_minutes) < max_flow) {
			//printf("[%ld] Time Me: %d  E: %d  MR: %d  MAX: %d\n", st.size(), tt.ps.minutes, tt.e.minutes, max_remaining(g, tt, max_minutes), max_flow);
			continue;
		}
		for (int i = 0; i < g.size; i++)  // elephant index is i - only do once if elephant is false
		{
			if (!elephant) i = g.size;
			for (int j = 0; j < g.size; j++) // my index is j
			{
				maxflow_t xt = tt;
				if (xt.visited == all_visited)
				{
					xt.ps.minutes = max_minutes;
					xt.e.minutes = max_minutes;
				}
				else if (IS_BIT(xt.visited, i))
				{
					continue;
				}
				else if (IS_BIT(xt.visited, j))
				{
					continue;
				}
				if (xt.ps.minutes + g.minutes[xt.ps.index][j] <= max_minutes)
				{
					add_route(g, xt, false, xt.ps.index, j, max_minutes);
				}
				else xt.ps.minutes = max_minutes;
				if (elephant)
				{
					if (xt.e.minutes + g.minutes[xt.e.index][i] <= max_minutes)
					{
						add_route(g, xt, true, xt.e.index, i, max_minutes);
					}
					else xt.e.minutes = max_minutes;
				}
				if (xt.ps.minutes >= max_minutes && xt.e.minutes >= max_minutes)
				{
					if (xt.flow > max_flow)
					{
						max_flow = xt.flow;
						if (debug) printf("Q: %ld Found solution at %d  flow is: %d\n", st.size(), xt.ps.minutes, xt.flow);
						if (debug) out(xt);
						fflush(stdout);
					}
					continue;
				}
				st.push(xt);
			}
		}
		if (debug) printf("%ld currently at %d  flow is: %d\n", st.size(), tt.ps.minutes, tt.flow);
	}
	return max_flow;
}


int nm_index(vector<valve> &v, string nm)
{
	for (size_t i = 0; i < v.size(); i++)
	    if (v[i].nm == nm) return i;
	if (debug) printf("nm_index::  |%s| not found\n", nm.c_str());
	return -1;
}


void getchars(char *inp, char *nm, int num)
{
	while (num-- > 0) *nm++ = *inp++;
	*nm = 0;
}

void init(const char *fn, graph_t &g)
{
	FILE *f = fopen(fn, "r");
	char inl[100];
	char *inp = inl;
	char nm[3];
	volcano.clear();
	printf("\n\nFile: %s\n", fn);
	int index = 0;
	while (!feof(f))
	{
		valve v;
		fgets(inl, 100, f);
		inp = inl;
		if (feof(f)) break;
		inp += 6;	// move past "Valve "
		getchars(inp, nm, 2);
		v.nm = nm;
		inp = strchr(inp, '=');
		inp++;	// get past rate=
		v.flow = atoi(inp);
		inp = strstr(inp, "valve");
		inp += 5;
		if (*inp == 's') inp++;
		inp++; // move to first valve name
		//printf("Valves: %s", inp);
		while ( (*inp) && (*inp != '\n') )
		{
			getchars(inp, nm, 2);
			string st = nm;
			//printf("%s\n", st.c_str());
			v.tunnels.push_back(st);
			inp+= 2;
			if (*inp == ',') inp += 2;
		}
		v.index = index++;
		volcano.push_back(v);
	}
	for (size_t i = 0; i < volcano.size(); i++)
	{
		for (size_t j = 0; j < volcano[i].tunnels.size(); j++)
		{
			int tunix = nm_index(volcano, volcano[i].tunnels[j]);
			volcano[i].tunix.push_back(tunix);
		}
	}
	fclose(f);
	zerog(g);
}

void output(vector<valve> &v)
{
	for (size_t i = 0; i < v.size(); i++)
	{
		printf("Valve %s[%ld] has flow rate=%ld; tunnels lead to valve", 
			v[i].nm.c_str(), i, v[i].flow);
		if (v[i].tunnels.size() > 1) { printf("s"); }
		printf(" %s[%d]", v[i].tunnels[0].c_str(), v[i].tunix[0]);
		for (size_t t = 1; t < v[i].tunnels.size(); t++)
		{
			printf(", %s[%d]", v[i].tunnels[t].c_str(), v[i].tunix[t]);
		}
		printf("\n");
	}
}





void solvept1(const char *fn, int answer)
{
	graph_t g;
	init(fn, g);
	compress(volcano, g);
	int maxp = maxflow(g, 30);

	printf("Part 1: Total Flow: %d\n", maxp);
	if (maxp == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %d\n", answer);
}

void solvept2(const char *fn, int answer)
{
	graph_t g;
	init(fn, g);
	compress(volcano, g);
	//output(volcano);	
	int maxp = maxflow(g, 26, true, false);

	printf("Part 2:  Elephant and me total Flow: %d\n", maxp);
	if (maxp == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %d\n", answer);
}

int main()
{
	solvept1("ex.txt", 1651);
	solvept1("input.txt", 1906);  // 1833 is too low!!  2544 is too high!!
	solvept2("ex.txt", 1707);
	solvept2("input.txt", 2548);
}
