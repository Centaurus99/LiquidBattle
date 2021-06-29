/*
 * @Author: Tong Haixuan
 * @Date: 2021-06-26 21:04:22
 * @LastEditTime: 2021-06-30 00:37:24
 * @LastEditors: Tong Haixuan
 * @Description: Obstacle Class
 */

#ifndef OBSTACLE_H
#define OBSTACLE_H

// A class used to manage obstacles in the world
class Obstacle {
private:
	b2World* m_world_;
	b2Body* m_body_;
public:
	Obstacle(b2World* const world, const float32& x, const float32& y, const float32& row,
		const float32& col, const float32& thickness, const float32& density = 2.5f) :
		m_world_{ world } {

		b2BodyDef bd;
		bd.position.Set(x, y);
		bd.type = b2_dynamicBody;
		bd.angularDamping = 0.0f;
		bd.linearDamping = 0.2f;
		m_body_ = world->CreateBody(&bd);
		m_body_->SetUserData(new CollisionData{ 0u });

		float32 delta = thickness + 0.25f;

		{
			b2PolygonShape shape;
			const b2Vec2 vertices[4] = {
				b2Vec2(-row, -col + delta),
				b2Vec2(-row + thickness, -col + delta),
				b2Vec2(-row + thickness, col - delta),
				b2Vec2(-row, col - delta) };
			shape.Set(vertices, 4);
			b2FixtureDef fd;
			fd.shape = &shape;
			fd.restitution = 0.2f;
			fd.density = density;
			fd.friction = 0.5f;
			m_body_->CreateFixture(&fd);
		}
		{
			b2PolygonShape shape;
			const b2Vec2 vertices[4] = {
				b2Vec2(row, col - delta),
				b2Vec2(row - thickness, col - delta),
				b2Vec2(row - thickness, -col + delta),
				b2Vec2(row, -col + delta) };
			shape.Set(vertices, 4);
			b2FixtureDef fd;
			fd.shape = &shape;
			fd.restitution = 0.2f;
			fd.density = density;
			fd.friction = 0.5f;
			m_body_->CreateFixture(&fd);
		}
		{
			b2PolygonShape shape;
			const b2Vec2 vertices[4] = {
				b2Vec2(-row + delta, col),
				b2Vec2(-row + delta, col - thickness),
				b2Vec2(row - delta, col - thickness),
				b2Vec2(row - delta, col) };
			shape.Set(vertices, 4);
			b2FixtureDef fd;
			fd.shape = &shape;
			fd.restitution = 0.2f;
			fd.density = density;
			fd.friction = 0.5f;
			m_body_->CreateFixture(&fd);
		}
		{
			b2PolygonShape shape;
			const b2Vec2 vertices[4] = {
				b2Vec2(row - delta, -col),
				b2Vec2(row - delta, -col + thickness),
				b2Vec2(-row + delta, -col + thickness),
				b2Vec2(-row + delta, -col) };
			shape.Set(vertices, 4);
			b2FixtureDef fd;
			fd.shape = &shape;
			fd.restitution = 0.2f;
			fd.density = density;
			fd.friction = 0.5f;
			m_body_->CreateFixture(&fd);
		}
		float32 mass = 2 * (2 * row + 2 * col - 4 * delta) * thickness * density;
		float32 volume = 4 * row * col;

		/* Obstacle debug information */
		MyDebug::Print(mass);
		MyDebug::Print(" | ");
		MyDebug::Print(volume);
		MyDebug::Print(" | ");
		MyDebug::Print(mass / volume);
		MyDebug::Print('\n');
	}

	Obstacle(const Obstacle& other) = delete;
	Obstacle(Obstacle&& other) = delete;

	~Obstacle() {
		m_world_->DestroyBody(m_body_);
		m_body_ = nullptr;
		m_world_ = nullptr;
	}

	// Get whether obstacle this need to be deleted
	bool IsNeedDelete() {
		return static_cast<CollisionData*>(m_body_->GetUserData())->IsNeedDelete();
	}
};

#endif