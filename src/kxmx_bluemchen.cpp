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

void Bluemchen::Init()
{
    seed.Configure();
    seed.Init(true);
    InitEncoder();
    InitDisplay();
    InitMidi();
    InitControls();
    InitMicroSD();

    daisy::DacHandle::Config cfg;
    cfg.bitdepth   = daisy::DacHandle::BitDepth::BITS_12;
    cfg.buff_state = daisy::DacHandle::BufferState::ENABLED;
    cfg.mode       = daisy::DacHandle::Mode::POLLING;
    cfg.chn        = daisy::DacHandle::Channel::BOTH;
    seed.dac.Init(cfg);
    seed.dac.WriteValue(daisy::DacHandle::Channel::BOTH, 0);
}

void Bluemchen::SetAudioSampleRate(daisy::SaiHandle::Config::SampleRate sr)
{
    seed.SetAudioSampleRate(sr);
}

float Bluemchen::AudioSampleRate()
{
    return seed.AudioSampleRate();
}

void Bluemchen::SetAudioBlockSize(size_t size)
{
    seed.SetAudioBlockSize(size);
}

size_t Bluemchen::AudioBlockSize()
{
    return seed.AudioBlockSize();
}

float Bluemchen::AudioCallbackRate()
{
    return seed.AudioCallbackRate();
}

void Bluemchen::StartAudio(daisy::AudioHandle::AudioCallback cb)
{
    seed.StartAudio(cb);
}

void Bluemchen::StartAudio(daisy::AudioHandle::InterleavingAudioCallback cb)
{
    seed.StartAudio(cb);
}

void Bluemchen::StopAudio()
{
    seed.StopAudio();
}

void Bluemchen::ChangeAudioCallback(daisy::AudioHandle::AudioCallback cb)
{
    seed.ChangeAudioCallback(cb);
}

void Bluemchen::StartAdc()
{
    seed.adc.Start();
}
void Bluemchen::StopAdc()
{
    seed.adc.Stop();
}

void Bluemchen::ProcessAnalogControls()
{
    for(size_t i = 0; i < CTRL_LAST; i++)
    {
        controls[i].Process();
    }
}
void Bluemchen::ProcessDigitalControls()
{
    encoder.Debounce();
}
float Bluemchen::GetKnobValue(Ctrl c)
{
    return (controls[c].Value());
}
void Bluemchen::DelayMs(size_t del)
{
    daisy::System::Delay(del);
}

void Bluemchen::InitControls()
{
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
    controls[CTRL_1].Init(seed.adc.GetPtr(0), AudioCallbackRate(), false);
    controls[CTRL_2].Init(seed.adc.GetPtr(1), AudioCallbackRate(), false);
    // Initialize the CV controls
    controls[CTRL_3].Init(seed.adc.GetPtr(2), AudioCallbackRate(), true);
    controls[CTRL_4].Init(seed.adc.GetPtr(3), AudioCallbackRate(), true);
}

void Bluemchen::InitDisplay()
{
    daisy::OledDisplay<daisy::SSD130xI2c64x32Driver>::Config display_config;
    display_config.driver_config.transport_config.Defaults();
    display.Init(display_config);
}

void Bluemchen::InitMidi()
{
    daisy::MidiUartHandler::Config midi_config;
    midi.Init(midi_config);
}

void Bluemchen::InitEncoder()
{
    encoder.Init(seed.GetPin(PIN_ENC_A),
                 seed.GetPin(PIN_ENC_B),
                 seed.GetPin(PIN_ENC_CLICK),
                 AudioCallbackRate());
}

void Bluemchen::InitMicroSD()
{
    sdmmc_cfg.Defaults();
    sd.Init(sdmmc_cfg);
}
