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

## usage: tf = tfir(is, ic, cp, fp, fl, at, uf, ft, wm)
##
## Time-frequency impulse response
##
## is: input signal
## is: input signal center
## cp: time axis cicles points
## fp: filtering frequency points
## fl: filter length used for ETC filtering
## at: filter stopband attenuation, negative use blackman
## uf: integer upsampling factor for internal computation
## ft: filter type, 0 = lowpass, 1 = bandpass, 2 = morlet
## wm: wavelet frequency multiplier for Morlet wavelet, usually 2

function tf = tfir(is, ic, cp, fp, fl, at, uf, ft, wm);
	# Extracts the minimum frequency point
	mfp = min(fp);
	
	# Computes the upsampling range
	ib = ic + floor(2 * min(cp) / mfp);
	ie = ic + ceil(2 * max(cp) / mfp);
	
	# Rescales the input signal
	is = is(ib:ie);
	ic = ic - ib;
	
	# Makes the upsampling factor an integer
	uf = round(uf);
	
	# Rescales the filter length	
	fl = fl * uf;
	
	# Perform the initial upsampling
	if (uf > 1)
		ul = length(is) * uf;
		us = zeros(ul,1);
		us(1:uf:ul) = is;
	else
		ul = length(is);
		us = is(:);
	endif
	
	# Allocates the cumulative IR output matrix
	nf = length(fp);
	nc = length(cp);
	tf = zeros(nf,nc);
		
	# Check if morlet
	if (ft == 2)
		# Compute the sampling frequency needed to get the
		# desired base wavelet frequency
		wf = (wm * uf) / mfp;
				
		# Computes the wavelet length needed to get the desired
		# base filter length
		wl  = (fl - 1) / (2 * wf);
	endif
	
	# Add the last frequency point if missing
	if (fp(nf) < 1.0)
		fp = [fp 1.0];
	else
		# Decrease the number of points
		nf = nf - 1;
	endif		

	# Cumulative IR computation loop
	for I = 1:nf 
		# Check the filter type
		if (ft == 0)
			# Computes the filter center frequency
			ff = sqrt(fp(I) * fp(I + 1));
			
			# Computes the effective filter len
			efl = round(fl * mfp / ff);
			
			# Lowpass
			if (at < 0)
				ifir = lowpassfir(efl,ff/uf)' .* blackman(efl);
			else
				ifir = lowpassfir(efl,ff/uf)' .* chebwin(efl,at);
			endif
			
			# Computes the compensation factor
			cf = ff;
		elseif (ft == 1)
			# Computes the filter center frequency
			ff = sqrt(fp(I) * fp(I + 1));
			
			# Computes the effective filter len
			efl = round(fl * mfp / ff);
			
			# Bandpass
			if (at < 0)
				ifir = bandpassfir(efl,fp(I)/uf,fp(I + 1)/uf)' .* blackman(efl);
			else
				ifir = bandpassfir(efl,fp(I)/uf,fp(I + 1)/uf)' .* chebwin(efl,at);
			endif
			
			# Computes the compensation factor
			cf = fp(I + 1) - fp(I);
		elseif (ft == 2)
			# Computes the filter center frequency
			ff = sqrt(fp(I) * fp(I + 1));
			
			# Morlet
			ifir = morletw(wf,mfp / ff,0,wl,1,wm * pi);			
			
			# Estract the filter len
			efl = length(ifir);
			
			# Computes the compensation factor
			cf = ff ^ 0.5;
		else
			# Computes the filter center frequency
			ff = sqrt(fp(I) * fp(I + 1));
			
			# Computes the effective filter len
			efl = round(fl * mfp / ff);
			
			# Lowpass
			if (at < 0)
				ifir = lowpassfir(efl,ff/uf)' .* blackman(efl);
			else			
				ifir = lowpassfir(efl,ff/uf)' .* chebwin(efl,at);
			endif
			
			# Computes the compensation factor
			cf = ff;
		endif
		
		# Applies the filter
		if (efl > 1)
			ec = real(fftconv(ifir,us));
		else
			ec = us;
		endif
		
		# Computes the filter delay
		fd = (efl) / 2;
	
		# Computes the references
		el = ul + efl - 1;
		uc = (ic * uf) + fd;		
		
		# Computes the cicle rescaling
		cs = (-uc * ff) / (2 * uf);
		ce = ((el - uc) * ff) / (2 * uf);
				
		# Interpolate at the requested cycle points
		tf(I,:) = interp1(linspace(cs,ce,el), ec, cp, '*pchip') / cf;
	endfor
endfunction