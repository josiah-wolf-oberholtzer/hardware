#include "DSP.h"
#include "FrequencyShifter.h"
#include "XFade.h"
#include "daisysp.h"

namespace planetbosch {

typedef struct {
  float brightness;
  float damping;
  float frequency_a;
  float frequency_b;
  float fx;
  float mix;
  float nonlinearity;
} HrongirParams;

class MetaKarplus {
public:
  MetaKarplus() {}
  ~MetaKarplus() {}

  void Init(float sample_rate) { karplus_.Init(sample_rate); }

  float Process(float value) { return karplus_.Process(value); }

  void SetFrequency(float frequency) { karplus_.SetFreq(frequency); }

  void SetBrightness(float value) { karplus_.SetBrightness(value); }

  void SetDamping(float value) { karplus_.SetDamping(powf(value, 0.25)); }

  void SetNonLinearity(float value) {
    karplus_.SetNonLinearity((value - 0.5) * 2.f);
  }

private:
  daisysp::String karplus_;
};

class Hrongir {
public:
  Hrongir() {}
  ~Hrongir() {}

  void Init(float sample_rate) {
    xfade_.Init(XFADE_CPOW);
    for (int i = 0; i < 4; i++) {
      karplus_[i].Init(sample_rate);
    };
    for (int i = 0; i < 2; i++) {
      frequency_shifters_[i].Init(sample_rate);
    }
  }

  void Process(FloatFrame *frame) {
    for (int i = 0; i < 2; i++) {
      float in  = frame->in[i];
      float out = frequency_shifters_[i].Process(
          (karplus_[i].Process(in) + karplus_[i + 2].Process(in)) / 2.f
      );
      frame->out[i] = xfade_.Process(in, out);
    };
  }

  void SetFrequencyShifterFrequency(float value) {
    float base      = (value - 0.5) * 2.f;
    int   sign      = (0.f < base) - (base < 0.f);
    float frequency = 1000.f * (float)sign * powf(abs(base), 4.f);
    for (int i = 0; i < 2; i++) {
      frequency_shifters_[i].SetFrequency(frequency);
    }
  }

  void SetKarplusAFrequency(float frequency) {
    karplus_[0].SetFrequency(frequency);
    karplus_[1].SetFrequency(frequency);
  }

  void SetKarplusBFrequency(float frequency) {
    karplus_[2].SetFrequency(frequency);
    karplus_[3].SetFrequency(frequency);
  }

  void SetKarplusBrightness(float value) {
    for (int i = 0; i < 4; i++) {
      karplus_[i].SetBrightness(value);
    }
  }

  void SetKarplusDamping(float value) {
    for (int i = 0; i < 4; i++) {
      karplus_[i].SetDamping(value);
    }
  }

  void SetKarplusNonLinearity(float value) {
    for (int i = 0; i < 4; i++) {
      karplus_[i].SetNonLinearity(value);
    }
  }

  void SetMix(float mix) { xfade_.SetPos(mix); }

  void Update(HrongirParams *params) {
    SetFrequencyShifterFrequency(params->fx);
    SetKarplusAFrequency(daisysp::mtof(params->frequency_a * 64.f));
    SetKarplusBFrequency(daisysp::mtof(params->frequency_b * 64.f));
    SetKarplusBrightness(params->brightness);
    SetKarplusDamping(params->damping);
    SetKarplusNonLinearity(params->nonlinearity);
    SetMix(params->mix);
  }

private:
  MetaKarplus                   karplus_[4];
  planetbosch::FrequencyShifter frequency_shifters_[2];
  planetbosch::XFade            xfade_;
};

} // namespace planetbosch
