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

****************************************************************************/

/* Opzioni di configurazione DRC */

#ifndef DRCCfg_h
	#define DRCCfg_h

	/* Inclusioni */
	#include "drc.h"
	#include "cmdline.h"

	/* Posizione opzione directory base di configurazione
	nella lista parametri */
	#define BCBaseDirParmPos 0

	/* Struttura definizione configurazione */
	/* Prefissi variabili
		BC = Base Configuration
		HD = Homomorphic Deconvolution
		MP = Minimum phase frequency dependent windowing
		DL = Dip limiting stage
		EP = Excess phase frequency dependent windowing
		PC = Prefiltering completion stage
		IS = Inversion stage
		PT = Psychoacoustic target stage
		PL = Peak limiting stage
		RT = Ringing truncation stage
		PS = Postfiltering stage
		MC = Mic compensation stage
		MS = Minimum phase filter extraction stage
		TC = Test convolution stage
	*/
	typedef struct
		{
			/* Base configuration */
			char * BCBaseDir;
			char * BCInFile;
			char * BCInFileType;
			int BCSampleRate;
			char * BCImpulseCenterMode;
			int BCImpulseCenter;
			int BCInitWindow;
			int BCPreWindowLen;
			int BCPreWindowGap;
			DRCFloat BCNormFactor;
			char * BCNormType;

      /* Mic compensation stage */
			char * MCFilterType;
			char * MCInterpolationType;
			int MCMultExponent;
			int MCFilterLen;
			int MCNumPoints;
			char * MCPointsFile;
			char * MCMagType;
			int MCOutWindow;
			char * MCFilterFile;
			char * MCFilterFileType;
			DRCFloat MCNormFactor;
			char * MCNormType;
			char * MCOutFile;
			char * MCOutFileType;

			/* Base configuration dip limiting stage */
			char * BCDLType;
			DRCFloat BCDLMinGain;
			DRCFloat BCDLStart;
			DRCFloat BCDLStartFreq;
			DRCFloat BCDLEndFreq;
			int BCDLMultExponent;

			/* Homomorphic Deconvolution */
			int HDMultExponent;
			DRCFloat HDMPNormFactor;
			char * HDMPNormType;
			char * HDMPOutFile;
			char * HDMPOutFileType;
			DRCFloat HDEPNormFactor;
			char * HDEPNormType;
			char * HDEPOutFile;
			char * HDEPOutFileType;

			/* Minimum phase prefiltering stage */
			char * MPPrefilterType;
			char * MPPrefilterFctn;
			int MPWindowGap;
			int MPLowerWindow;
			int MPUpperWindow;
			DRCFloat MPStartFreq;
			DRCFloat MPEndFreq;
			DRCFloat MPWindowExponent;
			int MPFilterLen;
			DRCFloat MPFSharpness;
			int MPBandSplit;
			char * MPHDRecover;
			char * MPEPPreserve;
			int MPHDMultExponent;
			int MPPFFinalWindow;
			DRCFloat MPPFNormFactor;
			char * MPPFNormType;
			char * MPPFOutFile;
			char * MPPFOutFileType;

			/* Dip limiting stage */
			char * DLType;
			DRCFloat DLMinGain;
			DRCFloat DLStart;
			DRCFloat DLStartFreq;
			DRCFloat DLEndFreq;
			int DLMultExponent;

			/* Excess fase phase prefiltering stage */
			char * EPPrefilterType;
			char * EPPrefilterFctn;
			int EPWindowGap;
			int EPLowerWindow;
			int EPUpperWindow;
			DRCFloat EPStartFreq;
			DRCFloat EPEndFreq;
			DRCFloat EPWindowExponent;
			int EPFilterLen;
			DRCFloat EPFSharpness;
			int EPBandSplit;
			DRCFloat EPPFFlatGain;
			DRCFloat EPPFOGainFactor;
			char * EPPFFlatType;
			int EPPFFGMultExponent;
			int EPPFFinalWindow;
			DRCFloat EPPFNormFactor;
			char * EPPFNormType;
			char * EPPFOutFile;
			char * EPPFOutFileType;

			/* Prefiltering completion stage */
			int PCOutWindow;
			DRCFloat PCNormFactor;
			char * PCNormType;
			char * PCOutFile;
			char * PCOutFileType;

			/* Inversion stage */
			char * ISType;
			char * ISPETType;
			char * ISPrefilterFctn;
			int ISPELowerWindow;
			int ISPEUpperWindow;
			int ISPEStartFreq;
			int ISPEEndFreq;
			int ISPEFilterLen;
			DRCFloat ISPEFSharpness;
			int ISPEBandSplit;
			DRCFloat ISPEWindowExponent;
			DRCFloat ISPEOGainFactor;
			int ISSMPMultExponent;
			int ISOutWindow;
			DRCFloat ISNormFactor;
			char * ISNormType;
			char * ISOutFile;
			char * ISOutFileType;

			/* Psychoacoustic target stage */
			char * PTType;
			int PTReferenceWindow;
			char * PTDLType;
			DRCFloat PTDLMinGain;
			DRCFloat PTDLStart;
			DRCFloat PTDLStartFreq;
			DRCFloat PTDLEndFreq;
			int PTDLMultExponent;
			DRCFloat PTBandWidth;
			DRCFloat PTPeakDetectionStrength;
			int PTMultExponent;
			int PTFilterLen;
			char * PTFilterFile;
			char * PTFilterFileType;
			DRCFloat PTNormFactor;
			char * PTNormType;
			char * PTOutFile;
			char * PTOutFileType;
			int PTOutWindow;

			/* Peak limiting stage */
			char * PLType;
			DRCFloat PLMaxGain;
			DRCFloat PLStart;
			DRCFloat PLStartFreq;
			DRCFloat PLEndFreq;
			int PLMultExponent;
			int PLOutWindow;
			DRCFloat PLNormFactor;
			char * PLNormType;
			char * PLOutFile;
			char * PLOutFileType;

			/* Ringing truncation stage */
			char * RTType;
			char * RTPrefilterFctn;
			int RTWindowGap;
			int RTLowerWindow;
			int RTUpperWindow;
			DRCFloat RTStartFreq;
			DRCFloat RTEndFreq;
			DRCFloat RTWindowExponent;
			int RTFilterLen;
			DRCFloat RTFSharpness;
			int RTBandSplit;
			int RTOutWindow;
			DRCFloat RTNormFactor;
			char * RTNormType;
			char * RTOutFile;
			char * RTOutFileType;

			/* Target response stage */
			char * PSFilterType;
			char * PSInterpolationType;
			int PSMultExponent;
			int PSFilterLen;
			int PSNumPoints;
			char * PSPointsFile;
			char * PSMagType;
			int PSOutWindow;
			DRCFloat PSNormFactor;
			char * PSNormType;
			char * PSOutFile;
			char * PSOutFileType;

			/* Minimum phase filter extraction stage */
			int MSMultExponent;
			int MSOutWindow;
			int MSFilterDelay;
			DRCFloat MSNormFactor;
			char * MSNormType;
			char * MSOutFile;
			char * MSOutFileType;

			/* Test convolution stage */
			DRCFloat TCNormFactor;
			char * TCNormType;
			char * TCOutFile;
			char * TCOutFileType;
			char * TCOWFile;
			char * TCOWFileType;
			DRCFloat TCOWNormFactor;
			char * TCOWNormType;
			int TCOWSkip;
			int TCOWPrewindow;
			int TCOWLength;
		}
	CfgParmsType;

	/* Opzioni di configurazione */
	extern CfgParmsType Cfg;

	/* Definizione struttura file di configurazione */
	extern CfgParameter CfgParmsDef[];

	/* Controllo validità parametri di configurazione */
	int CheckDRCCfg(const CfgParmsType * DRCCfg);

	/* Impostazione directory base di lavoro */
	int SetupDRCCfgBaseDir(CfgParmsType * DRCCfg, const CfgParameter * CfgParmsDef,
		const CmdLineType * OptData);
#endif
