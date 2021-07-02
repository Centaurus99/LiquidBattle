## Obstacle

- [Obstacle.h](../src/Obstacle.h)

### **[Class]** Obstacle

#### private:

`b2World* m_world_`：记录该 `Obstacle` 所属 `world`。用于析构时删除该 `Obstacle` 实体。

`b2Body* m_body_`：记录该 `Obstacle` 对应 `body` 实体。

#### public:

`Obstacle(b2World* const world, const float32& x, const float32& y, const float32& row, const float32& col, const float32& thickness, const float32& density = 2.5f)`：构造函数，参数分别是该 `Obstacle` 所属 `world`、生成位置、长宽、壁厚、密度。

`~Obstacle()`：析构函数，在 `world` 中删除对应 `body` 实体。

`bool IsNeedDelete()`：返回是否需要删除。通过获取对应 `body` 中存储的 `UserData` 判断。