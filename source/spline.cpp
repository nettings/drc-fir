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

/* Inclusioni */
#include "spline.h"
#include <stdlib.h>
#include <math.h>

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

/* Valore della spline lineare nel punto X */
DLReal L1SplineValue(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal X)
	{
		int IP;
		int IL;
		int IR;

		/* Recupera l'intervallo interessato */
		IL = 0;
		IR = N - 1;
		while (IR - IL > 1)
			{
				IP = (IR + IL) >> 1;
				if (XN[IP] > X)
					IR = IP;
				else
					IL = IP;
			}

		/* Ritorna il valore calcolato */
		return YN[IL] + ((X - XN[IL]) / (XN[IR] - XN[IL])) * (YN[IR] - YN[IL]);
	}

/* Valore della spline lineare nei punti X */
/* X deve essere un array crescente strettamente monotonico */
void AL1SplineValue(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal * X, const int XS, DLReal * Y)
	{
		/* Indici ricerca array spline */
		int ISL;
		int ISU;
		int ISP;

		/* Inici ricerca array funzione */
		int IFL;
		int IFU;
		int IFP;

		/* X Corrente */
		int IXP;
		DLReal CX;

		/* Estremi correnti array spline */
		DLReal XNL;
		DLReal XNU;
		DLReal YNL;
		DLReal YNU;

		/* Variabili intermedie calcolo spline */
		DLReal DX;
		DLReal DY;

		/* Ciclo sui punti disponibili */
		IXP = 0;
		ISL = 0;
		while (IXP < XS)
			{
				/* Estrae il punto corrente */
				CX = X[IXP];

				/* Recupera il primo intervallo spline interessato */
				ISU = N - 1;
				while (ISU - ISL > 1)
					{
						ISP = (ISU + ISL) >> 1;
						if (XN[ISP] > CX)
							ISU = ISP;
						else
							ISL = ISP;
					}

				/* Estrae i valori di riferimento per l'intervallo spline corrente */
				XNL = XN[ISL];
				XNU = XN[ISU];
				YNL = YN[ISL];
				YNU = YN[ISU];
				DX = XNU - XNL;
				DY = YNU - YNL;

				/* Estrae il punto massimo per l'intervallo funzione corrente */
				IFU = XS - 1;
				IFL = IXP;
				while (IFU - IFL > 1)
					{
						IFP = (IFU + IFL) >> 1;
						if (X[IFP] > XNU)
							IFU = IFP;
						else
							IFL = IFP;
					}

				/* Aggiunge l'ultimo punto a fine spline */
				if (ISU == N - 1)
					IFU++;

				/* Ciclo creazione valori spline */
				while (IXP < IFU)
					{
						/* Estrae il punto corrente */
						CX = X[IXP];

						/* Imposta il valore nel punto */
						Y[IXP] = YNL + DY * ((CX - XNL) / DX);

						/* Passa al punto successivo */
						IXP++;
					}
			}
	}

/* Prepara la base per la spline cubica interpolante, ponendola in SPD2YN */
static void IFSplinePrepare(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal SPD1Y1, const DLReal SPD1YN,
	const CSplineType SPType, DLReal * SPD2YN, DLReal * UN)
	{
		int IM1;
		int I;
		int IP1;
		DLReal P;
		DLReal Q;
		DLReal S;
		DLReal U;

		/* Verifica il tipo di condizioni al contorno */
		switch (SPType)
			{
				case SplineBDef:
					SPD2YN[0] = (DLReal) -0.5;
					UN[0] = (((DLReal) 3.0) / (XN[1] - XN[0]))
							* ((YN[1] - YN[0]) / (XN[1] - XN[0]) - SPD1Y1);
				break;

				case SplineBNat:
					SPD2YN[0] = (DLReal) 0.0;
					UN[0] = (DLReal) 0.0;
				break;
			}

		/* Decomposizione sistema tridiagonale */
		for (IM1 = 0,I = 1,IP1 = 2;I < N - 1;IM1++,I++,IP1++)
			{
				S = (XN[I] - XN[IM1]) / (XN[IP1] - XN[IM1]);
				P = S * SPD2YN[IM1] + (DLReal) 2.0;
				SPD2YN[I] = (S - (DLReal) 1.0) / P;
				UN[I] = (YN[IP1] - YN[I]) / (XN[IP1] - XN[I]) -
					(YN[I] - YN[IM1]) / (XN[I] - XN[IM1]);
				UN[I] = (((DLReal) 6.0) * UN[I] / (XN[IP1] - XN[IM1]) -
					S * UN[IM1]) / P;
			}

		/* Verifica il tipo di condizioni al contorno */
		switch (SPType)
			{
				case SplineBDef:
					Q = (DLReal) 0.5;
					U = (((DLReal) 3.0) / (XN[N - 1] - XN[N - 2])) *
						(SPD1YN - (YN[N - 1] - YN[N - 2]) / (XN[N - 1] - XN[N - 2]));
				break;

				case SplineBNat:
					Q = (DLReal) 0.0;
					U = (DLReal) 0.0;
				break;
			}

		/* Sostituzione all'indietro per recupero derivate */
		SPD2YN[N - 1] = (U - Q * UN[N - 2]) /
			(Q * SPD2YN[N - 2] + ((DLReal) 1.0));
		for (I = N - 2,IP1 = N - 1;I >= 0;I--,IP1--)
			SPD2YN[I] = SPD2YN[I] * SPD2YN[IP1] + UN[I];
	}

/* Prepara la base per la spline cubica interpolante, ponendola in SPD2YN */
Boolean SplinePrepare(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal SPD1Y1, const DLReal SPD1YN,
	const CSplineType SPType, DLReal * SPD2YN)
	{
		DLReal * UN;

		/* Alloca l'array intermedio calcolo spline */
		if ((UN = (DLReal *) malloc((N - 1) * sizeof(DLReal))) == NULL)
			return False;

		/* Effettua la preparazione della spline */
		IFSplinePrepare(XN, YN, N, SPD1Y1, SPD1YN, SPType, SPD2YN, UN);

		/* Dealloca il vettore temporaneo */
		free(UN);

		/* Operazione completata */
		return True;
	}

/* Valore della spline cubica interpolante nel punto X */
DLReal SplineValue(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal * SPD2YN, const DLReal X)
	{
		int II;
		int IS;
		int IP;
		DLReal DX;
		DLReal W;
		DLReal Q;

		/* Recupera l'intervallo interessato */
		II = 0;
		IS = N - 1;
		while (IS - II > 1)
			{
				IP = (IS + II) >> 1;
				if (XN[IP] > X)
					IS = IP;
				else
					II = IP;
			}

		/* Calcola il valore nel punto */
		DX = XN[IS] - XN[II];
		W = (XN[IS] - X) / DX;
		Q = (X - XN[II]) / DX;
		return W * YN[II] + Q * YN[IS] +
			((W * W * W - W) * SPD2YN[II] + (Q * Q * Q - Q) * SPD2YN[IS]) *
			(DX * DX) / (DLReal) 6.0;
	}

/* Valore della spline cubica interpolante nei punti X */
/* X deve essere un array crescente strettamente monotonico */
void ASplineValue(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal * SPD2YN, const DLReal * X,
	const int XS, DLReal * Y)
	{
		/* Indici ricerca array spline */
		int ISL;
		int ISU;
		int ISP;

		/* Inici ricerca array funzione */
		int IFL;
		int IFU;
		int IFP;

		/* X Corrente */
		int IXP;
		DLReal CX;

		/* Estremi correnti array spline */
		DLReal XNL;
		DLReal XNU;
		DLReal YNL;
		DLReal YNU;
		DLReal SPD2YNL;
		DLReal SPD2YNU;

		/* Variabili intermedie calcolo spline */
		DLReal DX;
		DLReal W;
		DLReal Q;
		DLReal DX2;

		/* Ciclo sui punti disponibili */
		IXP = 0;
		ISL = 0;
		while (IXP < XS)
			{
				/* Estrae il punto corrente */
				CX = X[IXP];

				/* Recupera il primo intervallo spline interessato */
				ISU = N - 1;
				while (ISU - ISL > 1)
					{
						ISP = (ISU + ISL) >> 1;
						if (XN[ISP] > CX)
							ISU = ISP;
						else
							ISL = ISP;
					}

				/* Estrae i valori di riferimento per l'intervallo spline corrente */
				XNL = XN[ISL];
				XNU = XN[ISU];
				DX = XNU - XNL;
				DX2 = DX * DX;

				YNL = YN[ISL];
				YNU = YN[ISU];
				SPD2YNL = SPD2YN[ISL];
				SPD2YNU = SPD2YN[ISU];

				/* Estrae il punto massimo per l'intervallo funzione corrente */
				IFU = XS - 1;
				IFL = IXP;
				while (IFU - IFL > 1)
					{
						IFP = (IFU + IFL) >> 1;
						if (X[IFP] > XNU)
							IFU = IFP;
						else
							IFL = IFP;
					}

				/* Aggiunge l'ultimo punto a fine spline */
				if (ISU == N - 1)
					IFU++;

				/* Ciclo creazione valori spline */
				while (IXP < IFU)
					{
						/* Estrae il punto corrente */
						CX = X[IXP];

						/* Imposta i valori intermedi calcolo spline */
						W = (XNU - CX) / DX;
						Q = (CX - XNL) / DX;

						/* Calcola il valore nel punto */
						Y[IXP] = W * YNL + Q * YNU +
							((W * W * W - W) * SPD2YNL + (Q * Q * Q - Q) * SPD2YNU) *
							DX2 / (DLReal) 6.0;

						/* Passa al punto successivo */
						IXP++;
					}
			}
	}

/* Valore della B spline cubica uniforme nel punto X */
DLReal B3SplineValue(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal X)
	{
		DLReal BV;
		DLReal RX;
		DLReal Y;
		int IP;
		int IL;
		int IR;

		/* Recupera l'intervallo interessato */
		IL = 0;
		IR = N - 1;
		while (IR - IL > 1)
			{
				IP = (IR + IL) >> 1;
				if (XN[IP] > X)
					IR = IP;
				else
					IL = IP;
			}

		/* Rimappa X secondo l'intervallo corrente */
		RX = (X - XN[IL]) / (XN[IR] - XN[IL]);

		/* Inizializza il valore */
		Y = (DLReal) 0.0;

		/* Fattore di blending per IL - 1 */
		/* (((-t+3)*t-3)*t+1)/6 */
		BV = (((-RX + (DLReal) 3.0) * RX - (DLReal) 3.0) * RX + (DLReal) 1.0) /
			(DLReal) 6.0;
		if (IL > 0)
			Y += YN[IL - 1] * BV;
		else
			Y += (((DLReal) 2.0) * YN[0] - YN[1]) * BV;

		/* Fattore di blending per IL */
		/* (((3*t-6)*t)*t+4)/6 */
		BV = (((((DLReal) 3.0) * RX - (DLReal) 6.0) * RX) * RX + (DLReal) 4.0) /
			(DLReal) 6.0;
		Y += YN[IL] * BV;

		/* Fattore di blending per IR */
		/* (((-3*t+3)*t+3)*t+1)/6 */
		BV = (((((DLReal) -3.0) * RX + (DLReal) 3.0) * RX + (DLReal) 3.0) * RX + (DLReal) 1.0) /
			(DLReal) 6.0;
		Y += YN[IR] * BV;

		/* Fattore di blending per IR + 1 */
		/* (t*t*t)/6 */
		BV = (RX * RX * RX) / (DLReal) 6.0;
		if (IR + 1 < N)
			Y += YN[IR + 1] * BV;
		else
			Y += (((DLReal) 2.0) * YN[N - 1] - YN[N - 2]) * BV;

		/* Ritorna il valore calcolato */
		return Y;
	}

/* Valore della B spline cubica uniforme nei punti X */
/* X deve essere un array crescente strettamente monotonico */
void AB3SplineValue(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal * X, const int XS, DLReal * Y)
	{
		/* Indici ricerca array spline */
		int ISL;
		int ISU;
		int ISP;

		/* Inici ricerca array funzione */
		int IFL;
		int IFU;
		int IFP;

		/* X Corrente */
		int IXP;
		DLReal CX;

		/* X Corrente rimappato */
		DLReal RX;

		/* Estremi correnti array spline */
		DLReal XNL;
		DLReal XNU;
		DLReal YNL;
		DLReal YNU;
		DLReal YNLL;
		DLReal YNUU;

		/* Variabili intermedie calcolo spline */
		DLReal DX;

		/* Ciclo sui punti disponibili */
		IXP = 0;
		ISL = 0;
		while (IXP < XS)
			{
				/* Estrae il punto corrente */
				CX = X[IXP];

				/* Recupera il primo intervallo spline interessato */
				ISU = N - 1;
				while (ISU - ISL > 1)
					{
						ISP = (ISU + ISL) >> 1;
						if (XN[ISP] > CX)
							ISU = ISP;
						else
							ISL = ISP;
					}

				/* Estrae i valori di riferimento per l'intervallo spline corrente */
				XNL = XN[ISL];
				XNU = XN[ISU];
				YNL = YN[ISL];
				YNU = YN[ISU];
				if (ISL > 0)
					YNLL = YN[ISL - 1];
				else
					YNLL = (((DLReal) 2.0) * YN[0] - YN[1]);
				if (ISU + 1 < N)
					YNUU = YN[ISU + 1];
				else
					YNUU = (((DLReal) 2.0) * YN[N - 1] - YN[N - 2]);
				DX = XNU - XNL;

				/* Estrae il punto massimo per l'intervallo funzione corrente */
				IFU = XS - 1;
				IFL = IXP;
				while (IFU - IFL > 1)
					{
						IFP = (IFU + IFL) >> 1;
						if (X[IFP] > XNU)
							IFU = IFP;
						else
							IFL = IFP;
					}

				/* Aggiunge l'ultimo punto a fine spline */
				if (ISU == N - 1)
					IFU++;

				/* Ciclo creazione valori spline */
				while (IXP < IFU)
					{
						/* Rimappa X secondo l'intervallo corrente */
						RX = (X[IXP] - XNL) / DX;

						/* Imposta il valore nel punto */
						Y[IXP] = ((YNLL * ((((-RX + (DLReal) 3.0) * RX - (DLReal) 3.0) * RX + (DLReal) 1.0)))
							+ (YNL * ((((((DLReal) 3.0) * RX - (DLReal) 6.0) * RX) * RX + (DLReal) 4.0)))
							+ (YNU * ((((((DLReal) -3.0) * RX + (DLReal) 3.0) * RX + (DLReal) 3.0) * RX + (DLReal) 1.0)))
							+ (YNUU * ((RX * RX * RX))))
							/ ((DLReal) 6.0);

						/* Passa al punto successivo */
						IXP++;
					}
			}
	}

/* Valore della B spline cubica di tipo Catmull Rom nel punto X */
DLReal B3CRSplineValue(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal X)
	{
		DLReal BV;
		DLReal RX;
		DLReal Y;
		int IP;
		int IL;
		int IR;

		/* Recupera l'intervallo interessato */
		IL = 0;
		IR = N - 1;
		while (IR - IL > 1)
			{
				IP = (IR + IL) >> 1;
				if (XN[IP] > X)
					IR = IP;
				else
					IL = IP;
			}

		/* Rimappa X secondo l'intervallo corrente */
		RX = (X - XN[IL]) / (XN[IR] - XN[IL]);

		/* Inizializza il valore */
		Y = (DLReal) 0.0;

		/* Fattore di blending per IL - 1 */
		/* ((-t+2)*t-1)*t/2 */
		BV = (((-RX + (DLReal) 2.0) * RX - (DLReal) 1.0) * RX) /
			(DLReal) 2.0;
		if (IL > 0)
			Y += YN[IL - 1] * BV;
		else
			Y += (((DLReal) 2.0) * YN[0] - YN[1]) * BV;

		/* Fattore di blending per IL */
		/* (((3*t-5)*t)*t+2)/2 */
		BV = (((((DLReal) 3.0) * RX - (DLReal) 5.0) * RX) * RX + (DLReal) 2.0) /
			(DLReal) 2.0;
		Y += YN[IL] * BV;

		/* Fattore di blending per IR */
		/* ((-3*t+4)*t+1)*t/2 */
		BV = (((((DLReal) -3.0) * RX + (DLReal) 4.0) * RX + (DLReal) 1.0) * RX) /
			(DLReal) 2.0;
		Y += YN[IR] * BV;

		/* Fattore di blending per IR + 1 */
		/* ((t-1)*t*t)/2 */
		BV = ((RX - (DLReal) 1.0) * RX * RX) / (DLReal) 2.0;
		if (IR + 1 < N)
			Y += YN[IR + 1] * BV;
		else
			Y += (((DLReal) 2.0) * YN[N - 1] - YN[N - 2]) * BV;

		/* Ritorna il valore calcolato */
		return Y;
	}

/* Valore della B spline cubica di tipo Catmull Rom nei punti X */
/* X deve essere un array crescente strettamente monotonico */
void AB3CRSplineValue(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal * X, const int XS, DLReal * Y)
	{
		/* Indici ricerca array spline */
		int ISL;
		int ISU;
		int ISP;

		/* Inici ricerca array funzione */
		int IFL;
		int IFU;
		int IFP;

		/* X Corrente */
		int IXP;
		DLReal CX;

		/* X Corrente rimappato */
		DLReal RX;

		/* Estremi correnti array spline */
		DLReal XNL;
		DLReal XNU;
		DLReal YNL;
		DLReal YNU;
		DLReal YNLL;
		DLReal YNUU;

		/* Variabili intermedie calcolo spline */
		DLReal DX;

		/* Ciclo sui punti disponibili */
		IXP = 0;
		ISL = 0;

		/* Verifica se è rischiesto un output monotonico */
		while (IXP < XS)
			{
				/* Estrae il punto corrente */
				CX = X[IXP];

				/* Recupera il primo intervallo spline interessato */
				ISU = N - 1;
				while (ISU - ISL > 1)
					{
						ISP = (ISU + ISL) >> 1;
						if (XN[ISP] > CX)
							ISU = ISP;
						else
							ISL = ISP;
					}

				/* Estrae i valori di riferimento per l'intervallo spline corrente */
				XNL = XN[ISL];
				XNU = XN[ISU];
				YNL = YN[ISL];
				YNU = YN[ISU];
				if (ISL > 0)
					YNLL = YN[ISL - 1];
				else
					YNLL = (((DLReal) 2.0) * YN[0] - YN[1]);
				if (ISU + 1 < N)
					YNUU = YN[ISU + 1];
				else
					YNUU = (((DLReal) 2.0) * YN[N - 1] - YN[N - 2]);
				DX = XNU - XNL;

				/* Estrae il punto massimo per l'intervallo funzione corrente */
				IFU = XS - 1;
				IFL = IXP;
				while (IFU - IFL > 1)
					{
						IFP = (IFU + IFL) >> 1;
						if (X[IFP] > XNU)
							IFU = IFP;
						else
							IFL = IFP;
					}

				/* Aggiunge l'ultimo punto a fine spline */
				if (ISU == N - 1)
					IFU++;

				/* Ciclo creazione valori spline */
				while (IXP < IFU)
					{
						/* Rimappa X secondo l'intervallo corrente */
						RX = (X[IXP] - XNL) / DX;

						/* Imposta il valore nel punto */
						Y[IXP] = ((YNLL * ((((-RX + (DLReal) 2.0) * RX - (DLReal) 1.0) * RX)))
							+ (YNL * ((((((DLReal) 3.0) * RX - (DLReal) 5.0) * RX) * RX + (DLReal) 2.0)))
							+ (YNU * ((((((DLReal) -3.0) * RX + (DLReal) 4.0) * RX + (DLReal) 1.0) * RX)))
							+ (YNUU * (((RX - (DLReal) 1.0) * RX * RX))))
							/ ((DLReal) 2.0);

						/* Passa al punto successivo */
						IXP++;
					}
			}
	}

/* Interpolazione su scala lineare tramite B spline cubica
uniforme con riparametrizzazione arc length tramite B spline
cubica Catmull-Rom interpolante */
Boolean AB3CRMSplineValue(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal * X, const int XS, DLReal * Y)
	{
		/* Array gestione spline rimappatura */
		DLReal * SX;
		DLReal * IX;

		/* Indice generico */
		int I;

		/* Indice recupero monotonicità */
		int XP;

		/* Alloca l'array interpolazione parametro */
		if ((SX = new DLReal[XS]) == NULL)
		 return(False);

		/* Interpolazione parametro */
		AB3SplineValue(XN,XN,N,X,XS,SX);

		/* Estrae i soli valori monotonici */
		XP = 0;
		Y[0] = X[0];
		for (I = 1;I < XS;I++)
			if (SX[I] > SX[XP])
				{
					XP++;
					SX[XP] = SX[I];
					Y[XP] = X[I];
				}
		XP++;

		/* Alloca l'array rimappatura parametro */
		if ((IX = new DLReal[XS]) == NULL)
			{
				delete[] SX;
				return(False);
			}

		/* Rimappatura parametro */
		AB3CRSplineValue(SX,Y,XP,X,XS,IX);

		/* Dealloca l'array interpolazione parametro */
		delete[] SX;

		/* Interpolazione spline */
		AB3SplineValue(XN,YN,N,IX,XS,Y);

		/* Dealloca l'array rimappatura parametro */
		delete[] IX;

		/* Operazione completata */
		return True;
	}

/* Funzione di supporto a APCHIPSplineValue.
-1. if X1 and X2 are of opposite sign.
0. if either argument is zero.
+1. if X1 and X2 are of the same sign. */
static int CHST(const DLReal X1, const DLReal X2)
	{
		if ((X1 > (DLReal) 0.0 && X2 > (DLReal) 0.0) || (X1 < (DLReal) 0.0 && X2 < (DLReal) 0.0))
			return 1;
		if ((X1 > (DLReal) 0.0 && X2 < (DLReal) 0.0) || (X1 < (DLReal) 0.0 && X2 > (DLReal) 0.0))
			return -1;
		return 0;
	}

/* Interpolazione su scala lineare tramite
Hermite spline cubica monotonica */
void APCHIPSplineValue(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal * X, const int XS, DLReal * Y)
	{
		/* Indici ricerca array spline */
		int ISL;
		int ISU;
		int ISP;

		/* Inici ricerca array funzione */
		int IFL;
		int IFU;
		int IFP;

		/* X Corrente */
		int IXP;
		DLReal CX;

		/* X Corrente rimappato */
		DLReal RX;

		/* Estremi correnti array spline */
		DLReal XNL;
		DLReal XNU;
		DLReal YNL;
		DLReal YNU;

		/* Variabili calcolo derivate spline */
		DLReal ML;
		DLReal MU;
		DLReal HP;
		DLReal HK;
		DLReal HN;
		DLReal DP;
		DLReal DK;
		DLReal DN;

		DLReal DMIN;
		DLReal DMAX;

		/* Variabili intermedie calcolo spline */
		DLReal C1;
		DLReal C2;
		DLReal C3;
		DLReal C4;

		/* Ciclo sui punti disponibili */
		IXP = 0;
		ISL = 0;
		while (IXP < XS)
			{
				/* Estrae il punto corrente */
				CX = X[IXP];

				/* Recupera il primo intervallo spline interessato */
				ISU = N - 1;
				while (ISU - ISL > 1)
					{
						ISP = (ISU + ISL) >> 1;
						if (XN[ISP] > CX)
							ISU = ISP;
						else
							ISL = ISP;
					}

				/* Estrae i valori di riferimento per l'intervallo spline corrente */
				XNL = XN[ISL];
				XNU = XN[ISU];
				YNL = YN[ISL];
				YNU = YN[ISU];
				HK = XNU - XNL;
				DK = (YNU - YNL) / HK;

				/* Calcola le derivate per gli estremi del segmento corrente */
				if (N == 2)
					{
						/* Interpolazione lineare */
						ML = DK;
						MU = DK;
					}
				else
					{
						/* Derivata inferiore */
						if (ISL == 0)
							{
								/* Punto estremo */
								HN = (XN[ISU + 1] - XNU);
								DN = (YN[ISU + 1] - YNU) / HN;

								ML = HK + HN;
								ML = ((DK * (ML + HK)) - (DN * HK)) / ML;
								if (CHST(ML,DK) <= 0)
									ML = (DLReal) 0.0;
								else
									if (CHST(DK,DN) < 0)
										{
											DMAX = 3 * DK;
											if (std::abs<DLReal>(ML) > std::abs<DLReal>(DMAX))
												ML = DMAX;
										}
							}
						else
							{
								/* Punto intermedio */
								HP = (XNL - XN[ISL - 1]);
								DP = (YNL - YN[ISL - 1]) / HP;

								if (CHST(DP,DK) > 0)
									{
										C1 = std::abs<DLReal>(DP);
										C2 = std::abs<DLReal>(DK);
										if (C1 < C2)
											{
												DMIN = C1;
												DMAX = C2;
											}
										else
											{
												DMIN = C2;
												DMAX = C1;
											}

										C4 = HP + HK;
										C3 = C4 + C4 + C4;
										C1 = (C4 + HP)/C3;
										C2 = (C4 + HK)/C3;
										C3 = DP / DMAX;
										C4 = DK / DMAX;
										ML = DMIN / (C1 * C3 + C2 * C4);
									}
								else
									ML = (DLReal) 0.0;
							}

						/* Derivata superiore */
						if (ISU == N - 1)
							{
								/* Punto estremo */
								HP = (XNL - XN[ISL - 1]);
								DP = (YNL - YN[ISL - 1]) / HP;

								MU = HP + HK;
								MU = ((DK * (MU + HK)) - (DP * HK)) / MU;
								if (CHST(MU,DK) <= 0)
									MU = (DLReal) 0.0;
								else
									if (CHST(DP,DK) < 0)
										{
											DMAX = 3 * DK;
											if (std::abs<DLReal>(MU) > std::abs<DLReal>(DMAX))
												MU = DMAX;
										}
							}
						else
							{
								/* Punto intermedio */
								HN = (XN[ISU + 1] - XNU);
								DN = (YN[ISU + 1] - YNU) / HN;

								if (CHST(DK,DN) > 0)
									{
										C1 = std::abs<DLReal>(DK);
										C2 = std::abs<DLReal>(DN);
										if (C1 < C2)
											{
												DMIN = C1;
												DMAX = C2;
											}
										else
											{
												DMIN = C2;
												DMAX = C1;
											}

										C4 = HK + HN;
										C3 = C4 + C4 + C4;
										C1 = (C4 + HK) / C3;
										C2 = (C4 + HN) / C3;
										C3 = DK / DMAX;
										C4 = DN / DMAX;
										MU = DMIN / (C1 * C3 + C2 * C4);
									}
								else
									MU = (DLReal) 0.0;
							}
					}

				/* Prepara i coefficienti calcolo spline */
				C1 = (ML - DK) / HK;
				C2 = (MU - DK) / HK;
				C3 = -(C1 + C1 + C2);
				C4 = (C1 + C2) / HK;

				/* Estrae il punto massimo per l'intervallo funzione corrente */
				IFU = XS - 1;
				IFL = IXP;
				while (IFU - IFL > 1)
					{
						IFP = (IFU + IFL) >> 1;
						if (X[IFP] > XNU)
							IFU = IFP;
						else
							IFL = IFP;
					}

				/* Aggiunge l'ultimo punto a fine spline */
				if (ISU == N - 1)
					IFU++;

				/* Ciclo creazione valori spline */
				while (IXP < IFU)
					{
						/* Rimappa X secondo l'intervallo corrente */
						RX = X[IXP] - XNL;

						/* Imposta il valore nel punto */
						Y[IXP] = YNL + RX * (ML + RX * (C3 + RX * C4));

						/* Passa al punto successivo */
						IXP++;
					}
			}
	}

/* Interpolazione su scala lineare tramite B spline cubica
uniforme con riparametrizzazione arc length tramite spline
cubica Hermite monotonica */
Boolean AB3HSMSplineValue(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal * X, const int XS, DLReal * Y)
	{
		/* Array gestione spline rimappatura */
		DLReal * SX;
		DLReal * IX;

		/* Indice generico */
		int I;

		/* Indice recupero monotonicità */
		int XP;

		/* Alloca l'array interpolazione parametro */
		if ((SX = new DLReal[XS]) == NULL)
		 return(False);

		/* Interpolazione parametro */
		AB3SplineValue(XN,XN,N,X,XS,SX);

		/* Estrae i soli valori monotonici */
		XP = 0;
		Y[0] = X[0];
		for (I = 1;I < XS;I++)
			if (SX[I] > SX[XP])
				{
					XP++;
					SX[XP] = SX[I];
					Y[XP] = X[I];
				}
		XP++;

		/* Alloca l'array rimappatura parametro */
		if ((IX = new DLReal[XS]) == NULL)
			{
				delete[] SX;
				return(False);
			}

		/* Rimappatura parametro */
		APCHIPSplineValue(SX,Y,XP,X,XS,IX);

		/* Dealloca l'array interpolazione parametro */
		delete[] SX;

		/* Interpolazione spline */
		AB3SplineValue(XN,YN,N,IX,XS,Y);

		/* Dealloca l'array rimappatura parametro */
		delete[] IX;

		/* Operazione completata */
		return True;
	}

/* Funzione generica interpolazione */
/* Nel caso si utilizzi un asse logaritmico i valori su tale
asse devono	essere strettamente maggiori di 0 */
Boolean CSLASplineValue(const DLReal * XN, const DLReal * YN,
	const int N, const DLReal * X, const int XS, DLReal * Y,
	const CSLSplineType SplineType, const CSLAxisType AxisType)
	{
		/* Indice generico */
		int I;

		/* Array riferimento assi effettivi */
		DLReal * XNE;
		DLReal * YNE;
		DLReal * XSE;

		/* Array preparazione spline */
		DLReal * SPD2YNR;

		/* Valore di ritorno */
		Boolean RV = True;

		/* Verifica il tipo di asse utilizzato */
		switch (AxisType)
			{
				case CSLLinXLinY:
					/* Assegna gli assi di default */
					XNE = (DLReal *) XN;
					YNE = (DLReal *) YN;
					XSE = (DLReal *) X;
				break;

				case CSLLogXLinY:
					/* Alloca l'array per l'asse X */
					if ((XNE = new DLReal[N]) == NULL)
						return(False);
					if ((XSE = new DLReal[XS]) == NULL)
						{
							delete[] XNE;
							return(False);
						}

					/* Converte su scala logaritmica */
					for (I = 0;I < N;I++)
						XNE[I] = (DLReal) log10(XN[I]);
					for (I = 0;I < XS;I++)
						XSE[I] = (DLReal) log10(X[I]);

					/* Assegna gli assi di default */
					YNE = (DLReal *) YN;
				break;

				case CSLLinXLogY:
					/* Alloca l'array per l'asse Y */
					if ((YNE = new DLReal[N]) == NULL)
						return(False);

					/* Converte su scala logaritmica */
					for (I = 0;I < N;I++)
						YNE[I] = (DLReal) log10(YN[I]);

					/* Assegna gli assi di default */
					XNE = (DLReal *) XN;
					XSE = (DLReal *) X;
				break;

				case CSLLogXLogY:
					/* Alloca l'array per l'asse X */
					if ((XNE = new DLReal[N]) == NULL)
						return(False);
					if ((XSE = new DLReal[XS]) == NULL)
						{
							delete[] XNE;
							return(False);
						}

					/* Alloca l'array per l'asse Y */
					if ((YNE = new DLReal[N]) == NULL)
						{
							delete[] XNE;
							delete[] XSE;
							return(False);
						}

					/* Converte su scala logaritmica */
					for (I = 0;I < N;I++)
						XNE[I] = (DLReal) log10(XN[I]);
					for (I = 0;I < N;I++)
						YNE[I] = (DLReal) log10(YN[I]);
					for (I = 0;I < XS;I++)
						XSE[I] = (DLReal) log10(X[I]);
				break;
			}

		/* Verifica il tipo di interpolazione */
		switch (SplineType)
			{
				/* Lineare */
				case CSLLinear:
					AL1SplineValue(XNE,YNE,N,XSE,XS,Y);
				break;

				/* Spline cubica interpolante classica, classe C2, non monotonica */
				case CSLCSpline:
					/* Alloca l'array preparazione spline */
					if ((SPD2YNR = new DLReal[N]) == NULL)
						RV = False;
					else
						{
							/* Prepara le derivate spline */
							if (SplinePrepare(XNE,YNE,N,(DLReal) 0.0,(DLReal) 0.0,SplineBNat,SPD2YNR) == False)
								{
									delete[] SPD2YNR;
									RV = False;
								}

							/* Calcola i valori spline */
							ASplineValue(XNE,YNE,N,SPD2YNR,XSE,XS,Y);
						}
				break;

				/* B Spline cubica approssimante, classe C2, non interpolante */
				case CSLBSpline:
					if (AB3HSMSplineValue(XNE,YNE,N,XSE,XS,Y) == False)
						RV = False;
				break;

				/* CatmullRom, interpolante, non monotonica */
				case CSLCRSpline:
					AB3CRSplineValue(XNE,YNE,N,XSE,XS,Y);
				break;

				/* Hermite spline, interpolante, monotonica (PCHIP) */
				case CSLHSSpline:
					APCHIPSplineValue(XNE,YNE,N,XSE,XS,Y);
				break;
			}

		/* Verifica il tipo di asse utilizzato */
		switch (AxisType)
			{
				case CSLLinXLinY:
					/* Nessun array da deallocare */
				break;

				case CSLLogXLinY:
					/* Dealloca gli array */
					delete[] XNE;
					delete[] XSE;
				break;

				case CSLLinXLogY:
					/* Converte dalla scala logaritmica */
					for (I = 0;I < XS;I++)
						Y[I] = (DLReal) pow(10,Y[I]);

					/* Dealloca gli array */
					delete[] YNE;
				break;

				case CSLLogXLogY:
					/* Converte dalla scala logaritmica */
					for (I = 0;I < XS;I++)
						Y[I] = (DLReal) pow(10,Y[I]);

					/* Dealloca gli array */
					delete[] XNE;
					delete[] XSE;
					delete[] YNE;
				break;
			}

		/* Operazione completata */
		return RV;
	}

/* Funzione generica interpolazione, versione complessa */
/* Nel caso si utilizzi un asse logaritmico i valori di ampiezza
su tale	asse devono	essere strettamente maggiori di 0.
I valori di fase vengono sempre considerati su ascissa lineare */
Boolean CSLCASplineValue(const DLReal * XN, const DLReal * MN,
	const DLReal * PN, const int N, const DLReal * X, const int XS,
	DLComplex * Y, const CSLSplineType SplineType, const CSLAxisType AxisType)
	{
		/* Valori intermedi interpolazione */
		DLReal * YV;

		/* Indice generico */
		int I;

		/* Tipo asse fase */
		CSLAxisType PAT;

		/* Alloca l'array intermedio interpolazione */
		if ((YV = new DLReal[XS]) == NULL)
			return(False);

		/* Effettua l'interpolazione dell'ampiezza */
		if (CSLASplineValue(XN,MN,N,X,XS,YV,SplineType,AxisType) == False)
			{
				delete[] YV;
				return(False);
			}

		/* Assegna l'ampiezza */
		for (I = 0;I < XS;I++)
			Y[I] = YV[I];

		/* Imposta il tipo di asse per la fase */
		switch (AxisType)
			{
				case CSLLinXLinY:
				case CSLLogXLinY:
					PAT = AxisType;
				break;

				case CSLLinXLogY:
					PAT = CSLLinXLinY;
				break;

				case CSLLogXLogY:
					PAT = CSLLogXLinY;
				break;
			}

		/* Effettua l'interpolazione sulla fase */
		if (CSLASplineValue(XN,PN,N,X,XS,YV,SplineType,PAT) == False)
			{
				delete[] YV;
				return(False);
			}

		/* Assegna la fase */
		for (I = 0;I < XS;I++)
			Y[I] = std::polar<DLReal>(Y[I].real(),YV[I]);

		/* Dealloca l'array intermedio */
		delete[] YV;

		/* Operazione completata */
		return True;
	}
