#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <libgen.h>
#include <errno.h>

#include "corsika2geant.h"

double corsika_times(char *filename, int np)
{
  int i, j, k, m, n;
  int blockLen; 
  int nWord, nBlock, evtcnt=0;
  float energy, sectheta, id, buf[NWORD], bufpart[NPART], buf3[NPART], x, y;
  float Z0=1430.e2;
  double count=0.;
  FILE *f, *flist;
  int nRUNH, nEVTH, nLONG, nEVTE, nRUNE;
  int nPARTSUB, outnum=0;
  char blockName[5], bufName[4], filename2[256];
  off_t c;
  off_t RB=sizeof(float);
  tmin = 1.e9;
  for (i = 0; i < NX; i++ )
    {
      for (j = 0; j < NY; j++ )
	{      
	  time1[i][j] = 1.e9;
	}
    }
  fprintf(stderr,"Opening %s filelist\n", filename);
  if (( flist = fopen(filename, "r")) == NULL)
    {
      fprintf(stderr,"Cannot open %s file: %s\n", filename, strerror(errno));
      exit(EXIT_FAILURE);
    }
  nRUNH = nEVTH = nLONG = nEVTE = nRUNE = 0;
  nPARTSUB = 0;
  nBlock = 0;
  while (fscanf(flist, "%s\n", filename2) != EOF)
    {
      if (( f = fopen(filename2, "rb")) == NULL )
        {
          fprintf(stderr,"Cannot open %s file \n", filename2);
          exit(EXIT_FAILURE);
        }
      fprintf(stderr,"Opening %s file\n", filename2);
      while (fread(&blockLen, sizeof(int), 1, f))
	{
	  nBlock++;
	  for (i = 0; i < NSUBBLOCK; i++)
	    {
	      fread(bufName, sizeof(char), 4, f);
	      fseeko(f, -RB, SEEK_CUR);
	      strncpy(blockName, bufName, 4);
	      blockName[4] = '\0';
	      if (!strcmp("RUNH", blockName))
		{
		  nRUNH++;
		  fread(buf, sizeof(float), NWORD, f);
		}
	      else if (!strcmp("EVTH", blockName))
		{
		  if ( nEVTH > evtcnt )
		    { 
		      fprintf(stderr, "There is more than one CORSIKA event in %s, only reading the first event\n",
			      filename2);			  
		      return count;
		    }
		  nEVTH++;
		  fread(eventbuf, sizeof(float), NWORD, f);
		  origin[0] = -eventbuf[7]/eventbuf[9]*(eventbuf[6]-Z0);
		  origin[1] = -eventbuf[8]/eventbuf[9]*(eventbuf[6]-Z0);
		  origin[2] = eventbuf[6];
		  tmin=hypotf(hypotf(origin[0],origin[1]),origin[2]-Z0)/CSPEED;
		  zenith = eventbuf[10];
		}
	      else if (!strcmp("LONG", blockName))
		{
		  fread(buf, sizeof(float), NWORD, f);
		  nLONG++;
		}
	      else if (!strcmp("EVTE", blockName))
		{
		  nEVTE++;
		  fread(buf, sizeof(float), NWORD, f);
		}
	      else if (!strcmp("RUNE", blockName))
		{
		  nRUNE++;
		  fread(buf, sizeof(float), NWORD, f);
		}
	      else
		{
		  for (j = 0; j < 39; j++)
		    {
		      nPARTSUB++;
		      fread(buf3, sizeof(float), NPART, f);
		      if (fabs(buf3[6]) < 1.e-15 ||
			  fabs(buf3[6]) > 1.e10 ||
			  fabs(buf3[1]) < 1.e-15 ||
			  fabs(buf3[1]) > 1.e10 ||
			  fabs(buf3[2]) < 1.e-15 ||
			  fabs(buf3[2]) > 1.e10 ||
			  fabs(buf3[3]) < 1.e-15 ||
			  fabs(buf3[3]) > 1.e10 ||
			  fabs(buf3[4]) < 1.e-15 ||
			  fabs(buf3[4]) > 1.e10 ||
			  fabs(buf3[5]) < 1.e-15 ||
			  fabs(buf3[5]) > 1.e10 ||
			  fabs(buf3[0]) < 1.e-15 ||
			  fabs(buf3[0]) > 1.e10 )
			{
		      //		      fprintf(stderr, "Short read from PARTICLE sub-block.\n");
		      //		      fprintf(stderr, "Read %d blocks\n", nBlock);
		      //		      fprintf(stderr, "RUNH: %d, EVTH: %d, PARTSUB: %d, LONG: %d, EVTE: %d, RUNE: %d\n",
		      //			     nRUNH, nEVTH, nPARTSUB, nLONG, nEVTE, nRUNE);
			  if ( buf3[0] != 0.)
			    {
			      fprintf(stderr, "Number of blocks: %d\n%g %g %g %g %g %g %g\n", nBlock, bufpart[0], bufpart[1], bufpart[2], bufpart[3], bufpart[4], bufpart[5], bufpart[6]); 	
			      fprintf(stderr, "%g %g %g %g %g %g %g\n\n", buf3[0], buf3[1], buf3[2], buf3[3], buf3[4], buf3[5], buf3[6]);
			    }
			  //		      exit(EXIT_FAILURE);
			}
		      bufpart[0] = buf3[0];
		      bufpart[1] = buf3[1];
		      bufpart[2] = buf3[2];
		      bufpart[3] = buf3[3];
		      bufpart[4] = buf3[4];
		      bufpart[5] = buf3[5];
		      bufpart[6] = buf3[6];
		      /*		  if ( buf3[6] < 370000)
					  {
					  fprintf(stderr, "%g %g %g %g %g %g %g\n", buf3[0], 
			      buf3[1], buf3[2], buf3[3], buf3[4], 
			      buf3[5], buf3[6]);
		      exit(EXIT_FAILURE);
		      }*/
		      
//  if(count < 1000) printf("np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]); 

		      if (fabsf(buf3[4]) < (float)(100*DISTMAX) &&
			  fabsf(buf3[5]) < (float)(100*DISTMAX) &&
//			  hypotf(buf3[4],buf3[5]) < (float)(100*DISTMAX) &&
			  buf3[6] > 1.e4)
			{
			  energy = hypotf(buf3[3], 
					  hypotf(buf3[1],buf3[2]));
			  sectheta = energy/buf3[3];
			  energy = hypotf(pmass[(int)(buf3[0]/1000.)],
					  energy) -
			    pmass[(int)(buf3[0]/1000.)];
			  m = (int)((buf3[4]+(float)(100*DISTMAX))/600.);
			  n = (int)((buf3[5]+(float)(100*DISTMAX))/600.);
			  if ( buf3[0] >= 1000. && buf3[0]  < 26000. &&
			       energy > emin/1.e6)
			    {
			      count++;
//  if(count < 1000) printf("count   np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]); 
			      if ( buf3[6] < time1[m][n])
				{
//  if(count < 1000) printf("if buf6<time1   np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]); 
//  if((np==1 && (int)(buf3[0]/1000) == 1) || (np==2 && (int)(buf3[0]/1000) == 2) || (np==2 && (int)(buf3[0]/1000) == 3)){
//  if((np==1 && (int)(buf3[0]/1000) == 1) || (np==2 && (int)(buf3[0]/1000) == 2) || (np==3 && (int)(buf3[0]/1000) >= 3)){
				  if ( buf3[6] < sqrtf(powf(origin[0]-buf3[4], 2.) + 
						       powf(origin[1]-buf3[5], 2.) + 
						       powf(origin[2]-Z0, 2.))/CSPEED)
				    {
				      outnum++;
				    }
				  else
				    {
//  if(count < 1000) printf("if not outnum   np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]); 

  if((np==1 && (int)(buf3[0]/1000) == 1) ){
//    if(count < 1000) printf("------ np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]);
				      time1[m][n] = buf3[6];
  }
  if((np==2 && (int)(buf3[0]/1000) == 2) || (np==2 && (int)(buf3[0]/1000) == 3) ){
//    if(count < 1000) printf("------ np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]);
				      time1[m][n] = buf3[6];
  }
  if((np==3 && (int)(buf3[0]/1000) == 5) || (np==3 && (int)(buf3[0]/1000) == 6) ){
//    if(count < 1000) printf("------ np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]);
                                      time1[m][n] = buf3[6];
  }
  if((np==4 && (int)(buf3[0]/1000) > 6) ){
//    if(count < 1000) printf("------ np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]);
                                      time1[m][n] = buf3[6];
  }
  if(np==0 ){
//    if(count < 1000) printf("------ np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]);
                                      time1[m][n] = buf3[6];
  }

				    }
/*  else if((np==3 && (int)(buf3[0]/1000) == 5) || (np==3 && (int)(buf3[0]/1000) == 6) || (np==4 && (int)(buf3[0]/1000) > 6)){
                                  if ( buf3[6] < sqrtf(powf(origin[0]-buf3[4], 2.) +
                                                       powf(origin[1]-buf3[5], 2.) +
                                                       powf(origin[2]-Z0, 2.))/CSPEED)
                                    {
                                      outnum++;
                                    }
                                  else
                                    {
                                      time1[m][n] = buf3[6];
                                    }
  }*/
				}
			    }
			}		      
		    }
		}
	    }
	  fread(&blockLen, sizeof(int), 1, f);
	  nWord = blockLen/sizeof(float);
	}
      fclose(f);
      evtcnt++;
    }
  fclose(flist);
  printf ("Number of Outliers: %d\nTime of Core Impact: %g", outnum, tmin);
  printf("read %d blocks\n", nBlock);
  printf("RUNH: %d, EVTH: %d, PARTSUB: %d, INT: %d, EVTE: %d, RUNE: %d\n",
			 nRUNH, nEVTH, nPARTSUB, nLONG, nEVTE, nRUNE);
//  if(np != 5) fill_time();
  fill_time();
  for (i = 0; i < NX; i++ )
    for (j = 0; j < NY; j++ )
      time1[i][j] = tmin + (float)DT*floorf((time1[i][j]-tmin)/(float)DT);
////
    for (j = 0; j < NY; j++ ) fprintf(stdout, "np: %d, j: %d, time[NX/2][j]: %f \n", np, j, time1[NX/2][j]);
////
  return count;
}


void fill_time()
{

  int m=NX/2, n, m1, m2, n1, n2;
  float  x, y, x2, y2;
  while ( time1[m][NY/2] == 1.e9 ) m++;  
  filldist = (float)m*6.0 - (float)DISTMAX + 5.;
  printf ( "Annulus Diameter: %g\n", filldist);
  dm = m - NX/2 + 5;
  dn = dm;
  for ( m = NX/2-dm; m < NX/2+dm; m++)
    for ( n = NY/2-dn; n < NY/2+dn; n++)
      {
	x = (float)m*6.0 - (float)DISTMAX + 3.0;
	y = (float)n*6.0 - (float)DISTMAX + 3.0;
	if ( hypotf( x, y ) < filldist )
	  {
	    x2 = x/hypotf(x,y)*(filldist+7.5);
	    y2 = y/hypotf(x,y)*(filldist+7.5);
	    m1 = (int)((x2+(float)DISTMAX)/6.);
	    n1 = (int)((y2+(float)DISTMAX)/6.);
	    m2 = (int)((-x2+(float)DISTMAX)/6.);
	    n2 = (int)((-y2+(float)DISTMAX)/6.);
	    time1[m][n] = (time1[m1][n1]+time1[m2][n2])/2.;
	    time1[m][n] += (time1[m1][n1]-time1[m2][n2])*
	      hypotf(x,y)/(filldist+7.5)/2.0;
	  }
      }
}
