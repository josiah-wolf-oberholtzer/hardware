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

  void Init(float *buffer, float sample_rate) {
    engine_.Init(buffer, sample_rate);
    engine_.SetLFOFrequency(LFO_1, 0.5f, sample_rate);
    engine_.SetLFOFrequency(LFO_2, 0.4f, sample_rate);
  }

  void Process(float in_l, float in_r, float* out_l, float* out_r) {
    typedef
        E::Reserve<126, E::Reserve<180, E::Reserve<269, E::Reserve<444,
        E::Reserve<151, E::Reserve<205, E::Reserve<245, E::Reserve<405,
        E::Reserve<1753, E::Reserve<2038, E::Reserve<4511,
        E::Reserve<1913, E::Reserve<1763, E::Reserve<4782
        >>>>>>>>>>>>>> Memory;

    E::DelayLine<Memory, 0> apl1;
    E::DelayLine<Memory, 1> apl2;
    E::DelayLine<Memory, 2> apl3;
    E::DelayLine<Memory, 3> apl4;
    E::DelayLine<Memory, 4> apr1;
    E::DelayLine<Memory, 5> apr2;
    E::DelayLine<Memory, 6> apr3;
    E::DelayLine<Memory, 7> apr4;
    E::DelayLine<Memory, 8> dap1a;
    E::DelayLine<Memory, 9> dap1b;
    E::DelayLine<Memory, 10> del1;
    E::DelayLine<Memory, 11> dap2a;
    E::DelayLine<Memory, 12> dap2b;
    E::DelayLine<Memory, 13> del2;

    E::Context              c;

    const float amount = amount_;
    const float gain   = input_gain_;
    const float kap    = diffusion_;
    const float khp    = hp_;
    const float klp    = lp_;
    const float krt    = reverb_time_;
    float       aprout = 0.0f;
    float       aplout = 0.0f;
    float       hp_1   = hp_decay_1_;
    float       lp_1   = lp_decay_1_;
    float       hp_2   = hp_decay_1_;
    float       lp_2   = lp_decay_1_;
    float       wet;

    engine_.Start(&c);

    c.Load(0);
    c.Interpolate(apl1, 10.0f, LFO_1, 60.0f, 1.0f);
    c.Write(apl1, 100, 0.0f);
    c.Read(in_l, gain);
    c.Read(apl1, -1, kap);
    c.WriteAllPass(apl1, -kap);
    c.Read(apl2, -1, kap);
    c.WriteAllPass(apl2, -kap);
    c.Read(apl3, -1, kap);
    c.WriteAllPass(apl3, -kap);
    c.Read(apl4, -1, kap);
    c.WriteAllPass(apl4, -kap);
    c.Hp(hp_1, khp);
    c.Lp(lp_1, klp);
    c.Write(aplout);

    c.Load(0);
    c.Interpolate(apr1, 10.0f, LFO_2, 60.0f, 1.0f);
    c.Write(apr1, 100, 0.0f);
    c.Read(in_r, gain);
    c.Read(apr1, -1, kap);
    c.WriteAllPass(apr1, -kap);
    c.Read(apr2, -1, kap);
    c.WriteAllPass(apr2, -kap);
    c.Read(apr3, -1, kap);
    c.WriteAllPass(apr3, -kap);
    c.Read(apr4, -1, kap);
    c.WriteAllPass(apr4, -kap);
    c.Hp(hp_2, khp);
    c.Lp(lp_2, klp);
    c.Write(aprout);

    c.Load(aplout);
    c.Interpolate(del2, 4680.0f, LFO_2, 100.0f, krt);
    c.Read(del2, -1, krt);
    c.Read(dap1a, -1, -kap);
    c.WriteAllPass(dap1a, kap);
    c.Read(dap1b, -1, kap);
    c.WriteAllPass(dap1b, -kap);
    c.Write(del1, 2.0f);
    c.Write(wet, 0.0f);

    out_l[0] = (wet - in_l) * amount;

    c.Load(aprout);
    c.Interpolate(del1, 4400.0f, LFO_1, 100.0f, krt);
    c.Read(del1, -1, krt);
    c.Read(dap2a, -1, kap);
    c.WriteAllPass(dap2a, -kap);
    c.Read(dap2b, -1, -kap);
    c.WriteAllPass(dap2b, kap);
    c.Write(del2, 2.0f);
    c.Write(wet, 0.0f);

    out_r[0] = (wet - in_r) * amount;

    hp_decay_1_ = hp_1;
    lp_decay_1_ = lp_1;
    hp_decay_2_ = hp_2;
    lp_decay_2_ = lp_2;
  }

  inline void SetAmount(float amount) { amount_ = amount; }

  inline void SetInputGain(float input_gain) { input_gain_ = input_gain; }

  inline void SetTime(float reverb_time) { reverb_time_ = reverb_time; }

  inline void SetDiffusion(float diffusion) { diffusion_ = diffusion; }

  inline void SetLp(float lp) { lp_ = lp; }

  inline void SetHp(float hp) { hp_ = hp; }

private:
  float                   amount_;
  float                   diffusion_;
  float                   hp_;
  float                   hp_decay_1_;
  float                   hp_decay_2_;
  float                   input_gain_;
  float                   lp_;
  float                   lp_decay_1_;
  float                   lp_decay_2_;
  float                   reverb_time_;
  typedef FxEngine<16384> E;
  E                       engine_;
};

} // namespace planetbosch
