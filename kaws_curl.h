/**** Header file for CURL getting function for KAWS ****/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <curl/curl.h>

#define KAWSURI "/data.txt"
#define ALTITUDE 390.0

char *trim (char *str);

struct KAWSstruct
{
  double T, H, p, bp, DP;
  int light;
};

struct KAWSstruct getKAWS(char *host);
/**** END *****/
