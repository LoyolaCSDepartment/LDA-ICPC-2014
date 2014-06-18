// cook up data
// data points (one per directory) need to be averages as LDA has sampled the 
// possible answers
// out-postgresql-07-00-00-norm.tagit-1-1000.30204/average_postgresql-07-00-00-norm.tagit_doc_0.txt
// 5,5,0.0968008255934

#include "stdio.h"
#include "strings.h"
#include "stdlib.h"

#define MAX_SCORES 100
#define MAX_POINTS 1000   // unique doc-topic_count-num_match combinations
#define MAX_LABELS 100   

typedef struct 
{
   int doc;
   int topic_count;
   int num_matches;
   int score_count;
   double average_score;
   double scores[MAX_SCORES];
} point;

static void fill_in_average(point *p)
{
    int i;
    double sum = 0.0;
    for(i=0; i < p->score_count; i++)
        sum += p->scores[i];
    p->average_score = sum / p->score_count;
}

static void point_print( point *p)
{
    int i;
    printf("%2d  %3d  %2d  %7.4lf", p->doc, p->topic_count, 
           p->num_matches, p->average_score);

    for(i=0; i < p->score_count; i++)
        printf("   %7.4lf", p->scores[i]);
    printf("\n");
}

static point *find_point(point *points, int doc, int tc, int nm)
{
    point *end = &points[MAX_POINTS];

    for(;points->doc >= 0; points++)
    {
        if (points->doc == doc && points->topic_count == tc 
            && points->num_matches == nm)
        {
            return points;
        }
    }

    if (points == end)
    {
        fprintf(stderr, "\nincrease MAX_POINTS!\n\n");
        exit(-1);
    }

    (points+1)->doc = -1; // just in case new high water mark
    return points;
}

// name is "..._doc_0.txt" or "..._topic_0.txt"
static int extract_doc(char *name)
{
    char *cp = rindex(name, '_');
    if (cp == NULL)
        return -1;
    cp++; //
    if (!isdigit(*cp) || *(cp+1) != '.')
        fprintf(stderr, "file name patter fail in %s at %s\n", name, cp);
    return *cp - '0';
}

static void add_label(int *a, int l)
{
    for(; *a != -1; a++)
        if (*a == l)
            return;
    *a = l;
}

static int int_compare(int *a, int *b)
{
    return *a - *b;
}

static int count(int *a)
{
    int i;
    for(i=0; *a > 0; i++, a++)
        ;
    return i;
}

static void dump_by_topic(point *points, int max_doc, int *topic_labels, 
       int topic_label_count, int *match_labels, int match_label_count)
{
    int doc;
    for(doc = 0; doc <= max_doc; doc++)
    {
        int t, n;
        printf("#DOC %d\n topics", doc);
        for(n=0; n<match_label_count; n++)
            printf("    n%d ", match_labels[n]);
        printf("\n");

        for(t=0; t<topic_label_count; t++)
        {
            printf(" %3d ", topic_labels[t]);

            for(n=0; n<match_label_count; n++)
            {
                point *p = find_point(points, doc,  topic_labels[t], match_labels[n]);
                if (p->doc < 0)
                    fprintf(stderr, "BAD LOOKUP Bert!\n");
               printf(" %7.4lf", p->average_score);
            }
            printf("\n");
        }
    }
}

static void dump_raw_by_topic(point *points, int max_doc, int *topic_labels, 
       int topic_label_count, int *match_labels, int match_label_count)
{
    int n, doc;
    for(doc = 0; doc <= max_doc; doc++)
    {
        for(n=0; n<match_label_count; n++)
        {
            int t;
            printf("#DOC-%d--matches-%d\n topics", doc, match_labels[n]);

            printf("    average  raw points \n");

            for(t=0; t<topic_label_count; t++)
            {
                printf(" %3d ", topic_labels[t]);
    
                point *p = find_point(points, doc,  topic_labels[t], match_labels[n]);
                if (p->doc < 0)
                    fprintf(stderr, "BAD LOOKUP Bert!\n");
                printf(" %7.4lf", p->average_score);
                int i;
                for(i=0; i< p->score_count; i++)
                    printf(" %7.4lf", p->scores[i]);
                  
                printf("\n");
            }
        }
    }
}


main (int argc, char*argv[])
{
    char buff[1024];
    FILE *fin;
    point points[MAX_POINTS];
    points[0].doc = -1; // high water mark
    int doc, max_doc = -1;   // document counts assumes sequential

    int topic_labels[MAX_LABELS];
    int match_labels[MAX_LABELS];
    int i;
    for(i=0; i<MAX_LABELS; i++)
    {
        topic_labels[i] = -1;
        match_labels[i] = -1;
    }

    for(argv++; *argv != NULL; argv++)
    {
        double score;
        int tc, nm;
        fin = fopen(*argv, "r");
        doc = extract_doc(*argv);
        if (doc < 0) 
        {
            fprintf(stderr, "%s bad: doc %d\n", *argv, doc);
            continue;
        }

        if (fin == NULL)
        {
            fprintf(stderr, "%s bad: Could not open file for reading\n", *argv);
            continue;
        }

        if (doc > max_doc)
            max_doc = doc;

        while (fgets(buff, 1024, fin) != NULL)
        {
            int x = sscanf(buff, "%d,%d,%lf", &tc, &nm, &score);
            if (x != 3)
                fprintf(stderr, "only converted %d entries from %s\n", x, buff);
            point *p = find_point(points, doc, tc, nm);

            add_label(topic_labels, tc);
            add_label(match_labels, nm);

            if (p->doc < 0)
            {
                p->doc = doc;
                (p+1)->doc = -1;
                p->topic_count = tc;
                p->num_matches = nm;
                p->score_count = 1;
                p->scores[0] = score;
            }
            else
            {
                p->scores[p->score_count++] = score;
            }
        }

        fclose(fin);
    }

    point *p;
    // printf(" doc  topic-count  number-matches  average  scores\n");
    for(p=points; p->doc >= 0; p++)
    {
        fill_in_average(p);
        //point_print(p);
    }

    int topic_label_count = count(topic_labels);
    int match_label_count = count(match_labels);
    qsort(topic_labels, topic_label_count, sizeof(int), 
          (int(*)(const void *, const void *)) int_compare);
    qsort(match_labels, match_label_count, sizeof(int), 
          (int(*)(const void *, const void *)) int_compare);

    dump_by_topic(points, max_doc, topic_labels, topic_label_count, match_labels, match_label_count);

    //dump_raw_by_topic(points, max_doc, topic_labels, topic_label_count, match_labels, match_label_count);


}

