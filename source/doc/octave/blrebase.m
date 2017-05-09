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

## usage: rs = blrebase(x, y, xi, flen, fcut, att)
##
## Rebase y=f(x) sampled on the uniform sampling vector x
## on the uniform sampling vector xi with a filter
## flen taps long, fcut normalized cutoff frequency
## and att dB of stopband attenuation

function rs = blrebase(x, y, xi, flen, fcut, att)
	# Computes the resampling factor
	rf = length(xi) / length(x);
	
	# Check if it is a downsampling or an upsampling
	if rf < 1.0
		rs = fftconv(y, lowpassfir(flen,rf * fcut)' .* chebwin(flen,att));
	else
		rs = fftconv(y, lowpassfir(flen,fcut)' .* chebwin(flen,att));
	endif
	
	# Computes the filter delay
	fd = (flen + 1) / 2;
	
	# Recompute the base
	xs = linspace(x(1) - fd,x(length(x)) + fd, length(rs));
	
	# Interpolates in the new base
	rs = blinterpolate(xs,rs,xi);
endfunction