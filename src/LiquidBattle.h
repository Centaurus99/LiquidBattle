/*
 * @Author: Tong Haixuan
 * @Date: 2021-05-24 20:30:21
 * @LastEditTime: 2021-06-30 00:39:22
 * @LastEditors: Tong Haixuan
 * @Description: The Main File of LiquidBattle
 */

#ifndef LIQUID_BATTLE_H
#define LIQUID_BATTLE_H

#include <iostream>
#include <vector>
#include <list>
#include "framework/ParticleEmitter.h"

#include "MyDebug.h"
#include "CollisionData.h"
#include "Player.h"
#include "Obstacle.h"
#include "ColorSet.h"

 // A Contact Filter to check collision
class MyContactFilter : public b2ContactFilter {
public:
	// Active when collision happen between body and particle
	bool ShouldCollide(b2Fixture* fixture, b2ParticleSystem* particleSystem, int32 particleIndex) {

		/* Here is the debug information of collision between body and particle. */
// #ifdef DEBUG
// 		if (fixture->GetBody()->GetUserData()) {
// 			if (fixture->GetBody()->GetUserData())
// 				MyDebug::Print(static_cast<CollisionData*>(fixture->GetBody()->GetUserData())->GetFlag());
// 			MyDebug::Print('-');
// 			if (particleSystem->GetUserDataBuffer()[particleIndex])
// 				MyDebug::Print(static_cast<CollisionData*>(particleSystem->GetUserDataBuffer()[particleIndex])->GetFlag());
// 			MyDebug::Print('\n');
// 		}
// #endif

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

		/* Here is the debug information of collision between body and body. */
		if (fixtureA->GetBody()->GetUserData() || fixtureB->GetBody()->GetUserData()) {
			MyDebug::Print("Collision: ");
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
		player_list_.push_back(new Player{ m_world, "wsad", 0, b2Vec2{-1.0f, 10.0f}, ColorSet[1] });
		player_list_.push_back(new Player{ m_world, "ikjl", 1, b2Vec2{1.0f, 10.0f}, ColorSet[2] });

		/* This player3 can be controlled with the small keyboard. */
		// player_list_.push_back(new Player{ m_world, "8546", 2, b2Vec2{1.5f, 10.0f}, ColorSet[0] });

		obstacle_list_.push_back(new Obstacle{ m_world, 0, 5, 2.0f, 1.0f, 0.06f, 2.5f });

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
			// obstacle_list_.push_back(new Obstacle{ m_world, 0, 3, 2.0f, 1.0f, 0.06f, 2.5f });
			player_list_.push_back(new Player{ m_world, "wsad", 0, b2Vec2{-1.0f, 10.0f} });
		}
		if (key == '-') {
			if (!player_list_.empty()) {
				delete* player_list_.rbegin();
				player_list_.pop_back();
			}
		}
	}

	// Print current infomation to screen
	void PrintInformation() {
		/* Operate tips */
		// m_debugDraw.DrawString(5, m_textLine, "Rotate: a,d   Emit: w,s");
		// m_textLine += DRAW_STRING_NEW_LINE;
		for (auto x : player_list_) {
			x->PrintInformation(&m_debugDraw, m_textLine);
		}
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
	}

	static Test* Create() {
		return new LiquidBattle;
	}
};

#endif
