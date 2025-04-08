#ifndef ONLINE_GAME_GAME_INCLUDE_PLAYER_CONTROLLER_H_
#define ONLINE_GAME_GAME_INCLUDE_PLAYER_CONTROLLER_H_

#include "SFML/System/Vector2.hpp"
#include "../../externals/Physics_923/crackitos_physics/physics/include/body.h" //todo demande comment faire autrement c'est pas ouf

class PlayerController {

 public:
  void Update (float dt);
  void Move(sf::Vector2f direction);

 private:
  float move_speed = 3;
  crackitos_physics::physics::Body* body_;
};



#endif //ONLINE_GAME_GAME_INCLUDE_PLAYER_CONTROLLER_H_
