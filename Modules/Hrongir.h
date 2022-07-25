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
  float mix;
  float nonlinearity;
  float fx;
} HrongirParams;

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
      frame->out[i] = (karplus_[i].Process(frame->in[i]) +
                       karplus_[i + 2].Process(frame->in[i])) *
                      0.5;
      frame->out[i] = xfade_.Process(frame->in[i], frame->out[i]);
      frame->out[i] = frequency_shifters_[i].Process(frame->out[i]);
    };
  }

  void SetFrequencyShifterFrequency(float value) {
    float base      = (value - 0.5) * 2.f;
    int   sign      = (0.f < base) - (base < 0.f);
    float frequency = 1000.f * (float)sign * powf(abs(base), 4.f);
    frequency_shifters_[0].SetFrequency(frequency);
    frequency_shifters_[1].SetFrequency(frequency);
  }

  void SetKarplusAFrequency(float frequency) {
    karplus_[0].SetFreq(frequency);
    karplus_[1].SetFreq(frequency);
  }

  void SetKarplusBFrequency(float frequency) {
    karplus_[2].SetFreq(frequency);
    karplus_[3].SetFreq(frequency);
  }

  void SetKarplusBrightness(float value) {
    for (int i = 0; i < 4; i++) {
      karplus_[i].SetBrightness(value);
    }
  }

  void SetKarplusDamping(float value) {
    float scaled = powf(value, 0.25);
    for (int i = 0; i < 4; i++) {
      karplus_[i].SetDamping(scaled);
    }
  }

  void SetKarplusNonLinearity(float value) {
    float base      = (value - 0.5) * 2.f;
    int   sign      = (0.f < base) - (base < 0.f);
    float scaled    = (float)sign * powf(abs(base), 0.25);
    for (int i = 0; i < 4; i++) {
      karplus_[i].SetNonLinearity(scaled);
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
  daisysp::String               karplus_[4];
  planetbosch::XFade            xfade_;
  planetbosch::FrequencyShifter frequency_shifters_[2];
};

} // namespace planetbosch
