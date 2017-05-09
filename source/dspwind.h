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
  File        : DSPWind.h
  Autore      : Sbragion Denis
  Descrizione : Funzioni per vari tipi di finestratura.
  Revisioni   :
  16/10/93    : Prima stesura.
****************************************************************************/

#ifndef DSPWind_h
  #define DSPWind_h

	#include "dsplib.h"

	// Tipo per finestrature parziali
	typedef enum { WLeft, WFull, WRight } WindowType;

  // Effettuano la finestratura dell' array A secondo la finestra
  // corrispondente al nome.
  void HammingWindow(DLReal * A,unsigned int Size);
  void HanningWindow(DLReal * A,unsigned int Size);
  void BlackmanWindow(DLReal * A,unsigned int Size);
  void BartlettWindow(DLReal * A,unsigned int Size);
	void BlackmanHarrisWindow(DLReal * A,unsigned int Size);
  void ExponentialWindow(DLReal * A,unsigned int Size,DLReal P = 2);

	// Finestratura parziale e spaziata
	void SpacedBlackmanWindow(DLReal * A,unsigned int Size, unsigned int Space, WindowType WType);
	void HalfBlackmanWindow(DLReal * A,unsigned int Size, unsigned int Space, WindowType WType);

  // Riempie A di 1.
  void Ones(DLReal * A,unsigned int Size);

#endif

/***************************************************************************/
