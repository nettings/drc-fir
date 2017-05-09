## Copyright (C) 2005 Denis Sbragion
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
##(at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##
## This function is part of DRC - Digital room correction

## usage: [f, hm] = cdde(n,fs)
## 
## Compute an unwindowed cd deemphasis FIR filter of length n
##
## n = filter len
## fs = sample frequency
## f = filter coefficients
## hm = filter magnitude response, same length of f

function [f, hm] = cdde(n,fs)	
	# Calcola le frequenze di interpolazione	
	f = linspace(0,fs/2,n);
	
	# Calcola le componenti per il calcolo della risposta in ampiezza
	# Costanti di tempo 0.5 uS e 0.15 uS
	l = 2 * pi * 0.00005 * f(2:n);
	h = 2 * pi * 0.000015 * f(2:n);
	b = 1 + 1 ./ (l .* l);
	a = 1 + 1 ./ (h .* h);

	# Calcola la risposta in ampiezza
	hm = [ 1 (((0.09 * a) ./ b) .^ 0.5) ];
		
	# Prepara l'array per il calcolo della risposta temporale
	if (fix(n/2) == n/2)
		hm2 = [ hm hm(n:-1:1) ];
	else
		hm2 = [ hm hm((n-1):-1:1) ];
	endif
		
	# Estrae la risposta del filtro a fase minima
	[a, f] = rceps(real(ifft(hm2)));
	
	# Estrae la parte rilevante
	f = f(1:n);
endfunction