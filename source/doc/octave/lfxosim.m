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
## usage: ir = lfxosim(fs,lpir,lpl,lpd,lpf,lpn,lpt,hpir,hpl,hpd,hpf,hpn,hpt)
##
## Makes linear phase xover simulation on two input impulse responses,
## returns the resulting combined impulse response.
##
## fs: sample rate
## lpir: lowpass impulse response
## lpl: lowpass level
## lpd: lowpass delay
## lpf: lowpass xover frequency
## lpn: lowpass xover filter length
## lpt: lowpass xover filter Dolph window stopband attenuation,
##      0 = use Blackman window
## hpir: highpass impulse response
## hpl: highpass level
## hpd: highpass delay
## hpf: highpass xover frequency
## lpn: highpass xover filter length
## hpt: highpass xover filter Dolph window stopband attenuation,
##      0 = use Blackman window
##
## Example:
##
##  ir = lrxosim(44100,ls,1,0,80,4096,0,lm,1,0,80,4096,0);
##

function ir = lfxosim(fs,lpir,lpl,lpd,lpf,lpn,lpt,hpir,hpl,hpd,hpf,hpn,hpt)
	# Computes the low pass filter
	if (lpt == 0)
		lpfir = lpl * prepad(bandpassfir(lpn - lpd,0,2 * lpf / fs) .* blackman(lpn - lpd)',lpn);
	else
		lpfir = lpl * prepad(bandpassfir(lpn - lpd,0,2 * lpf / fs) .* chebwin(lpn - lpd,lpt)',lpn);
	endif
	
	# Computes the high pass filter
	if (hpt == 0)
		hpfir = hpl * prepad(bandpassfir(hpn - hpd,2 * hpf / fs,1) .* blackman(hpn - hpd)',hpn);
	else
		hpfir = hpl * prepad(bandpassfir(hpn - hpd,2 * hpf / fs,1) .* chebwin(hpn - hpd,hpt)',hpn);
	endif	
	
	# Computes the xover impulse response
	ir = fftconv(lpfir,lpir) + fftconv(hpfir,hpir);
endfunction