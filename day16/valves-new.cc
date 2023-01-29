
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
	string 		path;
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



bool zvalue(path_state &p, int v)
{
	for (int i = 0; i < p.valves_left_to_open; i++)
	{
		if (p.flows_remaining[i] == v) { 
			for (int j = i; j < p.valves_left_to_open-1; j++)
				p.flows_remaining[j] = p.flows_remaining[j+1];
			p.flows_remaining[p.valves_left_to_open-1] = 0;
			return true;
		}
	}
	return false;
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


bool bigger (int a, int b) { return a > b; }


void setflows(vector<valve> &v, path_state &p)
{
	int k = 0;
	for (size_t i = 0; i < v.size(); i++)
	    if (v[i].flow > 0) p.flows_remaining[k++] = v[i].flow;
	std::sort(p.flows_remaining, p.flows_remaining + k, bigger);
	p.valves_left_to_open = k;
	//for (int i = 0; i < p.valves_left_to_open; i++)
	//    printf("Flow: %d   %d\n", i, p.flows_remaining[i]);
}

int numGTzero(vector<valve> f)
{
	int count = 0;
	for (size_t i = 0; i < f.size(); i++)
	{
		if (f[i].flow > 0) count++;
	}
	return count;
}
	

void outps(path_state &p)
{
	printf("\nMinute: %d\n", p.minute);
	printf("Open Values: %s\n", p.openvalves.c_str());
	printf("Path: %s\n", p.path.c_str());
	printf("Minute pressure: %d\n", p.pressure);
	printf("Total pressure: %d\n", p.total);
}

valve get(string nm)
{
	for (size_t i = 0; i < volcano.size(); i++)
	    if (volcano[i].nm == nm) return volcano[i];
	if (debug) printf("%s not found\n", nm.c_str());
	return volcano[0];
}



int nm_index(vector<valve> &v, string nm)
{
	for (size_t i = 0; i < v.size(); i++)
	    if (v[i].nm == nm) return i;
	if (debug) printf("nm_index::  |%s| not found\n", nm.c_str());
	return -1;
}

//
// getix returns the index into vector<valve> whose name matches to_str
//
int getix(vector<valve> &v, int from, string to_str)
{
	// if v[from].nm int v[to].nm list, remove it from v[to]'s list
	int to = nm_index(v, to_str);
	return to;
}	

int pressure(path_state &p)
{
	return p.total;
}

bool turn_on_valve(vector<valve> &v, path_state &ps, int ix, bool debug = false)
{
	//if (ps.minute >= MinutesToRun) return;
	if (debug) printf("Minute: %d turn_on_valve: %s    open: %s\n", ps.minute, v[ix].nm.c_str(), ps.openvalves.c_str());

	ps.openvalves += " " + v[ix].nm;
	//ps.path += " ^" + v[ix].nm;
	ps.valves_left_to_open--;
	//ps.pressure += v[ix].flow;	
	ps.total += ps.pressure;
	ps.pressure += v[ix].flow;
	ps.minute++;
	zvalue(ps, v[ix].flow);
	
	if (debug) printf("Path: %s  Turning on valve At %s flow: %ld  minute: %d  total: %d\n", 
		ps.path.c_str(), v[ix].nm.c_str(), v[ix].flow, ps.minute, ps.total);
    return true;
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
	fclose(f);
	spaths.clear();
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

uint64_t hash_path(path_state &p)
{
	uint64_t	rv = 0;
	rv = rv << 8 | (0xFF & p.atvalve1);
	rv = rv << 8 | (0xFF & p.atvalve2);
	rv = rv << 16 | (0xFFFF & p.pressure);
	rv = rv << 16 | (0xFFFF & p.total);
	rv = rv << 16 | (0xFFFF & p.minute);
	if (debug) printf("HP:%lx v1:%d  v2:%d  p:%d  t:%d  m:%d\n",  rv, p.atvalve1, 
		p.atvalve2, p.pressure, p.total, p.minute);
	return rv;
}

bool explored(path_state &p)
{
	uint64_t hp = hash_path(p);
	if (spaths.count(hp)) {
		if (debug) printf("FOUND::  %16.16lx\n", hp);
		return true; 
	}
	if (debug) printf("NEW::  %16.16lx\n", hp);
	return false;
}

void set_explored(path_state &p)
{
	uint64_t hp = hash_path(p);
	if (debug) printf("ADDING::  %16.16lx\n", hp);
	spaths.insert(hp);
}

//
// https://en.wikipedia.org/wiki/Breadth-first_search
//    tried depth first search, but in drilled down too fast and did not
//    seem to come back to find better solutions
//  procedure BFS(G, root) is
//   2      let Q be a queue
//   3      label root as explored
//   4      Q.enqueue(root)
//   5      while Q is not empty do
//   6          v := Q.dequeue()
//   7          if v is the goal then
//   8              return v
//   9          for all edges from v to w in G.adjacentEdges(v) do
//  10              if w is not labeled as explored then
//  11                  label w as explored
//  12                  w.parent := v
//  13                  Q.enqueue(w)
int dfsearch(vector<valve> &g, int num_bots, int start_index, int max_time)
{
	stack<path_state> q;		// (2) q will be our q for searching
// (3) create 1 or two roots (num_bots)
	path_state ps;
	ps.minute = 0;
	ps.pressure = 0;
	ps.total = 0;
	ps.path = volcano[0].nm;
	ps.atvalve1 = start_index;
	setflows(g, ps);
	
	if (num_bots > 1)
		ps.atvalve2 = start_index;
	else
		ps.atvalve2 = -1;
	ps.valves_left_to_open = numGTzero(volcano);
	int max_total = 0;

// (4) add root to q
	
	q.push(ps); 
	while (!q.empty())	// (5) while Q is not empty do
	{
		path_state	v = q.top();		//   (6)  v := Q.dequeue()  (back() - depth first search
		q.pop();
		// Check if pos_t has been 'explored', if so, continue
		if (explored(v)) {
			continue;
		}

		//   (7) if v is the goal then return v (8)
		// NEED TO LIMIT TIME - say bs.xpts * bs.ypts maximum
		if (v.minute >= max_time)
		{
			if (v.total > max_total) {
				max_total = v.total;
				//printf("Minute: %d  Total: %d\n", v.minute, v.total);
				//outps(v);
				fflush(stdout);
			}
			continue;
		}
		set_explored(v);
//
//   9          for all edges from v to w in G.adjacentEdges(v) do
//
		int ix1 = v.atvalve1;
		if ( (g[ix1].flow > 0) && v.openvalves.find(g[ix1].nm) == string::npos) {
			path_state	p = v;		//   (6)  v := Q.dequeue()  (back() - depth first search
			turn_on_valve(g, p, ix1);
			p.path += " " + to_string(p.minute) + "-*" + g[p.atvalve1].nm;
			q.push(p);
			p.total += p.pressure;
		}
		for (int i = 0; i < (int) g[ix1].tunix.size(); i++)
		{
			path_state	p = v;		//   (6)  v := Q.dequeue()  (back() - depth first search
			// if g[v.atvalve1]
			p.atvalve1 = g[ix1].tunix[i];
	
			p.minute++;
			p.path += " " + to_string(p.minute) + "-" + g[p.atvalve1].nm;
			p.total += p.pressure;
			q.push(p);

		}

		// it will add to the Q and mark explored
//  10              if w is not labeled as explored then
//  11                  label w as explored
//  12                  w.parent := v	// NOT NEEDED
//  13                  Q.enqueue(w)

	}
	return max_total;
}



void solvept1(const char *fn, int answer)
{
	init(fn);
	//output(volcano);
	int aa = nm_index(volcano, "AA");
	
	int maxp = dfsearch(volcano, 1, aa, 30);

	printf("Total Flow: %d\n", maxp);
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
	//solvept2("ex.txt", 1651);
	return 0;
}
