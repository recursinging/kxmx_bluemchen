#include "kxmx_bluemchen.h"
#include <string.h>

using namespace kxmx;

Bluemchen bluemchen;

void AudioCallback(float **in, float **out, size_t size)
{
    memcpy(out, in, sizeof(float) * size);
}

int main(void)
{
    bluemchen.Init();
    bluemchen.StartAudio(AudioCallback);
}