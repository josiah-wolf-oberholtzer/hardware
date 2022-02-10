#include "daisy_versio.h"
#include "daisysp.h"
#include <string>
#include "Panner.h"

using namespace daisy;
using namespace daisysp;

DaisyVersio hw;

Panner panner_a;
Panner panner_b;
CrossFade xfader;

static float buffer[2];
static float panned_a[2];
static float panned_b[2];
static float feedback[2];
static float feedback_level;

void callback(
    AudioHandle::InterleavingInputBuffer  in,
    AudioHandle::InterleavingOutputBuffer out,
    size_t                                size
) {
    for (size_t i = 0; i < size; i += 2) {
        buffer[0] = SoftClip(in[i] + (feedback[0] * feedback_level));
        buffer[1] = SoftClip(in[i + 1] + (feedback[1] * feedback_level));

        panner_a.Process(buffer[0], panned_a);
        panner_b.Process(buffer[1], panned_b);

        buffer[0] = xfader.Process(panned_a[0], panned_b[0]);
        buffer[1] = xfader.Process(panned_a[1], panned_b[1]);

        feedback[0] = buffer[0];
        feedback[1] = buffer[1];

        out[i] = buffer[0];
        out[i + 1] = buffer[1];
    }
}

int main(void) {
    hw.Init();

    feedback_level = 0.f;
    panner_a.Init();
    panner_b.Init();
    xfader.Init(CROSSFADE_CPOW);

    hw.StartAudio(callback);
    hw.StartAdc();

    while(1) {
        hw.ProcessAnalogControls();
        hw.UpdateExample();
        hw.UpdateLeds();

        panner_a.SetPos(hw.GetKnobValue(DaisyVersio::KNOB_0));
        xfader.SetPos(hw.GetKnobValue(DaisyVersio::KNOB_2));
        panner_b.SetPos(hw.GetKnobValue(DaisyVersio::KNOB_4));
        feedback_level = hw.GetKnobValue(DaisyVersio::KNOB_6);
    }
}
