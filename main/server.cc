
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/SocketSelector.hpp>


#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include <ranges>

#include "const.h"

int main()
{

    std::vector<std::unique_ptr<sf::TcpSocket>> sockets;
	sockets.reserve(15);
	sf::TcpListener listener;
	sf::SocketSelector socketSelector;

	listener.setBlocking(false);
	const auto listenerStatus = listener.listen(PORT_NUMBER);
	switch(listenerStatus)
	{
	case sf::Socket::Status::Done:
		break;
	default:
		std::cerr << "Error while listening\n";
		return EXIT_FAILURE;
	}
	while (true)
	{
		for (auto& socket : sockets)
		{
			if (socket == nullptr)
				continue;
			if (socket->getLocalPort() == 0)
			{
				socket = nullptr;
			}
		}
		{
			sf::TcpSocket socket;
			socket.setBlocking(false);
			const auto newSocketStatus = listener.accept(socket);
			if(newSocketStatus == sf::Socket::Status::Done)
			{
				auto newSocket = std::make_unique<sf::TcpSocket>(std::move(socket));
				socketSelector.add(*newSocket);
				auto it = std::ranges::find_if(sockets, [](auto& local_socket)
				{
					return local_socket == nullptr;
				});
				if (it != sockets.end())
				{
					*it = std::move(newSocket);
				}
				else
				{
					sockets.push_back(std::move(newSocket));
				}
			}
		}
		if(socketSelector.wait(sf::milliseconds(100)))
		{
			for(auto& socket: sockets)
			{
				if(socket == nullptr)
					continue;
				if(socketSelector.isReady(*socket))
				{
					std::string message;
					message.resize(MAX_MESSAGE_LENGTH, 0);
					size_t actualLength;

					sf::TcpSocket::Status receiveStatus = socket->receive(message.data(), MAX_MESSAGE_LENGTH, actualLength);
					switch(receiveStatus)
					{
					case sf::Socket::Status::Done:
					{
						std::cout << "Message received: " << message << std::endl;
						for(auto& otherSocket : sockets)
						{
							if(otherSocket == nullptr)
							{
								continue;
							}
							size_t sentDataCount;
							sf::TcpSocket::Status sendStatus;
							do
							{
								sendStatus = otherSocket->send(message.data(), actualLength, sentDataCount);
							} while (sendStatus == sf::Socket::Status::Partial);
						}
						break;
					}
					case sf::Socket::Status::Partial:
					{
						std::cerr << "Partial received...\n";
						break;
					}
					case sf::Socket::Status::Error:
					{
						std::cerr << "Error receiving\n";
						break;
					}

					case sf::Socket::Status::NotReady:
					{
						std::cerr << "Not ready on received\n";
						break;
					}
					}
				}
			}
		}

	}

}