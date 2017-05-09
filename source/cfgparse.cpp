/****************************************************************************

    DRC: Digital Room Correction
    Copyright (C) 2002, 2003 Denis Sbragion

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

/* Configuration file parsing utilities */

/* Inclusions */
#include "cfgparse.h"
#include "boolean.h"
#include "minIni/minIni.h"
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

/* Memory leaks debugger */
#ifdef DebugMLeaks
	#include "debug_new.h"
#endif

/* Configuration structure initialization */
static int CfgInit(const CfgParameter * CfgParameters)
	{
		int I;

		/* Cicle over the parameters list */
		I = 0;
		while (CfgParameters[I].PType != CfgEnd)
			{
				/* Check the parameter type */
				switch (CfgParameters[I].PType)
					{
						case CfgEnd:
							/* Should be impossible */
						break;
						case CfgBoolean:
							*((Boolean *) CfgParameters[I].PValue) = False;
						break;
						case CfgString:
							*((char **) CfgParameters[I].PValue) = NULL;
						break;
						case CfgInt:
							*((int *) CfgParameters[I].PValue) = (int) 0;
						break;
						case CfgUInt:
							*((unsigned int *) CfgParameters[I].PValue) = (unsigned int) 0;
						break;
						case CfgLong:
							*((long *) CfgParameters[I].PValue) = (long) 0;
						break;
						case CfgULong:
							*((unsigned long *) CfgParameters[I].PValue) = (unsigned long) 0;
						break;
						case CfgFloat:
							*((float *) CfgParameters[I].PValue) = (float) 0.0;
						break;
						case CfgDouble:
							*((double *) CfgParameters[I].PValue) = (double) 0.0;
						break;
					}

				/* Next parameter */
				I++;
			}

		/* Returns the parameter count */
		return I;
	}

/* Last error descritpion length */
#define CfgLastErrorDscLen 512

/* Last error descritpion */
static char CfgLastErrorDsc[CfgLastErrorDscLen];

/* Structure used to pass data to the callback procedure */
typedef struct
	{
		CfgFileType FType;
		CfgParameter * CfgParameters;
		int PCount;
		int PParsed;
		Boolean PError;
	}
CfgCallbackInfoStruct;

/* Parameter search function */
static int CfgPSearch(CfgParameter * CfgParameters, char * PKey)
	{
		int I;

		/* Cicle over the parameters list */
		I = 0;
		while (CfgParameters[I].PType != CfgEnd)
			{
				if (strcmp(CfgParameters[I].PName,PKey) == 0)
					return I;

				/* Next parameter */
				I++;
			}

		/* Parameter not found */
		return I;
	}

/* Configuration file parsing callback */
static int CfgCallback(const char * Section, const char * Key, const char * Value, const void * UserData)
	{
		/* Current callback informations */
		CfgCallbackInfoStruct * CCI = (CfgCallbackInfoStruct *) UserData;

		/* Parameter search tmp buffer */
		char PKey[INI_BUFFERSIZE];

		/* Current parameter index */
		int PI;

		/* Temporary variables */
		char TB;
		long int TLI;
		unsigned long int TLU;
		float TF;
		double TD;
		char * TEP;

		/* Check if sections are allowed */
		if (CCI->FType == CfgSimple)
			{
				/* Check that the section is empty */
				if (Section[0] != '\0')
					{
						CCI->PError = True;
						snprintf(CfgLastErrorDsc,CfgLastErrorDscLen,"Unexpected section '%s'.",Section);
						return (int) False;
					}

				/* Creates the search key */
				snprintf(PKey,INI_BUFFERSIZE,"%s",Key);
			}
		else
			/* Creates the search key */
			snprintf(PKey,INI_BUFFERSIZE,"%s:%s",Section,Key);

		/* Search the parameter */
		PI = CfgPSearch(CCI->CfgParameters,PKey);

		/* Check if the parameter has been found */
		if (PI == CCI->PCount)
			{
				CCI->PError = True;
				snprintf(CfgLastErrorDsc,CfgLastErrorDscLen,"Unknown parameter '%s'.",PKey);
				return (int) False;
			}

		/* Parameter conversion */
		switch (CCI->CfgParameters[PI].PType)
			{
				case CfgEnd:
					/* Should be impossible */
				break;

				case CfgBoolean:
					TB = toupper(Value[0]);
					if (TB == 'T' || TB == 'Y' || TB == '1')
						{
							*((Boolean *) (CCI->CfgParameters[PI].PValue)) = True;
							return (int) True;
						}
					if (TB == 'F' || TB == 'N' || TB == '0')
						{
							*((Boolean *) (CCI->CfgParameters[PI].PValue)) = False;
							return (int) True;
						}
					CCI->PError = True;
					snprintf(CfgLastErrorDsc,CfgLastErrorDscLen,"Invalid boolean parameter '%s': %s.",PKey,Value);
					return (int) False;
				break;

				case CfgString:
					if((*((char **) (CCI->CfgParameters[PI].PValue)) = (char *) malloc(sizeof(char) * (1 + strlen(Value)))) == NULL)
						{
							CCI->PError = True;
							snprintf(CfgLastErrorDsc,CfgLastErrorDscLen,"Parameter '%s' memory allocation failed.",PKey);
							return (int) False;
						}
					strcpy(*((char **) (CCI->CfgParameters[PI].PValue)),Value);
				break;

				case CfgInt:
					errno = 0;
					TLI = strtol(Value,&TEP,0);
					if (errno != 0 || Value[0] == '\0' || *TEP != '\0' || TLI > INT_MAX || TLI < INT_MIN)
						{
							CCI->PError = True;
							snprintf(CfgLastErrorDsc,CfgLastErrorDscLen,"Invalid integer parameter '%s': %s.",PKey,Value);
							return (int) False;
						}
					*((int *) (CCI->CfgParameters[PI].PValue)) = (int) TLI;
				break;

				case CfgUInt:
					errno = 0;
					TLU = strtoul(Value,&TEP,0);
					if (errno != 0 || Value[0] == '\0' || *TEP != '\0' || TLU > UINT_MAX)
						{
							CCI->PError = True;
							snprintf(CfgLastErrorDsc,CfgLastErrorDscLen,"Invalid integer parameter '%s': %s.",PKey,Value);
							return (int) False;
						}
					*((unsigned int *) (CCI->CfgParameters[PI].PValue)) = (unsigned int) TLU;
				break;

				case CfgLong:
					errno = 0;
					TLI = strtol(Value,&TEP,0);
					if (errno != 0 || Value[0] == '\0' || *TEP != '\0')
						{
							CCI->PError = True;
							snprintf(CfgLastErrorDsc,CfgLastErrorDscLen,"Invalid integer parameter '%s': %s.",PKey,Value);
							return (int) False;
						}
					*((long *) (CCI->CfgParameters[PI].PValue)) = (long) TLI;
				break;

				case CfgULong:
					errno = 0;
					TLU = strtoul(Value,&TEP,0);
					if (errno != 0 || Value[0] == '\0' || *TEP != '\0' || TLU > UINT_MAX)
						{
							CCI->PError = True;
							snprintf(CfgLastErrorDsc,CfgLastErrorDscLen,"Invalid integer parameter '%s': %s.",PKey,Value);
							return (int) False;
						}
					*((unsigned long *) (CCI->CfgParameters[PI].PValue)) = (unsigned long) TLU;
				break;

				case CfgFloat:
					errno = 0;
					TF = strtof(Value,&TEP);
					if (errno != 0 || Value[0] == '\0' || *TEP != '\0')
						{
							CCI->PError = True;
							snprintf(CfgLastErrorDsc,CfgLastErrorDscLen,"Invalid floating point parameter '%s': %s.",PKey,Value);
							return (int) False;
						}
					*((float *) (CCI->CfgParameters[PI].PValue)) = (float) TF;
				break;

				case CfgDouble:
					errno = 0;
					TD = strtod(Value,&TEP);
					if (errno != 0 || Value[0] == '\0' || *TEP != '\0')
						{
							CCI->PError = True;
							snprintf(CfgLastErrorDsc,CfgLastErrorDscLen,"Invalid floating point parameter '%s': %s.",PKey,Value);
							return (int) False;
						}
					*((double *) (CCI->CfgParameters[PI].PValue)) = (double) TD;
				break;
			}

		/* Increase the number of parameters parsed */
		CCI->PParsed++;

		/* Conversion completed */
		return (int) True;
	}

/* Configuration parsing, return the number of parameters read or a negative value on error. */
int CfgParse(const char * CfgFile,CfgParameter * CfgParameters,const CfgFileType FType)
	{
		/* Callback information structure */
		CfgCallbackInfoStruct CBInfo;

		/* Set the callback info structure */
		CBInfo.FType = FType;
		CBInfo.CfgParameters = CfgParameters;
		CBInfo.PParsed = 0;
		CBInfo.PError = False;

		/* Parameter initialization and count */
		CBInfo.PCount = CfgInit(CfgParameters);

		/* Reset the error description */
		CfgLastErrorDsc[0] = '\0';

		/* Parse the configuration file */
		errno = 0;
		if (ini_browse(CfgCallback,(const void *) &CBInfo,CfgFile) == (int) False)
			{
				snprintf(CfgLastErrorDsc,CfgLastErrorDscLen,"Configuration file parsing error: %s.",strerror(errno));
				return -1;
			}

		/* Check if there was a paring error */
		if (CBInfo.PError == True)
			return -1;

		/* Parsing completed */
		return CBInfo.PParsed;
	}


/* Free the memory allocated by CfgParse */
void CfgFree(const CfgParameter * CfgParameters)
	{
		int I;

		/* Cicle over the parameters list */
		I = 0;
		while (CfgParameters[I].PType != CfgEnd)
			{
				/* Check the parameter type */
				if (CfgParameters[I].PType == CfgString && *((char **) CfgParameters[I].PValue) != NULL)
					free(*((char **) CfgParameters[I].PValue));

				/* Next parameter */
				I++;
			}
	}

/* Get the last error descrition */
const char * CfgGetLastErrorDsc(void)
	{
		return CfgLastErrorDsc;
	}
