/*
 * @Author: Tong Haixuan
 * @Date: 2021-06-26 19:58:44
 * @LastEditTime: 2021-06-26 20:00:23
 * @LastEditors: Tong Haixuan
 * @Description: A class used to record collision
 */

#ifndef COLLISION_DATA_H
#define COLLISION_DATA_H

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

#endif