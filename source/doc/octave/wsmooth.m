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

## usage: [mag, freq] = wsmooth(ir,wl,ws);
##
## wsmooth - Smoothing by impulse response warping
##
## ir = input impulse response 
## wl = lambda used for warping, usually defined by barkwarp(fs) or erbwarp(fs) 
## with fs = sample rate of ir
## ws = smoothing window size, bigger values means less smoothing, typical value is 64
## ws must be less than floor(length(ir)/2)
##
## [mag, freq] = magnitude response in the frequency points defined by freq
## length(mag) = length(freq) = floor(length(ir)/2)
## freq is the normalized frequency between 0 and 1

function [mag, freq] = wsmooth(ir,wl,ws)

	# Computes the warped impulse response autocorrelation
	wir = fft(irwarp(ir,wl,length(ir)));
	wir = real(ifft(wir .* conj(wir)));
	
	# Computes the window
	wind = blackman(2 * ws);
	wind = [(wind((ws+1):(2*ws)))' zeros(1,length(wir) - (2*ws)) (wind(1:ws))'];
	
	# Windows the warped autocorrelation
	wir = wir .* wind;
	
	# Computes the frequency points
	freq = wfreq(wl,floor(length(ir)/2));
	
	# Computes the magnitude
	mag = fft(wir);
	mag = sqrt(abs(mag(1:floor(length(ir))/2)));

endfunction
