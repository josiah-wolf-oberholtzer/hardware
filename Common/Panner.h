#include <math.h>

using namespace daisysp;

namespace planetbosch {

class Panner {
public:
    Panner() {}
    ~Panner() {}

    inline void Init() { pos_ = 0.5f; }

    void Process(float in, float* out) {
        out[0] = in * sinf((1.0f - pos_) * HALFPI_F);
        out[1] = in * sinf(pos_ * HALFPI_F);
    }

    inline void SetPos(float pos) { pos_ = pos; }

private:
    float pos_;
};

}
