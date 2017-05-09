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

## usage: us = iupsample(is, uf, flen, fcut, att)
##
## Upsample is by uf times (integer) with a filter
## flen taps long, fcut normalized cutoff frequency
## and att dB of stopband attenuation. If att is negative
## a Blackman window is used.

function us = iupsample(is, uf, flen, fcut, att)
	uf = floor(uf);
	ul = length(is) * uf;
	us = zeros(ul,1);
	us(1:uf:ul) = is;
	if (att < 0)
		us = fftconv(us, lowpassfir(flen,fcut/uf)' .* blackman(flen));
	else
		us = fftconv(us, lowpassfir(flen,fcut/uf)' .* chebwin(flen,att));
	endif
endfunction