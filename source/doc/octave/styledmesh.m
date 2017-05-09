## Copyright (C) 1996, 1997 John W. Eaton
##
## This file is part of Octave.
##
## Octave is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## Octave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, write to the Free
## Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
## 02110-1301, USA.

## -*- texinfo -*-
## function styledmesh (x, y, z, style)
## function styledmesh (z, style)
##
## Plot a mesh given the optional vectors x, y and a matrix z corresponding 
## to the x and y coordinates of the mesh. The string variable style
## defines the gnuplot style to use.

## Modified by Denis Sbragion from mesh to add a generic style parameter

function styledmesh (x, y, z, pstyle)
  if (nargin == 2)
    z = x';
    pstyle = y;
    if (ismatrix (z))      
      fname = tmpnam();
      eval([ "save -text " fname " z"]);
      __gnuplot_raw__([ "splot \"" fname "\" matrix " pstyle ";\n" ]);
    else
      error ("styledmesh: argument must be a matrix");
    endif
  elseif (nargin == 4)
    if (isvector (x) && isvector (y) && ismatrix (z))
      xlen = length (x);
      ylen = length (y);
      if (xlen == columns (z) && ylen == rows (z))        
        y = y(:)';
        len = xlen * ylen;
        cb = zeros(3, ylen);
        k = 1;
		   [ bf, fname, msg ] = mkstemp([ tmpnam() "-styledmeshplot-XXXXXX" ],true);
        for i = 1:ylen:len
          cb(1,1:ylen) = x(k) * ones(1,ylen);
				cb(2,1:ylen) = y;
				cb(3,1:ylen) = z(:,k)';
          fprintf(bf,"%e %e %e\n",cb);
          fprintf(bf,"\n");
				k++;
         endfor
        fclose(bf);
        __gnuplot_raw__([ "splot \"" fname "\"" pstyle ";\n" ]);
      else
        msg = "styledmesh: rows (z) must be the same as length (y) and";
        msg = sprintf ("%s\ncolumns (z) must be the same as length (x)", msg);
        error (msg);
      endif
    else
      error ("styledmesh: x and y must be vectors and z must be a matrix");
    endif
  else
    usage ("styledmesh (z)");
  endif

endfunction
