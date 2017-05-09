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

##
## usage: sev = cplxsmooth(s,fs,bw,pds)
##
## Computes the smoothed complex spectral envelope of signal s on a
## logarithmic frequency scale with bw bandwidth resolution and pds peak
## detection strength. The signal s must be centers with respect to the
## 0 of the axis time.
##
## s   = input signal.
## fs  = input signal sample rate, needed only if Bark or ERB scaling 
##       is used.
## bw  = bandwidth resolution as a faction of octave, for example 0.25
##       means 1/4 of octave resolution. A value of -1 means Bark scaling
##       resolution, a value of -2 means ERB scaling resolution.
## pds = peak detection strength, values above 50 migth lead to 
##       overflow. When set to values less than or equal to 1, and 
##       without any prepeak detection enabled, the algorithm provides
##       the usual smoothing, either with fractional octave or Bark/ERB. 
##       resolution. If any prepeak detection is enabled the smoothing 
##       is applied to the prepeak envelope.
## 
## sev = output smoothed complex spectral envelope, same length of s.
##
## Typical values for psychoacoustic evaluation of the perceived 
## frequency response:
## 
## sev = cplxsmooth(s,fs,-2,15);
##

function sev = cplxsmooth(s,fs,bw,pds,ppk)
	# Get the number of signal samples
	n = length(s);
	
	# Allocates the envelope array
	sev = zeros(1,n);
	
	# Check the type of resolution to use
	if (bw >= 0)
		# Computes the half bandwidth
		hbw = (2 ^ (bw / 2));	
		
		# Computes the lower and upper bounds	index
		bl = 1 + round((0:(n - 1)) / hbw);
		bu = 2 + round((0:(n - 1)) * hbw);
		
		# Limit the upper bound to the signal length
		bu = min(n + 1,bu);	
	else 
		if (bw >= -1)
			# Computes the Bark scaling
			bl = linspace(fs / (2 * (n - 1)),fs / 2,n - 1);
			sev(2:n) = barkbwidth(bl);
			sev(2:n) = (sqrt(sev(2:n) .^ 2 + 4 * bl .^ 2) + sev(2:n)) ./  (2 * bl);
			sev(1) = sev(2);
			
			# Computes the lower and upper bounds	index
			bl = 1 + round((0:(n - 1)) ./ sev);
			bu = 2 + round((0:(n - 1)) .* sev);
			
			# Limit the upper bound to the signal length
			bu = min(n + 1,bu);			
		else
			# Computes the ERB scaling
			bl = linspace(fs / (2 * (n - 1)),fs / 2,n - 1);
			sev(2:n) = erbbwidth(bl);
			sev(2:n) = (sqrt(sev(2:n) .^ 2 + 4 * bl .^ 2) + sev(2:n)) ./  (2 * bl);
			sev(1) = sev(2);
			
			# Computes the lower and upper bounds	index
			bl = 1 + round((0:(n - 1)) ./ sev);
			bu = 2 + round((0:(n - 1)) .* sev);				
			
			# Limit the upper bound to the signal length
			bu = min(n + 1,bu);
		endif
	endif	
	
	# Computes the signal spectrum
	sev = fft(postpad(s(:)',2 * n))(1:n);
		
	# Check if peak detection is enabled
	if (pds <= 1)
		# No peak detection, creates the standard fractional octave 
		# smoothing cumulative sum with appropriate padding
		sev = cumsum([0 sev]);
		
		# Perform the standard fractional octave smoothing
		sev = ((sev(bu) - sev(bl)) ./ (bu - bl));
	else	
		# Creates the peak smoothing cumulative sum
		# with appropriate padding
		sev = cumsum([0 sev] .^ pds);
				
		# Perform the peak enhanced smoothing
		sev = ((sev(bu) - sev(bl)) ./ (bu - bl)) .^ (1 / pds);
		
		# Creates the peak smoothing cumulative sum
		# with appropriate padding
		sev = cumsum([0 sev]);
	
		# Smooth the peak function		
		sev = (sev(bu) - sev(bl)) ./ (bu - bl);
	endif	
	
	# Creates the final smoothing cumulative sum
	# with appropriate padding
	sev = cumsum([0 sev]);
	
	# Performs the final spectrum smoothing and rescaling
	sev = (sev(bu) - sev(bl)) ./ (bu - bl);
endfunction