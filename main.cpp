#include "daisysp.h"
#include "kxmx_bluemchen.h"
#include <string.h>

using namespace kxmx;
using namespace daisy;
using namespace daisysp;

Bluemchen bluemchen;

float        freqs[8];

Parameter q;
Parameter drive;

int bank;

struct ConditionalUpdate
{
    float oldVal = 0;

    bool Process(float newVal)
    {
        if(abs(newVal - oldVal) > .04)
        {
            oldVal = newVal;
            return true;
        }

        return false;
    }
};

ConditionalUpdate condUpdates[4];

struct Filter
{
    Svf   filt;
    float amp;

    void Init(float samplerate, float freq)
    {
        filt.Init(samplerate);
        filt.SetRes(1);
        filt.SetDrive(.002);
        filt.SetFreq(freq);
        
        amp = .5f;
    }

    float Process(float in)
    {
        filt.Process(in);
        return filt.Peak() * amp;
    }
    float ProcessBand(float in)
    {
        filt.Process(in);
        return filt.Band() * amp;
    }
};

Filter filters[8];
Filter filters2[8];


void UpdateControls();

static void AudioCallback(float **in, float **out, size_t size)
{
    UpdateControls();
    for(size_t i = 0; i < size; i++)
    {
        float sigL = 0.f;
        float sigR = 0.f;
        for(int j = 0; j < 8; j++)
        {
            sigL += filters[j].Process(in[0][i]);
        }
        for(int j = 0; j < 8; j++)
        {
            sigR += filters2[j].ProcessBand(in[0][i]);
        }
        sigL *= .06;
        sigR *= .06;

        out[0][i] = sigL;
        out[1][i] = sigR;
    }
}

void InitFreqs()
{
    freqs[0]  = 82.4;
    freqs[1]  = 97.99;
    freqs[2]  = 130.8;
    freqs[3]  = 164.8;
    freqs[4]  = 195.98;
    freqs[5]  = 261.6;
    freqs[6]  = 329.6;
    freqs[7]  = 391.96;
    freqs[8]  = 523.2;
}

void InitFilters(float samplerate)
{
    for(int i = 0; i < 8; i++)
    {
        filters[i].Init(samplerate, freqs[i]);
    }
    for(int i = 0; i < 8; i++)
    {
        filters2[i].Init(samplerate, freqs[i]);
    }
}

void UpdateOled();


int main(void)
{
    float samplerate;
    bluemchen.Init();
    samplerate = bluemchen.AudioSampleRate();

    InitFreqs();
    InitFilters(samplerate);
    bank = 0;

    
    drive.Init(bluemchen.knobs[0], 0.35f, 0.002f, daisy::Parameter::LINEAR);
    q.Init(bluemchen.knobs[0], 0.75f, 1.2f, daisy::Parameter::LINEAR);

    bluemchen.StartAdc();
    bluemchen.StartAudio(AudioCallback);
    while(1)
    {
        UpdateOled();
        UpdateControls();
    }
}

void UpdateOled()
{
    bluemchen.display.Fill(false);

    std::string str  = "FB:";
    char *      cstr = &str[0];
    bluemchen.display.SetCursor(0, 0);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = "D: ";
    bluemchen.display.SetCursor(0, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(static_cast<int32_t>(drive.Value() * 100));
    bluemchen.display.SetCursor(24, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = "Q: ";
    bluemchen.display.SetCursor(0, 16);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str =  str = std::to_string(static_cast<int32_t>(q.Value() * 100));
    bluemchen.display.SetCursor(24, 16);
    bluemchen.display.WriteString(cstr, Font_6x8, true);




    bluemchen.display.Update();
}

void UpdateControls()
{
    bluemchen.ProcessAllControls();
    q.Process();
    drive.Process();

    //encoder
    bank += bluemchen.encoder.Increment();
    
    if (bank > 8) {bank = 8;}
    if (bank < 0) {bank = 0;}

    for(int i = 0; i < 8; i++)
    {
        filters[i].filt.SetRes(q.Value());
        //filters[i].filt.SetDrive(drive.Value());
    }
    for(int i = 0; i < 8; i++)
    {
        filters2[i].filt.SetRes(q.Value());
        //filters2[i].filt.SetDrive(drive.Value());
    }

    // float val = bluemchen.knobs[0].Value();
    // if(condUpdates[0].Process(val))
    // {
    //     filters[bank].amp = val;
    // }
    
    
}