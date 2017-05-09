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

/****************************************************************************
  Progetto    : DSP Library.
  File        : DspLib.h
  Autore      : Sbragion Denis
  Descrizione : Definizioni base
  Revisioni   :
  16/10/93    : Prima stesura.
****************************************************************************/

#ifndef DspLib_h
  #define DspLib_h

	#include <complex>
	#include <math.h>
	#include "boolean.h"
	#include "drc.h"

	typedef DRCFloat DLReal;

	/* Imposta la funzioni trigonometriche ridotte */
	#define DLSin DRCSin
	#define DLCos DRCCos

	#ifndef M_PI
		#define M_PI ((DLReal) 3.14159265358979323846264338327950288)
	#endif

	#ifndef M_2PI
		#define M_2PI ((DLReal) 6.28318530717958647692528676655900576)
	#endif

	typedef std::complex<DLReal> DLComplex;
#endif

/***************************************************************************/
