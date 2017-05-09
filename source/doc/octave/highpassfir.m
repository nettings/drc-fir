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

## usage: f = highpassfir(n,fc)
##
## Compute and unwindowed highpass FIR filter
##
## n = filter len, if n is even an n-1 filter is generated
## fc = normalized cutoff frequency

function f = highpassfir(n,fc)

	ho = fix(n/2);
	c = pi * fc;
	f = -sin(c * (1:ho)) ./ (pi * (1:ho));
	f = [ f(ho:-1:1) (1-fc) f ];

endfunction