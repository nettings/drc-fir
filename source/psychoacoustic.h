/****************************************************************************

    DRC: Digital Room Correction
    Copyright (C) 2002, 2008 Denis Sbragion

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

/* Procedure a carattere psicoacustico e relative funzioni di supporto */

#ifndef PsychoAcoustic_h
	#define PsychoAcoustic_h

	/* Inclusioni */
	#include "dsplib.h"

	/* Ritorna l'ampiezza di banda a frazioni di ottava
	per una data frequenza */
	DLReal FOctBWidth(const DLReal F,const DLReal FOW);

	/* Versione della funzione precedente che opera su un
	array di frequenze */
	void AFOctBWidth(const DLReal * F, const int N,const DLReal FOW,DLReal * BW);

	/* Ritorna l'ampiezza di banda per una data frequenza
	secondo la scala di Bark */
	DLReal BarkBWidth(const DLReal F);

	/* Versione della funzione precedente che opera su un
	array di frequenze */
	void ABarkBWidth(const DLReal * F, const int N,DLReal * BW);

	/* Ritorna l'ampiezza di banda per una data frequenza
	secondo la scala ERB */
	DLReal ERBBWidth(const DLReal F);

	/* Versione della funzione precedente che opera su un
	array di frequenze */
	void AERBBWidth(const DLReal * F, const int N,DLReal * BW);

	/* Calcola la somma cumnulativa sull'array A.
	Se è l'array S non è null la somma cumulativa viene posta
	nell'array S, altrimenti viene posta direttamente in A
	sostituendo i valori contenuti */
	void CumulativeSum(DLReal * A,const int N,DLReal * S);

	/* Calcola l'inviluppo spettrale del segnale S con approssimazione
	dei picchi PDS e risoluzione BW, espressa in frazioni di ottava.
	Nel caso in cui PDS sia pari a 1.0 esegue il classico smoothing
	a frazioni di ottava. Nel caso in cui BW sia minore di 0.0 usa la
	scala di Bark, nel caso in cui BW sia minore di -1.0 usa la
	scala ERB */
	Boolean SpectralEnvelope(const DLReal * S,const int N,const int FS,
		const DLReal BW,const DLReal PDS,DLReal * SE);

	/* Tipo filtro target */
	typedef enum
		{
			MKSETFLinearPhase,
			MKSETFMinimumPhase
		}
	MKSETFType;

	/* Calcola un filtro target basato sull'inviluppo spettrale. Il filtro
	ha lunghezza pari a 2 volte la lunghezza del segnale in ingresso e non è
	finestrato */
	Boolean MKSETargetFilter(const DLReal * S,const int N,const int FS,
		const DLReal BW,const DLReal PDS,DLReal * TF,const MKSETFType TFType,
		const DLReal MinGain,const DLReal DLStart,const int SampleFreq,
		const DLReal StartFreq,const DLReal EndFreq);

	/* Versione della funzione precedente che effettua un padding del segnale
	in ingresso alla prima potenza di due disponibile. Il filtro in uscita ha
	lunghezza pari a TFN. TFN non può essere superiore alla lunghezza usata
	internamente per il calcolo del filtro, quindi 2 * N se  MExp < 0, oppure
	2 * nextpow2(N) * 2 ^ MExp per MExp >= 0 */
	Boolean P2MKSETargetFilter(const DLReal * S,const int N,const int FS,
		const DLReal BW,const DLReal PDS,DLReal * TF,const MKSETFType TFType,
		const int MExp,const int TFN,	const DLReal MinGain,const DLReal DLStart,
		const int SampleFreq,const DLReal StartFreq,const DLReal EndFreq);
#endif
