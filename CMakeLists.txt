cmake_minimum_required(VERSION 3.15)

project(AutoGainer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# Add JUCE as a subdirectory
add_subdirectory(F:/JUCE/JUCE ${CMAKE_BINARY_DIR}/JUCE)

juce_add_plugin(AutoGainer
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE Null
    NEEDS_MIDI_INPUT FALSE
    FORMATS VST3 Standalone
    PRODUCT_NAME "AutoGainer"
    VST3_CATEGORIES "Plugin"
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(AutoGainer
    PRIVATE
        src/PluginEditor.cpp
        src/PluginEditor.h
        src/PluginProcessor.cpp
        src/PluginProcessor.h
)

target_link_libraries(AutoGainer
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compiler definitions
target_compile_definitions(AutoGainer
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)