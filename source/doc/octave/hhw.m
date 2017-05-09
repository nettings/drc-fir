## Copyright (C) 2007 Denis Sbragion
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

##
## usage: hh = hhw(fi,f,n)
##
## Generate the Hilbert-Hermitian wavelet
##
## See: http://en.wikipedia.org/wiki/Hilbert-Hermitian_wavelet
##
## fi: fi coefficient, if fi <= 0 then the optimal fi = (2 + sqrt(2))/4 is used
## f: central frequency
## n: number of samples
##

function hh = hhw(fi,f,n);

# Check if fi needs to be computed
if (fi <= 0)
	fi = (2 + sqrt(2))/4;
endif

# Computes cfi
cfi = sqrt(2) * (((fi ^ -(fi + 0.5))  * gamma(fi + 0.5))  ^ -0.5);

# Computes the frequency definition of the wavelet
hh = linspace(0, 0.5 * pi * n / (f * ceil(n / 2)), ceil(n / 2));
hh = (cfi * (hh .^ fi)) .* exp(-0.5 * fi * (hh .^ 2));

# Returns the time domain definition of the wavelet
hh = fftshift(ifft([hh zeros(1,n - ceil(n / 2))]));

endfunction