# input file columns:  topic-count, neighborhood-size, avg_file, avg_directory, sum($3,$4)

BEGIN    {last = -1; printf("topics 5 10 25 50");}
         {
             if ($1 != last) { printf("\n%s ", $1); last = $1; }
             if (USE_AVERAGE_FILE == 1) printf("%s ", $3); else printf("%s ", $4);
         }
END      {printf("\n");}
