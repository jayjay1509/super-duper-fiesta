//
// Created by Mat on 5/5/2025.
//

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "network.h"
#include "client_interface.h"
#include "LoadBalancing-cpp/inc/RoomOptions.h"
#include "my_client.h"

int main() {
  sf::RenderWindow window;
  window.create(sf::VideoMode({800, 600}), "Photon Chat");
  window.setFramerateLimit(60);
  if (!ImGui::SFML::Init(window)) {
    std::cerr << "bad\n";
  };

  MyClient client;
  ExitGames::LoadBalancing::ClientConstructOptions options;
  SDF::NetworkManager::Begin(&client, options);

  std::string inputBuffer;

  sf::Clock deltaClock;
  while (window.isOpen()) {
    while (const auto event = window.pollEvent()) {
      ImGui::SFML::ProcessEvent(window, *event);
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }





    ImGui::SFML::Update(window, deltaClock.restart());

    ImGui::Begin("Chat");

    static char message[256] = "";
    ImGui::InputText("Message", message, IM_ARRAYSIZE(message));

    if (ImGui::Button("Send"))
    {
      std::cout << "[Photon] Trying to send message: " << message << "\n";
      ExitGames::Common::Hashtable data;
      bool success = SDF::NetworkManager::GetLoadBalancingClient().opRaiseEvent(true, ExitGames::Common::JString(message), 1);
      std::cout << "[Photon] Message sent status: " << (success ? "Success" : "Failure") << "\n";
    }


    // Photon service tick
    SDF::NetworkManager::Tick();

    ImGui::End();

    window.clear();
    ImGui::SFML::Render(window);
    window.display();
  }

  SDF::NetworkManager::End();
  ImGui::SFML::Shutdown();
  return 0;
}