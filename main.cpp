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


}