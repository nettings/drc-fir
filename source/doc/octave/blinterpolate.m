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

## usage: bls = blinterpolate(x, y, xi)
##
## Perform the band limited interpolation of 
## function y=f(x) at the points xi. f(x) is assumed
## equal to 0 outside of the values of x.

function bls = blinterpolate(x, y, xi)
	# Transpose the vectors appropriately
	x = x(:);
	y = y(:);	
	xi = xi(:);
	
	# Computes the interpolation
	bls = sinc(xi(:,ones(size(x))) - x(:,ones(size(xi)))') * y;	
endfunction