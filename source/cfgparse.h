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

#ifndef CfgParse_h
	#define CfgParse_h

	/* Configuration file type */
	typedef enum
		{
			CfgSimple,
			CfgSections
		}
	CfgFileType;

	/* Recognized configuration value types */
	typedef enum
		{
			CfgEnd,
			CfgBoolean,
			CfgString,
			CfgInt,
			CfgUInt,
			CfgLong,
			CfgULong,
			CfgFloat,
			CfgDouble
		}
	CfgValueType;

	/* Structure for parameters definition */
	typedef struct
		{
			char * PName;
			CfgValueType PType;
			void * PValue;
		}
	CfgParameter;

	#ifdef __cplusplus
		extern "C"
			{
	#endif

	/* Configuration parsing, return the number of parameters read or a negative value on error. */
	int CfgParse(const char * CfgFile,CfgParameter * CfgParameters,const CfgFileType FType);

	/* Free the memory allocated by CfgParse */
	void CfgFree(const CfgParameter * CfgParameters);

	/* Get the last error descrition */
	const char * CfgGetLastErrorDsc(void);

	#ifdef __cplusplus
		}
	#endif
#endif
