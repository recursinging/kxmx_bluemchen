#include "daisysp.h"
#include "kxmx_bluemchen.h"
#include <string.h>

using namespace kxmx;
using namespace daisy;
using namespace daisysp;

Bluemchen bluemchen;
DaisySeed seed;

void AudioCallback(float **in, float **out, size_t size)
{
    memcpy(out, in, sizeof(float) * size);
}

int main(void)
{
    seed.Init();
    seed.StartAudio(AudioCallback);
}