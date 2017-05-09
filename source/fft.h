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
  File        : Fft.h
  Autore      : Sbragion Denis
  Descrizione : Funzioni per fft, ifft etc.
  Revisioni   :
  16/10/93    : Prima stesura.
****************************************************************************/

#ifndef Fft_h
  #define Fft_h

  #include "boolean.h"
  #include "dsplib.h"

  // Ritorna la potenza N-esima di X
  // Complessita` : O(log2(N)).
  DLComplex Power(DLComplex X,unsigned int N);

  // Ritorna la J-esima radice N-esima dell' unita`.
  // La prima ha indice 0, l' ultima N-1, poi si ripetono ciclicamente.
  DLComplex UnitRoot(unsigned int I,unsigned int N);

  // Ritorna il primo divisore di N.
  unsigned int FirstFactor(unsigned int N);

  // Fft e IFft a radice 2 dell' array P di lunghezza N
  // N deve essere una potenza di 2
  // Complessita` : O(N*log2(N)).
  void Radix2Fft(DLComplex P[],unsigned int N);
  void Radix2IFft(DLComplex P[],unsigned int N);

  // Fft e IFft a radice mista dell' array P di lunghezza N.
  // N puo` essere qualsiasi ma se contiene pochi fattori le
  // prestazione si riducono enormemente. In particolare per N primo
  // la funzione ha complessita` O(N^2) come per Dft() e IDft().
  // Se N contiene molti fattori uguali a 2 le prestazioni aumentano.
  // Necessita di spazio libero in memoria per un array di complessi di
  // lunghezza pari al piu` grande fattore di N.
  // Se non vi e` spazio la funzione si arresta e ritorna False, altrimenti
  // ritorna True.
  // Se N e` una potenza di 2 la versione a radice 2 e` circa il 5%
  // piu` veloce.
  Boolean Fft(DLComplex P[],unsigned int N);
  Boolean IFft(DLComplex P[],unsigned int N);

  // Valutazione del polinomio P nel punto X col metodo di Horner
  // Il polinomio e` considerato nella forma :
  // P[0] + P[1]*X + P[2]*X^2 + ... + P[N-1]*X^(N-1)
  // Complessita` : O(N).
  DLComplex PolyEval(DLComplex P[],unsigned int N,DLComplex X);

  // Dft e IDft del polinomio P (Valutazione nelle radici dell'unita `).
  // Richiede spazio in memoria per almemo N complessi. Se l' allocazione
  // non ha sucesso ritorna False altrimenti ritorna True;
  // Complessita` : O(N^2).
  Boolean Dft(DLComplex P[],unsigned int N);
  Boolean IDft(DLComplex P[],unsigned int N);

#endif

/***************************************************************************/
