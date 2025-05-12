#ifndef ONLINEGAME_GAME_INCLUDE_MY_CLIENT_H_
#define ONLINEGAME_GAME_INCLUDE_MY_CLIENT_H_

#include <iostream>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "network.h"
#include "client_interface.h"

class MyClient : public SDF::ClientInterface {
 private:

  std::unordered_map<int, sf::Vector2f> directions_;

 public:
  void debugReturn(int /*debugLevel*/, const ExitGames::Common::JString& /*string*/) override {}
  void connectionErrorReturn(int /*errorCode*/) override {}
  void clientErrorReturn(int /*errorCode*/) override {}
  void warningReturn(int /*warningCode*/) override {}
  void serverErrorReturn(int /*errorCode*/) override {}
  void joinRoomEventAction(int /*playerNr*/,
                           const ExitGames::Common::JVector<int>& /*playernrs*/,
                           const ExitGames::LoadBalancing::Player& /*player*/) override {}
  void leaveRoomEventAction(int /*playerNr*/, bool /*isInactive*/) override {}
  void joinRoomReturn(int /*localPlayerNr*/,
                      const ExitGames::Common::JVector<int>& /*playernrs*/,
                      const ExitGames::LoadBalancing::Player& /*player*/) {}
  void leaveRoomReturn(int /*errorCode*/, const ExitGames::Common::JString& /*errorString*/) override {}

  void connectReturn(int errorCode,
                     const ExitGames::Common::JString& errorString,
                     const ExitGames::Common::JString& cluster)
  {
    std::cout << "[Photon] connectReturn code: " << errorCode
              << ", message: " << errorString.UTF8Representation().cstr() << "\n";
    if (errorCode == 0) {
      std::cout << "[Photon] Connected successfully! Joining room...\n";
      ExitGames::LoadBalancing::RoomOptions roomOptions;
      SDF::NetworkManager::GetLoadBalancingClient().opJoinOrCreateRoom(L"test-room", roomOptions);
    }
  }

  void disconnectReturn() override {
    std::cout << "[Photon] Disconnected from Photon Cloud.\n";
  }

  // Réception d’événements custom
  void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override {
    if (eventCode == 1) {
      // exemple de message texte
      auto msg = ExitGames::Common::ValueObject<ExitGames::Common::JString>(eventContent).getDataCopy();
      std::cout << "[Photon] Message from " << playerNr
                << ": " << msg.UTF8Representation().cstr() << "\n";
    }
    else if (eventCode == 2) {
      auto msg = ExitGames::Common::ValueObject<ExitGames::Common::JString>(eventContent).getDataCopy();
      storeDirection(playerNr, msg);
    }
  }

  void roomListUpdate(const ExitGames::Common::JVector<ExitGames::LoadBalancing::Room>& /*roomList*/)  {}
  void roomPropertiesChange(const ExitGames::Common::Hashtable& /*properties*/)  {}
  void playerPropertiesChange(int /*playerNr*/, const ExitGames::Common::Hashtable& /*properties*/)  {}


  void storeDirection(int playerNr, const ExitGames::Common::JString& message) {
    std::string dirStr = message.UTF8Representation().cstr();
    auto commaPos = dirStr.find(',');
    if (commaPos == std::string::npos) return;

    float dx = std::stof(dirStr.substr(0, commaPos));
    float dy = std::stof(dirStr.substr(commaPos + 1));
    directions_[playerNr] = sf::Vector2f{ dx, dy };
  }


  int getLocalPlayerNr() const {
    return SDF::NetworkManager::GetLoadBalancingClient()
        .getLocalPlayer()
        .getNumber();
  }

  std::vector<int> getAllPlayerNrs() const {
    std::vector<int> result;
    // Accède à la room jointe
    const auto& room = SDF::NetworkManager::GetLoadBalancingClient()
        .getCurrentlyJoinedRoom();
    // Photon renvoie un JVector de pointeurs Player*
    ExitGames::Common::JVector<ExitGames::LoadBalancing::Player*> players = room.getPlayers();
    // Itère dessus
    for (unsigned int i = 0; i < players.getSize(); ++i) {
      result.push_back(players[i]->getNumber());
    }
    return result;
  }


  int getRemotePlayerNr() const {
    auto all   = getAllPlayerNrs();
    int  local = getLocalPlayerNr();
    for (int n : all) {
      if (n != local)
        return n;
    }
    return 0;
  }


  sf::Vector2f getDirection(int actorNr) const {
    auto it = directions_.find(actorNr);
    return it == directions_.end() ? sf::Vector2f{300,100} : it->second;
  }


  sf::Vector2f getLocalDirection() const
  {
    int localNr = SDF::NetworkManager::GetLoadBalancingClient().getLocalPlayer().getNumber();
    return getDirection(localNr);
  }



};

#endif // ONLINEGAME_GAME_INCLUDE_MY_CLIENT_H_
