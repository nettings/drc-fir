/****************************************************************************

    DRC: Digital Room Correction
    Copyright (C) 2002, 2003 Denis Sbragion

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

		You can contact the author on Internet at the following address:

				d.sbragion@infotecna.it

****************************************************************************/

/****************************************************************************
  Progetto    : DSP Library.
  File        : Fft.cpp
  Autore      : Sbragion Denis
  Descrizione : Funzioni per fft, ifft etc.
  Revisioni   :
  16/10/93    : Prima stesura.
****************************************************************************/

#include "fft.h"

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

DLComplex Power(DLComplex X,unsigned int N)
  {
    unsigned int I;
    DLComplex Pow(1);

    for(I = 1;I <= N;I <<= 1)
      {
        if ((I & N) != 0)
          Pow *= X;
        X *= X;
      }

    return(Pow);
  }

DLComplex UnitRoot(unsigned int I,unsigned int N)
  {
    DLReal Arg = (DLReal) (((2*M_PI)*(I%N))/N);
    return(DLComplex((DLReal) DLCos(Arg),(DLReal) DLSin(Arg)));
  }

unsigned int FirstFactor(unsigned int N)
  {
    unsigned int I;

    if (N < 2)
      return(1);
    else
      if (N%2 == 0)
        return(2);
      else
        {
          for(I = 3;I <= N/I && N%I != 0; I += 2);

          if (I > N/I)
            I = N;
        }

    return(I);
  }

/* Controlla se è ablitato l'uso della FFT GSL o di Ooura */
#if !defined(UseGSLFft) && !defined(UseOouraFft)

static unsigned int Radix2Reverse(unsigned int I,unsigned int N)
  {
    unsigned int IR,J,Bit;

    IR = 0;

    for(J = 1,Bit = N >> 1;J < N;J <<= 1,Bit >>= 1)
      if((I & J) != 0)
        IR |= Bit;

    return(IR);
  }

static void Radix2Shuffle(DLComplex P[],unsigned int N)
  {
    unsigned int I,IR;
    DLComplex Temp;

    for(I = 0;I < N;I++)
      {
        IR = Radix2Reverse(I,N);

        if(I < IR)
          {
            Temp = P[I];
            P[I] = P[IR];
            P[IR] = Temp;
          }
      }
  }

static void DLRadix2Fft(DLComplex P[],unsigned int N)
  {
    unsigned int I,J,K,Span,Start,End;
    DLComplex P0,Root;

    for(K = 0,Span = N >> 1;K < Span;K++)
      {
        P0 = P[Span+K];
        P[Span+K] = P[K]-P0;
        P[K] += P0;
      }

    for(I = 2,Span >>= 1;I < N;I <<= 1,Span >>= 1)
      {
        for(J = 0;J < I;J++)
          {
						Root = std::conj(UnitRoot(J,I << 1));

            Start = (Radix2Reverse(J,I) << 1)*Span;
            End = Start+Span;

            for(K = Start;K < End;K++)
              {
                P0 = Root*P[Span+K];
                P[Span+K] = P[K]-P0;
                P[K] += P0;
              }
          }
      }

    Radix2Shuffle(P,N);
  }

static void DLRadix2IFft(DLComplex P[],unsigned int N)
  {
    unsigned int I,J,K,Span,Start,End;
    DLComplex P0,Root;

    for(K = 0,Span = N >> 1;K < Span;K++)
      {
        P0 = P[Span+K];
        P[Span+K] = P[K]-P0;
        P[K] += P0;
      }

    for(I = 2,Span >>= 1;I < N;I <<= 1,Span >>= 1)
      {
        for(J = 0;J < I;J++)
          {
            Root = UnitRoot(J,I << 1);

            Start = (Radix2Reverse(J,I) << 1)*Span;
            End = Start+Span;

            for(K = Start;K < End;K++)
              {
                P0 = Root*P[Span+K];
                P[Span+K] = P[K]-P0;
                P[K] += P0;
              }
          }
      }

    Radix2Shuffle(P,N);

    for(I = 0;I < N;I++)
      P[I] /= (DLReal) N;
  }

#endif

/* Verifica se è abiliato l'uso della GSL */
#ifndef UseGSLFft

static unsigned int MixedReverse(unsigned int I,unsigned int N)
  {
    unsigned int IR,J,Digit,Radix;

    if (N < 2)
      return(I);
    else
      {
        IR = 0;

        Radix = FirstFactor(N);

        for(J = 1,Digit = N/Radix;J < N;Digit /= Radix)
          {
            IR += ((I/J)%Radix)*Digit;

            J *= Radix;
            Radix = FirstFactor(Digit);
          }
      }

    return(IR);
  }

static void MixedShuffle(DLComplex P[],unsigned int N)
  {
    unsigned int I,IR,J;
    DLComplex Temp;

    for(I = 1;I < N-1;I++)
      {
        IR = MixedReverse(I,N);
        while(I > IR)
          IR = MixedReverse(IR,N);

        if (I == IR)
          {
            J = I;
            IR = MixedReverse(I,N);
            Temp = P[J];
            while(I != IR)
              {
                P[J] = P[IR];
                J = IR;
                IR = MixedReverse(IR,N);
              }
            P[J] = Temp;
          }
      }
  }

static Boolean DLFft(DLComplex P[],unsigned int N)
  {
    unsigned int I,J,K,L,Span,Radix,Start,End,RP;
    DLComplex * PT;
    DLComplex Root,P0;

    Radix = FirstFactor(N);

    for(I = 1,Span = N/Radix;I < N;Span /= Radix)
      {
        RP = I*Radix;

        if (Radix == 2)
          if (I == 1)
            for(K = 0;K < Span;K++)
              {
                P0 = P[Span+K];
                P[Span+K] = P[K]-P0;
                P[K] += P0;
              }
          else
            for(J = 0;J < I;J++)
              {
                Start = MixedReverse(J,I)*Radix*Span;
                End = Start+Span;
                Root = std::conj(UnitRoot(J,RP));

                for(K = Start;K < End;K++)
                  {
                    P0 = Root*P[Span+K];
                    P[Span+K] = P[K]-P0;
                    P[K] += P0;
                  }
              }
        else
          if ((PT = new(DLComplex[Radix])) != NULL)
            {
              for(J = 0;J < I;J++)
                {
                  Start = MixedReverse(J,I)*Radix*Span;
                  End = Start+Span;

                  for(K = Start;K < End;K++)
                    {
                      for (L = 0;L < Radix;L++)
                        PT[L] = P[Span*L+K];

                      for (L = 0;L < Radix;L++)
                        P[Span*L+K] = PolyEval(PT,Radix,std::conj(UnitRoot(L*I+J,RP)));
                    }
                }

              delete[](PT);
            }
          else
            return(False);

        I *= Radix;
        Radix = FirstFactor(Span);
      }

    MixedShuffle(P,N);

    return(True);
  }

static Boolean DLIFft(DLComplex P[],unsigned int N)
  {
    unsigned int I,J,K,L,Span,Radix,Start,End,RP;
    DLComplex * PT;
    DLComplex Root,P0;

    Radix = FirstFactor(N);

    for(I = 1,Span = N/Radix;I < N;Span /= Radix)
      {
        RP = I*Radix;

        if (Radix == 2)
          if (I == 1)
            for(K = 0;K < Span;K++)
              {
                P0 = P[Span+K];
                P[Span+K] = P[K]-P0;
                P[K] += P0;
              }
          else
            for(J = 0;J < I;J++)
              {
                Start = MixedReverse(J,I)*Radix*Span;
                End = Start+Span;
                Root = UnitRoot(J,RP);

                for(K = Start;K < End;K++)
                  {
                    P0 = Root*P[Span+K];
                    P[Span+K] = P[K]-P0;
                    P[K] += P0;
                  }
              }
        else
          if ((PT = new(DLComplex[Radix])) != NULL)
            {
              for(J = 0;J < I;J++)
                {
                  Start = MixedReverse(J,I)*Radix*Span;
                  End = Start+Span;

                  for(K = Start;K < End;K++)
                    {
                      for (L = 0;L < Radix;L++)
                        PT[L] = P[Span*L+K];

                      for (L = 0;L < Radix;L++)
                        P[Span*L+K] = PolyEval(PT,Radix,
                          UnitRoot(L*I+J,RP));
                    }
                }

              delete[](PT);
            }
          else
            return(False);

        I *= Radix;
        Radix = FirstFactor(Span);
      }

    MixedShuffle(P,N);

    for(I = 0;I < N;I++)
      P[I] /= (DLReal) N;

    return(True);
  }

#else

#include <stdlib.h>

#endif

/* Controlla se è ablitato l'uso della FFT di Ooura */
#ifdef UseOouraFft

#include "fftsg.h"
#include <math.h>

static unsigned int OTSize = 0;
static int * OWATable = NULL;
static DRCFloat * OCSTable = NULL;

static void FreeWAAndCS(void)
	{
		if (OWATable != NULL)
			{
				free(OWATable);
				OWATable = NULL;
			}

		if (OCSTable != NULL)
			{
				free(OCSTable);
				OCSTable = NULL;
				OTSize = 0;
			}
	}

static Boolean CheckWAAndCS(unsigned int N)
	{
		if (OWATable == NULL)
			{
				atexit(FreeWAAndCS);

				OWATable = (int *) malloc(sizeof(int) * (int) ceil(2 + sqrt(N)));
				if (OWATable == NULL)
					return(False);
				OWATable[0] = 0;

				OCSTable = (DRCFloat *) malloc(sizeof(DRCFloat) * N / 2);
				if (OCSTable == NULL)
					{
						FreeWAAndCS();
						return(False);
					}
			}
		else
			if (N / 2 > OTSize)
				{
					FreeWAAndCS();

					OWATable = (int *) malloc(sizeof(int) * (int) ceil(2 + sqrt(N)));
					if (OWATable == NULL)
						return(False);
					OWATable[0] = 0;

					OCSTable = (DRCFloat *) malloc(sizeof(DRCFloat) * N / 2);
					if (OCSTable == NULL)
						{
							FreeWAAndCS();
							return(False);
						}
				}

		return(True);
	}

#endif

/* Controlla se è ablitato l'uso della FFT GSL */
#ifdef UseGSLFft

#ifdef UseDouble
	#include "gsl/gsl_fft_complex.h"

	#define FftWTAlloc gsl_fft_complex_wavetable_alloc
	#define FftWTFree gsl_fft_complex_wavetable_free
	#define FftWSAlloc gsl_fft_complex_workspace_alloc
	#define FftWSFree gsl_fft_complex_workspace_free
	#define FftForward gsl_fft_complex_forward
	#define FftInverse gsl_fft_complex_inverse
	#define FftArrayType gsl_complex_packed_array
	#define FftWTType gsl_fft_complex_wavetable
	#define FftWSType gsl_fft_complex_workspace
#else
	#include "gsl/gsl_fft_complex_float.h"

	#define FftWTAlloc gsl_fft_complex_wavetable_float_alloc
	#define FftWTFree gsl_fft_complex_wavetable_float_free
	#define FftWSAlloc gsl_fft_complex_workspace_float_alloc
	#define FftWSFree gsl_fft_complex_workspace_float_free
	#define FftForward gsl_fft_complex_float_forward
	#define FftInverse gsl_fft_complex_float_inverse
	#define FftArrayType gsl_complex_packed_array_float
	#define FftWTType gsl_fft_complex_wavetable_float
	#define FftWSType gsl_fft_complex_workspace_float
#endif

static FftWTType * FftWT = NULL;
static FftWSType * FftWS = NULL;

static void FreeWTAndWS(void)
	{
		if (FftWT != NULL)
			{
				FftWTFree(FftWT);
				FftWT = NULL;
			}

		if (FftWS != NULL)
			{
				FftWSFree(FftWS);
				FftWS = NULL;
			}
	}

static Boolean CheckWTAndWS(unsigned int N)
	{
		if (FftWT == NULL)
			{
				atexit(FreeWTAndWS);

				FftWT = FftWTAlloc(N);
				if (FftWT == NULL)
					return(False);

				FftWS = FftWSAlloc(N);
				if (FftWS == NULL)
					{
						FreeWTAndWS();
						return(False);
					}
			}
		else
			if (FftWT->n != N)
				{
					FreeWTAndWS();

					FftWT = FftWTAlloc(N);
					if (FftWT == NULL)
						return(False);

					FftWS = FftWSAlloc(N);
					if (FftWS == NULL)
						{
							FreeWTAndWS();
							return(False);
						}
				}

		return(True);
	}

#endif

void Radix2Fft(DLComplex P[],unsigned int N)
  {
		#ifdef UseOouraFft
			if (CheckWAAndCS(N) == False)
				return;

			cdft(2 * N,OouraForward,(DRCFloat *) P,OWATable,OCSTable);
		#else
			#ifdef UseGSLFft
				if (CheckWTAndWS(N) == False)
					return;

				FftForward((FftArrayType) P,1,N,FftWT,FftWS);
			#else
				DLRadix2Fft(P,N);
			#endif
		#endif
	}

void Radix2IFft(DLComplex P[],unsigned int N)
  {
		#ifdef UseOouraFft
			unsigned int I;

			if (CheckWAAndCS(N) == False)
				return;

			cdft(2 * N,OouraBackward,(DRCFloat *) P,OWATable,OCSTable);

			for (I = 0;I < N;I++)
				P[I] /= (DRCFloat) N;
		#else
			#ifdef UseGSLFft
				if (CheckWTAndWS(N) == False)
					return;
				FftInverse((FftArrayType) P,1,N,FftWT,FftWS);
			#else
				DLRadix2IFft(P,N);
			#endif
		#endif
	}

Boolean Fft(DLComplex P[],unsigned int N)
  {
		/* Controlla se la lunghezza è una potenza di 2 */
		if ((N & (N - 1)) > 0)
			{
				#ifdef UseGSLFft
					if (CheckWTAndWS(N) == False)
						return(False);

					if (FftForward((FftArrayType) P,1,N,FftWT,FftWS) != 0)
						return(False);
				#else
					return(DLFft(P,N));
				#endif
			}
		else
			{
				#ifdef UseOouraFft
					if (CheckWAAndCS(N) == False)
						return(False);

					cdft(2 * N,OouraForward,(DRCFloat *) P,OWATable,OCSTable);
				#else
					#ifdef UseGSLFft
						if (CheckWTAndWS(N) == False)
							return(False);

						if (FftForward((FftArrayType) P,1,N,FftWT,FftWS) != 0)
							return(False);
					#else
						DLRadix2Fft(P,N);
					#endif
				#endif
			}

		return(True);
	}

Boolean IFft(DLComplex P[],unsigned int N)
  {
		/* Controlla se la lunghezza è una potenza di 2 */
		if ((N & (N - 1)) > 0)
			{
				#ifdef UseGSLFft
					if (CheckWTAndWS(N) == False)
						return(False);

					if (FftInverse((FftArrayType) P,1,N,FftWT,FftWS) != 0)
						return(False);
				#else
					return(DLIFft(P,N));
				#endif
			}
		else
			{
				#ifdef UseOouraFft
					unsigned int I;

					if (CheckWAAndCS(N) == False)
						return(False);

					cdft(2 * N,OouraBackward,(DRCFloat *) P,OWATable,OCSTable);

					for (I = 0;I < N;I++)
						P[I] /= (DRCFloat) N;
				#else
					#ifdef UseGSLFft
						if (CheckWTAndWS(N) == False)
							return(False);

						if (FftInverse((FftArrayType) P,1,N,FftWT,FftWS) != 0)
							return(False);
					#else
						DLRadix2IFft(P,N);
					#endif
				#endif
			}

		return(True);
	}

DLComplex PolyEval(DLComplex P[],unsigned int N,DLComplex X)
  {
    DLComplex Y(0);
    unsigned int I;

    if (N > 0)
      {
        Y = P[N-1];
        for(I = N-1;I > 0;I--)
          Y = X*Y+P[I-1];
      }

    return(Y);
  }

Boolean Dft(DLComplex P[],unsigned int N)
  {
    unsigned int I;
    DLComplex * R;

    if ((R = new DLComplex[N]) != NULL)
      {
        for(I = 0;I < N;I++)
          R[I] = PolyEval(P,N,std::conj(UnitRoot(I,N)));

        for(I = 0;I < N;I++)
          P[I] = R[I];

        delete[](R);

        return(True);
      }
    else
      return(False);
  }

Boolean IDft(DLComplex P[],unsigned int N)
  {
    unsigned int I;
    DLComplex * R;

    if ((R = new DLComplex[N]) != NULL)
      {
        for(I = 0;I < N;I++)
          R[I] = PolyEval(P,N,UnitRoot(I,N));

        for(I = 0;I < N;I++)
          P[I] = R[I]/((DLReal) N);

        delete[](R);

        return(True);
      }
    else
      return(False);
  }

/***************************************************************************/
