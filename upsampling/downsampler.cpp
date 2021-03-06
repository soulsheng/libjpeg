/*************************************************************************

    This project implements a complete(!) JPEG (10918-1 ITU.T-81) codec,
    plus a library that can be used to encode and decode JPEG streams. 
    It also implements ISO/IEC 18477 aka JPEG XT which is an extension
    towards intermediate, high-dynamic-range lossy and lossless coding
    of JPEG. In specific, it supports ISO/IEC 18477-3/-6/-7/-8 encoding.

    Copyright (C) 2012-2017 Thomas Richter, University of Stuttgart and
    Accusoft.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/
/*
** The actual downsampling implementation.
**
** $Id: downsampler.cpp,v 1.10 2017/05/18 09:28:43 thor Exp $
**
*/

/// Includes
#include "tools/environment.hpp"
#include "tools/rectangle.hpp"
#include "upsampling/downsamplerbase.hpp"
#include "upsampling/downsampler.hpp"
#include "std/string.hpp"
///

/// Downsampler::Downsampler
template<int sx,int sy>
Downsampler<sx,sy>::Downsampler(class Environ *env,ULONG width,ULONG height)
  : DownsamplerBase(env,sx,sy,width,height)
{
}
///

/// Downsampler::~Downsampler
template<int sx,int sy>
Downsampler<sx,sy>::~Downsampler(void)
{
}
///

/// Downsampler::DownsampleRegion
// The actual downsampling process. To be implemented by the actual
// classes inheriting from this. Coordinates are in the downsampled
// block domain the block indices. Requires an output buffer that
// will keep the downsampled data.
template<int sx,int sy>
void Downsampler<sx,sy>::DownsampleRegion(LONG bx,LONG by,LONG *buffer) const
{
  LONG ofs = (bx * sx) << 3; // first pixel in the buffer.
  LONG yfs = (by * sy) << 3; // first line.
  LONG *btop = buffer;
  LONG *bval = NULL;
  int lines = 0; // number of lines already managed to be summed up.
  int cnt   = 8; // number of output lines to go.
  struct Line *line = m_pInputBuffer;
  LONG y = m_lY;

  assert(yfs >= m_lY && yfs < m_lY + m_lHeight);

  //
  // Get the line.
  while(y < yfs) {
    line = line->m_pNext;
    y++;
  }
  assert(line);

  do {
    //
    // Start of a new line clear the entire output buffer.
    if (lines == 0) {
      // If there are no longer any lines to pull from, just replicate the last buffered line
      if (line || bval == NULL) {
        buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = buffer[5] = buffer[6] = buffer[7] = 0;
      } else {
        memcpy(buffer,bval, 8 * sizeof(LONG));
        bval -= 8;
        if (bval < btop)
          bval = btop;
      }
    }
    //
    // Still something in the image?
    if (line) {
      LONG *src = line->m_pData + ofs; // Current input buffer position.
      LONG *bp  = buffer;
      int i = 8; // pixel in the line

      do {
        switch(sx) { // actually this will be unrolled because it is a template
        case 4:
          *bp += src[3];
        case 3:
          *bp += src[2];
        case 2:
          *bp += src[1];
        case 1:
          *bp += src[0];
        }
        src += sx;
        bp++;
      } while(--i);
      //
      // Now continue with the next line if there is one, count the number of lines summed up.
      line   = line->m_pNext;
      lines++;
    }
    //
    // If we're done with this block, or if there are no more lines, normalize this
    // block and continue with the next.
    if (lines >= sy || line == NULL) {
      // Only if there is actually anything in the buffer, otherwise just leave it empty.
      if (lines) {
        WORD norm = lines * sx;
        if (norm > 1) {
          // Normalize the summed pixels.
          buffer[0] /= norm;buffer[1] /= norm;buffer[2] /= norm;buffer[3] /= norm;
          buffer[4] /= norm;buffer[5] /= norm;buffer[6] /= norm;buffer[7] /= norm;
        }
        // Keep the last valid (filled) buffer line
        bval     = buffer;
      }
      // Start the next buffer line.
      buffer    += 8;
      cnt--;
      lines      = 0;
    }
  } while(cnt);
}
///


/// Explicit instaciations
template class Downsampler<1,1>;
template class Downsampler<1,2>;
template class Downsampler<1,3>;
template class Downsampler<1,4>;
template class Downsampler<2,1>;
template class Downsampler<2,2>;
template class Downsampler<2,3>;
template class Downsampler<2,4>;
template class Downsampler<3,1>;
template class Downsampler<3,2>;
template class Downsampler<3,3>;
template class Downsampler<3,4>;
template class Downsampler<4,1>;
template class Downsampler<4,2>;
template class Downsampler<4,3>;
template class Downsampler<4,4>;
///
