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

## usage: f = fslprefilt(sf, sw, ef, ew, we, np)
## 
## Compute the exponential based prefiltering
## cutoff frequencies
## 
## sf = start frequency
## sw = start window
## ef = end frequency
## ew = end window
## we = window exponent
## np = number of points

function f = fslprefilt(sf, sw, ef, ew, we, np)

	if (nargin != 6)
		usage("f = fslprefilt(sf, sw, ef, ew, we, np)");
	endif

	# Computes the prefiltering coefficients
	B = exp(log(sf / ef) / we);
	Q = ((B * sw) - ew) / (1.0 - B);
	A = 1.0 / (ef * ((ew + Q) ^ we));
	
	# Computes the filter cut frequency
	f = 1.0 ./ (A * ((linspace(ew,sw,np) .+ Q) .^ we));

endfunction