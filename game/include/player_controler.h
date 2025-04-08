#ifndef PLAYER_CONTROLER_H_
#define PLAYER_CONTROLER_H_

#include <SFML/Graphics.hpp>
#include <crackitos_physics/physics/include/body.h>
#include <crackitos_physics/physics/include/physics_world.h>

class PlayerController {
 public:
  PlayerController(float m, crackitos_physics::physics::PhysicsWorld& world)
      : move_speed(m), world_(world) {


    crackitos_physics::physics::Body playerBody;
    playerBody.set_position(crackitos_core::math::Vec2f(100.f, 200.f)) ;
    playerBody.set_mass(1.0f);
    body_handle_ = world_.CreateBody(playerBody);

    // Vous pouvez également ajouter un collider si nécessaire

    crackitos_physics::physics::Collider playerCollider(
        crackitos_core::math::AABB( crackitos_core::math::Vec2f(0.0f, 0.0f), crackitos_core::math::Vec2f(10.0f, 10.0f)),
        0.5f,  // Bounciness (restitution)
        0.3f,  // Friction
        false, // Pas un trigger
        body_handle_ // Handle du corps
    );

    world_.CreateCollider(body_handle_, playerCollider);
  }

  void Update(float dt) {
    // Met à jour le body dans le monde physique
    crackitos_physics::physics::Body& body = world_.GetMutableBody(body_handle_);
    body.Update(dt);
  }

  void Move(sf::Vector2f direction) {
    crackitos_physics::physics::Body& body = world_.GetMutableBody(body_handle_);
    body.ApplyForce(crackitos_core::math::Vec2f((direction * move_speed).x, (direction * move_speed).y));
  }

  crackitos_core::math::Vec2<float> GetPosition() {
    crackitos_physics::physics::Body& body = world_.GetMutableBody(body_handle_);
    return body.position();
  }

 private:
  float move_speed = 3;
  crackitos_physics::physics::PhysicsWorld& world_;  // Référence vers le monde physique
  crackitos_physics::physics::BodyHandle body_handle_;  // Handle vers le body du joueur
};

#endif  // PLAYER_CONTROLER_H_
