#include <math.h>

using namespace daisysp;

class Panner {
public:
    Panner() {}
    ~Panner() {}

    inline void Init() { pos_ = 0.5f; }

    void Process(float &in, float* out) {
        out[0] = sinf((1.0f - pos_) * HALFPI_F);
        out[1] = sinf(pos_ * HALFPI_F);
    }

    inline void SetPos(float pos) { pos_ = pos; }

private:
    float pos_;
};
