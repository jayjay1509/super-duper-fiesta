#ifndef BULLET_H_
#define BULLET_H_

#include <SFML/Graphics.hpp>
#include "body.h"
#include <crackitos_physics/physics/include/physics_world.h>

class Bullet {
 public:
  Bullet() : active(false), lifetime(0.f) {}

  void Init(crackitos_physics::physics::PhysicsWorld& world,
            const crackitos_core::math::Vec2f& position,
            const crackitos_core::math::Vec2f& direction) {
    if (active) return;

    active = true;
    this->world = &world;
    lifetime = 6.0f;

    // Créer le corps physique de la balle
    crackitos_physics::physics::Body projBody;
    projBody.set_position(position);
    projBody.set_mass(0.1f);
    body_handle = world.CreateBody(projBody);

    // Créer le collider de la balle
    crackitos_physics::physics::Collider projCollider(
        crackitos_core::math::Circle(position, 5),
        0.98f, 0.1f, false, body_handle
    );
    world.CreateCollider(body_handle, projCollider);

    // Appliquer la force pour la direction
    crackitos_core::math::Vec2f normalizedDirection = direction.Normalized();
    world.GetMutableBody(body_handle).ApplyForce(normalizedDirection * 1000.f);
  }

  void Update(float dt) {
    if (!active) return;

    lifetime -= dt;
    if (lifetime <= 0.f) {
      active = false;
      return;
    }

    world->GetMutableBody(body_handle).Update(dt);
  }

  crackitos_core::math::Vec2f GetPosition() const {
    return world->GetBody(body_handle).position();
  }

  bool IsActive() const { return active; }

 private:
  crackitos_physics::physics::PhysicsWorld* world = nullptr;
  crackitos_physics::physics::BodyHandle body_handle;
  bool active;
  float lifetime;
};

#endif // BULLET_H_
