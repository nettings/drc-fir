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

/* Inversione tramite Kirkeby fast deconvolution */

#ifndef KirkebyFD_h
	#define KirkebyFD_h

	/* Inclusioni */
	#include "dsplib.h"
	#include "slprefilt.h"

	/* Inversione tramite Kirkeby fast deconvolution */
	Boolean KirkebyFDInvert(const DLReal * InSig, int InSigLen,DLReal * InvFilter,
		int InvFilterLen, const DLReal * EffortSig, int EffortSigLen, DLReal EffortFactor,
		int MExp);

	/* Inversione tramite selective minimum pahse fast deconvolution */
	/* Versione a misura indiretta degli effetti di pre echo */
	Boolean PEISMPKirkebyFDInvert(const DLReal * MPSig, int MPSigLen,
		const DLReal * EPSig, int EPSigLen,	DLReal * InvFilter,	int InvFilterLen,
		DLReal EffortFactor, int MExp);

	/* Inversione tramite selective minimum pahse fast deconvolution */
	/* Versione a misura diretta degli effetti di pre echo */
	Boolean PEMSMPKirkebyFDInvert(const DLReal * MPSig, int MPSigLen, const DLReal * EPSig, int EPSigLen,
		DLReal * InvFilter,	int InvFilterLen, DLReal EffortFactor, int PEStart, int PETransition, int MExp);

	/* Inversione tramite selective minimum pahse fast deconvolution */
	/* Versione a troncatura degli effetti di pre echo */
	Boolean PETFDInvert(const DLReal * MPSig, int MPSigLen, const DLReal * EPSig, int EPSigLen,
		DLReal * InvFilter,	int InvFilterLen, char PEType, int PELowerWindow, int PEUpperWindow,
		DLReal PEStartFreq, DLReal PEEndFreq, int PEFilterLen, DLReal FSharpness, int PEBandSplit,
		DLReal PEWindowExponent, const SLPPrefilteringType SLPType, const DLReal OGainFactor,
		int SampleRate, int MExp);

	/* Inversione tramite selective minimum pahse fast deconvolution */
	/* Versione a troncatura multipla degli effetti di pre echo
	con componente EP di riferimento */
	Boolean MRefPETFDInvert(const DLReal * MPSig, int MPSigLen, const DLReal * EPSig, int EPSigLen,
		DLReal * InvFilter,	int InvFilterLen, char PEType, int PELowerWindow, int PEUpperWindow,
		DLReal PEStartFreq, DLReal PEEndFreq, int PEFilterLen, DLReal FSharpness, int PEBandSplit,
		DLReal PEWindowExponent, const SLPPrefilteringType SLPType, const DLReal * EPRef, int EPRefLen,
		char RPEType, int RPELowerWindow,	int RPEUpperWindow,	DLReal RPEStartFreq, DLReal RPEEndFreq,
		int RPEFilterLen, DLReal RFSharpness,	int RPEBandSplit,	DLReal RPEWindowExponent,
		const SLPPrefilteringType RSLPType, const DLReal OGainFactor, int SampleRate,	int MExp);

#endif
