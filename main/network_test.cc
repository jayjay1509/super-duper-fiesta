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

class MyClient : public SDF::ClientInterface {
 public:
  // ✅ Méthodes obligatoires de Listener
  void debugReturn(int /*debugLevel*/, const ExitGames::Common::JString & /*string*/) override {}

  void connectionErrorReturn(int /*errorCode*/) override {}

  void clientErrorReturn(int /*errorCode*/) override {}

  void warningReturn(int /*warningCode*/) override {}

  void serverErrorReturn(int /*errorCode*/) override {}

  void joinRoomEventAction(int /*playerNr*/,
                           const ExitGames::Common::JVector<int> & /*playernrs*/,
                           const ExitGames::LoadBalancing::Player & /*player*/) override {}

  void leaveRoomEventAction(int /*playerNr*/, bool /*isInactive*/) override {}

  void joinRoomReturn(int /*localPlayerNr*/,
                      const ExitGames::Common::JVector<int> & /*playernrs*/,
                      const ExitGames::LoadBalancing::Player & /*player*/) {}

  void leaveRoomReturn(int /*errorCode*/, const ExitGames::Common::JString & /*errorString*/) override {}

  void connectReturn(int errorCode,
                     const ExitGames::Common::JString &errorString,
                     const ExitGames::Common::JString &cluster) {
    std::cout << "Connected to Photon Cloud!\n";
    ExitGames::LoadBalancing::RoomOptions roomOptions;
    SDF::NetworkManager::GetLoadBalancingClient().opJoinOrCreateRoom(L"test-room", roomOptions);
  }

  void disconnectReturn() override {
    std::cout << "Disconnected from Photon Cloud.\n";
  }

  void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object &eventContent) override {
    std::cout << "[Photon] customEventAction triggered!\n";
    if(eventCode == 1)
    {
      auto message = ExitGames::Common::ValueObject<ExitGames::Common::JString>(eventContent).getDataCopy();
      std::cout << "[Photon] Message from player " << playerNr << ": " << message.UTF8Representation().cstr() << std::endl;
    }
  }

  void roomListUpdate(const ExitGames::Common::JVector<ExitGames::LoadBalancing::Room> & /*roomList*/) {}

  void roomPropertiesChange(const ExitGames::Common::Hashtable & /*properties*/) {}

  void playerPropertiesChange(int /*playerNr*/, const ExitGames::Common::Hashtable & /*properties*/) {}

  void connectReturn(int errorCode,
                     const ExitGames::Common::JString &errorString,
                     const ExitGames::Common::JString &region,
                     const ExitGames::Common::JString &cluster) override {

    std::cout << "[Photon] connectReturn code: " << errorCode << ", message: " << errorString.UTF8Representation().cstr() << "\n";
    if(errorCode == 0)
    {
      std::cout << "[Photon] Connected successfully! Joining room...\n";
      ExitGames::LoadBalancing::RoomOptions roomOptions;
      SDF::NetworkManager::GetLoadBalancingClient().opJoinOrCreateRoom(L"test-room", roomOptions);
    }
  }
};

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