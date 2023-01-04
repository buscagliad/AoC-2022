
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

typedef struct path_s {
	int 		minute;
	string 		openvalves;
	string 		path;
	string 		path1;
	string 		path2;
	int			atvalve1;
	int			prevvalve1;
	int			atvalve2;
	int			prevvalve2;
	int			pressure;
	int			total;
	int			valves_left_to_open;
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

int MaxPressure[MAX_MINUTE+1];

path_state gPath[MAX_MINUTE+1];
path_state gEleph[MAX_MINUTE+1];

vector<valve> volcano;

vector<int> flows;  // if flow < 0 -> skip turning on this valve
					// but flow ,- -flow, so next time it will get turned on

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

void turn_on_valve(vector<valve> &v, path_state &ps, int ix, bool path2 = false)
{
	//if (ps.minute >= MinutesToRun) return;
	if (DEBUG) printf("turn_on_valve:  flows[ix]=%ld   open: %s\n", v[ix].flow, ps.openvalves.c_str());
	if ( (v[ix].flow == 0) || ps.openvalves.find(v[ix].nm) != string::npos) return;

	ps.openvalves += " " + v[ix].nm;
	if (path2)
	{
		ps.path2 += " " + to_string(ps.minute) + "=" + v[ix].nm + "*";
		ps.prevvalve2 = ix;
	}
	else 
	{
		ps.path1 += " " + to_string(ps.minute) + "=" + v[ix].nm + "*";
		ps.prevvalve1 = ix;
	}
	ps.valves_left_to_open--;
	ps.pressure += v[ix].flow;	
	if (DEBUG) printf("Path: %s  Turning on valve At %s flow: %ld  minute: %d\n", 
		ps.path.c_str(), v[ix].nm.c_str(), v[ix].flow, ps.minute);

}

void move_to_tunnel(vector<valve> &v, path_state ps, int ix)
{
	if (ps.minute >= MinutesToRun) return;
	for (int on_valve = 0; on_valve < 2; on_valve++)
	{
		// move to a tunnel
		for (int i = 0; i < (int)v[ix].tunnels.size(); i++)
		{
			path_state p = ps;
			
			//if (v[ix].tunix[i] == ps.ff_tunnel)
			//{
				//printf("Not returning to tunnel %s\n", volcano[ix].tunnels[i].c_str());
			//	continue;
			//}
			p.minute++;
			p.path += " " + to_string(p.minute) + "-" + v[v[ix].tunix[i]].nm;
			p.total += p.pressure;
			if (MaxPressure[p.minute] > p.total) continue;

			if (DEBUG) printf("%d  Tunnelling from %s to %s\n", p.minute, v[ix].nm.c_str(), v[v[ix].tunix[i]].nm.c_str());
			// on one path we first look to turn on the valve here -
			// on another path we do not
			if (on_valve && p.minute < MinutesToRun && p.valves_left_to_open) turn_on_valve(v, p, v[ix].tunix[i]);
			if (p.minute < MinutesToRun) 
			    move_to_tunnel(v, p, v[ix].tunix[i]);

			if (MaxPressure[p.minute] < p.total) 
			{
				MaxPressure[p.minute] = p.total;
				gPath[p.minute] = p;
			}
		}
	}
}

path_state maxps;

void		next_move(vector<valve> &v, path_state ps, int find_quick_path = 30)
{
//	ps.minute++;
	if (ps.valves_left_to_open && ps.minute > find_quick_path) return;
	if (ps.valves_left_to_open == 0)
	{
		ps.total += ps.pressure * (MinutesToRun - ps.minute);
		ps.minute = MinutesToRun;
	}
	if (ps.minute == MinutesToRun && maxps.total < ps.total) 
	{
		maxps = ps;
		printf("Minute: %d   Total: %d\n", maxps.minute, maxps.total);
		printf("     %s\n", maxps.path1.c_str());
		printf("     %s\n", maxps.path2.c_str());
		fflush(stdout);
	}
	ps.total += ps.pressure;
	if (ps.minute >= MinutesToRun) return;



	int ix1 = ps.atvalve1;
	int ix2 = ps.atvalve2;
	bool valve1;
	bool valve2;
	//printf("%s\n%s\n", ps.path1.c_str(), ps.path2.c_str());

	// is there a valve to open?
	if ( (v[ix1].flow > 0) && ps.openvalves.find(v[ix1].nm) == string::npos) valve1 = true;
	path_state pnn = ps;
	path_state pyn = ps;
	// move me
	if ( (v[ix2].flow > 0) && ps.openvalves.find(v[ix2].nm) == string::npos) valve2 = true;
	// move elephant
	path_state pny = ps;
	path_state pyy = ps;
	if (valve1 && valve2)
	{
		pyy.minute++;
		turn_on_valve(v, pyy, ix1, false);
		turn_on_valve(v, pyy, ix2, true);
		next_move(v, pyy);
	}		
	//
	// turn on valve 1 - move path2 to each possible square
	if (valve1 && !valve2)
	{
		pyn.minute++;
		turn_on_valve(v, pyn, ix1, false);
		for (int i = 0; i < (int)v[ix2].tunnels.size(); i++)
		{
			path_state p = pyn;
			
			p.atvalve2 = v[ix2].tunix[i];
			p.path2 += " " + to_string(p.minute) + "-" + v[v[ix2].tunix[i]].nm;
			p.prevvalve2 = ix2;
			next_move(v, p);
		}		
	}
	//
	// turn on valve 2 - move path1 to each possible square
	if (!valve1 && valve2)
	{
		pny.minute++;
		turn_on_valve(v, pny, ix2, true);
		
		for (int i = 0; i < (int)v[ix1].tunnels.size(); i++)
		{
			path_state p = pny;
			if (v[ix1].tunix[i] == p.prevvalve1) continue;
			p.atvalve1 = v[ix1].tunix[i];
			p.path1 += " " + to_string(p.minute) + "-" + v[v[ix1].tunix[i]].nm;
			//p.total += p.pressure;
			p.prevvalve1 = ix1;
			next_move(v, p);
		}
	
	}
	
    if (valve1 || valve2) return;
	pyy.minute++;
	for (int i = 0; i < (int)v[ix1].tunnels.size(); i++)
	{
		if (v[ix1].tunix[i] == pyy.prevvalve1) continue;
		for (int j = 0; j < (int)v[ix2].tunnels.size(); j++)
		{
			if (v[ix2].tunix[j] == pyy.prevvalve2) continue;
			path_state p = pyy;
			
			p.atvalve1 = v[ix1].tunix[i];
			p.path1 += " " + to_string(p.minute) + "-" + v[v[ix1].tunix[i]].nm;
			p.atvalve2 = v[ix2].tunix[j];
			p.path2 += " " + to_string(p.minute) + "-" + v[v[ix2].tunix[j]].nm;
			p.prevvalve1 = ix1;
			p.prevvalve2 = ix2;
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
	flows.clear();
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
	vector<valve> v = volcano;
	//output(nv);
	path_state ps;
	ps.minute = 1;
	ps.pressure = 0;
	ps.total = 0;
	ps.path = volcano[0].nm;
	ps.valves_left_to_open = numGTzero(volcano);
	int aa = nm_index(volcano, "AA");

	outps(ps);
    MinutesToRun = 26;
    maxps = ps;
    for (size_t i = 0; i < MAX_MINUTE; i++)
    {
		gPath[i] = ps;
		MaxPressure[i] = 0;
	}
	for (size_t k = 0; k < volcano[aa].tunix.size() - 1; k++)
	{
		for (size_t m = k+1; m < volcano[aa].tunix.size(); m++)
		{
			path_state nps = ps;
			nps.path1 += " " + to_string(nps.minute) + "-" + v[v[aa].tunix[k]].nm;
			nps.prevvalve1 = aa;
			nps.atvalve1 = v[aa].tunix[k];
			nps.path2 += " " + to_string(nps.minute) + "-" + v[v[aa].tunix[m]].nm;
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

	solvept2("input.txt", 1707);
	return 0;
	solvept2("ex.txt", 1707);
	solvept1("ex.txt", 1651);
	solvept1("input.txt", 1906);  // 1833 is too low!!  2544 is too high!!
	return 0;
}

