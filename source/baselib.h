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

/* Funzioni di libreria base */

#ifndef BaseLib_h
	#define BaseLib_h

	/* Inclusioni */
	#include "dsplib.h"
	#include "boolean.h"
	#include "drc.h"
	#include <stdio.h>

	/* Output stringhe con sync output e parametro */
	int sputsp(const char * s, const char * p);

	/* Output stringhe con sync output */
	int sputs(const char * s);

	/* Determina la lunghezza di un file */
	size_t FSize(FILE * F);

	/* Tipo file da leggere */
	typedef enum { PcmInt16Bit = 'I', PcmFloat32Bit = 'F', PcmFloat64Bit = 'D' } IFileType;

	/* Tipo ampiezza target function */
	typedef enum { MAGLinear = 'L', MAGdB = 'D' } TFMagType;

	/*
		Legge parte di un file di ingresso e lo pone nell'array indicato
		di dimensione InitWindow.
		Ritorna true se l'operazione ha successo.
	*/
	Boolean ReadSignal(const char * FName,DRCFloat * Dst,const int InitWindow,
		const int ImpulseCenter,const IFileType FType,
		int * PreSpikeStart, int * PostSpikeEnd);

	/* Scrive il segnale indicato su disco */
	Boolean WriteSignal(const char * FName,const DRCFloat * Src,const int SSize,
		const IFileType FType);

	/* Sovrascrive il segnale indicato su disco */
	Boolean OverwriteSignal(const char * FName,const DRCFloat * Src,const int SSize, const int Skip,
		const IFileType FType);

	/* Calcola l'autocorrelazione del sgnale S */
	Boolean AutoCorrelation(DLReal * S, int N);

	/* Calcola la cross correlazione tra S1 e S2 */
	/* XC deve avere lunghezza 2N - 1 */
	Boolean CrossCorrelation(DLReal * S1, DLReal * S2, int N, DLReal * XC);

	/* Calcola il ritardo di gruppo del segnale S */
	Boolean GroupDelay(const DLReal * S, const int N, DLReal * GD);

	/* I/O Delay computation, reliable only for simple impulse responses */
	DLReal LinearDelay(DLReal * Hn,unsigned int N,unsigned int Np,DLReal MZE);

	/* Conta il numero di righe in un file */
	int FLineCount(const char * FName);

	/* Legge i punti di generazione filtro FIR dal file indicato */
	Boolean ReadPoints(char * CorrFile,const TFMagType MagType,
		DLReal * FilterFreqs,DLReal * FilterM,DLReal * FilterP,const int NPoints,
		int SampleRate);

	/* Integra due funzioni di trsferimento definite per punti, usando
	una interpolazione lineare, ritorna il numero di punti generati,
	che non sono mai più di NPoints1 + NPoints2 */
	int LITFMerge(DLReal * FilterFreqs1,DLComplex * FilterPoints1,const int NPoints1,
		DLReal * FilterFreqs2,DLComplex * FilterPoints2,const int NPoints2,
		DLReal * FilterFreqsOut,DLComplex * FilterPointsOut);

	/* Trova il valore massimo all'interno di un file. */
	int FindMaxPcm(const char * FName,const IFileType FType);
#endif
