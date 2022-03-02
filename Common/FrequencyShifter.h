#include "daisysp.h"
#include <math.h>

using namespace daisysp;

namespace planetbosch {

class FrequencyShifter {
public:
  FrequencyShifter() {}
  ~FrequencyShifter() {}

  inline void Init(float sample_rate) {
    double gamma_const = (15.0 * PI_F) / sample_rate;
    double gamma[12]   = {
          gamma_const * 0.3609f,   gamma_const * 2.7412f,
          gamma_const * 11.1573f,  gamma_const * 44.7581f,
          gamma_const * 179.6242f, gamma_const * 798.4578f,
          gamma_const * 1.2524f,   gamma_const * 5.5671f,
          gamma_const * 22.3423f,  gamma_const * 89.6271f,
          gamma_const * 364.7914f, gamma_const * 2770.1114f,
    };
    for (int i = 0; i < 12; ++i) {
      coefs_[i] = (gamma[i] - 1.f) / (gamma[i] + 1.f);
    }
    phase_       = 0.f;
    frequency_   = 0.f;
    sample_rate_ = sample_rate;
    Process(0.f);
  }

  float Process(float in) {
    float ay[12];
    float y0[12];
    // Run Hilbert filter
    for (int i = 0; i < 12; ++i) {
      y0[i]  = in - coefs_[i] * y1_[i];
      ay[i]  = coefs_[i] * y0[i] + 1.f * y1_[i];
      y1_[i] = y0[i];
      in     = ay[i]; // ay[i] becomes the input of the next loop
    }
    // Increment phase
    phase_ += frequency_ * TWOPI_F / sample_rate_;
    if (phase > TWOPI_F) {
        phase -= TWOPI_F;
    } else if (phase < 0.f) {
        phase += TWOPI_F;
    }
    // Output
    return (ay[5] * cosf(phase_)) + (ay[11] * sinf(phase_));
  }

  inline void SetFrequency(float frequency) { frequency_ = frequency; }

private:
  double coefs_[12];
  float  phase_;
  float  frequency_;
  float  y1_[12];
  float  sample_rate_;
};

} // namespace planetbosch
