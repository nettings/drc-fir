## Copyright (C) 2007 Denis Sbragion
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
## usage: [rt, r2] = rt60(fs,ir,ic,rtf,rts,rta,fl,at)
##
## Compute the rt60 for the given frequency ranges.
## Returns an array with length(rtf) - 1 values.
## r2 contains the correlation coefficients of the linear 
## regressions to the Schroeder backward integration.
##
## fs: sample rate.
## ir: impulse response.
## ic: impulse center, -1 for automatic identification.
## rtf: array with the rt60 frequency bands.
## rts: initial sample after the impulse center where the rt60 computation starts
##      if rts < 0, then the computatin starts at the first sample with 
##      attenuation rts.
## rta: attenuation, in dB, after the inital sample where the rt60 computation stops.
## fl: length of the filter used for the band filtering,
##     if fl < 0 a butterwoth filter of order abs(fl) is used.
## at: stopband attenuation for the band filters, if fl < 0
##     and at > 0 then a double filtering with intermediate time reversal
##     is used to achieve a linear phase filter response.
##
## Examples:
##
## Standard ISO 1/3 octave RT60 analysis
##
##  [cf, fb] = oct3bands();
##  rt = rt60(44100,ir,-1,fb,-5,-30,-3,0)
##
## Equivalent FIR linear phase analysis
##
##  [cf, fb] = oct3bands();
##  rt = rt60(44100,ir,-1,fb,-5,-30,16384,90)
##
## Equivalent IIR double filtering linear phase analysis
##
##  [cf, fb] = oct3bands();
##  rt = rt60(44100,ir,-1,fb,-5,-30,-2,1)
##
## Note: usually the linear phase analysis, either FIR or IIR, provides
## more stable and reliable results, especially at the lowest frequencies
##

function [rt, r2] = rt60(fs,ir,ic,rtf,rts,rta,fl,at);
	# Find the impulse center if needed
	if (ic < 0)
		[m,ic] = max(abs(hilbert(ir)));
	endif
	
	# Check the kind of filter to be used
	if (fl > 0)
		# Computes the filter window
		fw = chebwin(fl,at)';
		
		# Computes the filtered impulse center
		fic = round(ic + (fl - 1) / 2);
	else
		fic = ic;
	endif
	
	# Allocates the output arrays
	rt = zeros(1,length(rtf) - 1);
	r2 = zeros(1,length(rtf) - 1);	
	
	# Cycles on the frequency ranges	
	for cb = 1:(length(rtf) - 1)
		# Check the kind of filter to be used
		if (fl > 0)
			# Computes the current filter
			bf = bandpassfir(fl,2 * rtf(cb) / fs,2 * rtf(cb + 1) / fs) .* fw;
			
			# Computes the band ir
			bir = fftconv(bf,ir);
		else
			# Computes the current filter
			[bfb, bfa] = butter(-fl,[min(1,2 * rtf(cb) / fs),min(1,2 * rtf(cb + 1) / fs)]);
			
			# Computes the band ir
			bir = filter(bfb,bfa,ir);
			if (at > 0)			
				bir(length(bir):-1:1) = filter(bfb,bfa,bir((length(bir):-1:1)));
			endif
		endif
		
		# Computes the Schroeder backward integration
		iidc(length(bir):-1:1) = cumsum(bir(length(bir):-1:1).^2);
		
		# Normalize the backward integration		
		iidc = iidc / iidc(1);
		
		# Convert to log scale
		iidc = 10 * log10(iidc);
		
		# Check if the first sample need to be found
		if (rts < 0)
			# Find the index of the start attenuation
			iai = min(find(iidc <= rts));
			
			# Assign the attenuation at the start point
			sat = iidc(iai);
		else		
			# Find the attenuation at the start point
			sat = iidc(fic + rts);
			
			# Assign the start point
			iai = fic + rts;
		endif		
		
		# Find the index of the stop attenuation
		sai = min(find(iidc <= (sat + rta)));
		
		# Check if the stop attenuation has been found
		if (sai > 0)		
			# Computes the linear slope
			[irs, S] = polyfit((0:(sai - iai)) / fs,iidc(iai:sai),1);
			
			# Assign the rt60 and the correlation coefficient
			rt(cb) = -60 / irs(1);
			r2(cb) = corrcoef(iidc(iai:sai),S.yf);
		endif
	endfor	
endfunction