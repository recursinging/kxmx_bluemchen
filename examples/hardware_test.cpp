#include "daisysp.h"
#include "../src/kxmx_bluemchen.h"
#include <string.h>

#define TEST_FILE_NAME "kxmx_bluemchen_sdtest.txt"
#define TEST_FILE_CONTENTS "kxmx_bluemchen - Testing microSD read/write functionality."

using namespace kxmx;
using namespace daisy;

Bluemchen bluemchen;

FatFSInterface fsi;
FIL            SDFile;

int enc_val = 0;
int midi_note = 0;
bool sd_test_result = false;

Parameter knob1;
Parameter knob1_dac;
Parameter knob2;
Parameter knob2_dac;
Parameter cv1;
Parameter cv2;


void UpdateOled()
{
    bluemchen.display.Fill(false);

    // Display Encoder test increment value and pressed state
    bluemchen.display.SetCursor(0, 0);
    std::string str = "Enc: ";
    char *cstr = &str[0];
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(enc_val);
    bluemchen.display.SetCursor(30, 0);
    bluemchen.display.WriteString(cstr, Font_6x8, !bluemchen.encoder.Pressed());

    // Display the knob values in millivolts
    str = std::to_string(static_cast<int>(knob1.Value()));
    bluemchen.display.SetCursor(0, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = ":";
    bluemchen.display.SetCursor(30, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(static_cast<int>(knob2.Value()));
    bluemchen.display.SetCursor(36, 8);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    // Display SD test routine result
    str = "SD";
    bluemchen.display.SetCursor(0, 16);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    if (sd_test_result)
    {
        str = "OK";
        bluemchen.display.SetCursor(18, 16);
        bluemchen.display.WriteString(cstr, Font_6x8, false);
    }
    else
    {
        str = "NA";
        bluemchen.display.SetCursor(18, 16);
        bluemchen.display.WriteString(cstr, Font_6x8, true);
    }
    str = ":";
    bluemchen.display.SetCursor(30, 16);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    // Display MIDI input note number
    str = "M:";
    bluemchen.display.SetCursor(36, 16);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    str = std::to_string(static_cast<int>(midi_note));
    bluemchen.display.SetCursor(48, 16);
    bluemchen.display.WriteString(cstr, Font_6x8, true);

    // Display CV input in millivolts
    str = std::to_string(static_cast<int>(cv1.Value()));
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

// Typical Switch case for Message Type.
void HandleMidiMessage(MidiEvent m)
{
    switch (m.type)
    {
    case NoteOn:
    {
        midi_note = m.AsNoteOn().note;
    }
    break;
    case NoteOff:
    {
        midi_note = 0;
    }
    break;
    default:
        break;
    }
}

void UpdateControls()
{
    bluemchen.ProcessAllControls();

    knob1.Process();
    knob2.Process();

    bluemchen.seed.dac.WriteValue(daisy::DacHandle::Channel::ONE, static_cast<uint16_t>(knob1_dac.Process()));
    bluemchen.seed.dac.WriteValue(daisy::DacHandle::Channel::TWO, static_cast<uint16_t>(knob2_dac.Process()));

    cv1.Process();
    cv2.Process();

    enc_val += bluemchen.encoder.Increment();
}

// Mounts SD card and reads TEST_FILE
// returns 0 if successful, else failure.
bool TestSdCard()
{
    int sta;
    char inbuff[2048];
    char refbuff[2048];
    char *buff;
    uint32_t len, bytesread, byteswritten;
    std::string fileName = "BluemchenTestFile.txt";

    memset(inbuff, 0, 2048);
    memset(refbuff, 0, 2048);

    // Init SDMMC
    // SdmmcHandler::Config sd_cfg;
    // sd_cfg.Defaults();
    // sd.Init(sd_cfg);
    fsi.Init(FatFSInterface::Config::MEDIA_SD);
    FATFS& fs = fsi.GetSDFileSystem();
    const char* rootpath = fsi.GetSDPath();
    f_mount(&fs, rootpath, 1);

    // Fill reference buffer with test contents
    sprintf(refbuff, "%s", TEST_FILE_CONTENTS);
    len = strlen(refbuff);

    // Open and write the test file to the SD Card.
    if(f_open(&SDFile, TEST_FILE_NAME, (FA_CREATE_ALWAYS) | (FA_WRITE))
       == FR_OK)
    {
        f_write(&SDFile, refbuff, len, (UINT *)&byteswritten);
        f_close(&SDFile);
    }

    // Re-read from file and compare the contents
    buff = inbuff;
    if (f_open(&SDFile, TEST_FILE_NAME, FA_READ) == FR_OK)
    {
        f_read(&SDFile, buff, len, (UINT *)&bytesread);
    }
    if (len == bytesread && strcmp(buff, refbuff) == 0)
    {
        sta = true;
    }
    else
    {
        sta = false;
    }
    return sta;
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    UpdateControls();
    memcpy(out, in, sizeof(float) * size);
}

int main(void)
{
    bluemchen.Init();
    bluemchen.StartAdc();

    knob1.Init(bluemchen.controls[bluemchen.CTRL_1], 0.0f, 5000.0f, Parameter::LINEAR);
    knob2.Init(bluemchen.controls[bluemchen.CTRL_2], 0.0f, 5000.0f, Parameter::LINEAR);

    knob1_dac.Init(bluemchen.controls[bluemchen.CTRL_1], 0.0f, 4095.0f, Parameter::LINEAR);
    knob2_dac.Init(bluemchen.controls[bluemchen.CTRL_2], 0.0f, 4095.0f, Parameter::LINEAR);

    cv1.Init(bluemchen.controls[bluemchen.CTRL_3], -5000.0f, 5000.0f, Parameter::LINEAR);
    cv2.Init(bluemchen.controls[bluemchen.CTRL_4], -5000.0f, 5000.0f, Parameter::LINEAR);

    bluemchen.StartAudio(AudioCallback);

    // Test the SD card
    sd_test_result = TestSdCard();

    while (1)
    {
        UpdateControls();
        UpdateOled();

        bluemchen.midi.Listen();
        while (bluemchen.midi.HasEvents())
        {
            HandleMidiMessage(bluemchen.midi.PopEvent());
        }
    }
}