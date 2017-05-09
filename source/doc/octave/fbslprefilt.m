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

## usage: f = fbslprefilt(sf, sw, ef, ew, we, np)
##
## Compute the bilinear transformation based 
## prefiltering cutoff frequencies
##
## sf = start frequency
## sw = start window
## ef = end frequency
## ew = end window
## we = window exponent
## np = number of points

function f = fbslprefilt(sf, sw, ef, ew, we, np)

	if (nargin != 6)
		usage("f = fbslprefilt(sf, sw, ef, ew, we, np)");
	endif

	# Computes the prefiltering coefficients
	we = -1 + (we ^ 4) * (sf / ef);
	
	# Remap the window size between 0 and 1 in reverse order
	w = linspace(sw - ew,0,np) / (sw - ew);
	
	# Computes the filter cut frequency using a bilinear transform
	# f = (w * (1 + we)) / (1 + w * we)
	f = sf + ((ef - sf) * (w * (1 + we)) ./ (1 + w * we));
	
endfunction