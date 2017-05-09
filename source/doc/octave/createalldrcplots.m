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

## usage: createalldrcplots(ls,lc,rs,rc,outdir);
##
## createalldrcplots - Creates all the DRC manual plots
##
## ls = left channel uncorrected source file name
## lc = left channel corrected source file name
## rs = left channel uncorrected source file name
## rc = left channel corrected source file name
## outdir = output directory for eps files

function createalldrcplots(ls,lc,rs,rc,outdir);
	# Creates the basic plots
	createbasicplots("sesample/ptr.pcm",outdir,"DBP");

	# Load the impulse response files
	lsir = loadpcm(ls);
	lcir = loadpcm(lc);
	rsir = loadpcm(rs);
	rcir = loadpcm(rc);
	
	# Creates the Dirac delta
	dcir = zeros(length(lcir),1);
	dcir(round(length(dcir) / 2)) = 1.0;
	
	# Create the plots for the left channel
	createdrcplots(lsir,-1,'L Uncorrected',lcir,-1,'L Corrected',outdir,'SR-L');
	
	# Create the plots for the rigth channel
	createdrcplots(rsir,-1,'R Uncorrected',rcir,-1,'R Corrected',outdir,'SR-R');
	
	# Create the baseline plots
	createdrcplots(dcir,-1,'Dirac delta',lcir,-1,'L Corrected',outdir,'SR-B');
	
	# Unload the variables used
	clear lsir lcir rsir rcir dcir;
endfunction
