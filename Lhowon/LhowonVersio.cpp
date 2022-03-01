#include "Lhowon.cpp"
#include "daisy_versio.h"
#include "daisysp.h"
#include <string>

daisy::DaisyVersio hw;

Lhowon lhowon;

void callback(
    daisy::AudioHandle::InterleavingInputBuffer  in,
    daisy::AudioHandle::InterleavingOutputBuffer out, size_t size
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

  while (1) {
    hw.ProcessAnalogControls();
    hw.UpdateExample();
    hw.UpdateLeds();
    lhowon.Update(
        hw.GetKnobValue(daisy::DaisyVersio::KNOB_0), // pan A
        hw.GetKnobValue(daisy::DaisyVersio::KNOB_1), // fx A
        hw.GetKnobValue(daisy::DaisyVersio::KNOB_2), // xfade
        hw.GetKnobValue(daisy::DaisyVersio::KNOB_3), // reverb
        hw.GetKnobValue(daisy::DaisyVersio::KNOB_4), // pan B
        hw.GetKnobValue(daisy::DaisyVersio::KNOB_5), // fx B
        hw.GetKnobValue(daisy::DaisyVersio::KNOB_6), // feedback
        hw.Gate() || hw.SwitchPressed()
    );
  }
}
