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

/* Inclusioni */
#include "kirkebyfd.h"
#include "fft.h"
#include "baselib.h"
#include "level.h"
#include "hd.h"
#include "dspwind.h"
#include "slprefilt.h"
#include "bwprefilt.h"
#include "convol.h"
#include <math.h>

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

/* Inversione tramite Kirkeby fast deconvolution */
Boolean KirkebyFDInvert(const DLReal * InSig, int InSigLen,DLReal * InvFilter,
	int InvFilterLen, const DLReal * EffortSig, int EffortSigLen, DLReal EffortFactor,
	int MExp)
	{
		/* Dimensione da considerare per il segnale */
		int SigLen;

		/* Dimensione FFT */
		int FS;

		/* Array FFT */
		DLComplex * FFTArray1;
		DLComplex * FFTArray2;

		/* Livello RMS segnale in ingresso */
		DLReal IRMS;

		/* Livello massimo del segnale di effort */
		DLReal EMax;

		/* Indice generico */
		int I = 0;
		int J = 0;

		/* Determina la dimensione massima tra i vari segnali */
		if (InSigLen > InvFilterLen)
			SigLen = InSigLen;
		else
			SigLen = InvFilterLen;
		if (EffortSig != NULL)
			if (EffortSigLen > SigLen)
				SigLen = EffortSigLen;

		/* Controlla se si deve adottare un potenza di due */
		if (MExp >= 0)
			{
				/* Calcola la potenza di due superiore a N */
				for(FS = 1;FS <= SigLen;FS <<= 1);
				FS *= 1 << MExp;
			}
		else
			FS = SigLen;

		/* Alloca gli array per l'FFT */
		if ((FFTArray1 = new DLComplex[FS]) == NULL)
			return False;
		if (EffortSig != NULL)
			if ((FFTArray2 = new DLComplex[FS]) == NULL)
				return False;

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0;I < InSigLen;I++)
			FFTArray1[I] = InSig[I];

		/* Azzera la parte rimanente */
		for (I = InSigLen;I < FS;I++)
			FFTArray1[I] = 0;

		if (EffortSig != NULL)
			{
				/* Copia l'array effort in quello temporaneo */
				for (I = 0;I < EffortSigLen;I++)
					FFTArray2[I] = EffortSig[I];

				/* Azzera la parte rimanente */
				for (I = EffortSigLen;I < FS;I++)
					FFTArray2[I] = 0;

				/* Porta nel dominio della frequenza */
				Fft(FFTArray2,FS);

				/* Calcola il valore massimo dell'array effort */
				EMax = 0;
				for (I = 0;I < FS;I++)
					if (std::abs(FFTArray2[I]) > EMax)
						EMax = std::abs(FFTArray2[I]);
				EMax *= EMax;

				/* Riscala il fattore di effort */
				EffortFactor /= EMax;
			}

		/* Porta nel dominio della frequenza */
		Fft(FFTArray1,FS);

		/* Calcola il livello RMS del segnale in ingresso */
		IRMS = GetRMSLevel(InSig,InSigLen);

		/* Riscala il fattore di effort */
		EffortFactor *= IRMS * IRMS;

		/* Effettua la fast deconvolution */
		if (EffortSig != NULL)
			{
				for (I = 0;I < FS;I++)
					FFTArray1[I] = std::conj<DLReal>(FFTArray1[I]) /
						(FFTArray1[I] * std::conj<DLReal>(FFTArray1[I]) +
						EffortFactor * (EMax - FFTArray2[I] * std::conj<DLReal>(FFTArray2[I])));
			}
		else
			{
				for (I = 0;I < FS;I++)
					FFTArray1[I] = std::conj<DLReal>(FFTArray1[I]) /
						(FFTArray1[I] * std::conj<DLReal>(FFTArray1[I]) + EffortFactor);
			}

		/* Ritorna nel dominio del tempo */
		IFft(FFTArray1,FS);

		/* Estrae il filtro inverso */
		for (I = 0,J = (FS + FS - (InSigLen - InvFilterLen) / 2) % FS;I < InvFilterLen;I++,J = (J + 1) % FS)
			InvFilter[I] = std::real<DLReal>(FFTArray1[J]);

		/* Dealloca gli array temporanei */
		delete[] FFTArray1;
		if (EffortSig != NULL)
			delete[] FFTArray2;

		/* Operazione completata */
		return True;
	}

/* Inversione tramite selective minimum pahse fast deconvolution */
/* Versione a misura indiretta degli effetti di pre echo */
Boolean PEISMPKirkebyFDInvert(const DLReal * MPSig, int MPSigLen, const DLReal * EPSig, int EPSigLen,
	DLReal * InvFilter,	int InvFilterLen, DLReal EffortFactor, int MExp)
	{
		/* Dimensione da considerare per il segnale */
		int SigLen;

		/* Dimensione FFT */
		int FS;

		/* Array FFT */
		DLComplex * FFTArray1;
		DLComplex * FFTArray2;

		/* Valore massimo e minimo componente EP */
		DLReal * AEPAbs;
		DLReal EPAbs;
		DLReal EMax;

		/* Indici generici */
		int I;
		int J;

		/* Determina la dimensione massima tra i vari segnali */
		if (MPSigLen > InvFilterLen)
			SigLen = MPSigLen;
		else
			SigLen = InvFilterLen;
		if (EPSigLen > SigLen)
			SigLen = EPSigLen;

		/* Controlla se si deve adottare una potenza di due */
		if (MExp >= 0)
			{
				/* Calcola la potenza di due superiore a N */
				for(FS = 1;FS <= SigLen;FS <<= 1);
				FS *= 1 << MExp;
			}
		else
			FS = SigLen;

		/* Alloca gli array per l'FFT */
		if ((FFTArray1 = new DLComplex[FS]) == NULL)
			return False;
		if ((FFTArray2 = new DLComplex[FS]) == NULL)
			return False;

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0;I < MPSigLen;I++)
			FFTArray1[I] = MPSig[I];

		/* Azzera la parte rimanente */
		for (I = MPSigLen;I < FS;I++)
			FFTArray1[I] = 0;

		/* Porta nel dominio della frequenza */
		Fft(FFTArray1,FS);

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0;I < EPSigLen;I++)
			FFTArray2[I] = EPSig[I];

		/* Azzera la parte rimanente */
		for (I = EPSigLen;I < FS;I++)
			FFTArray2[I] = 0;

		/* Porta nel dominio della frequenza */
		Fft(FFTArray2,FS);

		/* Alloca l' array per l'ampiezza EP */
		if ((AEPAbs = new DLReal[FS]) == NULL)
			return False;

		/* Estrae l'array valori assoluti */
		for (I = 0;I < FS;I++)
			AEPAbs[I] = std::abs<DLReal>(FFTArray2[I]);

		/* Calcola il valore massimo della componente EP */
		EMax = 0;
		for (I = 0;I < FS;I++)
			{
				if (AEPAbs[I] > EMax)
					EMax = AEPAbs[I];
			}

		/* Riscala il fattore di effort */
		if (EffortFactor >= 0)
			EffortFactor = -EffortFactor / ((DLReal) 1.0 + EffortFactor);
		else
			EffortFactor = EffortFactor / (EffortFactor - (DLReal) 1.0);

		/* Effettua la fast deconvolution selettiva
		della componente EP */
		for (I = 0;I < FS;I++)
			{
				/* Normalizza il valore assoluto */
				EPAbs = AEPAbs[I] / EMax;

				/* Applica il fattore di effort controllando
				eventuali singolarità */
				if (EPAbs <= (DLReal) 0.0)
					EPAbs = (DLReal) 0.0;
				else
					if (EPAbs >= (DLReal) 1.0)
						EPAbs = (DLReal) 1.0;
					else
						{
							EPAbs = sqrt(EPAbs);
							EPAbs = (EPAbs * ((DLReal) 1.0 + EffortFactor)) / ((DLReal) 1.0 + EPAbs * EffortFactor);
						}

				/* Calcola la trasformata della deconvoluzione selettiva */
				FFTArray2[I] = std::polar<DLReal>(EPAbs,-std::arg<DLReal>(FFTArray2[I])) +
					std::polar<DLReal>((DLReal) 1.0 - EPAbs,(I * M_PI * EPSigLen) / FS);
			}

		/* Dealloca gli array temporanei */
		delete[] AEPAbs;

		/* Compone l'inverso componente MP e EP */
		for (I = 0;I < FS;I++)
		  FFTArray1[I] = std::polar<DLReal>(1,std::arg<DLReal>(FFTArray2[I])) / FFTArray1[I];

		/* Ritorna nel dominio del tempo */
		IFft(FFTArray1,FS);

		/* Estrae il filtro inverso */
		for (I = 0,J = (1 + FS + FS - (MPSigLen + EPSigLen + InvFilterLen) / 2) % FS;I < InvFilterLen;I++,J = (J + 1) % FS)
			InvFilter[I] = std::real<DLReal>(FFTArray1[J]);

		/* Dealloca gli array temporanei */
		delete[] FFTArray1;
		delete[] FFTArray2;

		/* Operazione completata */
		return True;
	}

/* Inversione tramite selective minimum pahse fast deconvolution */
/* Versione a misura diretta degli effetti di pre echo */
Boolean PEMSMPKirkebyFDInvert(const DLReal * MPSig, int MPSigLen, const DLReal * EPSig, int EPSigLen,
	DLReal * InvFilter,	int InvFilterLen, DLReal EffortFactor, int PEStart, int PETransition, int MExp)
	{
		/* Dimensione da considerare per il segnale */
		int SigLen;

		/* Dimensione FFT */
		int FS;

		/* Array FFT */
		DLComplex * FFTArray1;
		DLComplex * FFTArray2;

		/* Valore massimo e minimo componente EP */
		DLReal * AEPAbs;
		DLReal EPAbs;
		DLReal EMax;

		/* Indici generici */
		int I;
		int J;

		/* Determina la dimensione massima tra i vari segnali */
		if (MPSigLen > InvFilterLen)
			SigLen = MPSigLen;
		else
			SigLen = InvFilterLen;
		if (EPSigLen > SigLen)
			SigLen = EPSigLen;

		/* Controlla se si deve adottare una potenza di due */
		if (MExp >= 0)
			{
				/* Calcola la potenza di due superiore a N */
				for(FS = 1;FS <= SigLen;FS <<= 1);
				FS *= 1 << MExp;
			}
		else
			FS = SigLen;

		/* Alloca gli array per l'FFT */
		if ((FFTArray1 = new DLComplex[FS]) == NULL)
			return False;
		if ((FFTArray2 = new DLComplex[FS]) == NULL)
			return False;

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0;I < MPSigLen;I++)
			FFTArray1[I] = MPSig[I];

		/* Azzera la parte rimanente */
		for (I = MPSigLen;I < FS;I++)
			FFTArray1[I] = 0;

		/* Porta nel dominio della frequenza */
		Fft(FFTArray1,FS);

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0;I < EPSigLen;I++)
			FFTArray2[I] = EPSig[I];

		/* Azzera la parte rimanente */
		for (I = EPSigLen;I < FS;I++)
			FFTArray2[I] = 0;

		/* Porta nel dominio della frequenza */
		Fft(FFTArray2,FS);

		/* Effettua l'inversione per l'estrazione degli effetti di pre-echo */
		for (I = 0;I < FS;I++)
			FFTArray2[I] = std::polar<DLReal>((DLReal) 1.0,-std::arg<DLReal>(FFTArray2[I]));

		/* Riporta nel dominio del tempo */
		IFft(FFTArray2,FS);

		/* Alloca l' array per l'estrazione della parte di pre-echo */
		if ((AEPAbs = new DLReal[FS]) == NULL)
			return False;

		/* Estrae l'inverso della parte EP selezionando solo la zona di pre-echo */
		for (I = 0; I < FS - EPSigLen;I++)
			AEPAbs[I] = 0;
		for (I = FS - EPSigLen; I < FS - (PEStart + EPSigLen / 2);I++)
			AEPAbs[I] = std::real<DLReal>(FFTArray2[I]);
		for (I = FS - (PEStart + EPSigLen / 2); I < FS;I++)
			AEPAbs[I] = 0;
		SpacedBlackmanWindow(&AEPAbs[FS - EPSigLen],(EPSigLen / 2) - PEStart,(EPSigLen / 2) - 2 * PETransition,WFull);

		/* Riporta la porzione di pre-echo nel dominio della frequenza */
		for (I = 0;I < FS;I++)
			FFTArray2[I] = AEPAbs[I];
		Fft(FFTArray2,FS);

		/* Estrae l'array valori assoluti porzione di pre-echo */
		for (I = 0;I < FS;I++)
			AEPAbs[I] = std::abs<DLReal>(FFTArray2[I]);

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0;I < EPSigLen;I++)
			FFTArray2[I] = EPSig[I];

		/* Azzera la parte rimanente */
		for (I = EPSigLen;I < FS;I++)
			FFTArray2[I] = 0;

		/* Porta nel dominio della frequenza */
		Fft(FFTArray2,FS);

		/* Calcola il valore massimo della componente di pre-echo */
		EMax = 0;
		for (I = 0;I < FS;I++)
			{
				if (AEPAbs[I] > EMax)
					EMax = AEPAbs[I];
			}

		/* Riscala il fattore di effort */
		if (EffortFactor >= 0)
			EffortFactor = -EffortFactor / ((DLReal) 1.0 + EffortFactor);
		else
			EffortFactor = EffortFactor / (EffortFactor - (DLReal) 1.0);

		/* Effettua la fast deconvolution selettiva
		della componente EP */
		for (I = 0;I < FS;I++)
			{
				/* Normalizza il valore assoluto */
				EPAbs = AEPAbs[I] / EMax;

				/* Applica il fattore di effort controllando
				eventuali singolarità */
				if (EPAbs <= (DLReal) 0.0)
					EPAbs = (DLReal) 1.0;
				else
					if (EPAbs >= (DLReal) 1.0)
						EPAbs = (DLReal) 0.0;
					else
						{
							EPAbs = (DLReal) 1.0 - sqrt(EPAbs);
							EPAbs = (EPAbs * ((DLReal) 1.0 + EffortFactor)) / ((DLReal) 1.0 + EPAbs * EffortFactor);
						}

				/* Calcola la trasformata della deconvoluzione selettiva */
				FFTArray2[I] = std::polar<DLReal>((DLReal) EPAbs,-std::arg<DLReal>(FFTArray2[I])) +
					std::polar<DLReal>(1.0 - EPAbs,(I * M_PI * EPSigLen) / FS);
			}

		/* Dealloca gli array temporanei */
		delete[] AEPAbs;

		/* Compone l'inverso componente MP e EP */
		for (I = 0;I < FS;I++)
		  FFTArray1[I] = std::polar<DLReal>(1,std::arg<DLReal>(FFTArray2[I])) / FFTArray1[I];

		/* Ritorna nel dominio del tempo */
		IFft(FFTArray1,FS);

		/* Estrae il filtro inverso */
		for (I = 0,J = (1 + FS + FS - (MPSigLen + EPSigLen + InvFilterLen) / 2) % FS;I < InvFilterLen;I++,J = (J + 1) % FS)
			InvFilter[I] = std::real<DLReal>(FFTArray1[J]);

		/* Dealloca gli array temporanei */
		delete[] FFTArray1;
		delete[] FFTArray2;

		/* Operazione completata */
		return True;
	}

/* Inversione tramite selective minimum pahse fast deconvolution */
/* Versione a troncatura degli effetti di pre echo */
Boolean PETFDInvert(const DLReal * MPSig, int MPSigLen, const DLReal * EPSig, int EPSigLen,
	DLReal * InvFilter,	int InvFilterLen, char PEType, int PELowerWindow, int PEUpperWindow,
	DLReal PEStartFreq, DLReal PEEndFreq, int PEFilterLen, DLReal FSharpness, int PEBandSplit,
	DLReal PEWindowExponent, const SLPPrefilteringType SLPType, const DLReal OGainFactor,
	int SampleRate, int MExp)
	{
		/* Dimensione da considerare per il segnale */
		int SigLen;

		/* Dimensione FFT */
		int FS;

		/* Array FFT */
		DLComplex * FFTArray1;
		DLComplex * FFTArray2;

		/* Array rimozione pre-echo */
		DLReal * EPIPERemove;

		/* Array prefiltratura componente EP */
		DLReal * EPPFOut;

		/* Dimensione blocco di uscita filtratura */
		int EPPFOutSize;

		/* Indici generici */
		int I;
		int J;

		/* Determina la dimensione massima tra i vari segnali */
		if (MPSigLen > InvFilterLen)
			SigLen = MPSigLen;
		else
			SigLen = InvFilterLen;
		if (EPSigLen > SigLen)
			SigLen = EPSigLen;

		/* Controlla se si deve adottare una potenza di due */
		if (MExp >= 0)
			{
				/* Calcola la potenza di due superiore a N */
				for(FS = 1;FS <= SigLen;FS <<= 1);
				FS *= 1 << MExp;
			}
		else
			FS = SigLen;

		/* Alloca l'array per la componente EP */
		if ((EPIPERemove = new DLReal[FS]) == NULL)
			return False;

		/* Copia l'array sorgente in quello temporaneo in ordine inverso
		in modo da ottenere direttamente il filtro inverso */
		for (I = 0,J = FS - 1;I < EPSigLen;I++,J--)
			EPIPERemove[J] = EPSig[I];

		/* Azzera la parte rimanente */
		for (I = EPSigLen,J = 0;I < FS;I++,J++)
			EPIPERemove[J] = (DLReal) 0.0;

		/* Controlla il tipo di rimozione pre-echo da adottare */
		if (PEType == 'f')
			{
				/* Estrae la parte EP rimuovendo la zona di pre-echo */
				for (I = 0; I < FS - (PELowerWindow + EPSigLen / 2);I++)
					EPIPERemove[I] = 0;
				SpacedBlackmanWindow(&EPIPERemove[FS - (PELowerWindow + EPSigLen / 2)],
					PELowerWindow + EPSigLen / 2,
					(PELowerWindow + EPSigLen / 2) - 2 * (PELowerWindow - PEUpperWindow),WLeft);

				/* Verifica se si deve effettuare riappianamento */
				if (OGainFactor >= 0)
					/* Rinormalizza a fase minima la componente EP inversa */
					CMPNormFlat(EPIPERemove,FS,(DLReal) 1.0,OGainFactor,-1);
			}
		else
			{
				/* Calcola la dimesione effettiva del filtro */
				if (PEFilterLen % 2 == 0)
					PEFilterLen--;

				/* Calcola la dimensione del blocco di uscita */
				EPPFOutSize = 2 * PELowerWindow + PEFilterLen - 1;

				/* Alloca gli array per la finestratura */
				if ((EPPFOut = new DLReal[EPPFOutSize]) == NULL)
					return False;

				/* Effettua la prefiltratura */
				sputs("Inverted EP sliding lowpass pre-echo windowing...");
				SLPreFilt(&EPIPERemove[FS - (PELowerWindow + EPSigLen / 2)],
					2 * PELowerWindow,2 * PEUpperWindow,PEFilterLen,
					PEBandSplit,PEWindowExponent,SampleRate,PEStartFreq,PEEndFreq,
					2 * PEUpperWindow,FSharpness,EPPFOut,WLeft,SLPType);

				/* Finestratura finale segnale prefiltrato */
				SpacedBlackmanWindow(&EPPFOut[EPPFOutSize / 2 - PELowerWindow],
					2 * PELowerWindow,2 * PEUpperWindow,WLeft);

				/* Ricompone la componente EP rimuovendo la zona di pre-echo */
				for (I = 0; I < FS - (PELowerWindow + EPSigLen / 2);I++)
					EPIPERemove[I] = 0;
				for (I = FS - (PELowerWindow + EPSigLen / 2),J = EPPFOutSize / 2 - PELowerWindow;J < EPPFOutSize / 2;I++,J++)
					EPIPERemove[I] = EPPFOut[J];

				/* Dealloca gli array prefiltratura */
				delete[] EPPFOut;

				/* Segnala il resto della procedura */
				sputs("Pre-echo truncation inversion completion...");

				/* Verifica se  si deve effettuare riappianamento */
				if (OGainFactor >= 0)
					/* Rinormalizza a fase minima la componente EP inversa */
					CMPNormFlat(EPIPERemove,FS,(DLReal) 1.0,OGainFactor,-1);
			}

		/* Alloca l'array per l'FFT componente EP */
		if ((FFTArray2 = new DLComplex[FS]) == NULL)
			return False;

		/* Copia l'array componente EP in quello per l'FFT */
		for (I = 0;I < FS;I++)
			FFTArray2[I] = EPIPERemove[I];

		/* Dealloca l'array componente EP */
		delete[] EPIPERemove;

		/* Porta nel dominio della frequenza */
		Fft(FFTArray2,FS);

		/* Alloca l'array per l'FFT componente MP */
		if ((FFTArray1 = new DLComplex[FS]) == NULL)
			return False;

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0;I < MPSigLen;I++)
			FFTArray1[I] = MPSig[I];

		/* Azzera la parte rimanente */
		for (I = MPSigLen;I < FS;I++)
			FFTArray1[I] = 0;

		/* Porta nel dominio della frequenza */
		Fft(FFTArray1,FS);

		/* Compone l'inverso componente MP e EP */
		for (I = 0;I < FS;I++)
			FFTArray1[I] = FFTArray2[I] / FFTArray1[I];

		/* Ritorna nel dominio del tempo */
		IFft(FFTArray1,FS);

		/* Estrae il filtro inverso */
		for (I = 0,J = (1 + FS + FS - (MPSigLen + EPSigLen + InvFilterLen) / 2) % FS;I < InvFilterLen;I++,J = (J + 1) % FS)
			InvFilter[I] = std::real<DLReal>(FFTArray1[J]);

		/* Dealloca gli array temporanei */
		delete[] FFTArray1;
		delete[] FFTArray2;

		/* Operazione completata */
		return True;
	}

/* Experimental, ignore it */

/* Inversione tramite selective minimum pahse fast deconvolution */
/* Versione a troncatura multipla degli effetti di pre echo
con componente EP di riferimento */
Boolean MRefPETFDInvert(const DLReal * MPSig, int MPSigLen, const DLReal * EPSig, int EPSigLen,
	DLReal * InvFilter,	int InvFilterLen, char PEType, int PELowerWindow, int PEUpperWindow,
	DLReal PEStartFreq, DLReal PEEndFreq, int PEFilterLen, DLReal FSharpness, int PEBandSplit,
	DLReal PEWindowExponent, const SLPPrefilteringType SLPType, const DLReal * EPRef, int EPRefLen,
	char RPEType, int RPELowerWindow,	int RPEUpperWindow,	DLReal RPEStartFreq, DLReal RPEEndFreq,
	int RPEFilterLen, DLReal RFSharpness,	int RPEBandSplit,	DLReal RPEWindowExponent,
	const SLPPrefilteringType RSLPType, const DLReal OGainFactor, int SampleRate,	int MExp)
	{
		/* Dimensione da considerare per il segnale */
		int SigLen;

		/* Dimensione FFT */
		int FS;

		/* Array FFT */
		DLComplex * FFTArray1;
		DLComplex * FFTArray2;

		/* Array rimozione pre-echo */
		DLReal * EPIPERemove;

		/* Componente EP di riferimento */
		DLComplex * EPFDRef;

		/* Array prefiltratura componente EP */
		DLReal * EPPFOut;

		/* Dimensione blocco di uscita filtratura */
		int EPPFOutSize;

		/* Indici generici */
		int I;
		int J;

		/* Determina la dimensione massima tra i vari segnali */
		if (MPSigLen > InvFilterLen)
			SigLen = MPSigLen;
		else
			SigLen = InvFilterLen;
		if (EPSigLen > SigLen)
			SigLen = EPSigLen;

		/* Controlla se si deve adottare una potenza di due */
		if (MExp >= 0)
			{
				/* Calcola la potenza di due superiore a N */
				for(FS = 1;FS <= SigLen;FS <<= 1);
				FS *= 1 << MExp;
			}
		else
			FS = SigLen;

		/* Alloca l'array per la componente EP */
		if ((EPIPERemove = new DLReal[FS]) == NULL)
			return False;

		/* Copia l'array sorgente in quello temporaneo in ordine inverso
		in modo da ottenere direttamente il filtro inverso */
		for (I = 0,J = FS - 1;I < EPSigLen;I++,J--)
			EPIPERemove[J] = EPSig[I];

		/* Azzera la parte rimanente */
		for (I = EPSigLen,J = 0;I < FS;I++,J++)
			EPIPERemove[J] = (DLReal) 0.0;

		/* Alloca l'array per l'FFT componente EP */
		if ((FFTArray2 = new DLComplex[FS]) == NULL)
			return False;

		/* Verifica se la rimozione pre-echo con riferimento è abilitata */
		if (RPEType != 'n')
			{
				/* Alloca l'array per la convoluzione con il riferimento */
				if ((EPFDRef = new DLComplex[FS]) == NULL)
					return False;

				/* Copia il riferimento allineandolo per ritardo zero */
				for (I = 0,J = FS - EPRefLen / 2;I < EPRefLen / 2;I++,J++)
					EPFDRef[J] = EPRef[I];
				for (I = EPRefLen / 2,J = 0;J < EPRefLen / 2;I++,J++)
					EPFDRef[J] = EPRef[I];

				/* Azzera la parte rimanente */
				for (I = EPRefLen / 2;I < FS - EPRefLen / 2;I++)
					EPFDRef[I] = 0;

				/* Porta nel dominio della frequenza */
				Fft(EPFDRef,FS);

				/* Copia l'array componente EP in quello per l'FFT */
				for (I = 0;I < FS;I++)
					FFTArray2[I] = EPIPERemove[I];

				/* Porta nel dominio della frequenza */
				Fft(FFTArray2,FS);

				/* Effettua la convoluzione tra il filtro inverso e il riferimento */
				for (I = 0;I < FS;I++)
					/* FFTArray2[I] *= std::polar<DLReal>(((DLReal) 1.0),
						std::arg(EPFDRef[I])); */
					FFTArray2[I] *= EPFDRef[I];

				/* Riporta nel dominio del tempo */
				IFft(FFTArray2,FS);

				/* Estrae la risposta nel tempo */
				for (I = 0; I < FS;I++)
					EPIPERemove[I] = std::real<DLReal>(FFTArray2[I]);

				/* Controlla il tipo di rimozione pre echo da adottare */
				if (RPEType == 'f')
					{
						/* Estrae la parte EP rimuovendo la zona di pre-echo */
						for (I = 0; I < FS - (RPELowerWindow + EPSigLen / 2);I++)
							EPIPERemove[I] = 0;
						SpacedBlackmanWindow(&EPIPERemove[FS - (RPELowerWindow + EPSigLen / 2)],
							RPELowerWindow + EPSigLen / 2,
							(RPELowerWindow + EPSigLen / 2) - 2 * (RPELowerWindow - RPEUpperWindow),WLeft);
					}
				else
					{
						/* Calcola la dimesione effettiva del filtro */
						if (RPEFilterLen % 2 == 0)
							RPEFilterLen--;

						/* Calcola la dimensione del blocco di uscita */
						EPPFOutSize = 2 * RPELowerWindow + RPEFilterLen - 1;

						/* Alloca gli array per la finestratura */
						if ((EPPFOut = new DLReal[EPPFOutSize]) == NULL)
							return False;

						/* Effettua la prefiltratura */
						sputs("Inverted EP sliding lowpass pre-echo windowing...");
						SLPreFilt(&EPIPERemove[FS - (RPELowerWindow + EPSigLen / 2)],
							2 * RPELowerWindow,2 * RPEUpperWindow,RPEFilterLen,
							RPEBandSplit,RPEWindowExponent,SampleRate,RPEStartFreq,RPEEndFreq,
							2 * RPEUpperWindow,RFSharpness,EPPFOut,WLeft,RSLPType);

						/* Finestratura finale segnale prefiltrato */
						SpacedBlackmanWindow(&EPPFOut[EPPFOutSize / 2 - RPELowerWindow],
							2 * RPELowerWindow,2 * RPEUpperWindow,WLeft);

						/* Estrae la parte EP rimuovendo la zona di pre-echo */
						for (I = 0; I < FS - (RPELowerWindow + EPSigLen / 2);I++)
							EPIPERemove[I] = 0;
						for (I = FS - (RPELowerWindow + EPSigLen / 2),J = EPPFOutSize / 2 - RPELowerWindow;J < EPPFOutSize / 2;I++,J++)
							EPIPERemove[I] = EPPFOut[J];

						/* Verifica se  si deve effettuare riappianamento */
						if (OGainFactor >= 0)
							/* Rinormalizza a fase minima la componente EP inversa */
							CMPNormFlat(EPIPERemove,FS,(DLReal) 1.0,OGainFactor,-1);

						/* Dealloca gli array prefiltratura */
						delete[] EPPFOut;
					}

				/* Copia l'array sorgente in quello temporaneo */
				for (I = 0;I < FS;I++)
					FFTArray2[I] = EPIPERemove[I];

				/* Porta nel dominio della frequenza */
				Fft(FFTArray2,FS);

				/* Rimuove per deconvoluzione il riferimento */
				for (I = 0;I < FS;I++)
					/*FFTArray2[I] /= EPFDRef[I];*/
					FFTArray2[I] *= std::polar<DLReal>(((DLReal) 1.0),
						-std::arg(EPFDRef[I]));

				/* Dealloca gli array temporanei */
				delete[] EPFDRef;

				/* Riporta nel dominio nel tempo */
				IFft(FFTArray2,FS);

				/* Estrae la risposta nel tempo */
				for (I = 0; I < FS;I++)
					EPIPERemove[I] = std::real<DLReal>(FFTArray2[I]);
			}

		/* Controlla il tipo di rimozione pre-echo da adottare */
		if (PEType == 'f')
			{
				/* Estrae la parte EP rimuovendo la zona di pre-echo */
				for (I = 0; I < FS - (PELowerWindow + EPSigLen / 2);I++)
					EPIPERemove[I] = 0;
				SpacedBlackmanWindow(&EPIPERemove[FS - (PELowerWindow + EPSigLen / 2)],
					PELowerWindow + EPSigLen / 2,
					(PELowerWindow + EPSigLen / 2) - 2 * (PELowerWindow - PEUpperWindow),WLeft);

				/* Verifica se si deve effettuare riappianamento */
				if (OGainFactor >= 0)
					/* Rinormalizza a fase minima la componente EP inversa */
					CMPNormFlat(EPIPERemove,FS,(DLReal) 1.0,OGainFactor,-1);
			}
		else
			{
				/* Calcola la dimesione effettiva del filtro */
				if (PEFilterLen % 2 == 0)
					PEFilterLen--;

				/* Calcola la dimensione del blocco di uscita */
				EPPFOutSize = 2 * PELowerWindow + PEFilterLen - 1;

				/* Alloca gli array per la finestratura */
				if ((EPPFOut = new DLReal[EPPFOutSize]) == NULL)
					return False;

				/* Effettua la prefiltratura */
				sputs("Inverted EP sliding lowpass pre-echo windowing...");
				SLPreFilt(&EPIPERemove[FS - (PELowerWindow + EPSigLen / 2)],
					2 * PELowerWindow,2 * PEUpperWindow,PEFilterLen,
					PEBandSplit,PEWindowExponent,SampleRate,PEStartFreq,PEEndFreq,
					2 * PEUpperWindow,FSharpness,EPPFOut,WLeft,SLPType);

				/* Finestratura finale segnale prefiltrato */
				SpacedBlackmanWindow(&EPPFOut[EPPFOutSize / 2 - PELowerWindow],
					2 * PELowerWindow,2 * PEUpperWindow,WLeft);

				/* Ricompone la componente EP rimuovendo la zona di pre-echo */
				for (I = 0; I < FS - (PELowerWindow + EPSigLen / 2);I++)
					EPIPERemove[I] = 0;
				for (I = FS - (PELowerWindow + EPSigLen / 2),J = EPPFOutSize / 2 - PELowerWindow;J < EPPFOutSize / 2;I++,J++)
					EPIPERemove[I] = EPPFOut[J];

				/* Dealloca gli array prefiltratura */
				delete[] EPPFOut;

				/* Segnala il resto della procedura */
				sputs("Pre-echo truncation inversion completion...");

				/* Verifica se  si deve effettuare riappianamento */
				if (OGainFactor >= 0)
					/* Rinormalizza a fase minima la componente EP inversa */
					CMPNormFlat(EPIPERemove,FS,(DLReal) 1.0,OGainFactor,-1);
			}

		/* Copia l'array componente EP in quello per l'FFT */
		for (I = 0;I < FS;I++)
			FFTArray2[I] = EPIPERemove[I];

		/* Dealloca l'array componente EP */
		delete[] EPIPERemove;

		/* Porta nel dominio della frequenza */
		Fft(FFTArray2,FS);

		/* Alloca l'array per l'FFT componente MP */
		if ((FFTArray1 = new DLComplex[FS]) == NULL)
			return False;

		/* Copia l'array sorgente in quello temporaneo */
		for (I = 0;I < MPSigLen;I++)
			FFTArray1[I] = MPSig[I];

		/* Azzera la parte rimanente */
		for (I = MPSigLen;I < FS;I++)
			FFTArray1[I] = 0;

		/* Porta nel dominio della frequenza */
		Fft(FFTArray1,FS);

		/* Compone l'inverso componente MP e EP */
		for (I = 0;I < FS;I++)
			FFTArray1[I] = FFTArray2[I] / FFTArray1[I];

		/* Ritorna nel dominio del tempo */
		IFft(FFTArray1,FS);

		/* Estrae il filtro inverso */
		for (I = 0,J = (1 + FS + FS - (MPSigLen + EPSigLen + InvFilterLen) / 2) % FS;I < InvFilterLen;I++,J = (J + 1) % FS)
			InvFilter[I] = std::real<DLReal>(FFTArray1[J]);

		/* Dealloca gli array temporanei */
		delete[] FFTArray1;
		delete[] FFTArray2;

		/* Operazione completata */
		return True;
	}
