#include "daisy_versio.h"
#include "daisysp.h"
#include <string>
#include "Panner.h"

using namespace daisy;
using namespace daisysp;

DaisyVersio hw;

Panner panner_l;
Panner panner_r;

CrossFade xfade_l;
CrossFade xfade_r;

void callback(
    AudioHandle::InterleavingInputBuffer  in,
    AudioHandle::InterleavingOutputBuffer out,
    size_t                                size
) {
    for (size_t i = 0; i < size; i += 2) {
        out[i] = 0.0f;
        out[i + 1] = 0.0f;
    }
}

int main(void) {
    hw.Init();

    panner_l.Init();
    panner_r.Init();

    xfade_l.Init(CROSSFADE_CPOW);
    xfade_r.Init(CROSSFADE_CPOW);

    hw.StartAudio(callback);
    hw.StartAdc();

    while(1) {
        hw.ProcessAnalogControls();
        hw.UpdateExample();
        hw.UpdateLeds();

        panner_l.SetPos(hw.GetKnobValue(DaisyVersio::KNOB_1));
        panner_r.SetPos(hw.GetKnobValue(DaisyVersio::KNOB_5));

        xfade_l.SetPos(hw.GetKnobValue(DaisyVersio::KNOB_3));
        xfade_r.SetPos(hw.GetKnobValue(DaisyVersio::KNOB_3));
    }
}
