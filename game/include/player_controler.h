#ifndef PLAYER_CONTROLER_H_
#define PLAYER_CONTROLER_H_

#include <SFML/Graphics.hpp>
#include <crackitos_physics/physics/include/body.h>
#include <crackitos_physics/physics/include/physics_world.h>
#include "bullet.h"

class PlayerController {
 public:
  std::vector<Bullet> bullets;
  PlayerController(float m,sf::Vector2f pos, crackitos_physics::physics::PhysicsWorld& world)
      : move_speed(m), world_(world) {

    crackitos_physics::physics::Body playerBody;
    playerBody.set_position(crackitos_core::math::Vec2f(pos.x, pos.y)) ;
    playerBody.set_mass(1.0f);
    body_handle_ = world_.CreateBody(playerBody);



    crackitos_physics::physics::Collider playerCollider(
        crackitos_core::math::AABB( crackitos_core::math::Vec2f(0.0f, 0.0f), crackitos_core::math::Vec2f(50.0f, 50.0f)),
        0.5f,  // Bounciness (restitution)
        0.5f,  // Friction
        false, // Pas un trigger
        body_handle_ // Handle du corps
    );

    world_.CreateCollider(body_handle_, playerCollider);
  }

  void Update(float dt) {
    // Met à jour le player body
    crackitos_physics::physics::Body& body = world_.GetMutableBody(body_handle_);
    body.Update(dt);

    for (auto it = bullets.begin(); it != bullets.end();) {
      it->Update(dt);  // Met à jour la balle

      if (!it->IsActive()) {
        it = bullets.erase(it);
      } else {
        ++it;
      }
    }
  }

  void Draw(sf::RenderWindow& window) {
    for (const auto& bullet : bullets) {
      if (bullet.IsActive()) {
        sf::CircleShape shape(5.f);  // Rayon de la balle
        shape.setPosition(sf::Vector2f (bullet.GetPosition().x, bullet.GetPosition().y));
        shape.setFillColor(sf::Color::Red);  // Vous pouvez changer la couleur
        window.draw(shape);  // Dessine la balle
      }
    }
  }

  void Move(sf::Vector2f direction) {
    crackitos_physics::physics::Body& body = world_.GetMutableBody(body_handle_);
    body.ApplyForce(crackitos_core::math::Vec2f((direction * move_speed).x, (direction * move_speed).y));
  }

  void pos(sf::Vector2f pos) {
    crackitos_physics::physics::Body& body = world_.GetMutableBody(body_handle_);
    body.set_position(crackitos_core::math::Vec2f(pos.x, pos.y));
  }


  void Shoot(const crackitos_core::math::Vec2f& direction) {
    crackitos_physics::physics::Body& body = world_.GetMutableBody(body_handle_);

    // Normalisation de la direction pour avoir un vecteur unitaire
    crackitos_core::math::Vec2f dir_normalized = direction.Normalized();

    // Distance devant le tank pour spawn la balle (ajuste si nécessaire)
    float offset_distance = 35.0f; // 10 cm devant par exemple

    // Position de spawn décalée vers l'avant
    crackitos_core::math::Vec2f spawn_position = body.position() + dir_normalized * offset_distance;

    Bullet bullet;
    bullet.Init(world_, spawn_position, direction);
    bullets.push_back(bullet);
  }


  crackitos_core::math::Vec2<float> GetPosition() {
    crackitos_physics::physics::Body& body = world_.GetMutableBody(body_handle_);
    return body.position();
  }

 private:
  float move_speed = 3; // todo change le noms et cree
  crackitos_physics::physics::PhysicsWorld& world_;  // Référence vers le monde physique
  crackitos_physics::physics::BodyHandle body_handle_;
};//todo mettre dans cc les move et tout

#endif  // PLAYER_CONTROLER_H_
