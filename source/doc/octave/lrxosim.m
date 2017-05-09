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
## usage: ir = lrxosim(fs,lpir,lpl,lpd,lpf,lpo,hpir,hpl,hpd,hpf,hpo)
##
## Makes an LR24 xover simulation on two input impulse responses,
## returns the resulting combined impulse response.
##
## fs: sample rate
## lpir: lowpass impulse response
## lpl: lowpass level
## lpd: lowpass delay
## lpf: lowpass xover frequency
## lpo: lowpass xover half order
## hpir: highpass impulse response
## hpl: highpass level
## hpd: highpass delay
## hpf: highpass xover frequency
## hpo: highpass xover half order
##
## Example:
##
##  ir = lrxosim(44100,ls,1,0,80,2,lm,1,0,80,2);
##

function ir = lrxosim(fs,lpir,lpl,lpd,lpf,lpo,hpir,hpl,hpd,hpf,hpo)
	# Computes the low pass filter
	[lpb, lpa] = butter(lpo,2 * lpf / fs);
	
	# Computes the high pass filter
	[hpb, hpa] = butter(hpo,2 * hpf / fs,'high');
	
	# Computes the xover impulse response
	ir = filter(lpb,lpa,filter(lpb,lpa,prepad(lpir(1:(length(lpir)-lpd)),length(lpir)) * lpl)) + filter(hpb,hpa,filter(hpb,hpa,prepad(hpir(1:(length(hpir)-hpd)),length(hpir)) * hpl));
endfunction