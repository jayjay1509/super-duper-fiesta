#ifndef SUPER_DUPER_FIESTA_GAME_INCLUDE_WALLFACTORY_H_
#define SUPER_DUPER_FIESTA_GAME_INCLUDE_WALLFACTORY_H_

#include "body.h"
#include "physics_world.h"
#include "collider.h"
#include "shape.h"
#include <SFML/Graphics.hpp>  // Assurez-vous d'avoir SFML ou une bibliothèque similaire pour le rendu

class WallFactory {
 public:
  explicit WallFactory(crackitos_physics::physics::PhysicsWorld& world)
      : world_(world) {}

  crackitos_physics::physics::BodyHandle CreateWall(const crackitos_core::math::Vec2f& position,
                                                    const crackitos_core::math::Vec2f& size)
  {
    using namespace crackitos_physics::physics;
    using namespace crackitos_core::math;

    // Crée le body statique
    Body wall_body;
    wall_body.set_type(BodyType::Static);
    wall_body.set_position(position);
    wall_body.set_mass(0.0f);

    BodyHandle body_handle = world_.CreateBody(wall_body);
    Vec2f half_size_vec = size * 0.5f;
    Vec2f min_bound = position - half_size_vec; // Coin inférieur gauche
    Vec2f max_bound = position + half_size_vec; // Coin supérieur droit

    float half_size_length = half_size_vec.Magnitude();

    Vec2f centre = position;

    AABB shape(min_bound, max_bound, centre, half_size_vec, half_size_length);

    Collider collider(shape, 0.0f, 0.8f, false, body_handle);
    world_.CreateCollider(body_handle, collider);

    return body_handle;
  }



 private:
  crackitos_physics::physics::PhysicsWorld& world_;
};

#endif // SUPER_DUPER_FIESTA_GAME_INCLUDE_WALLFACTORY_H_
