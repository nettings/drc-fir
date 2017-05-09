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

## usage: ds = idownsample(is, df, flen, fcut, att)
##
## Downsample is by df times (integer) with a filter
## flen taps long, fcut normalized cutoff frequency
## and att dB of stopband attenuation. If att is negative
## a Blackman window is used.

function ds = idownsample(is, df, flen, fcut, att)
	df = floor(df);
	if (att < 0)
		ds = fftconv(is, lowpassfir(flen,fcut/df)' .* blackman(flen));
	else
		ds = fftconv(is, lowpassfir(flen,fcut/df)' .* chebwin(flen,att));
	endif
	ds = ds(floor(df/2):df:length(ds));
endfunction