## Player

- [Player.h](../src/Player.h)

### **[Class]** Ejector

继承自 `RadialEmitter`，作为 `Player`  的喷射器。

#### private:

`class ParticleLifetimeSetter`：创建液体粒子时的回调函数，继承自 `EmittedParticleCallback`，用于设置液体喷射的参数。

`float32 timestep_`：单帧时间。在一些函数中会进行自动更新。

`b2Body* body_`：所属 `body` 的指针。

`int32 capacity_`：喷射背包容量/当前量。

`float32 emit_velocity_`：液体喷射速率。

`float32 emit_rate_`：每秒喷出的液体颗粒量

`float32 emit_point_distance_extension`：确定喷射点时的放大倍率。

`float32 default_dw`：默认旋转角速度增/减量。

`b2Vec2 RayCast(const float32 angle)`：通过倍增取得沿喷射器方向 `body` 以外的点，从而连线计算与 `body` 边界交点。返回交点坐标。

`void Update(const float32& delta_angle)`：每帧更新喷射点和喷射方向。

#### public:

`explicit Ejector(b2Body* const body)`：构造函数。依托于一个 `body`，创建喷射器，并给这个 `body` 添加 `UserData`。

`~Ejector()`：析构函数，并删除 `UserData`。

`ParticleLifetimeSetter* GetCallback() const`：获取回调函数。

`CollisionData* GetUserData() const`：获取 `body` 中创建的 `UserData`。

`b2Body* GetBody()`：获取 `body` 指针。

`void SetTimeStep(const float32& dt)`：更新单帧时间。

`void SetCapacity(const int32& capacity)`：设置背包容量。

`void SetLifetime(const float32& lifetime)`：设置喷射粒子的生命时间。

`void SetBodyPlayerFlag(const uint32& flag)`：设置 `body` 用户组。

`void SetParticlePlayerFlag(const uint32& flag)`：设置 `ParticleSystem` 用户组。

`void Emit(const int direction)`：根据 `emit_rate_` 和背包容量进行喷射。

`void EmitForward()`：玩家向前喷射（效果为玩家后退）。

`void EmitBackward()`：玩家向后喷射（效果为玩家前进）。

`void Rotate(const float32& dw)`：将旋转角速度增加 `dw`。

`void LeftRotate()`：逆时针转。

`void RightRotate()`：顺时针转。

`void PrintInformation(char* const buffer)`：将喷射器信息格式化为字符串。

### **[Class]** Player

#### private:

`b2World* const m_world_`：玩家所属 `world`。

`Ejector* m_ejector_`：玩家所绑定的喷射器。

`uint32 m_player_group_`：玩家用户组。

`b2Color m_color_`：玩家颜色。

`char binding_key[5]`：玩家键盘映射表。

`char keyboard_state_`：记录键盘状态。

`float32 MAX_HP_`：玩家初始血量。

`float32 NOW_HP_`：玩家当前血量。

`float32 damage_by_normal_particle_`：每个不属于任何用户组的液体粒子对玩家造成的伤害。

`float32 damage_by_player_particle_`：每个其他用户组的液体粒子对玩家造成的伤害。

#### public:

`Player(b2World* const world, const char keyboard[5], const uint32& player_group, const b2Vec2& position, const b2Color& color = { 0.9f, 0.7f, 0.7f })`：构造函数。参数分别为所属 `world`、键盘映射表、用户组、初始位置、颜色。

`~Player()`：析构函数。删除喷射器和玩家 `body` 实体。

`void KeyBoardOperator()`：按键盘状态执行对应操作。

`void DamageCalculater(const CollisionData* data)`：将伤害结算至玩家生命值。

`void KeyBoardPress(char key)`：按下按键。

`void KeyBoardRelease(char key)`：松开按键。

`void Maintain(const float32& dt)`：每帧执行，维护和更新玩家状态，执行玩家动作。

`void PrintInformation(DebugDraw* debug_draw, int& text_line)`：将玩家信息显示至屏幕。

`Ejector* GetEjector() const`：获取绑定的喷射器的指针。

`float32 GetHP() const`：获取玩家生命值。

`void SetColor(const b2Color& color)`：设置玩家颜色。