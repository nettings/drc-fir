/* 

Fast Fourier/Cosine/Sine Transform

Copyright Takuya OOURA, 1996-2001 
(Email: ooura@kurims.kyoto-u.ac.jp or ooura@mmm.t.u-tokyo.ac.jp) 

You may  use, copy,  modify  and  distribute  this code  for any
purpose (include commercial use) and without fee.

http://momonga.t.u-tokyo.ac.jp/~ooura/fft.html

*/

#ifndef fftsg_h
	#define fftsg_h
	
	#include "drc.h"
		
	#ifdef __cplusplus
		extern "C" {
	#endif

	#define OouraForward (-1)
	#define OouraBackward (1)	
	#define OouraRForward (1)
	#define OouraRBackward (-1)
	
	void cdft(int nx2, int dir, DRCFloat * data, int * ip, DRCFloat * w);
  void rdft(int n, int dir, DRCFloat * data, int * ip, DRCFloat * w);
  void ddct(int n, int dir, DRCFloat * data, int * ip, DRCFloat * w);
  void ddst(int n, int dir, DRCFloat * data, int * ip, DRCFloat * w);
  void dfct(int n, DRCFloat *a, DRCFloat *t, int *ip, DRCFloat *w);	
  void dfst(int n, DRCFloat *a, DRCFloat *t, int *ip, DRCFloat *w);
	
	#ifdef __cplusplus
		}
	#endif
#endif
