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

## usage: st = sintable(sv,sd,sn)
##
## Generate a sin table by recurrence
##
## sv: sine start value
## sd: sine delta
## sn: sine number of values

function st = sintable(sv,sd,sn);

	# Initializes the sin array
	st = zeros(1,sn);
	
	# Initializes the sine constants
	R = sin(sd/2);
	R = -4 * R * R;
	S = sin(sv);
	S = sin(sd) * cos(sv) + (S * (1 - cos(sd)));
				
	# Set the first value
	V = sin(sv);
	st(1) = V;
	
	# Sine generation loop
	for I = 2:sn
		S = R * V + S;
		V = V + S;
		st(I) = V;
	endfor

endfunction