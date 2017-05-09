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
  File        : Fir.cpp
  Autore      : Sbragion Denis
  Descrizione : Funzioni per il calcolo di filtri fir.
  Revisioni   :
  16/10/93    : Prima stesura.
****************************************************************************/

#include "fir.h"
#include "fft.h"
#include "spline.h"
#include <stddef.h>

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

void LowPassFir(DLReal * Filter,unsigned int Order,DLReal Freq)
  {
    unsigned int I,J,K;
    unsigned int HalfOrder = Order/2;
    DLReal C = (DLReal) (M_PI*Freq);
    DLReal FV;

    if (Order > 1)
			{
				if (Order%2 == 0)
					for(I = 1,J = HalfOrder - 1,K = HalfOrder; I <= HalfOrder;I++,J--,K++)
						{
							FV = (DLReal) (I-((DLReal) 0.5));
							FV = (DLReal) (DLSin(FV*C)/(FV*((DLReal) M_PI)));
							Filter[J] = FV;
							Filter[K] = FV;
						}
				else
					{
						for(I = 1,J = HalfOrder - 1,K = HalfOrder + 1; I <= HalfOrder;I++,J--,K++)
							{
								FV = (DLReal) (DLSin(I*C)/(I*((DLReal) M_PI)));
								Filter[J] = FV;
								Filter[K] = FV;
							}
						Filter[HalfOrder] = Freq;
					}
			}
  }

void HighPassFir(DLReal * Filter,unsigned int Order,DLReal Freq)
  {
    unsigned int I,HalfOrder;
    DLReal C = (DLReal) (M_PI*Freq);

    if (Order > 2)
      {
        if (Order%2 == 0)
          {
            Order--;
            Filter[Order] = 0;
          }

        HalfOrder = Order/2;

        for(I = 1; I <= HalfOrder;I++)
          {
            Filter[HalfOrder-I] = (DLReal) (-DLSin(I*C)/(I*M_PI));
            Filter[HalfOrder+I] = Filter[HalfOrder-I];
          }
        Filter[HalfOrder] = 1-Freq;
      }
  }

void BandPassFir(DLReal * Filter,unsigned int Order,DLReal Low,DLReal High)
  {
    unsigned int I,HalfOrder = Order/2;
    DLReal D = (DLReal) (M_PI*(High-Low)/2), S = (DLReal) (M_PI*(High+Low)/2);

    if (Order > 1)
			{
				if (Order%2 == 0)
					{
						for(I = 1;I <= HalfOrder;I++)
							{
								Filter[HalfOrder-I] = (DLReal) ((2 / M_PI)*DLSin((I-0.5)*D)*
									DLCos((I-0.5)*S)/(I-0.5));
								Filter[HalfOrder+I-1] = Filter[HalfOrder-I];
							}
					}
				else
					{
						for(I = 1;I <= HalfOrder;I++)
							{
								Filter[HalfOrder-I] = (DLReal) ((2 / M_PI)*DLSin(I*D)*
									DLCos(I*S)/I);
								Filter[HalfOrder+I] = Filter[HalfOrder-I];
							}
						Filter[HalfOrder] = (High-Low);
					}
			}
  }

void BandStopFir(DLReal * Filter,unsigned int Order,DLReal Low,DLReal High)
  {
    unsigned int I,HalfOrder;
    DLReal D = (DLReal) (M_PI*(High-Low)/2),S = (DLReal) (M_PI*(High+Low)/2);

    if (Order > 2)
      {
        if (Order%2 == 0)
          {
            Order--;
            Filter[Order] = 0;
          }

        HalfOrder = Order/2;

        for(I = 1; I <= HalfOrder;I++)
          {
            Filter[HalfOrder-I] = (DLReal) (-M_2PI*DLSin(I*D)*
              DLCos(I*S)/I);
            Filter[HalfOrder+I] = Filter[HalfOrder-I];
          }
        Filter[HalfOrder] = 1-(High-Low);
      }
  }

void HilbertFir(DLReal * Filter,unsigned int Order)
  {
    unsigned int I,HalfOrder;

    if (Order > 2)
      {
        if (Order%2 == 0)
          {
            Order--;
            Filter[Order] = 0;
          }

        HalfOrder = Order/2;

        for(I = 1;I <= HalfOrder;I++)
          {
            Filter[HalfOrder+I] = (DLReal) (M_2PI*(I%2)/I);
            Filter[HalfOrder-I] = -Filter[HalfOrder+I];
          }

        Filter[HalfOrder] = 0;
      }
  }

Boolean GenericFir(DLReal * Filter,unsigned int Order,DLReal * F,
  DLReal * M, DLReal * P,unsigned int Np,unsigned int Is,
	InterpolationType It)
  {
    DLComplex * Ht;
    DLReal * Hf;
    unsigned int I,HalfFilter,HtSize,HtSize2;
    DLReal * LF;
    DLReal * LM;
    DLReal * LP;

    if ((Order < 2) || (Np < 2))
      return(False);

    if ((F[0] != 0) || (F[Np-1] != 1))
      return(False);

		for(I = 0;I < Np-1;I++)
			if (F[I] > F[I+1])
				return(False);

    if (Is == 0)
      for (HtSize = 2;HtSize < Order;HtSize <<= 1);
    else
      HtSize = Is;

    if (HtSize < Order)
      return(False);

    HtSize2 = 2*HtSize;

    if ((Ht = new DLComplex[HtSize2]) == NULL)
      return(False);

		if ((Hf = new DLReal[HtSize]) == NULL)
			{
				delete[](Ht);
				return(False);
			}

		for(I = 0;I < HtSize;I++)
			Hf[I] = I / (HtSize - (DLReal) 1.0);

		switch (It)
			{
				case Linear:
					CSLCASplineValue(F,M,P,Np,Hf,HtSize,Ht,CSLLinear,CSLLinXLinY);
				break;

				case Logarithmic:
					/* Alloca gli array temporanei rimappatura logaritmica */
					if ((LF = new DLReal[Np]) == NULL)
						{
							delete[](Ht);
							delete[](Hf);
							return(False);
						}
					if ((LM = new DLReal[Np]) == NULL)
						{
							delete[](LF);
							delete[](Ht);
							delete[](Hf);
							return(False);
						}
					if ((LP = new DLReal[Np]) == NULL)
						{
							delete[](LM);
							delete[](LF);
							delete[](Ht);
							delete[](Hf);
							return(False);
						}

					/* Rimappa i valori per evitare l'interpolazione a frequenza 0.
					Il primo elemento viene interpolato linearmente. */
					LF[0] = L1SplineValue(F,F,Np,Hf[1]);
					LM[0] = L1SplineValue(F,M,Np,Hf[1]);
					LP[0] = L1SplineValue(F,P,Np,Hf[1]);

					for(I = 1;I < Np;I++)
						{
							LF[I] = F[I];
							LM[I] = M[I];
							LP[I] = P[I];
						}

					Ht[0] = std::polar<DLReal>(M[0],P[0]);

					if (CSLCASplineValue(LF,LM,LP,Np,&Hf[1],HtSize-1,&Ht[1],CSLLinear,CSLLogXLogY) == False)
						{
							delete[](LP);
							delete[](LM);
							delete[](LF);
							delete[](Ht);
							delete[](Hf);
							return(False);
						}

					/* Dealloca gli array temporanei rimappatura logaritmica */
					delete[](LP);
					delete[](LM);
					delete[](LF);
				break;

				case SplineLinear:
					if (CSLCASplineValue(F,M,P,Np,Hf,HtSize,Ht,CSLBSpline,CSLLinXLinY) == False)
						{
							delete[](Ht);
							delete[](Hf);
							return(False);
						}
				break;

				case SplineLogarithmic:
					/* Alloca gli array temporanei rimappatura logaritmica */
					if ((LF = new DLReal[Np]) == NULL)
						{
							delete[](Ht);
							delete[](Hf);
							return(False);
						}
					if ((LM = new DLReal[Np]) == NULL)
						{
							delete[](LF);
							delete[](Ht);
							delete[](Hf);
							return(False);
						}
					if ((LP = new DLReal[Np]) == NULL)
						{
							delete[](LM);
							delete[](LF);
							delete[](Ht);
							delete[](Hf);
							return(False);
						}

					/* Rimappa i valori per evitare l'interpolazione a frequenza 0.
					Il primo elemento viene interpolato linearmente. */
					LF[0] = L1SplineValue(F,F,Np,Hf[1]);
					LM[0] = L1SplineValue(F,M,Np,Hf[1]);
					LP[0] = L1SplineValue(F,P,Np,Hf[1]);

					for(I = 1;I < Np;I++)
						{
							LF[I] = F[I];
							LM[I] = M[I];
							LP[I] = P[I];
						}

					Ht[0] = std::polar<DLReal>(M[0],P[0]);

					if (CSLCASplineValue(LF,LM,LP,Np,&Hf[1],HtSize-1,&Ht[1],CSLBSpline,CSLLogXLogY) == False)
						{
							delete[](LP);
							delete[](LM);
							delete[](LF);
							delete[](Ht);
							delete[](Hf);
							return(False);
						}

					/* Dealloca gli array temporanei rimappatura logaritmica */
					delete[](LP);
					delete[](LM);
					delete[](LF);
				break;

				case PCHIPLinear:
					if (CSLCASplineValue(F,M,P,Np,Hf,HtSize,Ht,CSLHSSpline,CSLLinXLinY) == False)
						{
							delete[](Ht);
							delete[](Hf);
							return(False);
						}
				break;

				case PCHIPLogarithmic:
					/* Alloca gli array temporanei rimappatura logaritmica */
					if ((LF = new DLReal[Np]) == NULL)
						{
							delete[](Ht);
							delete[](Hf);
							return(False);
						}
					if ((LM = new DLReal[Np]) == NULL)
						{
							delete[](LF);
							delete[](Ht);
							delete(Hf);
							return(False);
						}
					if ((LP = new DLReal[Np]) == NULL)
						{
							delete[](LM);
							delete[](LF);
							delete[](Ht);
							delete[](Hf);
							return(False);
						}

					/* Rimappa i valori per evitare l'interpolazione a frequenza 0.
					Il primo elemento viene interpolato linearmente. */
					LF[0] = L1SplineValue(F,F,Np,Hf[1]);
					LM[0] = L1SplineValue(F,M,Np,Hf[1]);
					LP[0] = L1SplineValue(F,P,Np,Hf[1]);

					for(I = 1;I < Np;I++)
						{
							LF[I] = F[I];
							LM[I] = M[I];
							LP[I] = P[I];
						}

					Ht[0] = std::polar<DLReal>(M[0],P[0]);

					if (CSLCASplineValue(LF,LM,LP,Np,&Hf[1],HtSize-1,&Ht[1],CSLHSSpline,CSLLogXLogY) == False)
						{
							delete[](LP);
							delete[](LM);
							delete[](LF);
							delete[](Ht);
							delete[](Hf);
							return(False);
						}

					/* Dealloca gli array temporanei rimappatura logaritmica */
					delete[](LP);
					delete[](LM);
					delete[](LF);
				break;
			}

    if (Order%2 == 0)
      {
        for(I = 1;I < HtSize;I++)
          Ht[HtSize2-I] = -std::conj(Ht[I]);
        Ht[HtSize] = std::polar(M[Np-1],P[Np-1]);
        for (I = 0;I < HtSize2;I++)
          Ht[I] *= UnitRoot(I,2*HtSize2);
      }
    else
      {
        for(I = 1;I < HtSize;I++)
          Ht[2*HtSize-I] = std::conj(Ht[I]);
        Ht[HtSize] = std::polar(M[Np-1],P[Np-1]);
      }

    if (IFft(Ht,HtSize2) == False)
      {
        delete[](Ht);
        return(False);
      }

    HalfFilter = Order/2;

    for(I = 0;I < HalfFilter;I++)
      Filter[I] = std::real(Ht[HtSize2-HalfFilter+I]);
    for(I = HalfFilter;I < Order;I++)
      Filter[I] = std::real(Ht[I-HalfFilter]);

    delete[](Ht);
    delete[](Hf);
    return(True);
  }

/***************************************************************************/
