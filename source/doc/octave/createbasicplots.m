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

## usage: createbasicplots(ptfname,outdir,fprefix);
##
## createbasicplots - Creates the basic DRC plots
##
## ptfname = psychoacoustic target sample response file name
## outdir = output directory for eps files
## fprefix = prefix for eps files

function createbasicplots(ptfname,outdir,fprefix);
	# Set the frequency dependent windowing basic parameters
	sf = 20;
	sw = 500;
	ef = 20000;
	ew = 0.5;
	np = 31768;
	ptw = 0.3; # 300 ms, psychoacoustic window
	dlw = 1.5; # Default line width
	
	# Default font sizes
	GDFs = 8;

	# Creates the windows array
	wa = linspace(ew,sw,np);
	
	# Computes the gabor limit
	gl = 1000 ./ (2 * linspace(ew/4,sw,np));

	# Report the stage
	disp("Frequency dependent windowing, linear scale...");
	fflush(stdout);
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Set default font size
	set(0,"defaultaxesfontsize",GDFs)
	
	# Plot the frequency dependent windowing curves
	for gn = 1:10
		plot(wa,fslprefilt(sf,sw,ef,ew,0.2 * gn,np),sprintf(";WE = %.1f;",0.2 * gn),"color",hsv2rgb([0.9 * (gn - 1) / 9, 1, 0.75]),"LineWidth",dlw);
		hold("on");
	endfor
	hold("off");
	
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (Hz)");
	axis([0.3, 550, 10, 21000]);
	safetics("x",0:50:550);
	safetics("y",0:1000:21000);
	
	# Print to the output file
	print([ outdir fprefix "-Linear.eps" ],"-depsc2");
	
	# Report the stage
	disp("Frequency dependent windowing, semilogy scale...");
	fflush(stdout);
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the frequency dependent windowing curves
	for gn = 1:10
		semilogy(wa,fslprefilt(sf,sw,ef,ew,0.2 * gn,np),sprintf(";WE = %.1f;",0.2 * gn),"color",hsv2rgb([0.9 * (gn - 1) / 9, 1, 0.75]),"LineWidth",dlw);
		hold("on");
	endfor
	hold("off");
	
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (Hz)");
	axis([0.3, 550, 10, 21000]);
	safetics("x",0:50:550);
	safetics("y",[10,20,30,50,100,200,300,500,1000,2000,3000,5000,10000,20000]);
	
	# Print to the output file
	print([ outdir fprefix "-SemiLogY.eps" ],"-depsc2");
	
	# Report the stage
	disp("Frequency dependent windowing, semilogx scale...");
	fflush(stdout);
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the frequency dependent windowing curves
	for gn = 1:10
		semilogx(wa,fslprefilt(sf,sw,ef,ew,0.2 * gn,np),sprintf(";WE = %.1f;",0.2 * gn),"color",hsv2rgb([0.9 * (gn - 1) / 9, 1, 0.75]),"LineWidth",dlw);
		hold("on");
	endfor
	hold("off");
	
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (Hz)");
	axis([0.3, 550, 10, 21000]);
	safetics("x",[0.3,0.5,1,2,3,5,10,20,30,50,100,200,300,500]);
	safetics("y",0:1000:21000);
	
	# Print to the output file
	print([ outdir fprefix "-SemiLogX.eps" ],"-depsc2");
		
	# Report the stage
	disp("Frequency dependent windowing, loglog scale...");
	fflush(stdout);
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the frequency dependent windowing curves
	for gn = 1:10
		loglog(wa,fslprefilt(sf,sw,ef,ew,0.2 * gn,np),sprintf(";WE = %.1f;",0.2 * gn),"color",hsv2rgb([0.9 * (gn - 1) / 9, 1, 0.75]),"LineWidth",dlw);
		hold("on");
	endfor
	hold("off");
	
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (Hz)");
	axis([0.3, 550, 10, 21000]);
	safetics("x",[0.3,0.5,1,2,3,5,10,20,30,50,100,200,300,500]);
	safetics("y",[10,20,30,50,100,200,300,500,1000,2000,3000,5000,10000,20000]);
	
	# Print to the output file
	print([ outdir fprefix "-LogLog.eps" ],"-depsc2");

	# Report the stage
	disp("Frequency dependent windowing, loglog scale with gabor limit...");
	fflush(stdout);
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the frequency dependent windowing curves
	for gn = 1:10
		loglog(wa,fslprefilt(sf,sw,ef,ew,0.2 * gn,np),sprintf(";WE = %.1f;",0.2 * gn),"color",hsv2rgb([0.9 * (gn - 1) / 9, 1, 0.75]),"LineWidth",dlw);
		hold("on");
	endfor
	loglog(linspace(ew/4,sw,np),gl,";Gabor;","color",[0.5,0.5,0.5],"LineWidth",dlw);
	hold("off");
	
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (Hz)");
	axis([0.3, 550, 10, 21000]);
	safetics("x",[0.3,0.5,1,2,3,5,10,20,30,50,100,200,300,500]);
	safetics("y",[10,20,30,50,100,200,300,500,1000,2000,3000,5000,10000,20000]);
	
	# Print to the output file
	print([ outdir fprefix "-Gabor.eps" ],"-depsc2");

	# Report the stage
	disp("Frequency dependent windowing, time-frequency jail...");
	fflush(stdout);
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the frequency dependent windowing curves
	semilogy(-wa/20,fslprefilt(sf,sw/20,ef,ew,1.0,np),"1;Pre-echo truncation;","LineWidth",dlw);
	hold("on");
	semilogy(wa,fslprefilt(sf,sw,ef,ew,1.0,np),"3;Ringing truncation;","LineWidth",dlw);
	hold("off");
	
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (Hz)");
	axis([-50, 550, 10, 21000]);
	safetics("x",-50:50:550);
	safetics("y",[10,20,30,50,100,200,300,500,1000,2000,3000,5000,10000,20000]);
	
	# Print to the output file
	print([ outdir fprefix "-Jail.eps" ],"-depsc2");

	# Report the stage
	disp("Frequency dependent windowing, psychoacoustic bandwidth comparison...");
	fflush(stdout);
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the frequency dependent windowing curves
	loglog(linspace(sf,ef,np)/1000,(1000 * 0.5) ./ (wslprefilt(sf,sw,ef,ew,1.0,np) + wslprefilt(sf,sw/20,ef,ew,1.0,np)),"1;normal.drc;","LineWidth",dlw);
	hold("on");
	loglog(linspace(sf,ef,np)/1000,(1000 * 2.0) ./ (wbslprefilt(sf,65,ef,0.46,1.87,np) + wbslprefilt(sf,10,ef,0.46,1.87,np)),"2;erb.drc;","LineWidth",dlw);
	hold("on");
	loglog(linspace(sf,ef,np)/1000,barkbwidth(linspace(sf,ef,np)),"3;Bark;","LineWidth",dlw);
	hold("on");
	loglog(linspace(sf,ef,np)/1000,erbbwidth(linspace(sf,ef,np)),"4;ERB;","LineWidth",dlw);
	hold("on");
	loglog(linspace(sf,ef,np)/1000,foctbwidth(linspace(sf,ef,np),1/3),"5;1-3 oct;","LineWidth",dlw);
	hold("on");
	loglog(linspace(sf,ef,np)/1000,foctbwidth(linspace(sf,ef,np),1/6),"0;1-6 oct;","LineWidth",dlw);
	hold("off");
	
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Frequency (Hz)");
	axis([0.01, 21, 0.5, 10000]);
	safetics("x",[0.01,0.02,0.03,0.05,0.1,0.2,0.3,0.5,1,2,3,5,10,20]);
	safetics("y",[0.5,1,2,3,5,10,20,30,50,100,200,300,500,1000,2000,3000,5000,10000]);
	legend("location","northeast");
	
	# Print to the output file
	print([ outdir fprefix "-BWidthCmp.eps" ],"-depsc2");

	# Report the stage
	disp("Frequency dependent windowing, proportional and bilinear comparison...");
	fflush(stdout);
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the frequency dependent windowing curves
	for gn = 1:7
		loglog(wa,fslprefilt(sf,sw,ef,ew,0.2 + 0.2 * gn,np),sprintf(";Prop. WE = %.1f;",0.2 + 0.2 * gn),"color",hsv2rgb([0.9 * (gn - 1) / 7, 1, 0.75]),"LineWidth",dlw);
		hold("on");
	endfor
	for gn = 1:7
		loglog(wa,fbslprefilt(sf,sw,ef,ew,0.2 + 0.2 * gn,np),sprintf(";Bil. WE = %.1f;",0.2 + 0.2 * gn),"color",hsv2rgb([0.9 * (gn - 1) / 7, 1, 0.50]),"LineWidth",dlw);
		hold("on");
	endfor
	hold("off");
	
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (Hz)");
	axis([0.3, 550, 10, 21000]);
	safetics("x",[0.3,0.5,1,2,3,5,10,20,30,50,100,200,300,500]);
	safetics("y",[10,20,30,50,100,200,300,500,1000,2000,3000,5000,10000,20000]);
	
	# Print to the output file
	print([ outdir fprefix "-BPComparison.eps" ],"-depsc2");

	# Report the stage
	disp("Transfer functions...");
	fflush(stdout);
	
	# Prepare the target functions
	bkf = [ 0.01 10 20 400 12800 20000 21500 22050 ];
	bkm = [ -20.0 -10.0 0.00 0.00 -5.00 -6.00 -10.0 -20.0 ];
	
	bk2f = [ 0.01 10 20 200 12800 20000 21500 22050 ];
	bk2m = [ -20.0 -10.0 0.00 0.00 -3.0 -3.5 -10.0 -20.0 ];
	
	bk2sf = [ 0.01 16 18 200 12800 20000 21500 22050 ];
	bk2sm = [ -200.0 -120.0 0.00 0.00 -3.0 -3.5 -10.0 -20.0 ];
	
	bk3f = [ 0.01 10 20 100 12800 20000 21500 22050 ];
	bk3m = [ -20.0 -10.0 0.00 0.00 -3.50 -4.00 -10.0 -20.0 ];
	
	bk3sf = [ 0.01 16 18 100 12800 20000 21500 22050 ];
	bk3sm = [ -200.0 -120.0 0.00 0.00 -3.50 -4.00 -10.0 -20.0 ];
	
	bk3sfsp = [ 0.01 14 15 16 17 18 19 20 50 100 150 19900 20000 20100 20200 20300 20400 20500 22050 ];
	bk3smsp = [ -200.0 -200.0 -200.0 -200.0 -100.0 0.00 0.00 0.00 0.00 0.00 0.00 -3.95 -4.00 -4.00 -98.0 -200.0 -200.0 -200.0 -200.0 ];

	# Plot the target functions
	semilogx(bkf/1000,bkm,"1;bk;","LineWidth",dlw);
	hold("on");
	
	semilogx(bk2f/1000,bk2m,"2;bk-2;","LineWidth",dlw);
	hold("on");
	
	semilogx(bk3f/1000,bk3m,"5;bk-3;","LineWidth",dlw);
	hold("on");
	
	semilogx(bk2sf/1000,bk2sm,"4;bk-2-sub;","LineWidth",dlw);
	hold("on");
	
	semilogx(bk3sf/1000,bk3sm,"3;bk-3-sub;","LineWidth",dlw);
	hold("on");
	
	lbk3sfsp = log10(0.01 + bk3sfsp);
	spx = log10(logspace(log10(0.01),log10(22050.01),np));
	semilogx((10 .^ ub3splinev(lbk3sfsp,lbk3sfsp,spx)) / 1000,
		-6 + ub3splinev(lbk3sfsp,bk3smsp,spx),";bk-3-subultra-spline (-6 dB);","color",[0.5, 0.5, 0.5],"LineWidth",dlw);
	hold("on");
	
	semilogx(logspace(log10(0.01),log10(22050.01),np) / 1000,
		-9 + interp1(log10(0.01 + bk3sf),bk3sm,spx,'pchip'),
		"0;bk-3-sub-pchip (-9 dB);","LineWidth",dlw);
	clear spx lbk3sfsp;
	hold("off");
	
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Frequency (Hz)");
	axis([0.01, 22.05, -30, 9]);
	safetics("x",[0.01,0.02,0.03,0.05,0.1,0.2,0.3,0.5,1,2,3,5,10,20]);
	safetics("y",-30:3:9);
	
	# Print to the output file
	print([ outdir fprefix "-DTFCmp.eps" ],"-depsc2");
	
	# Remove variables no longer used
	clear sf sw ef ew wa gl;	

	# Report the stage
	disp("Spectral envelope and standard smoothing comparison...");
	fflush(stdout);
	
	# Calcola la finestratura
	ptwl = round(ptw * 44100);
	
	# Carica il file campione
	ptr = loadpcm(ptfname);
		
	# Trova la posizione del picco
	[ptm, pti] = max(abs(ptr));
	
	# Effettua la finestratura
	ptr = (ptr((pti-(ptwl-1)):(pti+ptwl)) .* blackman(2 * ptwl));
	
	# Normalizza il file
	ptr = ptr / norm(ptr);
	
	# Calcola l'inviluppo spettrale
	ptev = 20 * log10(ssev(ptr,44100,-2,15));
	
	# Calcola lo spettro unsmoothed
	ptus = 20 * log10(abs(fft(postpad(ptr,2 * length(ptev)))(1:length(ptev))));
	
	# Calcola lo smoothing standard
	ptss = 20 * log10(ssev(ptr,44100,-2,1));
	
	# Calcola l'array risposta in frequenza
	ptf = linspace(0.0001,22050,length(ptev));
	
	# Calcola il punto di riferimento in frequenza
	ptfp = round(100 * 22050 / length(ptf));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");

	# Plot the spectral envelope example
	semilogx(ptf/1000,ptus,"1;Unsmoothed;","LineWidth",dlw);
	hold("on");
	
	semilogx(ptf/1000,ptss,"0;ERB smoothed;","LineWidth",dlw);
	hold("on");
	
	semilogx(ptf/1000,ptev,"3;ERB spectral envelope;","LineWidth",dlw);
	hold("on");
	
	semilogx([20 100 20000] / 1000,[0 0 log2(20000 / 100)] + ptev(ptfp),"5;Reference target;","LineWidth",dlw);
	hold("off");
	
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Magnitude (dB)");
	axis([0.02, 20, -35, 25]);
	safetics("x",[0.02,0.03,0.05,0.1,0.2,0.3,0.5,1,2,3,5,10,20]);
	safetics("y",-35:5:25);
	
	# Print to the output file
	print([ outdir fprefix "-SpectralEnvelope.eps" ],"-depsc2");
	
	# Remove variables no longer used
	clear ptf ptr ptev ptus ptss;	
	
	# Report the stage
	disp("Completed, wait for GNUPlot to complete before opening output files.");
	fflush(stdout);
endfunction