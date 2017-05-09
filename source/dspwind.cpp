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
  File        : DSPWind.cpp
  Autore      : Sbragion Denis
  Descrizione : Funzioni per vari tipi di finestratura.
  Revisioni   :
  16/10/93    : Prima stesura.
****************************************************************************/

#include "dspwind.h"
#include <stdio.h>

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

void HammingWindow(DLReal * A,unsigned int Size)
  {
    unsigned int I,Half = Size/2;
    DLReal C;

    for(I = 0;I < Half;I++)
      {
        C = (DLReal) (0.54-0.46*DLCos(2*M_PI*I/(Size-1)));
        A[I] *= C;
        A[Size-I-1] *= C;
      }
  }

void HanningWindow(DLReal * A,unsigned int Size)
  {
    unsigned int I,Half = Size/2;
    DLReal C;

    for(I = 0;I < Half;I++)
      {
        C = (DLReal) (0.5*(1-DLCos(2*M_PI*I/(Size-1))));
        A[I] *= C;
        A[Size-I-1] *= C;
      }
  }

void BlackmanWindow(DLReal * A,unsigned int Size)
  {
    unsigned int I,J;
    unsigned int Half = Size/2;
    DLReal C;
		DLReal C1 = (DLReal) (2*M_PI/(Size-1));
		DLReal C2 = (DLReal) (4*M_PI/(Size-1));

    for(I = 0,J = Size - 1;I < Half;I++,J--)
      {
        C = (DLReal) (((DLReal) 0.42)-((DLReal) 0.5)*DLCos(C1*I)+((DLReal) 0.08)*DLCos(C2*I));
        A[I] *= C;
        A[J] *= C;
      }
  }

void SpacedBlackmanWindow(DLReal * A,unsigned int Size, unsigned int Space, WindowType WType)
  {
		unsigned int ESize = Size - Space;
    unsigned int I,Half = (ESize) / 2;
    DLReal C;
		DLReal C1 = 2*M_PI/(ESize-1);
		DLReal C2 = 4*M_PI/(ESize-1);

		switch (WType)
			{
				case WRight:
					for(I = 0;I < Half;I++)
						{
							C = (DLReal) (0.42-0.5*DLCos(C1*I)+0.08*DLCos(C2*I));
							A[Size-I-1] *= C;
						}
				break;

				case WFull:
					for(I = 0;I < Half;I++)
						{
							C = (DLReal) (0.42-0.5*DLCos(C1*I)+0.08*DLCos(C2*I));
							A[I] *= C;
							A[Size-I-1] *= C;
						}
				break;

				case WLeft:
					for(I = 0;I < Half;I++)
						{
							C = (DLReal) (0.42-0.5*DLCos(C1*I)+0.08*DLCos(C2*I));
							A[I] *= C;
						}
				break;
			}
  }

void HalfBlackmanWindow(DLReal * A,unsigned int Size, unsigned int Space, WindowType WType)
  {
		unsigned int ESize = Size - Space;
    unsigned int I;
    DLReal C;
		DLReal C1 = M_PI/(ESize-1);
		DLReal C2 = 2*M_PI/(ESize-1);

		switch (WType)
			{
				case WRight:
					for(I = 0;I < ESize;I++)
						{
							C = (DLReal) (0.42-0.5*DLCos(C1*I)+0.08*DLCos(C2*I));
							A[Size-I-1] *= C;
						}
				break;

				case WFull:
					perror("dspwind.cpp.HalfBlackmanWindow: WFull not supported");
				break;

				case WLeft:
					for(I = 0;I < ESize;I++)
						{
							C = (DLReal) (0.42-0.5*DLCos(C1*I)+0.08*DLCos(C2*I));
							A[I] *= C;
						}
				break;
			}
  }

void BartlettWindow(DLReal * A,unsigned int Size)
  {
    unsigned int I,Half = Size/2;
    DLReal C,R = ((DLReal) Size-1)/2;

    for(I = 0;I < Half;I++)
      {
        C = I/R;
        A[I] *= C;
        A[Size-I-1] *= C;
      }
  }

void BlackmanHarrisWindow(DLReal * A,unsigned int Size)
	{
		unsigned int I,Half = Size/2;
    DLReal C;

    for(I = 0;I < Half;I++)
      {
        C = (DLReal) (0.35875-0.48829*DLCos(2*M_PI*I/(Size-1))+0.14128*DLCos(4*M_PI*I/(Size-1))+0.01168*DLCos(6*M_PI*I/(Size-1)));
        A[I] *= C;
        A[Size-I-1] *= C;
      }
	}

void ExponentialWindow(DLReal * A,unsigned int Size,DLReal P)
  {
    unsigned int I,Half = Size/2;
    DLReal C,X;

    P = (DLReal) (sqrt(P));

    for(I = 0;I < Half;I++)
      {
        X = (P*(Half-I))/Half;
        C = (DLReal) (exp(-X*X));
        A[I] *= C;
        A[Size-I-1] *= C;
      }
  }

void Ones(DLReal * A,unsigned int Size)
  {
    unsigned int I;

    for(I = 0;I < Size;I++)
      A[I] = 1;
  }

/***************************************************************************/
