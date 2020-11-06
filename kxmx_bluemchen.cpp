#include "kxmx_bluemchen.h"

using namespace kxmx;

// Hardware Definitions
#define PIN_ENC_CLICK 28
#define PIN_ENC_B 27
#define PIN_ENC_A 26
#define PIN_MIDI_OUT 13

#define PIN_CTRL_1 15
#define PIN_CTRL_2 16
#define PIN_CTRL_3 21
#define PIN_CTRL_4 18

const float kAudioSampleRate = 48014.f;

/******************** OLED ***********************/

daisy::I2CHandle::Config i2c_conf;
daisy::I2CHandle i2c;

void BluemchenOledDisplay::Init(dsy_gpio_pin* pin_cfg) {
  i2c_conf.periph = daisy::I2CHandle::Config::Peripheral::I2C_1;
  i2c_conf.speed = daisy::I2CHandle::Config::Speed::I2C_100KHZ;
  i2c_conf.pin_config.scl =  {DSY_GPIOB, 8};
  i2c_conf.pin_config.sda = {DSY_GPIOB, 9};
  i2c.Init(i2c_conf);
}

void BluemchenOledDisplay::SendCommand(uint8_t byte) {
    
}
void BluemchenOledDisplay::SendData(uint8_t* buff, size_t size) {

}

/******************** Bluemchen ******************/

void Bluemchen::Init() {
  // Configure Seed first
  seed.Configure();
  InitAudio();
  seed.Init();
  InitEncoder();
  InitDisplay();
  InitMidi();
  InitControls();
  // Reset AK4556
  dsy_gpio_write(&ak4556_reset_pin_, 0);
  DelayMs(10);
  dsy_gpio_write(&ak4556_reset_pin_, 1);
  // Set Screen update vars
  screen_update_period_ = 17;  // roughly 60Hz
  screen_update_last_ = dsy_system_getnow();
}

void Bluemchen::DelayMs(size_t del) { dsy_system_delay(del); }
void Bluemchen::SetAudioBlockSize(size_t size) { seed.SetAudioBlockSize(size); }

void Bluemchen::StartAudio(daisy::AudioHandle::AudioCallback cb) {
  seed.StartAudio(cb);
}

void Bluemchen::ChangeAudioCallback(daisy::AudioHandle::AudioCallback cb) {
  seed.ChangeAudioCallback(cb);
}

void Bluemchen::StartAdc() { seed.adc.Start(); }
float Bluemchen::AudioSampleRate() { return kAudioSampleRate; }
size_t Bluemchen::AudioBlockSize() { return seed.AudioBlockSize(); }
float Bluemchen::AudioCallbackRate() { return seed.AudioCallbackRate(); }
void Bluemchen::UpdateAnalogControls() {
  for (size_t i = 0; i < CTRL_LAST; i++) {
    controls[i].Process();
  }
}
float Bluemchen::GetCtrlValue(Ctrl k) { return (controls[k].Value()); }

void Bluemchen::DebounceControls() { encoder.Debounce(); }

// Private Function Implementations
// set SAI2 stuff -- run this between seed configure and init
void Bluemchen::InitAudio() {
  // Handle Seed Audio as-is and then
  daisy::SaiHandle::Config sai_config;
  // Internal Codec
  sai_config.periph = daisy::SaiHandle::Config::Peripheral::SAI_1;
  sai_config.sr = daisy::SaiHandle::Config::SampleRate::SAI_48KHZ;
  sai_config.bit_depth = daisy::SaiHandle::Config::BitDepth::SAI_24BIT;
  sai_config.a_sync = daisy::SaiHandle::Config::Sync::MASTER;
  sai_config.b_sync = daisy::SaiHandle::Config::Sync::SLAVE;
  sai_config.a_dir = daisy::SaiHandle::Config::Direction::TRANSMIT;
  sai_config.b_dir = daisy::SaiHandle::Config::Direction::RECEIVE;
  sai_config.pin_config.fs = {DSY_GPIOE, 4};
  sai_config.pin_config.mclk = {DSY_GPIOE, 2};
  sai_config.pin_config.sck = {DSY_GPIOE, 5};
  sai_config.pin_config.sa = {DSY_GPIOE, 6};
  sai_config.pin_config.sb = {DSY_GPIOE, 3};

  daisy::SaiHandle sai_handle;
  sai_handle.Init(sai_config);

  daisy::AudioHandle::Config cfg;
  cfg.blocksize = 48;

  seed.audio_handle.Init(cfg, sai_handle);
}
void Bluemchen::InitControls() {
  daisy::AdcChannelConfig cfg[CTRL_LAST];

  // Init ADC channels with Pins
  cfg[CTRL_1].InitSingle(seed.GetPin(PIN_CTRL_1));
  cfg[CTRL_2].InitSingle(seed.GetPin(PIN_CTRL_2));
  cfg[CTRL_3].InitSingle(seed.GetPin(PIN_CTRL_3));
  cfg[CTRL_4].InitSingle(seed.GetPin(PIN_CTRL_4));

  // Initialize ADC
  seed.adc.Init(cfg, CTRL_LAST);

  // Initialize AnalogControls, with flip set to true
  for (size_t i = 0; i < CTRL_LAST; i++) {
    controls[i].Init(seed.adc.GetPtr(i), AudioCallbackRate(), true);
  }
}

void Bluemchen::InitDisplay() { display.Init(NULL);
display.DrawPixel(1,1,true);
 }

void Bluemchen::InitMidi() {
  midi.Init(daisy::MidiHandler::MidiInputMode::INPUT_MODE_UART1,
            daisy::MidiHandler::MidiOutputMode::OUTPUT_MODE_UART1);
}

void Bluemchen::InitEncoder() {
  encoder.Init(seed.GetPin(PIN_ENC_A), seed.GetPin(PIN_ENC_B),
               seed.GetPin(PIN_ENC_CLICK), AudioCallbackRate());
}
