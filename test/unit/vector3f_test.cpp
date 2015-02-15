#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "vector3.h"

TEST_CASE("Vector blank creation/modification", "[Vector3]")
{
  Vector3 v_test;

  REQUIRE(v_test.x == 0.0f);
  REQUIRE(v_test.y == 0.0f);
  REQUIRE(v_test.z == 0.0f);
}

TEST_CASE("Create Vector3 with arguments", "[Vector3(x,y,z)]")
{
  float x = 45.67f;
  float y = 23.89f;
  float z = 108.0f;
  Vector3 v_test(x, y, z);

  REQUIRE(v_test.x == x);
  REQUIRE(v_test.y == y);
  REQUIRE(v_test.z == z);

  Vector3 v_test2();

  REQUIRE(v_test.x == 0);
  REQUIRE(v_test.y == 0);
  REQUIRE(v_test.z == 0);

  const float vect[3] = {12.34f, 23.45f, 34.56f};

  REQUIRE(v_test.x == 12.34f);
  REQUIRE(v_test.y == 23.45f);
  REQUIRE(v_test.z == 34.56f);
}

TEST_CASE("Test vector3 set and get functions", "[Vector3(x1,y1,z1)]")
{
	Vector3 v_test(10.1f, 10.2f, 10.3f);
	float x = v_test.getX();
	REQUIRE(x == 10.1f);
	float y = v_test.getY();
	REQUIRE(y == 10.2f);
	float z = v_test.getZ();
	REQUIRE(z == 10.3f);

	x = 159.654f;
	v_test.setX(x);
	REQUIRE(v_test.getX() == 159.654f);
	REQUIRE(v_test.getY() == 10.2f);
	REQUIRE(v_test.getZ() == 10.3f);

	y = 258.753f;
	v_test.setY(y);
	REQUIRE(v_test.getX() == 159.654f);
	REQUIRE(v_test.getY() == 258.753f);
	REQUIRE(v_test.getZ() == 10.3f);

	z = 357.147f;
	v_test.setZ(z);
	REQUIRE(v_test.getX() == 159.654f);
	REQUIRE(v_test.getY() == 258.753f);
	REQUIRE(v_test.getZ() == 357.147f);

	v_test.Set(10.1f, 10.2f, 10.3f);
	REQUIRE(v_test.getX() == 10.1f);
	REQUIRE(v_test.getY() == 10.2f);
	REQUIRE(v_test.getZ() == 10.3f);
}

TEST_CASE("Test vetor3f equals logic", "[Vector3(x1,y1,z1), Vector3(x2,y2,z2)]")
{
  float x = 12.3f;
  float y = 23.4f;
  float z = 34.5f;
  float x1 = 12.2f;
  Vector3 v_test1(x, y, z);
  Vector3 v_test2(x, y, z);

  REQUIRE(v_test1 == v_test1);
  v_test2.x = x1;
  REQUIRE(v_test2 != v_test1);
}

TEST_CASE("Test Vector 3f Addition and subtraction operaotrs", "[Vector3(x1,y1,z1), Vector3(x2,y2,z2)]")
{
	float x1 = 12.3f;
	float y1 = 23.4f;
	float z1 = 34.5f;
	float x2 = 45.6f;
	float y2 = 56.7f;
	float z2 = 67.8f;
	Vector3 v_test1(x1, y1, z1);
	Vector3 v_test2(x2, y2, z2);

	Vector3 v_solution1(57.9f, 80.1f, 102.3f);
	Vector3 v_solution2;
	v_solution2 = v_test1 + v_test2;
	REQUIRE(v_solution2 == v_solution1);
	v_solution2 = v_solution2 - v_test1;
	REQUIRE(v_solution2 == v_test2);
	v_solution2 += v_test1;
	REQUIRE(v_solution2 == v_solution1);
	v_solution2 -= v_test2;
	REQUIRE(v_solution2 == v_test1);
	v_solution2 = -v_solution1;
	v_solution1.Set(-57.9f, -80.1f, -102.3);
	REQUIRE(v_solution2 == v_solution1);
}

TEST_CASE("Test Vector 3f scalar mutliplication and division operaotrs", "[Vector3(x1,y1,z1), Vector3(x2,y2,z2)]")
{
	float x1 = 12.8f;
	float y1 = 24.8f;
	float z1 = 36.8f;
	Vector3 v_test1(x1, y1, z1);

	Vector3 v_solution1(6.4f, 12.4f, 18.4f);
	Vector3 v_solution2;
	v_solution2 = v_test1 / 2;
	REQUIRE(v_solution2 == v_solution1);
	v_solution1.Set(x1, y1, z1);
	v_solution2 = v_solution2 * 2;
	REQUIRE(v_solution2 == v_solution1);
	v_solution2 /= 2;
	v_solution1.Set(6.4f, 12.4f, 18.4f);
	REQUIRE(v_solution2 == v_solution1);
	v_solution2 *= 2;
	v_solution1.Set(x1, y1, z1);
	REQUIRE(v_solution2 == v_solution1);
}

TEST_CASE("Test Vector 3f Addition and subtraction operaotrs", "[Vector3(x1,y1,z1), Vector3(x2,y2,z2)]")
{
	float x1 = 12.3f;
	float y1 = 23.4f;
	float z1 = 34.5f;
	float x2 = 45.6f;
	float y2 = 56.7f;
	float z2 = 67.8f;
	Vector3 v_test1(x1, y1, z1);
	Vector3 v_test2(x2, y2, z2);

	Vector3 v_solution1(57.9f, 80.1f, 102.3f);
	Vector3 v_solution2;
	v_solution2 = v_test1 + v_test2;
	REQUIRE(v_solution2 == v_solution1);
	v_solution2 = v_solution2 - v_test1;
	REQUIRE(v_solution2 == v_test2);
	v_solution2 += v_test1;
	REQUIRE(v_solution2 == v_solution1);
	v_solution2 -= v_test2;
	REQUIRE(v_solution2 == v_test1);
	v_solution2 = -v_solution1;
	v_solution1.Set(-57.9f, -80.1f, -102.3);
	REQUIRE(v_solution2 == v_solution1);
}

TEST_CASE("Test Vector 3f dot produc", "[Vector3(x1,y1,z1), Vector3(x2,y2,z2)]")
{
	float x1 = 12.3f;
	float y1 = 23.4f;
	float z1 = 34.5f;
	float x2 = 45.6f;
	float y2 = 56.7f;
	float z2 = 67.8f;
	Vector3 v_test1(x1, y1, z1);
	Vector3 v_test2(x2, y2, z2);

	float solution_1 = 4226.76;
	float solution_2;

	solution_2 = v_test1 * v_test2;
	REQUIRE(solution_2 == solution_1);
}

TEST_CASE("Test Vector 3f Addition and subtraction operaotrs", "[Vector3(x1,y1,z1), Vector3(x2,y2,z2)]")
{
	float x1 = 12.3f;
	float y1 = 23.4f;
	float z1 = 34.5f;
	float x2 = 45.6f;
	float y2 = 56.7f;
	float z2 = 67.8f;
	Vector3 v_test1(x1, y1, z1);
	Vector3 v_test2(x2, y2, z2);

	Vector3 solution_1(-363.63, 739026, -369.63);
	Vector3 solution_2;

	solution_2 = v_test1 % v_test2;
	REQUIRE(solution_2 == solution_1);
	solution_2 = v_test1;
	solution_2 %= v_test2;
	REQUIRE(solution_2 == solution_1);
}