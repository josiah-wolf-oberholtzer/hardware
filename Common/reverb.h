// Copyright 2014 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Reverb.

#include "fx_engine.h"

using namespace daisysp;

namespace planetbosch {

class Reverb {
public:
  Reverb() {}
  ~Reverb() {}

  void Init(float *buffer) {
    engine_.Init(buffer);
    engine_.SetLFOFrequency(LFO_1, 0.5f / 32000.0f);
    engine_.SetLFOFrequency(LFO_2, 0.3f / 32000.0f);
    lp_        = 0.7f;
    diffusion_ = 0.625f;
  }

  float Process(float in) {
    // This is the Griesinger topology described in the Dattorro paper
    // (4 AP diffusers on the input, then a loop of 2x 2AP+1Delay).
    // Modulation is applied in the loop of the first diffuser AP for additional
    // smearing; and to the two long delays for a slow shimmer/chorus effect.
    typedef E::Reserve<
        113,
        E::Reserve<
            162,
            E::Reserve<
                241, E::Reserve<
                         399, E::Reserve<
                                  1653, E::Reserve<2038, E::Reserve<3411>>>>>>>
                            Memory;
    E::DelayLine<Memory, 0> ap1;
    E::DelayLine<Memory, 1> ap2;
    E::DelayLine<Memory, 2> ap3;
    E::DelayLine<Memory, 3> ap4;
    E::DelayLine<Memory, 4> dap1a;
    E::DelayLine<Memory, 5> dap1b;
    E::DelayLine<Memory, 6> del1;
    E::Context              c;

    const float kap    = diffusion_;
    const float klp    = lp_;
    const float krt    = reverb_time_;
    const float amount = amount_;
    const float gain   = input_gain_;
    float       lp_1   = lp_decay_1_;
    float       wet;
    float       apout = 0.0f;

    engine_.Start(&c);

    // Smear AP1 inside the loop.
    c.Interpolate(ap1, 10.0f, LFO_1, 60.0f, 1.0f);
    c.Write(ap1, 100, 0.0f);

    c.Read(in, gain);

    // Diffuse through 4 allpasses.
    c.Read(ap1, -1, kap);
    c.WriteAllPass(ap1, -kap);
    c.Read(ap2, -1, kap);
    c.WriteAllPass(ap2, -kap);
    c.Read(ap3, -1, kap);
    c.WriteAllPass(ap3, -kap);
    c.Read(ap4, -1, kap);
    c.WriteAllPass(ap4, -kap);
    c.Write(apout);

    // Main reverb loop.
    c.Load(apout);
    c.Interpolate(del1, 4680.0f, LFO_2, 100.0f, krt);
    c.Lp(lp_1, klp);
    c.Read(dap1a, -1, -kap);
    c.WriteAllPass(dap1a, kap);
    c.Read(dap1b, -1, kap);
    c.WriteAllPass(dap1b, -kap);
    c.Write(del1, 2.0f);
    c.Write(wet, 0.0f);

    lp_decay_1_ = lp_1;

    return (wet - in) * amount;
  }

  inline void SetAmount(float amount) { amount_ = amount; }

  inline void SetInput_gain(float input_gain) { input_gain_ = input_gain; }

  inline void SetTime(float reverb_time) { reverb_time_ = reverb_time; }

  inline void SetDiffusion(float diffusion) { diffusion_ = diffusion; }

  inline void SetLp(float lp) { lp_ = lp; }

private:
  float                   amount_;
  float                   diffusion_;
  float                   input_gain_;
  float                   lp_;
  float                   lp_decay_1_;
  float                   reverb_time_;
  typedef FxEngine<16384> E;
  E                       engine_;
};

} // namespace planetbosch
