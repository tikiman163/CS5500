#ifndef _VECTOR3_H_
#define _VECTOR3_H_

// Implementation found on http://www.terathon.com/code/vector3d.html
class Vector3
{
public:
  float x;
  float y;
  float z;

  Vector3();

  Vector3(const float r, const float s, const float t);

  Vector3(const float vect[3]);

  const float getX();

  const float getY();

  const float getZ();

  void setX(const float newX);

  void setY(const float newY);

  void setZ(const float newZ);

  Vector3& Set(float r, float s, float t);

  Vector3& operator+=(const Vector3& vector);

  Vector3& operator-=(const Vector3& vector);

  Vector3& operator*=(float t);

  Vector3& operator/=(float t);

  Vector3& operator%=(const Vector3& other);

  Vector3& operator&=(const Vector3& vector);

  Vector3 operator-() const;

  Vector3 operator+(const Vector3& vector) const;

  Vector3 operator-(const Vector3& vector) const;

  Vector3 operator*(float t) const;

  Vector3 operator/(float t) const;

  //Dot product of a 2 vectors
  float operator*(const Vector3& vector) const;

  //Cross product of 2 vectors
  Vector3 operator%(const Vector3& vector) const;

  Vector3 operator&(const Vector3& vector) const;

  bool operator==(const Vector3& vector) const;

  bool operator!=(const Vector3& vector) const;

  const float GetLength();
  Vector3& Normalize();

  Vector3& RotateAboutX(float angle);
  Vector3& RotateAboutY(float angle);
  Vector3& RotateAboutZ(float angle);
  Vector3& RotateAboutAxis(float angle, const Vector3& axis);
};

#endif //_VECTOR3_H_
