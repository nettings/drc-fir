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

/* Prefiltratura a bande di un segnale */

#ifndef BWPreFilt_h
	#define BWPreFilt_h

	/* Inclusioni */
	#include "dsplib.h"
	#include "dspwind.h"

	/* Tipo di curva parametrica da utilizzare */
	typedef enum { BWPProportional, BWPBilinear } BWPPrefilteringType;

	/* Calcolo potenza intera di un numero */
	DLReal IntPow(DLReal X,unsigned int N);

	/* Prefiltratura a bande di un segnale */
	void BWPreFilt(const DLReal * InImp, const int IBS, const int FBS,
		const int FilterLen, const int OctaveSplit, const DLReal WindowExponent,
		const int SampleFreq, const DLReal FilterBegin, const DLReal FilterEnd,
		const int WindowGap, DLReal * OutImp,
		const WindowType WType, const BWPPrefilteringType BWPType);
#endif
