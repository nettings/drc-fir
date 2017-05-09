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

## usage: pir = irslprefilt(ir,ic,sf,sw,ef,ew,we,fl,fs,wg,pg)
##
## Perform a sliding lowpass prefiltering on the right side of the time-frequency plane
##
## ir = impulse response
## ic = impulse center
## sf = normalized start frequency
## sw = start window
## ef = normalized end frequency
## ew = end window
## we = window exponent
## fl = filter len
## fs = filter sharpness
## wg = window gap
## pg = pre impulse center gap
##
## The output signal length is increased by floor((1 + 2 * floor((fl-1)/2))/2)
## so the final output length is given by:
##
## pg + sw + floor((1 + 2 * floor((fl-1)/2))/2)
##
## The input impulse response is left untouched up to sample pg + ew -1

function pir = irslprefilt(ir,ic,sf,sw,ef,ew,we,fl,fs,wg,pg)

	# Computes the effective filter len
	efl = 1 + 2 * floor((fl-1)/2);	
	
	# Extract the impulse response to be filtered
	ir = ir(:);
	if ((length(ir) - ic) < sw)
		wir = [ ir(ic:length(ir)) zeros(1,sw - (lenght(ir) - ic)) ];
	else
		wir = ir(ic:(ic + sw - 1));
	endif
	
	# Prepare the initial window
	esw = sw - wg;
	wind = blackman(2 * esw);
	wind = [ ones(1,wg) wind((esw + 1):(2 * esw))' ]';
	 
	# Apply the initial window
	wir = wir .* wind;
	
	# Pad the input ir
	wir = [ wir' zeros(1,efl) ];
	
	# Computes the half filter length
	hfl = floor(efl/2);
	
	# Creates the output ir
	pir = zeros(1,sw + hfl);
	
	# Creates the base filter window
	wind = blackman(efl);
			
	# Computes the base filter
	wfir = lowpassfir(efl,sf) .* wind'; 
	
	# Set the initial parameters
	cfl = efl;
	chfl = hfl;
	
	# Initial lowpass convolution loop
	for wp = sw+hfl:-1:sw-1
		# Check if the window limit has been reached	
		cws = floor(fs * wp);	
		if (chfl > cws)
			# Recomputes the filter lengths
			chfl = cws;
			cfl = 1 + (chfl * 2);
			
			# Creates the window
			wind = blackman(cfl);
			
			# Creates the filter
			wfir = lowpassfir(cfl,sf) .* wind'; 
		endif		
		
		# Output the filtered sample
		pir(wp) = wir((1+wp-chfl):(1+wp+chfl)) * wfir';
	endfor
	
	## Computes the prefiltering coefficients
	B = exp(log(sf / ef) / we);
	Q = ((B * sw) - ew) / (1.0 - B);
	A = 1.0 / (ef * ((ew + Q) ^ we));
	
	# Set the initial parameters
	cfl = efl;
	chfl = hfl;
	
	# Main convolution loop
	for wp = sw:-1:ew
		# Computes the filter cut frequency
		fc = 1.0 / (A * ((wp + Q) ^ we));
		
		# Check if the window limit has been reached	
		cws = floor(fs * wp);
		if (chfl > cws)
			# Recomputes the filter lengths
			chfl = cws;
			cfl = 1 + (chfl * 2);		
			
			# Creates the window
			wind = blackman(cfl);
		endif	
	
		# Creates the filter
		wfir = lowpassfir(cfl,fc) .* wind'; 
		
		# Output the filtered sample
		pir(wp) = wir((1+wp-chfl):(1+wp+chfl)) * wfir';
	endfor
	
	# Add the initial part of the ir
	pir = [ ir((ic-pg):(ic+ew-1))' pir(ew:(sw+hfl)) ];
		
endfunction