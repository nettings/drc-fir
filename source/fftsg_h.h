/* 

Fast Fourier/Cosine/Sine Transform

Copyright Takuya OOURA, 1996-2001 
(Email: ooura@kurims.kyoto-u.ac.jp or ooura@mmm.t.u-tokyo.ac.jp) 

You may  use, copy,  modify  and  distribute  this code  for any
purpose (include commercial use) and without fee.

http://momonga.t.u-tokyo.ac.jp/~ooura/fft.html

*/

#ifndef fftsg_h_h
	#define fftsg_h_h

	#ifdef UseDouble
		/* Tipo floating point usato per le elaborazioni */
		#define DLReal double
	#else
		/* Tipo floating point usato per le elaborazioni */
		#define DLReal float
	#endif
	
	#ifdef __cplusplus
		extern "C" {
	#endif

	#define OouraForward (-1)
	#define OouraBackward (1)
	#define OouraRForward (1)
	#define OouraRBackward (-1)
	
	void cdft(int nx2, int dir, DLReal * data);
  void rdft(int n, int dir, DLReal * data);
  void ddct(int n, int dir, DLReal * data);
  void ddst(int n, int dir, DLReal * data);
  void dfct(int n, DLReal * data);
  void dfst(int n, DLReal * data);
	
	#ifdef __cplusplus
		}
	#endif
#endif
