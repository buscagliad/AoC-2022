
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

int MaxPressure[MAX_MINUTE+1];

vector<valve> volcano;

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

void turn_on_valve(vector<valve> &v, path_state &ps, int ix)
{
	//if (ps.minute >= MinutesToRun) return;
	if (DEBUG) printf("turn_on_valve:  flows[ix]=%ld   open: %s\n", v[ix].flow, ps.openvalves.c_str());
	if ( (v[ix].flow == 0) || ps.openvalves.find(v[ix].nm) != string::npos) return;

	ps.openvalves += " " + v[ix].nm;

	ps.valves_left_to_open--;
	//ps.pressure += v[ix].flow;	
	ps.new_pressure += v[ix].flow;	
	zvalue(ps, v[ix].flow);
	
	if (DEBUG) printf("Path: %s  Turning on valve At %s flow: %ld  minute: %d\n", 
		ps.path.c_str(), v[ix].nm.c_str(), v[ix].flow, ps.minute);

}

path_state maxps;
int earlyminute = 40;

void		next_move(vector<valve> &v, path_state ps, int find_quick_path = 30)
{
	ps.minute++;
//	if (max_remaining(ps) < maxps.total) return;
	ps.total += ps.pressure;
	ps.pressure += ps.new_pressure;
	ps.new_pressure = 0;
	if (ps.valves_left_to_open == 0)
	{
		if (ps.minute < earlyminute)
		{
			earlyminute = ps.minute;
			printf("Early minute: %d   pressure: %d   total: %d  remain: %d\n", earlyminute, ps.pressure, ps.total, (MinutesToRun-ps.minute)*ps.pressure);
		}
		ps.total += ps.pressure * (MinutesToRun - ps.minute - 1);
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

	// is there a valve to open?
	if ( (v[ix1].flow > 0) && ps.openvalves.find(v[ix1].nm) == string::npos) valve1 = true;
	path_state pnn = ps;
	path_state pyn = ps;
	// move me
	if ( ix1 != ix2 && ix2 >= 0 && (v[ix2].flow > 0) && ps.openvalves.find(v[ix2].nm) == string::npos) valve2 = true;
	// move elephant
	path_state pny = ps;
	path_state pyy = ps;
	//
	// turn on both valves
	if (valve1 && valve2)
	{
		//pyy.minute++;
		turn_on_valve(v, pyy, ix1);
		turn_on_valve(v, pyy, ix2); 
		next_move(v, pyy);
	}		
	//
	// turn on valve 1 - move path2 to each possible square
	if (valve1 && !valve2)
	{
		//pyn.minute++;
		turn_on_valve(v, pyn, ix1); 
		if (ix2 >= 0)
		{
			for (int i = 0; i < (int)v[ix2].tunnels.size(); i++)
			{
				path_state p = pyn;
				
				p.atvalve2 = v[ix2].tunix[i];
				p.prevvalve2 = ix2;
				next_move(v, p);
			}		
		}
		else
		{
			next_move(v, pyn);
		}
	}
	//
	// turn on valve 2 - move path1 to each possible square
	if (!valve1 && valve2)
	{
		//pny.minute++;
		turn_on_valve(v, pny, ix2);
		
		for (int i = 0; i < (int)v[ix1].tunnels.size(); i++)
		{
			path_state p = pny;
			if (v[ix1].tunix[i] == p.prevvalve1) continue;
			p.atvalve1 = v[ix1].tunix[i];
			p.prevvalve1 = ix1;
			next_move(v, p);
		}
	
	}
	
	//
	// if both valve1 and valve2 have not been opened
    if (valve1 || valve2) return;
	//pnn.minute++;
	for (int i = 0; i < (int)v[ix1].tunnels.size(); i++)
	{
		if (v[ix1].tunix[i] == pnn.prevvalve1) continue;
		pnn.atvalve1 = v[ix1].tunix[i];
		pnn.prevvalve1 = ix1;
		if (ix2 >= 0)
		{

			for (int j = 0; j < (int)v[ix2].tunnels.size(); j++)
			{
				if (v[ix2].tunix[j] == pnn.prevvalve2) continue;
				path_state p = pnn;
				
				p.atvalve2 = v[ix2].tunix[j];
				p.prevvalve2 = ix2;
				next_move(v, p);
			}
		}
		else
		{
			next_move(v, pnn);
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
    for (size_t i = 0; i <= MAX_MINUTE; i++)
    {
		MaxPressure[i] = 0;
	}
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
	vector<valve> v = volcano;
	//output(nv);
	path_state ps;
	ps.minute = 0;
	ps.pressure = 0;
	ps.new_pressure = 0;
	ps.total = 0;
	ps.path = volcano[0].nm;
	ps.valves_left_to_open = numGTzero(volcano);
	setflows(volcano, ps);
	int aa = nm_index(volcano, "AA");

	outps(ps);
    MinutesToRun = 26;
    maxps = ps;
    for (size_t i = 0; i <= MAX_MINUTE; i++)
    {
		MaxPressure[i] = 0;
	}
	for (size_t k = 0; k < volcano[aa].tunix.size() - 1; k++)
	{
		for (size_t m = k+1; m < volcano[aa].tunix.size(); m++)
		{
			path_state nps = ps;
			nps.prevvalve1 = aa;
			nps.atvalve1 = v[aa].tunix[k];
			nps.prevvalve2 = aa;
			nps.atvalve2 = v[aa].tunix[m];
			next_move(v, nps);
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

int main()
{

	//solvept1("ex.txt", 1651);
	//solvept11("ex.txt", 1651);
	//solvept1("input.txt", 1906);  
	solvept2("ex.txt", 1707);
	//solvept2("input.txt", 2548);
	return 0;
}

