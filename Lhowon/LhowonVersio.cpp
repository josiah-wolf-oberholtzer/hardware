#include "daisy_versio.h"
#include "daisysp.h"
#include <string>
#include "Lhowon.cpp"

daisy::DaisyVersio hw;

Lhowon lhowon;

void callback(
    daisy::AudioHandle::InterleavingInputBuffer  in,
    daisy::AudioHandle::InterleavingOutputBuffer out,
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
            hw.GetKnobValue(daisy::DaisyVersio::KNOB_0),
            hw.GetKnobValue(daisy::DaisyVersio::KNOB_4),
            hw.GetKnobValue(daisy::DaisyVersio::KNOB_2),
            hw.GetKnobValue(daisy::DaisyVersio::KNOB_6),
            hw.Gate()
        );
    }
}
