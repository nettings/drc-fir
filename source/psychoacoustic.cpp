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

/* Inclusioni */
#include "psychoacoustic.h"
#include "fft.h"
#include "hd.h"
#include "baselib.h"
#include "dspwind.h"

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

/* Ritorna l'ampiezza di banda a frazioni di ottava
per una data frequenza */
DLReal FOctBWidth(const DLReal F,const DLReal FOW)
	{
		DLReal BW;

		BW = (DLReal) pow(2.0,FOW/2.0);

		return F * ((BW * BW) - 1) / BW;
	}

/* Versione della funzione precedente che opera su un
array di frequenze */
void AFOctBWidth(const DLReal * F, const int N,const DLReal FOW,DLReal * BW)
	{
		DLReal B1;
		int I;

		B1 = (DLReal) pow(2.0,FOW/2.0);
		B1 = ((B1 * B1) - 1) / B1;

		for (I = 0;I < N;I ++)
			BW[I] = F[I] * B1;
	}

/* Ritorna l'ampiezza di banda per una data frequenza
secondo la scala di Bark */
DLReal BarkBWidth(const DLReal F)
	{
		return (DLReal) (94 + 71 * (pow(F / 1000,1.5)));
	}

/* Versione della funzione precedente che opera su un
array di frequenze */
void ABarkBWidth(const DLReal * F, const int N,DLReal * BW)
	{
		int I;

		for (I = 0;I < N;I ++)
			BW[I] = (DLReal) (94 + 71 * (pow(F[I] / 1000,1.5)));
	}

/* Ritorna l'ampiezza di banda per una data frequenza
secondo la scala ERB */
DLReal ERBBWidth(const DLReal F)
	{
		return ((DLReal) 0.108) * F + ((DLReal) 24.7);
	}

/* Versione della funzione precedente che opera su un
array di frequenze */
void AERBBWidth(const DLReal * F, const int N,DLReal * BW)
	{
		int I;

		for (I = 0;I < N;I ++)
			BW[I] = ((DLReal) 0.108) * F[I] + ((DLReal) 24.7);
	}

/* Calcola la somma cumnulativa sull'array A.
Se è l'array S non è null la somma cumulativa viene posta
nell'array S, altrimenti viene posta direttamente in A
sostituendo i valori contenuti */
void CumulativeSum(DLReal * A,const int N,DLReal * S)
	{
		DLReal * CS;
		DLReal CV;
		DLReal SC;
		DLReal SY;
		DLReal ST;
		int I;

		if (S == NULL)
			CS = A;
		else
			CS = S;

		/* Kahan summation algorithm */
		CV = (DLReal) 0.0;
		SC = (DLReal) 0.0;
		for (I = 0;I < N;I ++)
			{
				SY = A[I] - SC;
				ST = CV + SY;
				SC = (ST - CV) - SY;
				CS[I] = CV = ST;
			}
	}

/* Tipo per le elaborazioni interne dell'inviluppo spettrale */
/* Il calcolo dell'inviluppo spettrale richiede una elevata precisione
in particolare nel caso in cui il fattore di approssimazione dei picchi
sia molto elevato (> 30) */
typedef long double SEFloat;

/* Somma cumulativa per l'inviluppo spettrale */
static void SECumulativeSum(SEFloat * A,const int N,SEFloat * S)
	{
		SEFloat * CS;
		SEFloat CV;
		SEFloat SC;
		SEFloat SY;
		SEFloat ST;
		int I;

		if (S == NULL)
			CS = A;
		else
			CS = S;

		/* Kahan summation algorithm */
		CV = (SEFloat) 0.0;
		SC = (SEFloat) 0.0;
		for (I = 0;I < N;I ++)
			{
				SY = A[I] - SC;
				ST = CV + SY;
				SC = (ST - CV) - SY;
				CS[I] = CV = ST;
			}
	}

/* Funzione per il calcolo dell'esponenziale nella procedura
di estrazione dell'inviluppo spettrale */
#define SEPow(x,y) powl(x,y)

/* Calcola l'inviluppo spettrale del segnale S con approssimazione
dei picchi PDS e risoluzione BW, espressa in frazioni di ottava.
Nel caso in cui PDS sia pari a 1.0 esegue il classico smoothing
a frazioni di ottava. Nel caso in cui BW sia minore di 0.0 usa la
scala di Bark, nel caso in cui BW sia minore di -1.0 usa la
scala ERB */
Boolean SpectralEnvelope(const DLReal * S,const int N,const int FS,
	const DLReal BW,const DLReal PDS,DLReal * SE)
	{
		DLComplex * CSP;
		SEFloat * SP;
		SEFloat * TSP;
		SEFloat NF;
		DLReal BWF;
		int I;
		int J;
		int * BU;
		int * BL;

		/* Alloca l'array per l'estrazione dello spettro */
		if ((CSP = new DLComplex[2 * N]) == NULL)
			return False;

		/* Effettua il padding del segnale */
		for (I = 0;I < N;I++)
			CSP[I] = S[I];
		for (I = N;I < 2 * N;I++)
			CSP[I] = (DLReal) 0.0;

		/* Calcola lo spettro del asegnale */
		if (Fft(CSP,2 * N) == False)
			{
				delete[] CSP;
				return False;
			}

		/* Alloca l'array per l'estrazione dell'inviluppo */
		if ((SP = new SEFloat[N + 1]) == NULL)
			{
				delete[] CSP;
				return False;
			}

		/* Estrae l'ampiezza del segnale */
		SP[0] = (SEFloat) 0.0;
		for (I = 0,J = 1;I < N;I++,J++)
			SP[J] = std::abs<DLReal>(CSP[I]);

		/* Normalizza l'ampiezza */
		NF = (SEFloat) 0.0;
		for (I = 0;I < N + 1;I++)
			{
				if (SP[I] > NF)
					NF = SP[I];
			}
		if (NF > (SEFloat) 0.0)
			{
				for (I = 0;I < N + 1;I++)
					SP[I] /= NF;
			}

		/* Dealloca l'array per l'estrazione dello spettro */
		delete[] CSP;

		/* Alloca gli array per gli intervalli estrazione inviluppo */
		if ((BU = new int[N]) == NULL)
			{
				delete[] SP;
				return False;
			}
		if ((BL = new int[N]) == NULL)
			{
				delete[] BU;
				delete[] SP;
				return False;
			}

		/* Calcola gli intervalli calcolo inviluppo */
		if (BW >= ((DLReal) 0.0))
			{
				/* Risoluzione a frazioni di ottava */
				BWF = (DLReal) pow(2.0,BW / 2.0);

				/* Calcolo intervalli */
				for (I = 0;I < N;I++)
					{
						BU[I] = (int) floor(1.5 + (I * BWF));
						if (BU[I] > N)
							BU[I] = N;
						BL[I] = (int) floor(0.5 + (I / BWF));
					}
			}
		else
			if (BW >= ((DLReal) -1.0))
				{
					/* Scala di Bark */
					for (I = 0;I < N;I++)
						SE[I] = (((DLReal) 0.5) * I * FS) / (N - 1);
					ABarkBWidth(SE,N,SE);

					/* Imposta il primo punto */
					BL[0] = 0;
					BU[0] = 1;

					/* Calcolo intervalli */
					for (I = 1;I < N;I++)
						{
							BWF = (((DLReal) 0.5) * I * FS) / (N - 1);
							BWF = (DLReal) ((sqrt(SE[I] * SE[I] + 4 * BWF * BWF) + SE[I]) / (2 * BWF));
							BU[I] = (int) floor(1.5 + (I * BWF));
							if (BU[I] > N)
								BU[I] = N;
							BL[I] = (int) floor(0.5 + (I / BWF));
						}
				}
			else
				{
					/* Scala ERB */
					for (I = 0;I < N;I++)
						SE[I] = (((DLReal) 0.5) * I * FS) / (N - 1);
					AERBBWidth(SE,N,SE);

					/* Imposta il primo punto */
					BL[0] = 0;
					BU[0] = 1;

					/* Calcolo intervalli */
					for (I = 1;I < N;I++)
						{
							BWF = (((DLReal) 0.5) * I * FS) / (N - 1);
							BWF = (DLReal) ((sqrt(SE[I] * SE[I] + 4 * BWF * BWF) + SE[I]) / (2 * BWF));
							BU[I] = (int) floor(1.5 + (I * BWF));
							if (BU[I] > N)
								BU[I] = N;
							BL[I] = (int) floor(0.5 + (I / BWF));
						}
				}

		/* Alloca l'array di supporto per l'estrazione dell'inviluppo */
		if ((TSP = new SEFloat[N + 1]) == NULL)
			{
				delete[] BU;
				delete[] BL;
				delete[] SP;
				return False;
			}

		/* Verifica se è abilitata l'approssimazione dei picchi */
		if (PDS <= (DLReal) 1.0)
			{
				/* Smoothing tradizionale */
				SECumulativeSum(SP,N + 1,NULL);
				TSP[0] = (SEFloat) 0.0;
				for (I = 0,J = 1;I < N;I++,J++)
					TSP[J] = (SP[BU[I]] - SP[BL[I]]) / (BU[I] - BL[I]);
			}
		else
			{
				/* Smoothing con approssimazione picchi */
				for (I = 0;I < N + 1;I++)
					SP[I] = (SEFloat) SEPow(SP[I],PDS);
				SECumulativeSum(SP,N + 1,NULL);
				BWF = ((DLReal) 1.0) / PDS;
				TSP[0] = (SEFloat) 0.0;
				for (I = 0,J = 1;I < N;I++,J++)
					TSP[J] = (SEFloat) SEPow((SP[BU[I]] - SP[BL[I]]) / (BU[I] - BL[I]),BWF);

				/* Interpolazione picchi */
				SECumulativeSum(TSP,N + 1,NULL);
				SP[0] = (SEFloat) 0.0;
				for (I = 0,J = 1;I < N;I++,J++)
					SP[J] = (TSP[BU[I]] - TSP[BL[I]]) / (BU[I] - BL[I]);

				for (I = 0;I < N + 1;I++)
					TSP[I] = SP[I];
			}

		/* Smoothing finale */
		SECumulativeSum(TSP,N + 1,NULL);
		for (I = 0;I < N;I++)
			SE[I] = (DLReal) (NF * ((TSP[BU[I]] - TSP[BL[I]]) / (BU[I] - BL[I])));

		/* Dealloca gli array temporanei */
		delete[] BL;
		delete[] BU;
		delete[] TSP;
		delete[] SP;

		/* Operazione completata */
		return True;
	}

/* Calola il valore RMS dell'inviluppo spettrale sulla banda di frequenze indicate */
static DLReal SEGetBLFFTRMSLevel(const DLReal * SEArray,const int SESize,const int SampleFreq,
	const DLReal StartFreq,const DLReal EndFreq)
	{
		DLReal RMS;
		int I;
		int FS;
		int FE;

		/* Determina gli indici per il calcolo del valore RMS */
		FS = (int) floor(((2 * SESize * StartFreq) / SampleFreq));
		FE = (int) floor(((2 * SESize * EndFreq) / SampleFreq));

		/* Calcola il livello RMS */
		RMS = (DLReal) 0.0;
		for (I = FS;I < FE;I++)
			RMS += SEArray[I] * SEArray[I];

		return (DLReal) sqrt(RMS / SESize);
	}

/* Limitazione valli per inviluppo spettrale con calcolo del valore RMS sull banda indicata */
static void C1SEDipLimit(DLReal * SE,const int SESize,const DLReal MinGain,const DLReal DLStart,
	const int SampleFreq,const DLReal StartFreq,const DLReal EndFreq)
	{
		int I;
		DLReal RMSLevel;
		DLReal DLSLevel;
		DLReal DLLevel;
		DLReal DLGFactor;
		DLReal DLAbs;

		/* Determina il livello RMS del segnale */
		RMSLevel = MinGain * SEGetBLFFTRMSLevel(SE,SESize,SampleFreq,StartFreq,EndFreq);

		/* Verifica il tipo di limitazione impostata */
		if (DLStart >= (DLReal) 1.0)
			{
				/* Scansione per troncatura guadagno */
				for (I = 0; I < SESize; I++)
					{
						DLAbs = SE[I];
						if (DLAbs < RMSLevel)
							SE[I] = RMSLevel;
					}
			}
		else
			{
				/* Determina i fattori per la limitazione guadagno */
				DLSLevel = RMSLevel / DLStart;
				DLGFactor = DLSLevel - RMSLevel;

				/* Scansione per limitazione guadagno */
				for (I = 0; I < SESize; I++)
					{
						DLAbs = SE[I];
						if (DLAbs < DLSLevel)
							{
								/* Riassegna il guadagno del filtro */
								DLLevel = (DLSLevel - DLAbs) / DLGFactor;
								DLLevel = DLLevel / (((DLReal) 1.0) + DLLevel);
								SE[I] = DLSLevel - DLGFactor * DLLevel;
							}
					}
			}
	}

/* Calcola un filtro target basato sull'inviluppo spettrale. Il filtro
ha lunghezza pari a 2 volte la lunghezza del segnale in ingresso e non è
finestrato */
Boolean MKSETargetFilter(const DLReal * S,const int N,const int FS,
	const DLReal BW,const DLReal PDS,DLReal * TF,const MKSETFType TFType,
	const DLReal MinGain,const DLReal DLStart,const int SampleFreq,
	const DLReal StartFreq,const DLReal EndFreq)
	{
		DLComplex * CSE;
		int I;
		int J;
		Boolean LogLimit;
		DLReal CV;

		/* Calcola l'inviluppo spettrale */
		if (SpectralEnvelope(S,N,FS,BW,PDS,TF) == False)
			return False;

		/* Effettua la limitazione valli sull'inviluppo spettrale */
		C1SEDipLimit(TF,N,MinGain,DLStart,SampleFreq,StartFreq,EndFreq);

		/* Alloca l'array per il calcolo del filtro target */
		if ((CSE = new DLComplex[2 * N]) == NULL)
			return False;

		/* Verifica il tipo di filtro impostato */
		switch (TFType)
			{
				/* Fase lineare */
				case MKSETFLinearPhase:
					/* Imposta la risposta in ampiezza */
					for (I = 0,J = (2 * N) - 1;I < N;I++,J--)
						CSE[J] = -(CSE[I] = ((DLReal) 1.0) / TF[I]);
					for (I = 0;I < 2 * N;I++)
						CSE[I] *= UnitRoot(I,4 * N);

					/* Calcola il filtro */
					if (IFft(CSE,2 * N) == False)
						{
							delete[] CSE;
							return False;
						}

					/* Estrae il filtro a fase lineare */
					for (I = 0,J = N;I < N;I++,J++)
						{
							TF[I] = std::real<DLReal>(CSE[N - (I + 1)]);
							TF[J] = std::real<DLReal>(CSE[I]);
						}

					/* Dealloca l'array per il calcolo filtro target */
					delete[] CSE;
				break;

				/* Fase minima */
				case MKSETFMinimumPhase:
					/* Calcola i valori per il cepstrum */
					LogLimit = False;
					for (I = 0,J = (2 * N) - 1;I < N;I++,J--)
						{
							CV = TF[I];
							if (CV <= (DLReal) 0.0)
								{
									LogLimit = True;
									CSE[I] = CSE[J] = (DLReal) -log(DRCMinFloat);
								}
							else
								CSE[I] = CSE[J] = -std::log<DLReal>(CV);
						}

					/* Verifica se si è raggiunto il limite */
					if (LogLimit == True)
						sputs("Notice: log limit reached in cepstrum computation.");

					/* Calcola il cepstrum */
					IFft(CSE,2 * N);

					/* Finestra il cepstrum */
					for (I = 1; I < N;I++)
						CSE[I] *= 2;
					for (I = N + 1; I < 2 * N;I++)
						CSE[I] = 0;

					/* Calcola la trsformata del cepstrum finestrato */
					Fft(CSE,2 * N);

					/* Effettua il calcolo dell'esponenziale */
					for (I = 0;I < 2 * N;I++)
						CSE[I] = std::exp<DLReal>(CSE[I]);

					/* Determina la risposta del sistema a fase minima */
					IFft(CSE,2 * N);

					/* Copia il risultato nell'array destinazione */
					for (I = 0;I < 2 * N;I++)
						TF[I] = std::real<DLReal>(CSE[I]);

					/* Dealloca l'array per il calcolo filtro target */
					delete[] CSE;
				break;
			}

		/* Operazione completata */
		return True;
	}

/* Versione della funzione precedente che effettua un padding del segnale
in ingresso alla prima potenza di due disponibile. Il filtro in uscita ha
lunghezza pari a TFN. TFN non può essere superiore alla lunghezza usata
internamente per il calcolo del filtro, quindi 2 * N se  MExp < 0, oppure
2 * nextpow2(N) * 2 ^ MExp per MExp >= 0 */
Boolean P2MKSETargetFilter(const DLReal * S,const int N,const int FS,
	const DLReal BW,const DLReal PDS,DLReal * TF,const MKSETFType TFType,
	const int MExp,const int TFN,	const DLReal MinGain,const DLReal DLStart,
	const int SampleFreq,const DLReal StartFreq, const DLReal EndFreq)
	{
		DLReal * PS;
		DLReal * PTF;
		int I;
		int J;
		int PN;

		/* Controlla se si deve adottare una potenza di due */
		if (MExp >= 0)
			{
				/* Calcola la potenza di due superiore a N */
				for (PN = 1;PN <= N;PN <<= 1);
				PN *= 1 << MExp;
			}
		else
			PN = N;

		/* Controlla che la lunghezza richiesta sia valida */
		if (TFN > 2 * PN)
			return False;

		/* Alloca l'array per il padding del segnale */
		if ((PS = new DLReal[PN]) == NULL)
			return False;

		/* Alloca l'array per il filtro target */
		if ((PTF = new DLReal[2 * PN]) == NULL)
			{
				delete[] PS;
				return False;
			}

		/* Effettua il padding del segnale */
		for (I = 0;I < N;I++)
			PS[I] = S[I];
		for (I = N;I < PN;I++)
			PS[I] = (DLReal) 0.0;

		/* Calcola il filtro target */
		if (MKSETargetFilter(PS,PN,FS,BW,PDS,PTF,TFType,MinGain,DLStart,SampleFreq,StartFreq,EndFreq) == False)
			{
				delete[] PS;
				delete[] PTF;
				return False;
			}

		/* Verifica il tipo di filtro impostato */
		switch (TFType)
			{
				/* Fase lineare */
				case MKSETFLinearPhase:
					/* Effettua la finestratura */
					BlackmanWindow(&PTF[(2 * PN - TFN) / 2],TFN);

					/* Copia il filtro destinazione */
					for (I = 0,J = (2 * PN - TFN) / 2;I < TFN;I++,J++)
						TF[I] = PTF[J];
				break;

				/* Fase minima */
				case MKSETFMinimumPhase:
					/* Effettua la finestratura */
					HalfBlackmanWindow(PTF,TFN,0,WRight);

					/* Copia il filtro destinazione */
					for (I = 0;I < TFN;I++)
						TF[I] = PTF[I];
				break;
			}

		/* Dealloca gli array temporanei */
		delete[] PS;
		delete[] PTF;

		/* Operazione completata */
		return True;
	}
