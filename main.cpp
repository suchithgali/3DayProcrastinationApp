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
  @autoreleasepool{
    //using the sharedWorkspace instance of the NSWorkspace class, get the running applications and save the memory locations in a list
    /*NSArray<NSRunningApplication *> *apps = [[NSWorkspace sharedWorkspace] runningApplications];
    
    for (NSRunningApplication *app in apps){
      std::cout << [[app localizedName] UTF8String] << std::endl;
    }
      */
    
  }

  using namespace std::this_thread; // sleep_for, sleep_until
  using namespace std::chrono; // nanoseconds, system_clock, seconds
  bool sleeping;

  //check if the mp3 file opens
  sf::Music music;
  if (!music.openFromFile("music/Air Raid Siren Sound Effect.wav")){
    std::cerr << "Failed to load music file\n";
    return -1;
  }

  //get user input regarding their current state
  std::string isWorking = "N";
  std::cout << "Are you working? Y/N: " << std::flush;
  std::cin >> isWorking;

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

    std::thread runLoopThread([]() {
        CFRunLoopRun();
    });

    while (true) {
      sleep_until(system_clock::now() + minutes(1));
      std::cout << "Checking key press for 5 seconds" << std::endl;
      CFRunLoopRunInMode(kCFRunLoopDefaultMode, 5, false);
      std::cout << "Done checking" << std::endl;
      NSRunningApplication *frontApp = [[NSWorkspace sharedWorkspace] frontmostApplication];
      std::string appName = [[frontApp localizedName] UTF8String];

      if (globalKeyPressed == false || appName != "Code"){
        if (appName == "Google Chrome"){
          char buf[512];
          std::string url;

          // Get full URL from Chrome active tab
          FILE* pipe = popen("osascript -e 'tell application \"Google Chrome\" to get URL of active tab of front window'", "r");
          if (!pipe) return 1;
          while (fgets(buf, sizeof(buf), pipe)) url += buf;
          pclose(pipe);

          if (!url.empty() && url.back() == '\n') url.pop_back();

          // Remove protocol (https:// or http://)
          size_t pos = url.find("://");
          if (pos != std::string::npos) url = url.substr(pos + 3);

          // Keep only the domain up to first slash
          pos = url.find('/');
          if (pos != std::string::npos) url = url.substr(0, pos);

          if (url == "catcourses.ucmerced.edu"){
            continue;
          }
        }
        std::cout << "Playing siren. Press any key to stop.\n";
        music.play();
        system("osascript -e 'tell application \"Shortcuts\" to run shortcut \"ZenMode\"'");
        while (globalKeyPressed == false){
          CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.1, false);
        }
        music.stop();
        system("osascript -e 'tell application \"Shortcuts\" to run shortcut \"Disable ZenMode\"'");
        globalKeyPressed = false;
      }
      else{
      // Key pressed, stop listening and continue
      music.stop();
      CGEventTapEnable(eventTap, false);
      CFRunLoopRemoveSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
      CFRelease(eventTap);
      CFRelease(runLoopSource);
      system("osascript -e 'tell application \"Shortcuts\" to run shortcut \"Disable ZenMode\"'");
      globalKeyPressed = false;
      continue;
      }
    }
    //std::string warningText = "Warning. Warning. Procrastination Mode enabling in One minute.";
    //std::string command = "say -v Fred -r 200 \"" + warningText + "\"";
    //system(command.c_str());
      return 0;
    }
}

