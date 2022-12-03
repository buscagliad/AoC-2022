#ifdef PROBLEM 
000000000011111111
012345678902345678
PmmdzqPrVvPwwTWBwg
PmmdzqPrV
vPwwTWBwg

--- Day 3: Rucksack Reorganization ---

One Elf has the important job of loading all of the rucksacks with supplies for the jungle journey. Unfortunately, that Elf didnt quite follow the packing instructions, and so a few items now need to be rearranged.

Each rucksack has two large compartments. All items of a given type are meant to go into exactly one of the two compartments. The Elf that did the packing failed to follow this rule for exactly one item type per rucksack.

The Elves have made a list of all of the items currently in each rucksack (your puzzle input), but they need your help finding the errors. Every item type is identified by a single lowercase or uppercase letter (that is, a and A refer to different types of items).

The list of items for each rucksack is given as characters all on a single line. A given rucksack always has the same number of items in each of its two compartments, so the first half of the characters represent items in the first compartment, while the second half of the characters represent items in the second compartment.

For example, suppose you have the following list of contents from six rucksacks:

vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw

    The first rucksack contains the items vJrwpWtwJgWrhcsFMMfFFhFp, which means its first compartment contains the items vJrwpWtwJgWr, while the second compartment contains the items hcsFMMfFFhFp. The only item type that appears in both compartments is lowercase p.
    The second rucksacs compartments contain jqHRNqRjqzjGDLGL and rsFMfFZSrLrFZsSL. The only item type that appears in both compartments is uppercase L.
    The third rucksacks compartments contain PmmdzqPrV and vPwwTWBwg; the only common item type is uppercase P.
    The fourth rucksacks compartments only share item type v.
    The fifth rucksacks compartments only share item type t.
    The sixth rucksacks compartments only share item type s.

To help prioritize item rearrangement, every item type can be converted to a priority:

    Lowercase item types a through z have priorities 1 through 26.
    Uppercase item types A through Z have priorities 27 through 52.

In the above example, the priority of the item type that appears in both compartments of each rucksack is 16 (p), 38 (L), 42 (P), 22 (v), 20 (t), and 19 (s); the sum of these is 157.

Find the item type that appears in both compartments of each rucksack. What is the sum of the priorities of those item types?

--- Part Two ---

As you finish identifying the misplaced items, the Elves come to you with another issue.

For safety, the Elves are divided into groups of three. Every Elf carries a badge that identifies their group. For efficiency, within each group of three Elves, the badge is the only item type carried by all three Elves. That is, if a groups badge is item type B, then all three Elves will have item type B somewhere in their rucksack, and at most two of the Elves will be carrying any other item type.

The problem is that someone forgot to put this years updated authenticity sticker on the badges. All of the badges need to be pulled out of the rucksacks so the new authenticity stickers can be attached.

Additionally, nobody wrote down which item type corresponds to each groups badges. The only way to tell which item type is the right one is by finding the one item type that is common between all three Elves in each group.

Every set of three lines in your list corresponds to a single group, but each group can have a different badge item type. So, in the above example, the first groups rucksacks are the first three lines:

vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg

And the second groups rucksacks are the next three lines:

wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw

In the first group, the only item type that appears in all three rucksacks is lowercase r; this must be their badges. In the second group, their badge item type must be Z.

Priorities for these items must still be found to organize the sticker attachment efforts: here, they are 18 (r) for the first group and 52 (Z) for the second group. The sum of these is 70.

Find the item type that corresponds to the badges of each three-Elf group. What is the sum of the priorities of those item types?

#endif

#include <cstdio>
#include <cstring>

bool instr(char *l, char d)
{
	if (strchr(l, d)) return false;
	l[strlen(l)] = d;
	l[strlen(l)+1] = 0;
	return true;
}

int	compare(const char *left, const char *right)
{
	int prio = 0;
	int sum = 0;
	char emp[100];
	emp[0] = 0;
	for (size_t i = 0; i < strlen(left); i++)
	{
	    for (size_t j = 0; j < strlen(right); j++)
	    {
			if (left[i] == right[j])
			{
				if (!instr(emp, left[i])) continue;
				if (left[i] >= 'a')
				{
					prio = left[i] - 'a' + 1;
				}
				else
				{
					prio = left[i] - 'A' + 27;
				}			
				//printf("%ld,%ld  Left: %c   Right: %c    Prio: %d\n", i, j, left[i], right[j], prio);
				sum += prio;
			}
		}
	}
	return sum;
}


int part1(FILE *f)
{
	char inp[1000];
	fgets(inp, 1000, f);
	if (feof(f)) return -1;
	//printf("input: %s\n", inp);
	char *ls = strdup(inp);
	char *rs = strdup(inp);
	int index = strlen(ls)/2;
	
	ls[index] = 0;
	const char *l = ls;
	const char *r = rs + index;
	
	//printf("Left: %s  Right: %s\n", l, r);
	return compare(r, l);
}

int findcmn(const char *s1, const char *s2, const char *s3)
{
	for(int i = 0; i < (int)strlen(s1); i++)
	{
		for(int j = 0; j < (int)strlen(s2); j++)
		{
			for(int k = 0; k < (int)strlen(s3); k++)
			{
				if ( (s1[i] == s2[j]) && (s2[j]== s3[k]) )
				{
					int r = s1[i];
					if (r <= 'Z') r = r - 'A' + 27;
					else r = r - 'a' + 1;
					//printf("Found badge: %c - value: %d\n", s1[i], r);
					return r;
				}
			}
		}
	}
	return 0;
}

int part2(FILE *f)
{
	char inp1[1000];
	char inp2[1000];
	char inp3[1000];
	int cmn = 0;
	while (!feof(f))
	{
		fgets(inp1, 1000, f);
		fgets(inp2, 1000, f);
		fgets(inp3, 1000, f);
		if (feof(f)) break;
		cmn += findcmn(inp1, inp2, inp3);
	}
	return cmn;
}


int main()
{
	FILE *f = fopen("input.txt", "r");
	int rv;
	int sum = 0;
	while (!feof(f))
	{
		rv = part1(f);
		if (rv < 0) break;
		//printf("rv: %d\n", rv);
		sum += rv;
	}
	printf("Part 1 - Total is %d\n", sum);
	rewind(f);

	rv = part2(f);

	printf("Part 2 - Total is %d\n", rv);

}	
