# kxmx_bluemchen

The kxmx_bluemchen is a open source, 4HP, Eurorack module powered by the [Electrosmith Daisy Seed](https://www.electro-smith.com/daisy/daisy).

The Daisy Seed is an embedded DSP development platform with an ARM Cortex-M7 MCU, High fidelity AKM stereo audio codec with up to 24-bit 192kHz, 64MB of SDRAM, and 8MB of flash memory. The Daisy Seed is a formidable platform which already powers a number of reputable Eurorack modules such as the Electrosmith Daisy Patch, the Qubit Surface, and the entire Noise Engineering Versio series.

![kxmx_bluemchen detail](https://kxmx-bluemchen.recursinging.com/media/bluemchen_detail.jpg)

The kxmx_bluemchen is a fully open source hardware design that attempts to expose as many capabilities of the Electrosmith Daisy Seed as possible, while remaining a compact 4HP wide. It features:

* 1 rotary encoder with push-button function
* 1 64x32 pixel OLED display
* 2 potentiometers
* 1 MicroSD card socket
* 1 MIDI TRS input
* 2 CV input
* 2 Audio input
* 2 Audio output

These hardware features can be used, in combination with the open source software libraries, to create the module that's in your head.

Being built on the Electrosmith Daisy Seed, this module leverages the open source libDaisy library for simple, intuitive access to the hardware features.

The DaisySP library provides you with a rich selection of curated DSP building blocks such as:

* Synthesis Methods: Subtractive, Physical Modeling, FM
* Filters: Biquad, State-Variable, Modal, Comb
* Dynamics: Compressor, Limiter, Crossfade
* Effects Processors: Reverb, Delay, Decimate
* Utilities: Math Functions, Signal Conditioning, Aleatoric Generators

If C++ is not your thing, I'm working to make kxmx_bluemchen available as a target for oopsy - Which will allow you to export Max Gen~ patcher to the kxmx_bluemchen hardware.

The kxmx_bluemchen is available as a built module or a DIY kit

![kxmx_bluemchen detail](https://kxmx-bluemchen.recursinging.com/media/bluemchen_diy.jpg)

I’ve put up a [website](https://kxmx-bluemchen.recursinging.com/) and a registration form for anyone genuinely interested in buying a DIY kit or a module
