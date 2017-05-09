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

## usage: bw = barkbwidth(fp)
##
## Compute the Bark scale bandwidth for an array of
## frequency points
##
## fp = frequency points array

function bw = barkbwidth(fp)

	if (nargin != 1)
		usage("bw = barkbwidth(fp)");
	endif

	# Computes the bandwidths
	bw = 94 + 71 * ((fp / 1000) .^ (3/2));
	
endfunction