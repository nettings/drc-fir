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

/* Header generale DRC */

#ifndef DRC_h
	#define DRC_h

	#include "cfgparse.h"
	#include <float.h>

	/* Decommentare per abilitare la compilazione in doppia precisione */
	/* Uncomment to enable double precision computation */
	/* #define UseDouble */

	/* Commentare per disabilitare l'uso delle routine FFT incluse nella GSL */
	/* Comment out to disable GSL FFT routines */
	#define UseGSLFft

	/* Decommentare per abilitare l'uso delle routine FFT di Ooura per potenze di 2 */
	/* Le routine FFT di Ooura sono leggermente meno accurate delle routine GSL ma sono anche
	più veloci */
	/* Uncomment to enable the use of the Ooura FFT routines for power of 2 lenghts */
	/* The Ooura FFT routines are a little less accurate then the GSL FFT routines but are also
	noticeably faster */
	/* #define UseOouraFft */

	#ifdef UseDouble
		/* Tipo floating point usato per le elaborazioni */
		#define DRCFloat double

		/* Tipo corrispondente per l'importazione di valori di
		configurazione */
		#define DRCCfgFloat CfgDouble

		/* Valore minimo e massimo del tipo float in uso */
		#ifdef DBL_MIN
			#define  DRCMinFloat DBL_MIN
		#else
			#define  DRCMinFloat 2.2250738585072014e-308
		#endif

		#ifdef DBL_MAX
			#define  DRCMaxFloat DBL_MAX
		#else
			#define  DRCMaxFloat 1.7976931348623158e+308
		#endif

		#ifdef DBL_EPSILON
			#define  DRCEpsFloat DBL_EPSILON
		#else
			#define  DRCEpsFloat 2.2204460492503131e-016
		#endif
	#else
		/* Tipo floating point usato per le elaborazioni */
		#define DRCFloat float

		/* Tipo corrispondente per l'importazione di valori di
		configurazione */
		#define DRCCfgFloat CfgFloat

		/* Valore minimo e massimo del tipo float in uso */
		#ifdef FLT_MIN
			#define  DRCMinFloat FLT_MIN
		#else
			#define  DRCMinFloat 1.175494351e-38F
		#endif

		#ifdef FLT_MAX
			#define  DRCMaxFloat FLT_MAX
		#else
			#define  DRCMaxFloat 3.402823466e+38F
		#endif

		#ifdef FLT_EPSILON
			#define  DRCEpsFloat FLT_EPSILON
		#else
			#define  DRCEpsFloat 1.192092896e-07F
		#endif
	#endif

	/* Tipo floating point usato per l'importazione e
	l'esportazione dei segnali floating point in doppia precisione */
	#define DRCFileDouble double

	/* Tipo floating point usato per l'importazione e
	l'esportazione dei segnali floating point in singola precisione */
	#define DRCFileFloat float

	/* Tipo intero usato per l'importazione e
	l'esportazione dei segnali interi */
	#define DRCFileInt short int

	/* Imposta l'uso delle funzioni trigonometriche ridotte */
	#define UseTypedTrigs

	/* Verifica l'uso delle funzioni trigonometriche ridotte */
	#ifdef UseTypedTrigs
		#ifdef UseDouble
			#define DRCSin sin
			#define DRCCos cos
			#define DRCTan tan
			#define DRCATan atan
		#else
			#define DRCSin sinf
			#define DRCCos cosf
			#define DRCTan tanf
			#define DRCATan atanf
		#endif
	#else
		#define DRCSin sin
		#define DRCCos cos
		#define DRCTan tan
		#define DRCATan atan
	#endif
#endif
