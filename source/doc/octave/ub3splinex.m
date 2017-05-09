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

## usage: Y = ub3splinex(XN,YN,X) 
##
## Compute the value of the uniform cubic B spline
## defined by XN and YN in the point X

function Y = ub3splinex(XN,YN,X)

	# Find the spline interval by 
	# dichotomic search
	N = length(XN);
	IL = 1;
	IR = N;
	while (IR - IL > 1)
		IP = floor((IR + IL) / 2);
		if (XN(IP) > X) 
			IR = IP;
		else 
			IL = IP;
		endif
	endwhile
	
	# Remap X following the spline interval
	RX = (X - XN(IL)) / (XN(IR) - XN(IL));
	
	# Set the starting output value
	Y = 0.0;
	
	# Set the blending factor for IL - 1
	# (((-t+3)*t-3)*t+1)/6
	BV = (((-RX + 3.0) * RX - 3.0) * RX + 1.0) / 6.0;
	if (IL > 1)
		Y += YN(IL - 1) * BV;
	else
		Y += (2.0 * YN(1) - YN(2)) * BV;
	endif
		
	# Set the blending factor for IL
	# (((3*t-6)*t)*t+4)/6 */
	BV = (((3.0 * RX - 6.0) * RX) * RX + 4.0) / 6.0; 
	Y += YN(IL) * BV;
	
	# Set the blending factor for IR
	# (((-3*t+3)*t+3)*t+1)/6 
	BV = (((-3.0 * RX + 3.0) * RX + 3.0) * RX + 1.0) / 6.0;
	Y += YN(IR) * BV;
	
	# Set the blending factor for IR + 1
	# (t*t*t)/6 
	BV = (RX * RX * RX) / 6.0;
	if (IR + 1 <= N)
		Y += YN(IR + 1) * BV;
	else
		Y += (2.0 * YN(N) - YN(N - 1)) * BV;
	endif

endfunction