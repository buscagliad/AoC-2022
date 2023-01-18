
#include <cstring>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cctype>
#include <string>

using namespace std;

#define DEBUG 0
#define MAX_MINUTE 30

int MinutesToRun = MAX_MINUTE;

typedef enum {
	vs_move,	// if in this state, will move (state -> vs_flow)
	vs_flow 	// if in this state, can turn on flow if flow > 0;
} valve_state;


typedef struct valve_s {
	int index;
	string nm;
	long flow;
	int minute;
	vector<string> tunnels;
	vector<int>  tunix;
} valve;

int MaxPressure[MAX_MINUTE+1];


vector<valve> volcano;
int volcano_valve_flows = 0;

int numGTzero(vector<valve> f)
{
	int count = 0;
	for (size_t i = 0; i < f.size(); i++)
	{
		if (f[i].flow > 0) count++;
	}
	return count;
}
	


valve get(string nm)
{
	for (size_t i = 0; i < volcano.size(); i++)
	    if (volcano[i].nm == nm) return volcano[i];
	printf("%s not found\n", nm.c_str());
	return volcano[0];
}



int nm_index(vector<valve> &v, string nm)
{
	for (size_t i = 0; i < v.size(); i++)
	    if (v[i].nm == nm) return i;
	printf("nm_index::  |%s| not found\n", nm.c_str());
	return -1;
}

void getchars(char *inp, char *nm, int num)
{
	while (num-- > 0) *nm++ = *inp++;
	*nm = 0;
}

void init(const char *fn)
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
	volcano_valve_flows = numGTzero(volcano);
	fclose(f);
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


// starting with AA, find shortest path to each valve
// starting with each valve, find shortest path to each valve

// typedef struct {
// 	int		from;
// 	int		to;
// 	int		minutes;
// 	int		flow;
// } spath_t;
	
typedef struct {
	int		from;
	int		to;
	int		minutes;
	int		flow;
} spath_t;

typedef struct {
	int		cur_flow;
	int		minute;
	int		valve;
	int		total;
	int		visited[50];
	int		num_visited;
} v_state;

void	out(v_state &v)
{
	printf("@%s  Min: %d  Flow: %d  Total: %d  [%d", 
		volcano[v.valve].nm.c_str(), v.minute, v.cur_flow, v.total, v.visited[0]);
	for (int i = 1; i < v.num_visited; i++)
	    printf(",%d", v.visited[i]);
	printf("]\n");
}

v_state max_vs;

void	shortestPath(vector<valve> &v, int s, int e, int minutes, spath_t &np)
{
	if (minutes == 0)
	{
		np.from = s;
		np.to = e;
		np.minutes = -1;
		np.flow = v[e].flow;
	}
	if (minutes > 50) return;
	
	if (s == e)
	{ 
		if (np.minutes < 0) np.minutes = minutes+1;
		if (minutes < np.minutes) np.minutes = minutes+1;
		return;
	}
	if ( (np.minutes > 0) && (np.minutes < minutes) ) return;
	for (size_t j = 0; j < v[s].tunnels.size(); j++)
	{
		int k = v[s].tunix[j];
		if (k == s) continue;
		//printf("From %d  To: %d  Minute: %d\n", k, e, minutes);
		shortestPath(v, k, e, minutes+1, np);
	}
}

void getPaths(vector<valve> &v, vector<spath_t> &spaths, bool debug = true)
{
	int aa = nm_index(v, "AA");
	spath_t np;
	for (size_t j = 0; j < v.size(); j++)
	{
		if (v[j].flow > 0)
		{
			shortestPath(v, aa, j, 0, np);
			spaths.push_back(np);
		}
	}
	for (size_t i = 0; i < v.size(); i++)
	{
		if (v[i].flow == 0) continue;
		for (size_t j = 0; j < v.size(); j++)
		{
			if (v[j].flow == 0) continue;
			if (i == j) continue;
			shortestPath(v, i, j, 0, np);
			spaths.push_back(np);
		}
	}
	for (size_t i = 0; debug && i < spaths.size(); i++)
	{
		printf("From %s[%2d] to %s[%2d] Flow: %2d  Minutes: %2d\n",
			v[spaths[i].from].nm.c_str(), spaths[i].from, v[spaths[i].to].nm.c_str(), spaths[i].to, 
			spaths[i].flow, spaths[i].minutes);
	}
}


bool did_visit(int n, v_state &s)
{
	for (int i = 0; i < s.num_visited; i++)
		if (n == s.visited[i]) return true;
	return false;
}

void getMax(vector<spath_t> &sp, v_state vs, int minutes)
{
	if (vs.num_visited == volcano_valve_flows)
	{
		vs.total += minutes * vs.cur_flow;
		//printf("VVV MAX @ %d  flow: %d  total: %d\n", vs.minute, vs.cur_flow, vs.total);
		vs.minute = 0;
		minutes = 0;
	}
	if (minutes <= 0)
	{
		if (max_vs.total < vs.total) 
		{
			if (minutes == 0)
			{
				max_vs = vs;
				//printf("MAX @ %d  flow: %d  total: %d\n", max_vs.minute, max_vs.cur_flow, max_vs.total);
			}
		}
		return;
	}
	for (size_t i = 0; i < sp.size(); i++)
	{
		if (sp[i].from != vs.valve) continue;
		if (did_visit(sp[i].to, vs)) continue;
		v_state	nv = vs;
		int dm = minutes - sp[i].minutes;
		if (dm < 0) 
		{
			nv.total += minutes * nv.cur_flow;
			//out(nv);
		}
		else
		{
			nv.minute += sp[i].minutes;
			nv.valve = sp[i].to;
			nv.total += sp[i].minutes * nv.cur_flow;
			nv.cur_flow += sp[i].flow;
			nv.visited[nv.num_visited++] = sp[i].to;
		}
		//out(nv);
		getMax(sp, nv, dm);
	}
}

void solvept1(const char *fn, int answer)
{
	init(fn);
	//output(volcano);
	vector<spath_t> spaths;
	getPaths(volcano, spaths);
	int total = -1;
	int from = nm_index(volcano, "AA");
	v_state  vs;
	vs.cur_flow = 0;
	vs.minute = 0;
	vs.valve = from;
	vs.total = 0;
	vs.num_visited = 0;
	max_vs = vs;
	getMax(spaths, vs, 30);
	total = max_vs.total;
	out(max_vs);
	printf("Flow: %d\n", total);

	if (total == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %d\n", answer);
}

void solvept2(const char *fn, int answer)
{
	init(fn);
	output(volcano);
	vector<valve> v = volcano;

	int total = max_vs.total;

	printf("\n\ngPath\n");
	printf("Total Pressure: %d\n", total);

	if (total == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %d\n", answer);
}


int main()
{
	solvept2("ex.txt", 1707);

	return 0;
	solvept1("input.txt", 1906);
	solvept1("ex.txt", 1651);

	return 0;
	solvept1("ex.txt", 1651);
	solvept1("input.txt", 1906);  
	return 0;
}

