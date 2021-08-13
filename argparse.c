
#include "argparse.h"

int get_arg(int argc, char* argv[], char* argstr, int strlen, char** arg)
{

  int nopt=1, status=1;

  while (nopt < argc)
    {
      if (strncmp(argv[nopt],argstr,strlen) == 0)
	{
	  nopt++;
	  if (nopt >= argc)
	  {
	    ERREX("need argument after -input");
          } 
	  else if (strncmp(argv[nopt],"-",1) == 0)
	  {
	    ERREX("need argument after input");
	  }
	  else
	  {
	  *arg = argv[nopt];
	  status = 0;
	  }
	}
      nopt++;
    }
  return status;
}

/* int get_arg_array(int argc, char* argv[], char* argstr, int strlen, char** arg[])
{

  int nopt=1, status=1, count=0;

  while (nopt < argc)
    {
      if (strncmp(argv[nopt],argstr,strlen) == 0)
        {
	  nopt++;
	  while (!abs(strncmp(argv[nopt],"-",1)) && (nopt <= argc))
	    {
	      *arg[count] = argv[nopt];
	      count++;
	      nopt++;
	      status = 0;
	    }
        }
      nopt++;
    }
  return status;
} */

int get_arg_array_float(int argc, char* argv[], char* argstr, int strlen, char* arg)
{

  int nopt=1, status=1, count=0;

  while (nopt < argc)
    {
      printf("\nargc = %d, nopt = %d\n",argc,nopt);
      if (strncmp(argv[nopt],argstr,strlen) == 0)
        {
	  printf("\nif\n");
	  nopt++;
	  printf("\n%s\n",argv[nopt]);
	  while (!abs(strncmp(argv[nopt],"-",1)) && (nopt <= argc))
	    {
	      *(arg + count) = scanf("%f", argv[nopt]);
	      count++;
	      nopt++;
	      status = 0;
	      printf("\nwhile\n");
	    }
        }
      nopt++;
    }
  return status;
}      








