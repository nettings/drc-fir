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

/* Deconvoluzione omomorfa */

#ifndef HD_h
	#define HD_h

	/* Inclusioni */
	#include "dsplib.h"

	/* Effettua la deconvoluzione omomorfa del segnale In */
	/* Versione basata sul calcolo del Cepstrum */
	Boolean CepstrumHD(const DLReal * In, DLReal * MPOut, DLReal * EPOut,
		const int N, const int MExp);

	/* Effettua la deconvoluzione omomorfa del segnale In */
	/* Versione basata sulla trasformata di Hilbert */
	Boolean HilbertHD(const DLReal * In, DLReal * MPOut, DLReal * EPOut,
		const int N, const int MExp);

	/* Experimental nonsense, ignore */

	/* Effettua la decomposizione del segnale In nelle somponenti allmag/allphase */
	Boolean AMAPDecomposition(const DLReal * In, DLReal * AMOut, DLReal * APOut,
		const int N, const int PFactor);

	/* Effettua la decomposizione del segnale In nelle somponenti no phase/no mag  */
	Boolean NPNMDecomposition(const DLReal * In, DLReal * NPOut, DLReal * NMOut,
		const int N, const int PFactor);

#endif
