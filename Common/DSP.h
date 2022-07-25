#ifndef PLANETBOSCH_DSP
#define PLANETBOSCH_DSP

namespace planetbosch {

#define PI_F 3.1415927410125732421875f
#define TWOPI_F (2.0f * PI_F)
#define HALFPI_F (PI_F * 0.5f)

/** Soft Limiting function ported extracted from pichenettes/stmlib */
inline float SoftLimit(float x) {
  return x * (27.f + x * x) / (27.f + 9.f * x * x);
}

/** Soft Clipping function extracted from pichenettes/stmlib */
inline float SoftClip(float x) {
  if (x < -3.0f)
    return -1.0f;
  else if (x > 3.0f)
    return 1.0f;
  else
    return SoftLimit(x);
}

typedef struct {
  float in[2];
  float out[2];
} FloatFrame;

} // namespace planetbosch

#endif
