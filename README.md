# Gainbit
A bit-crusher with a volume slider.

![A screenshot of the plugin loaded in FL Studio, a DAW.](https://i.imgur.com/QKH9Wdj.png)

This plugin uses the [JUCE](https://github.com/juce-framework/JUCE) library.

### Using the plugin
To use the plugin you will need an audio plugin host, like a DAW, that can load plugins from custom paths. \
If that doesn't sound too daunting, head to the [releases page](https://github.com/LensPlaysGames/Gainbit/releases) to download the latest and greatest; check back if you have any issues.

#### Windows
FL Studio requires that all `.vst3` plugins be placed in a single specific folder: \
`C:\Program Files\Common Files\VST3\` \
and adding custom paths to a `.vst3` or `.vst3` directory isn't an option. 

Over in Ableton, custom `.vst3` directories are specifiable, so it's easy to find the right place to put it.

#### MacOSx, Linux
I have not tested anything on macOSx or Linux yet so if you have any comments, concerns, or criticisms, be sure to leave an [issue](https://github.com/LensPlaysGames/Gainbit/issues) or make a [pull request](https://github.com/LensPlaysGames/Gainbit/pulls).

### Building the plugin
If you: are a developer and want to edit the source code and actually build the thing, a PIP is included in the main directory as `Gainbit.h`

Despite the name, this isn't just a header file; it's a [PIP](https://forum.juce.com/t/what-is-a-pip/26821), or Projucer Instant Project.

[Projucer](https://juce.com/discover/stories/projucer-manual) is JUCE's build system generator akin to CMake (although they support that as well, I'm getting there). \
This 'PIP' file is openable in Projucer, and will create a project with build systems for different platforms (Visual Studio, XCode, Makefile).

Once you select the build system of your choice, you can save the project and open it in an IDE. \
From here, you are able to build the plugin, edit the code, and do everything you may need to.

To compile and build the `.vst3`, just build the 'VST3' project using your IDE. This will output a `.vst3`, which is usable in most DAWs and other plugin hosts.

If you are good at customising your debugger, I recommend JUCE's Audio Plugin Host which can be found in the extras folder of the offical JUCE repository linked above.

I don't recommend the standalone build all that much, as there isn't an audio input so there isn't much to hear.
