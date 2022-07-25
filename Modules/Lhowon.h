#include "DSP.h"
#include "FrequencyShifter.h"
#include "Panner.h"
#include "XFade.h"

namespace planetbosch {

class Lhowon {
public:
  Lhowon() {}
  ~Lhowon() {}

  void Init(float sample_rate) {
    xfade_.Init(XFADE_CPOW);
    feedback_value_ = 0.f;
    frequency_shifter_a_.Init(sample_rate);
    frequency_shifter_b_.Init(sample_rate);
    panner_a_.Init();
    panner_b_.Init();
  }

  void Process(FloatFrame *frame) {
    buffer_[0] = frame->in[0];
    buffer_[1] = frame->in[1];
    panner_a_.Process(buffer_[0], &buffer_[2]);
    panner_b_.Process(buffer_[1], &buffer_[4]);
    buffer_[0] = xfade_.Process(buffer_[2], buffer_[4]);
    buffer_[1] = xfade_.Process(buffer_[3], buffer_[5]);
    /*
    buffer_[1] = frame->in[1];
    buffer_[0] =
        planetbosch::SoftClip((frame->in[0] * !muted_) + (feedback_[0] *
    feedback_value_)); buffer_[1] = planetbosch::SoftClip((frame->in[1] *
    !muted_) + (feedback_[1] * feedback_value_)); buffer_[0] =
    frequency_shifter_a_.Process(buffer_[0]); buffer_[1] =
    frequency_shifter_b_.Process(buffer_[1]); panner_a_.Process(buffer_[0],
    &buffer_[2]); panner_b_.Process(buffer_[1], &buffer_[4]); buffer_[0]   =
    xfade_.Process(buffer_[2], buffer_[4]); buffer_[1]   =
    xfade_.Process(buffer_[3], buffer_[5]);
    */
    frame->out[0] = buffer_[0];
    frame->out[1] = buffer_[1];
    /*
    feedback_[0] = buffer_[0];
    feedback_[1] = buffer_[1];
    */
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

  void SetXfade(float value) { xfade_.SetPos(value); }

  void Update(
      float panner_a_value, // knob 0
      float fx_a_value,     // knob 1
      float xfade_value,    // knob 2
      float reverb_value,   // knob 3
      float panner_b_value, // knob 4
      float fx_b_value,     // knob 5
      float feedback_value, // knob 6
      bool  muted           // gate
  ) {
    fx_a_value_   = fx_a_value;
    fx_b_value_   = fx_b_value;
    muted_        = muted;
    reverb_value_ = reverb_value;
    SetPannerA(panner_a_value);
    SetPannerB(panner_b_value);
    SetXfade(xfade_value);
    SetFrequencyShifterA(fx_a_value_);
    SetFrequencyShifterB(fx_b_value_);
  }

private:
  bool                          muted_;
  float                         buffer_[8];
  float                         feedback_[2];
  float                         feedback_value_;
  float                         fx_a_value_;
  float                         fx_b_value_;
  float                         reverb_value_;
  planetbosch::FrequencyShifter frequency_shifter_a_;
  planetbosch::FrequencyShifter frequency_shifter_b_;
  planetbosch::Panner           panner_a_;
  planetbosch::Panner           panner_b_;
  planetbosch::XFade            xfade_;
};

} // namespace planetbosch
