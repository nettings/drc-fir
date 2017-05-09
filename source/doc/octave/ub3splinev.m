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

## usage: Y = ub3splinev(XN,YN,X)
##
## Computes the value of the uniform cubic B spline
## defined by XN and YN on the points of the vector X
## This is really slow because it simply evaluates the 
## spline function on all X points

function Y = ub3splinev(XN,YN,X)

	N = length(X);
	Y = zeros(1,N);
	for I = 1:N
		Y(I) = ub3splinex(XN,YN,X(I));
	endfor

endfunction