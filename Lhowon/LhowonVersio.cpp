#include "daisy_versio.h"
#include "daisysp.h"
#include <string>
#include "Lhowon.cpp"

using namespace daisy;
using namespace daisysp;

DaisyVersio hw;

Lhowon lhowon;

void callback(
    AudioHandle::InterleavingInputBuffer  in,
    AudioHandle::InterleavingOutputBuffer out,
    size_t                                size
) {
    for (size_t i = 0; i < size; i += 2) {
        lhowon.Process(in[i], in[i + 1], &out[i]);
    }
}

int main(void) {
    hw.Init();
    lhowon.Init(hw.AudioSampleRate());
    hw.StartAudio(callback);
    hw.StartAdc();

    while(1) {
        hw.ProcessAnalogControls();
        hw.UpdateExample();
        hw.UpdateLeds();

        lhowon.Update(
            hw.GetKnobValue(DaisyVersio::KNOB_0),
            hw.GetKnobValue(DaisyVersio::KNOB_4),
            hw.GetKnobValue(DaisyVersio::KNOB_2),
            hw.GetKnobValue(DaisyVersio::KNOB_6),
            hw.Gate()
        );
    }
}
