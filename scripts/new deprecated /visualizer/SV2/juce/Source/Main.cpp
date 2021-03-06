/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainWindow.h"

#define kWindowWidth 1000
#define kWindowHeight 650

//==============================================================================
class jSpikeApplication  : public JUCEApplication
{
public:
    //==============================================================================
    jSpikeApplication()
    {
    }

    ~jSpikeApplication()
    {
    }

    //==============================================================================
    void initialise (const String& commandLine)
    {
//        #if JUCE_IOS || JUCE_ANDROID
//        theMainWindow.setVisible (true);
//        theMainWindow.setFullScreen (true);
//        #else
        theMainWindow.centreWithSize (kWindowWidth, kWindowHeight);
        theMainWindow.setVisible (true);

//        #endif        
    }

    void shutdown()
    {
        // Do your application's shutdown code here..
        
    }

    //==============================================================================
    void systemRequestedQuit()
    {
        quit();
    }

    //==============================================================================
    const String getApplicationName()
    {
        return "jSpike";
    }

    const String getApplicationVersion()
    {
        return ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed()
    {
        return true;
    }

    void anotherInstanceStarted (const String& commandLine)
    {
        
    }

private:
    MainWindow theMainWindow;
};

//==============================================================================
// This macro generates the main() routine that starts the app.
START_JUCE_APPLICATION(jSpikeApplication)
