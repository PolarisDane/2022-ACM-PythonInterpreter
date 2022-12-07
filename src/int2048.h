#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER

#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

static const long long base_ = 1e3;
static const int base = 3;
static const long double Pi = acos(-1.0);

namespace sjtu {
  class int2048 {
  public:
    int op_minus;
    std::vector<long long> Array;
    int2048();
    int2048(long long x);
    int2048(const std::string& s);
    int2048(const int2048&);
    int2048(int2048&&);

    long long operator()(int x)const;

    void read(const std::string& s);
    void print()const;
    // ===================================
    // Integer1
    // ===================================
    inline friend int2048 get_abs(const int2048& a);

    int2048& add(const int2048& b);
    inline friend int2048 add(const int2048& a, const int2048& b);

    int2048& minus(const int2048& b);
    inline friend int2048 minus(const int2048& a, const int2048& b);
    // ===================================
    // Integer2
    // ===================================
    int2048& operator=(const int2048& a);
    int2048& operator=(int2048&& a);

    int2048& operator+=(const int2048& a);
    friend int2048 operator+(const int2048& a, const int2048& b);

    int2048& operator-=(const int2048& a);
    friend int2048 operator-(const int2048& a, const int2048& b);

    int2048& operator*=(const int2048& a);
    friend int2048 operator*(const int2048& a, const int2048& b);

    friend void shift(int2048& a, int shift);
    friend int2048 get_inverse(const int2048& a);
    int2048& operator/=(const int2048& a);
    friend int2048 div(int2048 a, const int2048& b);
    friend int2048 operator/(const int2048& a, const int2048& b);

    friend std::istream& operator>>(std::istream& a, int2048& b);
    friend std::ostream& operator<<(std::ostream& a, const int2048& b);

    friend bool operator==(const int2048& a, const int2048& b);
    friend bool operator!=(const int2048& a, const int2048& b);
    friend bool operator<(const int2048& a, const int2048& b);
    friend bool operator>(const int2048& a, const int2048& b);
    friend bool operator<=(const int2048& a, const int2048& b);
    friend bool operator>=(const int2048& a, const int2048& b);
  };
  static std::string buffer = "";
  class complex {
  public:
    long double x, y;
    complex(long double xx = 0, long double yy = 0) { x = xx, y = yy; }
    inline friend complex operator +(const complex& p, const complex& q);
    inline friend complex operator -(const complex& p, const complex& q);
    inline friend complex operator *(const complex& p, const complex& q);
    inline friend void FFT(int type);
  };
  static std::vector<complex> A;
  static std::vector<int> rev;
  static std::vector<int2048> v;
  static int limit, limit_;
} // namespace sjtu
#endif