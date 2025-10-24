#include <SFML/Audio.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>


  void doSleep(bool sleep){
    while (sleep == true){
      std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::minutes(5));
    }
  }

int main(){
  using namespace std::this_thread; // sleep_for, sleep_until
  using namespace std::chrono; // nanoseconds, system_clock, seconds
  bool sleeping;

  sf::Music music;
  if (!music.openFromFile("music/World War 2 Air Raid Siren Alarm Sound Effect.wav")){
    std::cerr << "Failed to load music file\n";
    return -1;
  }

  std::string isWorking = "N";
  std::cout << "Are you working? Y/N: ";
  std::cin >> isWorking;

  //doSleep()
  //sleep_until(system_clock::now() + minutes(5));
  

  if (isWorking == "Y"){
    std::string warningText = "Warning. Warning. Procrastination Mode enabling in One minute.";
    std::string command = "say -v Fred -r 200 \"" + warningText + "\"";
    system(command.c_str());
    std::cout << "Playing siren. Press 's' and Enter to stop.\n";
    music.play();

    char stopKey;
    std::cin >> stopKey;
    if (stopKey == 's') {
      music.stop();
      system("osascript -e 'tell application \"Shortcuts\" to run shortcut \"Disable ZenMode\"'");
      return 0;
    }
    std::this_thread::sleep_for(music.getDuration());
    system("osascript -e 'tell application \"Shortcuts\" to run shortcut \"ZenMode\"'");
  }


}