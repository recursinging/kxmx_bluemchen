#pragma once

#include "daisy_seed.h"

namespace kxmx
{

class Bluemchen
{
  public:
    /** Enum of Knobs on the bluemchen
     */
    enum Knob
    {
        KNOB_1,    /**< */
        KNOB_2,    /**< */
        KNOB_LAST, /**< */
    };
    /** Enum of CV inputs on the bluemchen
     */
    enum CV
    {
        CV_1,    /**< */
        CV_2,    /**< */
        CV_LAST, /**< */
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
    float GetKnobValue(Knob k);

    float GetCVValue(CV c);
    
    /** 
    General delay function
    \param del Delay time in ms.
    */
    void DelayMs(size_t del);

    daisy::DaisySeed     seed;                     /**< Seed object */
    daisy::Encoder       encoder;                  /**< Encoder object */
    daisy::AnalogControl knobs[KNOB_LAST];         /**< Array of knobs */
    daisy::AnalogControl cv[CV_LAST];              /**< Array of CV jacks */
    daisy::MidiHandler   midi;                     /**< MIDI handler */
    daisy::SdmmcHandler  sd;                       /**< MicroSD handler */
    daisy::OledDisplay   display;                  /**< OLED Display */


  private:
    void InitControls();
    void InitDisplay();
    void InitMidi();
    void InitEncoder();
    void InitMicroSD();
};

} // namespace kxmx

