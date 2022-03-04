#include "daisy_versio.h"
#include "daisysp.h"
#include "frequency_shifter.h"
#include <string>

daisy::DaisyVersio hw;

static planetbosch::FrequencyShifter shifter;

void callback(
    daisy::AudioHandle::InterleavingInputBuffer  in,
    daisy::AudioHandle::InterleavingOutputBuffer out, size_t size
) {
  hw.ProcessAnalogControls();
  shifter.SetFrequency(
      (hw.GetKnobValue(daisy::DaisyVersio::KNOB_0) - 0.5) * 250
  );
  for (size_t i = 0; i < size; i += 2) {
    out[i] = out[i + 1] = shifter.Process(in[i]);
  }
}

int main(void) {
  hw.Init();
  shifter.Init(hw.AudioSampleRate());
  hw.StartAudio(callback);
  hw.StartAdc();
  while (1) {
    hw.UpdateExample();
    hw.UpdateLeds();
  }
}
