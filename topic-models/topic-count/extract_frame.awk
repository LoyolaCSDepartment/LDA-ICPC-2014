
# currently only using data for suffix-remove (i.e., $2) == 0

BEGIN {current = -1;}
      {if ($3 == 0)
        {if (current != $2)
         {
            if (current != -1) printf("\n");
            current = $2;
            printf("%s ", $2);
         }
         printf("%s ", $5);
        }
      }
END   {printf ("\n");}
