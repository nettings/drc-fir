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

## usage: safetics(axis,tics,[labels])
## 
## Set tics and labels accordingly.
##
## axis = axis to be sets
## tics = tics to put on the axis
## labels = optional labels to put on the axis
## 
## See also tics()
##

function safetics(ax,tc,lb)	
	if (nargin < 3)
		# Imposta le etichette
		# tics(ax,tc,sprintf("%g|",tc))
		# tics(ax,tc,eval([ "[" sprintf("\"%g\";",tc) "]" ],"")')
		tics(ax,tc,num2str(tc,"%g\n"));
	else
		# Imposta le etichette
		tics(ax,tc,lb);
	endif
endfunction