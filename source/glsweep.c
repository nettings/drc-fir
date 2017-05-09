/****************************************************************************

    DRC: Digital Room Correction
    Copyright (C) 2002-2004 Denis Sbragion

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

		You can contact the author on Internet at the following address:

				d.sbragion@infotecna.it

****************************************************************************/

/* Log sweep and inverse filter generation program */

/* Derived from a perl script kindly provided by Edward Wildgoose */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Decommentare per abilitare la compilazione in doppia precisione */
/* Uncomment to enable double precision computation */
#define UseDouble

#ifdef UseDouble
	/* Tipo floating point usato per le elaborazioni */
	#define DLReal double
#else
	/* Tipo floating point usato per le elaborazioni */
	#define DLReal float
#endif

/* Imposta l'uso delle funzioni trigonometriche ridotte */
#define UseTypedTrigs

/* Verifica l'uso delle funzioni trigonometriche ridotte */
#ifdef UseTypedTrigs
	#ifdef UseDouble
		#define DLSin sin
		#define DLCos cos
		#define DLTan tan
		#define DLATan atan
	#else
		#define DLSin sinf
		#define DLCos cosf
		#define DLTan tanf
		#define DLATan atanf
	#endif
#else
	#define DLSin sin
	#define DLCos cos
	#define DLTan tan
	#define DLATan atan
#endif

#ifndef M_PI
	#define M_PI ((DLReal) 3.14159265358979323846264338327950288)
#endif

#ifndef M_2PI
	#define M_2PI ((DLReal) 6.28318530717958647692528676655900576)
#endif

/* Output stringhe con sync output e parametro */
int sputsp(const char * s, const char * p)
	{
		int Res;
		if (p == NULL)
			Res = puts(s);
		else
			Res = printf("%s%s\n",s,p);
		fflush(stdout);
		return(Res);
	}

/* Output stringhe con sync output */
int sputs(const char * s)
	{
		return(sputsp(s, NULL));
	}

/* Main procedure */
int main(int argc, char * argv[])
	{
		/* Input parameters */
		DLReal Rate;
		DLReal Amplitude;
		DLReal HzStart;
		DLReal HzEnd;
		DLReal Duration;
		DLReal Silence;
		DLReal LeadIn;
		DLReal LeadOut;
		char * SweepFile;
		char * InverseFile;

		/* Generation parameters */

		/* Base sweep generation */
		int SweepLen;
		int SilenceLen;
		DLReal W1;
		DLReal W2;
		DLReal Ratio;
		DLReal Sample;
		DLReal S1;
		DLReal S2;
		DLReal DecayTime;
		DLReal Decay;
		int I;
		int J;
		float FS;

		/* Sweep normalization factor */
		DLReal SNF;

		/* Lead in and lead out Blackman windowing */
		int LeadInLen;
		DLReal WC1In;
		DLReal WC2In;
		int LeadOutLen;
		DLReal WC1Out;
		DLReal WC2Out;
		DLReal WC;

		/* Output file */
		FILE * OF;

		/* Initial message */
		sputs("\nGLSweep 1.1.0: log sweep and inverse filter generation.");
		sputs("Copyright (C) 2002-2011 Denis Sbragion");
		#ifdef UseDouble
			sputs("\nCompiled with double precision arithmetic.");
		#else
			sputs("\nCompiled with single precision arithmetic.");
		#endif
		sputs("\nThis program may be freely redistributed under the terms of");
		sputs("the GNU GPL and is provided to you as is, without any warranty");
		sputs("of any kind. Please read the file \"COPYING\" for details.");

		/* Check program arguments */
		if (argc < 11)
			{
				sputs("\nUsage: glsweep rate amplitude hzstart hzend duration silence");
				sputs("        leadin leadout sweepfile inversefile");
				sputs("\nParameters:\n");
				sputs("  rate: reference sample rate");
				sputs("  amplitude: sweep amplitude");
				sputs("  hzstart: sweep start frequency");
				sputs("  hzend: sweep end frequency");
				sputs("  duration: sweep duration in seconds");
				sputs("  silence: leading and trailing silence duration in seconds");
				sputs("  leadin: leading window length as a fraction of duration");
				sputs("  leadout: trailing window length as a fraction of duration");
				sputs("  sweepfile: sweep file name");
				sputs("  inversefile: inverse sweep file name");
				sputs("\nExample: glsweep 44100 0.5 10 21000 45 2 0.05 0.005 sweep.pcm inverse.pcm\n");
				return 0;
			}

		/* Get the input parameters from the command line */
		sputs("\nCommand line parsing.");
		Rate = (DLReal) atof(argv[1]);
		Amplitude = (DLReal) atof(argv[2]);
		HzStart = (DLReal) atof(argv[3]);
		HzEnd = (DLReal) atof(argv[4]);
		Duration = (DLReal) atof(argv[5]);
		Silence = (DLReal) atof(argv[6]);
		LeadIn = (DLReal) atof(argv[7]);
		LeadOut = (DLReal) atof(argv[8]);
		SweepFile = argv[9];
		InverseFile = argv[10];

		/* Computes internal generation values */
		sputs("Sweep generation setup.");

		/* Base sweep generation */
		SweepLen = (int) (Rate * Duration);
		SilenceLen = (int) (Rate * Silence);
		W1 = (DLReal) (HzStart * M_2PI);
		W2 = (DLReal) (HzEnd * M_2PI);
		Ratio = (DLReal) log(W2 / W1);
		S1 = (DLReal) ((W1 * Duration) / Ratio);
		S2 = (DLReal) (Ratio / SweepLen);
		DecayTime = (DLReal) (SweepLen * log(2.0) / Ratio);

		/* Lead in and lead out Blackman windowing */
    LeadInLen = (int) (LeadIn * SweepLen);
    WC1In = (DLReal) M_PI / (LeadInLen - 1);
    WC2In = (DLReal) M_2PI / (LeadInLen - 1);
    LeadOutLen = (int) (LeadOut * SweepLen);
    WC1Out = (DLReal) M_PI / (LeadOutLen - 1);
    WC2Out = (DLReal) M_2PI / (LeadOutLen - 1);

    /* Report generation parameters */
    printf("\nSweep length: %d samples\n",SweepLen);
    printf("Silence length: %d samples\n",SilenceLen);
    printf("Total sweep length: %d samples\n",2 * SilenceLen + SweepLen);
    printf("Total sweep file size: %d bytes\n",sizeof(float) * (2 * SilenceLen + SweepLen));
    printf("Total inverse length: %d samples\n",SweepLen);
    printf("Total inverse file size: %d bytes\n\n",sizeof(float) * SweepLen);
    fflush(stdout);

		/* Open the sweep file */
		sputsp("Opening sweep file: ", SweepFile);
		if ((OF = fopen(SweepFile,"wb")) == NULL)
			{
				perror("Unable to open sweep file");
				return 1;
			}

		/* Generates the sweep file */
		sputs("Generating the sweep file...");

		/* Initial silence */
		FS = (DLReal) 0.0;
		for (I = 0;I < SilenceLen;I++)
			fwrite(&FS,sizeof(float),1,OF);

		/* Initial lead in */
		for (I = 0;I < LeadInLen;I++)
			{
				Sample = (DLReal) DLSin(S1 * (exp(I * S2) - 1.0));
				WC = (DLReal) (0.42 - 0.5 * DLCos(WC1In * I) + 0.08 * DLCos(WC2In * I));
				FS = (float) (Sample * WC * Amplitude);
				fwrite(&FS,sizeof(float),1,OF);
			}

		/* Full sweep */
		for (I = LeadInLen;I < SweepLen - LeadOutLen;I++)
			{
				Sample = (DLReal) DLSin(S1 * (exp(I * S2) - 1.0));
				FS = (float) (Sample * Amplitude);
				fwrite(&FS,sizeof(float),1,OF);
			}

		/* Final lead out */
		for (I = SweepLen - LeadOutLen,J = LeadOutLen;I < SweepLen;I++,J--)
			{
				Sample = (DLReal) DLSin(S1 * (exp(I * S2) - 1.0));
				WC = (DLReal) (0.42 - 0.5 * DLCos(WC1Out * J) + 0.08 * DLCos(WC2Out * J));
				FS = (float) (Sample * WC * Amplitude);
				fwrite(&FS,sizeof(float),1,OF);
			}

		/* Final silence */
		FS = (DLReal) 0.0;
		for (I = 0;I < SilenceLen;I++)
			fwrite(&FS,sizeof(float),1,OF);

		/* Close the sweep file */
		sputs("Sweep file generated.");
		fclose(OF);

		/* Computes the sweep normalization factor */

		/* Number of octaves involved */
		SNF = (DLReal) (log(HzEnd/HzStart) / log(2.0));

		/* Bandwidth and exponential decay compensation */
		SNF = (DLReal) ((2.0 * (HzEnd - HzStart) / Rate) * (log(4.0) * SNF / (1 - pow(2.0,-SNF))) / SweepLen);

		/* Report the normalization factor */
		printf("Sweep normalizaton factor: %g\n",SNF);
		fflush(stdout);

		/* Open the inverse file */
		sputsp("Opening inverse file: ", InverseFile);
		if ((OF = fopen(InverseFile,"wb")) == NULL)
			{
				perror("Unable to open inverse file");
				return 1;
			}

		/* Generates the sweep file */
		sputs("Generating the inverse file...");

		/* Final lead out */
		for (I = 0,J = SweepLen;I < LeadOutLen;I++,J--)
			{
				Decay = (DLReal) pow(0.5,I / DecayTime);
				Sample = (DLReal) DLSin(S1 * (exp(J * S2) - 1.0));
				WC = (DLReal) (0.42 - 0.5 * DLCos(WC1Out * I) + 0.08 * DLCos(WC2Out * I));
				FS = (float) (SNF * Sample * WC * Decay);
				fwrite(&FS,sizeof(float),1,OF);
			}

		/* Full sweep */
		for (I = LeadOutLen,J = SweepLen - LeadOutLen;I < SweepLen - LeadInLen;I++,J--)
			{
				Decay = (DLReal) pow(0.5,I / DecayTime);
				Sample = (DLReal) DLSin(S1 * (exp(J * S2) - 1.0));
				FS = (float) (SNF * Sample * Decay);
				fwrite(&FS,sizeof(float),1,OF);
			}

		/* Initial lead in */
		for (I = SweepLen - LeadInLen,J = LeadInLen;I < SweepLen;I++,J--)
			{
				Decay = (DLReal) pow(0.5,I / DecayTime);
				Sample = (DLReal) DLSin(S1 * (exp(J * S2) - 1.0));
				WC = (DLReal) (0.42 - 0.5 * DLCos(WC1In * J) + 0.08 * DLCos(WC2In * J));
				FS = (float) (SNF * Sample * WC * Decay);
				fwrite(&FS,sizeof(float),1,OF);
			}

		/* Close the inverse file */
		sputs("Inverse file generated.");
		fclose(OF);

		/* Execution completed */
		return 0;
	}
