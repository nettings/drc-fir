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

/* Log sweep and inverse filter convolution program */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fftsg_h.h"

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

/* Determina la lunghezza di un file */
size_t FSize(char * FName)
	{
		long FS;
		FILE * F;

		if ((F = fopen(FName,"rb")) == NULL)
			{
				perror("Unable to open file");
				return 0;
			}

		fseek(F,0,SEEK_END);
		FS = ftell(F);
		fclose(F);
		return (size_t) FS;
	}

/* Calola il valore RMS del segnale Sig */
DLReal GetRMSLevel(const DLReal * Sig,const int SigLen)
	{
		DLReal RMS;
		int I;

		RMS = 0;
		for (I = 0; I < SigLen; I++)
			RMS += Sig[I] * Sig[I];

		return (DLReal) sqrt(RMS);
	}

/* Halfcomplex array convolution */
void hcconvolve(DLReal * A,const DLReal * B,const int N)
	{
		int R;
		int I;
		DLReal T1;
		DLReal T2;

		A[0] *= B[0];
		A[1] *= B[1];
		for (R = 2,I = 3;R < N;R += 2,I += 2)
			{
				T1 = A[R] * B[R];
				T2 = A[I] * B[I];
				A[I] = ((A[R] + A[I]) * (B[R] + B[I])) - (T1 + T2);
				A[R] = T1 - T2;
			}
	}

/* Main procedure */
int main(int argc, char * argv[])
	{
		/* Input parameters */
		char * SweepFile;
		char * InverseFile;
		char * OutFile;
		char * RefSweepFile;
		DLReal MinGain;
		DLReal DLStart;

		/* Convolution parameters */
		int SS;
		int IS;
		int RS;
		int CS;
		int CL;
		int I;
		int J;
		DLReal * Sweep;
		DLReal * Inverse;
		float RF;

		/* Dip limiting */
		DLReal RMSLevel;
		DLReal RMSMin;
		DLReal DLSLevel;
		DLReal DLLevel;
		DLReal DLGFactor;
		DLReal DLAbs;
		DLReal DLTheta;

		/* Peak position */
		int MP;
		DLReal AMax;

		/* Input/output file */
		FILE * IOF;

		/* Initial message */
		sputs("\nLSConv 1.1.0: log sweep and inverse filter convolution.");
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
		if (argc < 4 || (argc > 4 && argc < 6))
			{
				sputs("\nUsage: LSConv sweepfile inversefile outfile [refsweep mingain [dlstart]]");
				sputs("\nParameters:\n");
				sputs("  sweepfile: sweep file name");
				sputs("  inversefile: inverse sweep file name");
				sputs("  outfile: output impulse response file");
				sputs("  refsweep: reference channel sweep file name");
				sputs("  mingain: min gain for reference channel inversion");
				sputs("  dlstart: dip limiting start for reference channel inversion");
				sputs("\nExample: lsconv sweep.pcm inverse.pcm impulse.pcm refchannel.pcm 0.1 0.8\n");
				return 0;
			}

		/* Get the input parameters from the command line */
		sputs("\nCommand line parsing.");
		SweepFile = argv[1];
		InverseFile = argv[2];
		OutFile = argv[3];
		if (argc > 4)
			{
				RefSweepFile = argv[4];
				MinGain = (DLReal) atof(argv[5]);
			}
		else
			RefSweepFile = NULL;
		if (argc >= 7)
			DLStart = (DLReal) atof(argv[6]);
		else
			DLStart = (DLReal) 2.0;

		/* Computes the convolution length */
		sputs("Convolution length computation.");
		SS = FSize(SweepFile) / sizeof(float);
		IS = FSize(InverseFile) / sizeof(float);
		if (RefSweepFile != NULL)
			RS = FSize(RefSweepFile) / sizeof(float);
		else
			RS = 1;
		CL = SS + IS + RS - 2;
		for(CS = 1;CS < CL;CS <<= 1);

		/* Convolution arrays allocation */
		sputs("Convolution arrays allocation.");
		if ((Sweep = (DLReal *) malloc(sizeof(DLReal) * CS)) == NULL)
			{
				sputs("Memory allocation failure.");
				return 1;
			}
		if ((Inverse = (DLReal *) malloc(sizeof(DLReal) * CS)) == NULL)
			{
				sputs("Memory allocation failure.");
				free(Sweep);
				return 1;
			}

		/* Read the inverse file */
		sputsp("Reading inverse file: ",InverseFile);
		if ((IOF = fopen(InverseFile,"rb")) == NULL)
			{
				perror("Unable to open inverse file");
				return 1;
			}
		for(I = 0;I < IS;I++)
			{
				fread(&RF,sizeof(float),1,IOF);
				Inverse[I] = (DLReal) RF;
			}
		for (I = IS;I < CS;I++)
			Inverse[I] = (DLReal) 0.0;
		fclose(IOF);

		sputs("Inverse filter FFT...");
		rdft(CS,OouraRForward,Inverse);

		/* Check the reference file */
		if (RefSweepFile != NULL)
			{
				/* Read the reference file */
				sputsp("Reading reference file: ",RefSweepFile);
				if ((IOF = fopen(RefSweepFile,"rb")) == NULL)
					{
						perror("Unable to open reference file");
						return 1;
					}
				for (I = 0;I < CS;I++)
					Sweep[I] = (DLReal) 0.0;
				for(I = CS - IS,J = 0;J < RS;I++,J++)
					{
						fread(&RF,sizeof(float),1,IOF);
						Sweep[I % CS] = (DLReal) RF;
					}
				fclose(IOF);

				/* Convolving sweep and inverse */
				sputs("Reference inversion and convolution...");
				rdft(CS,OouraRForward,Sweep);
				hcconvolve(Sweep,Inverse,CS);

				/* Computes the RMS Level */
				RMSLevel = Sweep[0] * Sweep[0];
				for (I = 2,J = 3;I < CS;I += 2,J += 2)
					RMSLevel += Sweep[I] * Sweep[I] + Sweep[J] * Sweep[J];
				RMSLevel = (DLReal) sqrt(2.0 * RMSLevel / CS);
				RMSMin = MinGain * RMSLevel;

				/* Check the gain truncation type */
				if (DLStart >= (DLReal) 1.0)
					{
						/* Check starting components */
						if (Sweep[0] < RMSMin)
							Sweep[0] = ((DLReal) 1.0) / RMSMin;
						else
							Sweep[0] = ((DLReal) 1.0) / Sweep[0];
						if (Sweep[1] < RMSMin)
							Sweep[1] = ((DLReal) 1.0) / RMSMin;
						else
							Sweep[1] = ((DLReal) 1.0) / Sweep[1];

						/* Gain truncation scan */
						for (I = 2,J = 3;I < CS;I += 2,J += 2)
							{
								/* Gain computation */
								DLAbs = (DLReal) hypot(Sweep[I],Sweep[J]);

								/* Gain check and limitation */
								if (DLAbs < RMSMin)
									{
										/* Gain limited inversion */
										DLTheta = (DLReal) atan2(Sweep[J],Sweep[I]);
										DLAbs = ((DLReal) 1.0) / RMSMin;
										Sweep[I] = DLAbs * (DLReal) cos(-DLTheta);
										Sweep[J] = DLAbs * (DLReal) sin(-DLTheta);
									}
								else
									{
										/* Inversion and renormalization */
										DLAbs = ((DLReal) 1.0) / (DLAbs * DLAbs);
										Sweep[I] *= DLAbs;
										Sweep[J] *= -DLAbs;
									}
							}
					}
				else
					{
						/* Determina i fattori per la limitazione guadagno */
						DLSLevel = RMSMin / DLStart;
						DLGFactor = DLSLevel - RMSMin;

						/* Check starting components */
						if (Sweep[0] < DLSLevel)
							{
								/* Riassegna il guadagno del filtro */
								DLLevel = (DLSLevel - Sweep[0]) / DLGFactor;
								DLLevel = DLLevel / (((DLReal) 1.0) + DLLevel);
								Sweep[0] = ((DLReal) 1.0) / (DLSLevel - DLGFactor * DLLevel);
							}
						else
							Sweep[0] = ((DLReal) 1.0) / Sweep[0];
						if (Sweep[1] < DLSLevel)
							{
								/* Riassegna il guadagno del filtro */
								DLLevel = (DLSLevel - Sweep[1]) / DLGFactor;
								DLLevel = DLLevel / (((DLReal) 1.0) + DLLevel);
								Sweep[1] = ((DLReal) 1.0) / (DLSLevel - DLGFactor * DLLevel);
							}
						else
							Sweep[1] = ((DLReal) 1.0) / Sweep[1];

						/* Scansione per limitazione guadagno */
						for (I = 2,J = 3;I < CS;I += 2,J += 2)
							{
								DLAbs = (DLReal) hypot(Sweep[I],Sweep[J]);
								if (DLAbs <= (DLReal) 0.0)
									{
										Sweep[I] = ((DLReal) 1.0) / RMSMin;
										Sweep[J] = 0;
									}
								else
									if (DLAbs < DLSLevel)
										{
											/* Riassegna il guadagno del filtro */
											DLLevel = (DLSLevel - DLAbs) / DLGFactor;
											DLLevel = DLLevel / (((DLReal) 1.0) + DLLevel);
											DLAbs = ((DLReal) 1.0) / (DLSLevel - DLGFactor * DLLevel);

											/* Gain limited inversion */
											DLTheta = (DLReal) atan2(Sweep[J],Sweep[I]);
											Sweep[I] = DLAbs * (DLReal) cos(-DLTheta);
											Sweep[J] = DLAbs * (DLReal) sin(-DLTheta);
										}
									else
										{
											/* Inversion and renormalization */
											DLAbs = ((DLReal) 1.0) / (DLAbs * DLAbs);
											Sweep[I] *= DLAbs;
											Sweep[J] *= -DLAbs;
										}
							}
					}

				/* Inverse convolution */
				hcconvolve(Inverse,Sweep,CS);
			}

		/* Read the sweep file */
		sputsp("Reading sweep file: ",SweepFile);
		if ((IOF = fopen(SweepFile,"rb")) == NULL)
			{
				perror("Unable to open sweep file");
				return 1;
			}
		for(I = 0;I < SS;I++)
			{
				fread(&RF,sizeof(float),1,IOF);
				Sweep[I] = (DLReal) RF;
			}
		fclose(IOF);
		for (I = SS;I < CS;I++)
			Sweep[I] = (DLReal) 0.0;

		/* Convolving sweep and inverse */
		sputs("Sweep and inverse convolution...");
		rdft(CS,OouraRForward,Sweep);
		hcconvolve(Sweep,Inverse,CS);

		/* Impulse response recover */
		rdft(CS,OouraRBackward,Sweep);
		for (I = 0;I < CS;I++)
			Sweep[I] *= (DLReal) (2.0  / CS);

		/* Peak search */
		sputs("Finding impulse response peak value...");
		MP = 0;
		AMax = (DLReal) 0.0;
		for (I = 0;I < CS;I++)
			if ((DLReal) fabs(Sweep[I]) > AMax)
				{
					MP = I;
					AMax = (DLReal) fabs(Sweep[I]);
				}
		printf("Peak position: %d\n",MP);
		if (AMax > (DLReal) 0.0)
			printf("Peak value: %f (%f dB)\n",(double) AMax, (double) (20 * log10((double) AMax)));
		else
			printf("Peak value: %f (-inf dB)\n",(double) AMax);
		fflush(stdout);


		/* Writes output file */
		sputsp("Writing output file: ",OutFile);
		if ((IOF = fopen(OutFile,"wb")) == NULL)
			{
				perror("Unable to open output file");
				return 1;
			}
		for(I = 0;I < CL;I++)
			{
				RF = (float) Sweep[I];
				fwrite(&RF,sizeof(float),1,IOF);
			}
		fclose(IOF);

		/* Memory deallocation */
		free(Sweep);
		free(Inverse);

		/* Execution completed */
		sputs("Completed.");
		return 0;
	}
