#include "DSP.h"
#include "Hrongir.h"
#include "daisy_versio.h"
#include "daisysp.h"
#include <string>

daisy::DaisyVersio hw;

planetbosch::Hrongir hrongir;
planetbosch::FloatFrame* frame;
float DSY_SDRAM_BSS buffers[8][48000];

void callback(
  daisy::AudioHandle::InterleavingInputBuffer  in,
  daisy::AudioHandle::InterleavingOutputBuffer out, 
  size_t                                       size
) {
  hw.ProcessAnalogControls();
  hrongir.Update(
    hw.GetKnobValue(daisy::DaisyVersio::KNOB_0), // mix
    hw.GetKnobValue(daisy::DaisyVersio::KNOB_1), // comb A
    hw.GetKnobValue(daisy::DaisyVersio::KNOB_2), // comb B
    hw.GetKnobValue(daisy::DaisyVersio::KNOB_3), // comb C
    hw.GetKnobValue(daisy::DaisyVersio::KNOB_4), // decay
    hw.GetKnobValue(daisy::DaisyVersio::KNOB_5), // comb D
    hw.GetKnobValue(daisy::DaisyVersio::KNOB_6) // fx freq
  );
  for (size_t i = 0; i < size; i += 2) {
    frame->in[0] = in[i];
    frame->in[1] = in[i + 1];
    hrongir.Process(frame);
    out[i] = frame->out[0];
    out[i + 1] = frame->out[1];
  }
}

int main(void) {
  hw.Init();
  hrongir.Init(hw.AudioSampleRate(), buffers[0], 48000);
  hw.StartAudio(callback);
  hw.StartAdc();

  while (1) {
    hw.UpdateExample();
    hw.UpdateLeds();
  }
}
