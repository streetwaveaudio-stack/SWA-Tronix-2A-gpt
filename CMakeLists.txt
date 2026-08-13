cmake_minimum_required(VERSION 3.24)
project(SWATronix2A VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 9.0.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)
juce_add_binary_data(SWAPresets SOURCES Source/Presets/FactoryPresets.xml)

juce_add_plugin(SWATronix2A
    COMPANY_NAME "StreetWave Audio"
    BUNDLE_ID com.streetwaveaudio.swatronix2a
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE StrW
    PLUGIN_CODE St2A
    FORMATS VST3 Standalone
    PRODUCT_NAME "SWA Tronix 2A"
    DESCRIPTION "Program-dependent optical leveling amplifier inspired by classic T4-cell hardware."
    VST3_CATEGORIES Dynamics Compressor
    VERSION ${PROJECT_VERSION}
)

target_sources(SWATronix2A PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DSP/OpticalCell.cpp
    Source/DSP/TubeStage.cpp
    Source/DSP/OutputTransformer.cpp
    Source/GUI/ThemeManager.cpp
    Source/GUI/ThemeLookAndFeel.cpp
    Source/GUI/Views.cpp
    Source/Presets/PresetManager.cpp
)

target_include_directories(SWATronix2A PRIVATE Source)

target_compile_definitions(SWATronix2A PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(SWATronix2A PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_opengl
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
    SWAPresets
)

if (WIN32)
    target_compile_definitions(SWATronix2A PRIVATE _USE_MATH_DEFINES NOMINMAX)
endif()

set_target_properties(SWATronix2A PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
)
