#include "daisysp.h"
#include "kxmx_bluemchen.h"
#include <string.h>

using namespace kxmx;
using namespace daisy;
using namespace daisysp;

Bluemchen bluemchen;

int        freqs[16];

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
};

Filter filters[16];

static void AudioCallback(float **in, float **out, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        float sig = 0.f;
        for(int j = 0; j < 16; j++)
        {
            sig += filters[j].Process(in[0][i]);
        }
        sig *= .06;

        out[0][i] = out[1][i] = out[2][i] = out[3][i] = sig;
    }
}

void InitFreqs()
{
    freqs[0]  = 50;
    freqs[1]  = 75;
    freqs[2]  = 110;
    freqs[3]  = 150;
    freqs[4]  = 220;
    freqs[5]  = 350;
    freqs[6]  = 500;
    freqs[7]  = 750;
    freqs[8]  = 1100;
    freqs[9]  = 1600;
    freqs[10] = 2200;
    freqs[11] = 3600;
    freqs[12] = 5200;
    freqs[13] = 7500;
    freqs[14] = 11000;
    freqs[15] = 15000;
}

void InitFilters(float samplerate)
{
    for(int i = 0; i < 16; i++)
    {
        filters[i].Init(samplerate, freqs[i]);
    }
}

void UpdateOled();
void UpdateControls();

int main(void)
{
    float samplerate;
    bluemchen.Init();
    samplerate = bluemchen.AudioSampleRate();

    InitFreqs();
    InitFilters(samplerate);
    bank = 0;

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

    std::string str  = "Filter Bank";
    char *      cstr = &str[0];
    bluemchen.display.SetCursor(0, 0);
    bluemchen.display.WriteString(cstr, Font_7x10, true);

    str = "";
    for(int i = 0; i < 2; i++)
    {
        str += std::to_string(freqs[i + 4 * bank]);
        str += "  ";
    }

    bluemchen.display.SetCursor(0, 25);
    bluemchen.display.WriteString(cstr, Font_7x10, true);

    str = "";
    for(int i = 2; i < 4; i++)
    {
        str += std::to_string(freqs[i + 4 * bank]);
        str += "  ";
    }

    bluemchen.display.SetCursor(0, 35);
    bluemchen.display.WriteString(cstr, Font_7x10, true);


    bluemchen.display.Update();
}

void UpdateControls()
{
    bluemchen.ProcessAnalogControls();
    bluemchen.ProcessDigitalControls();

    //encoder
    bank += bluemchen.encoder.Increment();
    bank = (bank % 2 + 2) % 2;

    bank = bluemchen.encoder.RisingEdge() ? 0 : bank;

    //controls
    for(int i = 0; i < 2; i++)
    {
        float val = bluemchen.knobs[i].Process();
        if(condUpdates[i].Process(val))
        {
            filters[i + bank * 2].amp = val;
        }
    }
}