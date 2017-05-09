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

## usage: freq = wfreq(wl,n);
##
## wfreq - Return the normalized frequency mapping for the warping factor wl
##
## wl = lambda used for warping, usually defined by barkwarp(fs) or erbwarp(fs) 
## with fs = sample rate of ir
## n = number of frequency points
##
## freq is the normalized frequency between 0 and 1

function freq = wfreq(wl,n)	

	# Computes the frequency points
	freq = linspace(0,pi,n);
	freq = (freq + 2 * atan((-wl * sin(freq))./(1 + wl * cos(freq))))/pi;	

endfunction
