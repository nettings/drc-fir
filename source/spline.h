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

/* Spline routines */

#ifndef Spline_h
	#define Spline_h

	/* Inclusioni */
	#include "dsplib.h"
  #include "boolean.h"

	/* Tipo spline da generare */
  typedef enum { SplineBDef, SplineBNat } CSplineType;

  /* Valore della spline lineare nel punto X */
	DLReal L1SplineValue(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal X);

	/* Valore della spline lineare nei punti X */
	/* X deve essere un array crescente strettamente monotonico */
	void AL1SplineValue(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal * X, const int XS, DLReal * Y);

	/* Prepara la base per la spline cubica interpolante, ponendola in SPD2YN */
	Boolean SplinePrepare(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal SPD1Y1, const DLReal SPD1YN,
		const CSplineType SPType, DLReal * SPD2YN);

	/* Valore della spline cubica interpolante nel punto X */
	DLReal SplineValue(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal * SPD2YN, const DLReal X);

	/* Valore della spline cubica interpolante nei punti X */
	/* X deve essere un array crescente strettamente monotonico */
	void ASplineValue(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal * SPD2YN, const DLReal * X,
		const int XS, DLReal * Y);

	/* Valore della B spline cubica uniforme nel punto X */
	DLReal B3SplineValue(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal X);

	/* Valore della B spline cubica uniforme nei punti X */
	/* X deve essere un array crescente strettamente monotonico */
	void AB3SplineValue(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal * X, const int XS, DLReal * Y);

	/* Valore della B spline cubica di tipo Catmull Rom nel punto X */
	DLReal B3CRSplineValue(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal X);

	/* Valore della B spline cubica di tipo Catmull Rom  nei punti X */
	/* X deve essere un array crescente strettamente monotonico */
	void AB3CRSplineValue(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal * X, const int XS, DLReal * Y);

	/* Interpolazione su scala lineare tramite B spline cubica
	uniforme con riparametrizzazione arc length tramite B spline
	cubica Catmull-Rom interpolante */
	Boolean AB3CRMSplineValue(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal * X, const int XS, DLReal * Y);

	/* Interpolazione su scala lineare tramite
	Hermite spline cubica monotonica */
	void APCHIPSplineValue(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal * X, const int XS, DLReal * Y);

	/* Interpolazione su scala lineare tramite B spline cubica
	uniforme con riparametrizzazione arc length tramite spline
	cubica Hermite monotonica */
	Boolean AB3HSMSplineValue(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal * X, const int XS, DLReal * Y);

	/* Definizione tipo spline da utilizzare */
	typedef enum
		{
			CSLLinear, /* Lineare */
			CSLCSpline, /* Spline cubica interpolante classica, classe C2, non monotonica */
			CSLBSpline, /* B Spline cubica approssimante, classe C2, non interpolante */
			CSLCRSpline, /* CatmullRom, interpolante, non monotonica */
			CSLHSSpline, /* Hermite spline, interpolante, monotonica (PCHIP) */
		}
	CSLSplineType;

	/* Definizione tipo asse da utilizzare */
	typedef enum
		{
				CSLLinXLinY,
				CSLLogXLinY,
				CSLLinXLogY,
				CSLLogXLogY,
		}
	CSLAxisType;

	/* Funzione generica interpolazione */
	/* Nel caso si utilizzi un asse logaritmico i valori su tale
	asse devono	essere strettamente maggiori di 0 */
	Boolean CSLASplineValue(const DLReal * XN, const DLReal * YN,
		const int N, const DLReal * X, const int XS, DLReal * Y,
		const CSLSplineType SplineType, const CSLAxisType AxisType);

	/* Funzione generica interpolazione, versione complessa */
	/* Nel caso si utilizzi un asse logaritmico i valori di ampiezza
	su tale	asse devono	essere strettamente maggiori di 0.
	I valori di fase vengono sempre considerati su ascissa lineare */
	Boolean CSLCASplineValue(const DLReal * XN, const DLReal * MN,
		const DLReal * PN, const int N, const DLReal * X, const int XS,
		DLComplex * Y, const CSLSplineType SplineType, const CSLAxisType AxisType);

#endif
