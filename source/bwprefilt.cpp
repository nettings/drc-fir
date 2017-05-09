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

/* Prefiltratura a bande di un segnale */

#include <stdio.h>
#include "bwprefilt.h"
#include "convol.h"
#include "fir.h"
#include "level.h"

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

/* Calcolo potenza intera di un numero */
DLReal IntPow(DLReal X,unsigned int N)
  {
    unsigned int I;
    DLReal Pow = 1;

    for(I = 1;I <= N;I <<= 1)
      {
        if ((I & N) != 0)
          Pow *= X;
        X *= X;
      }

    return(Pow);
  }

/* Prefiltratura a bande di un segnale */
void BWPreFilt(const DLReal * InImp, const int IBS, const int FBS,
	const int FilterLen, const int BandSplit, const DLReal WindowExponent,
	const int SampleFreq, const DLReal StartFreq, const DLReal EndFreq,
	const int WindowGap, DLReal * OutImp,
	const WindowType WType, const BWPPrefilteringType BWPType)
	{
		/* Array risultati parziali */
		DLReal * COut;

		/* Array filtro FIR */
		DLReal * FIRFilter;

		/* Array finestratura segnale di ingresso */
		DLReal * CIn;

		/* Array temporanei convoluzione */
		DLComplex * CA;
		DLComplex * CB;

		/* Numero banda corrente */
		int Band;

		/* Estremi della banda. */
		DLReal BLow;
		DLReal BHigh;

		/* Larghezza banda */
		DLReal BWidth;

		/* Inizio e fine filtratura */
		DLReal FilterBegin;
		DLReal FilterEnd;

  	/* Posizione finestra su segnale */
		int WStart;
		int WLen;
		int I;
		int OBS;

		/* Coefficienti calcolo finestra su segnale */
		DLReal A;
		DLReal Q;
		DLReal B;

		/* Calcola la dimensione del blocco in uscita dalla convoluzione */
		OBS = IBS + FilterLen - 1;

		/* Calcola inizio e fine della prefiltratura */
		FilterBegin = (2 * StartFreq) / SampleFreq;
		FilterEnd = (2 * EndFreq) / SampleFreq;

		/* Alloca gli array temporanei */
		CIn = new DLReal[IBS];
		COut = new DLReal[OBS];
		FIRFilter = new DLReal[FilterLen];

		/* Calcola la dimensione degli array di appoggio convoluzione */
    for (I = 1; I < OBS; I <<= 1);

		/* Alloca gli array di appoggio convoluzione */
		CA = new DLComplex[I];
		CB = new DLComplex[I];

		/* Imposta i parametri iniziali */
		Band = 0;
		BLow = 0;
		BHigh = 0;
		BWidth = (DLReal) pow(2,1.0/BandSplit);

		/* Verifica il tipo di curva di prefiltratura */
		switch (BWPType)
			{
				/* Proporzionale */
				case BWPProportional:
					/* Calcola i coefficienti per il calcolo finestratura */
					B = (DLReal) exp(log(((DLReal) FBS) / IBS) / WindowExponent);
					Q = (DLReal) (B * FilterEnd - FilterBegin) / (1.0 - B);
					A = (DLReal) (1.0 / (IBS * pow(FilterBegin + Q,WindowExponent)));
				break;

				/* Bilineare */
				case BWPBilinear:
					/* Calcola i coefficienti per il calcolo finestratura */
					A = (DLReal) (IBS - FBS);
					Q = (DLReal) ((pow(WindowExponent,4.0) * (FilterBegin / FilterEnd)) - 1.0);
					B = (DLReal) (Q + 1.0);
				break;
			}

		/* Ciclo sulle bande */
		while (BHigh < FilterEnd)
			{
				/* Calcola gli estremi banda */
				if (Band == 0)
					BLow = 0;
				else
					BLow = (DLReal) (FilterBegin * IntPow(BWidth,Band - 1));

				BHigh = (DLReal) (FilterBegin * IntPow(BWidth,Band));
				if (BHigh > FilterEnd)
					BHigh = 1.0;

				/* Verifica la banda di finestratura */
				if (Band == 0)
					WLen = IBS;
				else
					{
						/* Verifica il tipo di curva di prefiltratura */
						switch (BWPType)
							{
								/* Proporzionale */
								case BWPProportional:
									/* Calcola l'intervallo di finestratura */
									WLen = 1 + (int) (ceil(0.5 / (A * pow(BHigh + Q,WindowExponent))) * 2);
								break;

								/* Bilineare */
								case BWPBilinear:
									/* Calcola l'intervallo di finestratura */
									WLen = 1 + (int) (ceil(0.5 * (FBS + A * (1.0 - ((BHigh - FilterBegin) /
										(B - (BHigh - FilterBegin) * Q))))) * 2);
								break;
							}
					}

				/* Controlla che non vi siano errori di arrotondamento */
				if (WLen >= IBS)
					WLen = ((IBS / 2) * 2) - 1;
				if (WLen < FBS)
					WLen = FBS;

				/* Calcola il punto di partenza */
				WStart = (IBS - WLen) / 2;

				/* Riporta la banda */
				printf("Band: %3d, %7.1f - %7.1f Hz, FIR, ", (int) Band,
					(double) (BLow * SampleFreq) / 2, (double) (BHigh * SampleFreq) / 2);
				fflush(stdout);

				printf("wind: %6d, ", (int) WLen);
				fflush(stdout);

				/* Effettua la finestratura del segnale */
				for (I = 0; I < IBS; I++)
					CIn[I] = InImp[I];
				if (WindowGap > WLen)
					SpacedBlackmanWindow(&CIn[WStart],WLen,WindowGap,WType);
				else
					if (WLen > 2)
						SpacedBlackmanWindow(&CIn[WStart],WLen,WLen - 2,WType);
					else
						SpacedBlackmanWindow(&CIn[WStart],WLen,0,WType);

				/* Azzeramento parte esterna alla finestra */
				if (WType != WRight)
					for (I = 0; I < WStart; I++)
						CIn[I] = 0;
				if (WType != WLeft)
					for (I = WStart + WLen; I < IBS; I++)
						CIn[I] = 0;

				/* Calcola il filtro passa banda */
				BandPassFir(FIRFilter,FilterLen,BLow,BHigh);
				BlackmanWindow(FIRFilter,FilterLen);

				printf("conv, ");
				fflush(stdout);

				/* Effettua la convoluzione tra segnale e filtro */
				DFftConvolve(FIRFilter,FilterLen,CIn,IBS,COut,CA,CB);

				printf("sum, ");
				fflush(stdout);

				/* Aggiunge il segnale al segnale risultante */
				for (I = 0; I < OBS; I++)
					OutImp[I] += COut[I];

				printf("done.\n");
				fflush(stdout);

				Band++;
			}

		/* Dealloca gli array intermedi */
		delete[] CIn;
		delete[] COut;
		delete[] FIRFilter;
		delete[] CA;
		delete[] CB;
	}
