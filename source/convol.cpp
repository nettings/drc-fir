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
  File        : Convol.cpp
  Autore      : Sbragion Denis
  Descrizione : Funzioni per la convoluzione.
  Revisioni   :
  16/10/93    : Prima stesura.
****************************************************************************/

#include "convol.h"
#include "fft.h"

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

void Convolve(const DLReal * A,unsigned int NA,const DLReal * B,
	unsigned int NB,DLReal * R)
  {
    unsigned int I,J,S,E,NR = NA+NB-1;

    for(I = 0;I < NR;I++)
      {
        if (I < NB)
          S = 0;
        else
          S = I-NB+1;

        if (I < NA)
          E = I+1;
        else
          E = NA;

        R[I] = 0;
        for(J = S;J < E;J++)
          R[I] += A[J]*B[I-J];
      }
  }

void Convolve(const DLComplex * A,unsigned int NA,const DLComplex * B,
	unsigned int NB,DLComplex * R)
  {
    unsigned int I,J,S,E,NR = NA+NB-1;

    for(I = 0;I < NR;I++)
      {
        if (I < NB)
          S = 0;
        else
          S = I-NB+1;

        if (I < NA)
          E = I+1;
        else
          E = NA;

        R[I] = 0;
        for(J = S;J < E;J++)
          R[I] += A[J]*B[I-J];
      }
  }

Boolean DFftConvolve(const DLReal * A,unsigned int NA,const DLReal * B,
	unsigned int NB,DLReal * R,DLComplex * CA,DLComplex * CB)
  {
    unsigned int I;
    unsigned int L;
    unsigned int FS;
    DLComplex * FA;
    DLComplex * FB;

    L = NA + NB - 1;

    for(FS = 1;FS < L;FS <<= 1);

    if (CA != NULL)
      FA = CA;
    else
      if ((FA = new DLComplex[FS]) == NULL)
        return(False);
    if (CB != NULL)
      FB = CB;
    else
      if ((FB = new DLComplex[FS]) == NULL)
        {
          if (CA != NULL)
            delete[](FA);
          return(False);
        }

    for(I = 0;I < NA;I++)
      FA[I] = A[I];
    for(I = NA;I < FS;I++)
      FA[I] = 0;

    Radix2Fft(FA,FS);

    for(I = 0;I < NB;I++)
      FB[I] = B[I];
    for(I = NB;I < FS;I++)
      FB[I] = 0;

    Radix2Fft(FB,FS);

    for(I = 0;I < FS;I++)
      FA[I] *= FB[I];

    Radix2IFft(FA,FS);

    for(I = 0;I < L;I++)
      R[I] = FA[I].real();

    if (CA == NULL)
      delete[] FA;
    if (CB == NULL)
      delete[] FB;

    return(True);
  }

Boolean DFftConvolve(const DLComplex * A,unsigned int NA,const DLComplex * B,
	unsigned int NB, DLComplex * R,DLComplex * CA,DLComplex * CB)
  {
    unsigned int I;
    unsigned int L;
    unsigned int FS;
    DLComplex * FA;
    DLComplex * FB;

    L = NA + NB - 1;

    for(FS = 1;FS < L;FS <<= 1);

    if (CA != NULL)
      FA = CA;
    else
      if ((FA = new DLComplex[FS]) == NULL)
        return(False);
    if (CB != NULL)
      FB = CB;
    else
      if ((FB = new DLComplex[FS]) == NULL)
        {
          if (CA != NULL)
            delete[](FA);
          return(False);
        }

    for(I = 0;I < NA;I++)
      FA[I] = A[I];
    for(I = NA;I < FS;I++)
      FA[I] = 0;

    Radix2Fft(FA,FS);

    for(I = 0;I < NB;I++)
      FB[I] = B[I];
    for(I = NB;I < FS;I++)
      FB[I] = 0;

    Radix2Fft(FB,FS);

    for(I = 0;I < FS;I++)
      FA[I] *= FB[I];

    Radix2IFft(FA,FS);

    L = NA + NB - 1;
    for(I = 0;I < L;I++)
      R[I] = FA[I];

    if (CA == NULL)
      delete[] FA;
    if (CB == NULL)
      delete[] FB;

    return(True);
  }

Boolean OAFftConvolve(const DLReal * A,unsigned int NA,const DLReal * B,
	unsigned int NB,DLReal * R,DLComplex * CA,DLComplex * CB)
  {
    unsigned int I,J,INA,INB,FS,L,NR = NA+NB-1;
    const DLReal * IA;
    const DLReal * IB;
    DLComplex * FA;
    DLComplex * FB;
    Boolean FAAlloc;
    Boolean FBAlloc;

    if (NA <= NB)
      {
        INA = NA;
        INB = NB;
        IA = A;
        IB = B;
      }
    else
      {
        INA = NB;
        INB = NA;
        IA = B;
        IB = A;
      }

    for(FS = 1;FS <= INA;FS <<= 1);
		FS *= 2;

		if (CA == NULL)
			{
		    if ((FA = new DLComplex[FS]) == NULL)
					return(False);
				FAAlloc = True;
			}
		else
			{
				FA = CA;
				FAAlloc = False;
			}

		if (CB == NULL)
			{
		    if ((FB = new DLComplex[FS]) == NULL)
		      {
		      	if (FAAlloc == True)
		        	delete[](FA);
		        return(False);
		      }
		    FBAlloc = True;
		  }
		else
			{
				FB = CB;
				FBAlloc = False;
			}

    L = FS-INA+1;

    for(I = 0;I < INA;I++)
      FA[I] = IA[I];

    for(I = INA;I < FS;I++)
      FA[I] = 0;

    Radix2Fft(FA,FS);

    for(I = 0;I < NR;I++)
      R[I] = 0;

    for(I = 0;I < NR;I += L)
      {
        for(J = 0;(J < L) && (I+J < INB);J++)
          FB[J] = IB[I+J];

        for(;J < FS;J++)
          FB[J] = 0;

        Radix2Fft(FB,FS);

        for(J = 0;J < FS;J++)
          FB[J] *= FA[J];

        Radix2IFft(FB,FS);

        for(J = 0;(J < FS) && (I+J < NR);J++)
          R[I+J] += FB[J].real();
      }

		if (FAAlloc == True)
    	delete[] FA;
		if (FBAlloc == True)
    	delete[] FB;

    return(True);
  }

Boolean OAFftConvolve(const DLComplex * A,unsigned int NA,const DLComplex * B,
	unsigned int NB, DLComplex * R,DLComplex * CA,DLComplex * CB)
  {
    unsigned int I,J,INA,INB,FS,L,NR = NA+NB-1;
    const DLComplex * IA;
    const DLComplex * IB;
    DLComplex * FA;
    DLComplex * FB;
    Boolean FAAlloc;
    Boolean FBAlloc;

    if (NA <= NB)
      {
        INA = NA;
        INB = NB;
        IA = A;
        IB = B;
      }
    else
      {
        INA = NB;
        INB = NA;
        IA = B;
        IB = A;
      }

    for(FS = 1;FS <= INA;FS <<= 1);
		FS *= 2;

		if (CA == NULL)
			{
		    if ((FA = new DLComplex[FS]) == NULL)
					return(False);
				FAAlloc = True;
			}
		else
			{
				FA = CA;
				FAAlloc = False;
			}

		if (CB == NULL)
			{
		    if ((FB = new DLComplex[FS]) == NULL)
		      {
		      	if (FAAlloc == True)
		        	delete[](FA);
		        return(False);
		      }
		    FBAlloc = True;
		  }
		else
			{
				FB = CB;
				FBAlloc = False;
			}

    L = FS-INA+1;

    for(I = 0;I < INA;I++)
      FA[I] = IA[I];

    for(I = INA;I < FS;I++)
      FA[I] = 0;

    Radix2Fft(FA,FS);

    for(I = 0;I < NR;I++)
      R[I] = 0;

    for(I = 0;I < NR;I += L)
      {
        for(J = 0;(J < L) && (I+J < INB);J++)
          FB[J] = IB[I+J];

        for(;J < FS;J++)
          FB[J] = 0;

        Radix2Fft(FB,FS);

        for(J = 0;J < FS;J++)
          FB[J] *= FA[J];

        Radix2IFft(FB,FS);

        for(J = 0;(J < FS) && (I+J < NR);J++)
          R[I+J] += FB[J];
      }

		if (FAAlloc == True)
    	delete[] FA;
		if (FBAlloc == True)
    	delete[] FB;

    return(True);
  }

/***************************************************************************/
