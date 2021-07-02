## LiquidBattle

- [LiquidBattle.h](../src/LiquidBattle.h)
- [LiquidBattle.cpp](../src/LiquidBattle.cpp)

### **[Class]** MyContactFilter

继承自 `b2ContactFilter` 的碰撞监听器，用于在发生碰撞时执行操作。

#### public:

`bool ShouldCollide(b2Fixture* fixture, b2ParticleSystem* particleSystem, int32 particleIndex)`：实体和液体发生碰撞时调用，会执行实体对应的 `UserData` 对应发生液体碰撞的函数。

`bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) `：两个实体间发生碰撞时调用。如果是 `Obstacle` 与下边界墙发生碰撞，则将该 `Obstacle` 记为需要删除。

### **[Class]** LiquidBattle

继承自 `Test`，综合所有元素。

#### private:

`std::vector<Player*> player_list_`：`Player` 列表。

`std::list<Obstacle*> obstacle_list_`：`Obstacle` 列表。

`int32 obstacle_generation_interval_`：生成 `Obstacle` 的间隔时间。

`float32 Random(const float32& l, const float32& r)`：生成区间 [l,r] 中的随机数。

#### public:

`MyContactFilter m_contact_filter_`：碰撞监听器

`LiquidBattle()`：构造函数，创建初始界面，包括两个玩家，一片海洋，一个障碍物。

`void Keyboard(unsigned char key)`：按下按键。

`void KeyboardUp(unsigned char key)`：松开按键。

`void PrintInformation()`：向屏幕中打印每个玩家的信息。

`void Step(Settings* settings)`：执行每一帧的操作。

`float32 GetDefaultViewZoom()`：设定界面缩放默认值。

`static Test* Create()`：创建游戏。