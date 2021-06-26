/*
 * @Author: Tong Haixuan
 * @Date: 2021-05-24 20:30:21
 * @LastEditTime: 2021-06-26 18:00:03
 * @LastEditors: Tong Haixuan
 * @Description:
 */

#ifndef LIQUID_BATTLE_H
#define LIQUID_BATTLE_H
#include <iostream>
#include <vector>
#include <list>
#include "framework/ParticleEmitter.h"

class MyDebug {
public:
	template <typename T>
	static void Print(const T content) {
		// #ifdef DEBUG
		std::cerr << content;
		// #endif
	}
	static void PrintPos(const b2Vec2& pos, const char* name = "") {
#ifdef DEBUG
		std::cerr << name << ':' << pos.x << ' ' << pos.y << std::endl;
		// printf("%s:%.3f %.3f\n", name, pos.x, pos.y);
#endif
	}
	static void PrintChangeBegin(const float32& number, const char* name = "") {
#ifdef DEBUG
		std::cerr << name << ':' << number;
		// printf("%s:%.3f", name, number);
#endif
	}
	static void PrintChangeEnd(const float32& number, const char* name = "") {
#ifdef DEBUG
		std::cerr << "-->" << number << std::endl;
		// printf("-->%.3f\n", number);
#endif
	}
};

// A class to record the number of collision times with flag in consider
class CollisionData {
private:
	// Record normal partical collision
	uint32 normal_counter_ = 0;
	// Record player partical collision
	uint32 player_counter_ = 0;
	// Player Group
	uint32 flag_;
	// Delete tag
	bool need_delete_ = 0;
public:
	explicit CollisionData(const uint32 flag = 0) : flag_{ flag } {}

	uint32 GetFlag() const {
		return flag_;
	}

	void SetFlag(uint32 flag) {
		flag_ = flag;
	}

	// Reset counter
	void Reset() {
		normal_counter_ = 0;
		player_counter_ = 0;
	}

	// Get the number of normal partical collision
	uint32 GetNormalCounter() const {
		return normal_counter_;
	}

	// Get the number of player partical collision
	uint32 GetPlayerCounter_() const {
		return player_counter_;
	}

	/// Collide!
	/// @param type is used to decide collision's type
	void Collide(const uint32& type) {
		if (type) ++player_counter_;
		else ++normal_counter_;
	}

	// Get whether this need to be deleted
	bool IsNeedDelete() {
		return need_delete_;
	}

	// Mark as need to be deleted
	void Delete() {
		need_delete_ = 1;
	}
};

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
		// b2Vec2 point_velocity = body_->GetLinearVelocityFromWorldPoint(emit_point);

		//Set the position and speed
		SetPosition(emit_point);
		SetVelocity(emit_velocity_ * (-vec) + point_velocity);

		//Some information
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

	// Keyboard keys in order : Move Forward, Move Backward, Turn Left, Turn Right
	char binding_key[5];

	//Initial keyboard state
	u_char keyboard_state_ = 0;

	// Preset health points
	float32 MAX_HP_ = 100.0f;
	// Current health points
	float32 NOW_HP_ = MAX_HP_;
	// Damage per normal particle
	float32 damage_by_normal_particle_ = 0.01f;
	// Damage per player's particle
	float32 damage_by_player_particle_ = 0.1f;

public:
	/// Create a body as a Player and bind keyboard keys
	/// @param world in which world
	/// @param keyboard Keyboard keys to control Player. WARNING: This char array MUST have EXACTLY four elements and one '\0'
	/// @param player_group this player is blong to which group
	/// @param position this player will be set to where
	Player(b2World* const world, const char keyboard[5], const uint32& player_group, const b2Vec2& position) :
		m_world_{ world }, m_player_group_{ player_group } {
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

	// Update keyboard state with a key press or release
	void KeyBoardUpdate(u_char key) {
		for (uint32 i = 0; i < 4; ++i) {
			if (key == binding_key[i]) {
				keyboard_state_ ^= (1 << i);
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
};

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
			// fd.userData = (void*)2;
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
			// fd.userData = (void*)2;
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
			// fd.userData = (void*)2;
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
			// fd.userData = (void*)2;
			m_body_->CreateFixture(&fd);
		}
		float32 mass = 2 * (2 * row + 2 * col - 4 * delta) * thickness * density;
		float32 volume = 4 * row * col;
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

// A Contact Filter to check collision
class MyContactFilter : public b2ContactFilter {
public:
	// Active when collision happen between body and particle
	bool ShouldCollide(b2Fixture* fixture, b2ParticleSystem* particleSystem, int32 particleIndex) {
		// if (fixture->GetBody()->GetUserData()) {
		// 	if (fixture->GetBody()->GetUserData())
		// 		MyDebug::Print(static_cast<CollisionData*>(fixture->GetBody()->GetUserData())->GetFlag());
		// 	MyDebug::Print('-');
		// 	if (particleSystem->GetUserDataBuffer()[particleIndex])
		// 		MyDebug::Print(static_cast<CollisionData*>(particleSystem->GetUserDataBuffer()[particleIndex])->GetFlag());
		// 	MyDebug::Print('\n');
		// }
		if (fixture->GetBody()->GetUserData()) {
			uint32 particle_flag = 0;
			if (!particleSystem->GetUserDataBuffer()[particleIndex] ||
				!(static_cast<CollisionData*>(fixture->GetBody()->GetUserData())->GetFlag() &
					(particle_flag = static_cast<CollisionData*>(particleSystem->GetUserDataBuffer()[particleIndex])->GetFlag()))) {
				static_cast<CollisionData*>(fixture->GetBody()->GetUserData())->Collide(particle_flag);
			}
		}
		return true;
	}

	// Active when collision happen between body and body
	bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) {
		if (fixtureA->GetBody()->GetUserData() || fixtureB->GetBody()->GetUserData()) {
			if (fixtureA->GetBody()->GetUserData())
				MyDebug::Print(static_cast<CollisionData*>(fixtureA->GetBody()->GetUserData())->GetFlag());
			MyDebug::Print('-');
			if (fixtureB->GetBody()->GetUserData())
				MyDebug::Print(static_cast<CollisionData*>(fixtureB->GetBody()->GetUserData())->GetFlag());
			MyDebug::Print('\n');
		}
		CollisionData* UserDataA = static_cast<CollisionData*>(fixtureA->GetBody()->GetUserData());
		CollisionData* UserDataB = static_cast<CollisionData*>(fixtureB->GetBody()->GetUserData());
		if (UserDataA && UserDataB && (UserDataA->GetFlag() ^ UserDataB->GetFlag()) == ~0u) {
			MyDebug::Print("delete!\n");
			UserDataA->Delete();
			UserDataB->Delete();
		}
		return true;
	}
};

class LiquidBattle : public Test {
private:
	std::vector<Player*> player_list_;
	std::list<Obstacle*> obstacle_list_;

	int32 obstacle_generation_interval_ = 0x3ff;

	// Calculate a random number in [l, r].
	float32 Random(const float32& l, const float32& r) {
		float32 x = ((float32)rand() / (float32)RAND_MAX);
		return l + (r - l) * x;
	}

public:
	MyContactFilter m_contact_filter_;

	LiquidBattle() {
		m_world->SetContactFilter(&m_contact_filter_);
		player_list_.push_back(new Player{ m_world, "wsad", 0, b2Vec2{-1.0f, 10.0f} });
		player_list_.push_back(new Player{ m_world, "ikjl", 1, b2Vec2{1.0f, 10.0f} });
		// obstacle_list_.push_back(new Obstacle{ m_world, -7, 5 + 11, 2.0f, 1.0f, 0.06f, 2.5f });
		obstacle_list_.push_back(new Obstacle{ m_world, 0, 5, 2.0f, 1.0f, 0.06f, 2.5f });
		// obstacle_list_.push_back(new Obstacle{ m_world, 7, 5 + 11, 2.0f, 1.0f, 0.06f, 2.5f });
		// player_list_.push_back(new Player{ m_world, "8546", 2, b2Vec2{1.5f, 10.0f} });
		// player_.push_back();

		// body_debug = CreateBody(m_world, 0, 3, 2.0f, 1.0f, 0.06f, 2.5f);
		// CreateBody(m_world, 0, 3, 3.0f, 2.0f, 0.08f, 2.0f);
		// CreateBody(m_world, 5, 5, 2.0f, 1.0f, 0.1f);
		// CreateBody(m_world, -5, 5, 2.0f, 1.0f, 0.1f);

		{
			b2BodyDef bd;
			bd.position.Set(0, 5);
			b2Body* ground = m_world->CreateBody(&bd);
			ground->SetUserData(new CollisionData{ ~0u });

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(-11, -11.5),
					b2Vec2(11, -11.5),
					b2Vec2(11, -10.5),
					b2Vec2(-11, -10.5) };
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}
		}

		{
			b2BodyDef bd;
			bd.position.Set(0, 5);
			b2Body* ground = m_world->CreateBody(&bd);
			// ground->SetUserData(new CollisionData{ ~ });

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(-11, 13),
					b2Vec2(11, 13),
					b2Vec2(11, 14),
					b2Vec2(-11, 14) };
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(-11, -10.6f),
					b2Vec2(-9.5, -10.6f),
					b2Vec2(-9.5, 13.1f),
					b2Vec2(-11, 13.1f) };
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}

			{
				b2PolygonShape shape;
				const b2Vec2 vertices[4] = {
					b2Vec2(9.5, -10.6f),
					b2Vec2(11, -10.6f),
					b2Vec2(11, 13.1f),
					b2Vec2(9.5, 13.1f) };
				shape.Set(vertices, 4);
				ground->CreateFixture(&shape, 0.0f);
			}
		}

		m_particleSystem->SetRadius(0.05f);
		m_particleSystem->SetDensity(1.0f);

		{
			b2PolygonShape shape;
			shape.SetAsBox(9.4f, 3.0f);
			b2ParticleGroupDef pd;
			pd.flags = b2_waterParticle | b2_fixtureContactFilterParticle;
			pd.position.Set(0, -2.3f);
			pd.shape = &shape;
			pd.color.Set(0, 0, 255, 255);
			pd.userData = new CollisionData;
			m_particleSystem->CreateParticleGroup(pd);
		}
	}

	/// Here has a BUG: sometimes main.cpp wil miss some 'Keyboard' or 'KeyboardUp' and forget to call function.
	// Change the keyboard state
	void Keyboard(unsigned char key) {
		// MyDebug::Print(key + 'A' - 'a');
		for (auto x : player_list_) {
			x->KeyBoardUpdate(key);
		}
	}

	// Change the keyboard state
	void KeyboardUp(unsigned char key) {
		// MyDebug::Print(key);
		Keyboard(key);
		if (key == '=') {
			obstacle_list_.push_back(new Obstacle{ m_world, 0, 3, 2.0f, 1.0f, 0.06f, 2.5f });
			// player_list_.push_back(new Player{ m_world, "wsad", 0, b2Vec2{-1.0f, 10.0f} });
		}
		if (key == '-') {
			// delete *player_list_.end();
			// if (body_debug){
			// 	m_world->DestroyBody(body_debug);
			// 	body_debug = nullptr;
			// }
			// if (!player_list_.empty()) {
			// 	delete* player_list_.rbegin();
			// 	player_list_.pop_back();
			// }
		}
	}

	// Print current infomation to screen
	void PrintInformation() {
		for (auto x : player_list_) {
			x->PrintInformation(&m_debugDraw, m_textLine);
		}
		m_debugDraw.DrawString(5, m_textLine, "Rotate: a,d   Emit: w,s");
		m_textLine += DRAW_STRING_NEW_LINE;
	}

	void BeginContact(b2ParticleSystem* particleSystem,
		b2ParticleBodyContact* particleBodyContact) override {
#ifdef DEBUG
		std::cerr << "Contact!\n";
		std::cerr << particleBodyContact << std::endl;
#endif
	}

	void Step(Settings* settings) {
		Test::Step(settings);
		const float32 dt = 1.0f / settings->hz;
		for (auto x : player_list_) {
			x->Maintain(dt);
		}
		for (auto x = obstacle_list_.begin(); x != obstacle_list_.end();) {
			if ((*x)->IsNeedDelete()) {
				delete (*x);
				x = obstacle_list_.erase(x);
			}
			else {
				++x;
			}
		}
		PrintInformation();
		if ((m_stepCount & obstacle_generation_interval_) == obstacle_generation_interval_) {
			obstacle_list_.push_back(new Obstacle{ m_world, Random(-7.0f, 7.0f), 5 + 11, 2.0f, 1.0f, 0.06f, 2.5f });
			MyDebug::Print("Generate!\n");
		}
		int32 cnt = 0;
		for (auto x : player_list_) {
			if (x->GetHP() <= 0.0f) {
				settings->pause = 1;
				//Print Player Die
				char buffer[256];
				sprintf(buffer, "Player %d died.", cnt);
				m_debugDraw.DrawString(5, m_textLine, buffer);
				m_textLine += DRAW_STRING_NEW_LINE;
			}
			++cnt;
		}
	}

	float32 GetDefaultViewZoom() const {
		return 0.35f;
		// return 0.65f;
	}

	static Test* Create() {
		return new LiquidBattle;
	}
};

#endif
