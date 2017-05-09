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

## usage: createdrcplots(ir1,ic1,it1,ir2,ic2,it2,outdir,fprefix,fext,fdevice)
##
## createdrcplots - Create a full set of eps plots for DRC analysis
##
## ir1 = impulse response 1
## ic1 = center sample of impulse response 1, -1 for automatic search
## it1 = title for impulse response 1 plots
## ir2 = impulse response 2
## ic2 = center sample of impulse response 2, -1 for automatic search
## it2 = title for impulse response 2 plots
## outdir = output directory for graph files
## fprefix = prefix for graph files
## fext = extension for graph files, default '.eps'
## fdevice = print device for graph files, default '-depsc2'
##  
## Along with the device any other option supported by the Octave 
## 'print' command may be specified. For example setting fdevice to:
##
##   '-r300 -dpng'
##
## Sets the PNG output device with a resolution of 300 dpi.
##
## Sample rate of ir1 and ir2 *must* be 44.1 kHz
##

function createdrcplots(ir1,ic1,it1,ir2,ic2,it2,outdir,fprefix,fext = '.eps',fdevice = '-depsc2')
	# Find the impulse center if needed
	if (ic1 < 0)
		[irm1,ic1] = max(abs(hilbert(ir1)));	
	else
		irm1 = max(abs(hilbert(ir1)));	
	endif
	if (ic2 < 0)
		[irm2,ic2] = max(abs(hilbert(ir2)));
	else
		irm2 = max(abs(hilbert(ir2)));	
	endif
	
	# Frequency axis tics
	# gfp = [0.02,0.03,0.05,0.07,0.1,0.15,0.2,0.3,0.5,0.7,1,1.5,2,3,5,7,10,15,20];
	
	# Generate the labels from the tics
	# gfl = num2str(gfp','%.2g');	
	
	# Denser tics, with partial explicit labels
	gfp = [0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.1,0.15,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1,1.5,2,3,4,5,6,7,8,9,10,15,20];
	gfl = [ "0.02";"0.03";" ";"0.05";" ";"0.07";" ";" ";"0.1";"0.15";"0.2";"0.3";" ";"0.5";" ";"0.7";" ";" ";"1";"1.5";"2";"3";" ";"5";" ";"7";" ";" ";"10";"15";"20" ];
	
	# Normalize the impulse responses to the norm
	rms1 = norm(ir1);
	ir1 = ir1(:)/rms1;
	rms2 = norm(ir2);
	ir2 = ir2(:)/rms2;	
	
	# Report the impulse centers
	disp(sprintf("IR1 impulse center: %d, max: %f, RMS: %f",ic1, irm1, rms1));
	disp(sprintf("IR2 impulse center: %d, max: %f, RMS: %f",ic2, irm2, rms2));
	fflush(stdout);

	# Save the sample rate
	fs = 44100;
	
	# Set the default line width
	DLW = 1.5;
	
	# Colormap, line colors and line width for 3d surface plots
	EDLw = 0.25;
	EDCMap = hsv2rgb([linspace(0.50,0.00,256)', 1.0 * ones(256,1), 0.65 * ones(256,1)]);	
	ULColor = [0.85, 0.85, 0.85];
	LLColor = [0.85, 0.85, 0.85];	
	
	# Default font sizes
	GDFs = 8;
	
	# Enable contour display when set to > 0
	surcntenable = 0;
	mapcntenable = 0;
	
	# Report the stage
	disp("Impulse response upsampling...");
	fflush(stdout);

	# Resampling factor
	rf = 40;
	
	# Resampling filter length, cutoff frequency and attenuation
	rfl = 16383;
	rcf = 0.95;
	rat = 160;
	
	# Upsample from about -10 to +30 ms
	irp = 441;
	ira = 882+441;
	
	# Creates the oversampled impulse responses
	ir1os = rf * iupsample(ir1([ic1-irp:ic1+ira]),rf,rfl,rcf,rat);
	ir2os = rf * iupsample(ir2([ic2-irp:ic2+ira]),rf,rfl,rcf,rat);

	# Report the stage
	disp("Step response...");
	fflush(stdout);
	
	# Creates the step response
	ir1step = fftconv(ir1os,ones(1,length(ir1os)))/rf;
	ir2step = fftconv(ir2os,ones(1,length(ir2os)))/rf;

	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Set default font size
	set(0,"defaultaxesfontsize",GDFs)
	
	# Plot the step responses 1
	subplot(2,1,1);
	plot(1000 * ((([0:length(ir1step)-1])/(rf*fs))-((irp/fs) + ((rfl + 1)/(2*rf*fs)))),ir1step,["1;" it1 ";"],"LineWidth",DLW);
	axis([-5, 25]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (normalized)");	
	
	# Save the axis range
	g1ax = axis();
		
	# Plot the step responses 2
	subplot(2,1,2);
	plot(1000 * ((([0:length(ir2step)-1])/(rf*fs))-((irp/fs) + ((rfl + 1)/(2*rf*fs)))),ir2step,["3;" it2 ";"],"LineWidth",DLW);
	axis([-5, 25]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (normalized)");
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	subplot(2,1,2);	
	axis(gax);

	# Print to the output file
	print([ outdir fprefix "-IRStepResponse" fext ],fdevice);
	
	# Report the stage
	disp("Time response, full range...");
	fflush(stdout);
	
	# Open a new plot
	newplot();
	clf();	
	figure(1,"visible","off");
	
	# Plot the oversampled impulse responses 1
	subplot(2,1,1);
	plot(1000 * ((([0:length(ir1os)-1])/(rf*fs))-((irp/fs) + ((rfl + 1)/(2*rf*fs)))),ir1os,["1;" it1 ";"],"LineWidth",DLW);
	axis([-2, 5]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (normalized)");	
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the oversampled impulse responses 2
	subplot(2,1,2);
	plot(1000 * ((([0:length(ir2os)-1])/(rf*fs))-((irp/fs) + ((rfl + 1)/(2*rf*fs)))),ir2os,["3;" it2 ";"],"LineWidth",DLW);
	axis([-2, 5]); #, 1.1 * min(ir2os), 1.1 * max(ir2os)]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (normalized)");
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	subplot(2,1,2);	
	axis(gax);
	
	# Print to the output file
	print([ outdir fprefix "-IRFullRange" fext ],fdevice);

	# Remove variables no longer used
	clear ir1step ir2step;

	# Computes the analytical signal
	ir1osh = abs(hilbert(ir1os));
	ir2osh = abs(hilbert(ir2os));
	
	# Open a new plot
	newplot();
	clf();	
	figure(1,"visible","off");
	
	# Plot the impulse envelope 1
	subplot(2,1,1);
	plot(1000*((([0:length(ir1osh)-1])/(rf*fs))-((irp/fs) + ((rfl + 1)/(2*rf*fs)))),ir1osh,["1;" it1 ";"],"LineWidth",DLW);
	axis([-2, 5]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (normalized)");	
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the impulse envelope 2
	subplot(2,1,2);
	plot(1000*((([0:length(ir2osh)-1])/(rf*fs))-((irp/fs) + ((rfl + 1)/(2*rf*fs)))),ir2osh,["3;" it2 ";"],"LineWidth",DLW);
	axis([-2, 5]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (normalized)");	
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	subplot(2,1,2);	
	axis(gax);
	
	# Print to the output file
	print([ outdir fprefix "-IRFullRangeEnvelope" fext ],fdevice);
		
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the energy-time curve 1
	subplot(2,1,1);
	plot(1000*((([0:length(ir1osh)-1])/(rf*fs))-((irp/fs) + ((rfl + 1)/(2*rf*fs)))),20 * log10(ir1osh),["1;" it1 ";"],"LineWidth",DLW);
	axis([-2, 5, round(-30 + 20 * log10(max(ir1osh))), round(3 + 20 * log10(max(ir1osh)))]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (dB)");	
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the energy-time curve 2
	subplot(2,1,2);
	plot(1000*((([0:length(ir2osh)-1])/(rf*fs))-((irp/fs) + ((rfl + 1)/(2*rf*fs)))),20 * log10(ir2osh),["3;" it2 ";"],"LineWidth",DLW);
	axis([-2, 5, round(-30 + 20 * log10(max(ir2osh))), round(3 + 20 * log10(max(ir2osh)))]);
	axis([-2, 5]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (dB)");	
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	subplot(2,1,2);	
	axis(gax);
	
	# Print to the output file
	print([ outdir fprefix "-IRFullRangeETC" fext ],fdevice);

	# Remove variables no longer used
	clear ir1os ir2os ir1osh ir2osh rfl rcf rat;

	# Report the stage
	disp("Time response, 2 kHz brickwall...");
	fflush(stdout);
	
	# Create the 2 kHz brickwall filter
	fltlen = 4096;
	fltatt = 160;
	flt = lowpassfir(fltlen,2000/(fs/2)) .* chebwin(fltlen,fltatt)';
	fltdelay = (1 + length(flt)) / 2;
	
	# Creates the 2 kHz brickwall filtered impulse responses
	# about -20 to +50 ms
	ir1bw = fftconv(flt,ir1([ic1-882:ic1+2205]));
	ir2bw = fftconv(flt,ir2([ic2-882:ic2+2205]));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the brickwall filtered impulse responses 1
	subplot(2,1,1);
	plot(1000*((([0:length(ir1bw)-1])/fs)-((fltdelay + 882)/fs)),ir1bw,["1;" it1 ";"],"LineWidth",DLW);
	axis([-20, 50]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (normalized)");	
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the brickwall filtered impulse responses 2
	subplot(2,1,2);
	plot(1000*((([0:length(ir2bw)-1])/fs)-((fltdelay + 882)/fs)),ir2bw,["3;" it2 ";"],"LineWidth",DLW);
	axis([-20, 50]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (normalized)");	
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	subplot(2,1,2);	
	axis(gax);
	
	# Print to the output file
	print([ outdir fprefix "-IR2kHzBrickwall" fext ],fdevice);
	
	# Computes the analytical signal
	ir1bwh = abs(hilbert(ir1bw));
	ir2bwh = abs(hilbert(ir2bw));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the impulse envelope 1
	subplot(2,1,1);
	plot(1000*((([0:length(ir1bwh)-1])/fs)-((fltdelay + 882)/fs)),ir1bwh,["1;" it1 ";"],"LineWidth",DLW);
	axis([-20, 50]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (normalized)");
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the impulse envelope 2
	subplot(2,1,2);
	plot(1000*((([0:length(ir2bwh)-1])/fs)-((fltdelay + 882)/fs)),ir2bwh,["3;" it2 ";"],"LineWidth",DLW);
	axis([-20, 50]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (normalized)");	
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	subplot(2,1,2);	
	axis(gax);
	
	# Print to the output file
	print([ outdir fprefix "-IR2kHzBrickwallEnvelope" fext ],fdevice);
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the energy-time curve 1
	subplot(2,1,1);
	plot(1000*((([0:length(ir1bwh)-1])/fs)-((fltdelay + 882)/fs)),20 * log10(ir1bwh),["1;" it1 ";"],"LineWidth",DLW);
	axis([-20, 50, round(-30 + 20 * log10(max(ir1bwh))), round(3 + 20 * log10(max(ir1bwh)))]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (dB)");	
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the energy-time curve 2
	subplot(2,1,2);
	plot(1000*((([0:length(ir2bwh)-1])/fs)-((fltdelay + 882)/fs)),20 * log10(ir2bwh),["3;" it2 ";"],"LineWidth",DLW);
	axis([-20, 50, round(-30 + 20 * log10(max(ir2bwh))), round(3 + 20 * log10(max(ir2bwh)))]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Amplitude (dB)");	
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	subplot(2,1,2);	
	axis(gax);
	
	# Print to the output file
	print([ outdir fprefix "-IR2kHzBrickwallETC" fext ],fdevice);
	
	# Remove variables no longer used
	clear ir1bw ir2bw ir1bwh ir2bwh;

	# Report the stage
	disp("Time response, 200 Hz brickwall...");
	fflush(stdout);
	
	# Create the 200 Hz brickwall filter
	fltlen = 32768;
	fltatt = 160;
	flt = lowpassfir(fltlen,200/(fs/2)) .* chebwin(fltlen,fltatt)';
	fltdelay = (1 + length(flt)) / 2;
	
	# Creates the 200 Hz brickwall filtered impulse responses
	# about -200 to +500 ms
	ir1bw = fftconv(flt,ir1([ic1-8820:ic1+22050]));
	ir2bw = fftconv(flt,ir2([ic2-8820:ic2+22050]));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the brickwall filtered impulse responses 1
	subplot(2,1,1);
	plot(((([0:length(ir1bw)-1])/fs)-((fltdelay + 8820)/fs)),ir1bw,["1;" it1 ";"],"LineWidth",DLW);
	axis([-0.2, 0.5]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Amplitude (normalized)");	
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the brickwall filtered impulse responses 2
	subplot(2,1,2);
	plot(((([0:length(ir2bw)-1])/fs)-((fltdelay + 8820)/fs)),ir2bw,["3;" it2 ";"],"LineWidth",DLW);
	axis([-0.2, 0.5]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Amplitude (normalized)");	
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	subplot(2,1,2);	
	axis(gax);
	
	# Print to the output file
	print([ outdir fprefix "-IR200HzBrickwall" fext ],fdevice);
	
	# Computes the analytical signal
	ir1bwh = abs(hilbert(ir1bw));
	ir2bwh = abs(hilbert(ir2bw));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the impulse envelope 1
	subplot(2,1,1);
	plot(((([0:length(ir1bwh)-1])/fs)-((fltdelay + 8820)/fs)),ir1bwh,["1;" it1 ";"],"LineWidth",DLW);
	axis([-0.2, 0.5]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Amplitude (normalized)");	
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the impulse envelope 2
	subplot(2,1,2);
	plot(((([0:length(ir2bwh)-1])/fs)-((fltdelay + 8820)/fs)),ir2bwh,["3;" it2 ";"],"LineWidth",DLW);
	axis([-0.2, 0.5]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Amplitude (normalized)");	
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	subplot(2,1,2);	
	axis(gax);
	
	# Print to the output file
	print([ outdir fprefix "-IR200HzBrickwallEnvelope" fext ],fdevice);
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the energy-time curve 1
	subplot(2,1,1);
	plot(((([0:length(ir1bwh)-1])/fs)-((fltdelay + 8820)/fs)),20 * log10(ir1bwh),["1;" it1 ";"],"LineWidth",DLW);
	axis([-0.2, 0.5, round(-30 + 20 * log10(max(ir1bwh))), round(3 + 20 * log10(max(ir1bwh)))]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Amplitude (dB)");	
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the energy-time curve 2
	subplot(2,1,2);
	plot(((([0:length(ir2bwh)-1])/fs)-((fltdelay + 8820)/fs)),20 * log10(ir2bwh),["3;" it2 ";"],"LineWidth",DLW);
	axis([-0.2, 0.5, round(-30 + 20 * log10(max(ir2bwh))), round(3 + 20 * log10(max(ir2bwh)))]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Amplitude (dB)");	
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	subplot(2,1,2);	
	axis(gax);
	
	# Print to the output file
	print([ outdir fprefix "-IR200HzBrickwallETC" fext ],fdevice);
	
	# Remove variables no longer used
	clear ir1bw ir2bw ir1bwh ir2bwh;

	# Report the stage
	disp("Unsmoothed frequency response, 1 ms window...");
	fflush(stdout);
	
	# Computes the window, 1 ms
	wind = [ones(1,441) ones(1,22) (blackman(44)(23:44))' zeros(1,8192 - (441 + 44))]';
	
	# Estract the windowed impulse responses
	ir1w = ir1([ic1-441:ic1+(8192 - (441 + 1))]) .* wind;
	ir2w = ir2([ic2-441:ic2+(8192 - (441 + 1))]) .* wind;
	
	# Rotate the impulse response and transform to the frequency domain
	ir1w = fft([ir1w(442:length(ir1w))' ir1w(1:441)']);
	ir2w = fft([ir2w(442:length(ir2w))' ir2w(1:441)']);
	
	# Compute the index limits
	ixl = floor(1000 * length(ir1w) / fs);
	ixu = ceil(20000 * length(ir1w) / fs);
	ixg = ixl:ixu;
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");	
	
	# Plot the magnitude response 1
	subplot(2,1,1);
	semilogx(0.001 * fs * ixg / length(ir1w),20 * log10(abs(ir1w(1 + ixg))),["1;" it1 ";"],"LineWidth",DLW);
	axis([1, 20]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Amplitude (dB)");
	safetics("x",gfp,gfl);
		
	# Save the axis range
	g1ax = axis();
	
	# Plot the magnitude response 2
	subplot(2,1,2);
	semilogx(0.001 * fs * ixg / length(ir2w),20 * log10(abs(ir2w(1 + ixg))),["3;" it2 ";"],"LineWidth",DLW);
	axis([1, 20]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Amplitude (dB)");	
	safetics("x",gfp,gfl);
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set a minimum range of 30 dB
	if ((gax(4) - gax(3)) < 30)
		gax(4) = round(gax(4) + (30 - (gax(4) - gax(3))) / 2);
		gax(3) = gax(4) - 30;
	endif
	
	# Round up to 3 dB steps
	gax(3) = 3 * floor(gax(3) / 3);
	gax(4) = 3 * ceil(gax(4) / 3);
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	subplot(2,1,2);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	
	# Print to the output file
	print([ outdir fprefix "-MRUnsmoothed1ms" fext ],fdevice);

	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the phase response 1
	subplot(2,1,1);
	semilogx(0.001 * fs * ixg / length(ir1w),180 * angle(ir1w(1 + ixg))/pi,["1;" it1 ";"],"LineWidth",DLW);
	axis([1, 20, -180, 180]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Phase (deg)");	
	safetics("x",gfp,gfl);
	safetics("y",-180:30:180);
	
	# Plot the phase response 2
	subplot(2,1,2);
	semilogx(0.001 * fs * ixg / length(ir2w),180 * angle(ir2w(1 + ixg))/pi,["3;" it2 ";"],"LineWidth",DLW);
	axis([1, 20, -180, 180]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Phase (deg)");	
	safetics("x",gfp,gfl);
	safetics("y",-180:30:180);

	# Print to the output file
	print([ outdir fprefix "-PRUnsmoothed1ms" fext ],fdevice);

	# Report the stage
	disp("Unsmoothed frequency response, 5 ms window...");
	fflush(stdout);
	
	# Computes the window, 5 ms
	wind = [ones(1,441) ones(1,110) (blackman(220)(111:220))' zeros(1,8192 - (441 + 220))]';
	
	# Estract the windowed impulse responses
	ir1w = ir1([ic1-441:ic1+(8192 - (441 + 1))]) .* wind;
	ir2w = ir2([ic2-441:ic2+(8192 - (441 + 1))]) .* wind;
	
	# Rotate the impulse response and transform to the frequency domain
	ir1w = fft([ir1w(442:length(ir1w))' ir1w(1:441)']);
	ir2w = fft([ir2w(442:length(ir2w))' ir2w(1:441)']);
	
	# Compute the index limits
	ixl = floor(200 * length(ir1w) / fs);
	ixu = ceil(20000 * length(ir1w) / fs);
	ixg = ixl:ixu;
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the magnitude response 1
	subplot(2,1,1);
	semilogx(0.001 * fs * ixg / length(ir1w),20 * log10(abs(ir1w(1 + ixg))),["1;" it1 ";"],"LineWidth",DLW);
	axis([0.2, 20]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Amplitude (dB)");	
	safetics("x",gfp,gfl);
	safetics("y",-15:3:15);
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the magnitude response 2
	subplot(2,1,2);
	semilogx(0.001 * fs * ixg / length(ir2w),20 * log10(abs(ir2w(1 + ixg))),["3;" it2 ";"],"LineWidth",DLW);
	axis([0.2, 20]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Amplitude (dB)");	
	safetics("x",gfp,gfl);
	safetics("y",-15:3:15);
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set a minimum range of 30 dB
	if ((gax(4) - gax(3)) < 30)
		gax(4) = round(gax(4) + (30 - (gax(4) - gax(3))) / 2);
		gax(3) = gax(4) - 30;
	endif
	
	# Round up to 3 dB steps
	gax(3) = 3 * floor(gax(3) / 3);
	gax(4) = 3 * ceil(gax(4) / 3);
	
	# Set the uniforma axis range
	subplot(2,1,1);		
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	subplot(2,1,2);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	
	# Print to the output file
	print([ outdir fprefix "-MRUnsmoothed5ms" fext ],fdevice);
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the phase response 1
	subplot(2,1,1);
	semilogx(0.001 * fs * ixg / length(ir1w),180 * angle(ir1w(1 + ixg))/pi,["1;" it1 ";"],"LineWidth",DLW);
	axis([0.2, 20, -180, 180]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Phase (deg)");	
	safetics("x",gfp,gfl);
	safetics("y",-180:30:180);
	
	# Plot the phase response 2
	subplot(2,1,2);
	semilogx(0.001 * fs * ixg / length(ir2w),180 * angle(ir2w(1 + ixg))/pi,["3;" it2 ";"],"LineWidth",DLW);
	axis([0.2, 20, -180, 180]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Phase (deg)");	
	safetics("x",gfp,gfl);
	safetics("y",-180:30:180);
	
	# Print to the output file
	print([ outdir fprefix "-PRUnsmoothed5ms" fext ],fdevice);
	
	# Report the stage
	disp("Unsmoothed frequency response, 200 ms window...");
	fflush(stdout);

	# Computes the window, 200 ms
	wind = [ones(1,8820) ones(1,4410) (blackman(8820)(4411:8820))' zeros(1,32768 - (8820 + 8820))]';
	
	# Estract the windowed impulse responses
	ir1w = ir1([ic1-8820:ic1+(32768 - (8820 + 1))]) .* wind;
	ir2w = ir2([ic2-8820:ic2+(32768 - (8820 + 1))]) .* wind;
	
	# Rotate and pad the impulse response and transform to the frequency domain
	ir1w = fft([ir1w(8821:length(ir1w))' zeros(1,131072 - length(ir1w)) ir1w(1:8820)']);
	ir2w = fft([ir2w(8821:length(ir2w))' zeros(1,131072 - length(ir2w)) ir2w(1:8820)']);
	
	# Compute the index limits
	ixl = floor(20 * length(ir1w) / fs);
	ixu = ceil(200 * length(ir1w) / fs);
	ixg = ixl:ixu;
		
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the magnitude response 1
	subplot(2,1,1);
	semilogx(fs * ixg / length(ir1w),20 * log10(abs(ir1w(1 + ixg))),["1;" it1 ";"],"LineWidth",DLW);
	axis([20, 200]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Amplitude (dB)");	
	safetics("x",1000 * gfp,gfl);
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the magnitude response 2
	subplot(2,1,2);
	semilogx(fs * ixg / length(ir2w),20 * log10(abs(ir2w(1 + ixg))),["3;" it2 ";"],"LineWidth",DLW);
	axis([20, 200]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Amplitude (dB)");	
	safetics("x",1000 * gfp,gfl);
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set a minimum range of 30 dB
	if ((gax(4) - gax(3)) < 30)
		gax(4) = round(gax(4) + (30 - (gax(4) - gax(3))) / 2);
		gax(3) = gax(4) - 30;
	endif
	
	# Round up to 3 dB steps
	gax(3) = 3 * floor(gax(3) / 3);
	gax(4) = 3 * ceil(gax(4) / 3);
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	subplot(2,1,2);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	
	# Print to the output file
	print([ outdir fprefix "-MRUnsmoothed200ms" fext ],fdevice);
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the phase response 1
	subplot(2,1,1);
	semilogx(fs * ixg / length(ir1w),180 * angle(ir1w(1 + ixg))/pi,["1;" it1 ";"],"LineWidth",DLW);
	axis([20, 200, -180, 180]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Phase (deg)");	
	safetics("x",1000 * gfp,gfl);
	safetics("y",-180:30:180);
	
	# Plot the phase response 2
	subplot(2,1,2);
	semilogx(fs * ixg / length(ir2w),180 * angle(ir2w(ixg))/pi,["3;" it2 ";"],"LineWidth",DLW);
	axis([20, 200, -180, 180]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Phase (deg)");	
	safetics("x",1000 * gfp,gfl);
	safetics("y",-180:30:180);
	
	# Print to the output file
	print([ outdir fprefix "-PRUnsmoothed200ms" fext ],fdevice);
	
	# Remove variables no longer used
	clear wind ir1w ir2w;

	# Report the stage
	disp("Frequency dependent windowing, magnitude response, 1/3 oct settings...");
	fflush(stdout);
	
	# Estract the windowed impulse responses
	ir1w = ir1(ic1-32767:ic1+32768);
	ir2w = ir2(ic2-32767:ic2+32768);
	
	# Extract the minimum phase impulse response
	[ir1cps, ir1w] = rceps(ir1w);
	[ir2cps, ir2w] = rceps(ir2w);
	clear ir1cps ir2cps;
	
	# Performs the frequency dependant windowing
	ir1w = irslprefilt(ir1w,1,2 * 20/fs,4761,2 * 20000/fs,5,1.0,4096,1.0,5,0);
	ir2w = irslprefilt(ir2w,1,2 * 20/fs,4761,2 * 20000/fs,5,1.0,4096,1.0,5,0);
	
	# Pad the windowed impulse response
	ir1w = [ zeros(1,65536 - length(ir1w)) ir1w(:)' ];
	ir2w = [ zeros(1,65536 - length(ir2w)) ir2w(:)' ];
	
	# Computes the magnitude response
	ir1m = abs(fft(ir1w));
	ir2m = abs(fft(ir2w));
	
	# Compute the index limits
	ixl = floor(20 * length(ir1w) / fs);
	ixu = ceil(20000 * length(ir1w) / fs);
	ixg = ixl:ixu;
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the magnitude response 1
	subplot(2,1,1);
	semilogx(0.001 * fs * ixg / length(ir1w),20 * log10(ir1m(1 + ixg)),["1;" it1 ";"],"LineWidth",DLW);
	axis([0.02, 20]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Magnitude (dB)");	
	safetics("x",gfp,gfl);
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the magnitude response 2
	subplot(2,1,2);
	semilogx(0.001 * fs * ixg / length(ir2w),20 * log10(ir2m(1 + ixg)),["3;" it2 ";"],"LineWidth",DLW);
	axis([0.02, 20]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Magnitude (dB)");	
	safetics("x",gfp,gfl);
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set a minimum range of 30 dB
	if ((gax(4) - gax(3)) < 30)
		gax(4) = round(gax(4) + (30 - (gax(4) - gax(3))) / 2);
		gax(3) = gax(4) - 30;
	endif
	
	# Round up to 3 dB steps
	gax(3) = 3 * floor(gax(3) / 3);
	gax(4) = 3 * ceil(gax(4) / 3);
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	subplot(2,1,2);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));

	# Print to the output file
	print([ outdir fprefix "-MRFDWSmoothed-1-3" fext ],fdevice);

	# Report the stage
	disp("Frequency dependent windowing, phase response, 1/3 oct settings...");
	fflush(stdout);
	
	# Estract the windowed impulse responses
	ir1w = ir1([ic1-4096:ic1+32767]);
	ir2w = ir2([ic2-4096:ic2+32767]);
	
	# Performs the frequency dependant windowing
	ir1w = irslprefilt(ir1w,4097,2 * 20/fs,4761,2 * 20000/fs,5,1.0,4096,1.0,5,4096);
	ir2w = irslprefilt(ir2w,4097,2 * 20/fs,4761,2 * 20000/fs,5,1.0,4096,1.0,5,4096);
	
	# Rotate the impulse response and transform to the frequency domain
	ir1w = fft([ir1w(4097:length(ir1w)) ir1w(1:4096)]);
	ir2w = fft([ir2w(4097:length(ir2w)) ir2w(1:4096)]);
	
	# Compute the index limits
	ixl = floor(20 * length(ir1w) / fs);
	ixu = ceil(20000 * length(ir1w) / fs);
	ixg = ixl:ixu;
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the phase response 1
	subplot(2,1,1);
	semilogx(0.001 * fs * ixg / length(ir1w),180 * angle(ir1w(1 + ixg))/pi,["1;" it1 ";"],"LineWidth",DLW);
	axis([0.02, 20, -180, 180]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Phase (deg)");	
	safetics("x",gfp,gfl);
	safetics("y",-180:30:180);
	
	# Plot the phase response 2
	subplot(2,1,2);
	semilogx(0.001 * fs * ixg / length(ir2w),180 * angle(ir2w(ixg))/pi,["3;" it2 ";"],"LineWidth",DLW);
	axis([0.02, 20, -180, 180]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Phase (deg)");	
	safetics("x",gfp,gfl);
	safetics("y",-180:30:180);
	
	# Print to the output file
	print([ outdir fprefix "-PRFDWSmoothed-1-3" fext ],fdevice);

	# Report the stage
	disp("Frequency dependent windowing, magnitude response, 1/6 oct settings...");
	fflush(stdout);
	
	# Estract the windowed impulse responses
	ir1w = ir1(ic1-32767:ic1+32768);
	ir2w = ir2(ic2-32767:ic2+32768);
	
	# Extract the minimum phase impulse response
	[ir1cps, ir1w] = rceps(ir1w);
	[ir2cps, ir2w] = rceps(ir2w);
	clear ir1cps ir2cps;
	
	# Performs the frequency dependant windowing
	ir1w = irslprefilt(ir1w,1,2 * 20/fs,9538,2 * 20000/fs,10,1.0,4096,1.0,10,0);
	ir2w = irslprefilt(ir2w,1,2 * 20/fs,9538,2 * 20000/fs,10,1.0,4096,1.0,10,0);
	
	# Pad the windowed impulse response
	ir1w = [ zeros(1,65536 - length(ir1w)) ir1w(:)' ];
	ir2w = [ zeros(1,65536 - length(ir2w)) ir2w(:)' ];
	
	# Computes the magnitude response
	ir1m = abs(fft(ir1w));
	ir2m = abs(fft(ir2w));
	
	# Compute the index limits
	ixl = floor(20 * length(ir1w) / fs);
	ixu = ceil(20000 * length(ir1w) / fs);
	ixg = ixl:ixu;
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the magnitude response 1
	subplot(2,1,1);
	semilogx(0.001 * fs * ixg / length(ir1w),20 * log10(ir1m(1 + ixg)),["1;" it1 ";"],"LineWidth",DLW);
	axis([0.02, 20]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Magnitude (dB)");	
	safetics("x",gfp,gfl);
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the magnitude response 2
	subplot(2,1,2);
	semilogx(0.001 * fs * ixg / length(ir2w),20 * log10(ir2m(1 + ixg)),["3;" it2 ";"],"LineWidth",DLW);
	axis([0.02, 20]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Magnitude (dB)");	
	safetics("x",gfp,gfl);
	
	# Save the axis range
	g2ax = axis();
	
	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set a minimum range of 30 dB
	if ((gax(4) - gax(3)) < 30)
		gax(4) = round(gax(4) + (30 - (gax(4) - gax(3))) / 2);
		gax(3) = gax(4) - 30;
	endif
	
	# Round up to 3 dB steps
	gax(3) = 3 * floor(gax(3) / 3);
	gax(4) = 3 * ceil(gax(4) / 3);
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	subplot(2,1,2);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	
	# Print to the output file
	print([ outdir fprefix "-MRFDWSmoothed-1-6" fext ],fdevice);

	# Report the stage
	disp("Frequency dependent windowing, phase response, 1/6 oct settings...");
	fflush(stdout);
	
	# Estract the windowed impulse responses
	ir1w = ir1([ic1-4096:ic1+32767]);
	ir2w = ir2([ic2-4096:ic2+32767]);
	
	# Performs the frequency dependant windowing
	ir1w = irslprefilt(ir1w,4097,2 * 20/fs,9538,2 * 20000/fs,10,1.0,4096,1.0,10,4096);
	ir2w = irslprefilt(ir2w,4097,2 * 20/fs,9538,2 * 20000/fs,10,1.0,4096,1.0,10,4096);
	
	# Rotate the impulse response and transform to the frequency domain
	ir1w = fft([ir1w(4097:length(ir1w)) ir1w(1:4096)]);
	ir2w = fft([ir2w(4097:length(ir2w)) ir2w(1:4096)]);
	
	# Compute the index limits
	ixl = floor(20 * length(ir1w) / fs);
	ixu = ceil(20000 * length(ir1w) / fs);
	ixg = ixl:ixu;
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the phase response 1
	subplot(2,1,1);
	semilogx(0.001 * fs * ixg / length(ir1w),180 * angle(ir1w(1 + ixg))/pi,["1;" it1 ";"],"LineWidth",DLW);
	axis([0.02, 20, -180, 180]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Phase (deg)");	
	safetics("x",gfp,gfl);
	safetics("y",-180:30:180);
	
	# Plot the phase response 2
	subplot(2,1,2);
	semilogx(0.001 * fs * ixg / length(ir2w),180 * angle(ir2w(1 + ixg))/pi,["3;" it2 ";"],"LineWidth",DLW);
	axis([0.02, 20, -180, 180]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Phase (deg)");	
	safetics("x",gfp,gfl);
	safetics("y",-180:30:180);

	# Print to the output file
	print([ outdir fprefix "-PRFDWSmoothed-1-6" fext ],fdevice);

	# Report the stage
	disp("Frequency dependent windowing, magnitude response, normal.drc settings...");
	fflush(stdout);
	
	# Estract the windowed impulse responses
	ir1w = ir1(ic1-32767:ic1+32768);
	ir2w = ir2(ic2-32767:ic2+32768);
	
	# Extract the minimum phase impulse response
	[ir1cps, ir1w] = rceps(ir1w);
	[ir2cps, ir2w] = rceps(ir2w);
	clear ir1cps ir2cps;
	
	# Performs the frequency dependant windowing
	ir1w = irslprefilt(ir1w,1,2 * 20/fs,22050,2 * 20000/fs,22,1.0,4096,0.25,22,0);
	ir2w = irslprefilt(ir2w,1,2 * 20/fs,22050,2 * 20000/fs,22,1.0,4096,0.25,22,0);
	
	# Pad the windowed impulse response
	ir1w = [ zeros(1,65536 - length(ir1w)) ir1w(:)' ];
	ir2w = [ zeros(1,65536 - length(ir2w)) ir2w(:)' ];
	
	# Computes the magnitude response
	ir1m = abs(fft(ir1w));
	ir2m = abs(fft(ir2w));
	
	# Compute the index limits
	ixl = floor(20 * length(ir1w) / fs);
	ixu = ceil(20000 * length(ir1w) / fs);
	ixg = ixl:ixu;
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the magnitude response 1
	subplot(2,1,1);
	semilogx(0.001 * fs * ixg / length(ir1w),20 * log10(ir1m(1 + ixg)),["1;" it1 ";"],"LineWidth",DLW);
	axis([0.02, 20]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Magnitude (dB)");	
	safetics("x",gfp,gfl);
	
	# Save the axis range
	g1ax = axis();
	
	# Plot the magnitude response 2
	subplot(2,1,2);
	semilogx(0.001 * fs * ixg / length(ir2w),20 * log10(ir2m(1 + ixg)),["3;" it2 ";"],"LineWidth",DLW);
	axis([0.02, 20]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Magnitude (dB)");	
	safetics("x",gfp,gfl);
	
	# Save the axis range
	g2ax = axis();

	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set a minimum range of 30 dB
	if ((gax(4) - gax(3)) < 30)
		gax(4) = round(gax(4) + (30 - (gax(4) - gax(3))) / 2);
		gax(3) = gax(4) - 30;
	endif
	
	# Round up to 3 dB steps
	gax(3) = 3 * floor(gax(3) / 3);
	gax(4) = 3 * ceil(gax(4) / 3);
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	subplot(2,1,2);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	
	# Print to the output file
	print([ outdir fprefix "-MRFDWSmoothed" fext ],fdevice);

	# Report the stage
	disp("Frequency dependent windowing, phase response, normal.drc settings...");
	fflush(stdout);
	
	# Estract the windowed impulse responses
	ir1w = ir1([ic1-4096:ic1+32767]);
	ir2w = ir2([ic2-4096:ic2+32767]);
	
	# Performs the frequency dependant windowing
	ir1w = irslprefilt(ir1w,4097,2 * 20/fs,22050,2 * 20000/fs,22,1.0,4096,0.25,22,4096);
	ir2w = irslprefilt(ir2w,4097,2 * 20/fs,22050,2 * 20000/fs,22,1.0,4096,0.25,22,4096);
	
	# Rotate the impulse response and transform to the frequency domain
	ir1w = fft([ir1w(4097:length(ir1w)) ir1w(1:4096)]);
	ir2w = fft([ir2w(4097:length(ir2w)) ir2w(1:4096)]);
	
	# Compute the index limits
	ixl = floor(20 * length(ir1w) / fs);
	ixu = ceil(20000 * length(ir1w) / fs);
	ixg = ixl:ixu;
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the phase response 1
	subplot(2,1,1);
	semilogx(0.001 * fs * ixg / length(ir1w),180 * angle(ir1w(1 + ixg))/pi,["1;" it1 ";"],"LineWidth",DLW);
	axis([0.02, 20, -180, 180]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Phase (deg)");	
	safetics("x",gfp,gfl);
	safetics("y",-180:30:180);
	
	# Plot the phase response 2
	subplot(2,1,2);
	semilogx(0.001 * fs * ixg / length(ir2w),180 * angle(ir2w(1 + ixg))/pi,["3;" it2 ";"],"LineWidth",DLW);
	axis([0.02, 20, -180, 180]);
	grid("on");
	xlabel("Frequency (kHz)");
	ylabel("Phase (deg)");	
	safetics("x",gfp,gfl);
	safetics("y",-180:30:180);
	
	# Print to the output file
	print([ outdir fprefix "-PRFDWSmoothed" fext ],fdevice);

	# Report the stage
	disp("Bark smoothed magnitude response...");
	fflush(stdout);
	
	# Windows array, in ms
	wlst = [ 200 100 50 20 10 5 2 1 ];
	
	# Graph resolution
	gres = 16384;
	
	# Graph pregap
	gpgp = 441;
	
	# Scaling base (dB)
	scbs = 18;
	
	# Scaling delta (dB)
	scdt = -3;
	
	# Compute the index limits
	ixl = floor(2 * 20 * gres / fs);
	ixu = ceil(2 * 20000 * gres / fs);
		
	# Open a new plot	
	newplot();
	clf();
	figure(1,"visible","off");	
	
	# Enable holding the plot
	subplot(2,1,1);
	hold("on");
	subplot(2,1,2);
	hold("on");
	
	# Cycle on the windows sizes
	cscl = scbs;
	cg = 1;
	for cws = wlst
		# Computes the number of samples for the current window
		wns = round(fs * cws / (2 * 1000));
		
		# Creates the response prewindow
		wind = [ones(1,gpgp) ones(1,wns) (blackman(2 * wns)((wns + 1):(2 * wns)))' zeros(1,gres - (gpgp + 2 * wns))]';
		
		# Extract the windowed impulse responses
		ir1w = ir1([(ic1 - gpgp):(ic1 + (gres - (gpgp + 1)))]) .* wind;
		ir2w = ir2([(ic2 - gpgp):(ic2 + (gres - (gpgp + 1)))]) .* wind;
		
		# Computes the smoothed magnitude response
		ir1m = ssev(ir1w,fs,-1,1);
		ir2m = ssev(ir2w,fs,-1,1);
		
		# Computes the graph lower bounds
		glb = fs / (4 * wns);
		glb = round(glb / (fs / (2 * gres)));
		glb = max(glb,2);
		
		# Computes the graph index
		ixg = max(ixl,glb):ixu;
				
		# Plot the magnitude response
		subplot(2,1,1);
		semilogx(0.001 * 0.5 * fs * ixg / gres,cscl + 20 * log10(ir1m(1 + ixg)),["1;" it1 sprintf(" (%.1f ms, %+.1f dB)",cws,cscl) ";"],"color",hsv2rgb([0, 1, 0.5 + 0.5 * cg / length(wlst)]),"LineWidth",DLW);
		
		subplot(2,1,2);
		semilogx(0.001 * 0.5 * fs * ixg / gres,cscl + 20 * log10(ir2m(1 + ixg)),["3;" it2 sprintf(" (%.1f ms, %+.1f dB)",cws,cscl) ";"],"color",hsv2rgb([2/3, 1, 0.5 + 0.5 * cg / length(wlst)]),"LineWidth",DLW);
		
		# Adjust the scaling
		cscl = cscl + scdt;
		cg = cg + 1;
	endfor	
	
	# Disable holding the plot and set graph parameters
	subplot(2,1,1);
	hold("off");
	safetics("x",gfp,gfl);
	xlabel("Frequency (kHz)");
	ylabel("Magnitude (dB)");
	grid("on");
	legend("location","southwest");

	subplot(2,1,2);
	hold("off");
	safetics("x",gfp,gfl);
	xlabel("Frequency (kHz)");
	ylabel("Magnitude (dB)");
	grid("on");
	legend("location","southwest");
	
	# Save the axis range
	subplot(2,1,1);
	axis([0.02, 20]);	
	g1ax = axis();
	subplot(2,1,2);
	axis([0.02, 20]);	
	g2ax = axis();

	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set a minimum range of 30 dB
	if ((gax(4) - gax(3)) < 30)
		gax(4) = round(gax(4) + (30 - (gax(4) - gax(3))) / 2);
		gax(3) = gax(4) - 30;
	endif
	
	# Round up to 3 dB steps
	gax(3) = 3 * floor(gax(3) / 3);
	gax(4) = 3 * ceil(gax(4) / 3);
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	subplot(2,1,2);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	
	# Print to the output file
	print([ outdir fprefix "-MRBarkSmoothed" fext ],fdevice);

	# Report the stage
	disp("ERB smoothed magnitude response...");
	fflush(stdout);
	
	# Windows array, in ms
	wlst = [ 200 100 50 20 10 5 2 1 ];
	
	# Graph resolution
	gres = 16384;
	
	# Graph pregap
	gpgp = 441;
	
	# Scaling base (dB)
	scbs = 18;
	
	# Scaling delta (dB)
	scdt = -3;
	
	# Compute the index limits
	ixl = floor(2 * 20 * gres / fs);
	ixu = ceil(2 * 20000 * gres / fs);
		
	# Open a new plot	
	newplot();
	clf();
	figure(1,"visible","off");	
	
	# Enable holding the plot
	subplot(2,1,1);
	hold("on");
	subplot(2,1,2);
	hold("on");
	
	# Cycle on the windows sizes
	cscl = scbs;
	cg = 1;
	for cws = wlst
		# Computes the number of samples for the current window
		wns = round(fs * cws / (2 * 1000));
		
		# Creates the response prewindow
		wind = [ones(1,gpgp) ones(1,wns) (blackman(2 * wns)((wns + 1):(2 * wns)))' zeros(1,gres - (gpgp + 2 * wns))]';
		
		# Extract the windowed impulse responses
		ir1w = ir1([(ic1 - gpgp):(ic1 + (gres - (gpgp + 1)))]) .* wind;
		ir2w = ir2([(ic2 - gpgp):(ic2 + (gres - (gpgp + 1)))]) .* wind;
		
		# Computes the smoothed magnitude response
		ir1m = ssev(ir1w,fs,-2,1);
		ir2m = ssev(ir2w,fs,-2,1);
		
		# Computes the graph lower bounds
		glb = fs / (4 * wns);
		glb = round(glb / (fs / (2 * gres)));
		glb = max(glb,2);
		
		# Computes the graph index
		ixg = max(ixl,glb):ixu;
				
		# Plot the magnitude response
		subplot(2,1,1);
		semilogx(0.001 * 0.5 * fs * ixg / gres,cscl + 20 * log10(ir1m(1 + ixg)),["1;" it1 sprintf(" (%.1f ms, %+.1f dB)",cws,cscl) ";"],"color",hsv2rgb([0, 1, 0.5 + 0.5 * cg / length(wlst)]),"LineWidth",DLW);
		
		subplot(2,1,2);
		semilogx(0.001 * 0.5 * fs * ixg / gres,cscl + 20 * log10(ir2m(1 + ixg)),["3;" it2 sprintf(" (%.1f ms, %+.1f dB)",cws,cscl) ";"],"color",hsv2rgb([2/3, 1, 0.5 + 0.5 * cg / length(wlst)]),"LineWidth",DLW);
		
		# Adjust the scaling
		cscl = cscl + scdt;
		cg = cg + 1;
	endfor	
	
	# Disable holding the plot and set graph parameters
	subplot(2,1,1);
	hold("off");
	safetics("x",gfp,gfl);
	xlabel("Frequency (kHz)");
	ylabel("Magnitude (dB)");
	grid("on");
	legend("location","southwest");

	subplot(2,1,2);
	hold("off");
	safetics("x",gfp,gfl);
	xlabel("Frequency (kHz)");
	ylabel("Magnitude (dB)");
	grid("on");
	legend("location","southwest");
	
	# Save the axis range
	subplot(2,1,1);
	axis([0.02, 20]);	
	g1ax = axis();
	subplot(2,1,2);
	axis([0.02, 20]);	
	g2ax = axis();

	# Computes the uniform axis range
	gax = g1ax;
	gax(3) = min(g1ax(3),g2ax(3));
	gax(4) = max(g1ax(4),g2ax(4));
	
	# Set a minimum range of 30 dB
	if ((gax(4) - gax(3)) < 30)
		gax(4) = round(gax(4) + (30 - (gax(4) - gax(3))) / 2);
		gax(3) = gax(4) - 30;
	endif
	
	# Round up to 3 dB steps
	gax(3) = 3 * floor(gax(3) / 3);
	gax(4) = 3 * ceil(gax(4) / 3);
	
	# Set the uniforma axis range
	subplot(2,1,1);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	subplot(2,1,2);	
	axis(gax);
	safetics("y",gax(3):3:gax(4));
	
	# Print to the output file
	print([ outdir fprefix "-MRERBSmoothed" fext ],fdevice);

	# Remove variables no longer used
	clear wind ir1w ir1m ir2w ir2m ixl ixu ixg g1ax g2ax gax;

	# Report the stage
	disp("Spectral decay and formation, high range...");
	fflush(stdout);

	# Spectrogram from -1 to +2 ms
	tbs = 1;
	tbe = 2;
	sbs = 44;
	sas = 88;
	sfs = 64;
	sol = 1;
	
	# 0.5 ms spectrogram window
	swl = 22;
	
	# Computes the spectrogram window, 
	wind = [blackman(swl)' zeros(1,sfs-swl)]';
	
	# Computes the spectrograms
	[S1, f1, t1] = specgram(ir1((ic1 - (-1 + sbs + swl/2)):(ic1 + (sas + sfs))),sfs,fs,wind,sfs - sol);
	[S2, f2, t2] = specgram(ir2((ic2 - (-1 + sbs + swl/2)):(ic2 + (sas + sfs))),sfs,fs,wind,sfs - sol);
	
	# Normalize and truncates the spectral decay
	S1 = abs(S1);
	S1 = S1/max(S1(:));
	S1 = 20 * log10(max(S1,10^(-30/20)));
	
	S2 = abs(S2);
	S2 = S2/max(S2(:));
	S2 = 20 * log10(max(S2,10^(-30/20)));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectral decay 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([-tbs, tbe, 2, 20, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(15, 45);
	colormap(EDCMap);
	title(it1);
		
	# Plot the spectral decay 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([-tbs, tbe, 2, 20, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(15, 45);
	colormap(EDCMap);
	title(it2);	
		
	# Print to the output file
	print([ outdir fprefix "-SDHighRange" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");

	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectral decay 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([-tbs, tbe, 2, 20, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(345, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the spectral decay 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([-tbs, tbe, 2, 20, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(345, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-SFHighRange" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");

	# Report the stage
	disp("Spectral decay and formation, mid range...");
	fflush(stdout);
	
	# Spectrogram from -10 to +20 ms
	tbs = 10;
	tbe = 20;
	sbs = 441;
	sas = 882;
	sfs = 1024;
	sol = 10;
	
	# 5 ms spectrogram window
	swl = 220;
	
	# Computes the spectrogram window
	wind = [blackman(swl)' zeros(1,sfs-swl)]';
	
	# Computes the spectrograms
	[S1, f1, t1] = specgram(ir1((ic1 - (-1 + sbs + swl/2)):(ic1 + (sas + sfs))),sfs,fs,wind,sfs - sol);
	[S2, f2, t2] = specgram(ir2((ic2 - (-1 + sbs + swl/2)):(ic2 + (sas + sfs))),sfs,fs,wind,sfs - sol);
	
	# Normalize and truncates the spectral decay
	S1 = abs(S1);
	S1 = S1/max(S1(:));
	S1 = 20 * log10(max(S1,10^(-30/20)));
	
	S2 = abs(S2);
	S2 = S2/max(S2(:));
	S2 = 20 * log10(max(S2,10^(-30/20)));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectral decay 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([-tbs, tbe, 0.2, 2, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(15, 45);
	colormap(EDCMap);
	title(it1);
		
	# Plot the spectral decay 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([-tbs, tbe, 0.2, 2, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(15, 45);
	colormap(EDCMap);
	title(it2);	
		
	# Print to the output file
	print([ outdir fprefix "-SDMidRange" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectral decay 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([-tbs, tbe, 0.2, 2, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(345, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the spectral decay 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([-tbs, tbe, 0.2, 2, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(345, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-SFMidRange" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");

	# Report the stage
	disp("Spectral decay and formation, bass range...");
	fflush(stdout);
	
	# Spectrogram from -0.10 to +0.20 s
	tbs = 0.10;
	tbe = 0.20;
	sbs = 4410;
	sas = 8820;
	sfs = 1024;
	sol = 10;
	
	# 50 ms spctrogram window
	swl = 220;
	
	# Downsample the impulse response 10 times to speed up the spectrogram computation
	dsdf = 10;
	dsfs = fs/dsdf;
	dsfl = 16383;
	dsfd = (dsfl + 1) / (2 * fs); 
	ir1ds = idownsample(ir1(ic1 - (-10 + sbs + swl * 5):ic1 + (sas + sfs * 10)),dsdf,dsfl,0.5,160);
	ir2ds = idownsample(ir2(ic2 - (-10 + sbs + swl * 5):ic2 + (sas + sfs * 10)),dsdf,dsfl,0.5,160);
	
	# Computes the spectrogram window, 
	wind = [blackman(swl)' zeros(1,sfs-swl)]';
	
	# Computes the spectrograms
	[S1, f1, t1] = specgram(ir1ds,sfs,dsfs,wind,sfs - sol);
	[S2, f2, t2] = specgram(ir2ds,sfs,dsfs,wind,sfs - sol);
	
	# Normalize and truncates the spectral decay
	S1 = abs(S1);
	S1 = S1/max(S1(:));
	S1 = 20 * log10(max(S1,10^(-30/20)));
	
	S2 = abs(S2);
	S2 = S2/max(S2(:));
	S2 = 20 * log10(max(S2,10^(-30/20)));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectral decay 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(t1 - (dsfd + sbs/fs),f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(t1 - (dsfd + sbs/fs),f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([-tbs, tbe, 20, 200, -33, 1]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Frequency (kHz)");
	zlabel("Mag. (dB)");	
	safetics("y",1000 * gfp,gfl);
	safetics("z",-30:6:0);
	view(15, 45);
	colormap(EDCMap);
	title(it1);
		
	# Plot the spectral decay 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(t2 - (dsfd + sbs/fs),f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(t2 - (dsfd + sbs/fs),f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([-tbs, tbe, 20, 200, -33, 1]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",1000 * gfp,gfl);
	safetics("z",-30:6:0);
	view(15, 45);
	colormap(EDCMap);
	title(it2);	
		
	# Print to the output file
	print([ outdir fprefix "-SDBassRange" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectral decay 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(t1 - (dsfd + sbs/fs),f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(t1 - (dsfd + sbs/fs),f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([-tbs, tbe, 20, 200, -33, 1]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",1000 * gfp,gfl);
	safetics("z",-30:6:0);
	view(345, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the spectral decay 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(t2 - (dsfd + sbs/fs),f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(t2 - (dsfd + sbs/fs),f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([-tbs, tbe, 20, 200, -33, 1]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",1000 * gfp,gfl);
	safetics("z",-30:6:0);
	view(345, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-SFBassRange" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");

	# Report the stage
	disp("Spectral decay and formation, high range, wider window...");
	fflush(stdout);
	
	# Spectrogram from -2 to +4 ms
	tbs = 2;
	tbe = 4;
	sbs = 88;
	sas = 176;
	sfs = 64;
	sol = 1;
	
	# 1 ms spectrogram window
	swl = 44;
	
	# Computes the spectrogram window, 
	wind = [blackman(swl)' zeros(1,sfs-swl)]';
	
	# Computes the spectrograms
	[S1, f1, t1] = specgram(ir1((ic1 - (-1 + sbs + swl/2)):(ic1 + (sas + sfs))),sfs,fs,wind,sfs - sol);
	[S2, f2, t2] = specgram(ir2((ic2 - (-1 + sbs + swl/2)):(ic2 + (sas + sfs))),sfs,fs,wind,sfs - sol);
	
	# Normalize and truncates the spectral decay
	S1 = abs(S1);
	S1 = S1/max(S1(:));
	S1 = 20 * log10(max(S1,10^(-30/20)));
	
	S2 = abs(S2);
	S2 = S2/max(S2(:));
	S2 = 20 * log10(max(S2,10^(-30/20)));

	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectral decay 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([-tbs, tbe, 2, 20, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(15, 45);
	colormap(EDCMap);
	title(it1);
		
	# Plot the spectral decay 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([-tbs, tbe, 2, 20, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(15, 45);
	colormap(EDCMap);
	title(it2);	
		
	# Print to the output file
	print([ outdir fprefix "-SDHighRangeW" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectral decay 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([-tbs, tbe, 2, 20, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(345, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the spectral decay 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([-tbs, tbe, 2, 20, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(345, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-SFHighRangeW" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");

	# Report the stage
	disp("Spectral decay and formation, mid range, wider window...");
	fflush(stdout);
	
	# Spectrogram from -20 to +40 ms
	tbs = 20;
	tbe = 40;
	sbs = 882;
	sas = 1764;
	sfs = 1024;
	sol = 10;
	
	# 10 ms spectrogram window
	swl = 440;
	
	# Computes the spectrogram window
	wind = [blackman(swl)' zeros(1,sfs-swl)]';
	
	# Computes the spectrograms
	[S1, f1, t1] = specgram(ir1((ic1 - (-1 + sbs + swl/2)):(ic1 + (sas + sfs))),sfs,fs,wind,sfs - sol);
	[S2, f2, t2] = specgram(ir2((ic2 - (-1 + sbs + swl/2)):(ic2 + (sas + sfs))),sfs,fs,wind,sfs - sol);
	
	# Normalize and truncates the spectral decay
	S1 = abs(S1);
	S1 = S1/max(S1(:));
	S1 = 20 * log10(max(S1,10^(-30/20)));
	
	S2 = abs(S2);
	S2 = S2/max(S2(:));
	S2 = 20 * log10(max(S2,10^(-30/20)));

	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectral decay 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([-tbs, tbe, 0.2, 2, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(15, 45);
	colormap(EDCMap);
	title(it1);
		
	# Plot the spectral decay 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([-tbs, tbe, 0.2, 2, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(15, 45);
	colormap(EDCMap);
	title(it2);	
		
	# Print to the output file
	print([ outdir fprefix "-SDMidRangeW" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectral decay 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t1 - (sbs/fs)),0.001 * f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([-tbs, tbe, 0.2, 2, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(345, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the spectral decay 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(1000 * (t2 - (sbs/fs)),0.001 * f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([-tbs, tbe, 0.2, 2, -33, 1]);
	grid("on");
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",gfp,gfl);
	safetics("z",-30:6:0);
	view(345, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-SFMidRangeW" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");

	# Report the stage
	disp("Spectral decay and formation, bass range, wider window...");
	fflush(stdout);
	
	# Spectrogram from -0.20 to +0.40 s
	tbs = 0.20;
	tbe = 0.40;
	sbs = 8820;
	sas = 17640;
	sfs = 1024;
	sol = 20;
	
	# 100 ms spctrogram window
	swl = 440;
	
	# Downsample the impulse response 10 times to speed up the spectrogram computation
	dsdf = 10;
	dsfs = fs/dsdf;
	dsfl = 16383;
	dsfd = (dsfl + 1) / (2 * fs); 
	ir1ds = idownsample(ir1(ic1 - (-10 + sbs + swl * 5):ic1 + (sas + sfs * 10)),dsdf,dsfl,0.5,160);
	ir2ds = idownsample(ir2(ic2 - (-10 + sbs + swl * 5):ic2 + (sas + sfs * 10)),dsdf,dsfl,0.5,160);
	
	# Computes the spectrogram window, 
	wind = [blackman(swl)' zeros(1,sfs-swl)]';
	
	# Computes the spectrograms
	[S1, f1, t1] = specgram(ir1ds,sfs,dsfs,wind,sfs - sol);
	[S2, f2, t2] = specgram(ir2ds,sfs,dsfs,wind,sfs - sol);
	
	# Normalize and truncates the spectral decay
	S1 = abs(S1);
	S1 = S1/max(S1(:));
	S1 = 20 * log10(max(S1,10^(-30/20)));
	
	S2 = abs(S2);
	S2 = S2/max(S2(:));
	S2 = 20 * log10(max(S2,10^(-30/20)));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectral decay 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(t1 - (dsfd + sbs/fs),f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(t1 - (dsfd + sbs/fs),f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([-tbs, tbe, 20, 200, -33, 1]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Frequency (kHz)");
	zlabel("Mag. (dB)");	
	safetics("y",1000 * gfp,gfl);
	safetics("z",-30:6:0);
	view(15, 45);
	colormap(EDCMap);
	title(it1);
		
	# Plot the spectral decay 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(t2 - (dsfd + sbs/fs),f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(t2 - (dsfd + sbs/fs),f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([-tbs, tbe, 20, 200, -33, 1]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",1000 * gfp,gfl);
	safetics("z",-30:6:0);
	view(15, 45);
	colormap(EDCMap);
	title(it2);	
		
	# Print to the output file
	print([ outdir fprefix "-SDBassRangeW" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectral decay 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(t1 - (dsfd + sbs/fs),f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(t1 - (dsfd + sbs/fs),f1,S1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([-tbs, tbe, 20, 200, -33, 1]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",1000 * gfp,gfl);
	safetics("z",-30:6:0);
	view(345, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the spectral decay 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(t2 - (dsfd + sbs/fs),f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(t2 - (dsfd + sbs/fs),f2,S2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([-tbs, tbe, 20, 200, -33, 1]);
	grid("on");
	xlabel("Time (s)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	safetics("y",1000 * gfp,gfl);
	safetics("z",-30:6:0);
	view(345, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-SFBassRangeW" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");

	# Remove variables no longer used
	clear dsdf dsfs dsfl dsfd;
	clear wind S1 f1 t1 S2 f2 t2 ir1ds ir2ds sbs sas sfs sol swl;

	# Report the stage
	disp("Spectrogram, 1 ms window...");
	fflush(stdout);
	
	# Spectrogram from -10 to +40 ms
	tbs = 10;
	tbe = 40;	
	sbs = 441;
	sas = 1764;
	sfs = 512;
	sol = 4;
	mlb = -90;
	
	# 1 ms spectrogram window
	swl = 44;
	
	# Spaziatura tics asse x e y
	xtsp = 2.5;
	ytsp = 2;
	
	# Computes the spectrogram window
	wind = [blackman(swl)' zeros(1,sfs-swl)]';
	
	# Computes the spectrograms
	[S1, f1, t1] = specgram(ir1((ic1 - (-1 + sbs + swl/2)):(ic1 + (sas + sfs))),sfs,fs,wind,sfs - sol);
	[S2, f2, t2] = specgram(ir2((ic2 - (-1 + sbs + swl/2)):(ic2 + (sas + sfs))),sfs,fs,wind,sfs - sol);
	
	# Normalize the spectrogram
	S1 = abs(S1);
	S1 = S1/max(S1(:));
	S1 = 20 * log10(max(S1,10^(mlb/20)));
	
	S2 = abs(S2);
	S2 = S2/max(S2(:));
	S2 = 20 * log10(max(S2,10^(mlb/20)));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectrogram 1
	subplot(2,1,1);	
	imagesc(1000 * (t1 - (sbs/fs)),0.001 * f1,S1);
	set(gca(),"ydir","normal");
	axis([-tbs, tbe, 0, fs / 2000]);
	grid("on");	
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	text(1.1 * tbe,fs / 4000,"Mag. (dB)","Rotation",90,"HorizontalAlignment","center","fontsize",GDFs);
	safetics("x",-tbs:xtsp:tbe);
	safetics("y",0:ytsp:(fs / 2000));
	colormap(EDCMap);	
	colorbar("EastOutside");
	title(it1);
		
	# Plot the spectrogram 2
	subplot(2,1,2);	
	imagesc(1000 * (t2 - (sbs/fs)),0.001 * f2,S2);
	set(gca(),"ydir","normal");
	axis([-tbs, tbe, 0, fs / 2000]);
	grid("on");	
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	safetics("x",-tbs:xtsp:tbe);
	safetics("y",0:ytsp:(fs / 2000));
	text(1.1 * tbe,fs / 4000,"Mag. (dB)","Rotation",90,"HorizontalAlignment","center","fontsize",GDFs);
	colormap(EDCMap);	
	colorbar("EastOutside");
	title(it2);	
		
	# Print to the output file
	print([ outdir fprefix "-Spectrogram01ms" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");

	# Report the stage
	disp("Spectrogram, 20 ms window...");
	fflush(stdout);
	
	# Spectrogram from -100 to +400 ms
	tbs = 100;
	tbe = 400;	
	sbs = 4410;
	sas = 17640;
	sfs = 1024;
	sol = 20;
	mlb = -90;
	
	# 20 ms spectrogram window
	swl = 882;
	
	# Spaziatura tics asse x e y
	xtsp = 25;
	ytsp = 2;
	
	# Computes the spectrogram window
	wind = [blackman(swl)' zeros(1,sfs-swl)]';
	
	# Computes the spectrograms
	[S1, f1, t1] = specgram(ir1((ic1 - (-1 + sbs + swl/2)):(ic1 + (sas + sfs))),sfs,fs,wind,sfs - sol);
	[S2, f2, t2] = specgram(ir2((ic2 - (-1 + sbs + swl/2)):(ic2 + (sas + sfs))),sfs,fs,wind,sfs - sol);
	
	# Normalize the spectrogram
	S1 = abs(S1);
	S1 = S1/max(S1(:));
	S1 = 20 * log10(max(S1,10^(mlb/20)));
	
	S2 = abs(S2);
	S2 = S2/max(S2(:));
	S2 = 20 * log10(max(S2,10^(mlb/20)));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the spectrogram 1
	subplot(2,1,1);	
	imagesc(1000 * (t1 - (sbs/fs)),0.001 * f1,S1);
	set(gca(),"ydir","normal");
	axis([-tbs, tbe, 0, fs / 2000]);
	grid("on");	
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	safetics("x",-tbs:xtsp:tbe);
	safetics("y",0:ytsp:(fs / 2000));
	text(1.1 * tbe,fs / 4000,"Mag. (dB)","Rotation",90,"HorizontalAlignment","center","fontsize",GDFs);
	colormap(EDCMap);	
	colorbar("EastOutside");
	title(it1);
		
	# Plot the spectrogram 2
	subplot(2,1,2);	
	imagesc(1000 * (t2 - (sbs/fs)),0.001 * f2,S2);
	set(gca(),"ydir","normal");
	axis([-tbs, tbe, 0, fs / 2000]);
	grid("on");	
	xlabel("Time (ms)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	safetics("x",-tbs:xtsp:tbe);
	safetics("y",0:ytsp:(fs / 2000));
	text(1.1 * tbe,fs / 4000,"Mag. (dB)","Rotation",90,"HorizontalAlignment","center","fontsize",GDFs);
	colormap(EDCMap);	
	colorbar("EastOutside");
	title(it2);	
		
	# Print to the output file
	print([ outdir fprefix "-Spectrogram20ms" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Remove variables no longer used
	clear S1 S2;

	# Report the stage
	disp("Morlet cycle/octave scalogram, high time resolution...");
	fflush(stdout);
	
	# Morlet scalogram from -10 cicles to +20 cycles
	# 20 Hz to 20 kHz
	# 100 frequency points and 350 cicle points
	cecb = -10;
	cece = 20;
	cenc = 350;
	cesf = 20;
	ceef = 20000;
	cenf = 100;
	
	# Parametri upsampling e filtro
	cefl = 32768;
	ceat = 120;
	ceuf = 8;
	cewm = 0.5;
	
	# Morlet wavelet
	ceft = 2;
	
	# Spaziatura tics asse x
	xtsp = 2;
			
	# Computes the cicle points
	cecp = linspace(cecb,cece,cenc);
	
	# Computes the frequency points
	cebs = 2 ^ (log2(ceef / cesf) / cenf); 
	cefp = logspace(log10(2 * cesf / (cebs * fs)),log10(2 * ceef / (cebs * fs)),cenf);
	
	# Computes the Morlet cycle/octave scalogram
	ce1 = tfir(ir1, ic1, cecp, cefp, cefl, ceat, ceuf, ceft, cewm);
	ce2 = tfir(ir2, ic2, cecp, cefp, cefl, ceat, ceuf, ceft, cewm);
			
	# Computes the envelope
	for I = 1:cenf
		ce1(I,:) = abs(hilbert(ce1(I,:)));
		ce2(I,:) = abs(hilbert(ce2(I,:)));
	endfor
	
	# Perform the scalogram interpolation
	# Not effective, so disabled
	cpif = 1;
	fpif = 1;
	
	# Check if interpolation is required
	if (cpif > 1 || fpif > 1)
		# Computes the interpolation cicle points
		itcp = linspace(cecb,cece,cenc * cpif);
		
		# Computes the frequency points
		itfp = logspace(log10(2 * cesf / (cebs * fs)),log10(2 * ceef / (cebs * fs)),cenf * fpif);
		
		# Interpolates the envelope
		ce1 = pchip2d(cecp,cefp,ce1,itcp(:)',itfp(:));
		ce2 = pchip2d(cecp,cefp,ce2,itcp(:)',itfp(:));
		
		# Assign the points
		cecp = itcp;
		cefp = logspace(log10(2 * cesf / fs),log10(2 * ceef / fs),cenf * fpif);
		
		# Delete unused variables
		clear itcp itfp;
	else
		# Remaps the frequency points for plotting
		cefp = logspace(log10(2 * cesf / fs),log10(2 * ceef / fs),cenf);
	endif
	
	# Normalize the envelope
	ce1 = ce1/max(ce1(:));
	ce2 = ce2/max(ce2(:));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the scalogram 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([cecb, cece, cesf / 1000, ceef / 1000, 0, 1]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (norm.)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",[0.25,0.5,0.75,1.0]);
	set(gca(),"yscale","log")
	view(15, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the scalogram 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([cecb, cece, cesf / 1000, ceef / 1000, 0, 1]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (norm.)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",[0.25,0.5,0.75,1.0]);
	set(gca(),"yscale","log")
	view(15, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-MorletScalogramHTRENVSD" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the scalogram 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([cecb, cece, cesf / 1000, ceef / 1000, 0, 1]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (norm.)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",[0.25,0.5,0.75,1.0]);
	set(gca(),"yscale","log")
	view(345, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the scalogram 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([cecb, cece, cesf / 1000, ceef / 1000, 0, 1]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (norm.)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",[0.25,0.5,0.75,1.0]);
	set(gca(),"yscale","log")
	view(345, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-MorletScalogramHTRENVSF" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Resample the scalogram for image plotting
	scrf = 4;
	scff = 8;
	rcecp = linspace(cecb,cece,cenc * scrf);
	rcenf = scff * cenf;
	rce1 = pchip2d(cecp,0:(cenf-1),ce1,rcecp,linspace(0,cenf-1,rcenf));
	rce2 = pchip2d(cecp,0:(cenf-1),ce2,rcecp,linspace(0,cenf-1,rcenf));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the scalogram map 1
	subplot(2,1,1);	
	imagesc(rcecp,0:(rcenf-1),rce1);
	set(gca(),"ydir","normal");
	axis([cecb, cece, 0, rcenf-1]);
	grid("on");	
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (norm)");
	text(1.1 * cece,rcenf / 2,"Mag. (norm)","Rotation",90,"HorizontalAlignment","center","fontsize",GDFs);
	safetics("x",cecb:xtsp:cece);
	safetics("y",(rcenf-1) * log(gfp / 0.02) / log(20 / 0.02),gfl);
	safetics("z",[0.25,0.5,0.75,1.0]);	
	colormap(EDCMap);	
	colorbar("EastOutside");
	title(it1);
		
	# Plot the scalogram map 2
	subplot(2,1,2);	
	imagesc(rcecp,0:rcenf-1,rce2);
	set(gca(),"ydir","normal");	
	axis([cecb, cece, 0, rcenf-1]);
	grid("on");	
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (norm)");
	text(1.1 * cece,rcenf / 2,"Mag. (norm)","Rotation",90,"HorizontalAlignment","center","fontsize",GDFs);
	safetics("x",cecb:xtsp:cece);
	safetics("y",(rcenf-1) * log(gfp / 0.02) / log(20 / 0.02),gfl);
	safetics("z",[0.25,0.5,0.75,1.0]);	
	colormap(EDCMap);	
	colorbar("EastOutside");
	title(it2);	
		
	# Print to the output file
	print([ outdir fprefix "-MorletScalogramHTRENVMap" fext ],fdevice);
	
	# Convert to log scale
	ce1 = 20 * log10(max(ce1,10^(-30/20)));
	ce2 = 20 * log10(max(ce2,10^(-30/20)));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the scalogram 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([cecb, cece, cesf / 1000, ceef / 1000, -30, 2]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",-25:5:5);
	set(gca(),"yscale","log")
	view(15, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the scalogram 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([cecb, cece, cesf / 1000, ceef / 1000, -30, 2]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",-25:5:5);
	set(gca(),"yscale","log")
	view(15, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-MorletScalogramHTRETCSD" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the scalogram 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([cecb, cece, cesf / 1000, ceef / 1000, -30, 2]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",-25:5:5);
	set(gca(),"yscale","log")
	view(345, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the scalogram 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([cecb, cece, cesf / 1000, ceef / 1000, -30, 2]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",-25:5:5);
	set(gca(),"yscale","log")
	view(345, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-MorletScalogramHTRETCSF" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Resample the scalogram for image plotting
	scrf = 4;
	scff = 8;
	rcecp = linspace(cecb,cece,cenc * scrf);
	rcenf = scff * cenf;
	rce1 = pchip2d(cecp,0:(cenf-1),ce1,rcecp,linspace(0,cenf-1,rcenf));
	rce2 = pchip2d(cecp,0:(cenf-1),ce2,rcecp,linspace(0,cenf-1,rcenf));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the scalogram map 1
	subplot(2,1,1);	
	imagesc(rcecp,0:rcenf-1,rce1);
	set(gca(),"ydir","normal");
	axis([cecb, cece, 0, rcenf-1]);
	grid("on");	
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	text(1.1 * cece,rcenf / 2,"Mag. (dB)","Rotation",90,"HorizontalAlignment","center","fontsize",GDFs);
	safetics("x",cecb:xtsp:cece);
	safetics("y",(rcenf-1) * log(gfp / 0.02) / log(20 / 0.02),gfl);
	safetics("z",-25:5:5);
	colormap(EDCMap);	
	colorbar("EastOutside");
	title(it1);
		
	# Plot the scalogram map 2
	subplot(2,1,2);	
	imagesc(rcecp,0:rcenf-1,rce2);
	set(gca(),"ydir","normal");
	axis([cecb, cece, 0, rcenf-1]);
	grid("on");	
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");	
	text(1.1 * cece,rcenf / 2,"Mag. (dB)","Rotation",90,"HorizontalAlignment","center","fontsize",GDFs);
	safetics("x",cecb:xtsp:cece);
	safetics("y",(rcenf-1) * log(gfp / 0.02) / log(20 / 0.02),gfl);
	safetics("z",-25:5:5);
	colormap(EDCMap);	
	colorbar("EastOutside");
	title(it2);	

	# Print to the output file
	print([ outdir fprefix "-MorletScalogramHTRETCMap" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Remove variables no longer used
	clear cecp rcecp cefp ce1 ce2 rce1 rce2;

	# Report the stage
	disp("Morlet cycle/octave scalogram, medium time resolution...");
	fflush(stdout);
	
	# Morlet cycle/octave scalogram from -10 cicles to +20 cicles
	# 20 Hz to 20 kHz
	# 100 frequency points and 350 cicle points
	cecb = -10;
	cece = 20;
	cenc = 350;
	cesf = 20;
	ceef = 20000;
	cenf = 100;
	
	# Parametri upsampling e filtro
	cefl = 32768;
	ceat = 120;
	ceuf = 8;
	cewm = 2;
	
	# Morlet wavelet
	ceft = 2;
	
	# Spaziatura tics asse x
	xtsp = 2;
			
	# Computes the cicle points
	cecp = linspace(cecb,cece,cenc);
	
	# Computes the frequency points
	cebs = 2 ^ (log2(ceef / cesf) / cenf); 
	cefp = logspace(log10(2 * cesf / (cebs * fs)),log10(2 * ceef / (cebs * fs)),cenf);
	
	# Computes the Morlet cycle/octave scalogram
	ce1 = tfir(ir1, ic1, cecp, cefp, cefl, ceat, ceuf, ceft, cewm);
	ce2 = tfir(ir2, ic2, cecp, cefp, cefl, ceat, ceuf, ceft, cewm);
	
	# Computes the envelope
	for I = 1:cenf
		ce1(I,:) = abs(hilbert(ce1(I,:)));
		ce2(I,:) = abs(hilbert(ce2(I,:)));
	endfor
	
	# Perform the scalogram interpolation
	# Not effective, so disabled
	cpif = 1;
	fpif = 1;
	
	# Check if interpolation is required
	if (cpif > 1 || fpif > 1)
		# Computes the interpolation cicle points
		itcp = linspace(cecb,cece,cenc * cpif);
		
		# Computes the frequency points
		itfp = logspace(log10(2 * cesf / (cebs * fs)),log10(2 * ceef / (cebs * fs)),cenf * fpif);
		
		# Interpolates the envelope
		ce1 = pchip2d(cecp,cefp,ce1,itcp(:)',itfp(:));
		ce2 = pchip2d(cecp,cefp,ce2,itcp(:)',itfp(:));
		
		# Assign the points
		cecp = itcp;
		cefp = logspace(log10(2 * cesf / fs),log10(2 * ceef / fs),cenf * fpif);
		
		# Delete unused variables
		clear itcp itfp;
	else
		# Remaps the frequency points for plotting
		cefp = logspace(log10(2 * cesf / fs),log10(2 * ceef / fs),cenf);
	endif
	
	# Normalize the envelope
	ce1 = ce1/max(ce1(:));
	ce2 = ce2/max(ce2(:));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the scalogram 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([cecb, cece, cesf / 1000, ceef / 1000, 0, 1]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (norm.)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",[0.25,0.5,0.75,1.0]);
	set(gca(),"yscale","log")
	view(15, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the scalogram 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([cecb, cece, cesf / 1000, ceef / 1000, 0, 1]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (norm.)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",[0.25,0.5,0.75,1.0]);
	set(gca(),"yscale","log")
	view(15, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-MorletScalogramMTRENVSD" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the scalogram 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([cecb, cece, cesf / 1000, ceef / 1000, 0, 1]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (norm.)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",[0.25,0.5,0.75,1.0]);
	set(gca(),"yscale","log")
	view(345, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the scalogram 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([cecb, cece, cesf / 1000, ceef / 1000, 0, 1]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (norm.)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",[0.25,0.5,0.75,1.0]);
	set(gca(),"yscale","log")
	view(345, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-MorletScalogramMTRENVSF" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Resample the scalogram for image plotting
	scrf = 4;
	scff = 8;
	rcecp = linspace(cecb,cece,cenc * scrf);
	rcenf = scff * cenf;
	rce1 = pchip2d(cecp,0:(cenf-1),ce1,rcecp,linspace(0,cenf-1,rcenf));
	rce2 = pchip2d(cecp,0:(cenf-1),ce2,rcecp,linspace(0,cenf-1,rcenf));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the scalogram map 1
	subplot(2,1,1);	
	imagesc(rcecp,0:rcenf-1,rce1);
	set(gca(),"ydir","normal");	
	axis([cecb, cece, 0, rcenf-1]);
	grid("on");	
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (norm)");	
	text(1.1 * cece,rcenf / 2,"Mag. (norm)","Rotation",90,"HorizontalAlignment","center","fontsize",GDFs);
	safetics("x",cecb:xtsp:cece);
	safetics("y",(rcenf-1) * log(gfp / 0.02) / log(20 / 0.02),gfl);
	safetics("z",[0.25,0.5,0.75,1.0]);
	colormap(EDCMap);	
	colorbar("EastOutSide");
	title(it1);
		
	# Plot the scalogram map 2
	subplot(2,1,2);	
	imagesc(rcecp,0:rcenf-1,rce2);
	set(gca(),"ydir","normal");	
	axis([cecb, cece, 0, rcenf-1]);
	grid("on");	
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (norm)");	
	text(1.1 * cece,rcenf / 2,"Mag. (norm)","Rotation",90,"HorizontalAlignment","center","fontsize",GDFs);
	safetics("x",cecb:xtsp:cece);
	safetics("y",(rcenf-1) * log(gfp / 0.02) / log(20 / 0.02),gfl);
	safetics("z",[0.25,0.5,0.75,1.0]);
	colormap(EDCMap);	
	colorbar("EastOutSide");
	title(it2);	
		
	# Print to the output file
	print([ outdir fprefix "-MorletScalogramMTRENVMap" fext ],fdevice);
	
	# Convert to log scale
	ce1 = 20 * log10(max(ce1,10^(-30/20)));
	ce2 = 20 * log10(max(ce2,10^(-30/20)));
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the scalogram 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([cecb, cece, cesf / 1000, ceef / 1000, -30, 2]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",-25:5:5);
	set(gca(),"yscale","log")
	view(15, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the scalogram 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([cecb, cece, cesf / 1000, ceef / 1000, -30, 2]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",-25:5:5);
	set(gca(),"yscale","log")
	view(15, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-MorletScalogramMTRETCSD" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
	
	# Plot the scalogram 1
	subplot(2,1,1);	
	if (surcntenable > 0)		
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce1,"edgecolor",ULColor,"LineWidth",EDLw);
	endif	
	axis([cecb, cece, cesf / 1000, ceef / 1000, -30, 2]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",-25:5:5);
	set(gca(),"yscale","log")
	view(345, 45);
	colormap(EDCMap);
	title(it1);
	
	# Plot the scalogram 2
	subplot(2,1,2);	
	# Check for contour parameters
	if (surcntenable > 0)				
		# Plot the surface and contour
		surfc(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	else
		# Plot the surface
		surf(cecp,cefp * fs / 2000,ce2,"edgecolor",LLColor,"LineWidth",EDLw);
	endif
	axis([cecb, cece, cesf / 1000, ceef / 1000, -30, 2]);
	grid("on");
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	safetics("x",cecb:xtsp:cece);
	safetics("y",gfp,gfl);
	safetics("z",-25:5:5);
	set(gca(),"yscale","log")
	view(345, 45);
	colormap(EDCMap);
	title(it2);		
	
	# Print to the output file
	print([ outdir fprefix "-MorletScalogramMTRETCSF" fext ],fdevice);
	
	# Reset to the standard colormap
	colormap("default");
	
	# Resample the scalogram for image plotting
	scrf = 4;
	scff = 8;
	rcecp = linspace(cecb,cece,cenc * scrf);
	rcenf = scff * cenf;
	rce1 = pchip2d(cecp,0:(cenf-1),ce1,rcecp,linspace(0,cenf-1,rcenf));
	rce2 = pchip2d(cecp,0:(cenf-1),ce2,rcecp,linspace(0,cenf-1,rcenf));	
	
	# Open a new plot
	newplot();
	clf();
	figure(1,"visible","off");
		
	# Plot the scalogram map 1
	subplot(2,1,1);	
	imagesc(rcecp,0:rcenf-1,rce1);
	set(gca(),"ydir","normal");	
	axis([cecb, cece, 0, rcenf-1]);
	grid("on");	
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	text(1.1 * cece,rcenf / 2,"Mag. (dB)","Rotation",90,"HorizontalAlignment","center","fontsize",GDFs);
	safetics("x",cecb:xtsp:cece);
	safetics("y",(rcenf-1) * log(gfp / 0.02) / log(20 / 0.02),gfl);
	safetics("z",-25:5:5);
	colormap(EDCMap);	
	colorbar("EastOutside");
	title(it1);
		
	# Plot the scalogram map 2
	subplot(2,1,2);	
	imagesc(rcecp,0:rcenf-1,rce2);
	set(gca(),"ydir","normal");	
	axis([cecb, cece, 0, rcenf-1]);
	grid("on");	
	xlabel("Time (cycles)");
	ylabel("Frequency (kHz)");	
	zlabel("Mag. (dB)");
	text(1.1 * cece,rcenf / 2,"Mag. (dB)","Rotation",90,"HorizontalAlignment","center","fontsize",GDFs);
	safetics("x",cecb:xtsp:cece);
	safetics("y",(rcenf-1) * log(gfp / 0.02) / log(20 / 0.02),gfl);
	safetics("z",-25:5:5);
	colormap(EDCMap);	
	colorbar("EastOutside");
	title(it2);	
		
	# Print to the output file
	print([ outdir fprefix "-MorletScalogramMTRETCMap" fext ],fdevice);
		
	# Reset to the standard colormap
	colormap("default");
	
	# Remove variables no longer used
	clear cecp rcecp cefp ce1 ce2 rce1 rce2;

	# Report the stage
	disp("Completed, wait for GNUPlot to complete before opening output files.");
	fflush(stdout);

endfunction