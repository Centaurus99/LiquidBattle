## CollisionData

- [CollisionData.h](../src/CollisionData.h)

### **[Class]** CollisionData

#### private:

`uint32 normal_counter_`：与不属于任何用户组的液体的碰撞计数器。

`uint32 player_counter_`：与属于某个用户组的液体的碰撞计数器。

`uint32 flag_`：用户组。

`bool need_delete_`：记录是否需要删除。

#### public:

`explicit CollisionData(const uint32 flag = 0)`：构造函数。初始化用户组。

`uint32 GetFlag() const`：获取用户组值。

`void SetFlag(uint32 flag)`：设定用户组值。

`Reset()`：重置两个计数器。

`uint32 GetNormalCounter() const`：获取与不属于任何用户组的液体的碰撞计数器。

`uint32 GetPlayerCounter_() const`：获取与属于某个用户组的液体的碰撞计数器。

`void Collide(const uint32& type)`：与用户组为 `type` 的液体发生碰撞。

`bool IsNeedDelete()`：获取是否需要删除。

`void Delete()`：设置需要删除。