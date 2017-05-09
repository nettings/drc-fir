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

## usage: [mag, freq] = mrwsmooth(ir,wl,ws,fj);
##
## mrwsmooth - Multirate smoothing by impulse response warping
##
## ir = input impulse response 
## wll,wlh = lambda used for multirate warping, usually defined by barkwarp(fs) or erbwarp(fs) 
## with fs = sample rate of ir for wlh, and by barkwarp(fs/df) or erbwarp(fs/df) for wll
## ws = smoothing window size, bigger values means less smoothing, typical value is 64
## ws must be less than floor(length(ir/ds)/2)
## df = downsampling factor
## fj = normalized join frequency for the multirate filter, usually 0.5
## fl, att = filter length and stop band attenuation for the downsampling and the multirate filters
##
## [mag, freq] = magnitude response in the frequency points defined by freq
## length(mag) = length(freq) = floor(length(ir)/2)
## freq is the normalized frequency between 0 and 1

function [mag, freq] = mrwsmooth(ir,wl,ws,wm,fj,fl,att);

	# Makes the filter length odd
	fl = 1 + 2 * floor(fl/2);
	
	# Computes the warped impulse response
	wir = irwarp(ir,wl,length(ir))';
	wind = chebwin(fl,att)';
	wiru = fftconv(wir,highpassfir(fl,fj) .* wind);
	wird = fftconv(wir,lowpassfir(fl,fj) .* wind);
	
	# Computes the warped impulse response autocorrelation
	wiru = fft(wiru);
	wiru = real(ifft(wiru .* conj(wiru)));
	
	# Computes the window
	wsu = ws;
	wind = blackman(2*wsu);
	wind = [(wind((wsu+1):(2*wsu)))' zeros(1,length(wiru) - (2*wsu)) (wind(1:wsu))']';
	
	# Windows the warped autocorrelation
	wiru = wiru' .* wind;
	
	# Computes the downsampled warped impulse response autocorrelation
	wird = fft(wird);
	wird = real(ifft(wird .* conj(wird)));
	
	# Computes the window
	wsd = wm*ws;
	wind = blackman(2*wsd);
	wind = [(wind((wsd+1):(2*wsd)))' zeros(1,length(wird) - (2*wsd)) (wind(1:wsd))']';
	
	# Windows the warped autocorrelation
	wird = wird' .* wind;
	
	# Computes the magnitude
	mag = fft(wiru + wird);
	mag = sqrt(abs(mag(1:floor(length(wiru)/2))));
	
	# Computes the frequency points
	falu = floor(length(wiru)/2);
	freq = -angle(freqz([wl 1],[1 wl],falu))/pi;

endfunction
