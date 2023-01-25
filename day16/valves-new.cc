
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
#define MAX_FLOW 15	// cheating - counting number of non-zero valves in input.txt

int MinutesToRun = MAX_MINUTE;

typedef enum {
	vs_move,	// if in this state, will move (state -> vs_flow)
	vs_flow 	// if in this state, can turn on flow if flow > 0;
} valve_state;

typedef struct path_s {
	int 		minute;
	string 		openvalves;
	string 		path;
	int			atvalve1;
	int			prevvalve1;
	int			atvalve2;
	int			prevvalve2;
	int			pressure;
	int			new_pressure;
	int			total;
	int			valves_left_to_open;
	int			flows_remaining[MAX_FLOW];
} path_state;

path_state maxps;
int earlyminute = 40;

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

int max_remaining(path_state &p)
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
	//printf("Minute: %d  valves open: %d  Total: %d  Pressure: %d   Computed Max: %d  Current Max: %d\n",
	//	p.minute, p.valves_left_to_open, p.total, p.pressure, maxP, maxps.total);
	return maxP;
}


typedef struct valve_s {
	int index;
	string nm;
	long flow;
	int minute;
	//int times_visited;
	vector<string> tunnels;
	vector<int>  tunix;
	valve_state state;
} valve;

vector<valve> volcano;
int	shortestPath[50][50];


// shortestPath[i][j] is the minimum number of minutes required to go
//     from volcano[i] to volcano[j]
// OBVIOUSLY shortestPath[k][k] is zero
//           shortestPath[k][m] = shortestPath[m][k]

void out(vector<valve> &v, int shortestPath[50][50])
{
	for (int i = 0; i < (int) v.size(); i++) printf(",%s", v[i].nm.c_str());
	printf("\n");
	for (int i = 0; i < (int) v.size(); i++)
	{
		printf("%s", v[i].nm.c_str());
		for (int j = 0; j < (int) v[j].tunix.size(); j++)
		{
			printf(",%d", shortestPath[i][j]);
		}
		printf("\n");
	}
}
	
void initShortPaths(vector<valve> &v, int shortestPath[50][50])
{
	for (int i = 0; i < (int) v.size(); i++)
	{
		for (int k = 0; k < (int) v[k].tunix.size(); k++)
		{
			shortestPath[i][k] = 1;
			shortestPath[k][i] = 1;
		}
	}
}

void getShortPaths(vector<valve> &v, int shortestPath[50][50], int pathcnt = 1)
{
	for (int i = 0; i < (int) v.size(); i++)
	{
		for (int j = i + 1; j < (int) v.size(); j++)
		{
			// look for each connector nearby, if the path is shorter
			// that the current path, select the path
			for (int k = 0; k < (int) v[j].tunix.size(); k++)
			{
				//int	c = v[i].tunix[k];
				if (shortestPath[i][k] == 0) shortestPath[i][k] = pathcnt;
				if (v[i].tunix[k] == i)
				{
					shortestPath[i][k] = pathcnt + 1;
					shortestPath[k][i] = pathcnt + 1;
				}
			}
		}
	}
}


bool bigger (int a, int b) { return a > b; }

void setflows(vector<valve> &v, path_state &p)
{
	int k = 0;
	for (size_t i = 0; i < v.size(); i++)
	    if (v[i].flow > 0) p.flows_remaining[k++] = v[i].flow;
	std::sort(p.flows_remaining, p.flows_remaining + k, bigger);
	
	for (int i = 0; i < p.valves_left_to_open; i++)
	    printf("Flow: %d   %d\n", i, p.flows_remaining[i]);
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

void turn_on_valve(vector<valve> &v, path_state &ps, int ix, bool debug = false)
{
	//if (ps.minute >= MinutesToRun) return;
	if (debug) printf("Minute: %d turn_on_valve:  flows[ix]=%ld   open: %s\n", ps.minute, v[ix].flow, ps.openvalves.c_str());
	if ( (v[ix].flow == 0) || ps.openvalves.find(v[ix].nm) != string::npos) return;

	ps.openvalves += " " + v[ix].nm;
	ps.valves_left_to_open--;
	//ps.pressure += v[ix].flow;	
	ps.new_pressure += v[ix].flow;	
	zvalue(ps, v[ix].flow);
	
	if (0) printf("Path: %s  Turning on valve At %s flow: %ld  minute: %d\n", 
		ps.path.c_str(), v[ix].nm.c_str(), v[ix].flow, ps.minute);

}

#define UPDATE(ps)	ps.minute++; ps.total += ps.pressure; ps.pressure += ps.new_pressure; ps.new_pressure = 0;
void		next_move(vector<valve> &v, path_state ps)
{
	//ps.minute++;
	//ps.total += ps.pressure;
	//ps.pressure += ps.new_pressure;
	//ps.new_pressure = 0;
	UPDATE(ps);
	if (max_remaining(ps) <= maxps.total) return;
	if ( (ps.valves_left_to_open == 0) && (ps.minute < MinutesToRun) )
	{
		if (ps.minute < earlyminute)
		{
			earlyminute = ps.minute;
			printf("Early minute: %d   pressure: %d   total: %d  remain: %d\n", earlyminute, ps.pressure, ps.total, (MinutesToRun-ps.minute)*ps.pressure);
		}
		ps.total += ps.pressure * (MinutesToRun - ps.minute);
		ps.minute = MinutesToRun;
	}
	if (ps.minute == MinutesToRun && maxps.total < ps.total) 
	{
		maxps = ps;
		printf("Minute: %d   Total: %d\n",  maxps.minute, maxps.total);
		fflush(stdout);
		return;
	}
	if (ps.minute >= MinutesToRun) return;

	int ix1 = ps.atvalve1;
	int ix2 = ps.atvalve2;

	bool valve1 = false;
	bool valve2 = false;

	//
	// if both valve1 and valve2 have not been opened
    //if (valve1 || valve2) return;
	//UPDATE(pnn);
	path_state pnn = ps;
	for (int i = 0; i < (int)v[ix1].tunnels.size(); i++)
	{
		//if (v[ix1].tunix[i] == pnn.prevvalve1) continue;
		pnn.atvalve1 = v[ix1].tunix[i];
		pnn.prevvalve1 = ix1;
		for (int j = 0; j < (int)v[ix2].tunnels.size(); j++)
		{
			//if (v[ix2].tunix[j] == pnn.prevvalve2) continue;
			path_state p = pnn;
			
			p.atvalve2 = v[ix2].tunix[j];
			p.prevvalve2 = ix2;
			next_move(v, p);
		}
	}
	// is there a valve to open?
	if ( (v[ix1].flow > 0) && ps.openvalves.find(v[ix1].nm) == string::npos) valve1 = true;
	path_state pyn = ps;
	// move me
	if ( (v[ix2].flow > 0) && ps.openvalves.find(v[ix2].nm) == string::npos) valve2 = true;
	// move elephant
	path_state pny = ps;
	path_state pyy = ps;
	//
	// turn on both valves
	if (valve1 && valve2)
	{
		//UPDATE(pyy);
		turn_on_valve(v, pyy, ix1);
		turn_on_valve(v, pyy, ix2); 
		pyy.prevvalve1 = ix1;
		pyy.prevvalve2 = ix2;
		next_move(v, pyy);
	}		
	//
	// turn on valve 1 - move path2 to each possible square
	if (valve1 && !valve2)
	{
		//UPDATE(pyn);
		turn_on_valve(v, pyn, ix1); 
		pyn.prevvalve1 = ix1;
		for (int i = 0; i < (int)v[ix2].tunnels.size(); i++)
		{
			//if (v[ix2].tunix[i] == pyn.prevvalve2) continue;
			path_state p = pyn;
				
			p.atvalve2 = v[ix2].tunix[i];
			p.prevvalve2 = ix2;
			next_move(v, p);
		}
	}
	//
	// turn on valve 2 - move path1 to each possible square
	if (!valve1 && valve2)
	{
		//UPDATE(pny);
		turn_on_valve(v, pny, ix2);
		pny.prevvalve2 = ix1;
		
		for (int i = 0; i < (int)v[ix1].tunnels.size(); i++)
		{
			path_state p = pny;
			//if (v[ix1].tunix[i] == p.prevvalve1) continue;
			p.atvalve1 = v[ix1].tunix[i];
			p.prevvalve1 = ix1;
			next_move(v, p);
		}
	
	}
	

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
		v.state = vs_flow;
		volcano.push_back(v);
	}
	for (size_t i = 0; i < volcano.size(); i++)
	{
		for (size_t j = 0; j < volcano[i].tunnels.size(); j++)
		{
			int tunix = nm_index(volcano, volcano[i].tunnels[j]);
			volcano[i].tunix.push_back(tunix);
			shortestPath[i][j] = 0;
		}
	}
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

void solvept1(const char *fn, int answer)
{
	printf("Entering solvept1\n");
	init(fn);
	output(volcano);
	vector<valve> v = volcano;
	//output(nv);
	path_state ps;
	ps.minute = 1;
	ps.pressure = 0;
	ps.new_pressure = 0;
	ps.total = 0;
	ps.path = volcano[0].nm;
	ps.valves_left_to_open = numGTzero(volcano);
	setflows(volcano, ps);
	int aa = nm_index(volcano, "AA");

	outps(ps);
    MinutesToRun = 30;
    maxps = ps;

	for (size_t m = 0; m < volcano[aa].tunix.size(); m++)
	{
		path_state nps = ps;
		nps.prevvalve1 = aa;
		nps.atvalve1 = v[aa].tunix[m];
		nps.prevvalve2 = -1;
		nps.atvalve2 = -1;
		next_move(v, nps);
	}
	printf("\n\ngPath\n");
	outps(maxps);
	printf("Flow: %d\n", maxps.pressure);

	if (maxps.total == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %d\n", answer);
	printf("Exitting solvept1\n");
}



void solvept2(const char *fn, int answer)
{
	init(fn);
	output(volcano);

    MinutesToRun = 26;
	int aa = nm_index(volcano, "AA");

	for (size_t k = 0; k < volcano[aa].tunix.size() - 1; k++)
	{
		for (size_t m = k+1; m < volcano[aa].tunix.size(); m++)
		{
			path_state nps;
			nps.prevvalve1 = aa;
			nps.atvalve1 = volcano[aa].tunix[k];
			nps.prevvalve2 = aa;
			nps.atvalve2 = volcano[aa].tunix[m];
			next_move(volcano, nps);
		}
	}
	printf("\n\ngPath\n");
	outps(maxps);
	printf("Flow: %d\n", maxps.pressure);

	if (maxps.total == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %d\n", answer);
}

int main5()
{

	//solvept1("ex.txt", 1651);
	//solvept11("ex.txt", 1651);
	//solvept1("input.txt", 1906);  
	solvept2("ex.txt", 1707);
	solvept2("input.txt", 2548);
	return 0;
}


// CPP code for printing shortest path between
// two vertices of unweighted graph
#include <bits/stdc++.h>
using namespace std;

// utility function to form edge between two vertices
// source and dest
void add_edge(vector<int> adj[], int src, int dest)
{
	adj[src].push_back(dest);
	adj[dest].push_back(src);
}

// a modified version of BFS that stores predecessor
// of each vertex in array p
// and its distance from source in array d
bool BFS(vector<int> adj[], int src, int dest, int v,
		int pred[], int dist[])
{
	// a queue to maintain queue of vertices whose
	// adjacency list is to be scanned as per normal
	// DFS algorithm
	list<int> queue;

	// boolean array visited[] which stores the
	// information whether ith vertex is reached
	// at least once in the Breadth first search
	bool visited[v];

	// initially all vertices are unvisited
	// so v[i] for all i is false
	// and as no path is yet constructed
	// dist[i] for all i set to infinity
	for (int i = 0; i < v; i++) {
		visited[i] = false;
		dist[i] = INT_MAX;
		pred[i] = -1;
	}

	// now source is first to be visited and
	// distance from source to itself should be 0
	visited[src] = true;
	dist[src] = 0;
	queue.push_back(src);

	// standard BFS algorithm
	while (!queue.empty()) {
		int u = queue.front();
		queue.pop_front();
		for (int i = 0; i < (int) adj[u].size(); i++) {
			if (visited[adj[u][i]] == false) {
				visited[adj[u][i]] = true;
				dist[adj[u][i]] = dist[u] + 1;
				pred[adj[u][i]] = u;
				queue.push_back(adj[u][i]);

				// We stop BFS when we find
				// destination.
				if (adj[u][i] == dest)
					return true;
			}
		}
	}

	return false;
}

// utility function to print the shortest distance
// between source vertex and destination vertex
void printShortestDistance(vector<int> adj[], int s,
						int dest, int v)
{
	// predecessor[i] array stores predecessor of
	// i and distance array stores distance of i
	// from s
	int pred[v], dist[v];

	if (BFS(adj, s, dest, v, pred, dist) == false) {
		cout << "Given source and destination"
			<< " are not connected";
		return;
	}

	// vector path stores the shortest path
	vector<int> path;
	int crawl = dest;
	path.push_back(crawl);
	while (pred[crawl] != -1) {
		path.push_back(pred[crawl]);
		crawl = pred[crawl];
	}

	// distance from source is in distance array
	cout << "Shortest path length is : "
		<< dist[dest];

	// printing path from source to destination
	cout << "\nPath is::\n";
	for (int i = path.size() - 1; i >= 0; i--)
		cout << path[i] << " ";
}

// Driver program to test above functions
int main()
{
	// no. of vertices
	int v = 8;

	// array of vectors is used to store the graph
	// in the form of an adjacency list
	vector<int> adj[v];

	// Creating graph given in the above diagram.
	// add_edge function takes adjacency list, source
	// and destination vertex as argument and forms
	// an edge between them.
	add_edge(adj, 0, 1);
	add_edge(adj, 0, 3);
	add_edge(adj, 1, 2);
	add_edge(adj, 3, 4);
	add_edge(adj, 3, 7);
	add_edge(adj, 4, 5);
	add_edge(adj, 4, 6);
	add_edge(adj, 4, 7);
	add_edge(adj, 5, 6);
	add_edge(adj, 6, 7);
	int source = 0, dest = 7;
	printShortestDistance(adj, source, dest, v);
	return 0;
}

