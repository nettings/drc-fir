/****************************************************************************

    DRC: Digital Room Correction
    Copyright (C) 2002-2010 Denis Sbragion

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

		This program uses the FFT  routines from  Takuya Ooura and the GNU
		Scientific  Library (GSL).  Many thanks  to Takuya Ooura and the GSL
		developers for these efficient routines.

****************************************************************************/

/* Main file */

/* Inclusioni */
#include "drc.h"
#include "dsplib.h"
#include "dspwind.h"
#include "bwprefilt.h"
#include "slprefilt.h"
#include "baselib.h"
#include "level.h"
#include "cfgparse.h"
#include "convol.h"
#include "hd.h"
#include "toeplitz.h"
#include "fir.h"
#include "kirkebyfd.h"
#include "drccfg.h"
#include "cmdline.h"
#include "spline.h"
#include "psychoacoustic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fenv.h>

/* Versione corrente */
#define DRCVersion "3.2.1"
#define DRCCopyright "2002-2012"

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

/* Header iniziale programma */
void ShowDRCHeader(void)
	{
		sputs("\nDRC " DRCVersion ": Digital Room Correction");
		sputs("Copyright (C) " DRCCopyright " Denis Sbragion");
		#ifdef UseDouble
			sputs("\nCompiled with double precision arithmetic.");
		#else
			sputs("\nCompiled with single precision arithmetic.");
		#endif
		#if defined(UseGSLFft) && defined(UseOouraFft)
			sputs("Using the GNU Scientific Library and Ooura FFT routines.");
		#else
			#ifdef UseGSLFft
				sputs("Using the GNU Scientific Library FFT routines.");
			#else
				#ifdef UseOouraFft
					sputs("Using the Ooura FFT routines.");
				#else
					sputs("Using the builtin FFT routines.");
				#endif
			#endif
		#endif
		sputs("\nThis program may be freely redistributed under the terms of");
		sputs("the GNU GPL and is provided to you as is, without any warranty");
		sputs("of any kind. Please read the file \"COPYING\" for details.");
	}

/* Descrizione uso programma */
void ShowDRCUsage(void)
	{
		sputs("Usage: DRC [--help] [Options] DRCFile");
		sputs("\nParameters:\n");
		sputs("  --help : show the full options list (long)");
		sputs("  Options: parameters overwriting options");
		sputs("  DRCFile: name of DRC configration file\n");
		sputs("  Refer to the manual and samples for options");
		sputs("  details and file format\n");
	}

/* Main procedure */
int main(int argc, char * argv[])
	{
		/* Segnale in ingresso */
		DLReal * OInSig;
		DLReal * InSig;

		/* Punto iniziale e finale lettura da file */
		int PSStart;
		int PSEnd;

		/* Componenti minimum phase e excess phase del segnale */
		DLReal * MPSig;
		DLReal * EPSig;

		/* Punto iniziale e dimenione finestrature */
		int WStart1;
		int WLen1;
		int WStart2;
		int WLen2;
		int WStart3;
		int WLen3;

		/* Array correzione microfono */
		DLReal * MCFilterFreqs;
		DLReal * MCFilterM;
		DLReal * MCFilterP;
		DLReal * MCFilter;
		DLReal * MCOutSig;
		int MCOutSigStart;
		int MCOutSigLen;
		int MCMPFLen;

		/* Componenti MP e EP prefiltrate */
		DLReal * MPPFSig;
		int MPPFSigLen;
		DLReal * EPPFSig;
		int EPPFSigLen;

		/* Array convoluzione MP/EP */
		DLReal * MPEPSig;
		int MPEPSigLen;

		/* Array inversione impulso */
		DLReal * PTTConv;
		int PTTConvLen;
		int PTTConvStart;
		int PTTRefLen;
		DLReal * PTFilter;
		MKSETFType TFType;

		/* Array calcolo risposta target psicoacustica*/
		DLReal * ISRevSig;
		DLReal * ISMPEPSig;
		DLReal * ISRevOut;
		int ISSigLen;

		/* Array troncatura ringing */
		DLReal * RTSig;
		int RTSigLen;

		/* Array risposta target */
		DLReal * PSFilterFreqs;
		DLReal * PSFilterM;
		DLReal * PSFilterP;
		DLReal * PSFilter;
		DLReal * PSOutSig;
		int PSOutSigLen;
		int PSMPFLen;

		/* Valore RMS segnale in ingresso */
		DLReal SRMSValue;

		/* Array convoluzione finale */
		DLReal * TCSig;
		int TCSigLen;

		/* Indici generici */
		int I;
		int J;

		/* Tipo interpolazione filtri target */
		InterpolationType FIType;

		/* Tipo funzione di prefiltratura */
		SLPPrefilteringType SLPType;
		BWPPrefilteringType BWPType;

		/* Gestione parametri recuperati dalla linea di comando */
		CmdLineType * OptData;
		char * DRCFile;

		/* Salvataggio istante di avvio */
		time_t CStart = (time_t) 0;

		/* I386 Debug only, enables all floating point exceptions traps */
		/* int em = 0x372;
		__asm__ ("fldcw %0" : : "m" (em)); */

		/* Messaggio iniziale */
		ShowDRCHeader();

		/* Empty line */
		sputs("");

		/* Controllo presenza argomenti */
		if (argc < 2)
			{
				ShowDRCUsage();
				return 0;
			}

		/* Salvataggio istante di avvio */
		CStart = time(NULL);

		/* Registra le informazioni command line sulla base della struttura di
		configurazione */
		OptData = RegisterCmdLine(CfgParmsDef);
		if (OptData == NULL)
			{
				sputs("Memory allocation failed.");
				return 1;
			}

		/* Recupera i parametri della command line */
		if (GetCmdLine(argc,argv,CfgParmsDef,OptData,&DRCFile) != 0)
			{
				sputs("\nCommand line parsing error.");
				return 1;
			}

		/* Verifica se è stato richiesto l'help */
		if (OptData->ParmSet[OptData->OptCount] == True)
			{
				/* Visualizza le opzioni disponibili a linea di comando */
				ShowDRCUsage();
				sputs("Available options:\n");
				ShowCmdLine(CfgParmsDef);

				/* Dealloca le informazioni parsing command line */
				FreeCmdLine(OptData, CfgParmsDef);

				return 0;
			}

		/* Verifica che il nome del file sia presente */
		if (DRCFile == NULL)
			{
				ShowDRCUsage();
				return 1;
			}

		/* Segnala l'avvio della procedura */
		sputsp("Input configuration file: ",DRCFile);

		/* Recupera la configurazione */
		sputs("Parsing configuration file...");
		if (CfgParse(DRCFile,CfgParmsDef,CfgSimple) <= 0)
			{
				/* Dealloca le informazioni parsing command line */
				FreeCmdLine(OptData, CfgParmsDef);
				CfgFree(CfgParmsDef);

				sputs(CfgGetLastErrorDsc());
				sputs("Configuration file parsing error.");
				return 1;
			}
		sputs("Parsing completed.");

		/* Sovrascrive la configurazione base con i parametri
		a linea di comando. */
		sputs("Adding command line options...");
		CopyCmdLineParms(OptData,CfgParmsDef);

		/* Imposta la directory base recupero file */
		if (SetupDRCCfgBaseDir(&Cfg,CfgParmsDef,OptData) > 0)
			{
				/* Dealloca le informazioni parsing command line */
				FreeCmdLine(OptData, CfgParmsDef);
				CfgFree(CfgParmsDef);

				sputs("Base configuration setup error.");
				return 1;
			}

		/* Dealloca le informazioni parsing command line */
		FreeCmdLine(OptData, CfgParmsDef);

		/* Controllo validità parametri */
		sputs("Configuration parameters check.");
		if (CheckDRCCfg(&Cfg) != 0)
			{
				/* Libera la memoria della struttura di configurazione */
				CfgFree(CfgParmsDef);
				free(DRCFile);
				return 1;
			}

		/* Controlla se è stata definita un directory base */
		if (Cfg.BCBaseDir != NULL)
			if (strlen(Cfg.BCBaseDir) > 0)
				sputsp("Base directory: ",Cfg.BCBaseDir);

		/*********************************************************************************/
		/* Importazione iniziale risposta all'impulso */
		/*********************************************************************************/

		/* Controlla il tipo ricerca centro impulso */
		if (Cfg.BCImpulseCenterMode[0] == 'A')
			{
				/* Ricerca il centro impulso */
				sputsp("Seeking impulse center on: ", Cfg.BCInFile);
				Cfg.BCImpulseCenter = FindMaxPcm(Cfg.BCInFile,(IFileType) Cfg.BCInFileType[0]);
				if (Cfg.BCImpulseCenter < 0)
					return 1;
				printf("Impulse center found at sample %i.\n",Cfg.BCImpulseCenter);
				fflush(stdout);
			}

		/* Alloca l'array per il segnale in ingresso */
		sputs("Allocating input signal array.");
		InSig = new DLReal[Cfg.BCInitWindow];
		if (InSig == NULL)
			{
				sputs("Memory allocation failed.");
				return 1;
			}

		/* Legge il file di ingresso */
		sputsp("Reading input signal: ",Cfg.BCInFile);
		if (ReadSignal(Cfg.BCInFile,InSig,Cfg.BCInitWindow,Cfg.BCImpulseCenter,
			(IFileType) Cfg.BCInFileType[0],&PSStart,&PSEnd) == False)
			{
				sputs("Error reading input signal.");
				return 1;
			}
		sputs("Input signal read.");

		/* Effettua la prefinestratura del segnale */
		if (Cfg.BCPreWindowLen > 0)
			{
				sputs("Input signal prewindowing.");

				/* Verifica che la finestratura sia corretta */
				if ((Cfg.BCInitWindow / 2 - Cfg.BCPreWindowLen) < PSStart)
					sputs("!!Warning: input signal too short for correct signal prewindowing, spurios spikes may be generated.");

				for (I = 0;I < Cfg.BCInitWindow / 2 - Cfg.BCPreWindowLen;I++)
					InSig[I] = 0;
				SpacedBlackmanWindow(&InSig[Cfg.BCInitWindow / 2 - Cfg.BCPreWindowLen],Cfg.BCPreWindowLen,Cfg.BCPreWindowGap,WLeft);
			}

		/*********************************************************************************/
		/* Compensazione microfono */
		/*********************************************************************************/

		/* Verifica se abilitata */
		if (Cfg.MCFilterType[0] != 'N')
			{
				/* Verifica se si devono contare i punti filtro */
				if (Cfg.MCNumPoints == 0)
					{
						sputsp("Counting mic compensation definition file points: ",Cfg.MCPointsFile);
						Cfg.MCNumPoints = FLineCount(Cfg.MCPointsFile);
						printf("Mic compensation definition file points: %d\n",Cfg.MCNumPoints);
						fflush(stdout);
					}

				/* Alloca gli array per la generazione del filtro compensazione */
				sputs("Allocating mic compensation filter arrays.");
				MCFilterFreqs = new DLReal[Cfg.MCNumPoints];
				if (MCFilterFreqs == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}
				MCFilterM = new DLReal[Cfg.MCNumPoints];
				if (MCFilterM == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}
				MCFilterP = new DLReal[Cfg.MCNumPoints];
				if (MCFilterP == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}
				MCOutSigLen = Cfg.MCFilterLen + Cfg.BCInitWindow - 1;
				MCOutSig = new DLReal[MCOutSigLen];
				if (MCOutSig == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}

				/* Legge i punti del filtro */
				sputsp("Reading mic compensation definition file: ",Cfg.MCPointsFile);
				if (ReadPoints(Cfg.MCPointsFile,(TFMagType) Cfg.MCMagType[0],MCFilterFreqs,
					MCFilterM,MCFilterP,Cfg.MCNumPoints,Cfg.BCSampleRate) == False)
					{
						sputs("Mic compensation file input failed.");
						return 1;
					}

        /* Effettua l'inversione diretta */
				sputs("Mic compensation direct inversion.");
				for (I = 0;I < Cfg.MCNumPoints;I++)
          {
            MCFilterM[I] = ((DRCFloat) 1.0) / MCFilterM[I];
            MCFilterP[I] = -MCFilterP[I];
          }

				/* Verifica il tipo di interpolazione */
				switch(Cfg.MCInterpolationType[0])
					{
						case 'L':
							FIType = Linear;
						break;
						case 'G':
							FIType = Logarithmic;
						break;
						case 'R':
							FIType = SplineLinear;
						break;
						case 'S':
							FIType = SplineLogarithmic;
						break;
						case 'P':
							FIType = PCHIPLinear;
						break;
						case 'H':
							FIType = PCHIPLogarithmic;
						break;
					}

				/* Verifica il tipo di filtro da utilizzare */
				switch (Cfg.MCFilterType[0])
					{
						case 'L':
							/* Alloca gli array per il filtro */
							sputs("Allocating mic compensation filter arrays.");
							MCFilter = new DLReal[Cfg.MCFilterLen];
							if (MCFilter == NULL)
								{
									sputs("Memory allocation failed.");
									return 1;
								}
							for (I = 0; I < Cfg.MCFilterLen; I++)
								MCFilter[I] = 0;

							/* Calcola la dimensione richiesta per il calcolo del filtro */
							if (Cfg.MCMultExponent >= 0)
								{
									/* Calcola la potenza di due superiore a Cfg.MCFilterLen */
									for(I = 1;I <= Cfg.MCFilterLen;I <<= 1);
									I *= 1 << Cfg.MCMultExponent;
								}
							else
								I = Cfg.MCFilterLen;

							/* Calcola il filtro */
							sputs("Mic compensation FIR Filter computation...");
							if (GenericFir(MCFilter,Cfg.MCFilterLen,
								MCFilterFreqs,MCFilterM,MCFilterP,Cfg.MCNumPoints,I,FIType) == False)
								{
									sputs("FIR Filter computation failed.");
									return 1;
								}

							/* Effettua la finestratura del filtro */
							BlackmanWindow(MCFilter,Cfg.MCFilterLen);
						break;
						case 'M':
							/* Alloca gli array per il filtro */
							sputs("Allocating mic compensation filter arrays.");
							MCMPFLen = 1 + 2 * Cfg.MCFilterLen;
							MCFilter = new DLReal[MCMPFLen];
							if (MCFilter == NULL)
								{
									sputs("Memory allocation failed.");
									return 1;
								}
							for (I = 0; I < MCMPFLen; I++)
								MCFilter[I] = 0;

							/* Calcola la dimensione richiesta per il calcolo del filtro */
							if (Cfg.MCMultExponent >= 0)
								{
									/* Calcola la potenza di due superiore a Cfg.MCFilterLen */
									for(I = 1;I <= MCMPFLen;I <<= 1);
									I *= 1 << Cfg.MCMultExponent;
								}
							else
								I = MCMPFLen;

							/* Calcola il filtro */
							sputs("Mic compensation FIR Filter computation...");
							if (GenericFir(MCFilter,MCMPFLen,
								MCFilterFreqs,MCFilterM,MCFilterP,Cfg.MCNumPoints,I,FIType) == False)
								{
									sputs("FIR Filter computation failed.");
									return 1;
								}

							/* Alloca gli array per la deconvoluzione omomorfa */
							sputs("Allocating homomorphic deconvolution arrays.");
							MPSig = new DLReal[MCMPFLen];
							if (MPSig == NULL)
								{
									sputs("Memory allocation failed.");
									return 1;
								}

							/* Azzera gli array */
							for (I = 0;I < MCMPFLen;I++)
								MPSig[I] = 0;

							/* Effettua la deconvoluzione omomorfa*/
							sputs("MP mic compensation filter extraction homomorphic deconvolution stage...");
							if (CepstrumHD(MCFilter,MPSig,NULL,MCMPFLen,
								Cfg.MCMultExponent) == False)
								{
									sputs("Homomorphic deconvolution failed.");
									return 1;
								}

							/* Effettua la finestratura del filtro a fase minima */
							HalfBlackmanWindow(MPSig,Cfg.MCFilterLen,0,WRight);

							/* Copia il filtro a fase minima nell'array filtro */
							for (I = 0;I < Cfg.MCFilterLen;I++)
								MCFilter[I] = MPSig[I];

							/* Dealloca l'array deconvoluzione */
							delete[] MPSig;
						break;
					}

        /* Verifica se si deve salvare il filtro psicoacustico */
				if (Cfg.MCFilterFile != NULL)
					{
						/* Salva la componente MP */
						sputsp("Saving mic compensation filter: ",Cfg.MCFilterFile);
						if (WriteSignal(Cfg.MCFilterFile,MCFilter,Cfg.MCFilterLen,
							(IFileType) Cfg.MCFilterFileType[0]) == False)
							{
								sputs("Mic compensation filter save failed.");
								return 1;
							}
					}

				/* Convoluzione filtro segnale */
				sputs("Mic compensation FIR Filter convolution...");
				if (DFftConvolve(InSig,Cfg.BCInitWindow,MCFilter,
					Cfg.MCFilterLen,MCOutSig) == False)
					{
						perror("Convolution failed.");
						return 1;
					}

				/* Deallocazione array */
				delete[] MCFilter;
				delete[] InSig;

				/* Determina la dimensione della finestra di uscita */
				if (Cfg.MCOutWindow > 0)
					{
						/* Verifica il tipo di filtro */
						switch (Cfg.MCFilterType[0])
							{
								case 'L':
									/* Determina la finestratura filtro */
									MCOutSigStart = (MCOutSigLen - Cfg.MCOutWindow) / 2;
									MCOutSigLen = Cfg.MCOutWindow;

									/* Effetua la finestratura filtro */
									sputs("Mic compensated signal windowing.");
									BlackmanWindow(&MCOutSig[MCOutSigStart],MCOutSigLen);
								break;
								case 'M':
									/* Determina la finestratura filtro */
									MCOutSigStart = (Cfg.BCInitWindow - Cfg.MCOutWindow) / 2;
									MCOutSigLen = Cfg.MCOutWindow;

									/* Effetua la finestratura filtro */
									sputs("Mic compensated signal windowing.");
									BlackmanWindow(&MCOutSig[MCOutSigStart],MCOutSigLen);
								break;
							}
					}
				else
					{
						/* Verifica il tipo di filtro */
						switch (Cfg.MCFilterType[0])
							{
								case 'L':
									/* Determina la finestratura filtro */
									MCOutSigStart = 0;
									PSStart += Cfg.MCFilterLen / 2;
									PSEnd += Cfg.MCFilterLen / 2;
								break;
								case 'M':
									/* Determina la finestratura filtro */
									MCOutSigStart = 0;
									MCOutSigLen = Cfg.BCInitWindow;
								break;
							}
					}

				/* Normalizzazione segnale risultante */
				if (Cfg.MCNormFactor > 0)
					{
						sputs("Mic compensated signal normalization.");
						if (SigNormalize(&MCOutSig[MCOutSigStart],MCOutSigLen,Cfg.MCNormFactor,
							(NormType) Cfg.MCNormType[0]) == False)
							{
								sputs("Normalization failed.");
								return 1;
							}
					}

				/* Verifica se si deve salvare il segnale compensato */
				if (Cfg.MCOutFile != NULL)
					{
						/* Salva il segnale compensato  */
						sputsp("Saving mic compensated signal: ",Cfg.MCOutFile);
						if (WriteSignal(Cfg.MCOutFile,&MCOutSig[MCOutSigStart],MCOutSigLen,
							(IFileType) Cfg.MCOutFileType[0]) == False)
							{
								sputs("Mic compensated signal save failed.");
								return 1;
							}
					}

				/* Deallocazione array */
				delete[] MCFilterFreqs;
				delete[] MCFilterM;
				delete[] MCFilterP;
			}
    else
      {
        /* Imposta la lunghezza del segnale */
        MCOutSigStart = 0;
        MCOutSigLen = Cfg.BCInitWindow;
        MCOutSig = InSig;
      }

    /*********************************************************************************/
		/* Salvataggio segnale convoluzione di test */
		/*********************************************************************************/

    /* Verifica se è attiva la convoluzione di test */
    if (Cfg.TCOutFile != NULL || Cfg.PTType[0] != 'N')
      {
        /* Alloca l'array per la convoluzione di test */
        sputs("Allocating test convolution signal array.");
				OInSig = new DLReal[MCOutSigLen];
				if (OInSig == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}

        /* Copia il segnale in ingresso per la convoluzione finale */
        for (I = 0,J = MCOutSigStart;I < MCOutSigLen;I++,J++)
          OInSig[I] = MCOutSig[J];
			}

    /* Calcola il valore RMS del segnale in ingresso */
		SRMSValue = GetRMSLevel(OInSig,MCOutSigLen);
		if (SRMSValue > ((DLReal) 0.0))
			printf("Input signal RMS level %f (%f dB).\n",(double) SRMSValue, (double) (20 * log10((double) SRMSValue)));
		else
			printf("Input signal RMS level %f (-Inf dB).\n",(double) SRMSValue);
		fflush(stdout);

    /*********************************************************************************/
		/* Dip limiting preventivo */
		/*********************************************************************************/

		/* Verifica se si deve effettuare il dip limiting */
		if (Cfg.BCDLMinGain > 0)
			{
				switch (Cfg.BCDLType[0])
					{
						/* Fase lineare */
						case 'L':
						case 'P':
							sputs("Input signal linear phase dip limiting...");
							if (C1LPDipLimit(&MCOutSig[MCOutSigStart],MCOutSigLen,Cfg.BCDLMinGain,Cfg.BCDLStart,
								Cfg.BCSampleRate,Cfg.BCDLStartFreq,Cfg.BCDLEndFreq,Cfg.BCDLType[0] == 'P',Cfg.BCDLMultExponent) == False)
								{
									sputs("Dip limiting failed.");
									return 1;
								}
						break;

						/* Fase minima */
						case 'M':
						case 'W':
							sputs("Input signal minimum phase dip limiting...");
							if (C1HMPDipLimit(&MCOutSig[MCOutSigStart],MCOutSigLen,Cfg.BCDLMinGain,Cfg.BCDLStart,
								Cfg.BCSampleRate,Cfg.BCDLStartFreq,Cfg.BCDLEndFreq,Cfg.BCDLType[0] == 'W',Cfg.BCDLMultExponent) == False)
								{
									sputs("Dip limiting failed.");
									return 1;
								}
						break;
					}
			}

		/* Verifica se si deve effettuare rinormalizzazione */
		if (Cfg.BCNormFactor > 0)
			{
				sputs("Input signal normalization.");
				if (SigNormalize(&MCOutSig[MCOutSigStart],MCOutSigLen,Cfg.BCNormFactor,
					(NormType) Cfg.BCNormType[0]) == False)
					{
						sputs("Normalization failed.");
						return 1;
					}
			}

		/*********************************************************************************/
		/* Deconvoluzione omomorfa */
		/*********************************************************************************/

		/* Alloca gli array per la deconvoluzione omomorfa */
		sputs("Allocating homomorphic deconvolution arrays.");
		MPSig = new DLReal[2 * MCOutSigLen];
		if (MPSig == NULL)
			{
				sputs("Memory allocation failed.");
				return 1;
			}
		EPSig = new DLReal[MCOutSigLen];
		if (EPSig == NULL)
			{
				sputs("Memory allocation failed.");
				return 1;
			}

		/* Azzera gli array */
		for (I = 0;I < 2 * MCOutSigLen;I++)
			MPSig[I] = 0;
		for (I = 0;I < MCOutSigLen;I++)
			EPSig[I] = 0;

		/* Effettua la deconvoluzione omomorfa*/
		sputs("Homomorphic deconvolution stage...");
		if (CepstrumHD(&MCOutSig[MCOutSigStart],&MPSig[MCOutSigLen / 2 - (1 - (MCOutSigLen % 2))],EPSig,
			MCOutSigLen,Cfg.HDMultExponent) == False)
			{
				sputs("Homomorphic deconvolution failed.");
				return 1;
			}

		/* Verifica se si deve effettuare rinormalizzazione */
		if (Cfg.HDMPNormFactor > 0)
			{
				sputs("Minimum phase component normalization.");
				if (SigNormalize(MPSig,MCOutSigLen,Cfg.HDMPNormFactor,
					(NormType) Cfg.HDMPNormType[0]) == False)
					{
						sputs("Normalization failed.");
						return 1;
					}
			}
		if (Cfg.HDEPNormFactor > 0)
			{
				sputs("Excess phase component normalization.");
				if (SigNormalize(EPSig,MCOutSigLen,Cfg.HDEPNormFactor,
					(NormType) Cfg.HDEPNormType[0]) == False)
					{
						sputs("Normalization failed.");
						return 1;
					}
			}

		/* Verifica se si deve salvare la componente MP */
		if (Cfg.HDMPOutFile != NULL)
			{
				/* Salva la componente MP */
				sputsp("Saving minimum phase component: ",Cfg.HDMPOutFile);
				if (WriteSignal(Cfg.HDMPOutFile,MPSig,MCOutSigLen,
					(IFileType) Cfg.HDMPOutFileType[0]) == False)
					{
						sputs("Minimum phase component save failed.");
						return 1;
					}
			}

		/* Verifica se si deve salvare la componente EP */
		if (Cfg.HDEPOutFile != NULL)
			{
				/* Salva la componente EP */
				sputsp("Saving excess phase component: ",Cfg.HDEPOutFile);
				if (WriteSignal(Cfg.HDEPOutFile,EPSig,MCOutSigLen,
					(IFileType) Cfg.HDEPOutFileType[0]) == False)
					{
						sputs("Excess phase component save failed.");
						return 1;
					}
			}

		/* Dealloca il segnale di ingresso */
		delete[] MCOutSig;

		/*********************************************************************************/
		/* Prefiltratura componente MP */
		/*********************************************************************************/

		/* Alloca l'array per il segnale MP prefiltrato */
		sputs("Allocating minimum phase component prefiltering array.");
		MPPFSigLen = Cfg.MPLowerWindow + Cfg.MPFilterLen - 1;
		MPPFSig = new DLReal[MPPFSigLen];
		if (MPPFSig == NULL)
			{
				sputs("Memory allocation failed.");
				return 1;
			}

		/* Azzera l'array */
		for (I = 0;I < MPPFSigLen;I++)
			MPPFSig[I] = 0;

		/* Calcola il punto iniziale finestra */
		WStart1 = (MCOutSigLen - Cfg.MPLowerWindow) / 2;

		/* Verifica il tipo di funzione di prefiltratura */
		if (Cfg.MPPrefilterFctn[0] == 'P')
			{
				/* Proporzionale */
				SLPType = SLPProportional;
				BWPType = BWPProportional;
			}
		else
			{
				/* Bilineare */
				SLPType = SLPBilinear;
				BWPType = BWPBilinear;
			}

		/* Prefiltratura componente MP */
		switch (Cfg.MPPrefilterType[0])
			{
				case 'B':
					sputs("Minimum phase component band windowing.");

					/* Verifica che la finestratura sia corretta */
					if (((Cfg.BCPreWindowLen == 0) && (WStart1 < PSStart)) || ((WStart1 + Cfg.MPLowerWindow) > PSEnd))
						sputs("!!Warning: input signal too short for correct signal prefiltering, spurios spikes may be generated.");

					BWPreFilt(&MPSig[WStart1],Cfg.MPLowerWindow,Cfg.MPUpperWindow,
						Cfg.MPFilterLen,Cfg.MPBandSplit,Cfg.MPWindowExponent,
						Cfg.BCSampleRate,Cfg.MPStartFreq,Cfg.MPEndFreq,Cfg.MPWindowGap,
						MPPFSig,WFull,BWPType);
				break;

				case 'b':
					sputs("Minimum phase component single side band windowing.");

					/* Verifica che la finestratura sia corretta */
					if ((WStart1 + Cfg.MPLowerWindow) > PSEnd)
						sputs("!!Warning: input signal too short for correct signal prefiltering, spurios spikes may be generated.");

					BWPreFilt(&MPSig[WStart1],Cfg.MPLowerWindow,Cfg.MPUpperWindow,
						Cfg.MPFilterLen,Cfg.MPBandSplit,Cfg.MPWindowExponent,
						Cfg.BCSampleRate,Cfg.MPStartFreq,Cfg.MPEndFreq,Cfg.MPWindowGap,
						MPPFSig,WRight,BWPType);
				break;

				case 'S':
					sputs("Minimum phase component sliding lowpass prefiltering.");

					/* Verifica che la finestratura sia corretta */
					if (((Cfg.BCPreWindowLen == 0) && (WStart1 < PSStart)) || ((WStart1 + Cfg.MPLowerWindow) > PSEnd))
						sputs("!!Warning: input signal too short for correct signal prefiltering, spurios spikes may be generated.");

					SLPreFilt(&MPSig[WStart1],Cfg.MPLowerWindow,Cfg.MPUpperWindow,
						Cfg.MPFilterLen,Cfg.MPBandSplit,Cfg.MPWindowExponent,
						Cfg.BCSampleRate,Cfg.MPStartFreq,Cfg.MPEndFreq,Cfg.MPWindowGap,
						Cfg.MPFSharpness,MPPFSig,WFull,SLPType);
				break;

				case 's':
					sputs("Minimum phase component single side sliding lowpass prefiltering.");

					/* Verifica che la finestratura sia corretta */
					if ((WStart1 + Cfg.MPLowerWindow) > PSEnd)
						sputs("!!Warning: input signal too short for correct signal prefiltering, spurios spikes may be generated.");

					SLPreFilt(&MPSig[WStart1],Cfg.MPLowerWindow,Cfg.MPUpperWindow,
						Cfg.MPFilterLen,Cfg.MPBandSplit,Cfg.MPWindowExponent,
						Cfg.BCSampleRate,Cfg.MPStartFreq,Cfg.MPEndFreq,Cfg.MPWindowGap,
						Cfg.MPFSharpness,MPPFSig,WRight,SLPType);
				break;
			}

		/* Dealloca la componente MP */
		delete[] MPSig;

		/* Calcola la dimensione per la finestratura finale */
		if (Cfg.MPPFFinalWindow > 0)
			{
				WStart1 = (MPPFSigLen - Cfg.MPPFFinalWindow) / 2;
				WLen1 = Cfg.MPPFFinalWindow;
			}
		else
			{
				WStart1 = 0;
				WLen1 = MPPFSigLen;
			}

		/*********************************************************************************/
		/* Dip limiting */
		/*********************************************************************************/

		/* Verifica se si deve effettuare il dip limiting */
		if (Cfg.DLMinGain > 0)
			{
				switch (Cfg.DLType[0])
					{
						/* Fase lineare */
						case 'L':
						case 'P':
							sputs("MP signal linear phase dip limiting...");
							if (C1LPDipLimit(&MPPFSig[WStart1],WLen1,Cfg.DLMinGain,Cfg.DLStart,
								Cfg.BCSampleRate,Cfg.DLStartFreq,Cfg.DLEndFreq,Cfg.DLType[0] == 'P',Cfg.DLMultExponent) == False)
								{
									sputs("Dip limiting failed.");
									return 1;
								}
						break;

						/* Fase minima */
						case 'M':
						case 'W':
							sputs("MP signal minimum phase dip limiting...");
							if (C1HMPDipLimit(&MPPFSig[WStart1],WLen1,Cfg.DLMinGain,Cfg.DLStart,
								Cfg.BCSampleRate,Cfg.DLStartFreq,Cfg.DLEndFreq,Cfg.DLType[0] == 'W',Cfg.DLMultExponent) == False)
								{
									sputs("Dip limiting failed.");
									return 1;
								}
						break;
					}
			}

		/* Verifica se deve essere effettuata la rinormalizzazione MP */
		if (Cfg.MPHDRecover[0] == 'Y')
			{
				/* Alloca gli array per la deconvoluzione omomorfa */
				sputs("Allocating homomorphic deconvolution arrays.");
				MPSig = new DLReal[2 * WLen1];
				if (MPSig == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}

				/* Azzera gli array */
				for (I = 0;I < 2 * WLen1;I++)
					MPSig[I] = 0;

				/* Controlla se si deve preservare la componente EP della fase minima */
				if (Cfg.MPEPPreserve[0] == 'Y')
					{
						MPEPSig = new DLReal[WLen1];
						if (MPEPSig == NULL)
							{
								sputs("Memory allocation failed.");
								return 1;
							}

						/* Azzera gli array */
						for (I = 0;I < WLen1;I++)
							MPEPSig[I] = 0;
					}
				else
					MPEPSig = NULL;

				/* Effettua la deconvoluzione omomorfa*/
				sputs("MP Recover homomorphic deconvolution stage...");
				if (CepstrumHD(&MPPFSig[WStart1],&MPSig[WLen1 / 2 - (1 - (WLen1 % 2))],MPEPSig,
					WLen1,Cfg.MPHDMultExponent) == False)
					{
						sputs("Homomorphic deconvolution failed.");
						return 1;
					}

				/* Ricopia la componente MP nell'array originale */
				for (I = 0,J = WStart1;I < WLen1;I++,J++)
					MPPFSig[J] = MPSig[I];

				/* Dealloca l'array deconvoluzione */
				delete[] MPSig;
			}

		/* Verifica se si deve effettuare la finestratura finale */
		if (Cfg.MPPFFinalWindow > 0)
			{
				sputs("Minimum phase component final windowing.");
				BlackmanWindow(&MPPFSig[WStart1],WLen1);

				/* Controlla se si deve preservare la componente EP della fase minima */
				if (Cfg.MPHDRecover[0] == 'Y' && Cfg.MPEPPreserve[0] == 'Y')
					/* Effettua la finestratura della componente EP */
					BlackmanWindow(MPEPSig,WLen1);
			}

		/* Verifica se si deve effettuare rinormalizzazione */
		if (Cfg.MPPFNormFactor > 0)
			{
				sputs("Minimum phase component normalization.");
				if (SigNormalize(&MPPFSig[WStart1],WLen1,Cfg.MPPFNormFactor,
					(NormType) Cfg.MPPFNormType[0]) == False)
					{
						sputs("Normalization failed.");
						return 1;
					}
			}

		/* Verifica se si deve salvare la componente MP finestrata */
		if (Cfg.MPPFOutFile != NULL)
			{
				/* Salva la componente MP */
				sputsp("Saving minimum phase component: ",Cfg.MPPFOutFile);
				if (WriteSignal(Cfg.MPPFOutFile,&MPPFSig[WStart1],WLen1,
					(IFileType) Cfg.MPPFOutFileType[0]) == False)
					{
						sputs("Minimum phase component save failed.");
						return 1;
					}
			}

		/*********************************************************************************/
		/* Prefiltratura componente EP */
		/*********************************************************************************/

		/* Controlla se si deve preservare la componente EP della fase minima */
		if (Cfg.MPHDRecover[0] == 'Y' && Cfg.MPEPPreserve[0] == 'Y')
			{
				/* Alloca l'array per la convoluzione */
				sputs("Allocating minimum phase EP recovering arrays.");
				MPEPSigLen = MCOutSigLen + WLen1 - 1;
				EPPFSig = new DLReal[MPEPSigLen];
				if (EPSig == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}

				/* Effettua la convoluzione */
				sputs("Minimum phase EP recovering...");
				if (DFftConvolve(MPEPSig,WLen1,EPSig,MCOutSigLen,EPPFSig) == False)
					{
						sputs("Convolution failed.");
						return 1;
					}

				/* Recupera la componente EP */
				for (I = 0,J = WLen1 / 2;I < MCOutSigLen;I++, J++)
					EPSig[I] = EPPFSig[J];

				/* Dealloca l'array temporaneo convoluzione */
				delete[] MPEPSig;
				delete[] EPPFSig;
			}


		/* Alloca l'array per il segnale EP prefiltrato */
		sputs("Allocating excess phase component prefiltering array.");
		EPPFSigLen = Cfg.EPLowerWindow + Cfg.EPFilterLen - 1;
		EPPFSig = new DLReal[EPPFSigLen];
		if (EPPFSig == NULL)
			{
				sputs("Memory allocation failed.");
				return 1;
			}

		/* Azzera l'array */
		for (I = 0;I < EPPFSigLen;I++)
			EPPFSig[I] = 0;

		/* Calcola il punto iniziale finestra */
		WStart2 = (MCOutSigLen - Cfg.EPLowerWindow) / 2;

		/* Verifica il tipo di funzione di prefiltratura */
		if (Cfg.EPPrefilterFctn[0] == 'P')
			{
				/* Proporzionale */
				SLPType = SLPProportional;
				BWPType = BWPProportional;
			}
		else
			{
				/* Bilineare */
				SLPType = SLPBilinear;
				BWPType = BWPBilinear;
			}

		/* Prefiltratura componente EP */
		switch (Cfg.EPPrefilterType[0])
			{
				case 'B':
					sputs("Excess phase component band windowing.");

					/* Verifica che la finestratura sia corretta */
					if (((Cfg.BCPreWindowLen == 0) && (WStart2 < PSStart)) || ((WStart2 + Cfg.EPLowerWindow) > PSEnd))
						sputs("!!Warning: input signal too short for correct signal prefiltering, spurios spikes may be generated.");

					BWPreFilt(&EPSig[WStart2],Cfg.EPLowerWindow,Cfg.EPUpperWindow,
						Cfg.EPFilterLen,Cfg.EPBandSplit,Cfg.EPWindowExponent,
						Cfg.BCSampleRate,Cfg.EPStartFreq,Cfg.EPEndFreq,Cfg.EPWindowGap,
						EPPFSig,WFull,BWPType);
				break;

				case 'b':
					sputs("Excess phase component single side band windowing.");

					/* Verifica che la finestratura sia corretta */
					if ((WStart2 + Cfg.EPLowerWindow) > PSEnd)
						sputs("!!Warning: input signal too short for correct signal prefiltering, spurios spikes may be generated.");

					BWPreFilt(&EPSig[WStart2],Cfg.EPLowerWindow,Cfg.EPUpperWindow,
						Cfg.EPFilterLen,Cfg.EPBandSplit,Cfg.EPWindowExponent,
						Cfg.BCSampleRate,Cfg.EPStartFreq,Cfg.EPEndFreq,Cfg.EPWindowGap,
						EPPFSig,WRight,BWPType);
				break;

				case 'S':
					sputs("Excess phase component sliding lowpass prefiltering.");

					/* Verifica che la finestratura sia corretta */
					if (((Cfg.BCPreWindowLen == 0) && (WStart2 < PSStart)) || ((WStart2 + Cfg.EPLowerWindow) > PSEnd))
						sputs("!!Warning: input signal too short for correct signal prefiltering, spurios spikes may be generated.");

					SLPreFilt(&EPSig[WStart2],Cfg.EPLowerWindow,Cfg.EPUpperWindow,
						Cfg.EPFilterLen,Cfg.EPBandSplit,Cfg.EPWindowExponent,
						Cfg.BCSampleRate,Cfg.EPStartFreq,Cfg.EPEndFreq,Cfg.EPWindowGap,
						Cfg.EPFSharpness,EPPFSig,WFull,SLPType);
				break;

				case 's':
					sputs("Excess phase component single side sliding lowpass prefiltering.");

					/* Verifica che la finestratura sia corretta */
					if ((WStart2 + Cfg.EPLowerWindow) > PSEnd)
						sputs("!!Warning: input signal too short for correct signal prefiltering, spurios spikes may be generated.");

					SLPreFilt(&EPSig[WStart2],Cfg.EPLowerWindow,Cfg.EPUpperWindow,
						Cfg.EPFilterLen,Cfg.EPBandSplit,Cfg.EPWindowExponent,
						Cfg.BCSampleRate,Cfg.EPStartFreq,Cfg.EPEndFreq,Cfg.EPWindowGap,
						Cfg.EPFSharpness,EPPFSig,WRight,SLPType);
				break;
			}

		/* Dealloca la componente EP */
		delete[] EPSig;

		/* Determina la lunghezza della componente dopo la finestratura */
		if (Cfg.EPPFFinalWindow > 0)
			{
				WStart2 = (EPPFSigLen - Cfg.EPPFFinalWindow) / 2;
				WLen2 = Cfg.EPPFFinalWindow;
			}
		else
			{
				WStart2 = 0;
				WLen2 = EPPFSigLen;
			}

		/* Verifica se si deve effettuare riappianamento */
		if (Cfg.EPPFFlatGain > 0)
			{
				switch (Cfg.EPPFFlatType[0])
					{
						case 'L':
							sputs("Excess phase component linear phase flattening...");
							LPNormFlat(&EPPFSig[WStart2],WLen2,Cfg.EPPFFlatGain,
								Cfg.EPPFOGainFactor,Cfg.EPPFFGMultExponent);
						break;

						case 'M':
							sputs("Excess phase component minimum phase flattening...");
							CMPNormFlat(&EPPFSig[WStart2],WLen2,Cfg.EPPFFlatGain,
								Cfg.EPPFOGainFactor,Cfg.EPPFFGMultExponent);
						break;

						case 'D':
							/* Alloca gli array per la deconvoluzione omomorfa */
							sputs("Allocating homomorphic deconvolution arrays.");
							EPSig = new DLReal[WLen2];
							if (EPSig == NULL)
								{
									sputs("Memory allocation failed.");
									return 1;
								}

							/* Azzera gli array per la deconvoluzione omomorfa */
							for (I = 0;I < WLen2;I++)
								EPSig[I] = 0;

							/* Effettua la deconvoluzione omomorfa*/
							sputs("Excess phase component homomorphic deconvolution flattening...");
							if (CepstrumHD(&EPPFSig[WStart2],NULL,EPSig,
								WLen2,Cfg.EPPFFGMultExponent) == False)
								{
									sputs("Homomorphic deconvolution failed.");
									return 1;
								}

							/* Copia il risultato nell'array destinazione */
							for (I = 0,J = WStart2;I < WLen2;I++,J++)
								EPPFSig[J] = EPSig[I];

							/* Dealloca gli array per la deconvoluzione omomorfa */
							delete[] EPSig;
						break;
					}
			}

		/* Verifica se si deve effettuare la finestratura finale */
		if (Cfg.EPPFFinalWindow > 0)
			{
				sputs("Excess phase component final windowing.");
				BlackmanWindow(&EPPFSig[WStart2],WLen2);
			}

		/* Verifica se si deve effettuare rinormalizzazione */
		if (Cfg.EPPFNormFactor > 0)
			{
				sputs("Excess phase component normalization.");
				if (SigNormalize(&EPPFSig[WStart2],WLen2,Cfg.EPPFNormFactor,
					(NormType) Cfg.EPPFNormType[0]) == False)
					{
						sputs("Normalization failed.");
						return 1;
					}
			}

		/* Verifica se si deve salvare la componente EP finestrata */
		if (Cfg.EPPFOutFile != NULL)
			{
				/* Salva la componente MP */
				sputsp("Saving excess phase component: ",Cfg.EPPFOutFile);
				if (WriteSignal(Cfg.EPPFOutFile,&EPPFSig[WStart2],WLen2,
					(IFileType) Cfg.EPPFOutFileType[0]) == False)
					{
						sputs("Excess phase component save failed.");
						return 1;
					}
			}

		/*********************************************************************************/
		/* Combinazione componente MP e EP */
		/*********************************************************************************/

		/* Controlla se si deve attuare la fase PC */
		if (Cfg.ISType[0] == 'L' || Cfg.PCOutFile != NULL)
			{
				/* Alloca l'array per la convoluzione MP/EP */
				sputs("Allocating MP/EP convolution array.");
				MPEPSigLen = WLen1 + WLen2 - 1;
				MPEPSig = new DLReal[MPEPSigLen];
				if (MPEPSig == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}

				/* Convoluzione MP/EP */
				sputs("MP/EP Convolution...");
				if (DFftConvolve(&MPPFSig[WStart1],WLen1,&EPPFSig[WStart2],WLen2,MPEPSig) == False)
					{
						sputs("Convolution failed.");
						return 1;
					}

				/* Dealloca gli array MP/EP finestrati */
				if (Cfg.ISType[0] == 'L')
					{
						delete[] MPPFSig;
						delete[] EPPFSig;
					}

				/* Finestratura segnale risultante */
				if (Cfg.PCOutWindow > 0)
					{
						sputs("MP/EP signal windowing.");
						WStart3 = (MPEPSigLen - Cfg.PCOutWindow) / 2;
						WLen3 = Cfg.PCOutWindow;
						BlackmanWindow(&MPEPSig[WStart3],WLen3);
					}
				else
					{
						WStart3 = 0;
						WLen3 = MPEPSigLen;
					}

				/* Normalizzazione segnale risultante */
				if (Cfg.PCNormFactor > 0)
					{
						sputs("MP/EP normalization.");
						if (SigNormalize(&MPEPSig[WStart3],WLen3,Cfg.PCNormFactor,
							(NormType) Cfg.PCNormType[0]) == False)
							{
								sputs("Normalization failed.");
								return 1;
							}
					}

				/* Verifica se si deve salvare il segnale prefinestrato */
				if (Cfg.PCOutFile != NULL)
					{
						/* Salva la componente MP */
						sputsp("Saving MP/EP signal: ",Cfg.PCOutFile);
						if (WriteSignal(Cfg.PCOutFile,&MPEPSig[WStart3],WLen3,
							(IFileType) Cfg.PCOutFileType[0]) == False)
							{
								sputs("MP/EP signal save failed.");
								return 1;
							}
					}

				/* Dealloca gli array */
				if (Cfg.ISType[0] != 'L')
					delete[] MPEPSig;
			}

		/*********************************************************************************/
		/* Inversione risposta all'impulso */
		/*********************************************************************************/

		/* Verifica tipo inversione */
		switch (Cfg.ISType[0])
			{
				/* Fase lineare con matrice Toeplitz */
				case 'L':
					/* Ricalcola le finestre effettive per l'inversione */
					if (Cfg.ISOutWindow > 0)
						{
							/* Finestra di inversione predefinita */
							ISSigLen = Cfg.ISOutWindow;

							/* Verifica che il segnale in ingresso sia di lunghezza adeguata */
							if (WLen3 > ISSigLen)
								{
									/* Ricalcola la lunghezza del segnale in ingresso */
									WStart3 += (WLen3 - ISSigLen) / 2;
									WLen3 = ISSigLen;

									/* Rifiniestra il segnale per riportarlo alla lunghezza dell'inversione */
									BlackmanWindow(&MPEPSig[WStart3],ISSigLen);
								}
						}
					else
						/* Adotta la finestra precedente */
						ISSigLen = WLen3;

					/* Alloca l'array per l'inversione segnale */
					sputs("Allocating delay/reverse array.");
					ISRevSig = new DLReal[ISSigLen];
					if (ISRevSig == NULL)
						{
							sputs("Memory allocation failed.");
							return 1;
						}
					for (I = 0;I < ISSigLen;I++)
						ISRevSig[I] = (DLReal) 0.0;

					/* Alloca l'array per l'autocorrelazione */
					sputs("Allocating autocorrelation array.");
					ISMPEPSig = new DLReal[ISSigLen];
					if (ISMPEPSig == NULL)
						{
							sputs("Memory allocation failed.");
							return 1;
						}
					for (I = 0;I < ISSigLen;I++)
						ISMPEPSig[I] = (DLReal) 0.0;
					for (I = WStart3,J = (ISSigLen - WLen3) / 2;I < WStart3 + WLen3;I++,J++)
						ISMPEPSig[J] =	MPEPSig[I];

					/* Dealloca il segnale composto MP/EP */
					delete[] MPEPSig;

					/* Inversione e ritardo segnale */
					sputs("Signal delay/reverse.");
					for (I = 0,J = ISSigLen - 1;I < ISSigLen;I++,J--)
						ISRevSig[J] =	ISMPEPSig[I];

					/* Calcolo autocorrelazione e setup inversione */
					sputs("Autocorrelation computation...");
					if (AutoCorrelation(ISMPEPSig,ISSigLen) == False)
						{
							sputs("Autocorrelation computation failed.");
							return 1;
						}
					for (I = ISSigLen / 2; I < ISSigLen; I++)
						ISMPEPSig[I] = 0;

					/* Alloca l'array per l'inversione segnale */
					sputs("Allocating inversion array.");
					ISRevOut = new DLReal[ISSigLen];
					if (ISRevOut == NULL)
						{
							sputs("Memory allocation failed.");
							return 1;
						}

					/* Effettua l'inversione del segnale */
					sputs("Toeplitz least square inversion...");
					if (ToeplitzSolve(ISMPEPSig,ISRevSig,ISRevOut,ISSigLen) != 0)
						{
							sputs("Inversion failed.");
							return 1;
						}

					/* Dealloca gli array */
					delete[] ISRevSig;
					delete[] ISMPEPSig;

					/* Reimposta la lunghezza */
					WLen3 = ISSigLen;
				break;

				/* A fase minima con pre-echo truncation */
				case 'T':
					/* Verifica la dimensione filtro richiesta */
					if (Cfg.ISOutWindow > 0)
						WLen3 = Cfg.ISOutWindow;
					else
						WLen3 = WLen1 + WLen2 - 1;

					/* Alloca l'array per l'inversione segnale */
					sputs("Allocating inversion array.");
					ISRevOut = new DLReal[WLen3];
					if (ISRevOut == NULL)
						{
							sputs("Memory allocation failed.");
							return 1;
						}

					/* Verifica il tipo di funzione di prefiltratura */
					if (Cfg.ISPrefilterFctn[0] == 'P')
						/* Proporzionale */
						SLPType = SLPProportional;
					else
						/* Bilineare */
						SLPType = SLPBilinear;

					/* Inversione a fase minima selettiva */
					sputs("Pre-echo truncation fast deconvolution...");
					if (PETFDInvert(&MPPFSig[WStart1],WLen1,&EPPFSig[WStart2],WLen2,ISRevOut,WLen3,
						Cfg.ISPETType[0],Cfg.ISPELowerWindow,Cfg.ISPEUpperWindow,Cfg.ISPEStartFreq,
						Cfg.ISPEEndFreq,Cfg.ISPEFilterLen,Cfg.ISPEFSharpness,Cfg.ISPEBandSplit,
						Cfg.ISPEWindowExponent,SLPType,Cfg.ISPEOGainFactor,Cfg.BCSampleRate,
						Cfg.ISSMPMultExponent) == False)
						{
							sputs("Inversion failed.");
							return 1;
						}

					/* Dealloca gli array MP/EP finestrati */
					delete[] MPPFSig;
					delete[] EPPFSig;
				break;
			}

		/* Finestratura segnale risultante */
		if (Cfg.ISOutWindow > 0)
			{
				sputs("Inverted signal windowing.");
				WStart2 = (WLen3 - Cfg.ISOutWindow) / 2;
				WLen2 = Cfg.ISOutWindow;
				BlackmanWindow(&ISRevOut[WStart2],WLen2);
			}
		else
			{
				WStart2 = 0;
				WLen2 = WLen3;
				BlackmanWindow(&ISRevOut[WStart2],WLen2);
			}

		/* Normalizzazione segnale risultante */
		if (Cfg.ISNormFactor > 0)
			{
				sputs("Inverted signal normalization.");
				if (SigNormalize(&ISRevOut[WStart2],WLen2,Cfg.ISNormFactor,
					(NormType) Cfg.ISNormType[0]) == False)
					{
						sputs("Normalization failed.");
						return 1;
					}
			}

		/* Verifica se si deve salvare il segnale invertito */
		if (Cfg.ISOutFile != NULL)
			{
				/* Salva la componente MP */
				sputsp("Saving inverted signal: ",Cfg.ISOutFile);
				if (WriteSignal(Cfg.ISOutFile,&ISRevOut[WStart2],WLen2,
					(IFileType) Cfg.ISOutFileType[0]) == False)
					{
						sputs("Inverted signal save failed.");
						return 1;
					}
			}

		/*********************************************************************************/
		/* Calcolo target psicoacustico */
		/*********************************************************************************/

		/* Verifica se il target psicoacustico è abilitato */
		if (Cfg.PTType[0] != 'N')
			{
				/* Alloca l'array per la convoluzione filtro e risposta */
				sputs("Allocating psychoacoustic target reference convolution array.");
				PTTConvLen = WLen2 + MCOutSigLen - 1;
				PTTConv = new DLReal[PTTConvLen];
				if (PTTConv == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}

				/* Effettua la convoluzione tra filtro e risposta */
				sputs("Psychoacoustic target reference convolution...");
				if (DFftConvolve(OInSig,MCOutSigLen,ISRevOut,WLen2,PTTConv) == False)
					{
						sputs("Convolution failed.");
						return 1;
					}

				/* Effettua la finestratura della convoluzione di riferimento */
				PTTRefLen = (PTTConvLen - Cfg.PTReferenceWindow) / 2;
				for (I = 0;I < PTTRefLen;I++)
					PTTConv[I] = (DLReal) 0.0;
				BlackmanWindow(&PTTConv[PTTRefLen],Cfg.PTReferenceWindow);
				for (I = (PTTRefLen + Cfg.PTReferenceWindow);I < PTTConvLen;I++)
					PTTConv[I] = (DLReal) 0.0;

				/* Verifica se si deve effettuare il dip limiting sulla risposta target */
				if (Cfg.PTDLMinGain > 0)
					{
						switch (Cfg.PTDLType[0])
							{
								/* Fase lineare */
								case 'L':
								case 'P':
									sputs("Target reference signal linear phase dip limiting...");
									if (C1LPDipLimit(&PTTConv[PTTRefLen],Cfg.PTReferenceWindow,Cfg.PTDLMinGain,Cfg.PTDLStart,
										Cfg.BCSampleRate,Cfg.PTDLStartFreq,Cfg.PTDLEndFreq,Cfg.PTDLType[0] == 'P',Cfg.PTDLMultExponent) == False)
										{
											sputs("Dip limiting failed.");
											return 1;
										}
								break;

								/* Fase minima */
								case 'M':
								case 'W':
									sputs("Target reference minimum phase dip limiting...");
									if (C1HMPDipLimit(&PTTConv[PTTRefLen],Cfg.PTReferenceWindow,Cfg.PTDLMinGain,Cfg.PTDLStart,
										Cfg.BCSampleRate,Cfg.PTDLStartFreq,Cfg.PTDLEndFreq,Cfg.PTDLType[0] == 'W',Cfg.PTDLMultExponent) == False)
										{
											sputs("Dip limiting failed.");
											return 1;
										}
								break;
							}
					}

				/* Alloca l'array per il calcolo del filtro target */
				sputs("Allocating psychoacoustic target filter array.");
				PTFilter = new DLReal[Cfg.PTFilterLen];
				if (PTFilter == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}

				/* Imposta il tipo filtro target */
				switch (Cfg.PTType[0])
					{
						case 'L':
							TFType = MKSETFLinearPhase;
						break;

						case 'M':
							TFType = MKSETFMinimumPhase;
						break;
					}

				/* Calcola il filtro target psicoacustico */
				sputs("Computing psychoacoustic target filter...");
				if (P2MKSETargetFilter(&PTTConv[PTTRefLen],Cfg.PTReferenceWindow,Cfg.BCSampleRate,
					Cfg.PTBandWidth,Cfg.PTPeakDetectionStrength,PTFilter,TFType,
					Cfg.PTMultExponent,Cfg.PTFilterLen,Cfg.PTDLMinGain,Cfg.PTDLStart,
					Cfg.BCSampleRate,Cfg.PTDLStartFreq,Cfg.PTDLEndFreq) == False)
					{
						sputs("Psychoacoustic target filter computation failed.");
						return 1;
					}

				/* Dealloca l'array per la convoluzione target */
				delete[] PTTConv;

				/* Verifica se si deve salvare il filtro psicoacustico */
				if (Cfg.PTFilterFile != NULL)
					{
						/* Normalizzazione segnale risultante */
						if (Cfg.PTNormFactor > 0)
							{
								sputs("Psychoacoustic target filter normalization.");
								if (SigNormalize(PTFilter,Cfg.PTFilterLen,Cfg.PTNormFactor,
									(NormType) Cfg.PTNormType[0]) == False)
									{
										sputs("Normalization failed.");
										return 1;
									}
							}

						/* Salva la componente MP */
						sputsp("Saving psychoacoustic target filter: ",Cfg.PTFilterFile);
						if (WriteSignal(Cfg.PTFilterFile,PTFilter,Cfg.PTFilterLen,
							(IFileType) Cfg.PTFilterFileType[0]) == False)
							{
								sputs("Psychoacoustic target filter save failed.");
								return 1;
							}
					}

				/* Verifica il tipo di filtro target */
				switch (TFType)
					{
						case MKSETFLinearPhase:
							PTTConvStart = 0;
							PTTConvLen = WLen2 + Cfg.PTFilterLen  - 1;
						break;

						case MKSETFMinimumPhase:
							PTTConvStart = Cfg.PTFilterLen - 1;
							PTTConvLen = WLen2 + 2 * (Cfg.PTFilterLen - 1);
						break;
					}

				/* Alloca l'array per la convoluzione filtro e target */
				sputs("Allocating psychoacoustic target correction filter convolution array.");
				PTTConv = new DLReal[PTTConvLen];
				if (PTTConv == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}
				for (I = 0;I < PTTConvStart;I++)
					PTTConv[I] = (DLReal) 0.0;

				/* Effettua la convoluzione tra filtro e target */
				sputs("Psychoacoustic target correction filter convolution...");
				if (DFftConvolve(PTFilter,Cfg.PTFilterLen,&ISRevOut[WStart2],WLen2,&PTTConv[PTTConvStart]) == False)
					{
						sputs("Convolution failed.");
						return 1;
					}

				/* Dealloca il filtro target */
				delete[] PTFilter;

				/* Finestratura finale filtro risultante */
				if (Cfg.PTOutWindow > 0)
					{
						sputs("Psychoacoustic target correction filter windowing.");

						WStart2 = (PTTConvLen - Cfg.PTOutWindow) / 2;
						WLen2 = Cfg.PTOutWindow;
						BlackmanWindow(&PTTConv[WStart2],WLen2);
					}
				else
					{
						WStart2 = 0;
						WLen2 = PTTConvLen;
					}

				/* Normalizzazione segnale risultante */
				if (Cfg.PTNormFactor > 0)
					{
						sputs("Psychoacoustic target correction filter normalization.");
						if (SigNormalize(&PTTConv[WStart2],WLen2,Cfg.PTNormFactor,
							(NormType) Cfg.PTNormType[0]) == False)
							{
								sputs("Normalization failed.");
								return 1;
							}
					}

				/* Verifica se si deve salvare il filtro correzione psicoacustico */
				if (Cfg.PTOutFile != NULL)
					{
						/* Salva il filtro correzione psicoacustico */
						sputsp("Saving psychoacoustic target correction filter: ",Cfg.PTOutFile);
						if (WriteSignal(Cfg.PTOutFile,&PTTConv[WStart2],WLen2,
							(IFileType) Cfg.PTOutFileType[0]) == False)
							{
								sputs("Psychoacoustic target correction filter save failed.");
								return 1;
							}
					}

				/* Dealloca e riassegna il filtro inverso */
				delete[] ISRevOut;
				ISRevOut = PTTConv;
			}

		/*********************************************************************************/
		/* Peak limiting */
		/*********************************************************************************/

		/* Controlla se si deve effettuare il peak limiting */
		if (Cfg.PLMaxGain > 0)
			{
				switch (Cfg.PLType[0])
					{
						/* Fase lineare */
						case 'L':
						case 'P':
							sputs("Linear phase peak limiting...");
							if (C1LPPeakLimit(&ISRevOut[WStart2],WLen2,Cfg.PLMaxGain,Cfg.PLStart,
								Cfg.BCSampleRate,Cfg.PLStartFreq,Cfg.PLEndFreq,Cfg.PLType[0] == 'P',Cfg.PLMultExponent) == False)
								{
									sputs("Peak limiting failed.");
									return 1;
								}
						break;

						/* Fase minima */
						case 'M':
						case 'W':
							sputs("Minimum phase peak limiting...");
							if (C1HMPPeakLimit(&ISRevOut[WStart2],WLen2,Cfg.PLMaxGain,Cfg.PLStart,
								Cfg.BCSampleRate,Cfg.PLStartFreq,Cfg.PLEndFreq,Cfg.PLType[0] == 'W',Cfg.PLMultExponent) == False)
								{
									sputs("Peak limiting failed.");
									return 1;
								}
						break;
					}
			}

		/* Effettua la finestratura finale */
		if (Cfg.PLOutWindow > 0)
			{
				WStart2 = (WLen2 - Cfg.PLOutWindow) / 2;
				WLen2 = Cfg.PLOutWindow;
				sputs("Peak limited signal final windowing.");
				BlackmanWindow(&ISRevOut[WStart2],WLen2);
			}

		/* Normalizzazione segnale risultante */
		if (Cfg.PLNormFactor > 0)
			{
				sputs("Peak limited signal normalization.");
				if (SigNormalize(&ISRevOut[WStart2],WLen2,Cfg.PLNormFactor,
					(NormType) Cfg.PLNormType[0]) == False)
					{
						sputs("Normalization failed.");
						return 1;
					}
			}

		/* Verifica se si deve salvare il segnale limitato */
		if (Cfg.PLOutFile != NULL)
			{
				/* Salva il segnale limitato*/
				sputsp("Saving peak limited signal: ",Cfg.PLOutFile);
				if (WriteSignal(Cfg.PLOutFile,&ISRevOut[WStart2],WLen2,
					(IFileType) Cfg.PLOutFileType[0]) == False)
					{
						sputs("Peak limited signal save failed.");
						return 1;
					}
			}

		/*********************************************************************************/
		/* Troncatura ringing */
		/*********************************************************************************/

		/* Controlla se è abilitata */
		if (Cfg.RTType[0] != 'N')
			{
				/* Alloca l'array per la troncatura ringing */
				sputs("Allocating ringing truncation array.");
				RTSigLen = Cfg.RTLowerWindow + Cfg.RTFilterLen - 1;
				RTSig = new DLReal[RTSigLen];
				if (RTSig == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}

				/* Azzera l'array */
				for (I = 0;I < RTSigLen;I++)
					RTSig[I] = 0;

				/* Calcola il punto iniziale finestra */
				WStart3 = (WLen2 - Cfg.RTLowerWindow) / 2;

				/* Verifica il tipo di funzione di prefiltratura */
				if (Cfg.RTPrefilterFctn[0] == 'P')
					{
						/* Proporzionale */
						SLPType = SLPProportional;
						BWPType = BWPProportional;
					}
				else
					{
						/* Bilineare */
						SLPType = SLPBilinear;
						BWPType = BWPBilinear;
					}

				/* Prefiltratura componente EP */
				switch (Cfg.RTType[0])
					{
						case 'B':
							sputs("Ringing truncation band windowing.");
							BWPreFilt(&ISRevOut[WStart3],Cfg.RTLowerWindow,Cfg.RTUpperWindow,
								Cfg.RTFilterLen,Cfg.RTBandSplit,Cfg.RTWindowExponent,
								Cfg.BCSampleRate,Cfg.RTStartFreq,Cfg.RTEndFreq,Cfg.RTWindowGap,
								RTSig,WFull,BWPType);
						break;

						case 'b':
							sputs("Ringing truncation single side band windowing.");
							BWPreFilt(&ISRevOut[WStart3],Cfg.RTLowerWindow,Cfg.RTUpperWindow,
								Cfg.RTFilterLen,Cfg.RTBandSplit,Cfg.RTWindowExponent,
								Cfg.BCSampleRate,Cfg.RTStartFreq,Cfg.RTEndFreq,Cfg.RTWindowGap,
								RTSig,WRight,BWPType);
						break;

						case 'S':
							sputs("Ringing truncation sliding lowpass filtering.");
							SLPreFilt(&ISRevOut[WStart3],Cfg.RTLowerWindow,Cfg.RTUpperWindow,
								Cfg.RTFilterLen,Cfg.RTBandSplit,Cfg.RTWindowExponent,
								Cfg.BCSampleRate,Cfg.RTStartFreq,Cfg.RTEndFreq,Cfg.RTWindowGap,
								Cfg.RTFSharpness,RTSig,WFull,SLPType);
						break;

						case 's':
							sputs("Ringing truncation single side sliding lowpass filtering.");
							SLPreFilt(&ISRevOut[WStart3],Cfg.RTLowerWindow,Cfg.RTUpperWindow,
								Cfg.RTFilterLen,Cfg.RTBandSplit,Cfg.RTWindowExponent,
								Cfg.BCSampleRate,Cfg.RTStartFreq,Cfg.RTEndFreq,Cfg.RTWindowGap,
								Cfg.RTFSharpness,RTSig,WRight,SLPType);
						break;
					}

				/* Dealloca il segnale invertito */
				delete[] ISRevOut;

				/* Determina la lunghezza della componente dopo la finestratura */
				if (Cfg.RTOutWindow > 0)
					{
						WStart2 = (RTSigLen - Cfg.RTOutWindow) / 2;
						WLen2 = Cfg.RTOutWindow;
					}
				else
					{
						WStart2 = 0;
						WLen2 = RTSigLen;
					}

				/* Verifica se si deve effettuare la finestratura finale */
				if (Cfg.RTOutWindow > 0)
					{
						sputs("Ringing truncation final windowing.");
						BlackmanWindow(&RTSig[WStart2],WLen2);
					}

				/* Verifica se si deve effettuare rinormalizzazione */
				if (Cfg.RTNormFactor > 0)
					{
						sputs("Ringing truncation normalization.");
						if (SigNormalize(&RTSig[WStart2],WLen2,Cfg.RTNormFactor,
							(NormType) Cfg.RTNormType[0]) == False)
							{
								sputs("Normalization failed.");
								return 1;
							}
					}

				/* Verifica se si deve salvare la troncatura ringing */
				if (Cfg.RTOutFile != NULL)
					{
						/* Salva la componente MP */
						sputsp("Saving ringing truncation: ",Cfg.RTOutFile);
						if (WriteSignal(Cfg.RTOutFile,&RTSig[WStart2],WLen2,
							(IFileType) Cfg.RTOutFileType[0]) == False)
							{
								sputs("Ringing truncation save failed.");
								return 1;
							}
					}

				/* Reimposta il segnale invertito */
				ISRevOut = RTSig;
			}


		/*********************************************************************************/
		/* Applicazione risposta target */
		/*********************************************************************************/

		/* Verifica se si devono contare i punti filtro */
		if (Cfg.PSNumPoints == 0)
			{
				sputsp("Counting target response definition file points: ",Cfg.PSPointsFile);
				Cfg.PSNumPoints = FLineCount(Cfg.PSPointsFile);
				printf("Target response definition file points: %d\n",Cfg.PSNumPoints);
				fflush(stdout);
			}

		/* Alloca gli array per la generazione della risposta target */
		sputs("Allocating target response arrays.");
		PSFilterFreqs = new DLReal[Cfg.PSNumPoints];
		if (PSFilterFreqs == NULL)
			{
				sputs("Memory allocation failed.");
				return 1;
			}
		PSFilterM = new DLReal[Cfg.PSNumPoints];
		if (PSFilterM == NULL)
			{
				sputs("Memory allocation failed.");
				return 1;
			}
		PSFilterP = new DLReal[Cfg.PSNumPoints];
		if (PSFilterP == NULL)
			{
				sputs("Memory allocation failed.");
				return 1;
			}
		PSOutSigLen = Cfg.PSFilterLen + WLen2 - 1;
		PSOutSig = new DLReal[PSOutSigLen];
		if (PSOutSig == NULL)
			{
				sputs("Memory allocation failed.");
				return 1;
			}

		/* Legge i punti del filtro */
		sputsp("Reading target response definition file: ",Cfg.PSPointsFile);
		if (ReadPoints(Cfg.PSPointsFile,(TFMagType) Cfg.PSMagType[0],PSFilterFreqs,
			PSFilterM,PSFilterP,Cfg.PSNumPoints,Cfg.BCSampleRate) == False)
			{
				sputs("Target response point file input failed.");
				return 1;
			}

		/* Verifica il tipo di interpolazione */
		switch(Cfg.PSInterpolationType[0])
			{
				case 'L':
					FIType = Linear;
				break;
				case 'G':
					FIType = Logarithmic;
				break;
				case 'R':
					FIType = SplineLinear;
				break;
				case 'S':
					FIType = SplineLogarithmic;
				break;
				case 'P':
					FIType = PCHIPLinear;
				break;
				case 'H':
					FIType = PCHIPLogarithmic;
				break;
			}

		/* Verifica il tipo di filtro da utilizzare */
		switch (Cfg.PSFilterType[0])
			{
				case 'L':
					/* Alloca gli array per il filtro */
					sputs("Allocating target filter arrays.");
					PSFilter = new DLReal[Cfg.PSFilterLen];
					if (PSFilter == NULL)
						{
							sputs("Memory allocation failed.");
							return 1;
						}
					for (I = 0; I < Cfg.PSFilterLen; I++)
						PSFilter[I] = 0;

					/* Calcola la dimensione richiesta per il calcolo del filtro */
					if (Cfg.PSMultExponent >= 0)
						{
							/* Calcola la potenza di due superiore a Cfg.PSFilterLen */
							for(I = 1;I <= Cfg.PSFilterLen;I <<= 1);
							I *= 1 << Cfg.PSMultExponent;
						}
					else
						I = Cfg.PSFilterLen;

					/* Calcola il filtro */
					sputs("FIR Filter computation...");
					if (GenericFir(PSFilter,Cfg.PSFilterLen,
						PSFilterFreqs,PSFilterM,PSFilterP,Cfg.PSNumPoints,I,FIType) == False)
						{
							sputs("FIR Filter computation failed.");
							return 1;
						}

					/* Effettua la finestratura del filtro */
					BlackmanWindow(PSFilter,Cfg.PSFilterLen);
				break;
				case 'M':
				case 'T':
					/* Alloca gli array per il filtro */
					sputs("Allocating target filter arrays.");
					PSMPFLen = 1 + 2 * Cfg.PSFilterLen;
					PSFilter = new DLReal[PSMPFLen];
					if (PSFilter == NULL)
						{
							sputs("Memory allocation failed.");
							return 1;
						}
					for (I = 0; I < PSMPFLen; I++)
						PSFilter[I] = 0;

					/* Calcola la dimensione richiesta per il calcolo del filtro */
					if (Cfg.PSMultExponent >= 0)
						{
							/* Calcola la potenza di due superiore a Cfg.PSFilterLen */
							for(I = 1;I <= PSMPFLen;I <<= 1);
							I *= 1 << Cfg.PSMultExponent;
						}
					else
						I = PSMPFLen;

					/* Calcola il filtro */
					sputs("FIR Filter computation...");
					if (GenericFir(PSFilter,PSMPFLen,
						PSFilterFreqs,PSFilterM,PSFilterP,Cfg.PSNumPoints,I,FIType) == False)
						{
							sputs("FIR Filter computation failed.");
							return 1;
						}

					/* Alloca gli array per la deconvoluzione omomorfa */
					sputs("Allocating homomorphic deconvolution arrays.");
					MPSig = new DLReal[PSMPFLen];
					if (MPSig == NULL)
						{
							sputs("Memory allocation failed.");
							return 1;
						}

					/* Azzera gli array */
					for (I = 0;I < PSMPFLen;I++)
						MPSig[I] = 0;

					/* Effettua la deconvoluzione omomorfa*/
					sputs("MP target response extraction homomorphic deconvolution stage...");
					if (CepstrumHD(PSFilter,MPSig,NULL,PSMPFLen,
						Cfg.PSMultExponent) == False)
						{
							sputs("Homomorphic deconvolution failed.");
							return 1;
						}

					/* Effettua la finestratura del filtro a fase minima */
					HalfBlackmanWindow(MPSig,Cfg.PSFilterLen,0,WRight);

					/* Copia il filtro a fase minima nell'array filtro */
					for (I = 0;I < Cfg.PSFilterLen;I++)
						PSFilter[I] = MPSig[I];

					/* Dealloca l'array deconvoluzione */
					delete[] MPSig;
				break;
			}

		/* Convoluzione filtro segnale */
		sputs("Target response FIR Filter convolution...");
		if (DFftConvolve(&ISRevOut[WStart2],WLen2,PSFilter,
			Cfg.PSFilterLen,PSOutSig) == False)
			{
				perror("Convolution failed.");
				return 1;
			}

		/* Deallocazione array */
		delete[] ISRevOut;
		delete[] PSFilter;

		/* Determina la dimensione della finestra di uscita */
		if (Cfg.PSOutWindow > 0)
			{
				/* Alloca l'array temporaneo per il filtro */
				PSFilter = new DLReal[Cfg.PSOutWindow];
				if (PSFilter == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}

				/* Verifica il tipo di filtro */
				switch (Cfg.PSFilterType[0])
					{
						case 'L':
							/* Determina la finestratura filtro */
							WStart2 = (PSOutSigLen - Cfg.PSOutWindow) / 2;
							WLen2 = Cfg.PSOutWindow;
							WLen3 = PSOutSigLen;

							/* Salva il filtro per la convoluzione test */
							for (I = 0,J = WStart2;I < WLen2;I++,J++)
								PSFilter[I] = PSOutSig[J];

							/* Effetua la finestratura filtro */
							sputs("Target response signal windowing.");
							BlackmanWindow(PSFilter,WLen2);
						break;
						case 'M':
							/* Determina la finestratura filtro */
							WStart2 = (WLen2 - Cfg.PSOutWindow) / 2;
							WLen3 = WLen2;
							WLen2 = Cfg.PSOutWindow;

							/* Salva il filtro per la convoluzione test */
							for (I = 0,J = WStart2;I < WLen2;I++,J++)
								PSFilter[I] = PSOutSig[J];

							/* Effetua la finestratura filtro */
							sputs("Target response signal windowing.");
							BlackmanWindow(PSFilter,WLen2);
						break;
						case 'T':
							/* Determina la finestratura filtro */
							WStart2 = (WLen2 / 2) - Cfg.ISPELowerWindow;
							WLen3 = WLen2;
							WLen2 = Cfg.PSOutWindow;

							/* Salva il filtro per la convoluzione test */
							for (I = 0,J = WStart2;I < WLen2;I++,J++)
								PSFilter[I] = PSOutSig[J];

							/* Effetua la finestratura filtro */
							sputs("Target response signal windowing.");
							HalfBlackmanWindow(PSFilter,WLen2,Cfg.ISPELowerWindow,WRight);
						break;
					}
			}
		else
			{
				/* Verifica il tipo di filtro */
				switch (Cfg.PSFilterType[0])
					{
						case 'L':
							/* Determina la finestratura filtro */
							WStart2 = 0;
							WLen2 = PSOutSigLen;
							WLen3 = PSOutSigLen;
						case 'M':
							/* Determina la finestratura filtro */
							WStart2 = 0;
							WLen3 = WLen2;
						break;
						case 'T':
							/* Determina la finestratura filtro */
							WStart2 = (WLen2 / 2) - Cfg.ISPELowerWindow;
							WLen3 = WLen2;
							WLen2 = PSOutSigLen - WStart2;
						break;
					}

				/* Alloca l'array temporaneo per il filtro */
				PSFilter = new DLReal[WLen2];
				if (PSFilter == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}

				/* Salva il filtro per la convoluzione test */
				for (I = 0,J = WStart2;I < WLen2;I++,J++)
					PSFilter[I] = PSOutSig[J];
			}

		/* Normalizzazione segnale risultante */
		if (Cfg.PSNormFactor > 0)
			{
				sputs("Target response signal normalization.");
				if (SigNormalize(PSFilter,WLen2,Cfg.PSNormFactor,
					(NormType) Cfg.PSNormType[0]) == False)
					{
						sputs("Normalization failed.");
						return 1;
					}
			}

		/* Verifica se si deve salvare il segnale risposta target */
		if (Cfg.PSOutFile != NULL)
			{
				/* Salva la componente MP */
				sputsp("Saving Target response signal: ",Cfg.PSOutFile);
				if (WriteSignal(Cfg.PSOutFile,PSFilter,WLen2,
					(IFileType) Cfg.PSOutFileType[0]) == False)
					{
						sputs("Target response signal save failed.");
						return 1;
					}
			}

		/* Deallocazione array */
		delete[] PSFilterFreqs;
		delete[] PSFilterM;
		delete[] PSFilterP;

    /* Reimposta la lunghezza filtro */
    PSOutSigLen = WLen2;

		/*********************************************************************************/
		/* Estrazione filtro a fase minima */
		/*********************************************************************************/

		/* Verifica se deve essere estratto il filtro a fase minima */
		if (Cfg.MSOutFile != NULL)
			{
				/* Alloca gli array per la deconvoluzione omomorfa */
				sputs("Allocating homomorphic deconvolution arrays.");
				PSMPFLen = Cfg.MSFilterDelay + WLen2;
				MPSig = new DLReal[PSMPFLen];
				if (MPSig == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}

				/* Azzera gli array */
				for (I = 0;I < PSMPFLen;I++)
					MPSig[I] = 0;

				/* Effettua la deconvoluzione omomorfa*/
				sputs("MP filter extraction homomorphic deconvolution stage...");
				if (CepstrumHD(&PSOutSig[WStart2],&MPSig[Cfg.MSFilterDelay],NULL,
					WLen2,Cfg.MSMultExponent) == False)
					{
						sputs("Homomorphic deconvolution failed.");
						return 1;
					}

				/* Verifica se si deve finestrare il filtro */
				sputs("MP filter extraction windowing.");
				if (Cfg.MSOutWindow > 0)
					{
						HalfBlackmanWindow(&MPSig[Cfg.MSFilterDelay],Cfg.MSOutWindow - Cfg.MSFilterDelay,0,WRight);
						WLen1 = Cfg.MSOutWindow;
					}
				else
					WLen1 = PSMPFLen;

				/* Normalizzazione segnale risultante */
				if (Cfg.MSNormFactor > 0)
					{
						sputs("Minimum phase filter normalization.");
						if (SigNormalize(MPSig,WLen1,Cfg.MSNormFactor,
							(NormType) Cfg.MSNormType[0]) == False)
							{
								sputs("Normalization failed.");
								return 1;
							}
					}

				/* Salva il il filtro a fase minima */
				sputsp("Saving MP filter signal: ",Cfg.MSOutFile);
				if (WriteSignal(Cfg.MSOutFile,MPSig,WLen1,
					(IFileType) Cfg.MSOutFileType[0]) == False)
					{
						sputs("MP filter signal save failed.");
						return 1;
					}

				/* Dealloca l'array deconvoluzione */
				delete[] MPSig;
			}

		/* Deallocazione array */
		delete[] PSOutSig;

		/*********************************************************************************/
		/* Convoluzione di test */
		/*********************************************************************************/

		/* Verifica se va effettuata la convoluzione finale */
		if (Cfg.TCOutFile != NULL)
			{
				/* Alloca l'array per la convoluzione finale */
				sputs("Allocating test convolution arrays.");
				TCSigLen = MCOutSigLen + PSOutSigLen - 1;
				TCSig = new DLReal[TCSigLen];
				if (TCSig == NULL)
					{
						sputs("Memory allocation failed.");
						return 1;
					}

				/* Effettua la convoluzione */
				sputs("Convolving input signal with target response signal...");
				if (DFftConvolve(OInSig,MCOutSigLen,PSFilter,PSOutSigLen,TCSig) == False)
					{
						sputs("Convolution failed.");
						return 1;
					}

				/* Calcola il valore RMS del segnale dopo la filtratura */
				SRMSValue = GetRMSLevel(TCSig,TCSigLen);
				if (SRMSValue >= 0)
					printf("Filtered signal RMS level %f (%f dB).\n",(double) SRMSValue, (double) (20 * log10((double) SRMSValue)));
				else
					printf("Filtered signal RMS level %f (-inf dB).\n",(double) SRMSValue);
				fflush(stdout);

				/* Normalizzazione segnale risultante */
				if (Cfg.TCNormFactor > 0)
					{
						sputs("Test convolution signal normalization.");
						if (SigNormalize(TCSig,TCSigLen,Cfg.TCNormFactor,
							(NormType) Cfg.TCNormType[0]) == False)
							{
								sputs("Normalization failed.");
								return 1;
							}
					}

				/* Calcola la dimensione in uscita */
				if (Cfg.PSFilterType[0] == 'T')
					WLen3 = MCOutSigLen + 2 * Cfg.ISPELowerWindow;
				else
					WLen3 = TCSigLen;

				/* Salva il segnale convoluzione test */
				sputsp("Saving test convolution signal: ",Cfg.TCOutFile);
				if (WriteSignal(Cfg.TCOutFile,TCSig,WLen3,
					(IFileType) Cfg.TCOutFileType[0]) == False)
					{
						sputs("Test convolution save failed.");
						return 1;
					}

				/* Effettua la sovrascrittura del segnale convoluzione test */
				if (Cfg.TCOWFile != NULL)
					{
						sputsp("Saving test convolution overwrite: ",Cfg.TCOWFile);

						/* Normalizzazione segnale risultante */
						if (Cfg.TCOWNormFactor > 0)
							{
								sputs("Test convolution overwrite signal normalization.");
								if (SigNormalize(TCSig,TCSigLen,Cfg.TCOWNormFactor,
									(NormType) Cfg.TCOWNormType[0]) == False)
									{
										sputs("Normalization failed.");
										return 1;
									}
							}

						/* Controlla il tipo di filtro */
						if (Cfg.PSFilterType[0] == 'T')
							{

								if (((MCOutSigLen / 2 + Cfg.ISPELowerWindow) - Cfg.TCOWPrewindow) < (TCSigLen - Cfg.TCOWLength))
									WLen3 = Cfg.TCOWLength;
								else
									WLen3 = TCSigLen - ((MCOutSigLen / 2 + Cfg.ISPELowerWindow) - Cfg.TCOWPrewindow);

								if (OverwriteSignal(Cfg.TCOWFile,&TCSig[(MCOutSigLen / 2 + Cfg.ISPELowerWindow) - Cfg.TCOWPrewindow],
									WLen3,Cfg.TCOWSkip,(IFileType) Cfg.TCOWFileType[0]) == False)
									{
										sputs("Test convolution overwrite failed.");
										return 1;
									}
							}
						else
							{
								if ((TCSigLen / 2 - Cfg.TCOWPrewindow) < (TCSigLen - Cfg.TCOWLength))
									WLen3 = Cfg.TCOWLength;
								else
									WLen3 = TCSigLen / 2 + Cfg.TCOWPrewindow;

								if (OverwriteSignal(Cfg.TCOWFile,&TCSig[TCSigLen / 2 - Cfg.TCOWPrewindow],
									WLen3,Cfg.TCOWSkip,(IFileType) Cfg.TCOWFileType[0]) == False)
									{
										sputs("Test convolution overwrite failed.");
										return 1;
									}
							}
					}

				/* Dealloca gli array temporanei */
				delete[] TCSig;
			}

		/* Dealloca gli array temporanei */
		if (Cfg.TCOutFile != NULL || Cfg.PTType[0] != 'N')
			delete[] OInSig;

		/* Dealloca il filtro convoluzione test */
		delete[] PSFilter;

		/* Libera la memoria della struttura di configurazione */
		CfgFree(CfgParmsDef);
		free(DRCFile);

		/* Esecuzione completata */
		sputs("Execution completed.");

		/* Segnala la durata */
		printf("Total computing time: %lu s\n",(unsigned long int) (time(NULL) - CStart));
		fflush(stdout);

		return 0;
	}
