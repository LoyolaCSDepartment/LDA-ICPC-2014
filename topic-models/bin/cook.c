// cook data on impact of alpha on tpica models 
// exmaple input
//     67  --alpha 0.00015   aa aa-bb-cc bb
// presently keeping only those column with at least 1% of the data

#include "stdio.h"
#include "string.h"
#include <stdlib.h>

 
#define MAX_PATTERNS  1000
#define MAX_OUT_LINES 100

struct sColumn
{
    char *pattern;
    int counts[MAX_OUT_LINES];
} column[MAX_PATTERNS+1];
/*
  ".. .. aa-bb-cc", ".. aa aa-bb-cc", ".. aa bb-cc", ".. aa-bb aa-bb-cc",
  ".. aa-bb aa-cc", ".. aa-bb cc", ".. aa-bb-cc bb", ".. aa-cc bb",
  ".. aa-cc bb-cc", "aa aa aa-bb-cc", "aa aa bb-cc", "aa aa-bb aa-bb-cc",
  "aa aa-bb aa-cc", "aa aa-bb bb-cc", "aa aa-bb cc", "aa aa-bb-cc bb",
  "aa aa-bb-cc bb-cc", "aa aa-cc bb", "aa aa-cc bb-cc", "aa bb bb-cc",
  "aa bb cc", "aa bb-cc bb-cc", "aa bb-cc cc", "aa-bb aa-bb aa-cc",
  "aa-bb aa-bb cc", "aa-bb aa-bb-cc bb", "aa-bb aa-cc aa-cc", "aa-bb aa-cc bb",
  "aa-bb aa-cc cc", "aa-bb bb bb-cc", "aa-bb bb cc", "aa-bb-cc aa-cc bb",
  "aa-bb-cc bb bb", "aa-bb-cc bb cc", "aa-cc aa-cc bb", "aa-cc bb bb",
  "aa-cc bb bb-cc", "aa-cc bb cc", NULL
};
*/

// #define PATTERN_COUNT (sizeof(patterns) / sizeof (char*)-1)


static void output_counts(int outline, int *counts)
{
    int i;
	for(i=0; column[i].pattern != NULL; i++)
	    column[i].counts[outline] = *counts++;
}

static int column_compare(struct sColumn *c1, struct sColumn *c2)
{
    return strcmp(c1->pattern, c2->pattern);
}


// sort columns
static void sort_counts(outline)
{
    int column_size = sizeof(struct sColumn);
    int column_count;
    for(column_count=0; column[column_count].pattern != NULL; column_count++)
	    ;
    qsort (column, column_count, column_size,  
           (int(*)(const void *, const void *))column_compare);
}

static int column_sum(int *counts, int max)
{
    int sum = 0;
    int i;
	for(i=0; i<max; i++)
	  sum += *counts++;
    return sum;
}

static void print_counts(int max, int cutoff, double *alphas)
{
	int i,j;

	// fprintf(stderr, "cutoff %d \n", cutoff);

	printf("alpha");
	for(i=0; column[i].pattern != NULL; i++)
	    if (column_sum(column[i].counts,max) < cutoff)
		    column[i].pattern[0] = '!';
		else
	        printf("\t\"%s\"", column[i].pattern);
	printf("\n");
	
	for(j=0; j<max; j++)
	{
	    printf("%.3lg", alphas[j]);
	    for(i=0; column[i].pattern != NULL; i++)
	        if (column[i].pattern[0] != '!')
			{
			    if ( column[i].counts[j] == 0)
			        printf("\tNA");
				else
			        printf("\t%d", column[i].counts[j]);
			}
	    printf("\n");
	}
}

static int find_pattern(char *needle)
{
	int i;
	
	for(i=0; column[i].pattern != NULL; i++)
	    if (strcmp(needle, column[i].pattern) == 0)
		    return i;

    if (i >= MAX_PATTERNS)
    {
        fprintf(stderr, "PANIC!! out of pattern space for %s\n", needle);
        return -1;
    }

    column[i].pattern = strdup(needle);
    column[i+1].pattern = NULL;
	return i;
}

int main()
{
    int counts[MAX_PATTERNS];
	double alphas[MAX_OUT_LINES];
	int outline = 0;
	int total_datapoints = 0;
	int i;
	double current_alpha = -1.0;
	char buff[100];
	//for(i=0; i<MAX_MATTERNS+1 i++)
	  column[0].pattern = NULL;
	
    while (fgets(buff, 100, stdin) != NULL)
	{
	    char p1[10], p2[10], p3[10];
		double alpha;
		int count;
        if (*buff == '#')
            continue;
	    int x = sscanf(buff, "%d %lf %s %s %s", &count, &alpha, p1, p2, p3);
		if (x != 5)
		    fprintf(stderr, "OOPs %d != 5 for %s", x, buff);
	    total_datapoints += count;

		if (alpha != current_alpha)
		{
		    if (current_alpha > 0)
			{
                alphas[outline] = current_alpha;
			    output_counts(outline++, counts);
			}
			current_alpha = alpha;
            for(i=0; i<MAX_PATTERNS; i++)
                counts[i] = 0;
		}
        if (*p2 == '.') *p2 = ' ';
        if (*p3 == '.') *p3 = ' ';

        sprintf(buff, "%s %s %s", p1, p2, p3);
		int p = find_pattern(buff);
		if (p != -1) counts[p] = count;
	}


    alphas[outline] = current_alpha;
    output_counts(outline++, counts);

	sort_counts(outline);
	print_counts(outline, total_datapoints*01/100, alphas);

    return 0;
}
