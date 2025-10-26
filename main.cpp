#include <SFML/Audio.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ApplicationServices/ApplicationServices.h>
#include <AppKit/AppKit.h>
#include <atomic>

std::atomic<bool> globalKeyPressed(false);


//when key down happens set key pressed to true
CGEventRef myCGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    if (type == kCGEventKeyDown) {
        globalKeyPressed = true;
        CGKeyCode keyCode = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    }
    return event;
}



void doSleep(bool sleep){
    while (sleep == true){
      std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::minutes(5));
    }
  }

int main(){

  using namespace std::this_thread; // sleep_for, sleep_until
  using namespace std::chrono; // nanoseconds, system_clock, seconds
  bool sleeping;

    //get user input regarding their current state
  std::string isWorking = "N";
  std::cout << "Are you working? Y/N: " << std::flush;
  std::cin >> isWorking;

  //check if the mp3 file opens
  sf::Music music;
  if (!music.openFromFile("music/Air Raid Siren Sound Effect.wav")){
    std::cerr << "Failed to load music file\n";
    return -1;
  }


  //doSleep()
  //sleep_until(system_clock::now() + minutes(5));
  

  //yes working then play the noise if no keypoad input every 5 minutes
  if (isWorking == "Y"){

    // Set up filter to only let through key down
    CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown);
    //sets up the actual listener that listens for key down events
    CFMachPortRef eventTap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault, eventMask, myCGEventCallback, NULL);
    if (!eventTap) {
        std::cerr << "Failed to create event tap. Make sure Accessibility permissions are enabled.\n";
        return -1;
    }
    //create a run loop source to listen continuosly for key down
    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    //link the source to the event listener
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    //enable listening of the event (key down)
    CGEventTapEnable(eventTap, true);

    // Key pressed, stop listening and continue
    CGEventTapEnable(eventTap, false);
    CFRunLoopRemoveSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CFRelease(eventTap);
    CFRelease(runLoopSource);
    //std::string warningText = "Warning. Warning. Procrastination Mode enabling in One minute.";
    //std::string command = "say -v Fred -r 200 \"" + warningText + "\"";
    //system(command.c_str());
    std::cout << "Playing siren. Press 's' and Enter to stop.\n";
    music.play();

    //std::this_thread::sleep_for(music.getDuration());
    system("osascript -e 'tell application \"Shortcuts\" to run shortcut \"ZenMode\"'");
    
    //code for the specific stop key 
    char stopKey;
    std::cin >> stopKey;
    if (stopKey == 's') {
      music.stop();
      system("osascript -e 'tell application \"Shortcuts\" to run shortcut \"Disable ZenMode\"'");
      return 0;
    }
  }


}