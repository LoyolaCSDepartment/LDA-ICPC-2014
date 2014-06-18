//REPLACED with perl script select words

// massage R's output to the format mallet output so that R's 
// output can be fed into the mallet pipeline.
// *** this is the works that make up topic part of the transformation (the top of the file)
// scott needs the lower part
#include "stdio.h"
#include <stdlib.h>
#include "string.h"
#include "malloc.h"

typedef int(*cmpfn)(const void *, const void *);


//#define THRESHOLD 0.01
//#define THRESHOLD 0.33  -- to high 20% cc is sufficient
#define THRESHOLD 0.01
typedef struct
{
    char word[10];
    double weight;
} tWord;

typedef struct
{
    tWord words[3];
} tTopic;



static int cmp(char **a, char **b)
{
    // printf("compare %s %s\n", *a , *b);
    return strcmp(*b, *a);
}


static int topic_cmp(tTopic *a, tTopic *b)
{
    if (a->words[0].weight != b->words[0].weight)
        return (int)(10000*(b->words[0].weight - a->words[0].weight));
    if (a->words[1].weight != b->words[1].weight)
        return (int)(10000*(b->words[1].weight - a->words[1].weight));
    return (int)(10000*b->words[2].weight - a->words[2].weight);
}

static int word_cmp(tWord *a, tWord *b)
{
    return *a->word - *b->word;
}

static void lint() { lint(); topic_cmp(0,0);}

int main()
{
    char buff[1024], alpha[1024];
    char *out[3];
    out[0] = malloc(20);
    out[1] = malloc(20);
    out[2] = malloc(20);
    double largest_ignored = 0.0;
  
    strcpy(alpha, "no alpha line found yet");
    while (fgets(buff, 1024, stdin) != NULL)
    {
        char order[3][10] = {"aa", "bb", "cc"};
        int i, j;
        tTopic t[3];

        if (strncmp(buff, "alpha", 5) == 0)
        {
            char *ch = strchr(buff, '\n');
            *ch = '\0';
            ch = strchr(buff, '=');
            strcpy(alpha, ch+1);
            continue;
        }

        if (strstr(buff, "BREAK BREAK BREAK") == NULL)
            continue;

        // sometimes you get all three topics on one line
        // sometimes not ....
        fgets(buff, 1024, stdin); // scan off "Topic #1 ..."
        int x = sscanf(buff, "%s %s %s", t[0].words[0].word, t[0].words[1].word, t[0].words[2].word);
        if (x != 3)
          fprintf(stderr, "counts are wrong :( %d\n", x);
 
        for(i=0; i<3; i++)
        {
            strcpy(t[i].words[0].word, t[0].words[0].word);
            strcpy(t[i].words[1].word, t[0].words[1].word);
            strcpy(t[i].words[2].word, t[0].words[2].word);
            char *ch;
            fgets(buff, 1024, stdin); 
            for(ch = index(buff, '"'); ch != NULL; ch = index(ch, '"'))
                *ch = ' ';
            x = sscanf(buff, "%*s %*s %lf %lf %lf", &t[i].words[0].weight, 
                           &t[i].words[1].weight, &t[i].words[2].weight);
            if (x != 3)
              fprintf(stderr, "counts are wrong :( %d\n", x);
        }

        // put words in abc order  
        // [[ 3/13 malcom output forms likely obviates the need for this ]]
        for(i=0; i<3; i++)
            qsort(t[i].words, 3, sizeof(tWord), (cmpfn)word_cmp);

        for(i=0; i<3; i++)
        {
		    if (t[0].words[i].weight < THRESHOLD && t[0].words[i].weight > largest_ignored)
			    largest_ignored = t[0].words[i].weight;
		    if (t[1].words[i].weight < THRESHOLD && t[1].words[i].weight > largest_ignored)
			    largest_ignored = t[1].words[i].weight;
		    if (t[2].words[i].weight < THRESHOLD && t[2].words[i].weight > largest_ignored)
			    largest_ignored = t[2].words[i].weight;
		}

        //for(i=0; i<3; i++)
        //{
            //printf("Topic %d: %s %5.10lf   %s %5.10lf   %s %5.10lf\n", i+1,
                    //t[i].words[0].word, t[i].words[0].weight,
                    //t[i].words[1].word, t[i].words[1].weight,
                    //t[i].words[2].word, t[i].words[2].weight);
        //}

        // sequencing and sorting below obviates the need for this
        //qsort(t, 3, sizeof(tTopic), (cmpfn)topic_cmp);

        //for(i=0; i<3; i++)
        //{
            //printf("Topic %d: %s %5.10lf   %s %5.10lf   %s %5.10lf\n", i+1,
                    //t[i].words[0].word, t[i].words[0].weight,
                    //t[i].words[1].word, t[i].words[1].weight,
                    //t[i].words[2].word, t[i].words[2].weight);
        //}

		
		for(i=0; i<3; i++)
		    out[i][0] = '\0';

		for(i=0; i<3; i++)
		{
		    for(j=0; j<3; j++)
			{
		        if (strcmp(order[i], t[0].words[j].word) == 0)
				    strcat(out[0], (t[0].words[j].weight > THRESHOLD ? t[0].words[j].word: ".."));
		        if (strcmp(order[i], t[1].words[j].word) == 0)
				    strcat(out[1], (t[1].words[j].weight > THRESHOLD ? t[1].words[j].word: ".."));
		        if (strcmp(order[i], t[2].words[j].word) == 0)
				    strcat(out[2], (t[2].words[j].weight > THRESHOLD ? t[2].words[j].word: ".."));
			}
		    if (i < 2)
			{
			    strcat(out[0], "-");
			    strcat(out[1], "-");
			    strcat(out[2], "-");
			}
		}

//		printf("iteration 000 %s  %s %s %s\n", alpha, out[0], out[1], out[2]);
		qsort(out, 3, sizeof(out[0]), (cmpfn)cmp);
		printf("iteration 000 %s  %s %s %s\n", alpha, out[0], out[1], out[2]);
	}
    fprintf(stderr, "largest ignored = %10.8lf\n", largest_ignored);
    return 0;
}

