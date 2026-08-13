# SWA Tronix 2A

StreetWave Audio — cross-platform JUCE 9 VST3/Standalone leveling amplifier, inspired by classic T4 optical tube hardware.

> This implementation is an original DSP/software emulation. It does not contain proprietary schematics, firmware, or trademarked artwork from the original manufacturer. The default UI uses procedural/vector drawing rather than copied hardware assets.

## Features

- JUCE **9.0.0** pinned through CMake FetchContent.
- VST3 priority + Standalone target for Windows, macOS and Linux.
- Program-dependent optical envelope with fast attack and two-stage release.
- Compress/Limit modes with soft-knee behavior and program-dependent ratio.
- Tube-style even-harmonic coloration and output-transformer saturation, both oversampled.
- Realistic-ish 300 ms VU ballistics with damped needle.
- Gain, Peak Reduction, Mode, Meter and Power exposed through `AudioProcessorValueTreeState`.
- Four themes: photorealistic procedural classic, minimal, neon purple and animated aurora/blob.
- Factory presets with 12 starting points.
- Resizable editor with layout scaling.
- State persistence through APVTS XML.
- GitHub Actions builds Windows/Linux/macOS VST3 + Standalone.

## Build locally

### Linux
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake ninja-build libasound2-dev libfreetype6-dev libfontconfig1-dev libgl1-mesa-dev libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### macOS
```bash
cmake -B build -G Xcode -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Windows
```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The VST3 bundle is emitted under the CMake target output directory. GitHub Actions packages the produced artifacts as workflow artifacts.

## DSP notes

The optical model deliberately avoids a generic threshold/ratio compressor. Peak reduction controls the excitation of the virtual light-cell drive. Gain reduction is derived from a smoothed detector whose attack is ~10 ms and whose release combines a fast recovery component with a seconds-long memory component. The detector time constants are program-dependent and are further modulated by the held optical state, approximating the familiar memory effect of a T4-style optocell.

This is not a component-level SPICE recreation of a specific production revision. Exact hardware matching requires measurement data from a reference unit; the project is structured so those curves can be calibrated later without changing the GUI/API.

## Factory presets

The 12 factory presets are stored in `Source/Presets/FactoryPresets.xml` and embedded into the binary through JUCE `juce_add_binary_data`, so the preset data travels with the plugin and does not depend on a user file path.

## GitHub build artifacts

Push this repository to GitHub. The `Build SWA Tronix 2A` workflow builds three artifacts: Windows x64, macOS universal (arm64 + x86_64), and Linux. For release packaging, add your own code-signing/notarisation certificates and secrets; the workflow intentionally does not fabricate signatures.
