/* readCorsika.c
 *
 * reads CORSIKA binary DAT files.
 *
 * It only dumps the few blocks that I am interested in, but can easily be
 * modified to do more. Assumptions about block sizes have been made on
 * how we ran CORSIKA here (e.g., THIN option is turned on). Written for
 * CORSIKA version 6.003.
 *
 * whanlon@cosmic.utah.edu
 * 29 jul 2006
 *
 * gcc -o readCorsika readCorsika.c -lm
 *
 */

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
////
//  double avem[NSUBBLOCK][40][2];
////

int main(int argc, char **argv)
{
  FILE *f, *f2;
  char outFile[256];
  char tmpFile[1000][256], geaFile[256];
  unsigned short buf[6];
  short gowrite = 0;
  //  int i, j, m, n, dist,tcount, np, flg1[NX][NY], flg2[NT], flg3[1000][NX][NY], flg4[1000][NT];
  //  int i, j, m, n, dist,tcount, np, flg1[(int)ceilf((float)TMAX/(float)NT)][NX][NY][NT];
  //  float b2[NX][NY][NT][2], b3[NX][NY][NT], b4[NX][NY][NT], b5[NX][NY][NT];
  int i, j, m, n, dist, tcount, np;
  long int atime;
  const int npmax = 6;
  double count, count2;
  srand48(314159265);
  tcount = 0;
  count = count2 = 0.;
  if (argv[2] != NULL)
    sscanf(argv[2], "%f", &emin);
  else
    emin = 0.;
  printf("Energy threshold: %f keV\n", emin);
  fflush(stdout);
  strcpy(tmpFile[0], argv[1]);
  strcpy(geaFile, argv[2]);
  if (load_elosses(geaFile) == -1)
  {
    fprintf(stderr, "Cannot open %s file\n", geaFile);
    return -1;
  }
  ////
  printf("aaa\naaa\n");
  for (np = 0; np < npmax; np++)
  {
    srand48(314159265);
    printf("aa\naa\nnp:%d\n", np);
    aa = 0;
    //printf("vem_1021:%lf %lf\n", avem[13][35][0], avem[13][35][1]);
    printf("vem_1021:%lf %lf\n", avem0[13][35], avem1[13][35]);
    tcount = 0;
    count = count2 = 0.;
    j = 0;
    ////
    /*    for (m = 0; m < 100; m++){
      for ( n = 0; n < 100; n++){
        if(time1[m][n] != 1.e9 ){
    printf("-----file0-----   np:%d, m:%d, n:%d, vem0:%f, vem1:%f, time1:%f\n", np, (float)vemcount[m][n][40][0], (float)vemcount[m][n][40][1], time1[m][n]);
    }}}
*/
    printf("time:%d, %f\n", time(NULL), time(NULL));
    count = corsika_times(argv[1], np);
    sprintf(outFile, "%s_%d_gea.dat", argv[1], np);
    f2 = fopen(outFile, "w");
    fwrite(eventbuf, sizeof(float), NWORD, f2);
    printf("%g:%g\t %g Percent\n", count2, count, count2 / count * 100.);
    fflush(stdout);
    sprintf(tmpFile[1], "tmp%3.3d", 1);
    /*
    for (m = 0; m < 100; m++){
      for ( n = 0; n < 100; n++){
        if(time1[m][n] != 1.e9 ){
    printf("-----file2-----   np:%d, m:%d, n:%d, vem0:%f, vem1:%f, time1:%f\n", np, (float)vemcount[m][n][40][0], (float)vemcount[m][n][40][1], time1[m][n]);
    }}}
*/
    //printf( "a times nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f \n", 13,35, (float)avem[13][35][0], (float)avem[13][35][1]);
    printf("a times nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f \n", 13, 35, (float)avem0[13][35], (float)avem1[13][35]);
    for (m = 0; m < NX; m++)
    {
      for (n = 0; n < NY; n++)
      {
        if (np == 0)
          atime1[m][n] = time1[m][n];
        else
          time1[m][n] = atime1[m][n];
      }
    }

    count2 += corsika_vem_init(argv[1], tmpFile[1], tcount, np, avem0, avem1);
    //printf( "a vem_init nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f \n", 13,35, (float)avem[13][35][0], (float)avem[13][35][1]);
    printf("a vem_init nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f \n", 13, 35, (float)avem0[13][35], (float)avem1[13][35]);

    printf("%g:%g\t %g Percent\n", count2, count, count2 / count * 100.);
    fflush(stdout);
    /*
    for (m = 0; m < 100; m++){
      for ( n = 0; n < 100; n++){
        if(time1[m][n] != 1.e9 ){
    printf("-----file2-----   np:%d, m:%d, n:%d, vem0:%f, vem1:%f, time1:%f\n", np, (float)vemcount[m][n][40][0], (float)vemcount[m][n][40][1], time1[m][n]);
    }}}
*/
    //printf( "a vem_init nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f \n", 13,35, (float)avem[13][35][0], (float)avem[13][35][1]);
    printf("a vem_init nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f \n", 13, 35, (float)avem0[13][35], (float)avem1[13][35]);

    for (m = 0; m < NX; m++)
    {
      for (n = 0; n < NY; n++)
      {
        //if(np==0){
        if (time1[m][n] != 1.e9)
        {
          buf[0] = (unsigned short)m;
          buf[1] = (unsigned short)n;
          for (i = 0; i < NT; i++)
          {
            if (vemcount[m][n][i][0] > 0. ||
                vemcount[m][n][i][1] > 0.)
            {
              buf[2] = vemcount[m][n][i][0];
              buf[3] = vemcount[m][n][i][1];
              buf[4] = (unsigned short)((time1[m][n] +
                                         (float)(tcount * DT * NT) +
                                         (float)(i * DT) - tmin) /
                                        DT);
              buf[5] = pz[m][n][i];
              /*
if(np==0){
  b2[m][n][i][0] = vemcount[m][n][i][0];
  b2[m][n][i][1] = vemcount[m][n][i][1];
  b4[m][n][i] = (float)buf[4];
  b5[m][n][i] = pz[m][n][i];
}
else{
  if(vemcount[m][n][i][0] != b2[m][n][i][0] ) printf("--- error vem0! m:%d, n:%d, i:%d, vem0:%f, vem0_np0:%f --- \n", m, n, i, vemcount[m][n][i][0], b2[m][n][i][0]);
  if( vemcount[m][n][i][1] != b2[m][n][i][1] ) printf("--- error vem0! m:%d, n:%d, i:%d, vem1:%f, vem1_np0:%f --- \n", m, n, i, vemcount[m][n][i][1], b2[m][n][i][1]);
  if( buf[4] != b4[m][n][i] ) printf("--- error vem0! m:%d, n:%d, i:%d, time:%f, time_np0:%f --- \n", m, n, i, (float)buf[4], b4[m][n][i]);
  if(pz[m][n][i] != b5[m][n][i] ) printf("--- error vem0! m:%d, n:%d, i:%d, pz:%f, pz_np0:%f --- \n", m, n, i, pz[m][n][i],  b5[m][n][i]);
}
*/
              if (buf[5] == 0 || 2 * buf[5] > buf[2] + buf[3])
                buf[5] = (unsigned short)(cosf(zenith) *
                                          (float)(buf[2] + buf[3]) / 2.);
              fwrite(buf, sizeof(short), 6, f2);
              //  flg1[j][m][n][i]=1;
              //printf("np:%d, buf0:%d, buf1:%d, buf2:%f, buf4:%d\n", np, (int)buf[0], (int)buf[1], (float)buf[2], (int)buf[4]);
            }
          }
        }
        //}
        /*else{
//          if ( flg1[m][n]==1 )
//            {
              buf[0] = (unsigned short) m;
              buf[1] = (unsigned short) n;
              for ( i = 0; i < NT; i++ )
                {
                  if ( flg1[j][m][n][i]==1 )
                    {
                      buf[2] = vemcount[m][n][i][0];
                      buf[3] = vemcount[m][n][i][1];
                      buf[4] = (unsigned short) ((time1[m][n] +
                                                  (float)(tcount*DT*NT) +
                                                  (float)(i*DT) - tmin)/DT);
                      buf[5] = pz[m][n][i];
//                      if ( buf[5] == 0 || 2*buf[5] > buf[2]+buf[3])
                        buf[5] = (unsigned short)(cosf(zenith)*
                                                  (float)(buf[2]+buf[3])/2.);
                      fwrite(buf, sizeof(short), 6, f2);
printf("np:%d, buf0:%d, buf1:%d, buf2:%f, buf4:%d\n", np, (int)buf[0], (int)buf[1], (float)buf[2], (int)buf[4]);
                    }
                }
//            }
}*/
      }
    }
    //printf( "a vem_init_fill nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f \n", 13,35, (float)avem[13][35][0], (float)avem[13][35][1]);
    printf("a vem_init_fill nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f \n", 13, 35, (float)avem0[13][35], (float)avem1[13][35]);

    tcount++;
    for (j = 1; j < (int)ceilf((float)TMAX / (float)NT); j++)
    {
      sprintf(tmpFile[j + 1], "tmp%3.3d", j + 1);
      count2 += corsika_vem(tmpFile[j], tmpFile[j + 1], tcount, np, avem0, avem1);
      printf("%g:%g\t %g Percent\n", count2, count, count2 / count * 100.);
      fflush(stdout);
      for (m = 0; m < NX; m++)
      {
        for (n = 0; n < NY; n++)
        {
          /*if(np==0){
  for ( i = 0; i < NT; i++ ){
    flg1[j][m][n][i]=0;
  }
*/
          if (time1[m][n] != 1.e9)
          {
            buf[0] = (unsigned short)m;
            buf[1] = (unsigned short)n;
            for (i = 0; i < NT; i++)
            {
              if (vemcount[m][n][i][0] > 0. ||
                  vemcount[m][n][i][1] > 0.)
              {
                buf[2] = vemcount[m][n][i][0];
                buf[3] = vemcount[m][n][i][1];
                buf[4] = (unsigned short)((time1[m][n] +
                                           (float)(tcount * DT * NT) +
                                           (float)(i * DT) - tmin) /
                                          DT);
                buf[5] = pz[m][n][i];
                if (buf[5] == 0 || 2 * buf[5] > buf[2] + buf[3])
                  buf[5] = (unsigned short)(cosf(zenith) *
                                            (float)(buf[2] + buf[3]) / 2.);
                fwrite(buf, sizeof(short), 6, f2);
                // flg1[j][m][n][i]=1;
              }
            }
          }
          /*}
else{
//              if ( flg1[m][n][i]==1 )
//                {
                  buf[0] = (unsigned short) m;
                  buf[1] = (unsigned short) n;
                  for ( i = 0; i < NT; i++ )
                    {
                      if ( flg1[j][m][n][i]==1 )
                        {
                          buf[2] = vemcount[m][n][i][0];
                          buf[3] = vemcount[m][n][i][1];
                          buf[4] = (unsigned short) ((time1[m][n] +
                                                     (float)(tcount*DT*NT) +
                                                      (float)(i*DT) - tmin)/DT);
                          buf[5] = pz[m][n][i];
//                          if ( buf[5] == 0 || 2*buf[5] > buf[2]+buf[3])
                            buf[5] = (unsigned short)(cosf(zenith)*
                                                      (float)(buf[2]+buf[3])/2.);
                          fwrite(buf, sizeof(short), 6, f2);
                        }
                    }
//                }
}*/
        }
      }
      tcount++;
    }
    //printf( "a vem_fill nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f \n", 13,35, (float)avem[13][35][0], (float)avem[13][35][1]);
    printf("a vem_fill nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f \n", 13, 35, (float)avem0[13][35], (float)avem1[13][35]);

    fclose(f2);

    //printf( "a fclose(f2) nevt:%d, nsd:%d, vem_tmp0_np0:%f, vem_tmp1_np0:%f \n", 13,35, (float)avem[13][35][0], (float)avem[13][35][1]);
  } // np
  exit(EXIT_SUCCESS);
}
