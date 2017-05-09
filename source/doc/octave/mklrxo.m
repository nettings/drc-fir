## Copyright (C) 2008 Denis Sbragion
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
## usage: [lp, hp] = mklrxo(fs,lpd,lpf,lpo,lpn,lpw,hpd,hpf,hpo,hpn,hpw)
##
## Makes an LR24 FIR xover pair
##
## fs: sample rate
## lpd: lowpass delay
## lpf: lowpass xover frequency
## lpo: lowpass xover half order
## lpn: lowpass xover filter length
## lpw: lowpass xover window start, from the end, 0 = no window
## hpd: highpass delay
## hpf: highpass xover frequency
## hpo: highpass xover half order
## hpn: highpass xover filter length
## hpw: highpass xover window start, from the end, 0 = no window
##
## Example:
##
##  [lp, hp] = mklrxo(44100,0,80,2,4096,512,0,80,2,4096,512);
##

function [lp, hp] = mklrxo(fs,lpd,lpf,lpo,lpn,lpw,hpd,hpf,hpo,hpn,hpw)
	# Computes the low pass filter
	[lpb, lpa] = butter(lpo,2 * lpf / fs);
	
	# Computes the high pass filter
	[hpb, hpa] = butter(hpo,2 * hpf / fs,'high');	
	
	# Computes the low pass impulse response
	lp = filter(lpb,lpa,filter(lpb,lpa,[1 zeros(1,lpn - 1)]));	
	lp = prepad(lp(1:(lpn-lpd)),lpn);
	if (lpw > 0)
		lp((1 + lpn - lpw):lpn) = lp((1 + lpn - lpw):lpn) .* blackman(2 * lpw)((lpw+1):(2 * lpw))';
	endif
	
	# Computes the high pass impulse response
	hp = filter(hpb,hpa,filter(hpb,hpa,[1 zeros(1,hpn - 1)]));	
	hp = prepad(hp(1:(hpn-hpd)),hpn);	
	if (hpw > 0)
		hp((1 + hpn - hpw):hpn) = hp((1 + hpn - hpw):hpn) .* blackman(2 * hpw)((hpw+1):(2 * hpw))';
	endif
endfunction