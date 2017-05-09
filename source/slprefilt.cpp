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

/* Prefiltratura a bande tramite sliding lowpass di un segnale */

#include <stdio.h>
#include "slprefilt.h"
#include "level.h"
#include "baselib.h"

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

/* Calcolo filtro passa basso finestrato Blackman */
/* Il calcolo viene effettuato usando una ricorsione trigonometrica
per migliorare le prestazioni di generazione */
/* La finestratura viene generata solo per metà */
void FastLowPassFir(DLReal * Filter,const unsigned int Order,const DLReal Freq,
	DLReal * Window,const unsigned int WSize)
	{
		/* Variabili per le ricorsioni trigonometriche */
		DLReal SR1,SS1,SV1;
		DLReal CR1,CS1,CV1;
		DLReal CR2,CS2,CV2;

		/* Indici creazione filtro e finestra */
		unsigned int I,J,K;
		unsigned int HalfOrder = Order / 2;

		/* Periodo filtro */
		DLReal C = (DLReal) (((DLReal) M_PI) * Freq);

		/* Valore attuale finestra */
		DLReal WV;

		/* Periodo finestratura */
		DLReal C1;
		DLReal C2;

		/* Verifica che non si tratti del filtro banale */
		if (Order == 1)
			{
				Filter[0] = (DLReal) 1.0;
				return;
			}

		/* Verifica se deve essere calcolata la finestratura */
		if (WSize > 0)
			{
				/* Finestratura già presente, procede con la creazione del filtro */

				/* Verifica l'ordine del filtro */
				if (Order%2 == 0)
					{
						/* Inizializza le variabili per il calcolo della ricorsione
						per il filtro */

						/* R = sin(sd/2); */
						SR1 = (DLReal) DLSin(((DLReal) 0.5) * C);

						/* S = sin(sv); */
						/* S = sin(sd) * cos(sv) + (S * (1 - cos(sd))); */
						SS1 = ((DLReal) DLSin(C)) * ((DLReal) DLCos(((DLReal) 0.5) * C)) +
							(SR1 * (((DLReal) 1.0) - ((DLReal) DLCos(C))));

						/* V = sin(sv); */
						SV1 = SR1;

						/* R = -4 * R * R; */
						SR1 = ((DLReal) -4.0) * SR1 * SR1;

						/* Ciclo creazione filtro */
						for(I = 1,J = HalfOrder - 1,K = HalfOrder; I <= HalfOrder;I++,J--,K++)
							{
								/* Calcola e assegna il valore per il filtro */
								Filter[J] = Filter[K] = (DLReal) (Window[J] * SV1 /
									((I - ((DLReal) 0.5)) * ((DLReal) M_PI)));

								/* Calcola il valore successivo filtro per ricorsione */
								SS1 += SR1 * SV1;
								SV1 += SS1;
							}
					}
				else
					{
						/* Inizializza le variabili per il calcolo della ricorsione
						per il filtro */

						/* S = sin(sv); */
						SS1 = ((DLReal) DLSin(C));

						/* V = sin(sv); */
						SV1 = SS1;

						/* S = sin(sd) * cos(sv) + (S * (1 - cos(sd))); */
						SR1 = ((DLReal) DLCos(C));
						SS1 = SS1 * SR1 + (SS1 * (((DLReal) 1.0) - SR1));

						/* R = sin(sd/2); */
						/* R = -4 * R * R; */
						SR1 = (DLReal) DLSin(((DLReal) 0.5) * C);
						SR1 = ((DLReal) -4.0) * SR1 * SR1;

						/* Ciclo creazione filtro */
						for(I = 1,J = HalfOrder - 1,K = HalfOrder + 1; I <= HalfOrder;I++,J--,K++)
							{
								/* Calcola e assegna il valore per il filtro */
								Filter[J] = Filter[K] = (DLReal) (Window[J] * SV1 / (I * ((DLReal) M_PI)));

								/* Calcola il valore successivo filtro per ricorsione */
								SS1 += SR1 * SV1;
								SV1 += SS1;
							}

						/* Assegna il valore centrale */
						Filter[HalfOrder] = Freq;
					}
			}
		else
			{
				/* Finestratura da calcolare, inizializza il periodo */
				C1 = (DLReal) (2 * ((DLReal) M_PI) / (Order - 1));
				C2 = (DLReal) (4 * ((DLReal) M_PI) / (Order - 1));

				/* Verifica l'ordine del filtro */
				if (Order%2 == 0)
					{
						/* Inizializza le variabili per il calcolo della ricorsione
						per la finestratura */

						/* R = sin(cd/2); */
						/* R = -4 * R * R; */
						CR1 = (DLReal) DLSin(((DLReal) 0.5) * C1);
						CR1 = ((DLReal) -4.0) * CR1 * CR1;

						/* S = cos(cv); */
						CS1 = (DLReal) DLCos(HalfOrder * C1);

						/* S = (S * (1 - cos(cd))) - sin(cv) * sin(cd); */
						CS1 = (CS1 * (((DLReal) 1.0) - ((DLReal) DLCos(C1)))) -
							((DLReal) DLSin(HalfOrder * C1)) * ((DLReal) DLSin(C1));

						/* V = cos(cv); */
						CV1 = (DLReal) DLCos(HalfOrder * C1);

						/* R = sin(cd/2); */
						/* R = -4 * R * R; */
						CR2 = (DLReal) DLSin(((DLReal) 0.5) * C2);
						CR2 = ((DLReal) -4.0) * CR2 * CR2;

						/* S = cos(cv); */
						CS2 = (DLReal) DLCos(HalfOrder * C2);

						/* S = (S * (1 - cos(cd))) - sin(cv) * sin(cd); */
						CS2 = (CS2 * (((DLReal) 1.0) - ((DLReal) DLCos(C2)))) -
							((DLReal) DLSin(HalfOrder * C2)) * ((DLReal) DLSin(C2));

						/* V = cos(cv); */
						CV2 = (DLReal) DLCos(HalfOrder * C2);

						/* Inizializza le variabili per il calcolo della ricorsione
						per il filtro */

						/* R = sin(sd/2); */
						SR1 = (DLReal) DLSin(((DLReal) 0.5) * C);

						/* S = sin(sv); */
						/* S = sin(sd) * cos(sv) + (S * (1 - cos(sd))); */
						SS1 = ((DLReal) DLSin(C)) * ((DLReal) DLCos(((DLReal) 0.5) * C)) +
							(SR1 * (((DLReal) 1.0) - ((DLReal) DLCos(C))));

						/* V = sin(sv); */
						SV1 = SR1;

						/* R = -4 * R * R; */
						SR1 = ((DLReal) -4.0) * SR1 * SR1;

						/* Ciclo creazione filtro */
						for(I = 1,J = HalfOrder - 1,K = HalfOrder; I <= HalfOrder;I++,J--,K++)
							{
								/* Calcola e assegna il valore della finestra */
								Window[J] = WV = (DLReal) (((DLReal) 0.42) - ((DLReal) 0.5) * CV1 +
									((DLReal) 0.08) * CV2);

								/* Calcola e assegna il valore per il filtro */
								Filter[J] = Filter[K] = (DLReal) (WV * SV1 / ((I - ((DLReal) 0.5)) * ((DLReal) M_PI)));

								/* Calcola il valore successivo filtro per ricorsione */
								SS1 += SR1 * SV1;
								SV1 += SS1;

								/* Calcola il valore successivo finestra per ricorsione */
								CS1 += CR1 * CV1;
								CV1 += CS1;
								CS2 += CR2 * CV2;
								CV2 += CS2;
							}
					}
				else
					{
						/* Inizializza le variabili per il calcolo della ricorsione
						per la finestratura */

						/* R = sin(cd/2); */
						/* R = -4 * R * R; */
						CR1 = (DLReal) DLSin(((DLReal) 0.5) * C1);
						CR1 = ((DLReal) -4.0) * CR1 * CR1;

						/* S = cos(cv); */
						CS1 = (DLReal) DLCos((HalfOrder + 1) * C1);

						/* S = (S * (1 - cos(cd))) - sin(cv) * sin(cd); */
						CS1 = (CS1 * (((DLReal) 1.0) - ((DLReal) DLCos(C1)))) -
							((DLReal) DLSin((HalfOrder + 1) * C1)) * ((DLReal) DLSin(C1));

						/* V = cos(cv); */
						CV1 = (DLReal) DLCos((HalfOrder + 1) * C1);

						/* R = sin(cd/2); */
						/* R = -4 * R * R; */
						CR2 = (DLReal) DLSin(((DLReal) 0.5) * C2);
						CR2 = ((DLReal) -4.0) * CR2 * CR2;

						/* S = cos(cv); */
						CS2 = (DLReal) DLCos((HalfOrder + 1) * C2);

						/* S = (S * (1 - cos(cd))) - sin(cv) * sin(cd); */
						CS2 = (CS2 * (((DLReal) 1.0) - ((DLReal) DLCos(C2)))) -
							((DLReal) DLSin((HalfOrder + 1) * C2)) * ((DLReal) DLSin(C2));

						/* V = cos(cv); */
						CV2 = (DLReal) DLCos((HalfOrder + 1) * C2);

						/* Inizializza le variabili per il calcolo della ricorsione
						per il filtro */

						/* S = sin(sv); */
						SS1 = ((DLReal) DLSin(C));

						/* V = sin(sv); */
						SV1 = SS1;

						/* S = sin(sd) * cos(sv) + (S * (1 - cos(sd))); */
						SR1 = ((DLReal) DLCos(C));
						SS1 = SS1 * SR1 + (SS1 * (((DLReal) 1.0) - SR1));

						/* R = sin(sd/2); */
						/* R = -4 * R * R; */
						SR1 = (DLReal) DLSin(((DLReal) 0.5) * C);
						SR1 = ((DLReal) -4.0) * SR1 * SR1;

						/* Ciclo creazione filtro */
						for(I = 1,J = HalfOrder - 1,K = HalfOrder + 1; I <= HalfOrder;I++,J--,K++)
							{
								/* Calcola e assegna il valore della finestra */
								Window[J] = WV = (DLReal) (((DLReal) 0.42) - ((DLReal) 0.5) * CV1 +
									((DLReal) 0.08) * CV2);

								/* Calcola e assegna il valore per il filtro */
								Filter[J] = Filter[K] = (DLReal) (WV * SV1 / (I * ((DLReal) M_PI)));

								/* Calcola il valore successivo filtro per ricorsione */
								SS1 += SR1 * SV1;
								SV1 += SS1;

								/* Calcola il valore successivo finestra per ricorsione */
								CS1 += CR1 * CV1;
								CV1 += CS1;
								CS2 += CR2 * CV2;
								CV2 += CS2;
							}

						/* Assegna il valore centrale del filtro */
						Filter[HalfOrder] = Freq;
					}
			}
	}

/* Prefiltratura a bande tramite sliding lowpass di un segnale */
void SLPreFilt(DLReal * InImp, const int IBS, const int FBS,
	const int FilterLen, const int BandSplit, const DLReal WindowExponent,
	const int SampleFreq, const DLReal StartFreq, const DLReal EndFreq,
	int WindowGap, DLReal FSharpness, DLReal * OutImp,
	const WindowType WType, const SLPPrefilteringType SLPType)
	{
		/* Array filtro FIR */
		DLReal * FIRFilter;

		/* Array finestratura filtro */
		DLReal * FWin;

		/* Dimensione effettiva filtro FIR */
		int EFL, HEFL;

		/* Dimensione corrente filtro FIR */
		int CFL;
		int CHFL;
		int CFLM;

		/* Dimensione attuale finestra */
		int CWL;

		/* Dimensione effettiva finestra */
		int EWL;

		/* Indici convoluzione */
		int I,J,IS,FS,EI;

		/* Dimensioni blocco in uscita */
		int NR,HNR;

		/* Posizione semiblocchi destro e sinistro */
		int OCP;

		/* Numero banda corrente */
		int Band;

		/* Frequenza di taglio. */
		DLReal BCut;

		/* Larghezza banda */
		DLReal BWidth;

		/* Inizio e fine filtratura */
		DLReal FilterBegin;
		DLReal FilterEnd;

		/* Coefficienti calcolo frequenza di taglio su finestra */
		DLReal A;
		DLReal Q;
		DLReal B;
		int HFBS;
		int HIBS;

		/* Sommatoria filtratura */
		DLReal Sum;

		/* Variabili per il Kahan summation algorithm */
		DLReal SC;
		DLReal SY;
		DLReal ST;

		/* Calcola inizio e fine della prefiltratura */
		FilterBegin = (2 * StartFreq) / SampleFreq;
		FilterEnd = (2 * EndFreq) / SampleFreq;

		/* Calcola la dimensione effettiva filtro FIR */
		if (FilterLen % 2 == 0)
			{
				EFL = FilterLen - 1;

				/* Azzera l'ultimo campione del blocco in uscita in quanto non utilizzato */
				OutImp[(IBS + FilterLen - 1) - 1] = 0;
			}
		else
			EFL = FilterLen;
		HEFL = EFL / 2;

		/* Calcola la dimensione del blocco in uscita */
		NR = IBS + EFL - 1;

		/* Calcola il centro dei semiblocchi */
		HNR = NR / 2;

		/* Alloca gli array temporanei */
		FIRFilter = new DLReal[EFL];
		FWin = new DLReal[HEFL];

		/* Imposta i parametri iniziali per la visualizzazione */
		BWidth = (DLReal) pow(2,1.0/BandSplit);
		Band = 0;

		/* Calcola le dimensioni finestratura effettive */
		HFBS = FBS / 2;
		HIBS = IBS / 2;

		/* Effettua la finestratura iniziale del segnale in ingresso */
		sputs("Input signal prewindowing.");
		SpacedBlackmanWindow(InImp,IBS,WindowGap,WType);

		/* Verifica il tipo di curva di prefiltratura */
		switch (SLPType)
			{
				/* Proporzionale */
				case SLPProportional:
					/* Calcola i coefficienti per il calcolo finestratura */
					B = (DLReal) exp(log(FilterBegin / FilterEnd) / WindowExponent);
					Q = (DLReal) ((B * HIBS - HFBS) / (1.0 - B));
					A = (DLReal) (1.0 / (FilterEnd * pow(HFBS + Q,WindowExponent)));
				break;

				/* Bilineare */
				case SLPBilinear:
					A = (DLReal) (HIBS - HFBS);
					B = (FilterEnd - FilterBegin);
					Q = (DLReal) ((pow(WindowExponent,4.0) * (FilterBegin / FilterEnd)) - 1.0);
				break;
			}

		/* Finestratura sinistra */
		if ((WType == WLeft) || (WType == WFull))
			{
				/* Prepara gli indici per la convoluzione */
				EWL = (int) floor(FSharpness * HNR);
				CHFL = HEFL;

				/* Verifica che il filtro stia all'interno della finestratura corrente */
				if (CHFL >= EWL)
					{
						/* Ricalcola la dimensione del filtro */
						if (EWL > 1)
							CHFL = EWL - 1;
						else
							CHFL = 1;
						CFL	= 1 + (CHFL * 2);
					}
				else
					/* Usa la dimensione base */
					CFL = EFL;

				/* Ricalcola il filtro e la finestra */
				FastLowPassFir(FIRFilter,CFL,FilterBegin,FWin,0);

				/* Segnala lo stato */
				sputs("L - Initial lowpass convolution...");

				/* Ciclo di convoluzione lowpass iniziale */
				for(I = 0,CWL = HNR,OCP = 0;I < (HNR - HIBS);I++,CWL--,OCP++)
					{
						/* Calcola la finestratura effettiva corrente */
						EWL = (int) floor(FSharpness * CWL);

						/* Verifica che il filtro stia all'interno della finestratura corrente */
						if (CHFL >= EWL)
							{
								/* Ricalcola la dimensione del filtro */
								if (EWL > 1)
									CHFL = EWL - 1;
								else
									CHFL = 1;
								CFL	= 1 + (CHFL * 2);

								/* Ricalcola il filtro e la finestra */
								FastLowPassFir(FIRFilter,CFL,FilterBegin,FWin,0);
							}

						/* Verifica se si può utilizzare il filtro completo */
						if (OCP - CHFL >= HEFL)
							{
								/* Imposta gli indici per il filtro completo */
								CFLM = CFL;
								IS = OCP - (CHFL + HEFL);
								FS = 0;
							}
						else
							{
								/* Imposta gli indici per il filtro parziale */
								CFLM = CFL;
								IS = 0;
								FS = (CHFL + HEFL) - OCP;
							}

						/* Esegue la convoluzione, Kahan summation algorithm */
						Sum = (DLReal) 0.0;
						SC = (DLReal) 0.0;
						for (J = FS,EI = IS;J < CFLM;J++,EI++)
							{
								SY = (InImp[EI] * FIRFilter[J]) - SC;
								ST = Sum + SY;
								SC = (ST - Sum) - SY;
								Sum = ST;
							}
						OutImp[OCP] = Sum;
					}

				/* Imposta i parametri iniziali per la visualizzazione */
				Band = 0;

				/* Ciclo di convoluzione sliding lowpass */
				for(I = HNR - HIBS,CWL = HIBS,OCP = HNR - HIBS;I < (HNR - HFBS);I++,CWL--,OCP++)
					{
						/* Calcola la finestratura effettiva corrente */
						EWL = (int) floor(FSharpness * CWL);

						/* Verifica il tipo di curva di prefiltratura */
						switch (SLPType)
							{
								/* Proporzionale */
								case SLPProportional:
									/* Calcola la frequenza di taglio */
									BCut = (DLReal) 1.0 / (A * pow(CWL + Q,WindowExponent));
								break;

								/* Bilineare */
								case SLPBilinear:
									/* Calcola la frequenza di taglio */
									BCut = (DLReal) FilterBegin + (B * (((HIBS - CWL) / A) * (1 + Q)) /
										(1 + ((HIBS - CWL) / A) * Q));
								break;
							}

						/* Verifica che il filtro stia all'interno della finestratura corrente */
						if (CHFL >= EWL)
							{
								/* Ricalcola la dimensione del filtro */
								if (EWL > 1)
									CHFL = EWL - 1;
								else
									CHFL = 1;
								CFL	= 1 + (CHFL * 2);

								/* Ricalcola il filtro e la finestra */
								FastLowPassFir(FIRFilter,CFL,BCut,FWin,0);
							}
						else
							/* Ricalcola il filtro usando la finestra corrente */
							FastLowPassFir(FIRFilter,CFL,BCut,FWin,CFL);

						/* Verifica visualizzazione stato prefiltratura */
						if (BCut >= (DLReal) (FilterBegin * pow(BWidth,Band)))
							{
								/* Segnala lo stato */
								printf("L - Band: %3d, %7.1f Hz, width: %6d, FIR, ", (int) Band,
									(double) (BCut * SampleFreq) / 2, CWL);

								/* Passa alla banda successiva */
								Band++;

								/* Inizio convoluzione */
								sputs("convolution...");
							}

						/* Verifica se si può utilizzare il filtro completo */
						if (OCP - CHFL >= HEFL)
							{
								/* Imposta gli indici per il filtro completo */
								CFLM = CFL;
								IS = OCP - (CHFL + HEFL);
								FS = 0;
							}
						else
							{
								/* Imposta gli indici per il filtro parziale */
								CFLM = CFL;
								IS = 0;
								FS = (CHFL + HEFL) - OCP;
							}

						/* Esegue la convoluzione, Kahan summation algorithm */
						Sum = (DLReal) 0.0;
						SC = (DLReal) 0.0;
						for (J = FS,EI = IS;J < CFLM;J++,EI++)
							{
								SY = (InImp[EI] * FIRFilter[J]) - SC;
								ST = Sum + SY;
								SC = (ST - Sum) - SY;
								Sum = ST;
							}
						OutImp[OCP] = Sum;
					}

				/* Verifica il tipo di curva di prefiltratura */
				switch (SLPType)
					{
						/* Proporzionale */
						case SLPProportional:
							/* Calcola la frequenza di taglio */
							BCut = (DLReal) 1.0 / (A * pow(CWL + Q,WindowExponent));
						break;

						/* Bilineare */
						case SLPBilinear:
							/* Calcola la frequenza di taglio */
							BCut = (DLReal) FilterBegin + (B * (((HIBS - CWL) / A) * (1 + Q)) /
								(1 + ((HIBS - CWL) / A) * Q));
						break;
					}

				/* Segnala lo stato finale */
				printf("F - Band: %3d, %7.1f Hz, width: %6d, FIR, ", (int) Band,
					(double) (BCut * SampleFreq) / 2, CWL);
				sputs("completed.");
			}

		/* Effettua la convoluzione, lato destro */
		if ((WType == WRight) || (WType == WFull))
			{
				/* Prepara gli indici per la convoluzione */
				EWL = (int) floor(FSharpness * HNR);
				CHFL = HEFL;

				/* Verifica che il filtro stia all'interno della finestratura corrente */
				if (CHFL >= EWL)
					{
						/* Ricalcola la dimensione del filtro */
						if (EWL > 1)
							CHFL = EWL - 1;
						else
							CHFL = 1;
						CFL	= 1 + (CHFL * 2);
					}
				else
					/* Usa la dimensione base */
					CFL = EFL;

				/* Ricalcola il filtro e la finestra */
				FastLowPassFir(FIRFilter,CFL,FilterBegin,FWin,0);

				/* Segnala lo stato */
				sputs("R - Initial lowpass convolution...");

				/* Ciclo di convoluzione lowpass iniziale */
				for(I = 0,CWL = HNR,OCP = NR - 1;I < (HNR - HIBS);I++,CWL--,OCP--)
					{
						/* Calcola la finestratura effettiva corrente */
						EWL = (int) floor(FSharpness * CWL);

						/* Verifica che il filtro stia all'interno della finestratura corrente */
						if (CHFL >= EWL)
							{
								/* Ricalcola la dimensione del filtro */
								if (EWL > 1)
									CHFL = EWL - 1;
								else
									CHFL = 1;
								CFL	= 1 + (CHFL * 2);

								/* Ricalcola il filtro e la finestra */
								FastLowPassFir(FIRFilter,CFL,FilterBegin,FWin,0);
							}

						/* Verifica se si può utilizzare il filtro completo */
						if (OCP + CHFL < HEFL + IBS)
							{
								/* Imposta gli indici per il filtro completo */
								CFLM = CFL;
								IS = OCP - (CHFL + HEFL);
								FS = 0;
							}
						else
							{
								/* Imposta gli indici per il filtro parziale */
								CFLM = HEFL + IBS + CHFL - OCP;
								IS = OCP - (CHFL + HEFL);
								FS = 0;
							}

						/* Esegue la convoluzione, Kahan summation algorithm */
						Sum = (DLReal) 0.0;
						SC = (DLReal) 0.0;
						for (J = FS,EI = IS;J < CFLM;J++,EI++)
							{
								SY = (InImp[EI] * FIRFilter[J]) - SC;
								ST = Sum + SY;
								SC = (ST - Sum) - SY;
								Sum = ST;
							}
						OutImp[OCP] = Sum;
					}

				/* Imposta i parametri iniziali per la visualizzazione */
				Band = 0;

				/* Ciclo di convoluzione sliding lowpass */
				for(I = HNR - HIBS,CWL = HIBS,OCP = NR - 1 - (HNR - HIBS);I < (HNR - HFBS);I++,CWL--,OCP--)
					{
						/* Calcola la finestratura effettiva corrente */
						EWL = (int) floor(FSharpness * CWL);

						/* Verifica il tipo di curva di prefiltratura */
						switch (SLPType)
							{
								/* Proporzionale */
								case SLPProportional:
									/* Calcola la frequenza di taglio */
									BCut = (DLReal) 1.0 / (A * pow(CWL + Q,WindowExponent));
								break;

								/* Bilineare */
								case SLPBilinear:
									/* Calcola la frequenza di taglio */
									BCut = (DLReal) FilterBegin + (B * (((HIBS - CWL) / A) * (1 + Q)) /
										(1 + ((HIBS - CWL) / A) * Q));
								break;
							}

						/* Verifica che il filtro stia all'interno della finestratura corrente */
						if (CHFL >= EWL)
							{
								/* Ricalcola la dimensione del filtro */
								if (EWL > 1)
									CHFL = EWL - 1;
								else
									CHFL = 1;
								CFL	= 1 + (CHFL * 2);

								/* Ricalcola il filtro e la finestra */
								FastLowPassFir(FIRFilter,CFL,BCut,FWin,0);
							}
						else
							/* Ricalcola il filtro usando la finestra corrente */
							FastLowPassFir(FIRFilter,CFL,BCut,FWin,CFL);

						/* Verifica visualizzazione stato prefiltratura */
						if (BCut >= (DLReal) (FilterBegin * pow(BWidth,Band)))
							{
								/* Segnala lo stato */
								printf("R - Band: %3d, %7.1f Hz, width: %6d, FIR, ", (int) Band,
									(double) (BCut * SampleFreq) / 2, CWL);

								/* Passa alla banda successiva */
								Band++;

								/* Inizio convoluzione */
								sputs("convolution...");
							}

						/* Verifica se si può utilizzare il filtro completo */
						if (OCP + CHFL < HEFL + IBS)
							{
								/* Imposta gli indici per il filtro completo */
								CFLM = CFL;
								IS = OCP - (CHFL + HEFL);
								FS = 0;
							}
						else
							{
								/* Imposta gli indici per il filtro parziale */
								CFLM = HEFL + IBS + CHFL - OCP;
								IS = OCP - (CHFL + HEFL);
								FS = 0;
							}

						/* Esegue la convoluzione, Kahan summation algorithm */
						Sum = (DLReal) 0.0;
						SC = (DLReal) 0.0;
						for (J = FS,EI = IS;J < CFLM;J++,EI++)
							{
								SY = (InImp[EI] * FIRFilter[J]) - SC;
								ST = Sum + SY;
								SC = (ST - Sum) - SY;
								Sum = ST;
							}
						OutImp[OCP] = Sum;
					}

				/* Verifica il tipo di curva di prefiltratura */
				switch (SLPType)
					{
						/* Proporzionale */
						case SLPProportional:
							/* Calcola la frequenza di taglio */
							BCut = (DLReal) 1.0 / (A * pow(CWL + Q,WindowExponent));
						break;

						/* Bilineare */
						case SLPBilinear:
							/* Calcola la frequenza di taglio */
							BCut = (DLReal) FilterBegin + (B * (((HIBS - CWL) / A) * (1 + Q)) /
								(1 + ((HIBS - CWL) / A) * Q));
						break;
					}

				/* Segnala lo stato finale */
				printf("F - Band: %3d, %7.1f Hz, width: %6d, FIR, ", (int) Band,
					(double) (BCut * SampleFreq) / 2, CWL);
				sputs("completed.");
			}

		/* Segnala lo stato finale */
		sputs("Final allpass convolution...");

		/* Copia la parte rimanente della risposta */
		switch (WType)
			{
				case WLeft:
					for (I = HNR - HFBS,J = HIBS - HFBS;J < IBS;I++,J++)
						OutImp[I] = InImp[J];
					for (I = NR - HEFL;I < NR;I++)
						OutImp[I] = (DLReal) 0.0;
				break;
				case WRight:
					for (I = 0;I < HEFL;I++)
						OutImp[I] = (DLReal) 0.0;
					FS = HFBS + (IBS % 2);
					for (I = HEFL,J = 0;I < (HNR + FS);I++,J++)
						OutImp[I] = InImp[J];
				break;
				case WFull:
					FS = HFBS + (IBS % 2);
					for (I = HNR - HFBS,J = HIBS - HFBS;I < (HNR + FS);I++,J++)
						OutImp[I] = InImp[J];
				break;
			}

		/* Dealloca gli array intermedi */
		delete[] FIRFilter;
		delete[] FWin;
	}
