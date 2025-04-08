//
// Created by Maintenant prêt on 08.04.2025.
//

#include "PlayerController.h"

void PlayerController::Move(sf::Vector2f direction) {
  body_->ApplyForce(direction*move_speed);
}
void PlayerController::Update(float dt) {
  body_->Update(dt);
}
