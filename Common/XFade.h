#ifndef PLANETBOSCH_XFADE
#define PLANETBOSCH_XFADE

#include "DSP.h"
#include <math.h>

#define REALLYSMALLFLOAT 0.000001f

const float kCrossLogMin = logf(REALLYSMALLFLOAT);
const float kCrossLogMax = logf(1.0f);

namespace planetbosch {

enum {
  XFADE_LIN,
  XFADE_CPOW,
  XFADE_LOG,
  XFADE_EXP,
  XFADE_LAST,
};

class XFade {
public:
  XFade() {}
  ~XFade() {}

  inline void Init(int curve) {
    pos_   = 0.5f;
    curve_ = curve < XFADE_LAST ? curve : XFADE_LIN;
  }

  inline void Init() { Init(XFADE_LIN); }

  float Process(float &in1, float &in2) {
    float scalar_1, scalar_2;
    switch (curve_) {
    case XFADE_LIN:
      scalar_1 = pos_;
      return (in1 * (1.0f - scalar_1)) + (in2 * scalar_1);
    case XFADE_CPOW:
      scalar_1 = sinf(pos_ * HALFPI_F);
      scalar_2 = sinf((1.0f - pos_) * HALFPI_F);
      return (in1 * scalar_2) + (in2 * scalar_1);
    case XFADE_LOG:
      scalar_1 = expf(pos_ * (kCrossLogMax - kCrossLogMin) + kCrossLogMin);
      return (in1 * (1.0f - scalar_1)) + (in2 * scalar_1);
    case XFADE_EXP:
      scalar_1 = pos_ * pos_;
      return (in1 * (1.0f - scalar_1)) + (in2 * scalar_1);
    default:
      return 0;
    }
  }

  inline void SetPos(float pos) { pos_ = pos; }

  inline void SetCurve(uint8_t curve) { curve_ = curve; }

  inline float GetPos(float pos) { return pos_; }

  inline uint8_t GetCurve(uint8_t curve) { return curve_; }

private:
  float   pos_;
  uint8_t curve_;
};

} // namespace planetbosch

#endif
