function [w,t]=morletw(fs,a,b,ki,timeunits,woo)

%MORLETW  Calculate the Morlet Wavelet.
%
%         W = MORLETW (FS,SC) calculates the Morlet wavelet
%         at scale SC sampled at rate FS.
%
%         W = MORLETW (FS,SC,SHF) calculates the Morlet wavelet
%         at scale SC, sampled at rate FS and delayed SHF seconds.
%
%         W = MORLETW (FS,SC,SHF,TIM) calculates the Morlet wavelet
%         at scale SC, sampled at rate FS and delayed SHF seconds,
%         changing the time interval to -TIM:TIM seconds.
%
%         W = MORLETW (FS,SC,SHF,TIM,TIMEUNITS) makes the wavelet be
%         expressed in terms of TIMEUNITS instead of seconds. The default
%         value of TIMEUNITS is 1 (seconds). Example: 0.001 = milliseconds. 
%         A value of 0 means the default (1).
%
%         W = MORLETW (FS,SC,SHF,TIM,TIMEUNITS,W0) changes the central 
%         frequency of the wavelet to W0. The default value is 2*PI.  
%
%         The SC scale wavelet is calculated by compressing or expanding
%         the scale 1 wavelet. This last is computed in the interval 
%         -TIM:TIM timeunits, and compressions or dilations modify this
%         interval too. The default value for TIM is 6 time units, since
%         the values of the Morlet wavelet are negligible for ABS(t)>6 t.u.
%
%         [W,T] = MORLETW (FS,SC,SHF,TIM,TIMEUNITS,W0) also returns
%         the time basis for the desired wavelet, in TIMEUNITS.
%
%         See also: SCALOG.
%

%--------------------------------------------------------
% Copyright (C) 1994, 1995, 1996, by Universidad de Vigo 
%                                                      
%                                                      
% Uvi_Wave is free software; you can redistribute it and/or modify it      
% under the terms of the GNU General Public License as published by the    
% Free Software Foundation; either version 2, or (at your option) any      
% later version.                                                           
%                                                                          
% Uvi_Wave is distributed in the hope that it will be useful, but WITHOUT  
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
% FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
% for more details.                                                        
%                                                                          
% You should have received a copy of the GNU General Public License        
% along with Uvi_Wave; see the file COPYING.  If not, write to the Free    
% Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.             
%                                                                          
%       Author: Nuria Gonzalez Prelcic
%       e-mail: Uvi_Wave@tsc.uvigo.es
%--------------------------------------------------------


if nargin<3              % The default time shifting value 
	b=0;             % is b=0.
end

if nargin<4              % Default timing [-6 sec. to 6 sec.]
	ki=6;
end

if ki<5
	fprintf('WARNING: Small time interval. Wavelet ends may be too abrupt.\n');
end
	
% Set the time units to 1 (seconds) as a default value.
% When representing scalograms of sampled signals, the 
% time units must be in the same range that the sampling time 
% interval (This is documented in SCALOG.M).
if nargin<5
	timeunits=1;
end
if timeunits==0
	timeunits=1;
end

fs=fs*timeunits;

% We find the number of points according to the sampling rate 
% and the time space.

n=ki*2*fs+1;
res=(2*ki)/n;             
N=(2*ki)*a/res;            % Number of points for the scaled wavelet

t=linspace(-ki*a,ki*a,N);  % Time domain for the scaled wavelet

wo=2*pi;                 % wo=2*pi makes the 1st scale have
			 % the central freq. at 1 Hz
if nargin>5
	wo=woo;
end

w=sqrt(1/a)*exp(j*wo*(t-b)/a).*exp(-((t-b)/a).^2/2);
