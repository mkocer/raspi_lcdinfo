#include "kaws_curl.h"

char *trim (char *str);
double pAtSeeLevel(double p);
double dewarPoint(double temp, double hum);

struct MemoryStruct
{
  char *memory;
  size_t size;
};


static size_t
WriteMemoryCallback (void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *) userp;

  mem->memory = realloc (mem->memory, mem->size + realsize + 1);
  if (mem->memory == NULL)
    {
      /* out of memory! */
      printf ("not enough memory (realloc returned NULL)\n");
      exit (EXIT_FAILURE);
    }

  memcpy (&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}


struct KAWSstruct getKAWS (char *host)
{
  struct KAWSstruct KAWSdata = {-99.0, -99.0, -99.0, -99.0, -99.0, -1};
  CURL *curl_handle;
  struct MemoryStruct chunk;
  char kawsurl[255];
  
  strcpy(kawsurl, "http://");
  strcat(kawsurl, host);
  strcat(kawsurl, KAWSURI);

  chunk.memory = malloc (1);	/* will be grown as needed by the realloc above */
  chunk.size = 0;		/* no data at this point */

  curl_global_init (CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init ();

  /* specify URL to get */
  curl_easy_setopt (curl_handle, CURLOPT_URL, kawsurl);

  /* send all data to this function  */
  curl_easy_setopt (curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  /* we pass our 'chunk' struct to the callback function */
  curl_easy_setopt (curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);

  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt (curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  /* get it! */
  curl_easy_perform (curl_handle);

  /* cleanup curl stuff */
  curl_easy_cleanup (curl_handle);

  /*
   * Now, our chunk.memory points to a memory block that is chunk.size
   * bytes big and contains the remote file.
   *
   * Do something nice with it!
   *
   * You should be aware of the fact that at this point we might have an
   * allocated data block, and nothing has yet deallocated that data. So when
   * you're done with it, you should free() it as a nice application.
   */

  /* doing analysis of the chungk */
#define BUFFSIZE 32  
  if (chunk.memory) {
   char *pc1 = NULL, *pc2 = NULL, b[BUFFSIZE + 1];
	long i = 0;

   chunk.memory[chunk.size + 1] = '\0';
	trim(chunk.memory);
   if((pc1 = strchr(chunk.memory,'\n'))) {
      *pc1 = '\0';
   }
  // printf ("%s\n", chunk.memory);
	//04.10.2012 13:04:02   1958  5710  96519  115
	// date
	pc1 = chunk.memory;
	if ((pc2 = strchr(pc1,' '))) {
		i=0;
      while (pc1 < pc2) {
			b[i] = *pc1;
			pc1++; i++;
		}
		b[i] = '\0';
	}
	// time
	pc1 = pc2 + 1;
	if ((pc2 = strchr(pc1,' '))) {
		i=0;
      while (pc1 < pc2) {
			b[i] = *pc1;
			pc1++; i++;
		}
		b[i] = '\0';
	}
	// temp
	pc1 = pc2 + 1;
	if ((pc2 = strchr(pc1,' '))) {
		i=0;
      while (pc1 < pc2) {
			b[i] = *pc1;
			pc1++; i++;
		}
		b[i] = '\0';
	}
	KAWSdata.T = (double) atoi(b) / 100.0;
	// humid
	pc1 = pc2 + 1;
	if ((pc2 = strchr(pc1,' '))) {
		i=0;
      while (pc1 < pc2) {
			b[i] = *pc1;
			pc1++; i++;
		}
		b[i] = '\0';
	}
   KAWSdata.H = (double) atoi(b) / 100.0;
	// press
	pc1 = pc2 + 1;
	if ((pc2 = strchr(pc1,' '))) {
		i=0;
      while (pc1 < pc2) {
			b[i] = *pc1;
			pc1++; i++;
		}
		b[i] = '\0';
	}
	KAWSdata.p = (double) atoi(b) / 100.0;
	// light
	strcpy(b, pc2 + 1);
	KAWSdata.light = atoi(b);
  }
  // printf ("%lu bytes retrieved\n", (long) chunk.size);
  if (KAWSdata.p > -99) {
	  KAWSdata.bp = pAtSeeLevel(KAWSdata.p);
  }
  if ((KAWSdata.T > -99) && (KAWSdata.H > -99)) {
	  KAWSdata.DP = dewarPoint(KAWSdata.T, KAWSdata.H); 
  }

  /* free the chunk memory */
  if (chunk.memory)
    free (chunk.memory);

  /* we're done with libcurl, so clean it up */
  curl_global_cleanup ();

  return (KAWSdata);
}

/*
  Remove leading, trailing, & excess embedded spaces
 */
char *trim (char *str)
{
      char *ibuf, *obuf;

      if (str)
      {
            for (ibuf = obuf = str; *ibuf; )
            {
                  while (*ibuf && (isspace (*ibuf)))

		    ibuf++;
                  if (*ibuf && (obuf != str))
                        *(obuf++) = ' ';
                  while (*ibuf && (!isspace (*ibuf)))
                        *(obuf++) = *(ibuf++);
            }
            *obuf = '\0';
      }
      return (str);
}


double pAtSeeLevel(double p) {
	double bp = 0;
	
	bp = p / (exp(log(1.0 - (ALTITUDE / 44330.0)) * 5.256));

	return(bp);
}

double dewarPoint(double temp, double hum) {
	double dp = 0;

	dp = (log(hum) / log(10) - 2) / 0.4343 + (17.62 * temp) / (243.12 + temp);
	dp = 243.12 * dp / (17.62 - dp);

	return(dp);
}
