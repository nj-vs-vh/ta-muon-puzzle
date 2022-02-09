/*
 * dstcat.c 
 *
 * Concatenates several DST files into one output DST file
 *
 * $Source: /hires_soft/uvm2k/pro/dstcat.c,v $
 * $Log: dstcat.c,v $
 * Revision 1.2  1996/05/30 18:00:26  mjk
 * Fixed bug - had a ; instead of a , in a variable list
 *
 * Revision 1.1  1996/05/30  00:11:35  mjk
 * Initial revision
 *
 *
*/


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "univ_dst.h"
#include "event.h"
#include "filestack.h"
#define E0 0.891250938134
#define ENCOR 1.27
#define ETHROW -2.0
#define E1 4.42788445031
#define E2 51.2861383991
#define INDX1 3.25
#define INDX2 2.81
#define INDX3 2.81
real8 NORM1, NORM2, E0NEW; 
/* Prototypes */
integer4 cull_file(integer1 *);
real8 prob_spctr(real8 energy);
real8 spctr(real8 energy);

/* Command line option flags */
integer4 flag_o=0;
integer1 *outfile;

integer4 dst_unit_in = 1, dst_unit_out = 2;
integer4 banklist_want, banklist_got;
real8 norm=1., frac=1.;

int main(int argc, char **argv) {

  integer4 rcode, arg, randnum;
  integer4 dst_mode = MODE_WRITE_DST;
  integer1 *filename;
  E0NEW=E0/ENCOR;
  NORM1=pow(E0NEW/E1,INDX1-INDX2);
  NORM2=pow(E0NEW/E2, INDX2-INDX3)*NORM1;
  if (argc == 1 ) {
    fprintf(stderr,"\n  Usage: %s [-o output] [-frac Fraction of events to retain] [-num Number of events desired when frac=1] [-tot Total number of events in file set] dst_file [dst_file...]\n\n", 
	    argv[0]);
    fprintf(stderr,"     -o : output dst filename\n");
    fprintf(stderr,"\n  Concatenates several DST files into one output\n\n");
    exit(1);
  }

  /* Otherwise, scan the arguments first */
  
  for (arg = 1; arg < argc; ++arg) {
    if (argv[arg][0] != '-') 
      pushFile( argv[arg] );

    else {
      switch (argv[arg][1]) {
      case 'o': 
	flag_o = 1; arg++; outfile = argv[arg]; break;
      case 'n':
	arg++; sscanf(argv[arg], "%lf", &norm); 
	break;
      case 'f':
	arg++; sscanf(argv[arg], "%lf", &frac); 
	break;
      case 'r':
	arg++; sscanf(argv[arg], "%i", &randnum); 
	break;
      default: 
	fprintf(stderr,"Warning: unknown option: %s\n",argv[arg]); 
	break;
      }
    }
  }
  fprintf(stderr, "%g %g %6.6d\n", norm, frac, randnum); 
  if ( !flag_o ) {
    fprintf(stderr, "\n  Error: No output file given\n\n");
    exit(1);
  }

  if ( (rcode = dst_open_unit_(&dst_unit_out, outfile, &dst_mode)) ) { 
    fprintf(stderr,"\n  Unable to open/write file: %s\n\n", outfile);
    exit(1);
  }
  srand48(randnum);
  banklist_want = newBankList(512);
  banklist_got  = newBankList(512);
  eventAllBanks(banklist_want);
  
  /* Now process input file(s) */
  
  while ( (filename = pullFile()) )
    cull_file( filename );
  

  /* close DST unit */
  dst_close_unit_(&dst_unit_out);
  return SUCCESS;
}


integer4 cull_file(integer1 *dst_filename) {

  integer4 rcode, ssf, dst_mode = MODE_READ_DST;
  real8 prob01, prob_smallest=1.e20, prob_greatest=0.;
  real8 en_smallest=1.e20, en_greatest=0.;
  if ( (rcode = dst_open_unit_(&dst_unit_in, dst_filename, &dst_mode)) ) {
    fprintf(stderr,"\n  Warning: Unable to open/read file: %s\n", 
	    dst_filename);
    return(-1);
  }

  else {
    fprintf(stdout, "  Culling: %s   \n", dst_filename);

    
    for (;;) {
      rcode = eventRead(dst_unit_in, banklist_want, banklist_got, &ssf);
      if ( rcode < 0 ) break ;
      prob01=prob_spctr((real8)rusdmc_.energy);
      if (prob01 < prob_smallest) 
	{
	  prob_smallest = prob01;
	  en_smallest=rusdmc_.energy;
	}
      if (prob01 > prob_greatest) 
	{
	  prob_greatest = prob01;
	  en_greatest=rusdmc_.energy;
	}
      if (drand48() < prob01)
	if ( eventWrite( dst_unit_out, banklist_got, ssf) < 0) {
	  fprintf(stderr, "  Failed to write event\n");
	  dst_close_unit_(&dst_unit_in);
	  return -2;
	}
    }
      
    dst_close_unit_(&dst_unit_in);
    if ( rcode != END_OF_FILE ) {
      fprintf(stderr,"  Error reading file\n");
      return -3;
    }
    fprintf(stdout, "%g:%g %g:%g\n", en_greatest, prob_greatest, 
	    en_smallest, prob_smallest);
    return rcode;
  }
} 

real8 prob_spctr(real8 energy)
{
  real8 prob01, enew;
  enew=energy/ENCOR;
  prob01=frac*enew*enew/E0NEW/E0NEW/norm;
  prob01=prob01*spctr(enew);
  return prob01;
}
real8 spctr(real8 energy)
{
  real8 prob01;
  if ( energy <= E1 )
    {
      prob01=pow(E0NEW/energy, INDX1);
    }
  else if (energy > E1 && energy< E2 )
    {
      prob01=pow(E0NEW/energy, INDX2)*NORM1;
    }
  else
    {
      prob01=pow(E0NEW/energy, INDX3)*NORM2;
    }
  return prob01;
}
