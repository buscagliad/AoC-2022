
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
		pnn.atvalve1 = v[ix1].tunix[i];
		for (int j = 0; j < (int)v[ix2].tunnels.size(); j++)
		{
			path_state p = pnn;
			
			p.atvalve2 = v[ix2].tunix[j];
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
		next_move(v, pyy);
	}		
	//
	// turn on valve 1 - move path2 to each possible square
	if (valve1 && !valve2)
	{
		//UPDATE(pyn);
		turn_on_valve(v, pyn, ix1); 
		for (int i = 0; i < (int)v[ix2].tunnels.size(); i++)
		{
			path_state p = pyn;
				
			p.atvalve2 = v[ix2].tunix[i];
			next_move(v, p);
		}
	}
	//
	// turn on valve 2 - move path1 to each possible square
	if (!valve1 && valve2)
	{
		//UPDATE(pny);
		turn_on_valve(v, pny, ix2);
		
		for (int i = 0; i < (int)v[ix1].tunnels.size(); i++)
		{
			path_state p = pny;
			p.atvalve1 = v[ix1].tunix[i];
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
		nps.atvalve1 = v[aa].tunix[m];
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
			nps.atvalve1 = volcano[aa].tunix[k];
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



bool explored(path_state &p)
{
	return bmap_time[p.t].check[p.x][p.y];
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
int bfsearch(vector<valve> &g, int num_bots, int start_index, int max_time)
{
	queue<path_state> q;		// (2) q will be our q for searching
// (3) create 1 or two roots (num_bots)
	path_state ps;
	ps.minute = 1;
	ps.pressure = 0;
	ps.total = 0;
	ps.path = volcano[0].nm;
	ps.atvalve1 = start_index;
	if (num_bots > 1)
		ps.atvalve2 = start_index;
	else
		ps.atvalve2 = -1;
	ps.valves_left_to_open = numGTzero(volcano);

// (4) add root to q
	
	q.push(ps); 
	while (!q.empty())	// (5) while Q is not empty do
	{
		path_state	v = q.front();		//   (6)  v := Q.dequeue()
		q.pop();
		// Check if pos_t has been 'explored', if so, continue
		if (c(v)) {
			continue;
		}
		bmap_time[v.t].check[v.x][v.y] = true;
		if (v == goal) return  v;	//   (7) if v is the goal then return v (8)
		// NEED TO LIMIT TIME - say bs.xpts * bs.ypts maximum
		if (v.t > max_time) continue;
//
//   9          for all edges from v to w in G.adjacentEdges(v) do
//
		// it will add to the Q and mark explored
//  10              if w is not labeled as explored then
//  11                  label w as explored
//  12                  w.parent := v	// NOT NEEDED
//  13                  Q.enqueue(w)
		update(bs, v.t + 1);
		pos_t pyp1 = v.incy();
		pos_t pym1 = v.decy();
		pos_t pxp1 = v.incx();
		pos_t pxm1 = v.decx();
		pos_t pstay = v.stay();

		if (procede(bs, pyp1, "DOWN"))  q.push(pyp1);  	// move DOWN
		if (procede(bs, pxp1, "RIGHT")) q.push(pxp1); 	// move RIGHT
		if (procede(bs, pym1, "UP"))    q.push(pym1);	// move UP
		if (procede(bs, pxm1, "LEFT"))  q.push(pxm1);	// move LEFT
		if (procede(bs, pstay, "STAY")) q.push(pstay); 	// STAY
	}
	return {-1,-1,-1};
}



void solvept1(const char *fn, int answer)
{
	init(fn);
	output(volcano);
	//vector<valve> nv = volcano;
	//output(nv);
	path_state ps;
	ps.minute = 0;
	ps.pressure = 0;
	ps.total = 0;
	ps.path = volcano[0].nm;
	outps(ps);
	for (int i = 0; i <= MAX_MINUTE; i++) 
	{
		gPath[i] = ps;
		MaxPressure[i] = 0;
	}
	move_to_tunnel(volcano, ps, nm_index(volcano, "AA"));
	printf("\n\ngPath\n");
	outps(gPath[MAX_MINUTE]);
	printf("Flow: %d\n", MaxPressure[MAX_MINUTE]);
	if (MaxPressure[MAX_MINUTE] == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %d\n", answer);
}

void solvept2(const char *fn, int answer)
{
	init(fn);
	output(volcano);
	//vector<valve> nv = volcano;
	//output(nv);
	path_state ps;
	ps.minute = 1;
	ps.pressure = 0;
	ps.total = 0;
	ps.path = volcano[0].nm;
	ps.valves_left_to_open = numGTzero(volcano);
	int aa = nm_index(volcano, "AA");
	path_state el = ps;
	outps(ps);
    MinutesToRun = 26;
	for (size_t k = 0; k < volcano[aa].tunix.size(); k++)
	{
		for (int i = 0; i <= MAX_MINUTE; i++) 
		{
			gPath[i] = ps;
			MaxPressure[i] = 0;
		}
		path_state nps = ps;
		move_to_tunnel(volcano, nps, volcano[aa].tunix[k]);
		printf("\n\ngPath\n");
		outps(gPath[MinutesToRun]);
		printf("Flow: %d\n", MaxPressure[MinutesToRun]);
	}
	if (MaxPressure[MAX_MINUTE] == answer)
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
