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

/* Inclusioni */
#include "baselib.h"
#include "fft.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

/* Output stringhe con sync output e parametro */
int sputsp(const char * s, const char * p)
	{
		int Res;
		if (p == NULL)
			Res = puts(s);
		else
			Res = printf("%s%s\n",s,p);
		fflush(stdout);
		return(Res);
	}

/* Output stringhe con sync output */
int sputs(const char * s)
	{
		return(sputsp(s, NULL));
	}

/* Determina la lunghezza di un file */
size_t FSize(FILE * F)
	{
		long CPos;
		long FS;

		CPos = ftell(F);
		fseek(F,0,SEEK_END);
		FS = ftell(F);
		fseek(F,CPos,SEEK_SET);
		return (size_t) FS;
	}

/* Legge parte di un file di ingresso e lo pone nell'array indicato
	di dimensione InitWindow.
	Ritorna true se l'operazione ha successo.
*/
Boolean ReadSignal(const char * FName,DRCFloat * Dst,const int InitWindow,
	const int ImpulseCenter,const IFileType FType,
	int * PreSpikeStart, int * PostSpikeEnd)
	{
		int I;
		int IStart;
		int IEnd;
		int WHalf;
		DRCFileDouble RWD;
		DRCFileFloat RWF;
		DRCFileInt RWI;
		int RWDim;

		/* File gestione IO */
		FILE * IOF;
		long FS;

		/* Azzera l'array destinazione */
		for (I = 0;I < InitWindow;I++)
			Dst[I] = 0;

		/* Salva la dimensione del blocco lettura */
		RWDim = sizeof(DRCFileFloat);
		switch (FType)
			{
				case PcmFloat64Bit:
					RWDim = sizeof(DRCFileDouble);
				break;

				case PcmFloat32Bit:
					RWDim = sizeof(DRCFileFloat);
				break;

				case PcmInt16Bit:
					RWDim = sizeof(DRCFileInt);
				break;
			}

		/* Apre il file di input */
		if ((IOF = fopen(FName,"rb")) == NULL)
			{
				perror("\nUnable to open input file");
				return False;
			}

		/* Recupera la dimensione del file */
		FS = FSize(IOF) / RWDim;

		/* Controllo validità parametri */
		if (FS == 0)
			{
				puts("\nInput file is zero length.");
				return False;
			}
		if (ImpulseCenter > FS)
			{
				puts("\nImpulseCenter is out of input file range.");
				return False;
			}

		/* Calcolo la posizione centrale finestra */
		if (InitWindow % 2 == 0)
			WHalf = (InitWindow - 1) / 2;
		else
			WHalf = InitWindow / 2;

		/* Calcola gli estremi lettura finestra */
		if (ImpulseCenter > WHalf)
			{
				if (fseek(IOF,(ImpulseCenter - WHalf) * RWDim,SEEK_SET) != 0)
					{
						perror("\nError reading input file");
						return False;
					}
				IStart = 0;
				if (InitWindow > FS - (ImpulseCenter - WHalf))
					IEnd = FS - (ImpulseCenter - WHalf);
				else
					IEnd = InitWindow;
			}
		else
			{
				IStart = WHalf - ImpulseCenter;
				if (InitWindow > IStart + FS)
					IEnd = IStart + FS;
				else
					IEnd = InitWindow;
			}

		/* Imposta inizio pre spike */
		if (PreSpikeStart != NULL)
			*PreSpikeStart = IStart;

		/* Imposta fine post spike */
		if (PostSpikeEnd != NULL)
			*PostSpikeEnd = IEnd;

		/* Legge la risposta all'impulso */
		switch (FType)
			{
				case PcmFloat64Bit:
					for (I = IStart; I < IEnd; I++)
						{
							if (fread((void *) &RWD,sizeof(DRCFileDouble),1,IOF) != 1)
								{
									perror("\nError reading input file");
									return False;
								}
							Dst[I] = (DLReal) RWD;
						}
				break;

				case PcmFloat32Bit:
					for (I = IStart; I < IEnd; I++)
						{
							if (fread((void *) &RWF,sizeof(DRCFileFloat),1,IOF) != 1)
								{
									perror("\nError reading input file");
									return False;
								}
							Dst[I] = (DLReal) RWF;
						}
				break;

				case PcmInt16Bit:
					for (I = IStart; I < IEnd; I++)
						{
							if (fread((void *) &RWI,sizeof(DRCFileInt),1,IOF) != 1)
								{
									perror("\nError reading input file");
									return False;
								}
							Dst[I] = (DLReal) RWI;
						}
				break;
			}

		/* Chiude il file */
		fclose(IOF);

		/* Operazione completata */
		return True;
	}

/* Scrive il segnale indicato su disco */
Boolean WriteSignal(const char * FName,const DRCFloat * Src,const int SSize,
	const IFileType FType)
	{
		/* File gestione IO */
		FILE * IOF;
		int I;
		DRCFileDouble RWD;
		DRCFileFloat RWF;
		DRCFileInt RWI;

		/* Apre il file di output */
		if ((IOF = fopen(FName,"wb")) == NULL)
			{
				perror("\nUnable to open ouput file");
				return False;
			}

		/* Salva la risposta risultante */
		switch (FType)
			{
				case PcmFloat64Bit:
					for (I = 0; I < SSize; I++)
						{
							RWD = (DRCFileDouble) Src[I];
							if (fwrite((void *) &RWD,sizeof(DRCFileDouble),1,IOF) != 1)
								{
									perror("\nError writing ouput file");
									return False;
								}
						}
				break;

				case PcmFloat32Bit:
					for (I = 0; I < SSize; I++)
						{
							RWF = (DRCFileFloat) Src[I];
							if (fwrite((void *) &RWF,sizeof(DRCFileFloat),1,IOF) != 1)
								{
									perror("\nError writing ouput file");
									return False;
								}
						}
				break;

				case PcmInt16Bit:
					for (I = 0; I < SSize; I++)
						{
							RWI = (DRCFileInt) floor(0.5 + Src[I]);
							if (fwrite((void *) &RWI,sizeof(DRCFileInt),1,IOF) != 1)
								{
									perror("\nError writing ouput file");
									return False;
								}
						}
				break;
			}

		/* Chiude il file */
		fclose(IOF);

		/* Operazione completata */
		return True;
	}

/* Sovrascrive il segnale indicato su disco */
Boolean OverwriteSignal(const char * FName,const DRCFloat * Src,const int SSize, const int Skip,
	const IFileType FType)
	{
		/* File gestione IO */
		FILE * IOF;
		int I;
		DRCFileDouble RWD;
		DRCFileFloat RWF;
		DRCFileInt RWI;

		/* Apre il file di output */
		IOF = fopen(FName,"r+b");

		/* Salva la risposta risultante */
		switch (FType)
			{
				case PcmFloat64Bit:
					fseek(IOF,Skip * sizeof(DRCFileDouble), SEEK_SET);
					for (I = 0; I < SSize; I++)
						{
							RWD = (DRCFileDouble) Src[I];
							if (fwrite((void *) &RWD,sizeof(DRCFileDouble),1,IOF) != 1)
								{
									perror("\nError writing ouput file.");
									return False;
								}
						}
				break;

				case PcmFloat32Bit:
					fseek(IOF,Skip * sizeof(DRCFileFloat), SEEK_SET);
					for (I = 0; I < SSize; I++)
						{
							RWF = (DRCFileFloat) Src[I];
							if (fwrite((void *) &RWF,sizeof(DRCFileFloat),1,IOF) != 1)
								{
									perror("\nError writing ouput file.");
									return False;
								}
						}
				break;

				case PcmInt16Bit:
					fseek(IOF,Skip * sizeof(DRCFileInt), SEEK_SET);
					for (I = 0; I < SSize; I++)
						{
							RWI = (DRCFileInt) floor(0.5 + Src[I]);
							if (fwrite((void *) &RWI,sizeof(DRCFileInt),1,IOF) != 1)
								{
									perror("\nError writing ouput file.");
									return False;
								}
						}
				break;
			}

		/* Chiude il file */
		fclose(IOF);

		/* Operazione completata */
		return True;
	}

/* Calcola l'autocorrelazione del segnale S */
Boolean AutoCorrelation(DLReal * S, int N)
	{
		DLComplex * C;
		int I;

		if ((C = new DLComplex[N]) == NULL)
			return False;

		for (I = 0; I < N; I++)
			C[I] = S[I];

		Fft(C,N);

		for (I = 0; I < N; I++)
			/* C[I] = C[I] * std::conj(C[I]); */
			C[I] = std::real(C[I]) * std::real(C[I]) + std::imag(C[I]) * std::imag(C[I]);

		IFft(C,N);

		for (I = 0; I < N; I++)
			S[I] = std::real(C[I]) / N;

		delete[] C;

		return True;
	}

/* Calcola la cross correlazione tra S1 e S2 */
/* XC deve avere lunghezza 2N - 1 */
Boolean CrossCorrelation(DLReal * S1, DLReal * S2, int N, DLReal * XC)
	{
		DLComplex * C1;
		DLComplex * C2;
		int I;

		if ((C1 = new DLComplex[2 * N]) == NULL)
			return False;
		if ((C2 = new DLComplex[2 * N]) == NULL)
			{
				delete[] C1;
				return False;
			}

		for (I = 0; I < N; I++)
			{
				C1[I] = S1[I];
				C2[I] = S2[I];
			}
		for (I = N; I < 2 * N; I++)
			{
				C1[I] = 0;
				C2[I] = 0;
			}

		Fft(C1,2 * N);
		Fft(C2,2 * N);

		for (I = 0; I < 2 * N; I++)
			C1[I] *= std::conj<DLReal>(C2[I]);

		delete[] C2;

		IFft(C1,2 * N);

		for (I = 0; I < 2 * N - 1; I++)
			XC[I] = std::real(C1[I]) / N;

		delete[] C1;

		return True;
	}

/* Calcola il ritardo di gruppo del segnale S */
Boolean GroupDelay(const DLReal * S, const int N, DLReal * GD)
	{
		DLComplex * HA;
		DLComplex * DHA;
		int I;

		if ((HA = new DLComplex[N]) == NULL)
			return False;

		if ((DHA = new DLComplex[N]) == NULL)
			{
				delete[] HA;
				return False;
			}

		for(I = 0;I < N;I++)
			{
				HA[I] = S[I];
				DHA[I] = S[I] * I;
			}

		Fft(HA,N);
		Fft(DHA,N);

		for(I = 0;I < N;I++)
			GD[I] = std::real(DHA[I] / HA[I]);

		delete[] HA;
		delete[] DHA;

		return True;
	}

/* I/O Delay computation, reliable only for simple impulse responses */
DLReal LinearDelay(DLReal * Hn,unsigned int N,unsigned int Np,
  DLReal MZE)
  {
    unsigned int I;
    DLComplex * H;
    DLComplex * H1;
    DLComplex Sum,i(0,1);

    if (Np == 0)
      for(Np = 2;Np < N;Np <<= 1);

    Np = 2*Np;

    if ((H = new DLComplex[Np]) == NULL)
      return(-1);

    if ((H1 = new DLComplex[Np]) == NULL)
      {
        delete[](H);
        return(-1);
      }

    for(I = 0;I < N;I++)
      H[I] = Hn[I];

    for(I = N;I < Np;I++)
      H[I] = 0;

    if (Fft(H,Np) == False)
      {
        delete[](H);
				delete[](H1);
        return(-1);
      }

    for(I = 0;I < N;I++)
      H1[I] = I*Hn[I];

    for(I = N;I < Np;I++)
      H1[I] = 0;

    if (Fft(H1,Np) == False)
      {
        delete[](H);
        delete[](H1);
        return(-1);
      }

    for(I = 0;I < Np;I++)
      if (std::abs<DLReal>(H[I]) <= MZE)
        {
          H[I] = 1;
          H1[I] = 0;
        }

    Sum = 0;
    for(I = 0;I < Np;I++)
      Sum += (-i*H1[I])/H[I];

    delete[](H);
    delete[](H1);
    return(std::real<DLReal>(-Sum/(i*(DLReal) Np)));
  }

/* Conta il numero di righe in un file */
int FLineCount(const char * FName)
	{
		/* File gestione IO */
		FILE * IOF;

		/* Variabili ausiliarie */
		int LC;
		char InStr[256];

		/* Apre il file di input */
		if ((IOF = fopen(FName,"rt")) == NULL)
			{
				perror("\nUnable to open input file.");
				return False;
			}

		/* Conta le righe */
		LC = 0;
		while (fgets(InStr,256,IOF) != NULL)
			LC++;

		/* Chiude il file di input */
		fclose(IOF);

		/* Ritorna il numero di righe */
		return LC;
	}

/* Legge i punti di generazione filtro FIR dal file indicato */
Boolean ReadPoints(char * CorrFile,const TFMagType MagType,
	DLReal * FilterFreqs,DLReal * FilterM,DLReal * FilterP,const int NPoints,
	int SampleRate)
	{
		/* File gestione IO */
		FILE * IOF;

		/* Variabili ausiliarie */
		int I;
		float Freq;
		float Mag;
		float Phase;
		char InStr[256];

		/* Apre il file di correzione */
		if ((IOF = fopen(CorrFile,"rt")) == NULL)
			{
				perror("\nUnable to open correction file.");
				return False;
			}

		/* Legge i valori */
		for (I = 0; I < NPoints; I++)
			{
				if (fgets(InStr,256,IOF) == NULL)
					{
						perror("\nError reading correction file");
						return False;
					}

				Phase = 0;
				if (sscanf(InStr,"%f %f %f",&Freq,&Mag,&Phase) < 2)
					{
						printf("Not enough parameters on line %d.",I);
						sputs("Error reading correction file.");
						return False;
					}

				/* Verifica il tipo di ampiezza */
				if (MagType == MAGdB)
					/* Ricava l'ampiezza assoluta dai dB */
					Mag = (DLReal) pow(10.0,Mag / 20.0);

				/* Imposta i punti per il filtro */
				FilterFreqs[I] = (DLReal) (2 * Freq) / SampleRate;
				FilterM[I] = (DLReal) Mag;
				FilterP[I] = (DLReal) ((Phase * M_PI) / 180);
			}
		FilterFreqs[NPoints - 1] = (DLReal) 1.0;

		/* Chiude il file di input */
		fclose(IOF);

		/* Operazione completata */
		return True;
	}

/* Integra due funzioni di trsferimento definite per punti, usando
una interpolazione lineare, ritorna il numero di punti generati,
che non sono mai più di NPoints1 + NPoints2 */
int LITFMerge(DLReal * FilterFreqs1,DLComplex * FilterPoints1,const int NPoints1,
	DLReal * FilterFreqs2,DLComplex * FilterPoints2,const int NPoints2,
	DLReal * FilterFreqsOut,DLComplex * FilterPointsOut)
	{
		/* Indice sull'array di output */
		int OI;

		/* Indice sugli array di ingresso */
		int I1;
		int I2;

		/* Calcolo interpolazione */
		DLReal DMag;
		DLReal DArg;
		DLReal DFreq;

		/* Inizializza gli indici ciclo interpolazione */
		OI = 0;
		I1 = 0;
		I2 = 0;

		/* Ciclo interpolazione */
		while (I1 < NPoints1 || I2 < NPoints2)
			{
				if (FilterFreqs1[I1] == FilterFreqs2[I2])
					{
						/* Calcola il punto di uscita */
						FilterFreqsOut[OI] = FilterFreqs1[I1];
						FilterPointsOut[OI] = FilterPoints1[I1] * FilterPoints2[I2];

						/* Avanza gli indici elaborati */
						I1++;
						I2++;
					}
				else
					if (FilterFreqs1[I1] > FilterFreqs2[I2])
						{
							/* Calcola la frequeunza del punto di uscita */
							FilterFreqsOut[OI] = FilterFreqs2[I2];

							/* Calcola le variazioni per l'interpolazione */
							DMag = std::abs(FilterPoints1[I1]) -
								std::abs(FilterPoints1[I1 - 1]);
							DArg = std::arg(FilterPoints1[I1]) -
								std::arg(FilterPoints1[I1 - 1]);
							DFreq = FilterFreqs1[I1] -
								FilterFreqs1[I1 - 1];

							/* Calcola il punto di uscita */
							FilterPointsOut[OI] = FilterPoints2[I2] *
								std::polar(std::abs(FilterPoints1[I1 - 1]) +
								(FilterFreqsOut[OI] - FilterFreqs1[I1 - 1]) * DMag / DFreq,
								std::arg(FilterPoints1[I1 - 1]) +
								(FilterFreqsOut[OI] - FilterFreqs1[I1 - 1]) * DArg / DFreq);

							/* Avanza l'indice elaborato */
							I2++;
						}
					else
						{
							/* Calcola la frequeunza del punto di uscita */
							FilterFreqsOut[OI] = FilterFreqs1[I1];

							/* Calcola le variazioni per l'interpolazione */
							DMag = std::abs(FilterPoints2[I2]) -
								std::abs(FilterPoints2[I2 - 1]);
							DArg = std::arg(FilterPoints2[I2]) -
								std::arg(FilterPoints2[I2 - 1]);
							DFreq = FilterFreqs2[I2] -
								FilterFreqs2[I2 - 1];

							/* Calcola il punto di uscita */
							FilterPointsOut[OI] = FilterPoints1[I1] *
								std::polar(std::abs(FilterPoints2[I2 - 1]) +
								(FilterFreqsOut[OI] - FilterFreqs2[I2 - 1]) * DMag / DFreq,
								std::arg(FilterPoints2[I2 - 1]) +
								(FilterFreqsOut[OI] - FilterFreqs2[I2 - 1]) * DArg / DFreq);

							/* Avanza l'indice elaborato */
							I1++;
						}

				/* Avanza l'indice di uscita */
				OI++;
			}

		/* Ritorna il numero punti generati */
		return OI;
	}

/* Trova il valore massimo all'interno di un file. Versione float 64 bit. */
static int FindMaxPcmFloat64Bit(const char * FName)
	{
		DRCFileDouble RWD;
		int MaxPos;
		int Pos;
		DRCFileDouble PcmMax;

		/* File gestione IO */
		FILE * IOF;

		/* Apre il file di input */
		if ((IOF = fopen(FName,"rb")) == NULL)
			{
				perror("\nUnable to open input file.");
				return -1;
			}

		/* Ciclo ricerca massimo */
		Pos = 0;
		MaxPos = 0;
		PcmMax = 0;
		while(feof(IOF) == 0)
			{
				if (fread((void *) &RWD,sizeof(DRCFileDouble),1,IOF) != 1)
					{
						if (feof(IOF) == 0)
							{
								perror("\nError reading input file.");
								return -1;
							}
					}

				if (((DRCFileFloat) fabs(RWD)) > PcmMax)
					{
						PcmMax = (DRCFileFloat) fabs(RWD);
						MaxPos = Pos;
					}

				Pos++;
			}

		/* Chiude il file di input */
		fclose(IOF);

		/* Operazione completata */
		return MaxPos;
	}

/* Trova il valore massimo all'interno di un file. Versione float 32 bit. */
static int FindMaxPcmFloat32Bit(const char * FName)
	{
		DRCFileFloat RWF;
		int MaxPos;
		int Pos;
		DRCFileFloat PcmMax;

		/* File gestione IO */
		FILE * IOF;

		/* Apre il file di input */
		if ((IOF = fopen(FName,"rb")) == NULL)
			{
				perror("\nUnable to open input file.");
				return -1;
			}

		/* Ciclo ricerca massimo */
		Pos = 0;
		MaxPos = 0;
		PcmMax = 0;
		while(feof(IOF) == 0)
			{
				if (fread((void *) &RWF,sizeof(DRCFileFloat),1,IOF) != 1)
					{
						if (feof(IOF) == 0)
							{
								perror("\nError reading input file.");
								return -1;
							}
					}

				if (((DRCFileFloat) fabs(RWF)) > PcmMax)
					{
						PcmMax = (DRCFileFloat) fabs(RWF);
						MaxPos = Pos;
					}

				Pos++;
			}

		/* Chiude il file di input */
		fclose(IOF);

		/* Operazione completata */
		return MaxPos;
	}

/* Trova il valore massimo all'interno di un file. Versione int 16 Bit. */
static int FindMaxPcmInt16Bit(const char * FName)
	{
		DRCFileInt RWI;
		int MaxPos;
		int Pos;
		DRCFileInt PcmMax;

		/* File gestione IO */
		FILE * IOF;

		/* Apre il file di input */
		if ((IOF = fopen(FName,"rb")) == NULL)
			{
				perror("\nUnable to open input file.");
				return -1;
			}

		/* Ciclo ricerca massimo */
		Pos = 0;
		MaxPos = 0;
		PcmMax = 0;
		while(feof(IOF) == 0)
			{
				if (fread((void *) &RWI,sizeof(DRCFileInt),1,IOF) != 1)
					{
						if (feof(IOF) == 0)
							{
								perror("\nError reading input file.");
								return -1;
							}
					}

				if (((DRCFileInt) abs(RWI)) > PcmMax)
					{
						PcmMax = (DRCFileInt) abs(RWI);
						MaxPos = Pos;
					}

				Pos++;
			}

		/* Chiude il file di input */
		fclose(IOF);

		/* Operazione completata */
		return MaxPos;
	}

/* Trova il valore massimo all'interno di un file. */
int FindMaxPcm(const char * FName,const IFileType FType)
	{
		/* Controlla il tipo file */
		switch (FType)
			{
				case PcmFloat64Bit:
					return FindMaxPcmFloat64Bit(FName);

				case PcmFloat32Bit:
					return FindMaxPcmFloat32Bit(FName);

				case PcmInt16Bit:
					return FindMaxPcmInt16Bit(FName);
			}

		/* Tipo file errato */
		return -1;
	}
