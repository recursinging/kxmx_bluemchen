#include "kxmx_bluemchen.h"

using namespace kxmx;

#define PIN_ENC_CLICK 28
#define PIN_ENC_B 26
#define PIN_ENC_A 27

#define PIN_KNOB_1 16
#define PIN_KNOB_2 15

#define PIN_CV_1 21
#define PIN_CV_2 18

daisy::SdmmcHandler::Config sdmmc_cfg;


void Bluemchen::Init() {
  seed.Configure();
  seed.Init(true);
  InitEncoder();
  InitDisplay();
  InitMidi();
  InitControls();
  InitMicroSD();
}

void Bluemchen::SetAudioSampleRate(daisy::SaiHandle::Config::SampleRate sr) {
  seed.SetAudioSampleRate(sr);
}

float Bluemchen::AudioSampleRate() { return seed.AudioSampleRate(); }

void Bluemchen::SetAudioBlockSize(size_t size) { seed.SetAudioBlockSize(size); }

size_t Bluemchen::AudioBlockSize() { return seed.AudioBlockSize(); }

float Bluemchen::AudioCallbackRate() { return seed.AudioCallbackRate(); }

void Bluemchen::StartAudio(daisy::AudioHandle::AudioCallback cb) {
  seed.StartAudio(cb);
}

void Bluemchen::StopAudio() { seed.StopAudio(); }

void Bluemchen::ChangeAudioCallback(daisy::AudioHandle::AudioCallback cb) {
  seed.ChangeAudioCallback(cb);
}

void Bluemchen::StartAdc() { seed.adc.Start(); }
void Bluemchen::StopAdc() { seed.adc.Stop(); }

void Bluemchen::ProcessAnalogControls() {
  knobs[KNOB_1].Process();
  knobs[KNOB_2].Process();
  cv[CV_1].Process();
  cv[CV_2].Process();
}
void Bluemchen::ProcessDigitalControls() { encoder.Debounce(); }
float Bluemchen::GetKnobValue(Knob k) { return (knobs[k].Value()); }
float Bluemchen::GetCVValue(CV c) { return (cv[c].Value()); }
void Bluemchen::DelayMs(size_t del) { daisy::System::Delay(del); }

void Bluemchen::InitControls() {
  daisy::AdcChannelConfig cfg[4];

  // Init ADC channels for the knobs...
  cfg[0].InitSingle(seed.GetPin(PIN_KNOB_1));
  cfg[1].InitSingle(seed.GetPin(PIN_KNOB_2));

  // ...and the CV inputs.
  cfg[2].InitSingle(seed.GetPin(PIN_CV_1));
  cfg[3].InitSingle(seed.GetPin(PIN_CV_2));

  // Initialize ADC config
  seed.adc.Init(cfg, 4);

  // Initialize the Knob controls
  knobs[KNOB_1].Init(seed.adc.GetPtr(0), AudioCallbackRate(), false);
  knobs[KNOB_2].Init(seed.adc.GetPtr(1), AudioCallbackRate(), false);
  // Initialize the CV controls
  cv[CV_1].Init(seed.adc.GetPtr(2), AudioCallbackRate(), true);
  cv[CV_2].Init(seed.adc.GetPtr(3), AudioCallbackRate(), true);
}

void Bluemchen::InitDisplay() { display.Init(NULL); }

void Bluemchen::InitMidi() {
  midi.Init(daisy::MidiHandler::MidiInputMode::INPUT_MODE_UART1,
            daisy::MidiHandler::MidiOutputMode::OUTPUT_MODE_UART1);
}

void Bluemchen::InitEncoder() {
  encoder.Init(seed.GetPin(PIN_ENC_A), seed.GetPin(PIN_ENC_B),
               seed.GetPin(PIN_ENC_CLICK), AudioCallbackRate());
}

void Bluemchen::InitMicroSD() {
  sdmmc_cfg.Defaults();
  sd.Init(sdmmc_cfg);
}
