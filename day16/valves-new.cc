
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

using namespace std;

#define DEBUG 0
int debug = DEBUG;
#define MAX_MINUTE 30
#define MAX_FLOW 15	// cheating - counting number of non-zero valves in input.txt

int MinutesToRun = MAX_MINUTE;


typedef struct path_s {
	int 		minute;
	string 		openvalves;
	int			atvalve1;
	int			atvalve2;
	int			pressure;
	int			new_pressure;
	int			total;
	int			valves_left_to_open;
	int			flows_remaining[MAX_FLOW];	// index of positive flow valves
} path_state;

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

// need to update these two functions 
// has the state into a unit32_t and insert into an
// ordered list - set_explored
// for explored, hash the state, search for it, if found, return true

std::set<uint64_t> spaths;	// search paths

typedef struct {
	int 	flow;
	int		minutes;
} min_t;	// this structure will create a graph of ONLY flow positive valves, and the 
			// minimum time to get to this valve from any other of the valves

void out(min_t &t)
{
	printf(" %d   Min: %d\n", t.flow, t.minutes);
}

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
#define IS_BIT(u, n)	u & (1UL << n)

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
	string path;
	int	index;
	int	flow;
	int minutes;
	int visited;
} maxflow_t;

void out(maxflow_t &t)
{
	printf("path: %s   flow: %d   minute: %d\n", t.path.c_str(), t.flow, t.minutes);
}

#define ADD_PATH(V, n)	V.path += g.nm[n] + "[" + to_string(V.minutes) + "] "
#define ADD_FLOW(V, n)	V.flow += (max_minutes - V.minutes) * g.flow[n]
#define ADD_MINUTES(V, f, t)	V.minutes += g.minutes[f][t] + 1;
#define ALL_VISITED(n, v)	

int maxflow(graph_t &g, int max_minutes, bool debug = false)
{
	if (debug) dump(g);
	int	aa = nm_index(g, "AA");
	queue<maxflow_t> st;
	int max_flow = 0;
	int	all_visited = (1 << g.size) - 1;
	for (int i = 0; i < (int) g.size; i++)
	{
		if (i == aa) continue;
		maxflow_t tt;
		tt.path = "AA[0] ";
		tt.visited = 0;
		tt.flow = 0;
		tt.index = i;
		tt.minutes = 0;
		ADD_MINUTES(tt, aa, i);
		ADD_FLOW(tt, i);
		ADD_PATH(tt, i);
		SET_BIT(tt.visited, i);
		SET_BIT(tt.visited, aa);
		st.push(tt);
		if (debug) { printf("Q size: %ld  ", st.size()); out(tt); }
	}
	while (!st.empty())
	{
		maxflow_t tt = st.front();
		
		st.pop();
		for (int i = 0; i < g.size; i++)
		{
			maxflow_t xt = tt;
			if (xt.visited == all_visited)
			{
				xt.minutes = max_minutes;
			}
			else if (IS_BIT(xt.visited, i))
			{
				continue;
			}
			else if (xt.minutes + g.minutes[xt.index][i] <= max_minutes)
			{
				ADD_MINUTES(xt, xt.index, i);
				ADD_FLOW(xt, i);
				ADD_PATH(xt, i);
				SET_BIT(xt.visited, i);
				xt.index = i;
				if (debug) { printf("Q size: %ld  ", st.size()); out(xt); }
			}
			else xt.minutes = max_minutes;
			if (xt.minutes >= max_minutes)
			{
				if (xt.flow > max_flow)
				{
					max_flow = xt.flow;
					if (debug) printf("Q: %ld Found solution at %d  flow is: %d\n", st.size(), xt.minutes, xt.flow);
					if (debug) out(xt);
					fflush(stdout);
				}
				continue;
			}
			st.push(xt);
		}
	}
	return max_flow;
}


int max_remaining(vector<valve> &v, path_state &p)
{
	//if (p.atvalve2 < 0) return 1000000;
	int maxP = p.total;
	int curP = p.pressure;
	int i = -1;
	for (int m = p.minute+1; m <= MinutesToRun; m++)
	{
		if (i < p.valves_left_to_open - 1) {
			i++;
			curP += p.flows_remaining[i];
		}
		maxP += curP;
	}

	return maxP;
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
	spaths.clear();
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
	printf("Flow %d\n", maxp);

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
	//output(volcano);	
	int maxp = 0;

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
	
	return 0;
	solvept2("input.txt", 2548);
	solvept2("ex.txt", 1707);
	solvept1("ex.txt", 1651);
	solvept1("input.txt", 1906);  // 1833 is too low!!  2544 is too high!!
	return 0;
}
