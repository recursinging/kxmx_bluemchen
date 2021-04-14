#include "kettensaege.h"
#include "daisysp.h"
#include "kxmx_bluemchen.h"
#include <string.h>
#include "daisy.h"

using namespace kxmx;
using namespace daisy;

Bluemchen bluemchen;

Parameter knob1;
Parameter knob2;
Parameter cv1;
Parameter cv2;

float osc_e1vo;
float osc_st;
float osc_st_offset;
float osc_hz;

bool trigger1;

daisysp::AdEnv envelopeL;
daisysp::AdEnv envelopeR;
daisysp::Svf filterL;
daisysp::Svf filterR;



float semitone_to_hz(float semitone) { return daisysp::mtof(semitone); }

float e1vo_to_hz(float e1vo) { return semitone_to_hz(e1vo * 12); }

int e1vo_to_qsemitone(float e1vo)
{
    return static_cast<int>(std::round(e1vo * 12.0));
}

float e1vo_to_semitone(float e1vo)
{
    return e1vo * 12.0;
}

float semitone_to_e1vo(int semitone)
{
    return static_cast<float>(semitone) / 12.0f;
}

float quantize_e1vo(float e1vo)
{
    return semitone_to_e1vo(e1vo_to_semitone(e1vo));
}

const char *semitones[] = {
    "C",
    "C#",
    "D",
    "D#",
    "E",
    "F",
    "F#",
    "G",
    "G#",
    "A",
    "A#",
    "B",
};

// std::string semitone_name(float fSemitone) {
//   // C0 is 12
//   // B8 is 119
//   int semitone = static_cast<int>(std::round(fSemitone));
//   if (semitone < st_min || semitone > st_max) {
//     return "???";
//   }

//   float cents = fSemitone - semitone;

//   semitone -= 12;
//   int octave = semitone / 12;
//   int note   = semitone % 12;

//   std::string out(semitones[note]);
//   out.push_back('0' + octave);

//   if (cents >= 0.15) {
//     out.push_back('+');
//   } else if (cents <= -0.15) {
//     out.push_back('-');
//   } else {
//     out.push_back(' ');
//   }
//   return out;
// }

void Kettensaege::Init(float sample_rate)
{
    for (int i = 0; i < 7; i++)
    {
        osc[i].Init(sample_rate);
        if (i == 1 || i == 2)
        {
            osc[i].SetWaveform(daisysp::Oscillator::WAVE_POLYBLEP_SAW);
        }
        else
        {
            osc[i].SetWaveform(daisysp::Oscillator::WAVE_POLYBLEP_SAW);
        }
    }
    freq_ = 130.81f; // C3
    amp_ = 0.75f;
    detune_ = 1.0f;
    mix_ = 1.0f;

    UpdateFreqs();
    UpdateAmps();
}

void Kettensaege::SetFreq(float f)
{
    if (f != freq_)
    {
        freq_ = f;
        UpdateFreqs();
    }
}

void Kettensaege::SetAmp(float amp)
{
    if (amp != amp_)
    {
        amp_ = amp;
        UpdateAmps();
    }
}

void Kettensaege::SetDetune(float detune)
{
    if (detune != detune_)
    {
        detune_ = detune;
        UpdateFreqs();
    }
}

void Kettensaege::SetDetuneAndFreq(float detune, float freq)
{
    if (detune != detune_ || freq != freq_)
    {
        detune_ = detune;
        freq_ = freq;
        UpdateFreqs();
    }
}

void Kettensaege::SetMix(float mix)
{
    if (mix != mix_)
    {
        mix_ = mix;
        UpdateAmps();
    }
}

float Kettensaege::ProcessL()
{
    float s = 0;

    s += osc[0].Process();
    s += osc[1].Process();
    s += osc[3].Process();
    s += osc[5].Process();

    return s;
}

float Kettensaege::ProcessR()
{
    float s = 0;

    s += osc[0].Process();
    s += osc[2].Process();
    s += osc[4].Process();
    s += osc[6].Process();

    return s;
}

void Kettensaege::Reset()
{
    for (int i = 0; i < 7; i++)
    {
        osc[i].Reset();
    }
}

// The max detune for each of the side oscillators.
// static const float detune_factors[] = {-0.11002313, -0.06288439, -0.01952356,
//                                        0.01991221, 0.06216538, 0.10745242};

static const float detune_factors[] = {-0.749, 0.501, 0.25,
                                       0.249, 0.49, 1.99};

void Kettensaege::UpdateFreqs()
{
    // We approximate the curve in the paper by the following formula
    // (y=detune_scale, x=detune_):
    //    y = 1.5*x^6 - 0.8*x^5 + 0.3*x^3 - 0.3*x^2 + 0.3*x
    // You can play with that curve here:
    //   https://www.desmos.com/calculator/rwzsa4gsou
    detune_scale_ = 1.5 * powf(detune_, 6) - 0.8 * powf(detune_, 5) +
                    0.3 * powf(detune_, 3) - 0.3 * powf(detune_, 2) +
                    0.3 * detune_;
    osc[0].SetFreq(freq_);
    for (int i = 0; i < 6; i++)
    {
        osc[i + 1].SetFreq(freq_ + freq_ * detune_factors[i] * detune_scale_);
        //osc[i + 1].SetFreq(freq_ + freq_ * 0.33 * detune_scale_);
    }
}

void Kettensaege::UpdateAmps()
{
    // Formulas are taken from the paper referened above.
    float center_amp = -0.55366 * mix_ + 0.99785;
    osc[0].SetAmp(amp_ * center_amp);

    //osc[0].SetAmp(0.0f);

    float side_amp = -0.73764 * mix_ * mix_ + 1.2841 * mix_ + 0.044372;
    for (int i = 1; i < 7; i++)
    {
        osc[i].SetAmp(amp_ * side_amp);
    }
}

Kettensaege kettensaege;

void UpdateOled()
{
    bluemchen.display.Fill(false);

    // Display Encoder test increment value and pressed state
    bluemchen.display.SetCursor(0, 0);
    std::string str = "Enc: ";
    char *cstr = &str[0];
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(bluemchen.encoder.Increment());
    bluemchen.display.SetCursor(30, 0);
    bluemchen.display.WriteString(cstr, Font_6x8, !bluemchen.encoder.Pressed());

    // Display the knob values in millivolts
    str = std::to_string(static_cast<int>(osc_e1vo * 1000));
    bluemchen.display.SetCursor(0, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = ":";
    bluemchen.display.SetCursor(30, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(static_cast<int>(osc_st * 1000));
    bluemchen.display.SetCursor(36, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    // Display CV input in millivolts
    str = std::to_string(static_cast<int>(osc_hz));
    bluemchen.display.SetCursor(0, 24);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    if (cv2.Value() > -999.0f)
    {
        str = ":";
        bluemchen.display.SetCursor(30, 24);
        bluemchen.display.WriteString(cstr, Font_6x8, true);
    }
    str = std::to_string(static_cast<int>(cv2.Value()));
    bluemchen.display.SetCursor((cv2.Value() > -999.0f) ? 36 : 30, 24);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    bluemchen.display.Update();
}

void UpdateControls()
{
    bluemchen.ProcessAllControls();

    knob1.Process();
    knob2.Process();

    cv1.Process();
    cv2.Process();

    envelopeL.SetTime(daisysp::AdEnvSegment::ADENV_SEG_DECAY, knob1.Value());
    envelopeR.SetTime(daisysp::AdEnvSegment::ADENV_SEG_DECAY, knob1.Value());

    envelopeL.Process();
    envelopeR.Process();

    if (cv2.Value() > 2.5f) {
        trigger1 = true;
    } else {
        trigger1 = false;
    }

    if (trigger1 ) {
        envelopeL.Trigger();
        envelopeR.Trigger();
    }
    
    filterL.SetFreq(envelopeL.GetValue());
    filterR.SetFreq(envelopeR.GetValue());

    if (bluemchen.encoder.Pressed()) {
        osc_st_offset += bluemchen.encoder.Increment();    
    } else {
        osc_st_offset += (bluemchen.encoder.Increment() / 100.0f);
    }
    

    osc_e1vo = cv1.Value();
    osc_st = e1vo_to_semitone(osc_e1vo) + osc_st_offset;
    osc_hz = semitone_to_hz(osc_st);
    kettensaege.SetDetuneAndFreq(knob2.Value(), osc_hz);
}

void AudioCallback(float **in, float **out, size_t size)
{
    UpdateControls();

    for (size_t i = 0; i < size; i++)
    {

        float sigL = kettensaege.ProcessL();
        float sigR = kettensaege.ProcessR();

        filterL.Process(sigL);
        filterR.Process(sigR);

        sigL = filterL.Low();
        sigR = filterR.Low();

        

        out[0][i] = sigL;
        out[1][i] = sigR;
    }

    //memcpy(out, in, sizeof(float) * size);
}

int main(void)
{
    bluemchen.Init();
    bluemchen.StartAdc();

    kettensaege.Init(bluemchen.AudioSampleRate());
    kettensaege.SetAmp(0.2f);
    kettensaege.SetMix(1.0f);

    envelopeL.Init(bluemchen.AudioSampleRate());
    envelopeL.SetTime(daisysp::AdEnvSegment::ADENV_SEG_ATTACK, 0.0001f);
    envelopeL.SetTime(daisysp::AdEnvSegment::ADENV_SEG_DECAY, 0.010f);
    envelopeL.SetCurve(-5.0f);
    envelopeL.SetMax(10000.0f);
    envelopeL.SetMin(0.0f);

    envelopeR.Init(bluemchen.AudioSampleRate());
    envelopeR.SetTime(daisysp::AdEnvSegment::ADENV_SEG_ATTACK, 0.0001f);
    envelopeR.SetTime(daisysp::AdEnvSegment::ADENV_SEG_DECAY, 0.010f);
    envelopeR.SetCurve(-4.0f);
    envelopeR.SetMax(8000.0f);
    envelopeR.SetMin(0.0f);

    filterL.Init(bluemchen.AudioSampleRate());
    filterL.SetFreq(0.0f);
    filterL.SetRes(0.4f);

    filterR.Init(bluemchen.AudioSampleRate());
    filterR.SetFreq(0.0f);
    filterR.SetRes(0.4f);


    knob1.Init(bluemchen.knobs[bluemchen.KNOB_1], 0.0005f, 0.05f, Parameter::LINEAR);
    knob2.Init(bluemchen.knobs[bluemchen.KNOB_2], 0.0f, 1.0f, Parameter::LINEAR);

    cv1.Init(bluemchen.cv[bluemchen.CV_1], -5.0f, 5.0f, Parameter::LINEAR);
    cv2.Init(bluemchen.cv[bluemchen.CV_2], -5000.0f, 5000.0f, Parameter::LINEAR);

    bluemchen.StartAudio(AudioCallback);

    while (1)
    {
        UpdateOled();
    }
}