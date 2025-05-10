//
// Created by Mat on 5/6/2025.
//

#ifndef MICROMACHINE_GAME_INCLUDE_CLIENT_INTERFACE_H_
#define MICROMACHINE_GAME_INCLUDE_CLIENT_INTERFACE_H_



#include <iostream>
#include "LoadBalancing-cpp/inc/Listener.h"

namespace micromachine
{

class ClientInterface : public ExitGames::LoadBalancing::Listener
{
 public:
  ClientInterface() = default;
  ~ClientInterface() override = default;

  // Callback: connexion réussie
  void connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster) override;

  // Callback: déconnexion
  void disconnectReturn() override;

  // Callback: message reçu
  void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override;

  // Callback: réponse à une opération
  void operationReturn(const ExitGames::Photon::OperationResponse& operationResponse);
};

} // namespace micromachine



#endif //MICROMACHINE_GAME_INCLUDE_CLIENT_INTERFACE_H_
