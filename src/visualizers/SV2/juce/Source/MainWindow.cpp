//
//  MainWindow.cpp
//  jSpike
//
//  Created by Stuart Layton on 12/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "MainWindow.h"

MainWindow::MainWindow()
: DocumentWindow ("jSpike",
                  Colours::azure,
                  DocumentWindow::allButtons,
                  true)
{
    setUsingNativeTitleBar(true);
    setResizable (true, false); // resizability is a property of ResizableWindow
    setResizeLimits (400, 300, 8192, 8192);

    setVisible (true);
    oglc1 = new ArteOpenGLComponent("6300"); 
    oglc2 = new ArteOpenGLComponent("6301"); 
    oglc3 = new ArteOpenGLComponent("6302"); 
    oglc4 = new ArteOpenGLComponent("6303"); 
    oglc5 = new ArteOpenGLComponent("6304"); 
    oglc6 = new ArteOpenGLComponent("6305"); 

    //    oglc->initialize("6300");

    addAndMakeVisible(oglc1);
    addAndMakeVisible(oglc2);
    addAndMakeVisible(oglc3);
    addAndMakeVisible(oglc4);
    addAndMakeVisible(oglc5);
    addAndMakeVisible(oglc6);
    
//    myComp.setBounds (RelativeBounds ("otherComp.right + 50, topMarker, left + 100, top + 100"));
    oglc1->setBounds(30,  20, 300, 300);
    oglc2->setBounds(345, 20, 300, 300);
    oglc3->setBounds(660, 20, 300, 300);
    oglc4->setBounds(30,  330, 300, 300);
    oglc5->setBounds(345, 330, 300, 300);
    oglc6->setBounds(660, 330, 300, 300);
    
    grabKeyboardFocus();
    addKeyListener(this);
}

MainWindow::~MainWindow()
{
    deleteAllChildren();
}

void MainWindow::closeButtonPressed()
{
    // The correct thing to do when you want the app to quit is to call the
    // JUCEApplication::systemRequestedQuit() method.
    
    // That means that requests to quit that come from your own UI, or from other
    // OS-specific sources (e.g. the dock menu on the mac) all get handled in the
    // same way.
    
    JUCEApplication::getInstance()->systemRequestedQuit();
}

bool MainWindow::keyPressed (const KeyPress &key, Component *originatingComponent){
    oglc1->processKeyPress(key);
    oglc2->processKeyPress(key);
    oglc3->processKeyPress(key);
    oglc4->processKeyPress(key);
    oglc5->processKeyPress(key);
    oglc6->processKeyPress(key);
}

