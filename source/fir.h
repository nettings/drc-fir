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
  File        : Fir.h
  Autore      : Sbragion Denis
  Descrizione : Funzioni per il calcolo di filtri fir.
  Revisioni   :
  16/10/93    : Prima stesura.
****************************************************************************/

#ifndef Fir_h
  #define Fir_h

  #include "boolean.h"
  #include "dsplib.h"

	// Definizione tipo interpolazione
	typedef enum
		{
			Linear,
			Logarithmic,
			SplineLinear,
			SplineLogarithmic,
			PCHIPLinear,
			PCHIPLogarithmic
		}
	InterpolationType;

  // Ritornano i filtri fir a fase lineare di ordine Order corrispondenti
  // al nome della funzione. Tutti i filtri sono ricavati con la sola
  // finestra rettangolare.
  // Per i filtri HighPass e BandStop Order deve essere dispari.
  // Se non lo e` viene calcolato un filtro con Order-1 coefficienti
  // trascurando l' ultimo che viene posto a 0.
  // Order deve comunque essere maggiore o uguale a 2.
  void LowPassFir(DLReal * Filter,unsigned int Order,DLReal Freq);
  void HighPassFir(DLReal * Filter,unsigned int Order,DLReal Freq);
  void BandPassFir(DLReal * Filter,unsigned int Order,DLReal Low,
    DLReal High);
  void BandStopFir(DLReal * Filter,unsigned int Order,DLReal Low,
    DLReal High);

  // Ritorna il trasformatore di Hilbert di ordine Order ricavato con la
  // sola finestra rettangolare.
  // Order deve essere dispari altrimenti l' ultimo coefficiente viene
  // ignorato e posto uguale a 0. Inoltre Order deve essere maggiore di 2.
  void HilbertFir(DLReal * Filter,unsigned int Order);

  // Genera un filtro Fir con risposta generica tramite IFft e
  // finestratura. L' ordine Order deve essere maggiore o uguale a 2.
  // Il filtro e` ricavato con la sola finestra rettangolare. Si ricordi
  // che i filtri di ordine dispari presentano generalmente un
  // comportamento piu` regolare.
  // F rappresenta l' array delle frequenze su cui costruire la risposta
  // del  filtro. Deve essere in ordine crescente con il primo
  // coefficiente 0 e l' ultimo, corrispondente a meta` della frequenza
  // di campionamento, uguale a 1. M[I] e P[I] rappresentano
  // guadagno e fase desiderati nei corrispondenti punti F[I].
  // Ritorna False in caso di errori.
  // Se Is e` 0 l' array per l' interpolazione intermedia viene preso
  // con dimensione pari alla piu` piccola potenza di 2 che supera Order,
  // altrimenti viene preso con dimensione Is. Is non deve comunque essere
  // inferiore a Order. Per avere una elevata precisione nella
  // ricostruzione della risposta utilizzare un elevato valore di Is.
  // Se Is e` una potenza di 2 il calcolo risulta molto piu` rapido.
  Boolean GenericFir(DLReal * Filter,unsigned int Order,DLReal * F,
    DLReal * M, DLReal * P,unsigned int Np,unsigned int Is = 0,
		InterpolationType It = Linear);

#endif

/***************************************************************************/
