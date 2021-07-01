/*
 * @Author: Tong Haixuan
 * @Date: 2021-06-26 20:29:11
 * @LastEditTime: 2021-07-01 23:43:08
 * @LastEditors: Tong Haixuan
 * @Description: Player Class
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "CollisionData.h"
#include "framework/ParticleEmitter.h"

 // Emit particles from a body
class Ejector : public RadialEmitter {
private:
	// Callback used to notify the user of created particles.
	class ParticleLifetimeSetter : public EmittedParticleCallback {
	public:
		//Destruct User Data
		virtual ~ParticleLifetimeSetter() {
			delete user_data_;
		};

		//Get User Data
		CollisionData* GetUserData() const {
			return user_data_;
		}

		//Set User Data
		void SetUserData(CollisionData* const user_data) {
			user_data_ = user_data;
		}

		//Set Particle Lifetime
		void SetLifetime(const float32& lifetime) {
			lifetime_ = lifetime;
		}

		// Called for each created particle.
		virtual void ParticleCreated(b2ParticleSystem* const system,
			const int32 particleIndex) {
			system->SetParticleLifetime(particleIndex, lifetime_);
			system->GetUserDataBuffer()[particleIndex] = user_data_;
		}
	private:
		// Lifetime of emitted particle
		float32 lifetime_ = 10.0f;
		// Recorder of Collision, every Particle share same Recorder
		CollisionData* user_data_ = nullptr;
	};

	// Timestep in the world. Please keep in sync.
	float32 timestep_ = 1.0f / 60.0f;
	// Pointer to a body
	b2Body* body_ = nullptr;
	// Capacity of Ejector
	int32 capacity_ = 0;
	// The Velocity of particles
	float32 emit_velocity_ = 80.0f;
	// The number of particles emitted per second
	float32 emit_rate_;
	// Slightly extend the distance between the emit point and the body. This is the multiplier.
	float32 emit_point_distance_extension_ = 1.0000000f;
	// Default delta angular velocity
	float32 default_dw = 1.0f;

	/// Calculate the first intersection of a ray from body position with the edge
	/// Return a vector from body'position to intersection
	/// @param angle the angle of the ray
	b2Vec2 RayCast(const float32 angle) {

		static float32 RayCastPreset = 0.1f;

		b2Fixture* fixture = body_->GetFixtureList();
		b2Vec2 p2 = body_->GetPosition();
		b2Vec2 p1 = p2 + RayCastPreset * b2Rot{ angle }.GetXAxis();

		//Extend until p1 is not in the body
		while (fixture->TestPoint(p1)) {
			MyDebug::PrintChangeBegin(RayCastPreset, "RayCastPreset");
			RayCastPreset *= 2.0f;
			MyDebug::PrintChangeEnd(RayCastPreset, "RayCastPreset");
			p1 = p2 + RayCastPreset * b2Rot{ angle }.GetXAxis();
		}

		b2RayCastOutput ray_cast_result;
		fixture->RayCast(&ray_cast_result, b2RayCastInput{ p1, p2, 1.0f }, 0);
		return p1 + ray_cast_result.fraction * (p2 - p1) - p2;
	}

	// Keep emit point update
	void Update(const float32& delta_angle) {
		//Determine the emit point and emit velocity
		b2Vec2 vec = RayCast(body_->GetAngle() + delta_angle);
		b2Vec2 emit_point = body_->GetPosition() + vec * emit_point_distance_extension_;
		vec.Normalize();

		//The speed of the reference frame to which the current emit point belongs
		b2Vec2 point_velocity = body_->GetLinearVelocity();

		//Set the position and speed
		SetPosition(emit_point);
		SetVelocity(emit_velocity_ * (-vec) + point_velocity);

		/* Some debug information */
		// MyDebug::PrintPos(GetPosition() - body_->GetPosition(), "DeltaPosition");
		// MyDebug::PrintPos(GetPosition(), "EmitPosition");
		// MyDebug::PrintPos(GetVelocity(), "EmitVelocity");
		// MyDebug::PrintPos(body_->GetLinearVelocity(), "BodyVelocity");
		// MyDebug::PrintPos(point_velocity, "PointVelocity");
	}

public:
	// Bind an ejector with a body
	explicit Ejector(b2Body* const body) :
		body_{ body } {
		SetCallback(new ParticleLifetimeSetter);
		body_->SetUserData(new CollisionData);
	}

	Ejector(const Ejector& other) = delete;
	Ejector(Ejector&& other) = delete;

	/// Destruct UserData
	/// WARNING: Ejector must be destructed before body being destructed
	~Ejector() {
		delete static_cast<CollisionData*>(body_->GetUserData());
		body_->SetUserData(nullptr);
	}

	// Get Callback
	ParticleLifetimeSetter* GetCallback() const {
		return static_cast<ParticleLifetimeSetter*>(RadialEmitter::GetCallback());
	}

	// Get the UserData
	CollisionData* GetUserData() const {
		return static_cast<CollisionData*>(body_->GetUserData());
	}

	// Get belonging body
	b2Body* GetBody() const {
		return body_;
	}

	// Use this to keep timestep in sync with world
	void SetTimeStep(const float32& dt) {
		timestep_ = dt;
	}

	// Set the capacity of ejector
	void SetCapacity(const int32& capacity) {
		capacity_ = capacity;
	}

	//Set Particle Lifetime
	void SetLifetime(const float32& lifetime) {
		GetCallback()->SetLifetime(lifetime);
	}

	//Set body flag to destinguish between players
	void SetBodyPlayerFlag(const uint32& flag) {
		GetUserData()->SetFlag(flag);
	}

	//Set particle flag to destinguish between players
	void SetParticlePlayerFlag(const uint32& flag) {
		if (GetCallback()->GetUserData()) {
			GetCallback()->GetUserData()->SetFlag(flag);
		}
		else {
			GetCallback()->SetUserData(new CollisionData(flag));
		}
	}

	/// Emit the particles
	/// @param direction 0 is forward, 1 is backward
	void Emit(const int direction) {
		Update(b2_pi * direction);
		if (GetParticleSystem()->GetParticleCount() + GetEmitRate() * timestep_ <= capacity_) {
			RadialEmitter::Step(timestep_, NULL, 0);
		}
	}
	// Emit the particles forward
	void EmitForward() {
		Emit(0);
	}
	// Emit the particles backward
	void EmitBackward() {
		Emit(1);
	}

	/// Change the angular velocity of body
	/// @param dw delta angular velocity, postive is left, negtive is right
	void Rotate(const float32& dw) {
		// MyDebug::PrintChangeBegin(body_->GetAngularVelocity(), "AngularVelocity");
		body_->SetAngularVelocity(body_->GetAngularVelocity() + dw);
		// MyDebug::PrintChangeEnd(body_->GetAngularVelocity(), "AngularVelocity");
	}
	// Anticlockwise rotate with default default_dw
	void LeftRotate() {
		Rotate(default_dw);
	}
	// Clockwise rotate with default default_dw
	void RightRotate() {
		Rotate(-default_dw);
	}

	// Print information to buffer
	void PrintInformation(char* const buffer) {
		//Print Remaining Water
		sprintf(buffer, "Remaining water: %3d/%3d",
			capacity_ - GetParticleSystem()->GetParticleCount(), capacity_);
	}
};

// Player class, combine Body, Ejector, Keyboard, and Player State
class Player {
private:
	// The world this player belong to
	b2World* const m_world_;

	// Ejector and body binded to it
	Ejector* m_ejector_;

	//Which group this player belong to
	uint32 m_player_group_;

	//Player color
	b2Color m_color_;

	// Keyboard keys in order : Move Forward, Move Backward, Turn Left, Turn Right
	char binding_key[5];

	//Initial keyboard state
	char keyboard_state_ = 0;

	// Preset health points
	float32 MAX_HP_ = 100.0f;
	// Current health points
	float32 NOW_HP_ = MAX_HP_;
	// Damage per normal particle
	float32 damage_by_normal_particle_ = 0.005f;
	// Damage per player's particle
	float32 damage_by_player_particle_ = 0.1f;

public:
	/// Create a body as a Player and bind keyboard keys
	/// @param world in which world
	/// @param keyboard Keyboard keys to control Player. WARNING: This char array MUST have EXACTLY four elements and one '\0'
	/// @param player_group this player is blong to which group
	/// @param position this player will be set to where
	/// @param color the color of this player
	Player(b2World* const world, const char keyboard[5], const uint32& player_group, const b2Vec2& position, const b2Color& color = { 0.9f, 0.7f, 0.7f }) :
		m_world_{ world }, m_player_group_{ player_group }, m_color_{ color } {
		// Bind keyboard
		memcpy(binding_key, keyboard, sizeof(char) * 5);

		// Create body
		b2BodyDef bodydef;
		bodydef.position = position;
		bodydef.type = b2_dynamicBody;
		bodydef.angularDamping = 15.0f;
		bodydef.linearDamping = 0.3f;
		bodydef.angle = b2_pi / 2.0f;
		b2Body* body = world->CreateBody(&bodydef);
		{
			b2CircleShape shape;
			shape.m_radius = 0.5f;
			b2FixtureDef fixturedef;
			fixturedef.shape = &shape;
			fixturedef.restitution = 0.2f;
			fixturedef.density = 1.0f;
			body->CreateFixture(&fixturedef);
		}

		// Create ejector
		m_ejector_ = new Ejector{ body };
		m_ejector_->SetLifetime(5.0f);
		m_ejector_->SetBodyPlayerFlag(1 << player_group);
		m_ejector_->SetParticlePlayerFlag(1 << player_group);
		m_ejector_->SetParticleFlags(b2_waterParticle | b2_fixtureContactFilterParticle);
		m_ejector_->SetEmitRate(60.0f);
		m_ejector_->SetSize(b2Vec2{ 0.00f, 0.0f });
		m_ejector_->SetColor(b2ParticleColor(255, 255, 255, 255));
		m_ejector_->SetCapacity(200);

		// Create particle system for ejector
		b2ParticleSystemDef psd;
		psd.radius = 0.05f;
		psd.destroyByAge = true;
		psd.lifetimeGranularity = 0.1f;
		m_ejector_->SetParticleSystem(world->CreateParticleSystem(&psd));

		SetColor(m_color_);
	}

	Player(const Player& other) = delete;
	Player(Player&& other) = delete;

	// Destruct Player's Body and ParticleSystem
	~Player() {
		b2Body* body = m_ejector_->GetBody();
		b2ParticleSystem* ps = m_ejector_->GetParticleSystem();
		delete m_ejector_;
		m_world_->DestroyBody(body);
		m_world_->DestroyParticleSystem(ps);
		body = nullptr;
		ps = nullptr;
	}

	// Operate according to current keyboard state
	void KeyBoardOperator() {
		// MyDebug::PrintKeyBoard(keyboard_state_);
		if (keyboard_state_ & (1 << 0)) {
			m_ejector_->EmitBackward();
		}
		if (keyboard_state_ & (1 << 1)) {
			m_ejector_->EmitForward();
		}
		if (keyboard_state_ & (1 << 2)) {
			m_ejector_->LeftRotate();
		}
		if (keyboard_state_ & (1 << 3)) {
			m_ejector_->RightRotate();
		}
	}

	// Take damage
	void DamageCalculater(const CollisionData* data) {
		NOW_HP_ -= damage_by_normal_particle_ * data->GetNormalCounter();
		NOW_HP_ -= damage_by_player_particle_ * data->GetPlayerCounter_();
	}

	// Update keyboard state with a key press
	void KeyBoardPress(char key) {
		for (uint32 i = 0; i < 4; ++i) {
			if (key == binding_key[i]) {
				keyboard_state_ |= (1 << i);
			}
		}
	}

	// Update keyboard state with a key release
	void KeyBoardRelease(char key) {
		for (uint32 i = 0; i < 4; ++i) {
			if (key == binding_key[i]) {
				keyboard_state_ &= (~0) ^ (1 << i);
			}
		}
	}

	/// Maintain player state. This is need to be called in each Timestep
	/// @param dt seconds that have passed
	void Maintain(const float32& dt) {
		KeyBoardOperator();
		m_ejector_->SetTimeStep(dt);
		DamageCalculater(m_ejector_->GetUserData());
		m_ejector_->GetUserData()->Reset();
	}

	// Print information to screen
	void PrintInformation(DebugDraw* debug_draw, int& text_line) {
		char buffer[256];
		//Print Player Group
		sprintf(buffer, "Player %d:  ", m_player_group_);
		//Print Remaining Water
		m_ejector_->PrintInformation(buffer + strlen(buffer));
		//Print Health Points
		sprintf(buffer + strlen(buffer), "    HP: %3.0f/%3.0f", NOW_HP_, MAX_HP_);
		debug_draw->DrawString(5, text_line, buffer);
		text_line += DRAW_STRING_NEW_LINE;
	}

	// Get this player's ejector
	Ejector* GetEjector() const {
		return m_ejector_;
	}

	// Get this player's HP
	float32 GetHP() const {
		return NOW_HP_;
	}

	// Set the color of this player
	void SetColor(const b2Color& color) {
		m_color_ = color;
		m_ejector_->SetColor(color);
		m_ejector_->GetBody()->SetColor(color);
	}
};

#endif