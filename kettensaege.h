#pragma once

#include "daisysp.h"

// A "supersaw" oscillator that is, under the covers, 7 oscillators centered on
// a fundamental frequency. They are detuned based on a detune factor. They are
// also mixed in different ways depending on the mix factor.
//
// This is inspired by Roland JP-8000 and JP-8080 and documented at
// https://api.semanticscholar.org/CorpusID:13616851
class Kettensaege {
 public:
  void Init(float sample_rate);

  // Sets the fundamental frequency
  void SetFreq(float f);

  // Sets the amplitude (0-1) of the composite waveform
  void SetAmp(float amp);

  // Sets the amount of detune (0-1).  This is the spread of the frequency of
  // the side oscillators from the fundamental frequency.
  void SetDetune(float detune);

  // Sets both detune and freq. Combined to reduce recalc cost.
  void SetDetuneAndFreq(float detune, float freq);

  // Sets the mix factor (0-1).  This determines the amplitude of the
  // fundamental oscillator vs. the side oscillators.  At 0 the side oscillators
  // are very quiet. At 1 they are all equal.
  void SetMix(float mix);

  // Processes the waveform to be generated, returning one sample. This should
  // be called once per sample period.
  float ProcessL();
  float ProcessR();

  // Reset the cycle for all oscillators.  This is often done at the start of a
  // note.
  void Reset();

  // Return the effective detune amount
  float DetuneScale() { return detune_scale_; }

 private:
  void UpdateFreqs();
  void UpdateAmps();

  daisysp::Oscillator osc[7];
  float               freq_;
  float               amp_;
  float               detune_;
  float detune_scale_;
  float               mix_;
};