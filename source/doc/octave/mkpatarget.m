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

## Usage: mkpatarget(s,fs,bw,pds,ppk,sf,ef,np,fname)

function mkpatarget(s,fs,bw,pds,ppk,sf,ef,np,fname)
	# Computes the envelope
	tev = ssev(s,fs,bw,pds,ppk);
	
	# Get the target frequency points
	tfp = logspace(log10(sf),log10(ef),np);
	
	# Interpolates the envenlope at the target frequencies
	tfm = interp1(linspace(0,fs/2,length(tev)),tev,tfp,'pchip');
	
	# Invert and escales the envelope to 0 db at the lower end
	tfm = tfm(1) ./ tfm;
	
	# Creates the target array
	ta = zeros(1,2 * np);
	ta(1:2:(2 * np)) = tfp;
	ta(2:2:(2 * np)) = 20 * log10(tfm(:));
	
	# Save the target
	tf = fopen(fname,"wt");
	fprintf(tf,"%f %f\n",ta);
	fclose(tf);
endfunction