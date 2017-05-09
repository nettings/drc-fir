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

## usage: rs = blresample(is, rf, flen, fcut, att)
##
## Resample is by rf times with a filter
## flen taps long, fcut normalized cutoff frequency
## and att dB of stopband attenuation

function rs = blresample(is, rf, flen, fcut, att)
	# Check if it is a downsampling or an upsampling
	if rf < 1.0
		rs = fftconv(is, lowpassfir(flen,rf * fcut)' .* chebwin(flen,att));
	else
		rs = fftconv(is, lowpassfir(flen,fcut)' .* chebwin(flen,att));
	endif
	os = ceil(length(rs) * rf);
	rs = blinterpolate((0:(length(rs) - 1)),rs,linspace(0,length(rs) * os / (length(rs) * rf),os));
endfunction