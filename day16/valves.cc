
#include <cstring>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cctype>
#include <string>

using namespace std;

typedef enum {
	vs_move,	// if in this state, will move (state -> vs_flow)
	vs_flow 	// if in this state, can turn on flow if flow > 0;
} valve_state;

typedef struct path_s {
	int 		minute;
	string 		openvalves;
	string 		path;
	int			from_tunnel;
	int			ff_tunnel;
	int			pressure;
	int			total;
	bool		has_moved;	// set to true when a new valve is moved to
} path_state;


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

int MaxPressure = 0;

path_state gPath;

vector<valve> volcano;

vector<int> flows;  // if flow < 0 -> skip turning on this valve
					// but flow ,- -flow, so next time it will get turned on
					

void outps(path_state &p)
{
	printf("\nMinute: %d\n", p.minute);
	printf("Open Values: %s\n", p.openvalves.c_str());
	printf("Path: %s\n", p.path.c_str());
	printf("Minute pressure: %d\n", p.pressure);
	printf("Last two tunnels: %s %s\n", volcano[p.ff_tunnel].nm.c_str(), volcano[p.from_tunnel].nm.c_str());
	printf("Total pressure: %d\n", p.total);
}

valve get(string nm)
{
	for (size_t i = 0; i < volcano.size(); i++)
	    if (volcano[i].nm == nm) return volcano[i];
	printf("%s not found\n", nm.c_str());
	return volcano[0];
}

int sumflow(int minute, vector<int> flows)
{
	int sum = 0;
	//printf("SF: %d:: ", minute);
	for (size_t i = 0; i < flows.size(); i++)
	{
		if (flows[i] < 0) sum -= flows[i];
		//printf("%d ", flows[i]);
	}
	//printf("\n");
	return sum;
}

bool need_to_move(vector<int> flows)
{
	for (size_t i = 0; i < flows.size(); i++)
	{
		if (flows[i] > 0) return true;;
	}
	return false;
}

typedef struct paths_s {
	int min[30];
} paths;


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
	
void procxx(vector<valve> &v, path_state ps, int ix, vector<int> flows)
{
	//ps.minute++;
	//ps.path += " " + v[ix].nm;
	//ps.total += ps.pressure;
	if (gPath.total < ps.total) gPath = ps;
	if (ps.minute >= 4) {
		printf("Last ps:\n");
		outps(ps);
		printf("\n\ngPath:\n");
		outps(gPath);
		exit(1);
	}
	
	if (ps.minute >= 30) {
		//outps(ps);
		return;
	}
	if (ps.has_moved && flows[ix]>0 && ps.openvalves.find(v[ix].nm) == string::npos)
	{
		ps.minute++;
		ps.total += ps.pressure;
		ps.pressure += flows[ix];
		ps.openvalves += " " + v[ix].nm;
		ps.path += " " + to_string(ps.minute) + "=" + v[ix].nm + "*";
		ps.has_moved = false;
		//ps.flows[ix] = 0;
		printf("Turning on valve At %s flow: %ld  minute: %d\n", v[ix].nm.c_str(), v[ix].flow, ps.minute);
		procxx(v, ps, ix, flows);
		//if (pressure > maxp) { maxp = pressure; }
		if (gPath.total < ps.total) gPath = ps;
	}
	// move to a tunnel
	for (int i = 0; i < (int)v[ix].tunnels.size(); i++)
	{
		int maxp = 0;
		path_state p = ps;
		
		if (v[ix].tunix[i] == ps.ff_tunnel)
		{
			//printf("Not returning to tunnel %s\n", volcano[ix].tunnels[i].c_str());
			continue;
		}
		p.ff_tunnel = p.from_tunnel;
		p.from_tunnel = ix; 
		p.minute++;
		p.path += " " + to_string(p.minute) + "-" + v[p.from_tunnel].nm;
		p.total += p.pressure;
		p.has_moved = true;
		printf("%d  Tunnelling from %s to %s\n", p.minute, v[ix].nm.c_str(), v[ix].tunnels[i].c_str());
		procxx(v, p, v[ix].tunix[i], flows);
		int press = pressure(p);
		if (press > maxp) {maxp = press; ps = p; if (gPath.total < ps.total) gPath = ps; }

	}

	if (ps.minute >= 30){
		//outps(ps);
		return;
	}
	procxx(v, ps, ix, flows);
	
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
		flows.push_back(v.flow);
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
		printf("Valve %s has flow rate=%ld; tunnels lead to valve", 
			v[i].nm.c_str(), v[i].flow);
		if (v[i].tunnels.size() > 1) { printf("s"); }
		printf(" %s", v[i].tunnels[0].c_str());
		for (size_t t = 1; t < v[i].tunnels.size(); t++)
		{
			printf(", %s", v[i].tunnels[t].c_str());
		}
		printf("\n");
	}
}

void solvept1(const char *fn, int answer)
{
	init(fn);
	output(volcano);
	vector<valve> nv = volcano;
	output(nv);
	path_state ps;
	ps.minute = 0;
	ps.pressure = 0;
	ps.has_moved = false;
	ps.total = 0;	
	outps(ps);
	MaxPressure = 0;
	procxx(nv, ps, 0, flows);
	printf("\n\ngPath\n");
	outps(gPath);
	printf("Flow: %d\n", MaxPressure);
	if (MaxPressure == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %d\n", answer);
}

void solvept1_flow(const char *fn, int answer)
{
	init(fn);
	output(volcano);
	vector<valve> nv = volcano;
	output(nv);
	path_state ps;
	ps.minute = 0;
	ps.pressure = 0;
	ps.total = 0;	
	outps(ps);
	MaxPressure = 0;
	procxx(nv, ps, 0, flows);
	outps(gPath);
	printf("Flow: %d\n", MaxPressure);
	if (MaxPressure == answer)
		printf("Answer is correct!!!\n");
	else
		printf("Answer is not correct - should be %d\n", answer);
}

int main()
{
	solvept1("ex.txt", 1651);

	return 0;
	solvept1("input.txt", 1);
}
