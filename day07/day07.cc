#ifdef PROBLEM




#endif

#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <cstdlib>

using namespace std;

typedef struct filestruct {
	std::string name;
	int			size;
} filex;

char *second(char *n)
{
	//n[strlen(n)-1] = 0;
	while (*n != ' ') n++;
	return ++n;
}

void trim(char *s)
{
	s[strlen(s) - 1] = 0;
}

void	Fgets(char *s, int n, FILE *f)
{
	fgets(s, n, f);
	if (feof(f)) return;
	trim(s);
	//printf("---- %s\n", s);
	//fflush(stdout);
}

class dirx {
	public:
		dirx(string n);
		void file(string name, int size);
		dirx *mkdir(string name);
		dirx *up();
		dirx *top();
		dirx *cd(string name);
		int size();
		void display(int tab=0);
		string name() { return dname; };
		int sum_small_dirs(int max, int &val)
		{
			int s = size();
			if (s <= max) val += s;
			//printf("%s   size: %d\n", dname.c_str(), s);
			
			for (size_t i = 0; i < dirs.size(); i++)
				dirs[i]->sum_small_dirs(max, val);
			return val;
		}
		int getdirlist(vector<filex> &fl)
		{
			filex f;
			f.name = dname;
			f.size = size();
			fl.push_back(f);
			for (size_t i = 0; i < dirs.size(); i++)
				dirs[i]->getdirlist(fl);
			return fl.size();
		}
			
	private:
		std::string dname;
		dirx *parent;
		vector<dirx *> dirs;
		vector<filex> files;
};

void dirx::display(int tab)
{
	for (int i = 0; i < tab; i++) printf(" ");
	printf("DIR:: %s  - size: %d\n", this->dname.c_str(), this->size());
	for (size_t i = 0; i < files.size(); i++)
	{
		for (int i = 0; i < tab; i++) printf(" ");
	    printf("    %8d  %s\n", files[i].size, files[i].name.c_str());
	}
	for (size_t i = 0; i < dirs.size(); i++)
		dirs[i]->display(tab+4);
}

dirx *slash = NULL;

dirx::dirx(std::string name)
{
	dname = name;
	parent = NULL;
}

void	dirx::file(std::string name, int s)
{
	filex f={name, s};
	files.push_back(f);
}

dirx *dirx::mkdir(std::string name)
{
	dirx *nd = new dirx(name);
	nd->parent = this;
	dirs.push_back(nd);
	return nd;
}

dirx    *dirx::up()
{
	if (parent == NULL) return slash;
	return parent;
}

dirx *dirx::top()
{
	return slash;
}

dirx *dirx::cd(std::string name)
{
	if (name[0] == '.' && name[1] == '.') 
	{
		if (parent == NULL) parent = slash;
		//printf("In %s Returning parent %16.16ux\n", 
		//	dname.c_str(), parent);
		return parent;
	}
	for (size_t i = 0; i < dirs.size(); i++)
	{
		if (name == dirs[i]->dname)
		{
			//printf("CDing into directory %s\n", 
			//		dirs[i]->dname.c_str());
			return dirs[i];
		}
	}
	printf("ERROR cd to dir %s\n", name.c_str());
	exit(1);
	return NULL;
}

int dirx::size()
{
	int s = 0;
	for (size_t i = 0; i < files.size(); i++)
	    s += files[i].size;
	//printf("Files %d\n", s);
	for (size_t i = 0; i < dirs.size(); i++)
	    s += dirs[i]->size();
	return s;
}

int func(const char *fn)
{
	FILE *f = fopen(fn, "r");
	char cmd[100];
	Fgets(cmd, 100, f);
	slash = new dirx("/");
	dirx *curdir = slash;
	bool doread = true;
	while (!feof(f))
	{
		if (doread) Fgets(cmd, 100, f);
		doread = true;
		char *c = cmd;
		if (strncmp(c, "$ ", 2) == 0) // cmd
		{
			c += 2;
			if (strncmp(c, "ls", 2) == 0)
			{
				Fgets(cmd, 100, f);
				while (!feof(f) && cmd[0] != '$')
				{
					if (strncmp(cmd, "dir", 3) == 0)
					{
						curdir->mkdir(cmd+4);
					}
					else
					{
						int sz = atoi(cmd);
						curdir->file(second(cmd), sz);
					}
					Fgets(cmd, 100, f);
				}
				doread = false;;
			}
			else if (strncmp(c, "cd", 2) == 0)
			{
				curdir = curdir->cd(c+3);
				//printf("%s cd to directory %s\n", c+3, curdir->name().c_str());
			}
			else
			{
				printf("WHAT THE F?\n");
			}
			
		}
	}
	return 1;
}

int main()
{
	func("input.txt");
	int val = 0;
	slash->sum_small_dirs(100000, val);
	printf("Sum of small directories is %d\n", val);
	printf("Total size of system is %d\n", slash->size());
	int freespace = 70000000 - slash->size();
	int mindelete = 30000000 - freespace;
	printf("Need to delete at least %d bytes\n", mindelete);
	vector<filex> dlist;
	slash->getdirlist(dlist);
	int m = 0;
	string s = "";
	for (size_t i = 0; i < dlist.size(); i++)
	{
		if (dlist[i].size > mindelete)
		{
			if (m == 0)
			{
				m = dlist[i].size;
				s = dlist[i].name;
			}
			else if (m > dlist[i].size)
			{
				m = dlist[i].size;
				s = dlist[i].name;
			}
		}
		//printf("%s   %d\n", dlist[i].name.c_str(), dlist[i].size);
	}
	printf("Dir: %s  Size: %d should be deleted\n", s.c_str(), m);
}
