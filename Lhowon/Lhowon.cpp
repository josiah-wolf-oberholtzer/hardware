#include "FrequencyShifter.h"
#include "Panner.h"
#include "daisysp.h"

class Lhowon {
public:
  Lhowon() {}
  ~Lhowon() {}

  void Init(float sample_rate) {
    feedback_level_ = 0.f;
    panner_a_.Init();
    panner_b_.Init();
    reverb_.Init(sample_rate);
    shifter_a_.Init(sample_rate);
    shifter_b_.Init(sample_rate);
    crossfader_.Init(daisysp::CROSSFADE_CPOW);
  }

  void Process(float in_1, float in_2, float *out) {
    buffer_[0] =
        daisysp::SoftClip((in_1 * !muted_) + (feedback_[0] * feedback_level_));
    buffer_[1] =
        daisysp::SoftClip((in_2 * !muted_) + (feedback_[1] * feedback_level_));

    panner_a_.Process(buffer_[0], &buffer_[2]);
    panner_b_.Process(buffer_[1], &buffer_[4]);

    buffer_[0] = crossfader_.Process(buffer_[2], buffer_[4]);
    buffer_[1] = crossfader_.Process(buffer_[3], buffer_[5]);

    reverb_.Process(buffer_[0], buffer_[1], &buffer_[2], &buffer_[3]);

    feedback_[0] = buffer_[0];
    feedback_[1] = buffer_[1];

    out[0] = buffer_[0];
    out[1] = buffer_[1];
  }

  void SetFrequencyShifterA(float value) {
    float frequency = (value - 0.5) * 1000.f;
    shifter_a_.SetFrequency(frequency);
  }

  void SetFrequencyShifterB(float value) {
    float frequency = (value - 0.5) * 1000.f;
    shifter_b_.SetFrequency(frequency);
  }

  void SetPannerA(float value) { panner_a_.SetPos(value); }

  void SetPannerB(float value) { panner_b_.SetPos(value); }

  void SetReverb(float value) {}

  void SetCrossfader(float value) { crossfader_.SetPos(value); }

  void Update(float panner_a_value,   // knob 0
              float fx_a_value,       // knob 1
              float crossfader_value, // knob 2
              float reverb_value,     // knob 3
              float panner_b_value,   // knob 4
              float fx_b_value,       // knob 5
              float feedback_value,   // knob 6
              bool muted              // gate
  ) {
    SetPannerA(panner_a_value);
    SetPannerB(panner_b_value);
    SetCrossfader(crossfader_value);
    SetFrequencyShifterA(fx_a_value);
    SetFrequencyShifterB(fx_b_value);
    SetReverb(reverb_value);
    feedback_level_ = feedback_value;
    muted_ = muted;
  }

private:
  bool muted_;
  daisysp::CrossFade crossfader_;
  daisysp::ReverbSc reverb_;
  float buffer_[8];
  float feedback_[2];
  float feedback_level_;
  planetbosch::FrequencyShifter shifter_a_;
  planetbosch::FrequencyShifter shifter_b_;
  planetbosch::Panner panner_a_;
  planetbosch::Panner panner_b_;
};
