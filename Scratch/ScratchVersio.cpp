#include "daisy_versio.h"
#include "daisysp.h"
#include "reverb.h"
#include <string>

daisy::DaisyVersio hw;

static planetbosch::Reverb reverb;

static float buffer[32768];

void callback(
    daisy::AudioHandle::InterleavingInputBuffer  in,
    daisy::AudioHandle::InterleavingOutputBuffer out, size_t size
) {
  hw.ProcessAnalogControls();
  reverb.SetAmount(hw.GetKnobValue(daisy::DaisyVersio::KNOB_0));
  reverb.SetInputGain(hw.GetKnobValue(daisy::DaisyVersio::KNOB_4));
  reverb.SetTime(hw.GetKnobValue(daisy::DaisyVersio::KNOB_1));
  reverb.SetDiffusion(hw.GetKnobValue(daisy::DaisyVersio::KNOB_5));
  reverb.SetHp(hw.GetKnobValue(daisy::DaisyVersio::KNOB_2));
  reverb.SetLp(hw.GetKnobValue(daisy::DaisyVersio::KNOB_3));
  for (size_t i = 0; i < size; i += 2) {
    reverb.Process(in[i], in[i + 1], &out[i], &out[i + 1]);
  }
}

int main(void) {
  hw.Init();
  reverb.Init(buffer, hw.AudioSampleRate());
  hw.StartAudio(callback);
  hw.StartAdc();
  while (1) {
    hw.UpdateExample();
    hw.UpdateLeds();
  }
}
