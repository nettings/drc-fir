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
## usage: sub = ssub(s,bw)
##
## Computes the smoothed spectral upper bound of signal s on a
## logarithmic frequency scale with bw bandwidth resolution.
##
## s   = input signal.
## bw  = bandwidth resolution as a faction of octave, for example 0.25
##       means 1/4 of octave resolution.
## sub = output spectral envelope, same length of s.
##

function sub = ssub(s,bwidth)
	# Get the number signal samples
	n = length(s);
	
	# Computes the signal spectrum
	sp = abs(fft(postpad(s,2 * n)))(1:n);
	
	# Computes the half bandwidth
	hbw = (2 ^ (bwidth / 2));
	
	# Allocates the upper bound array
	sub = zeros(1,n);
	
	# Set the initial max position
	mp = 0;
		
	# Cicles on the signal spectrum
	for I = 1:n
		# Computes the upper and lower limit of computation
		bl = 1 + round((I - 1) / hbw);
		bu = 1 + round((I - 1) * hbw);
		
		# Check the computation limits
		bu = min(bu,n);
		
		# Check if the last max position 
		# has been passed away
		if (bl > mp)
			mv = 0;
			pbu = bl;
		endif		
		
		# Get the new max
		[m, p] = max(sp(pbu:bu));
		if (m > mv)
			mv = m;
			mp = p;
		endif
		sub(I) = mv;
		pbu = bu + 1;
	endfor
endfunction