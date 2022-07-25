#include "DSP.h"
#include "FrequencyShifter.h"
#include "XFade.h"
#include "daisysp.h"

namespace planetbosch {

class Hrongir {
public:
  Hrongir() {}
  ~Hrongir() {}

  void Init(float sample_rate, float *buffers, size_t buffer_size) {
    xfade_.Init(XFADE_CPOW);
    for (int i = 0; i < 8; i++) {
      combs_[i].Init(sample_rate, &buffers[i * buffer_size], buffer_size);
    };
    for (int i = 0; i < 2; i++) {
      frequency_shifters_[i].Init(sample_rate);
    };
  }

  void Process(FloatFrame *frame) {
    float temp;
    for (int i = 0; i < 2; i++) {
      temp = frame->in[i];
      // temp = frequency_shifters_[i].Process(temp);
      temp = combs_[i].Process(temp) + combs_[i + 2].Process(temp) +
             combs_[i + 4].Process(temp) + combs_[i + 6].Process(temp);
      temp *= 0.1;
      frame->out[i] = xfade_.Process(frame->in[i], temp);
      // frame->out[i] = temp;
    };
  }

  void SetCombAFrequency(float frequency) {
    combs_[0].SetFreq(frequency);
    combs_[1].SetFreq(frequency);
  }

  void SetCombBFrequency(float frequency) {
    combs_[2].SetFreq(frequency);
    combs_[3].SetFreq(frequency);
  }

  void SetCombCFrequency(float frequency) {
    combs_[4].SetFreq(frequency);
    combs_[5].SetFreq(frequency);
  }

  void SetCombDFrequency(float frequency) {
    combs_[6].SetFreq(frequency);
    combs_[7].SetFreq(frequency);
  }

  void SetCombDecay(float seconds) {
    for (int i = 0; i < 8; i++) {
      combs_[i].SetRevTime(seconds);
    }
  }

  void SetFrequencyShifterFrequency(float frequency) {
    frequency_shifters_[0].SetFrequency(frequency);
    frequency_shifters_[1].SetFrequency(frequency);
  }

  void SetMix(float mix) { xfade_.SetPos(mix); }

  void Update(
      float mix_value_, float comb_a_value_, float comb_b_value_,
      float comb_c_value_, float decay_value_, float comb_d_value_,
      float fx_value_
  ) {
    SetCombAFrequency(daisysp::mtof(comb_a_value_ * 128.f));
    SetCombBFrequency(daisysp::mtof(comb_b_value_ * 128.f));
    SetCombCFrequency(daisysp::mtof(comb_c_value_ * 128.f));
    SetCombDFrequency(daisysp::mtof(comb_d_value_ * 128.f));
    SetCombDecay(powf(10.f, 2.f * (decay_value_ - 0.5)));
    SetFrequencyShifterFrequency(
        (float)((0.f < fx_value_) - (fx_value_ < 0.f)) *
        powf((fx_value_ - 0.5) * 2.f, 2.f) * 1000.f
    );
    SetMix(mix_value_);
  }

private:
  daisysp::Comb                 combs_[8];
  planetbosch::FrequencyShifter frequency_shifters_[2];
  planetbosch::XFade            xfade_;
};

} // namespace planetbosch
