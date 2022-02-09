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
#include "eloss_sdgeant.h"

void gauss(double pair[]);


//double corsika_vem_init(char *filename, char *tmpfile, int tcount, int np, double avem[NSUBBLOCK][40][2])
double corsika_vem_init(char *filename, char *tmpfile, int tcount, int np, double avem0[NSUBBLOCK][40], double avem1[NSUBBLOCK][40])
{
  int i, j, k, m, n;
  FILE *f, *fout, *flist;
  short pz_tmp;
  int nRUNH=0, nEVTH=0, nLONG=0, nEVTE=0, nRUNE=0;
  int nPARTSUB=0, sblockcount=0, partcount=0;
  int blockLen; 
  int nWord, nBlock, evtcnt=0;;
  float buf3[NPART], R0[3], d1[3], evtebuf[NWORD], runebuf[NWORD];
  float energy, sectheta, id, buf[NWORD], sectheta2, e1, e2, rnum, rnum2;
  double count2=0., vem_tmp[2], gp[2];
////
//  double agp[NSUBBLOCK][40][2];
//  double avem[16][NSUBBLOCK][40][2];
////
  char blockName[5], bufName[4], filename2[256];
  off_t c;
  off_t RB=sizeof(float);
  for (i = 0; i < NX; i++ )
    {
      for (j = 0; j < NY; j++)
	{
	  for (k = 0; k < NT; k++)
	    {
	      vemcount[i][j][k][0] = 0;
	      vemcount[i][j][k][1] = 0;
	      pz[i][j][k] = 0;
	    }
	}
    }
  if (( flist = fopen(filename, "r")) == NULL)
    {
      fprintf(stderr,"Cannot open %s file: %s\n", filename, strerror(errno));
      exit(EXIT_FAILURE);
    }
  if (( fout = fopen(tmpfile, "w")) == NULL)
    {
      fprintf(stderr,"Cannot open %s file: %s\n", tmpfile, strerror(errno));
      exit(EXIT_FAILURE);
    }
  fwrite(&blockLen, sizeof(int), 1, fout); 
  while (fscanf(flist, "%s\n", filename2) != EOF)
    {
printf("in: %s\n", filename2);
printf( "in   ( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem0[13][35], (float)avem1[13][35]);
      if (( f = fopen(filename2, "rb")) == NULL )
        {
          fprintf(stderr,"Cannot open %s file \n", filename2);
          exit(EXIT_FAILURE);
        }
      fprintf(stderr,"Opening %s file\n", filename2);
      while (fread(&blockLen, sizeof(int), 1, f))
	{

//printf( "fread   ( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem0[13][35], (float)avem1[13][35]);

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
		  if (evtcnt == 0)
		    {
		      fwrite(buf, sizeof(float), NWORD, fout);
		      sblockcount++;
		      if (sblockcount == NSUBBLOCK)
			{
			  sblockcount = 0;
			  fwrite(&blockLen, sizeof(int), 1, fout);
			  fwrite(&blockLen, sizeof(int), 1, fout);
			}
		    }
		}
	      else if (!strcmp("EVTH", blockName))
		{
		  nEVTH++;
		  fread(buf, sizeof(float), NWORD, f);
		  if (evtcnt == 0)
		    {
		      fwrite(buf, sizeof(float), NWORD, fout);
		      sblockcount++;
		      if (sblockcount == NSUBBLOCK)
			{
			  sblockcount = 0;
			  fwrite(&blockLen, sizeof(int), 1, fout);
			  fwrite(&blockLen, sizeof(int), 1, fout);
			}
		    }
		}	      
	      else if (!strcmp("LONG", blockName))
		{
		  nLONG++;
		  fread(buf, sizeof(float), NWORD, f);
		}
	      else if (!strcmp("EVTE", blockName))
		{
		  nEVTE++;
		  fread(evtebuf, sizeof(float), NWORD, f);
		}
	      else if (!strcmp("RUNE", blockName))
		{
		  nRUNE++;
		  fread(runebuf, sizeof(float), NWORD, f);
		}
	      else
		{
		  for (j = 0; j < 39; j++)
		    {
		      nPARTSUB++;
		      fread(buf3, sizeof(float), NPART, f);
		      if (fabsf(buf3[4]) < (float)(100*DISTMAX) &&
			  fabsf(buf3[5]) < (float)(100*DISTMAX) &&
//			  hypotf(buf3[4],buf3[5]) < (float)(100*DISTMAX) &&
			  buf3[0] >= 1000. && buf3[0]  < 26000. &&
			  buf3[6] > 1.e4)
			{
//// empty for atmos
  if(np==5 ){
  }
//// gamma
//  if(np==1 && (int)buf3[0] % 1000 == 1){
//  printf("----------------------------------------------\n");
//  printf("np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]);
////  if(count2 < 1000) printf( "( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem[13][35][0], (float)avem[13][35][1]);
//  if(count2 < 10000) printf( "( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem0[13][35], (float)avem1[13][35]);

			  m = (int)((buf3[4]+(float)(100*DISTMAX))/600.);
			  n = (int)((buf3[5]+(float)(100*DISTMAX))/600.);
			  k = (int)(( buf3[6] - time1[m][n] - 
				      (float)(NT*DT*tcount))/DT);

//  if(count2 < 10000) printf("start   np:%d, buf0:%f, vem0:%f, vem1:%f, time1:%f, m:%d, n:%d, k:%d\n", np, buf3[0], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n], m, n, k);
//  if(count2 < 10000) printf("   tcount:%d, time1:%f, time1+1:%f\n", tcount, time1[m][n]+(float)(NT*DT*tcount), time1[m][n]+(float)(NT*DT*(tcount+1)));
			  if ( buf3[6] >= time1[m][n]+(float)(NT*DT*tcount)
			       && buf3[6] < 
			       time1[m][n]+(float)(NT*DT*(tcount+1)))
			    {
			      energy = hypotf(buf3[3], 
					      hypotf(buf3[1],buf3[2]));
			      sectheta = energy/buf3[3];
			      energy = hypotf(pmass[(int)(buf3[0]/1000.)],
					      energy) -
				pmass[(int)(buf3[0]/1000.)];
			      k = (int)(( buf3[6] - time1[m][n] - 
					  (float)(NT*DT*tcount))/(float)DT);

//  if(count2 < 10000) printf("start   np:%d, buf0:%f, vem0:%f, vem1:%f, time1:%f, m:%d, n:%d, k:%d, energy:%f\n", np, buf3[0], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n], m, n, k, (float)energy);

			      if (energy > emin/1.e6) 
				{
				  count2++;
				}
			      if ((int)vemcount[m][n][k][0] < 60000 && 
				  (int)vemcount[m][n][k][1] < 60000 &&
				  energy > emin/1.e6 )
				{
//  if(count2 < 10000) printf("b eloss   np:%d, buf0:%f, vem_tmp0:%f, vem_tmp1:%f, vem0:%f, vem1:%f, time1:%f\n", np, buf3[0], (float)vem_tmp[0], (float)vem_tmp[1],  (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
				  get_elosses((int)(buf3[0]/1000), 
					      energy*1000., sectheta, 
					      &vem_tmp[0], &vem_tmp[1]);
//  if(count2 < 10000) printf("a eloss   np:%d, buf0:%f, vem_tmp0:%f, vem_tmp1:%f, vem0:%f, vem1:%f, time1:%f\n", np, buf3[0], (float)vem_tmp[0], (float)vem_tmp[1], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);

//  if(np==0){
				  gauss(gp);
/*    agp[i][j][0] = gp[0];
    agp[i][j][1] = gp[1];
  }
  else{
    gp[0] = agp[i][j][0];
    gp[1] = agp[i][j][1];
  }*/
				  vem_tmp[0] *= 1.+0.07*gp[0];
				  vem_tmp[1] *= 1.+0.07*gp[1];
  if(np==0){
    avem0[i][j] = vem_tmp[0];
    avem1[i][j] = vem_tmp[1];
//    avem[i][j][0] = vem_tmp[0];
//    avem[i][j][1] = vem_tmp[1];

    if(vem_tmp[0] > 0 || vem_tmp[1] > 0){
//      iflg[aa] = i;//      jflg[aa] = j;
      npflg[aa] = buf3[0];
      tflg[aa] = time1[m][n];
      xflg[aa] = buf3[4];
      yflg[aa] = buf3[5];
      bvem[aa][0] = vem_tmp[0];
      bvem[aa][1] = vem_tmp[1];
//      if(count2 < 10000) printf(" hit---  aa:%d, nevt:%d, nsd:%d, vem_tmp0_np0:%lf, vem_tmp1_np0:%lf \n", aa, i, j, bvem[aa][0], bvem[aa][1] );
////      if(count2 < 10000) printf(" hit---  iflg:%d, jflg:%d \n", iflg[aa], jflg[aa] );
//      if(count2 < 10000) printf(" hit---  npflg:%f, tflg:%f \n", npflg[aa], tflg[aa] );
       aa++;
    }
  }
  else{
    vem_tmp[0] = 0;
    vem_tmp[1] = 0;

//    vem_tmp[0] = avem0[i][j];
//    vem_tmp[1] = avem1[i][j];

//    vem_tmp[0] = avem[i][j][0];
//    vem_tmp[1] = avem[i][j][1];

//    if(i == iflg[aa] && j == jflg[aa]){
    if(buf3[0] == npflg[aa] && time1[m][n] == tflg[aa] && buf3[4] == xflg[aa] && buf3[5] == yflg[aa]){
//    if(vem_tmp[0] > 0 || vem_tmp[1] > 0){
      vem_tmp[0] = bvem[aa][0];
      vem_tmp[1] = bvem[aa][1];
//      if(count2 < 10000) printf(" hit---  aa:%d, nevt:%d, nsd:%d, vem_tmp0_np0:%lf, vem_tmp1_np0:%lf \n", aa, i, j, bvem[aa][0], bvem[aa][1] );
      aa++;
    }
  }

////  if(count2 < 1000) printf( "( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem[13][35][0], (float)avem[13][35][1]);

//  if(count2 < 10000) printf("a vemset    nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f, vem_tmp0:%f, vem_tmp1:%f\n",  i, j, (float)avem0[i][j], (float)avem1[i][j], (float)vem_tmp[0], (float)vem_tmp[1]);
////  if(count2 < 1000) printf("a vemset    nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f, vem_tmp0:%f, vem_tmp1:%f\n",  i, j, (float)avem[i][j][0], (float)avem[i][j][1], (float)vem_tmp[0], (float)vem_tmp[1]);
////  if((np==1 && (int)(buf3[0]/1000) == 1) || (np==2 && (int)(buf3[0]/1000) == 2) || (np==2 && (int)(buf3[0]/1000) == 3)){
  if(np==1 && (int)(buf3[0]/1000) == 1){
////    if(count2 < 1000) printf("b vemcount-----   np:%d, buf0:%f, vem_tmp0_np0:%f, vem_tmp1_np0:%f, vem0:%f, vem1:%f, time1:%f\n", np, buf3[0], (float)avem[i][j][0], (float)avem[i][j][1], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
//    if(count2 < 10000) printf("b vemcount-----   np:%d, buf0:%f, vem_tmp0_np0:%f, vem_tmp1_np0:%f, vem0:%f, vem1:%f, time1:%f\n", np, buf3[0], (float)avem0[i][j], (float)avem1[i][j], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
				  vemcount[m][n][k][0] += 
				    (unsigned short) rintf((float)vem_tmp[0]/VEM*100.);
				  pz[m][n][k] += (unsigned short)
				    rintf(((float)vem_tmp[0]/VEM*100.)/sectheta/2.);
				  vemcount[m][n][k][1] +=
				    (unsigned short) rintf((float)vem_tmp[1]/VEM*100.);
				  pz[m][n][k] += (unsigned short)
				    rintf(((float)vem_tmp[1]/VEM*100.)/sectheta/2.);
    if(count2 < 100000 ){
    if(vemcount[m][n][k][0] > 0 || vemcount[m][n][k][1] > 0 ){
  printf("----------------------------------------------\n");
  printf("np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]);
  printf("----- nevt:%d, nsd:%d, np:%d, m:%d, n:%d, k:%d, buf0:%f, vem_tmp0_np0:%f, vem_tmp1_np0:%f, vem0:%f, vem1:%f, time1:%f\n", i, j, np, m, n, k, buf3[0], (float)avem0[i][j], (float)avem1[i][j], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
  }}
//   if(count2 < 1000) printf("-----   nevt:%d, nsd:%d, np:%d, buf0:%f, vem_tmp0_np0:%f, vem_tmp1_np0:%f, vem0:%f, vem1:%f, time1:%f\n", i, j, np, buf3[0], (float)avem[i][j][0], (float)avem[i][j][1], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
//    if(count2 < 1000) printf("-----   np:%d, buf0:%f, vem0:%f, vem1:%f, time1:%f\n", np, buf3[0], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
  }
  if((np==2 && (int)(buf3[0]/1000) == 2) || (np==2 && (int)(buf3[0]/1000) == 3)){
				  vemcount[m][n][k][0] += 
				    (unsigned short) rintf((float)vem_tmp[0]/VEM*100.);
				  pz[m][n][k] += (unsigned short)
				    rintf(((float)vem_tmp[0]/VEM*100.)/sectheta/2.);
				  vemcount[m][n][k][1] +=
				    (unsigned short) rintf((float)vem_tmp[1]/VEM*100.);
				  pz[m][n][k] += (unsigned short)
				    rintf(((float)vem_tmp[1]/VEM*100.)/sectheta/2.);
    if(count2 < 100000 ){
    if(vemcount[m][n][k][0] > 0 || vemcount[m][n][k][1] > 0 ){
  printf("----------------------------------------------\n");
  printf("np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]);
  printf("----- nevt:%d, nsd:%d, np:%d, m:%d, n:%d, k:%d, buf0:%f, vem_tmp0_np0:%f, vem_tmp1_np0:%f, vem0:%f, vem1:%f, time1:%f\n", i, j, np, m, n, k, buf3[0], (float)avem0[i][j], (float)avem1[i][j], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
//   printf("-----   np:%d, buf0:%f, vem0:%f, vem1:%f, time1:%f\n", np, buf3[0], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
    }}
  }
  if((np==3 && (int)(buf3[0]/1000) == 5) || (np==3 && (int)(buf3[0]/1000) == 6)){
                                  vemcount[m][n][k][0] +=
                                    (unsigned short) rintf((float)vem_tmp[0]/VEM*100.);
                                  pz[m][n][k] += (unsigned short)
                                    rintf(((float)vem_tmp[0]/VEM*100.)/sectheta/2.);
                                  vemcount[m][n][k][1] +=
                                    (unsigned short) rintf((float)vem_tmp[1]/VEM*100.);
                                  pz[m][n][k] += (unsigned short)
                                    rintf(((float)vem_tmp[1]/VEM*100.)/sectheta/2.);
    if(count2 < 100000 ){
    if(vemcount[m][n][k][0] > 0 || vemcount[m][n][k][1] > 0 ){
  printf("----------------------------------------------\n");
  printf("np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]);
  printf("----- nevt:%d, nsd:%d, np:%d, m:%d, n:%d, k:%d, buf0:%f, vem_tmp0_np0:%f, vem_tmp1_np0:%f, vem0:%f, vem1:%f, time1:%f\n", i, j, np, m, n, k, buf3[0], (float)avem0[i][j], (float)avem1[i][j], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
//   printf("-----   np:%d, buf0:%f, vem0:%f, vem1:%f, time1:%f\n", np, buf3[0], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
    }}
  }
  if( (np==4 && (int)(buf3[0]/1000) > 6)){
                                  vemcount[m][n][k][0] +=
                                    (unsigned short) rintf((float)vem_tmp[0]/VEM*100.);
                                  pz[m][n][k] += (unsigned short)
                                    rintf(((float)vem_tmp[0]/VEM*100.)/sectheta/2.);
                                  vemcount[m][n][k][1] +=
                                    (unsigned short) rintf((float)vem_tmp[1]/VEM*100.);
                                  pz[m][n][k] += (unsigned short)
                                    rintf(((float)vem_tmp[1]/VEM*100.)/sectheta/2.);
    if(count2 < 100000 ){
    if(vemcount[m][n][k][0] > 0 || vemcount[m][n][k][1] > 0 ){
  printf("----------------------------------------------\n");
  printf("np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]);
  printf("----- nevt:%d, nsd:%d, np:%d, m:%d, n:%d, k:%d, buf0:%f, vem_tmp0_np0:%f, vem_tmp1_np0:%f, vem0:%f, vem1:%f, time1:%f\n", i, j, np, m, n, k, buf3[0], (float)avem0[i][j], (float)avem1[i][j], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
//   printf("-----   np:%d, buf0:%f, vem0:%f, vem1:%f, time1:%f\n", np, buf3[0], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
    }}
  }
  if( np==0){
                                  vemcount[m][n][k][0] +=
                                    (unsigned short) rintf((float)vem_tmp[0]/VEM*100.);
                                  pz[m][n][k] += (unsigned short)
                                    rintf(((float)vem_tmp[0]/VEM*100.)/sectheta/2.);
                                  vemcount[m][n][k][1] +=
                                    (unsigned short) rintf((float)vem_tmp[1]/VEM*100.);
                                  pz[m][n][k] += (unsigned short)
                                    rintf(((float)vem_tmp[1]/VEM*100.)/sectheta/2.);
    if(count2 < 100000 ){
    if(vemcount[m][n][k][0] > 0 || vemcount[m][n][k][1] > 0 ){
  printf("----------------------------------------------\n");
  printf("np:%d, buf0:%f, buf4:%f, buf5:%f, buf6:%f\n", np, buf3[0], buf3[4], buf3[5], buf3[6]);
  printf("----- nevt:%d, nsd:%d, np:%d, m:%d, n:%d, k:%d, buf0:%f, vem_tmp0_np0:%f, vem_tmp1_np0:%f, vem0:%f, vem1:%f, time1:%f\n", i, j, np, m, n, k, buf3[0], (float)avem0[i][j], (float)avem1[i][j], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
//   printf("-----   np:%d, buf0:%f, vem0:%f, vem1:%f, time1:%f\n", np, buf3[0], (float)vemcount[m][n][k][0], (float)vemcount[m][n][k][1], time1[m][n]);
     }}
  }
/*  else if((np==3 && (int)(buf3[0]/1000) == 5) || (np==3 && (int)(buf3[0]/1000) == 6) || (np==4 && (int)(buf3[0]/1000) > 6)){
                                  vemcount[m][n][k][0] +=
                                    (unsigned short) rintf((float)vem_tmp[0]/VEM*100.);
                                  pz[m][n][k] += (unsigned short)
                                    rintf(((float)vem_tmp[0]/VEM*100.)/sectheta/2.);
                                  vemcount[m][n][k][1] +=
                                    (unsigned short) rintf((float)vem_tmp[1]/VEM*100.);
                                  pz[m][n][k] += (unsigned short)u
                                    rintf(((float)vem_tmp[1]/VEM*100.)/sectheta/2.);
  }*/
				}
			    }

//if(count2 < 10000) printf( "part end   ( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem0[13][35], (float)avem1[13][35]);

			  else if ( buf3[6] >= 
				    time1[m][n]+(float)(NT*DT*(tcount+1)))
			    {

//if(count2 < 10000) printf( "part elseif end   ( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem0[13][35], (float)avem1[13][35]);

			      fwrite(buf3, sizeof(float), NPART, fout);
			      partcount++;
			      if (partcount == NSENTENCE) 
				{
				  partcount = 0;
				  sblockcount++;
				  if ( sblockcount == NSUBBLOCK ) 
				    {
				      sblockcount = 0;
				      fwrite(&blockLen, sizeof(int), 1, fout);
				      fwrite(&blockLen, sizeof(int), 1, fout);
				    }
				}

//if(count2 < 10000) printf( "part elseif end   ( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem0[13][35], (float)avem1[13][35]);

			    }
//  }
////
			}		      
		    }
		}
	    } // for i
	  fread(&blockLen, sizeof(int), 1, f);

//printf( "f loop end   ( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem0[13][35], (float)avem1[13][35]);

	} // fread(f)
//printf( "a f loop, b fclose   ( aa:%d, nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", aa, 13,35, (float)avem0[13][35], (float)avem1[13][35]);

      fclose(f);

//printf( "a fclose, flist loop end  ( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem0[13][35], (float)avem1[13][35]);

      evtcnt++;
    }

//printf( "a flist loop ( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem0[13][35], (float)avem1[13][35]);

  if ( partcount != 0 )
    {
      for ( m = 0; m < NPART; m++)
	buf3[m] = 0.;
      for (m = 0; m < NSENTENCE - partcount; m++)
	fwrite(buf3, sizeof(float), NPART, fout);
      partcount = 0;
      sblockcount++;
      if ( sblockcount == NSUBBLOCK )
	{
	  sblockcount = 0;
	  fwrite(&blockLen, sizeof(int), 1, fout);
	  fwrite(&blockLen, sizeof(int), 1, fout);
	}
    }
  fwrite(evtebuf, sizeof(float), NWORD, fout);
  sblockcount++;
  if (sblockcount == NSUBBLOCK)
    {
      sblockcount = 0;
      fwrite(&blockLen, sizeof(int), 1, fout);
      fwrite(&blockLen, sizeof(int), 1, fout);
    } 
  fwrite(runebuf, sizeof(float), NWORD, fout);
  sblockcount++;
  if ( sblockcount != NSUBBLOCK )
    {
      for ( m = 0; m < NWORD; m++)
	buf[m] = 0.;
      for (m = 0; m < NSUBBLOCK - sblockcount; m++)
	fwrite(buf, sizeof(float), NWORD, fout);
    }
  fwrite(&blockLen, sizeof(int), 1, fout);

//printf( "b fclose flist( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem0[13][35], (float)avem1[13][35]);

  fclose(flist);
  fclose(fout);
  fill_vem();
  fprintf(stderr, "RUNH: %d, EVTH: %d, PARTSUB: %d, LONG: %d, EVTE: %d, RUNE: %d\n",
	  nRUNH, nEVTH, nPARTSUB, nLONG, nEVTE, nRUNE);
////
//  printf( "vem_init_end  ( nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f )\n", 13,35, (float)avem0[13][35], (float)avem1[13][35]);
////
  return count2;
}


