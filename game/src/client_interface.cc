//
// Created by Mat on 5/6/2025.
//

#include "client_interface.h"
#include "network.h"

namespace micromachine
{

void ClientInterface::connectReturn(int errorCode, const ExitGames::Common::JString& errorString,
                                    const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster)
{
  if (errorCode == 0)
    std::cout << "Connected to Photon successfully! Region: " << region.UTF8Representation().cstr() << "\n";
  else
    std::cerr << "Failed to connect: " << errorString.UTF8Representation().cstr() << "\n";
}

void ClientInterface::disconnectReturn()
{
  std::cout << "Disconnected from Photon.\n";
}

void ClientInterface::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
{
  if(eventCode == 1)
  {
    auto msg = ExitGames::Common::ValueObject<ExitGames::Common::JString>(eventContent).getDataCopy();
    std::cout << "Message from player " << playerNr << ": " << msg.UTF8Representation().cstr() << "\n";
  }
}

void ClientInterface::operationReturn(const ExitGames::Photon::OperationResponse& operationResponse)
{
  std::cout << "Operation returned. Code: " << operationResponse.getOperationCode() << ", Return Code: " << operationResponse.getReturnCode() << "\n";
}

bool SendMessageToRoom(const ExitGames::Common::JString& msg)
{
  ExitGames::Common::Hashtable data;
  data.put((nByte)1, msg); // clé = 1, valeur = ton message

  return micromachine::NetworkManager::GetLoadBalancingClient().opRaiseEvent(false, data, 1); // 1 = eventCode arbitraire
}


}
