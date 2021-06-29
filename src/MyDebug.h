/*
 * @Author: Tong Haixuan
 * @Date: 2021-06-26 19:54:04
 * @LastEditTime: 2021-06-30 00:41:22
 * @LastEditors: Tong Haixuan
 * @Description: Debug tools
 */

#ifndef MY_DEBUG_H
#define MY_DEBUG_H

#include <iostream>

class MyDebug {
public:
	template <typename T>
	static void Print(const T content) {
#ifdef DEBUG
		std::cerr << content;
#endif
	}
	static void PrintPos(const b2Vec2& pos, const char* name = "") {
#ifdef DEBUG
		std::cerr << name << ':' << pos.x << ' ' << pos.y << std::endl;
#endif
	}
	static void PrintChangeBegin(const float32& number, const char* name = "") {
#ifdef DEBUG
		std::cerr << name << ':' << number;
#endif
	}
	static void PrintChangeEnd(const float32& number, const char* name = "") {
#ifdef DEBUG
		std::cerr << "-->" << number << std::endl;
#endif
	}
};

#endif