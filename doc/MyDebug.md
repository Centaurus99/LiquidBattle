## MyDebug

- [MyDebug.h](../src/MyDebug.h)

### **[Class]** MyDebug

#### public:

`template <typename T> static void Print(const T content)`：向标准错误流输出 `content`。**仅在 `Debug` 版本下启用。**

`static void PrintPos(const b2Vec2& pos, const char* name = "")`：输出名字和坐标。**仅在 `Debug` 版本下启用。**

`static void PrintChangeBegin(const float32& number, const char* name = "")`：格式化输出变量改变的前半部分。**仅在 `Debug` 版本下启用。**

`static void PrintChangeEnd(const float32& number, const char* name = "")`：格式化输出变量改变的后半部分。**仅在 `Debug` 版本下启用。**