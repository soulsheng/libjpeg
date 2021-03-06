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
**
** This is the base class for all predictors, to be used for the
** lossless predictive mode. It performs the prediction, based
** on the mode, in a virtual subclass.
**
** $Id: predictorbase.hpp,v 1.3 2014/11/17 12:15:25 thor Exp $
**
*/

#ifndef CODESTREAM_PREDICTORBASE_HPP
#define CODESTREAM_PREDICTORBASE_HPP

/// Includes
#include "tools/environment.hpp"
#include "std/assert.hpp"
///

/// class PredictorBase
// This is the base class for all predictors, to be used for the
// lossless predictive mode. It performs the prediction, based
// on the mode, in a virtual subclass.
class PredictorBase : public JObject {
  //
public:
  //
  // Prediction directions
  enum PredictionMode {
    None     = 0,   // use zero as predicted value, only for differential modes.
    Left     = 1,   // predict from left
    Top      = 2,   // predict from above
    LeftTop  = 3,   // predict from left-top
    Linear   = 4,   // linear interpolation
    WeightA  = 5,   // linear interpolation with weight on A
    WeightB  = 6,   // linear interpolation with weight on B
    Diagonal = 7,   // diagonal interpolation, only A and B
    Neutral  = 8    // interpolation from neutral value.
  };
  //
#if ACCUSOFT_CODE 
  //
private:
  //
  // Predictors for the next sample to the right,
  // and for the next sample to the bottom.
  // These are part of the prediction state
  // machine that is advanced as soon as we move 
  // to one or the other direction.
  //
  // Next predictor to use when moved to the right.
  class PredictorBase *m_pNextRight;
  //
  // Next predictor to be used when moving down.
  class PredictorBase *m_pNextDown;
  //
  // Create a predictor of the given preshift and mode.
  template<PredictionMode mode>
  static PredictorBase *CreatePredictor(class Environ *env,UBYTE preshift,LONG neutral);
  //
  // Create a predictor of the given preshift and mode.
  static PredictorBase *CreatePredictor(class Environ *env,PredictionMode mode,UBYTE preshift,
                                              LONG neutral);
  //
protected:
  //
  PredictorBase(void)
  : m_pNextRight(NULL), m_pNextDown(NULL)
  { }
  //
public:
  //
  // Create a prediction chain for the given neutral value and
  // the given prediction mode. The mode "none" is used to
  // indicate the differential mode.
  // This requires an array of four predictors to be used for
  // life-time management. The first element is the initial predictor.
  static void CreatePredictorChain(class Environ *env,
                                   class PredictorBase *chain[4],PredictionMode mode,UBYTE preshift,
                                   LONG neutral);
  //
  virtual ~PredictorBase(void)
  { }
  //
  // Predict a sample value depending on the prediction mode.
  // lp is the pointer to the current line, pp the one to the previous line.
  // v is the decoded sample value.
  virtual LONG DecodeSample(LONG v,const LONG *lp,const LONG *pp) const = 0;
  //
  // This is the inverse, it creates the sample to encode from the
  // present samples.
  virtual LONG EncodeSample(const LONG *lp,const LONG *pp) const = 0;
  //
  // Return the next predictor when moving one sample to the right.
  class PredictorBase *MoveRight(void) const
  {
    assert(m_pNextRight);
    return m_pNextRight;
  }
  //
  // Return the next predictor when moving one samle down.
  class PredictorBase *MoveDown(void) const
  {
    assert(m_pNextDown);
    return m_pNextDown;
  }
#endif
};
///

///
#endif
