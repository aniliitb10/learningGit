#pragma once

#include "Helper.h"

namespace utils
{
  static bool isIntegerType(const std::string& str_)
  {
    return std::regex_match(str_, INT_TYPE);
  }

  static bool isUnsignedIntegerType(const std::string& str_)
  {
    return std::regex_match(str_, UNSIGNED_INT_TYPE);
  }

  static bool isDoubleType(const std::string& str_)
  {
    return std::regex_match(str_, DOUBLE_TYPE);
  }

  static bool isUnsignedDoubleType(const std::string& str_)
  {
    return std::regex_match(str_, UNSIGNED_DOUBLE_TYPE);
  }

  static doubleVec cross(const doubleVec& v1_, const doubleVec& v2_)
  {
    doubleVec ret = { v1_[1] * v2_[2] - v1_[2] * v2_[1], v1_[2] * v2_[0] - v1_[0] * v2_[2], v1_[0] * v2_[1] - v1_[1] * v2_[0] };
    return ret;
  }

  //dot product of any 2d, 3d vector or -------"quaternion" ----- Of course of the same dimension
  static double dot(const doubleVec& v1_, const doubleVec& v2_)
  {
    int size_ = v1_.size();
    double ret = 0;
    for (int i = 0; i < size_; ++i)
    {
      ret += v1_[i] * v2_[i];
    }
    //double ret = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
    return ret;
  }

  static void quatMult(double t1_, const doubleVec& v1_, double t2_, const doubleVec& v2_, double &t3_, doubleVec& v3_)
  {
    t3_ = t1_*t2_ - dot(v1_, v2_);
    v3_ = cross(v1_, v2_);

    for (int i = 0; i < 3; ++i)
    {
      v3_[i] += t1_*v2_[i] + t2_*v1_[i];
    }
  }

  static double vectorMod(const doubleVec& v_)
  {
    unsigned int size_ = v_.size();
    double ret = 0;

    for (unsigned int i = 0; i < size_; ++i)
    {
      ret += pow(v_[i], 2);
    }

    ret = sqrt(ret);
    return ret;
  }

  static void makeItUnitQuatOrVector(doubleVec& col_)
  {
    double temp_mod = vectorMod(col_);
    if (temp_mod == ZERO_DOUBLE)
    {
      for (unsigned int i = 0; i < col_.size(); ++i)
        col_[i] = 0;
    }
    else
    {
      for (unsigned int i = 0; i < col_.size(); ++i)
        col_[i] /= temp_mod;
    }
  }

  //this one is direct quat and quat multiplication giving another quat, both q1 and q2 have 4 elements
  static void quatMultQuat(const doubleVec& q1_, const  doubleVec& q2_, doubleVec& q_)
  {
    double t1, t2, t3;
    doubleVec v1(3), v2(3), v3(3);
    t1 = q1_[3]; t2 = q2_[3]; //And it should not be t1 = v1[0]; t1 = v2[0]; angular/real part is the last one

    for (int i = 0; i < 3; ++i)
    {
      v1[i] = q1_[i];
      v2[i] = q2_[i];
    }

    quatMult(t1, v1, t2, v2, t3, v3);
    for (int i = 0; i < 3; ++i)
    {
      q_[i] = v3[i];
    }
    q_[3] = t3; //angular/real part is the last one
    makeItUnitQuatOrVector(q_);
  }

  static void quatInverse(doubleVec& quat_)
  {
    quat_[0] *= -1;
    quat_[1] *= -1;
    quat_[2] *= -1;
    //quat[3] *= -1; 4th one is the angular part, so not this one!
  }

  static void eulerToQuat(const doubleVec& euler_, doubleVec& q_)
  {
    //from http://mathworld.wolfram.com/EulerParameters.html follows x -convention as by Goldstein (1980, p. 176)
    q_[0] = cos(0.5 * (euler_[0] - euler_[2]))*sin(0.5 * euler_[1]);
    q_[1] = sin(0.5 * (euler_[0] - euler_[2]))*sin(0.5 * euler_[1]);
    q_[2] = sin(0.5 * (euler_[0] + euler_[2]))*cos(0.5 * euler_[1]);
    q_[3] = cos(0.5 * (euler_[0] + euler_[2]))*cos(0.5 * euler_[1]); //actually q_[3] represents the real/angular part

    makeItUnitQuatOrVector(q_);
    //finding the minima of real/angular part makes no sense here, so not going for the symmetry opertaions
  }

  static double misorientation(const doubleVec& euler1, const doubleVec& euler2, double ci1, double ci2, double min_ci = 0.1)
  {
    if (ci1 < min_ci || ci2 < min_ci) //incorporating low ci pixels!
      return 99.99;

    if (euler1 == euler2)
      return 0.0;

    static std::vector <double> quat1(4), quat2(4), quatF(4);
    eulerToQuat(euler1, quat1);
    eulerToQuat(euler2, quat2);
    quatInverse(quat1); //got it inverted
    quatMultQuat(quat1, quat2, quatF); //effectively inverse(q1)*q2

    double e1 = quatF[0], e2 = quatF[1], e3 = quatF[2], e4 = quatF[3];
    //e4 is the real/angular part

    //just storing the 4th quaterenian, instead of storing all 4; 
    const doubleVec eq{
      -1 * e1,
      -1 * e2,
      -1 * e3,
      (e4 - e1 - e2 - e3) / 2,
      (e4 + e2 + e1 + e3) / 2,
      (e4 - e2 + e1 - e3) / 2,
      (e4 + e2 - e1 + e3) / 2,
      (e4 + e2 - e1 - e3) / 2,
      (e4 - e2 + e1 + e3) / 2,
      (e4 - e2 - e1 + e3) / 2,
      (e4 + e2 + e1 - e3) / 2,
      (-e2 - e3) / sqrt(2),
      (-e1 - e3) / sqrt(2),
      (-e2 - e1) / sqrt(2),
      (-e2 + e3) / sqrt(2),
      (-e1 + e3) / sqrt(2),
      (-e1 + e2) / sqrt(2),
      (e4 - e1) / sqrt(2),
      (e4 - e2) / sqrt(2),
      (e4 - e3) / sqrt(2),
      (e4 + e1) / sqrt(2),
      (e2 + e4) / sqrt(2),
      (e4 + e3) / sqrt(2),
    };

    double largest = abs(e4);

    for (size_t i = 0; i < eq.size(); i++)
    {
      if (largest < abs(eq[i]))
      {
        largest = abs(eq[i]);
      }
    }

    if (largest > 1) largest = 1;
    double ret = 2 * acos(largest) * 180 / PI;

    if (ret > 90)
      return (180 - ret);
    else
      return ret;
  }
}
