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

/* Risolve il sistema di equazioni:

	AX = B

dove A è una matrice Toeplitz simmetrica di dimensione N. In A va posta
solo la prima riga della matrice. Per la soluzione viene usato il
classico metodo di Levinson. Ritorna 0 in caso di successo, 1 altrimenti. */

#ifndef Toeplitz_h
	#define Toeplitz_h

	#include "dsplib.h"

	int ToeplitzSolve(const DLReal * A, const DLReal * B, DLReal * X, int N);

#endif
