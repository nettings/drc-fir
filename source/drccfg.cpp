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

/* Opzioni di configurazione DRC */

/* Inclusioni */
#include "drccfg.h"
#include "baselib.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

/* Opzioni di configurazione */
CfgParmsType Cfg;

/* Definizione struttura file di configurazione */
CfgParameter CfgParmsDef[] =
	{
		/* Base configuration */
		{ (char *) "BCBaseDir",CfgString,&Cfg.BCBaseDir },
		{ (char *) "BCInFile",CfgString,&Cfg.BCInFile },
		{ (char *) "BCInFileType",CfgString,&Cfg.BCInFileType },
		{ (char *) "BCSampleRate",CfgInt,&Cfg.BCSampleRate },
		{ (char *) "BCImpulseCenterMode",CfgString,&Cfg.BCImpulseCenterMode },
		{ (char *) "BCImpulseCenter",CfgInt,&Cfg.BCImpulseCenter },
		{ (char *) "BCInitWindow",CfgInt,&Cfg.BCInitWindow },
		{ (char *) "BCPreWindowLen",CfgInt,&Cfg.BCPreWindowLen },
		{ (char *) "BCPreWindowGap",CfgInt,&Cfg.BCPreWindowGap },
		{ (char *) "BCNormFactor",DRCCfgFloat,&Cfg.BCNormFactor },
		{ (char *) "BCNormType",CfgString,&Cfg.BCNormType },

    /* Mic correction stage */
		{ (char *) "MCFilterType",CfgString,&Cfg.MCFilterType },
		{ (char *) "MCInterpolationType",CfgString,&Cfg.MCInterpolationType },
		{ (char *) "MCMultExponent",CfgInt,&Cfg.MCMultExponent },
		{ (char *) "MCFilterLen",CfgInt,&Cfg.MCFilterLen },
		{ (char *) "MCNumPoints",CfgInt,&Cfg.MCNumPoints },
		{ (char *) "MCPointsFile",CfgString,&Cfg.MCPointsFile },
		{ (char *) "MCMagType",CfgString,&Cfg.MCMagType },
		{ (char *) "MCFilterFile",CfgString,&Cfg.MCFilterFile },
		{ (char *) "MCFilterFileType",CfgString,&Cfg.MCFilterFileType },
		{ (char *) "MCOutWindow",CfgInt,&Cfg.MCOutWindow },
		{ (char *) "MCNormFactor",DRCCfgFloat,&Cfg.MCNormFactor },
		{ (char *) "MCNormType",CfgString,&Cfg.MCNormType },
		{ (char *) "MCOutFile",CfgString,&Cfg.MCOutFile },
		{ (char *) "MCOutFileType",CfgString,&Cfg.MCOutFileType },

		/* Base configuration dip limiting stage */
		{ (char *) "BCDLType",CfgString,&Cfg.BCDLType },
		{ (char *) "BCDLMinGain",DRCCfgFloat,&Cfg.BCDLMinGain },
		{ (char *) "BCDLStartFreq",DRCCfgFloat,&Cfg.BCDLStartFreq },
		{ (char *) "BCDLEndFreq",DRCCfgFloat,&Cfg.BCDLEndFreq },
		{ (char *) "BCDLStart",DRCCfgFloat,&Cfg.BCDLStart },
		{ (char *) "BCDLMultExponent",CfgInt,&Cfg.BCDLMultExponent },

		/* Homomorphic Deconvolution */
		{ (char *) "HDMultExponent",CfgInt,&Cfg.HDMultExponent },
		{ (char *) "HDMPNormFactor",DRCCfgFloat,&Cfg.HDMPNormFactor },
		{ (char *) "HDMPNormType",CfgString,&Cfg.HDMPNormType },
		{ (char *) "HDMPOutFile",CfgString,&Cfg.HDMPOutFile },
		{ (char *) "HDMPOutFileType",CfgString,&Cfg.HDMPOutFileType },
		{ (char *) "HDEPNormFactor",DRCCfgFloat,&Cfg.HDEPNormFactor },
		{ (char *) "HDEPNormType",CfgString,&Cfg.HDEPNormType },
		{ (char *) "HDEPOutFile",CfgString,&Cfg.HDEPOutFile },
		{ (char *) "HDEPOutFileType",CfgString,&Cfg.HDEPOutFileType },

		/* Minimum phase prefiltering stage */
		{ (char *) "MPPrefilterType",CfgString,&Cfg.MPPrefilterType },
		{ (char *) "MPPrefilterFctn",CfgString,&Cfg.MPPrefilterFctn },
		{ (char *) "MPWindowGap",CfgInt,&Cfg.MPWindowGap },
		{ (char *) "MPLowerWindow",CfgInt,&Cfg.MPLowerWindow },
		{ (char *) "MPUpperWindow",CfgInt,&Cfg.MPUpperWindow },
		{ (char *) "MPStartFreq",DRCCfgFloat,&Cfg.MPStartFreq },
		{ (char *) "MPEndFreq",DRCCfgFloat,&Cfg.MPEndFreq },
		{ (char *) "MPWindowExponent",DRCCfgFloat,&Cfg.MPWindowExponent },
		{ (char *) "MPFilterLen",CfgInt,&Cfg.MPFilterLen },
		{ (char *) "MPFSharpness",DRCCfgFloat,&Cfg.MPFSharpness },
		{ (char *) "MPBandSplit",CfgInt,&Cfg.MPBandSplit },
		{ (char *) "MPHDRecover",CfgString,&Cfg.MPHDRecover },
		{ (char *) "MPEPPreserve",CfgString,&Cfg.MPEPPreserve },
		{ (char *) "MPHDMultExponent",CfgInt,&Cfg.MPHDMultExponent },
		{ (char *) "MPPFFinalWindow",CfgInt,&Cfg.MPPFFinalWindow },
		{ (char *) "MPPFNormFactor",DRCCfgFloat,&Cfg.MPPFNormFactor },
		{ (char *) "MPPFNormType",CfgString,&Cfg.MPPFNormType },
		{ (char *) "MPPFOutFile",CfgString,&Cfg.MPPFOutFile },
		{ (char *) "MPPFOutFileType",CfgString,&Cfg.MPPFOutFileType },

		/* Dip limiting stage */
		{ (char *) "DLType",CfgString,&Cfg.DLType },
		{ (char *) "DLMinGain",DRCCfgFloat,&Cfg.DLMinGain },
		{ (char *) "DLStartFreq",DRCCfgFloat,&Cfg.DLStartFreq },
		{ (char *) "DLEndFreq",DRCCfgFloat,&Cfg.DLEndFreq },
		{ (char *) "DLStart",DRCCfgFloat,&Cfg.DLStart },
		{ (char *) "DLMultExponent",CfgInt,&Cfg.DLMultExponent },

		/* Excess fase phase prefiltering stage */
		{ (char *) "EPPrefilterType",CfgString,&Cfg.EPPrefilterType },
		{ (char *) "EPPrefilterFctn",CfgString,&Cfg.EPPrefilterFctn },
		{ (char *) "EPWindowGap",CfgInt,&Cfg.EPWindowGap },
		{ (char *) "EPLowerWindow",CfgInt,&Cfg.EPLowerWindow },
		{ (char *) "EPUpperWindow",CfgInt,&Cfg.EPUpperWindow },
		{ (char *) "EPStartFreq",DRCCfgFloat,&Cfg.EPStartFreq },
		{ (char *) "EPEndFreq",DRCCfgFloat,&Cfg.EPEndFreq },
		{ (char *) "EPWindowExponent",DRCCfgFloat,&Cfg.EPWindowExponent },
		{ (char *) "EPFilterLen",CfgInt,&Cfg.EPFilterLen },
		{ (char *) "EPFSharpness",DRCCfgFloat,&Cfg.EPFSharpness },
		{ (char *) "EPBandSplit",CfgInt,&Cfg.EPBandSplit },
		{ (char *) "EPPFFlatGain",DRCCfgFloat,&Cfg.EPPFFlatGain },
		{ (char *) "EPPFOGainFactor",DRCCfgFloat,&Cfg.EPPFOGainFactor },
		{ (char *) "EPPFFlatType",CfgString,&Cfg.EPPFFlatType },
		{ (char *) "EPPFFGMultExponent",CfgInt,&Cfg.EPPFFGMultExponent },
		{ (char *) "EPPFFinalWindow",CfgInt,&Cfg.EPPFFinalWindow },
		{ (char *) "EPPFNormFactor",DRCCfgFloat,&Cfg.EPPFNormFactor },
		{ (char *) "EPPFNormType",CfgString,&Cfg.EPPFNormType },
		{ (char *) "EPPFOutFile",CfgString,&Cfg.EPPFOutFile },
		{ (char *) "EPPFOutFileType",CfgString,&Cfg.EPPFOutFileType },

		/* Prefiltering completion stage */
		{ (char *) "PCOutWindow",CfgInt,&Cfg.PCOutWindow },
		{ (char *) "PCNormFactor",DRCCfgFloat,&Cfg.PCNormFactor },
		{ (char *) "PCNormType",CfgString,&Cfg.PCNormType },
		{ (char *) "PCOutFile",CfgString,&Cfg.PCOutFile },
		{ (char *) "PCOutFileType",CfgString,&Cfg.PCOutFileType },

		/* Inversion stage */
		{ (char *) "ISType",CfgString,&Cfg.ISType },
		{ (char *) "ISPETType",CfgString,&Cfg.ISPETType },
		{ (char *) "ISPrefilterFctn",CfgString,&Cfg.ISPrefilterFctn },
		{ (char *) "ISPELowerWindow",CfgInt,&Cfg.ISPELowerWindow },
		{ (char *) "ISPEUpperWindow",CfgInt,&Cfg.ISPEUpperWindow },
		{ (char *) "ISPEStartFreq",CfgInt,&Cfg.ISPEStartFreq },
		{ (char *) "ISPEEndFreq",CfgInt,&Cfg.ISPEEndFreq },
		{ (char *) "ISPEFilterLen",CfgInt,&Cfg.ISPEFilterLen },
		{ (char *) "ISPEFSharpness",DRCCfgFloat,&Cfg.ISPEFSharpness },
		{ (char *) "ISPEBandSplit",CfgInt,&Cfg.ISPEBandSplit },
		{ (char *) "ISPEWindowExponent",DRCCfgFloat,&Cfg.ISPEWindowExponent },
		{ (char *) "ISPEOGainFactor",DRCCfgFloat,&Cfg.ISPEOGainFactor },
		{ (char *) "ISSMPMultExponent",CfgInt,&Cfg.ISSMPMultExponent },
		{ (char *) "ISOutWindow",CfgInt,&Cfg.ISOutWindow },
		{ (char *) "ISNormFactor",DRCCfgFloat,&Cfg.ISNormFactor },
		{ (char *) "ISNormType",CfgString,&Cfg.ISNormType },
		{ (char *) "ISOutFile",CfgString,&Cfg.ISOutFile },
		{ (char *) "ISOutFileType",CfgString,&Cfg.ISOutFileType },

		/* Psychoacoustic target stage */
		{ (char *) "PTType",CfgString,&Cfg.PTType },
		{ (char *) "PTReferenceWindow",CfgInt,&Cfg.PTReferenceWindow },
		{ (char *) "PTDLType",CfgString,&Cfg.PTDLType },
		{ (char *) "PTDLMinGain",DRCCfgFloat,&Cfg.PTDLMinGain },
		{ (char *) "PTDLStart",DRCCfgFloat,&Cfg.PTDLStart },
		{ (char *) "PTDLStartFreq",DRCCfgFloat,&Cfg.PTDLStartFreq },
		{ (char *) "PTDLEndFreq",DRCCfgFloat,&Cfg.PTDLEndFreq },
		{ (char *) "PTDLMultExponent",CfgInt,&Cfg.PTDLMultExponent },
		{ (char *) "PTBandWidth",DRCCfgFloat,&Cfg.PTBandWidth },
		{ (char *) "PTPeakDetectionStrength",DRCCfgFloat,&Cfg.PTPeakDetectionStrength },
		{ (char *) "PTMultExponent",CfgInt,&Cfg.PTMultExponent },
		{ (char *) "PTFilterLen",CfgInt,&Cfg.PTFilterLen },
		{ (char *) "PTFilterFile",CfgString,&Cfg.PTFilterFile },
		{ (char *) "PTFilterFileType",CfgString,&Cfg.PTFilterFileType },
		{ (char *) "PTNormFactor",DRCCfgFloat,&Cfg.PTNormFactor },
		{ (char *) "PTNormType",CfgString,&Cfg.PTNormType },
		{ (char *) "PTOutFile",CfgString,&Cfg.PTOutFile },
		{ (char *) "PTOutFileType",CfgString,&Cfg.PTOutFileType },
		{ (char *) "PTOutWindow",CfgInt,&Cfg.PTOutWindow },

		/* Peak limiting stage */
		{ (char *) "PLType",CfgString,&Cfg.PLType },
		{ (char *) "PLMaxGain",DRCCfgFloat,&Cfg.PLMaxGain },
		{ (char *) "PLStart",DRCCfgFloat,&Cfg.PLStart },
		{ (char *) "PLStartFreq",DRCCfgFloat,&Cfg.PLStartFreq },
		{ (char *) "PLEndFreq",DRCCfgFloat,&Cfg.PLEndFreq },
		{ (char *) "PLMultExponent",CfgInt,&Cfg.PLMultExponent },
		{ (char *) "PLOutWindow",CfgInt,&Cfg.PLOutWindow },
		{ (char *) "PLNormFactor",DRCCfgFloat,&Cfg.PLNormFactor },
		{ (char *) "PLNormType",CfgString,&Cfg.PLNormType },
		{ (char *) "PLOutFile",CfgString,&Cfg.PLOutFile },
		{ (char *) "PLOutFileType",CfgString,&Cfg.PLOutFileType },

		/* Ringing truncation stage */
		{ (char *) "RTType",CfgString,&Cfg.RTType },
		{ (char *) "RTPrefilterFctn",CfgString,&Cfg.RTPrefilterFctn },
		{ (char *) "RTWindowGap",CfgInt,&Cfg.RTWindowGap },
		{ (char *) "RTLowerWindow",CfgInt,&Cfg.RTLowerWindow },
		{ (char *) "RTUpperWindow",CfgInt,&Cfg.RTUpperWindow },
		{ (char *) "RTStartFreq",DRCCfgFloat,&Cfg.RTStartFreq },
		{ (char *) "RTEndFreq",DRCCfgFloat,&Cfg.RTEndFreq },
		{ (char *) "RTWindowExponent",DRCCfgFloat,&Cfg.RTWindowExponent },
		{ (char *) "RTFilterLen",CfgInt,&Cfg.RTFilterLen },
		{ (char *) "RTFSharpness",DRCCfgFloat,&Cfg.RTFSharpness },
		{ (char *) "RTBandSplit",CfgInt,&Cfg.RTBandSplit },
		{ (char *) "RTOutWindow",CfgInt,&Cfg.RTOutWindow },
		{ (char *) "RTNormFactor",DRCCfgFloat,&Cfg.RTNormFactor },
		{ (char *) "RTNormType",CfgString,&Cfg.RTNormType },
		{ (char *) "RTOutFile",CfgString,&Cfg.RTOutFile },
		{ (char *) "RTOutFileType",CfgString,&Cfg.RTOutFileType },

		/* Target response stage */
		{ (char *) "PSFilterType",CfgString,&Cfg.PSFilterType },
		{ (char *) "PSInterpolationType",CfgString,&Cfg.PSInterpolationType },
		{ (char *) "PSMultExponent",CfgInt,&Cfg.PSMultExponent },
		{ (char *) "PSFilterLen",CfgInt,&Cfg.PSFilterLen },
		{ (char *) "PSNumPoints",CfgInt,&Cfg.PSNumPoints },
		{ (char *) "PSPointsFile",CfgString,&Cfg.PSPointsFile },
		{ (char *) "PSMagType",CfgString,&Cfg.PSMagType },
		{ (char *) "PSOutWindow",CfgInt,&Cfg.PSOutWindow },
		{ (char *) "PSNormFactor",DRCCfgFloat,&Cfg.PSNormFactor },
		{ (char *) "PSNormType",CfgString,&Cfg.PSNormType },
		{ (char *) "PSOutFile",CfgString,&Cfg.PSOutFile },
		{ (char *) "PSOutFileType",CfgString,&Cfg.PSOutFileType },

		/* Minimum phase filter extraction stage */
		{ (char *) "MSMultExponent",CfgInt,&Cfg.MSMultExponent },
		{ (char *) "MSOutWindow",CfgInt,&Cfg.MSOutWindow },
		{ (char *) "MSFilterDelay",CfgInt,&Cfg.MSFilterDelay },
		{ (char *) "MSNormFactor",DRCCfgFloat,&Cfg.MSNormFactor },
		{ (char *) "MSNormType",CfgString,&Cfg.MSNormType },
		{ (char *) "MSOutFile",CfgString,&Cfg.MSOutFile },
		{ (char *) "MSOutFileType",CfgString,&Cfg.MSOutFileType },

		/* Test convolution stage */
		{ (char *) "TCNormFactor",DRCCfgFloat,&Cfg.TCNormFactor },
		{ (char *) "TCNormType",CfgString,&Cfg.TCNormType },
		{ (char *) "TCOutFile",CfgString,&Cfg.TCOutFile },
		{ (char *) "TCOutFileType",CfgString,&Cfg.TCOutFileType },
		{ (char *) "TCOWFile",CfgString,&Cfg.TCOWFile },
		{ (char *) "TCOWFileType",CfgString,&Cfg.TCOWFileType },
		{ (char *) "TCOWNormFactor",DRCCfgFloat,&Cfg.TCOWNormFactor },
		{ (char *) "TCOWNormType",CfgString,&Cfg.TCOWNormType },
		{ (char *) "TCOWSkip",CfgInt,&Cfg.TCOWSkip },
		{ (char *) "TCOWPrewindow",CfgInt,&Cfg.TCOWPrewindow },
		{ (char *) "TCOWLength",CfgInt,&Cfg.TCOWLength },

		/* Chiusura lista parametri */
		{NULL, CfgEnd, NULL}
	};

/* Elenco dei parametri di configurazione che richiedono gestione della
directory base */
static const char * BaseDirParmsList[] =
	{
		"BCInFile",
		"HDMPOutFile",
		"HDEPOutFile",
		"MPPFOutFile",
		"EPPFOutFile",
		"PCOutFile",
		"ISOutFile",
		"PTFilterFile",
		"PTOutFile",
		"PLOutFile",
		"RTOutFile",
		"PSPointsFile",
		"PSOutFile",
		"MCPointsFile",
		"MCOutFile",
		"MSOutFile",
		"TCOutFile",
		"TCOWFile",
		NULL
	};

/* Concatena due stringhe allocando lo spazio necessario */
static char * StrJoin(const char * S1, const char * S2)
	{
		char * SJ;
		if ((SJ = (char *) malloc((1 + strlen(S1) + strlen(S2)) * sizeof(char))) == NULL)
			return NULL;
		strcpy(SJ,S1);
		strcat(SJ,S2);
		return SJ;
	}

/* Ritorna la posizione del parametro indicato */
static int GetCfgParmPos(const CfgParameter * CfgParmsDef, const char * ParmName)
	{
		int P = 0;
		while (CfgParmsDef[P].PType != CfgEnd)
			{
				if (strcmp(CfgParmsDef[P].PName,ParmName) == 0)
					return P;
				P++;
			}
		return -1;
	}

/* Impostazione directory base di lavoro */
int SetupDRCCfgBaseDir(CfgParmsType * DRCCfg, const CfgParameter * CfgParmsDef,
	const CmdLineType * OptData)
	{
		/* Parametro corrente */
		int P;

		/* Indice verifica parametri */
		int I;

		/* Concatenazione stringhe */
		char * S;

		/* Verifica che la directory base sia presente. */
		if (DRCCfg->BCBaseDir == NULL)
			return 0;
		if (strlen(DRCCfg->BCBaseDir) == 0)
			return 0;

		/* Ciclo sui parametri modificabili */
		I = 0;
		while (BaseDirParmsList[I] != NULL)
			{
				/* Recupera la posizione del parametro */
				P = GetCfgParmPos(CfgParmsDef,BaseDirParmsList[I]);

				/* Verifica se il parametro è stato impostato
				a linea di comando */
				if ((OptData->ParmSet[P] == False ||
					OptData->ParmSet[BCBaseDirParmPos] == True) &&
					(*((void **) CfgParmsDef[P].PValue) != NULL))
					{
						/* Aggiunge la directory base al parametro */
						S = StrJoin(DRCCfg->BCBaseDir,
							(char *) (* ((void **) CfgParmsDef[P].PValue)));

						/* Verifica che la concatenazione sia riuscita */
						if (S == NULL)
							return 1;

						/* Dealloca la stringa precedente */
						free(*((void **) CfgParmsDef[P].PValue));

						/* Sostituisce la stringa precedente */
						*((void **) CfgParmsDef[P].PValue) = (void *) S;
					}

				/* Passa al parametro successivo */
				I++;
			}

		/* Operazione completata */
		return 0;
	}

/* Controllo validità parametri di configurazione */
int CheckDRCCfg(const CfgParmsType * DRCCfg)
	{
		/* Calcolo finestra validazione parametri */
		int PWLen;
		int IWLen;

		/* Stringa temporanea errori */
		char TStr[256];

		/* Calcolo dimensione massima target psicoacustico */
		int PTLen;

		/*********************************************************************************/
		/* Importazione iniziale risposta all'impulso */
		/*********************************************************************************/

		if (DRCCfg->BCInFile == NULL)
			{
				sputs("BC->BCInFile: No input file name supplied.");
				return 1;
			}
		if (DRCCfg->BCInFileType == NULL)
			{
				sputs("BC->BCInFileType: No input file type supplied.");
				return 1;
			}
		if (DRCCfg->BCImpulseCenterMode == NULL)
			{
				sputs("BC->BCImpulseCenterMode: No impulse center mode supplied.");
				return 1;
			}
		if (DRCCfg->BCNormFactor > 0 && DRCCfg->BCNormType == NULL)
			{
				sputs("BC->BCNormType: No input normalization type supplied.");
				return 1;
			}
		if (DRCCfg->BCInitWindow < 3)
			{
				sputs("BC->BCInitWindow: Initial window should be at least 3.");
				return 1;
			}
		if (DRCCfg->BCPreWindowGap < 0)
			{
				sputs("BC->BCPreWindowGap: BCPreWindowGap can't be less than 0.");
				return 1;
			}
		if (DRCCfg->BCPreWindowLen > 0)
			if (DRCCfg->BCPreWindowLen + DRCCfg->BCPreWindowGap > DRCCfg->BCInitWindow)
				{
					sputs("BC: BCPreWindowLen + BCPreWindowGap must be less than BCInitWindow.");
					return 1;
				}

    /*********************************************************************************/
		/* Compensazione microfono */
		/*********************************************************************************/

		if (DRCCfg->MCFilterType == NULL)
			{
				sputs("MC->MCFilterType: No filter type supplied.");
				return 1;
			}
    if (DRCCfg->MCFilterType[0] != 'N')
      {
        if (DRCCfg->MCFilterType[0] != 'L' && DRCCfg->MCFilterType[0] != 'M'
          && DRCCfg->MCFilterType[0] != 'N')
          {
            sputs("MC->MCFilterType: Invalid filter type supplied.");
            return 1;
          }
        if (DRCCfg->MCInterpolationType == NULL)
          {
            sputs("MC->MCInterpolationType: No interpolation type supplied.");
            return 1;
          }
        if (DRCCfg->MCInterpolationType[0] != 'L' && DRCCfg->MCInterpolationType[0] != 'G'
          && DRCCfg->MCInterpolationType[0] != 'R' && DRCCfg->MCInterpolationType[0] != 'S'
          && DRCCfg->MCInterpolationType[0] != 'P' && DRCCfg->MCInterpolationType[0] != 'H')
          {
            sputs("MC->MCInterpolationType: Invalid interpolation type supplied.");
            return 1;
          }
        if (DRCCfg->MCMagType == NULL)
          {
            sputs("MC->MCMagType: No filter definition magnitude type supplied.");
            return 1;
          }
        if (DRCCfg->MCMagType[0] != 'L' && DRCCfg->MCMagType[0] != 'D')
          {
            sputs("MC->MCMagType: Invalid filter definition magnitude type supplied.");
            return 1;
          }
        if (DRCCfg->MCPointsFile == NULL)
          {
            sputs("MC->MCPointsFile: No correction point file supplied.");
            return 1;
          }
        if (DRCCfg->MCNumPoints < 0 || DRCCfg->MCNumPoints == 1)
          {
            sputs("MC->MCNumPoints: Invalid MCNumPoints supplied, it must be 0 or at least 2.");
            return 1;
          }
        if (DRCCfg->MCFilterFile != NULL && DRCCfg->MCFilterFileType == NULL)
          {
            sputs("MC->MCFilterFileType: No MCFilterFileType supplied.");
            return 1;
          }
        if (DRCCfg->MCNormFactor > 0 && DRCCfg->MCNormType == NULL)
          {
            sputs("MC->MCNormType: No normalization type supplied.");
            return 1;
          }
        if (DRCCfg->MCOutFile != NULL && DRCCfg->MCOutFileType == NULL)
          {
            sputs("MC->MCOutFileType: No output file type supplied.");
            return 1;
          }
        if (DRCCfg->MCFilterLen <= 0)
          {
            sputs("MC->MCFilterLen: MCFilterLen must be greater than 0.");
            return 1;
          }
        switch (DRCCfg->MCFilterType[0])
          {
            case 'L':
              if (DRCCfg->MCOutWindow > DRCCfg->BCInitWindow + DRCCfg->MCFilterLen - 1)
                {
                  sprintf(TStr,"%d.",DRCCfg->BCInitWindow + DRCCfg->MCFilterLen - 1);
                  sputsp("MC->MCOutWindow: MCOutWindow too big. Max allowed value: ", TStr);
                  return 1;
                }

              if (DRCCfg->MCOutWindow > 0)
                IWLen = DRCCfg->MCOutWindow;
              else
                IWLen = DRCCfg->BCInitWindow + DRCCfg->MCFilterLen - 1;
            break;
            case 'M':
              if (DRCCfg->MCOutWindow > DRCCfg->BCInitWindow)
                {
                  sprintf(TStr,"%d.",DRCCfg->BCInitWindow);
                  sputsp("MC->MCOutWindow: MCOutWindow too big. Max allowed value: ", TStr);
                  return 1;
                }

              if (DRCCfg->MCOutWindow > 0)
                IWLen = DRCCfg->MCOutWindow;
              else
                IWLen = DRCCfg->BCInitWindow;
            break;
          }
      }
    else
      IWLen = DRCCfg->BCInitWindow;

		/*********************************************************************************/
		/* Base configuration dip limiting */
		/*********************************************************************************/

		if (DRCCfg->BCDLType == NULL)
			{
				sputs("BC->BCDLType: No BCDLType supplied.");
				return 1;
			}
		if (DRCCfg->BCDLType[0] != 'L' && DRCCfg->BCDLType[0] != 'M'
				&& DRCCfg->BCDLType[0] != 'P' && DRCCfg->BCDLType[0] != 'W')
			{
				sputs("BC->BCDLType: Invalid dip limiting type supplied.");
				return 1;
			}
		if (DRCCfg->BCDLStart < (DLReal) 0.0)
			{
				sputs("BC->BCDLStart: BCDLStart must be greater than or equal to 0.");
				return 1;
			}

		/*********************************************************************************/
		/* Deconvoluzione omomorfa */
		/*********************************************************************************/

		if (DRCCfg->HDMPNormFactor > 0 && DRCCfg->HDMPNormType == NULL)
			{
				sputs("HD->HDMPNormType: No MP normalization type supplied.");
				return 1;
			}
		if (DRCCfg->HDEPNormFactor > 0 && DRCCfg->HDEPNormType == NULL)
			{
				sputs("HD->HDEPNormType: No EP normalization type supplied.");
				return 1;
			}
		if (DRCCfg->HDMPOutFile != NULL && DRCCfg->HDMPOutFileType == NULL)
			{
				sputs("HD->HDMPOutFileType: No MP output file type supplied.");
				return 1;
			}
		if (DRCCfg->HDEPOutFile != NULL && DRCCfg->HDEPOutFileType == NULL)
			{
				sputs("HD->HDEPOutFileType: No EP output file type supplied.");
				return 1;
			}

		/*********************************************************************************/
		/* Prefiltratura componente MP */
		/*********************************************************************************/

		if (DRCCfg->MPPrefilterType == NULL)
			{
				sputs("MP->MPPrefilterType: No MPPrefilterType supplied.");
				return 1;
			}
		if (DRCCfg->MPPrefilterType[0] != 'B' && DRCCfg->MPPrefilterType[0] != 'b'
			&& DRCCfg->MPPrefilterType[0] != 'S' && DRCCfg->MPPrefilterType[0] != 's')
			{
				sputs("MP->MPPrefilterType: Invalid MPPrefilterType supplied.");
				return 1;
			}
		if (DRCCfg->MPPrefilterFctn == NULL)
			{
				sputs("MP->MPPrefilterFctn: No MPPrefilterFctn supplied.");
				return 1;
			}
		if (DRCCfg->MPPrefilterFctn[0] != 'P' && DRCCfg->MPPrefilterFctn[0] != 'B')
			{
				sputs("MP->MPPrefilterFctn: Invalid MPPrefilterFctn supplied.");
				return 1;
			}
		if (DRCCfg->MPWindowGap < 0)
			{
				sputs("MP->MPWindowGap: MPWindowGap must be greater or equal to 0.");
				return 1;
			}
		if (DRCCfg->MPLowerWindow <= 0)
			{
				sputs("MP->MPLowerWindow: No MPLowerWindow supplied.");
				return 1;
			}
		if (DRCCfg->MPLowerWindow > IWLen)
			{
        sprintf(TStr,"%d.",IWLen);
        sputsp("MP->MPLowerWindow: MPLowerWindow too big. Max allowed value: ", TStr);
				return 1;
			}
		if (DRCCfg->MPUpperWindow <= 0)
			{
				sputs("MP->MPUpperWindow: No MPUpperWindow supplied.");
				return 1;
			}
		if (DRCCfg->MPFSharpness <= (DLReal) 0.0)
			{
				sputs("MP->MPFSharpness: MPFSharpness must be greater than 0.");
				return 1;
			}
		if (DRCCfg->MPUpperWindow > DRCCfg->MPLowerWindow)
			{
				sputs("MP->MPUpperWindow: MPUpperWindow can't be greater than MPLowerWindow.");
				return 1;
			}
		if (DRCCfg->MPWindowExponent <= (DLReal) 0.0)
			{
				sputs("MP->MPWindowExponent: MPWindowExponent must be greater than 0.");
				return 1;
			}
		if (DRCCfg->MPHDRecover == NULL)
			{
				sputs("MP->MPHDRecover: No MPHDRecover supplied.");
				return 1;
			}
		if (DRCCfg->MPEPPreserve == NULL)
			{
				sputs("MP->MPEPPreserve: No MPEPPreserve supplied.");
				return 1;
			}
		if (DRCCfg->MPPFNormFactor > 0 && DRCCfg->MPPFNormType == NULL)
			{
				sputs("MP->MPPFNormType: No PF normalization type supplied.");
				return 1;
			}
		if (DRCCfg->MPPFOutFile != NULL && DRCCfg->MPPFOutFileType == NULL)
			{
				sputs("MP->MPPFOutFileType: No MPPF output file type supplied.");
				return 1;
			}
		if (DRCCfg->MPPFFinalWindow > DRCCfg->MPLowerWindow + DRCCfg->MPFilterLen - 1)
			{
				sputs("MP->MPPFFinalWindow: MPPFFinalWindow can't be greater than MPLowerWindow + MPFilterLen - 1.");
				return 1;
			}
		if (DRCCfg->MPPFFinalWindow <= 0)
			PWLen = DRCCfg->MPLowerWindow + DRCCfg->MPFilterLen - 1;
		else
			PWLen = DRCCfg->MPPFFinalWindow;

		/*********************************************************************************/
		/* Dip limiting */
		/*********************************************************************************/

		if (DRCCfg->DLType == NULL)
			{
				sputs("DL->DLType: No DLType supplied.");
				return 1;
			}
		if (DRCCfg->DLType[0] != 'L' && DRCCfg->DLType[0] != 'M'
				&& DRCCfg->DLType[0] != 'P' && DRCCfg->DLType[0] != 'W')
			{
				sputs("DL->DLType: Invalid dip limiting type supplied.");
				return 1;
			}
		if (DRCCfg->DLStart < (DLReal) 0.0)
			{
				sputs("DL->DLStart: DLStart must be greater than or equal to 0.");
				return 1;
			}

		/*********************************************************************************/
		/* Prefiltratura componente EP */
		/*********************************************************************************/

		if (DRCCfg->EPPrefilterType == NULL)
			{
				sputs("EP->EPPrefilterType: No EPPrefilterType supplied.");
				return 1;
			}
		if (DRCCfg->EPPrefilterType[0] != 'B' && DRCCfg->EPPrefilterType[0] != 'b'
			&& DRCCfg->EPPrefilterType[0] != 'S' && DRCCfg->EPPrefilterType[0] != 's')
			{
				sputs("EP->EPPrefilterType: Invalid EPPrefilterType supplied.");
				return 1;
			}
		if (DRCCfg->EPPrefilterFctn == NULL)
			{
				sputs("EP->EPPrefilterFctn: No EPPrefilterFctn supplied.");
				return 1;
			}
		if (DRCCfg->EPPrefilterFctn[0] != 'P' && DRCCfg->EPPrefilterFctn[0] != 'B')
			{
				sputs("EP->EPPrefilterFctn: Invalid EPPrefilterFctn supplied.");
				return 1;
			}
		if (DRCCfg->EPWindowGap < 0)
			{
				sputs("EP->EPWindowGap: EPWindowGap must be greater or equal to 0.");
				return 1;
			}
		if (DRCCfg->EPLowerWindow <= 0)
			{
				sputs("EP->EPLowerWindow: EPLowerWindow must be greater than 0.");
				return 1;
			}
		if (DRCCfg->EPLowerWindow > IWLen)
			{
        sprintf(TStr,"%d.",IWLen);
        sputsp("EP->EPLowerWindow: EPLowerWindow  too big. Max allowed value: ", TStr);
				return 1;
			}
		if (DRCCfg->EPUpperWindow <= 0)
			{
				sputs("EP->EPUpperWindow: EPUpperWindow must be greater than 0.");
				return 1;
			}
		if (DRCCfg->EPUpperWindow > DRCCfg->EPLowerWindow)
			{
				sputs("EP->EPUpperWindow: EPUpperWindow can't be greater than EPLowerWindow.");
				return 1;
			}
		if (DRCCfg->EPFSharpness <= (DLReal) 0.0)
			{
				sputs("EP->EPFSharpness: EPFSharpness must be greater than 0.");
				return 1;
			}
		if (DRCCfg->EPWindowExponent <= (DLReal) 0.0)
			{
				sputs("EP->EPWindowExponent: EPWindowExponent must be greater than 0.");
				return 1;
			}
		if (DRCCfg->EPPFFlatGain > 0 && DRCCfg->EPPFFlatType == NULL)
			{
				sputs("EP->EPPFFlatType: No EPPFFlatType supplied.");
				return 1;
			}
		if (DRCCfg->EPPFFlatType[0] != 'L' && DRCCfg->EPPFFlatType[0] != 'M' &&
			DRCCfg->EPPFFlatType[0] != 'D')
			{
				sputs("EP->EPPFFlatType: Invalid EPPFFlatType supplied.");
				return 1;
			}
		if (DRCCfg->EPPFNormFactor > 0 && DRCCfg->EPPFNormType == NULL)
			{
				sputs("EP->EPPFNormType: No PF normalization type supplied.");
				return 1;
			}
		if (DRCCfg->EPPFOutFile != NULL && DRCCfg->EPPFOutFileType == NULL)
			{
				sputs("EP->EPPFOutFileType: No EPPF output file type supplied.");
				return 1;
			}
		if (DRCCfg->EPPFFinalWindow > DRCCfg->EPLowerWindow + DRCCfg->EPFilterLen - 1)
			{
				sputs("EP->EPPFFinalWindow: EPPFFinalWindow can't be greater than EPLowerWindow + EPFilterLen - 1.");
				return 1;
			}
		if (DRCCfg->EPPFFinalWindow <= 0)
			PWLen += (DRCCfg->EPLowerWindow + DRCCfg->EPFilterLen - 1) - 1;
		else
			PWLen += DRCCfg->EPPFFinalWindow - 1;

		/*********************************************************************************/
		/* Combinazione componente MP e EP */
		/*********************************************************************************/

		if (DRCCfg->PCOutWindow < 0)
			{
				sputs("PC->PCOutWindow: No PCOutWindow supplied.");
				return 1;
			}
		if (DRCCfg->PCNormFactor > 0 && DRCCfg->PCNormType == NULL)
			{
				sputs("PC->PCNormType: No normalization type supplied.");
				return 1;
			}
		if (DRCCfg->PCOutFile != NULL && DRCCfg->PCOutFileType == NULL)
			{
				sputs("PC->PCOutFileType: No output file type supplied.");
				return 1;
			}
		if (DRCCfg->PCOutWindow > PWLen)
			{
				sprintf(TStr,"%d.",PWLen);
				sputsp("PC->PCOutWindow: PCOutWindow too big. Max value allowed: ",TStr);
				return 1;
			}

		/*********************************************************************************/
		/* Inversione risposta all'impulso */
		/*********************************************************************************/

		if (DRCCfg->ISType == NULL)
			{
				sputs("IS->ISType: No inversion stage type supplied.");
				return 1;
			}
		if (DRCCfg->ISType[0] != 'L' && DRCCfg->ISType[0] != 'T')
			{
				sputs("IS->ISType: Invalid inversion stage type supplied.");
				return 1;
			}
		if (DRCCfg->ISPETType == NULL)
			{
				sputs("IS->ISPETType: No pre echo truncation type supplied.");
				return 1;
			}
		if (DRCCfg->ISPETType[0] != 'f' && DRCCfg->ISPETType[0] != 's')
			{
				sputs("IS->ISPETType: Invalid pre echo truncation type supplied.");
				return 1;
			}
		if (DRCCfg->ISPrefilterFctn == NULL)
			{
				sputs("IS->ISPrefilterFctn: No ISPrefilterFctn supplied.");
				return 1;
			}
		if (DRCCfg->ISPrefilterFctn[0] != 'P' && DRCCfg->ISPrefilterFctn[0] != 'B')
			{
				sputs("IS->ISPrefilterFctn: Invalid ISPrefilterFctn supplied.");
				return 1;
			}
		if (DRCCfg->ISPEUpperWindow >= DRCCfg->ISPELowerWindow)
			{
				sputs("IS->ISPEUpperWindow: ISPELowerWindow must be greater than ISPEUpperWindow.");
				return 1;
			}
		if (DRCCfg->ISPEFSharpness <= (DLReal) 0.0)
			{
				sputs("IS->ISPEFSharpness: ISPEFSharpness must be greater than 0.");
				return 1;
			}
		if (DRCCfg->ISNormFactor > 0 && DRCCfg->ISNormType == NULL)
			{
				sputs("IS->ISNormType: No normalization type supplied.");
				return 1;
			}
		if (DRCCfg->ISOutFile != NULL && DRCCfg->ISOutFileType == NULL)
			{
				sputs("IS->ISOutFileType: No output file type supplied.");
				return 1;
			}
		if (DRCCfg->ISOutWindow < 0)
			{
				sputs("IS->ISOutWindow: ISOutWindow must be greater than 0.");
				return 1;
			}
		if (DRCCfg->ISOutWindow > 0)
			PWLen = DRCCfg->ISOutWindow;

		/*********************************************************************************/
		/* Calcolo target psicoacustico */
		/*********************************************************************************/
		if (DRCCfg->PTType == NULL)
			{
				sputs("PT->PTType: No psychoacoustic target type supplied.");
				return 1;
			}
		if (DRCCfg->PTType[0] != 'N' && DRCCfg->PTType[0] != 'L' && DRCCfg->PTType[0] != 'M')
			{
				sputs("PT->PTType: Invalid psychoacoustic target type supplied.");
				return 1;
			}
		if (DRCCfg->PTType[0] != 'N')
			{
				if (DRCCfg->PTReferenceWindow <= 0)
					{
						sputs("PT->PTReferenceWindow: PTReferenceWindow must be greater than 0.");
						return 1;
					}
				if (DRCCfg->PTReferenceWindow > (DRCCfg->BCInitWindow + PWLen - 1))
					{
						sprintf(TStr,"%d.",DRCCfg->BCInitWindow + PWLen - 1);
						sputsp("PT->PTReferenceWindow: PTReferenceWindow too big. Max allowed value: ", TStr);
						return 1;
					}
				if (DRCCfg->PTDLType == NULL)
					{
						sputs("PT->PTDLType: No DLType supplied.");
						return 1;
					}
				if (DRCCfg->PTDLType[0] != 'L' && DRCCfg->PTDLType[0] != 'M'
						&& DRCCfg->PTDLType[0] != 'P' && DRCCfg->PTDLType[0] != 'W')
					{
						sputs("PT->PTDLType: Invalid dip limiting type supplied.");
						return 1;
					}
				if (DRCCfg->PTDLStart < (DLReal) 0.0)
					{
						sputs("PT->PTDLStart: PTDLStart must be greater than or equal to 0.");
						return 1;
					}
				if (DRCCfg->PTFilterLen <= 0)
					{
						sputs("PT->PTFilterLen: PTFilterLen must be greater than 0.");
						return 1;
					}
				/* Calcola la lunghezza del target psicoacustico */
				if (DRCCfg->PTDLMultExponent >= 0)
					{
						/* Calcola la potenza di due superiore a N */
						for (PTLen = 1;PTLen <= DRCCfg->PTReferenceWindow;PTLen <<= 1);
						PTLen *= 1 << DRCCfg->PTDLMultExponent;
					}
				else
					PTLen = DRCCfg->PTReferenceWindow;
				PTLen *= 2;
				if (DRCCfg->PTFilterLen > PTLen)
					{
						sprintf(TStr,"%d.",PTLen);
						sputsp("PT->PTFilterLen: PTFilterLen too big. Max allowed value: ", TStr);
						return 1;
					}
				if (DRCCfg->PTFilterFile != NULL && DRCCfg->PTFilterFileType == NULL)
					{
						sputs("PT->PTFilterFileType: No PT filter file type supplied.");
						return 1;
					}
				if (DRCCfg->PTNormFactor > 0 && DRCCfg->PTNormType == NULL)
					{
						sputs("PT->PTNormType: No PT normalization type supplied.");
						return 1;
					}
				if (DRCCfg->PTOutFile != NULL && DRCCfg->PTOutFileType == NULL)
					{
						sputs("PT->PTOutFileType: No PT output file type supplied.");
						return 1;
					}
				if (DRCCfg->PTOutWindow < 0)
					{
						sputs("PT->PTOutWindow: PTOutWindow can't be less than 0.");
						return 1;
					}
				switch (DRCCfg->PTDLType[0])
					{
						case 'L':
							if (DRCCfg->PTOutWindow > PWLen + DRCCfg->PTFilterLen - 1)
								{
									sprintf(TStr,"%d.",PWLen + DRCCfg->PTFilterLen - 1);
									sputsp("PT->PTOutWindow: PTOutWindow too big. Max value allowed: ",TStr);
									return 1;
								}
							if (DRCCfg->PTOutWindow > 0)
								PWLen = DRCCfg->PTOutWindow;
							else
								PWLen = PWLen + DRCCfg->PTFilterLen - 1;
						break;

						case 'M':
							if (DRCCfg->PTOutWindow > PWLen + 2 * (DRCCfg->PTFilterLen - 1))
								{
									sprintf(TStr,"%d.",PWLen + 2 * (DRCCfg->PTFilterLen - 1));
									sputsp("PT->PTOutWindow: PTOutWindow too big. Max value allowed: ",TStr);
									return 1;
								}
							if (DRCCfg->PTOutWindow > 0)
								PWLen = DRCCfg->PTOutWindow;
							else
								PWLen = PWLen + DRCCfg->PTFilterLen - 1;
						break;
					}
			}

		/*********************************************************************************/
		/* Peak limiting */
		/*********************************************************************************/

		if (DRCCfg->PLType == NULL)
			{
				sputs("PL->PLType: No peak limiting type supplied.");
				return 1;
			}
		if (DRCCfg->PLType[0] != 'L' && DRCCfg->PLType[0] != 'M'
				&& DRCCfg->PLType[0] != 'P' && DRCCfg->PLType[0] != 'W')
			{
				sputs("PL->PLType: Invalid peak limiting type supplied.");
				return 1;
			}
		if (DRCCfg->PLStart < (DLReal) 0.0)
			{
				sputs("PL->PLStart: PLStart must be greater than or equal to 0.");
				return 1;
			}
		if (DRCCfg->PLNormFactor > 0 && DRCCfg->PLNormType == NULL)
			{
				sputs("PL->PLNormType: No normalization type supplied.");
				return 1;
			}
		if (DRCCfg->PLOutFile != NULL && DRCCfg->PLOutFileType == NULL)
			{
				sputs("PL->PLOutFileType: No output file type supplied.");
				return 1;
			}
		if (DRCCfg->PLOutWindow > PWLen)
			{
				sprintf(TStr,"%d.",PWLen);
				sputsp("PL->PLOutWindow: PLOutWindow too big. Max allowed value: ", TStr);
				return 1;
			}
		if (DRCCfg->PLOutWindow > 0)
			PWLen = DRCCfg->PLOutWindow;

		/*********************************************************************************/
		/* Troncatura ringing */
		/*********************************************************************************/

		if (DRCCfg->RTType == NULL)
			{
				sputs("RT->RTType: No RTType supplied.");
				return 1;
			}
		if (DRCCfg->RTType[0] != 'N' && DRCCfg->RTType[0] != 'B' && DRCCfg->RTType[0] != 'b'
			&& DRCCfg->RTType[0] != 'S' && DRCCfg->RTType[0] != 's')
			{
				sputs("RT->RTType: Invalid RTType supplied.");
				return 1;
			}
		if (DRCCfg->RTType[0] != 'N')
			{
				if (DRCCfg->RTPrefilterFctn == NULL)
					{
						sputs("RT->RTPrefilterFctn: No RTPrefilterFctn supplied.");
						return 1;
					}
				if (DRCCfg->RTPrefilterFctn[0] != 'P' && DRCCfg->RTPrefilterFctn[0] != 'B')
					{
						sputs("RT->RTPrefilterFctn: Invalid RTPrefilterFctn supplied.");
						return 1;
					}
				if (DRCCfg->RTWindowGap < 0)
					{
						sputs("RT->RTWindowGap: RTWindowGap must be greater or equal to 0.");
						return 1;
					}
				if (DRCCfg->RTLowerWindow <= 0)
					{
						sputs("RT->RTLowerWindow: RTLowerWindow must be greater than 0.");
						return 1;
					}
				if (DRCCfg->RTLowerWindow > PWLen)
					{
						sprintf(TStr,"%d.",PWLen);
						sputsp("RT->RTLowerWindow: RTLowerWindow too big. Max allowed value: ", TStr);
						return 1;
					}
				if (DRCCfg->RTUpperWindow <= 0)
					{
						sputs("RT->RTUpperWindow: RTUpperWindow must be greater than 0.");
						return 1;
					}
				if (DRCCfg->RTFSharpness <= (DLReal) 0.0)
					{
						sputs("RT->RTFSharpness: RTFSharpness must be greater than 0.");
						return 1;
					}
				if (DRCCfg->RTUpperWindow > PWLen)
					{
						sprintf(TStr,"%d.",PWLen);
						sputsp("RT->RTUpperWindow: RTUpperWindow too big. Max allowed value: ", TStr);
						return 1;
					}
				if (DRCCfg->RTWindowExponent <= (DLReal) 0.0)
					{
						sputs("RT->RTWindowExponent: RTWindowExponent must be greater than 0.");
						return 1;
					}
				if (DRCCfg->RTNormFactor > 0 && DRCCfg->RTNormType == NULL)
					{
						sputs("RT->RTNormType: No RT normalization type supplied.");
						return 1;
					}
				if (DRCCfg->RTOutFile != NULL && DRCCfg->RTOutFileType == NULL)
					{
						sputs("RT->RTOutFileType: No RT output file type supplied.");
						return 1;
					}
				if (DRCCfg->RTOutWindow > DRCCfg->RTLowerWindow + DRCCfg->RTFilterLen - 1)
					{
						sputs("RT->RTOutWindow: RTOutWindow can't be greater than RTLowerWindow + RTFilterLen - 1.");
						return 1;
					}
				if (DRCCfg->RTOutWindow <= 0)
					PWLen = DRCCfg->RTLowerWindow + DRCCfg->RTFilterLen - 1;
				else
					PWLen = DRCCfg->RTOutWindow;
			}

		/*********************************************************************************/
		/* Applicazione risposta target */
		/*********************************************************************************/

		if (DRCCfg->PSFilterType == NULL)
			{
				sputs("PS->PSFilterType: No filter type supplied.");
				return 1;
			}
		if (DRCCfg->PSFilterType[0] != 'L' && DRCCfg->PSFilterType[0] != 'M'
			&& DRCCfg->PSFilterType[0] != 'T')
			{
				sputs("PS->PSFilterType: Invalid filter type supplied.");
				return 1;
			}
		if (DRCCfg->PSInterpolationType == NULL)
			{
				sputs("PS->PSInterpolationType: No interpolation type supplied.");
				return 1;
			}
		if (DRCCfg->PSInterpolationType[0] != 'L' && DRCCfg->PSInterpolationType[0] != 'G'
			&& DRCCfg->PSInterpolationType[0] != 'R' && DRCCfg->PSInterpolationType[0] != 'S'
			&& DRCCfg->PSInterpolationType[0] != 'P' && DRCCfg->PSInterpolationType[0] != 'H')
			{
				sputs("PS->PSInterpolationType: Invalid interpolation type supplied.");
				return 1;
			}
		if (DRCCfg->PSMagType== NULL)
			{
				sputs("PS->PSMagType: No filter definition magnitude type supplied.");
				return 1;
			}
		if (DRCCfg->PSMagType[0] != 'L' && DRCCfg->PSMagType[0] != 'D')
			{
				sputs("PS->PSMagType: Invalid filter definition magnitude type supplied.");
				return 1;
			}
		if (DRCCfg->PSPointsFile == NULL)
			{
				sputs("PS->PSPointsFile: No correction point file supplied.");
				return 1;
			}
		if (DRCCfg->PSNumPoints < 0 || DRCCfg->PSNumPoints == 1)
			{
				sputs("PS->PSNumPoints: Invalid PSNumPoints supplied, it must be 0 or at least 2.");
				return 1;
			}
		if (DRCCfg->PSNormFactor > 0 && DRCCfg->PSNormType == NULL)
			{
				sputs("PS->PSNormFactor: No normalization type supplied.");
				return 1;
			}
		if (DRCCfg->PSOutFile != NULL && DRCCfg->PSOutFileType == NULL)
			{
				sputs("PS->PSOutFileType: No output file type supplied.");
				return 1;
			}
		if (DRCCfg->PSFilterLen <= 0)
			{
				sputs("PS->PSFilterLen: PSFilterLen must be greater than 0.");
				return 1;
			}
		switch (DRCCfg->PSFilterType[0])
			{
				case 'L':
					if (DRCCfg->PSOutWindow > PWLen + DRCCfg->PSFilterLen - 1)
						{
							sprintf(TStr,"%d.",PWLen + DRCCfg->PSFilterLen - 1);
							sputsp("PS->PSOutWindow: PSOutWindow too big. Max allowed value: ", TStr);
							return 1;
						}
				break;
				case 'M':
					if (DRCCfg->PSOutWindow > PWLen)
						{
							sprintf(TStr,"%d.",PWLen);
							sputsp("PS->PSOutWindow: PSOutWindow too big. Max allowed value: ", TStr);
							return 1;
						}
				break;
				case 'T':
					if (DRCCfg->PSOutWindow > PWLen / 2 + DRCCfg->ISPELowerWindow + DRCCfg->PSFilterLen - 1)
						{
							sprintf(TStr,"%d.",PWLen / 2 + DRCCfg->ISPELowerWindow + DRCCfg->PSFilterLen - 1);
							sputsp("PS->PSOutWindow: PSOutWindow too big. Max allowed value: ", TStr);
							return 1;
						}
					if (DRCCfg->PSOutWindow < 2 * DRCCfg->ISPELowerWindow)
						{
							sprintf(TStr,"%d.",2 * DRCCfg->ISPELowerWindow);
							sputsp("PS->PSOutWindow: PSOutWindow too small. Min allowed value: ", TStr);
							return 1;
						}
					if (PWLen < 2 * DRCCfg->ISPELowerWindow)
						{
							sprintf(TStr,"%d.",2 * DRCCfg->ISPELowerWindow);
							sputsp("PS->PSFilterType: Output of previous stages too small to allow for the required pre-echo truncation length. Check either RTOutWindow, PLOutWindow, PTOutWindow or decrease ISPELowerWindow. Min allowed value: ", TStr);
							return 1;
						}
				break;
			}
    PWLen += DRCCfg->PSFilterLen - 1;

		/*********************************************************************************/
		/* Estrazione filtro a fase minima */
		/*********************************************************************************/

    if (DRCCfg->MSOutFile != NULL)
      {
        if (DRCCfg->MSOutWindow > PWLen)
          {
            sprintf(TStr,"%d.",PWLen);
            sputsp("MS->MSOutWindow: MSOutWindow too big. Max allowed value: ",TStr);
            return 1;
          }
        if (DRCCfg->MSFilterDelay < 0)
          {
            sputs("MS->MSFilterDelay: MSFilterDelay must be greater than 0.");
            return 1;
          }
        if (DRCCfg->MSOutWindow > 0)
          {
            if (DRCCfg->MSFilterDelay >= DRCCfg->MSOutWindow)
              {
                sputs("MS->MSFilterDelay: MSFilterDelay must be smaller than MSOutWindow.");
                return 1;
              }
          }
        else
          {
            if (DRCCfg->MSFilterDelay >= PWLen)
              {
                sprintf(TStr,"%d.",PWLen);
                sputsp("MS->MSFilterDelay: MSFilterDelay too big. Max allowed value: ",TStr);
                return 1;
              }
          }
        if (DRCCfg->MSNormFactor > 0 && DRCCfg->MSNormType == NULL)
          {
            sputs("MS->MSNormType: No normalization type supplied.");
            return 1;
          }
        if (DRCCfg->MSOutFile != NULL && DRCCfg->MSOutFileType == NULL)
          {
            sputs("MS->MSOutFileType: No output file type supplied.");
            return 1;
          }
      }
		/*********************************************************************************/
		/* Convoluzione di test */
		/*********************************************************************************/

		if (DRCCfg->TCNormFactor > 0 && DRCCfg->TCNormType == NULL)
			{
				sputs("TC->TCNormType: No normalization type supplied.");
				return 1;
			}
		if (DRCCfg->TCOutFile != NULL && DRCCfg->TCOutFileType == NULL)
			{
				sputs("TC->TCOutFileType: No output file type supplied.");
				return 1;
			}
		if (DRCCfg->TCOWFile != NULL)
			{
				if (DRCCfg->TCOWFileType == NULL)
					{
						sputs("TC->TCOWFileType: No overwrite file type supplied.");
						return 1;
					}
				if (DRCCfg->TCOWNormFactor > 0 && DRCCfg->TCOWNormType == NULL)
					{
						sputs("TC->TCOWNormType: No normalization type supplied.");
						return 1;
					}
			}

		/* Controllo completato */
		return 0;
	}
