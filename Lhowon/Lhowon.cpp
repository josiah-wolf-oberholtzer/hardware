#include "daisysp.h"
#include "Panner.h"

class Lhowon {
public:
    Lhowon() {}
    ~Lhowon() {}

    void Init(float sample_rate) {
        feedback_level_ = 0.f;
        panner_a_.Init();
        panner_b_.Init();
        xfader_.Init(daisysp::CROSSFADE_CPOW);
    }

    inline void Process(float in_1, float in_2, float* out) {
        if (!muted_) {
            buffer_[0] = daisysp::SoftClip(in_1 + (feedback_[0] * feedback_level_));
            buffer_[1] = daisysp::SoftClip(in_2 + (feedback_[1] * feedback_level_));
        } else {
            buffer_[0] = daisysp::SoftClip(feedback_[0] * feedback_level_);
            buffer_[1] = daisysp::SoftClip(feedback_[1] * feedback_level_);
        }
        panner_a_.Process(buffer_[0], panned_a_);
        panner_b_.Process(buffer_[1], panned_b_);
        buffer_[0] = xfader_.Process(panned_a_[0], panned_b_[0]);
        buffer_[1] = xfader_.Process(panned_a_[1], panned_b_[1]);
        feedback_[0] = buffer_[0];
        feedback_[1] = buffer_[1];
        out[0] = buffer_[0];
        out[1] = buffer_[1];
    }

    inline void Update(
        float panner_a_pos,
        float panner_b_pos,
        float xfader_pos,
        float feedback_level,
        bool muted
    ) {
        panner_a_.SetPos(panner_a_pos);
        panner_b_.SetPos(panner_b_pos);
        xfader_.SetPos(xfader_pos);
        feedback_level_ = feedback_level;
        muted_ = muted;
    }

private:
    daisysp::CrossFade xfader_;
    planetbosch::Panner panner_a_;
    planetbosch::Panner panner_b_;
    bool muted_;
    float buffer_[2];
    float feedback_[2];
    float feedback_level_;
    float panned_a_[2];
    float panned_b_[2];
};
