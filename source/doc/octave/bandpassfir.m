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

## usage: f = bandpassfir(n,lc,hc)
## 
## Compute an unwindowed bandpass FIR filter
##
## n = filter len
## lc = normalized low cutoff frequency
## hc = normalized high cutoff frequency

function f = bandpassfir(n,lc,hc)

	ho = fix(n/2);
	d = pi * (hc - lc) / 2;
	s = pi * (hc + lc) / 2;
	
	if (ho == n/2)
		f = (2 / pi) * sin(d * ((1:ho) - 0.5)) .* cos(s * ((1:ho) - 0.5)) ./ ((1:ho) - 0.5);
		f = [ f(ho:-1:1) f];
	else
	  f = (2 / pi) * sin(d * (1:ho)) .* cos(s * (1:ho)) ./ (1:ho);              
		f = [ f(ho:-1:1) (hc - lc) f ];	
	endif

endfunction