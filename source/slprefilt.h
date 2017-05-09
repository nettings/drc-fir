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

#ifndef SLPreFilt_h
	#define SLPreFilt_h

	/* Inclusioni */
	#include "dsplib.h"
	#include "dspwind.h"

	/* Tipo di curva parametrica da utilizzare */
	typedef enum { SLPProportional, SLPBilinear } SLPPrefilteringType;

	/* Prefiltratura a bande tramite sliding lowpass di un segnale */
	void SLPreFilt(DLReal * InImp, const int IBS, const int FBS,
		const int FilterLen, const int BandSplit, const DLReal WindowExponent,
		const int SampleFreq, const DLReal StartFreq, const DLReal EndFreq,
		int WindowGap, DLReal FSharpness, DLReal * OutImp,
		const WindowType WType, const SLPPrefilteringType SLPType);
#endif
