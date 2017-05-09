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
## usage: [lp, hp] = mklfxo(fs,lpd,lpf,lpn,lpt,lpl,hpd,hpf,hpn,hpt,hpl)
##
## Makes a linear phase FIR xover pair
##
## fs: sample rate
## lpd: lowpass delay
## lpf: lowpass xover frequency
## lpn: lowpass xover filter length
## lpt: lowpass xover filter Dolph window stopband attenuation,
##      0 = use Blackman window
## lpl: lowpass lower frequency, usually 0
## hpd: highpass delay
## hpf: highpass xover frequency
## hpo: highpass xover half order
## hpn: highpass xover filter length
## hpt: highpass xover filter Dolph window stopband attenuation,
##      0 = use Blackman window
## hpl: highpass upper frequency, usually fs / 2
##
## Example:
##
##  [lp, hp] = mklfxo(44100,0,80,4096,0,0,0,80,4096,0,22050);
##

function [lp, hp] = mklfxo(fs,lpd,lpf,lpn,lpt,lpl,hpd,hpf,hpn,hpt,hpl)
	# Computes the low pass filter
	if (lpt == 0)
		lp = prepad(bandpassfir(lpn - lpd,2 * lpl / fs,2 * lpf / fs) .* blackman(lpn - lpd)',lpn);
	else
		lp = prepad(bandpassfir(lpn - lpd,2 * lpl / fs,2 * lpf / fs) .* chebwin(lpn - lpd,lpt)',lpn);
	endif
	
	# Computes the high pass filter
	if (hpt == 0)
		hp = prepad(bandpassfir(hpn - hpd,2 * hpf / fs,2 * hpl / fs) .* blackman(hpn - hpd)',lpn);
	else
		hp = prepad(bandpassfir(hpn - hpd,2 * hpf / fs,2 * hpl / fs) .* chebwin(hpn - hpd,hpt)',lpn);
	endif
endfunction