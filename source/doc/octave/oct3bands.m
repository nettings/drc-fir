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
## usage: [ cf, fb ] = oct3bands()
##
## Returns the center frequency and the frequency
## bounds for standard 1/3 of octave bands
##

function [ cf, fb ] = oct3bands()
	# Assign the center frequencies
	# extended by one band
	cf = [16 20 25 31.5 40 50 63 80 100 125 160 200 250 315 400 500 630 800 1000 1250 1600 2000 2500 3150 4000 5000 6300 8000 10000 12500 16000 20000 25000];
	
	# Computes the frequency bounds
	fb = (cf(1:(end-1)) .* cf(2:(end))) .^ 0.5;
	
	# Remove the unused center frequencies
	cf = cf(2:(end-1));
endfunction