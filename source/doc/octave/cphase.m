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

##
## usage: cp = cphase(s)
##
## Computes the continuous phase function of signal s.
## cp = continuous phase, same length of s.

function cp = cphase(s)
	# Get the number of signal samples
	n = length(s);
	
	# Computes the signal spectrum	
	cp = fft(postpad(s(:)',2 * n))(1:n);
	
	# Save the reference angle
	r = angle(cp(1));
	
	# Computes the continuous phase
	cp(2:n) = r + cumsum(imag(diff(cp) ./ cp(2:n)));	
	cp(1) = r;
endfunction