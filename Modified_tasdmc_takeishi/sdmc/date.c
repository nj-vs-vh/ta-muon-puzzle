#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <libgen.h>
#include <errno.h>
#include "date.h"
#include "sdmc.h"
//Zero point: April 17, 2008 00:00 UT


float Date2TADay( float D, int M, int Y ) 
{
  float F;
  if ( M < 3 ) 
    {
      M += 12;
      Y -= 1;
    }
  F = (float)(-122 + (int)(30.6 *(float)( M + 1 )));
  return D + F + 365.*(float)Y + floorf((float)Y/4.0) - 
    floorf((float)Y /100.0) + floorf((float)Y/400.0) - 733455.;
}

void TADay2Date( float J, int *yymmdd, int *hhmmss)
{
  float Z, R, G, A, B, C, Y, M, D, H, MIN, S, frac;
  Z = floorf(J + 733455.);
  R = J - floorf(J);
  G = Z - 0.25;
  A = floorf( G/36524.25 );
  B = A - floorf(A/4.);
  Y = floorf((B + G)/365.25);
  C = B + Z - floorf(365.25*Y);
  M = floorf((5.*C + 456.)/153.);
  D = C - floorf(30.6*M - 91.4);
  if (M > 12.) 
    {
      Y += 1.;
      M -= 12.;
    }
  H = floorf(24.*R);
  MIN = floorf(1440.*R-60.*H);
  S = floorf(86400.*R-3600.*H-60.*MIN);
  *yymmdd=1e4*(int)(Y-2000.)+1e2*(int)M+(int)D;
  *hhmmss=1e4*(int)H+1e2*(int)MIN+(int)S;
}
