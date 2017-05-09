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

/* Deconvoluzione omomorfa */

/* Inclusioni */
#include "hd.h"
#include "fft.h"
#include "baselib.h"
#include "level.h"
#include <math.h>

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

/* Effettua la deconvoluzione omomorfa del segnale In */
/* Versione basata sul calcolo del Cepstrum */
Boolean CepstrumHD(const DLReal * In, DLReal * MPOut, DLReal * EPOut,
	const int N, const int MExp)
	{
		DLComplex * FFTArray1;
		DLComplex * FFTArray2;
		int FS;
		int I;
		Boolean LogLimit;
		DLReal CV;

		/* Controlla se si deve adottare una potenza di due */
		if (MExp >= 0)
			{
				/* Calcola la potenza di due superiore a N */
				for (FS = 1;FS <= N;FS <<= 1);
				FS *= 1 << MExp;
			}
		else
			FS = N;

		/* Alloca gli array per l'FFT */
		if ((FFTArray1 = new DLComplex[FS]) == NULL)
			return False;
		if ((FFTArray2 = new DLComplex[FS]) == NULL)
			return False;

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0;I < N;I++)
			FFTArray1[I] = In[I];

		/* Azzera la parte rimanente */
		for (I = N;I < FS;I++)
			FFTArray1[I] = 0;

		/* Trasforma l'array risultante */
		Fft(FFTArray1,FS);

		/* Calcola i valori per il cepstrum */
		LogLimit = False;
		for (I = 0;I < FS;I++)
			{
				CV = std::abs<DLReal>(FFTArray1[I]);
				if (CV <= 0)
					{
						LogLimit = True;
						FFTArray2[I] = (DLReal) log(DRCMinFloat);
					}
				else
					FFTArray2[I] = std::log<DLReal>(CV);
			}

		/* Verifica se si è raggiunto il limite */
		if (LogLimit == True)
			sputs("Notice: log limit reached in cepstrum computation.");

		/* Calcola il cepstrum */
		IFft(FFTArray2,FS);

		/* Finestra il cepstrum */
		for (I = 1; I < FS/2;I++)
			FFTArray2[I] *= 2;
		for (I = FS/2 + 1; I < FS;I++)
			FFTArray2[I] = 0;

		/* Calcola la trsformata del cepstrum finestrato */
		Fft(FFTArray2,FS);

		/* Effettua il calcolo dell'esponenziale */
		for (I = 0;I < FS;I++)
			FFTArray2[I] = std::exp<DLReal>(FFTArray2[I]);

		/* Verifica se deve estrarre la componente EP */
		if (EPOut != NULL)
			{
				/* Determina la trasformata della parte excess phase */
				for (I = 0;I < FS;I++)
					FFTArray1[I] = std::polar((DLReal) 1.0,
						std::arg(FFTArray1[I]) - std::arg(FFTArray2[I]));

				/* Determina la risposta del sistema excess phase */
				IFft(FFTArray1,FS);

				/* Copia il risultato nell'array destinazione */
				for (I = 0;I < N;I++)
					EPOut[I] = std::real<DLReal>(FFTArray1[I]);
			}

		/* Verifica se deve estrarre la componente MP */
		if (MPOut != NULL)
			{
				/* Determina la risposta del sistema a fase minima */
				IFft(FFTArray2,FS);

				/* Copia il risultato nell'array destinazione */
				for (I = 0;I < N;I++)
					MPOut[I] = std::real<DLReal>(FFTArray2[I]);
			}

		/* Dealloca gli array */
		delete[] FFTArray1;
		delete[] FFTArray2;

		/* Operazione completata */
		return True;
	}

/* Effettua la deconvoluzione omomorfa del segnale In */
/* Versione basata sulla trasformata di Hilbert */
Boolean HilbertHD(const DLReal * In, DLReal * MPOut, DLReal * EPOut,
	const int N,const int MExp)
	{
		DLComplex * FFTArray1;
		DLComplex * FFTArray2;
		DLReal * FFTArray3;
		int FS;
		int I;
		Boolean LogLimit;
		DLReal CV;

		/* Controlla se si deve adottare una potenza di due */
		if (MExp >= 0)
			{
				/* Calcola la potenza di due superiore a N */
				for (FS = 1;FS <= N;FS <<= 1);
				FS *= 1 << MExp;
			}
		else
			FS = N;

		/* Alloca gli array per l'FFT */
		if ((FFTArray1 = new DLComplex[FS]) == NULL)
			return False;
		if ((FFTArray2 = new DLComplex[FS]) == NULL)
			return False;
		if ((FFTArray3 = new DLReal[FS]) == NULL)
			return False;

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0;I < N;I++)
			FFTArray1[I] = In[I];

		/* Azzera la parte rimanente */
		for (I = N;I < FS;I++)
			FFTArray1[I] = 0;

		/* Trasforma l'array risultante */
		Fft(FFTArray1,FS);

		/* Calcola i valori per la trasformata di Hilbert */
		LogLimit = False;
		for (I = 0;I < FS;I++)
			{
				CV = std::abs<DLReal>(FFTArray1[I]);
				if (CV <= DRCMinFloat)
					{
						LogLimit = True;
						FFTArray2[I] = (DLReal) log(DRCMinFloat);
						FFTArray3[I] = DRCMinFloat;
					}
				else
					{
						FFTArray2[I] = std::log<DLReal>(CV);
						FFTArray3[I] = CV;
					}
			}

		/* Verifica se si è raggiunto il limite */
		if (LogLimit == True)
			sputs("Notice: log limit reached in Hilbert computation.");

		/* Calcola la fase per la componente a fase minima */
		IFft(FFTArray2,FS);
		for (I = 1 + FS / 2; I < FS;I++)
			FFTArray2[I] = -FFTArray2[I];
		FFTArray2[0] = 0;
		FFTArray2[FS / 2] = 0;
		Fft(FFTArray2,FS);

		/* Effettua la convoluzione per l'estrazione del sistema
		a fase minima */
		for (I = 0; I < FS;I++)
			FFTArray2[I] = FFTArray3[I] * std::exp<DLReal>(FFTArray2[I]);

		/* Dealloca gli array */
		delete[] FFTArray3;

		/* Verifica se deve estrarre la componente EP */
		if (EPOut != NULL)
			{
				/* Determina la trasformata della parte excess phase */
				for (I = 0;I < FS;I++)
					FFTArray1[I] = std::polar((DLReal) 1.0,
						std::arg(FFTArray1[I]) - std::arg(FFTArray2[I]));

				/* Determina la risposta del sistema excess phase */
				IFft(FFTArray1,FS);

				/* Copia il risultato nell'array destinazione */
				for (I = 0;I < N;I++)
					EPOut[I] = std::real<DLReal>(FFTArray1[I]);
			}

		/* Verifica se deve estrarre la componente MP */
		if (MPOut != NULL)
			{
				/* Determina la risposta del sistema a fase minima */
				IFft(FFTArray2,FS);

				/* Copia il risultato nell'array destinazione */
				for (I = 0;I < N;I++)
					MPOut[I] = std::real<DLReal>(FFTArray2[I]);
			}

		/* Dealloca gli array */
		delete[] FFTArray1;
		delete[] FFTArray2;

		/* Operazione completata */
		return True;
	}

/* Experimental nonsense, ignore */

/* Effettua la decomposizione del segnale In nelle somponenti allmag/allphase */
Boolean AMAPDecomposition(const DLReal * In, DLReal * AMOut, DLReal * APOut,
		const int N, const int PFactor)
	{
		DLComplex * FFTArray1;
		DLComplex * FFTArray2;

		int FS;
		int I,J;
		DLReal PA[2];

		/* Calcola la potenza di due superiore a N */
		for(FS = 1;FS <= N;FS <<= 1);
		FS *= 1 << PFactor;

		/* Alloca gli array per l'FFT */
		if ((FFTArray1 = new DLComplex[FS]) == NULL)
			return False;
		if ((FFTArray2 = new DLComplex[FS]) == NULL)
			return False;

		/* Azzera l'array conversione */
		for (I = 0;I < FS;I++)
			FFTArray1[I] = 0;

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0,J = (FS - N) / 2;I < N;I++,J++)
			FFTArray1[J] = In[I];

		/* Trasforma l'array risultante */
		Radix2Fft(FFTArray1,FS);

		/* Trasforma nella componente allmag */
		PA[0] = 1;
		PA[1] = -1;
		for (I = 0;I < FS;I++)
			FFTArray2[I] = FFTArray1[I] = PA[I % 2] * std::abs(FFTArray1[I]);

		/* Calcola la componente allmag */
		Radix2IFft(FFTArray1,FS);

		/* Copia il risultato nell'array destinazione */
		for (I = 0,J = (FS - N) / 2;I < N;I++,J++)
			AMOut[I] = std::real<DLReal>(FFTArray1[J]);

		/* Azzera l'array conversione */
		for (I = 0;I < FS;I++)
			FFTArray1[I] = 0;

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0,J = (FS - N) / 2;I < N;I++,J++)
			FFTArray1[J] = In[I];

		/* Trasforma l'array risultante */
		Radix2Fft(FFTArray1,FS);

		/* Trasforma nella componente allphase */
		for (I = 0;I < FS;I++)
			FFTArray1[I] = FFTArray1[I] / FFTArray2[I];

		/* Calcola la componente allphase */
		Radix2IFft(FFTArray1,FS);

		/* Copia il risultato nell'array destinazione */
		for (I = 0;I < N / 2;I++)
			APOut[I] = std::real<DLReal>(FFTArray1[FS - I]);
		for (I = N / 2;I < N;I++)
			APOut[I] = std::real<DLReal>(FFTArray1[I]);

		/* Rinormalizza il risultato */
		// NormFlat(APOut,N,1,PFactor);

		/* Dealloca gli array temporanei */
		delete[] FFTArray1;
		delete[] FFTArray2;

		/* Operazione completata */
		return True;
	}

/* Effettua la decomposizione del segnale In nelle somponenti no phase/no mag  */
Boolean NPNMDecomposition(const DLReal * In, DLReal * NPOut, DLReal * NMOut,
		const int N, const int PFactor)
	{
		DLComplex * FFTArray1;
		DLComplex * FFTArray2;

		int FS;
		int I;

		/* Calcola la potenza di due superiore a N */
		for(FS = 1;FS <= N;FS <<= 1);
		FS *= 1 << PFactor;

		/* Alloca gli array per l'FFT */
		if ((FFTArray1 = new DLComplex[FS]) == NULL)
			return False;
		if ((FFTArray2 = new DLComplex[FS]) == NULL)
			return False;

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0;I < N;I++)
			FFTArray1[I] = In[I];

		/* Azzera la parte rimanente */
		for (I = N;I < FS;I++)
			FFTArray1[I] = 0;

		/* Trasforma l'array risultante */
		Radix2Fft(FFTArray1,FS);

		/* Trasforma nella componente no phase */
		for (I = 0;I < FS;I++)
			FFTArray2[I] = std::abs(FFTArray1[I]);

		/* Calcola la componente allmag */
		Radix2IFft(FFTArray2,FS);

		/* Copia il risultato nell'array destinazione */
		for (I = 0;I < N;I++)
			NPOut[I] = std::real<DLReal>(FFTArray2[I]);

		/* Trasforma nella componente allphase */
		for (I = 0;I < FS;I++)
			FFTArray2[I] = FFTArray1[I] / std::abs(FFTArray1[I]);

		/* Calcola la componente allphase */
		Radix2IFft(FFTArray2,FS);

		/* Copia il risultato nell'array destinazione */
		for (I = 0;I < N;I++)
			NMOut[I] = std::real<DLReal>(FFTArray2[I]);

		/* Dealloca gli array temporanei */
		delete[] FFTArray1;
		delete[] FFTArray2;

		/* Operazione completata */
		return True;
	}

