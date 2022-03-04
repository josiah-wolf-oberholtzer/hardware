#include "daisysp.h"
#include "diffuser.h"
#include "frequency_shifter.h"
#include "panner.h"

class Lhowon {
public:
  Lhowon() {}
  ~Lhowon() {}

  void Init(float sample_rate) {
    crossfade_.Init(daisysp::CROSSFADE_CPOW);
    diffuser_a_.Init(sample_rate);
    diffuser_b_.Init(sample_rate);
    feedback_value_ = 0.f;
    frequency_shifter_a_.Init(sample_rate);
    frequency_shifter_b_.Init(sample_rate);
    panner_a_.Init();
    panner_b_.Init();
    slew_coeff_ = 0.05f;
  }

  void Process(float in_1, float in_2, float *out) {
    buffer_[0] =
        daisysp::SoftClip((in_1 * !muted_) + (feedback_[0] * feedback_value_));
    buffer_[1] =
        daisysp::SoftClip((in_2 * !muted_) + (feedback_[1] * feedback_value_));
    buffer_[0] = frequency_shifter_a_.Process(buffer_[0]);
    buffer_[1] = frequency_shifter_b_.Process(buffer_[1]);
    panner_a_.Process(buffer_[0], &buffer_[2]);
    panner_b_.Process(buffer_[1], &buffer_[4]);
    buffer_[0] = crossfade_.Process(buffer_[2], buffer_[4]);
    buffer_[1] = crossfade_.Process(buffer_[3], buffer_[5]);
    buffer_[0] = diffuser_a_.Process(buffer_[0]);
    buffer_[1] = diffuser_a_.Process(buffer_[1]);
    feedback_[0] = buffer_[0];
    feedback_[1] = buffer_[1];
    out[0]       = buffer_[0];
    out[1]       = buffer_[1];
  }

  void SetFrequencyShifterA(float value) {
    int   sign      = (0.f < value) - (value < 0.f);
    float scale     = powf((value - 0.5) * 2.f, 2.f);
    float frequency = 1000.f * (float)sign * scale;
    frequency_shifter_a_.SetFrequency(frequency);
  }

  void SetFrequencyShifterB(float value) {
    int   sign      = (0.f < value) - (value < 0.f);
    float scale     = powf((value - 0.5) * 2.f, 2.f);
    float frequency = 1000.f * (float)sign * scale;
    frequency_shifter_b_.SetFrequency(frequency);
  }

  void SetPannerA(float value) { panner_a_.SetPos(value); }

  void SetPannerB(float value) { panner_b_.SetPos(value); }

  void SetReverb(float value) {
      float time;
      float amount = abs((value - 0.5f) * 2.0f);
      if (value > 0.5) {
          time = (value - 0.5f) * 2.0f;
      } else {
          time = (0.5f - value) * 2.0f;
      }
      diffuser_a_.SetAmount(amount);
      diffuser_b_.SetAmount(amount);
      diffuser_a_.SetTime(time);
      diffuser_b_.SetTime(time);
  }

  void SetCrossfader(float value) { crossfade_.SetPos(value); }

  void Update(
      float panner_a_value,  // knob 0
      float fx_a_value,      // knob 1
      float crossfade_value, // knob 2
      float reverb_value,    // knob 3
      float panner_b_value,  // knob 4
      float fx_b_value,      // knob 5
      float feedback_value,  // knob 6
      bool  muted            // gate
  ) {
    fx_a_value_ = fx_a_value;
    fx_b_value_ = fx_b_value;
    SetPannerA(panner_a_value_);
    SetPannerB(panner_b_value_);
    SetCrossfader(crossfade_value_);
    SetFrequencyShifterA(fx_a_value_);
    SetFrequencyShifterB(fx_b_value_);
    SetReverb(reverb_value_);
    muted_ = muted;
  }

private:
  bool                          muted_;
  daisysp::CrossFade            crossfade_;
  float                         buffer_[8];
  float                         crossfade_value_;
  float                         feedback_[2];
  float                         feedback_value_;
  float                         fx_a_value_;
  float                         fx_b_value_;
  float                         panner_a_value_;
  float                         panner_b_value_;
  float                         reverb_value_;
  float                         slew_coeff_;
  planetbosch::Diffuser         diffuser_a_;
  planetbosch::Diffuser         diffuser_b_;
  planetbosch::FrequencyShifter frequency_shifter_a_;
  planetbosch::FrequencyShifter frequency_shifter_b_;
  planetbosch::Panner           panner_a_;
  planetbosch::Panner           panner_b_;
};
