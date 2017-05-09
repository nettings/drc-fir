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
  File        : Convol.h
  Autore      : Sbragion Denis
  Descrizione : Funzioni per la convoluzione.
  Revisioni   :
  16/10/93    : Prima stesura.
****************************************************************************/

#ifndef Convol_h
  #define Convol_h

  #include "dsplib.h"

  // Effettua la convoluzione con il metodo diretto degli array A e B.
  // Il risultato viene posto in R che deve essere di lunghezza NA+NB-1.
  void Convolve(const DLReal * A,unsigned int NA,const DLReal * B,
  	unsigned int NB,DLReal * R);
  void Convolve(const DLComplex * A,unsigned int NA,const DLComplex * B,
  	unsigned int NB,DLComplex * R);

    // Effettua la convoluzione usando l' Fft col metodo diretto.
  // Questo sistema puo`, in alcunni casi, risultare piu` veloce del
  // metodo standard.
  // Richiede l' allocazione di array temporanei e ritorna False in
  // caso di memoria insufficiente.
  Boolean DFftConvolve(const DLReal * A,unsigned int NA,const DLReal * B,
    unsigned int NB,DLReal * R,DLComplex * CA = NULL,DLComplex * CB = NULL);
  Boolean DFftConvolve(const DLComplex * A,unsigned int NA,const DLComplex * B,
	unsigned int NB, DLComplex * R,DLComplex * CA = NULL,DLComplex * CB = NULL);

  // Effettua la convoluzione usando l' Fft col metodo overlap-add.
  // Questo sistema puo`, in alcunni casi, risultare piu` veloce del
  // metodo standard.
  // Richiede l' allocazione di array temporanei e ritorna False in
  // caso di memoria insufficiente.
  Boolean OAFftConvolve(const DLReal * A,unsigned int NA,const DLReal * B,
    unsigned int NB,DLReal * R,DLComplex * CA = NULL,DLComplex * CB = NULL);
  Boolean OAFftConvolve(const DLComplex * A,unsigned int NA,const DLComplex * B,
    unsigned int NB,DLComplex * R,DLComplex * CA = NULL,DLComplex * CB = NULL);

#endif

/***************************************************************************/
