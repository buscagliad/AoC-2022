#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <cstring>

using namespace std;

typedef enum {
	ADD, SUB, MULT, DIV, EQ } op_t;

char char_op(op_t e)
{
	switch(e)
	{
		case EQ: return '='; 
		case ADD: return '+'; 
		case SUB: return '-';
		case MULT: return '*';
		case DIV: return '/';
		default:
			printf("ERROR unknown op: %d\n", e);
	}
	return 'E';
}


typedef struct monkey {
	string	name;
	int64_t	number;
	bool	use_number;
	op_t 	op;
	string	lname;
	string  rname;
	struct monkey *lmonk;
	struct monkey *rmonk;
} monkey_t;

vector<monkey *> monkey_list;


int64_t getnum(monkey *m, bool debug = false)
{
	if (m->use_number)
	{
		if (debug) printf("%s = %ld\n", m->name.c_str(), m->number);
		return m->number;
	}
	int64_t 	setval;
	int64_t   lv = getnum(m->lmonk);
	int64_t   rv = getnum(m->rmonk);
	if (m->op == ADD)
	{
	    setval = lv + rv;
	}
	else if (m->op == SUB)
	{
	    setval = lv - rv;
	}
	else if(m->op == MULT)
	{
	    setval = lv * rv;
	}
	else if(m->op == DIV)
	{
	    setval = lv / rv;
	}
	else if(m->op == EQ)
	{
	    //if (lv == rv) 
		//	printf("From %s  %ld and %ld are EQUAL!!\n", m->name.c_str(), lv, rv);
		//else
		//	printf("From %s  %ld and %ld are NOT EQUAL.\n", m->name.c_str(), lv, rv);
		return lv - rv;
	}
	if (debug) printf("%s = %s (%ld) %c %s (%ld) = %ld\n", m->name.c_str(), m->lmonk->name.c_str(), lv,
			char_op(m->op), m->rmonk->name.c_str(), rv, setval);
	m->use_number = true;
	m->number = setval;
	return setval;
}

void split(char *s, monkey *m, bool debug = false)
{
	m->name = strtok(s, ": \n");
	char *rs = strtok(NULL, ": \n");
	if (isdigit(*rs))
	{
		if (debug) printf("Found number::  %s : %s %lld\n", m->name.c_str(), rs, atoll(rs));
		m->number = atoll(rs);
		m->use_number = true;
	}
	else
	{
		m->use_number = false;
		m->lname = rs;
		rs = strtok(NULL, ": \n");
		switch(*rs)
		{
			case '+': m->op = ADD; break;
			case '-': m->op = SUB; break;
			case '*': m->op = MULT; break;
			case '/': m->op = DIV; break;
			default:
				printf("ERROR unknown op: %c\n", *rs);
		}
		m->rname = strtok(NULL, ": \n");
	}
	m->lmonk = NULL;
	m->rmonk = NULL;
}

monkey *find(vector<monkey *> &l, string nm)
{
	for (size_t i = 0; i < l.size(); i++)
	{
		if (l[i]->name == nm) return l[i];
	}
	return NULL;
}

void out(monkey *m)
{
	if (m->use_number)
	{
		printf("%s: %ld\n", m->name.c_str(), m->number);
	}
	else
	{
		printf("%s: %s %c %s\n", m->name.c_str(), 
			m->lmonk->name.c_str(),
			char_op(m->op),
			m->rmonk->name.c_str());
	}
}

void out(vector<monkey *> &ml)
{
	for (size_t i = 0; i < ml.size(); i++)
	{
		out(ml[i]);
	}	
}

void init(const char *fn, vector<monkey *> &mlist)
{
	FILE *f = fopen(fn, "r");
	char s[100];
	mlist.clear();
	while (!feof(f))
	{
		fgets(s, 100, f);
		if (feof(f)) break;
		monkey *nm = new monkey;
		split(s, nm);
		mlist.push_back(nm);
	}
	
	//
	// set pointers (if needed)
	//
	for (size_t i = 0; i < mlist.size(); i++)
	{
		if (mlist[i]->use_number) continue;
		mlist[i]->lmonk = find(mlist, mlist[i]->lname);
		mlist[i]->rmonk = find(mlist, mlist[i]->rname);
	}

	fclose(f);
}

void solvept1(const char *fn, int64_t answer, bool debug = false)
{
	init(fn, monkey_list);
	if (debug) out(monkey_list);
	int64_t root = getnum(find(monkey_list, "root"));
	printf("File: %s -- Part 1 Monkey answer is %ld\n", fn, root);
	if (root == answer)
	    printf("Answer is correct!!!\n");
	else
	    printf("Answer is INCORRECT - should be %ld\n", answer);
}

void outToNum(monkey *m)
{
	out(m);
	while (! m->use_number)
	{
		outToNum(m->lmonk);
		outToNum(m->rmonk);
	}
}

void outRoot(vector<monkey *> &ml)
{
	monkey_t *root = find(ml, "root");
	outToNum(root);
}

void deep_copy(vector<monkey *> &to, vector<monkey *> &from)
{
	to.clear();
	for (size_t i = 0; i < from.size(); i++)
	{
		monkey *n = new monkey;
		n->name = from[i]->name;
		n->number = from[i]->number;
		n->use_number = from[i]->use_number;
		n->op = from[i]->op;
		n->lname = from[i]->lname;
		n->rname = from[i]->rname;
		to.push_back(n);
	}
	for (size_t i = 0; i < to.size(); i++)
	{
		if (to[i]->use_number) continue;
		to[i]->lmonk = find(to, to[i]->lname);
		if (!to[i]->lmonk) printf("Unable to find %s during deep copy\n", to[i]->lname.c_str());
		to[i]->rmonk = find(to, to[i]->rname);
		if (!to[i]->rmonk) printf("Unable to find %s during deep copy\n", to[i]->rname.c_str());
	}
}

bool	same_sign(int64_t a, int64_t b)
{
	if (a > 0 && b < 0) return false;
	if (b > 0 && a < 0) return false;
	return true;
}

int64_t getRoot(vector<monkey *> &ml_orig, int64_t hm)
{
	vector<monkey *> ml;
	
	deep_copy(ml, ml_orig);
	monkey_t *root = find(ml, "root");
	monkey_t *human = find(ml, "humn");
	root->op = EQ;
	human->number = hm;
	return getnum(root);
}
	
//
// doing binary search
void solvept2(const char *fn, int64_t answer, bool debug = false)
{
	vector<monkey *> monkey_copy;
	init(fn, monkey_copy);
	int64_t hmn = 2;
	int64_t	lroot = getRoot(monkey_copy, hmn);
	hmn *= 2;
	int64_t rroot = getRoot(monkey_copy, hmn);
	while (same_sign(lroot, rroot))
	{
		hmn*=2;
		lroot = rroot;
		rroot = getRoot(monkey_copy, hmn);
	}
	printf("H: %ld, R: %ld\n", hmn/2, lroot);
	printf("H: %ld, R: %ld\n", hmn, rroot);
	//
	// binary search
	int64_t  l = hmn/2;
	int64_t  r = hmn;
	int64_t  nroot;
	while (lroot != 0)
	{
		hmn = (l + r) / 2;
		nroot = getRoot(monkey_copy, hmn);
		if (same_sign(lroot, nroot))
		{
			lroot = nroot;
			l = hmn;
		}
		else
		{
			rroot = nroot;
			r = hmn;
		}
		printf("H: %ld, R: %ld\n", hmn, nroot);
	}
	
	if (getRoot(monkey_copy, hmn-1) == 0)
	{
		l = hmn-1;
	}
	if (lroot == 0)
	{
		printf("File: %s -- Part 2 Human answer is %ld\n", fn, l);
		if (l == answer)
			printf("Answer is correct!!!\n");
		else
			printf("Answer is INCORRECT - should be %ld\n", answer);
		return;
	}
		

}


int main()
{
	solvept1("ex.txt", 152);
	solvept1("input.txt", 286698846151845);
	solvept2("ex.txt", 301);
	solvept2("input.txt", 301);
}

#ifdef SAVETHIS
	monkey_t *root = find(monkey_list, "root");
	monkey_t *hm = find(monkey_list, "humn");
	root->op = EQ;
	hm->number = 2;
	deep_copy(monkey_list, monkey_copy);
	
	root = find(monkey_list, "root");
	hm = find(monkey_list, "humn");
	root->op = EQ;
	hm->number = end;
	int64_t lp2 = 0;
	int64_t hp2 = 0;

	for (int64_t h = start; h <  end; h*=2)
	{
		//monkey_list = monkey_copy;
		deep_copy(monkey_list, monkey_copy);
		//init(fn, monkey_list);
		//out(monkey_list);
		monkey_t *root = find(monkey_list, "root");
		monkey_t *hm = find(monkey_list, "humn");
		root->op = EQ;
		hm->number = h;
		hp2 = h;
		int64_t root_answer = getnum(root);
		printf("File: %s -- Part 2 Human answer is %ld  root: %ld\n", fn, h, root_answer);
		if (sgn(root_answer) != sgn(last_root_answer))
		{
			break;
		}
		lp2 = h;
	}
	while (root_answer != 0)
	{
		h = (lp2 + hp2)/2;
		deep_copy(monkey_list, monkey_copy);
		//init(fn, monkey_list);
		//out(monkey_list);
		monkey_t *root = find(monkey_list, "root");
		monkey_t *hm = find(monkey_list, "humn");
		root->op = EQ;
		hm->number = h;
		hp2 = h;
		int64_t root_answer = getnum(root);
		printf("File: %s -- Part 2 Human answer is %ld  root: %ld\n", fn, h, root_answer);
		
		if (root_answer == 0)
		{
			printf("File: %s -- Part 2 Human answer is %ld\n", fn, h);
			if (h == answer)
				printf("Answer is correct!!!\n");
			else
				printf("Answer is INCORRECT - should be %ld\n", answer);
			return;
		}
	}
	
#endif
