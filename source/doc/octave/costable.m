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

## usage: ct = costable(cv,cd,cn)
##
## Generate a cosine table by recurrence
##
## cv: cosine start value
## sd: cosine delta
## cn: cosine number of values

function ct = costable(cv,cd,cn);

	# Initializes the cos array
	ct = zeros(1,cn);
	
	# Initializes the cosine constants
	R = sin(cd/2);
	R = -4 * R * R;
	S = cos(cv);
	S = (S * (1 - cos(cd))) - sin(cv) * sin(cd);
		
	# Set the first value
	V = cos(cv);
	ct(1) = V;
	
	# Sine generation loop
	for I = 2:cn
		S = R * V + S;
		V = V + S;
		ct(I) = V;
	endfor

endfunction