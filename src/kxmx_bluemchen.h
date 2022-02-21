#pragma once

#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"

namespace kxmx
{
class Bluemchen
{
  public:
    /** Enum of Ctrls to represent the two knobs and two CV inputs of the kxmx_*
     */
    enum Ctrl
    {
        CTRL_1,   /**< */
        CTRL_2,   /**< */
        CTRL_3,   /**< */
        CTRL_4,   /**< */
        CTRL_LAST /**< */
    };

    /** Constructor */
    Bluemchen() {}
    /** Destructor */
    ~Bluemchen() {}

    /** Initializes the daisy seed, and bluemchen hardware.*/
    void Init();

    /** Audio Block size defaults to 48.
  Change it using this function before StartingAudio
  \param size Audio block size.
  */
    void SetAudioBlockSize(size_t size);

    /** Set the sample rate for the audio */
    void SetAudioSampleRate(daisy::SaiHandle::Config::SampleRate samplerate);

    /** Get sample rate */
    float AudioSampleRate();

    /** Get block size */
    size_t AudioBlockSize();

    /** Get callback rate */
    float AudioCallbackRate();

    /** Start the bluemchen audio with the given callback function
    \cb AudioCallback callback function
    */
    void StartAudio(daisy::AudioHandle::AudioCallback cb);

    /** Starts the callback
    \param cb Interleaved callback function
    */
    void StartAudio(daisy::AudioHandle::InterleavingAudioCallback cb);

    /**
     Change the AudioCallback function
     \param cb The new callback function.
  */
    void ChangeAudioCallback(daisy::AudioHandle::AudioCallback cb);

    /** Stops the audio */
    void StopAudio();

    /** Start the ADC */
    void StartAdc();

    /** Stop the ADC */
    void StopAdc();

    /** Process all analog controls */
    void ProcessAnalogControls();

    /** Process all digital controls */
    void ProcessDigitalControls();

    /** Process Analog and Digital Controls */
    inline void ProcessAllControls()
    {
        ProcessAnalogControls();
        ProcessDigitalControls();
    }

    /**
     Get value for a particular control
     \param k Which control to get
   */
    float GetKnobValue(Ctrl k);

    /**
  General delay function
  \param del Delay time in ms.
  */
    void DelayMs(size_t del);

    daisy::DaisySeed       seed;                /**< Seed object */
    daisy::Encoder         encoder;             /**< Encoder object */
    daisy::AnalogControl   controls[CTRL_LAST]; /**< Array of AnalogControls */
    daisy::MidiUartHandler midi;                /**< MIDI handler */
    daisy::SdmmcHandler    sd;                  /**< MicroSD handler */
    daisy::OledDisplay<daisy::SSD130xI2c64x32Driver>
        display; /**< OLED Display */

  private:
    void InitControls();
    void InitDisplay();
    void InitMidi();
    void InitEncoder();
    void InitMicroSD();
};

} // namespace kxmx
