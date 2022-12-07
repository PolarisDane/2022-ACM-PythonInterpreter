#include "int2048.h"
using sjtu::int2048;
using std::string;

namespace sjtu {
  std::istream& operator>>(std::istream& is, int2048& b) {
    is >> buffer;
    b = int2048(buffer);
    return is;
  }
  std::ostream& operator<<(std::ostream& os, const int2048& b) {
    if (b.Array.size() == 1 && b.Array[0] == 0) {
      os << '0'; return os;
    }
    if (b.op_minus == -1) os << '-';
    for (int i = b.Array.size() - 1; i >= 0; i--) {
      if (i != b.Array.size() - 1) {
        for (int j = base_ / 10; j >= 10; j /= 10) {
          if (b.Array[i] >= j) break;
          os << '0';
        }//maintaining zeros
      }
      os << b.Array[i];
    }
    return os;
  }
}//overload streams

namespace sjtu {
  int2048::int2048() :op_minus(1) {
    Array.clear();
    Array.push_back(0);
  }
  int2048::int2048(long long x) {
    Array.clear();
    op_minus = 1;
    if (x == 0) {
      Array.push_back(0); return;
    }
    if (x < 0) {
      op_minus = -1; x = -x;
    }
    while (x) {
      Array.push_back(x % base_);
      x /= base_;
    }
  }
  int2048::int2048(const std::string& s) {
    Array.clear();
    op_minus = 1;
    if (s == "0") {
      Array.push_back(0); return;
    }
    if (s[0] == '-') {
      op_minus = -1;
      for (int i = s.length() - 1; i >= 1; i -= base) {
        long long tmpVal = 0;
        for (int j = std::max(i - base + 1, 1); j <= i; j++) {
          tmpVal = tmpVal * 10ll + (s[j] - '0');
        }
        Array.push_back(tmpVal);
      }
      return;
    }
    for (int i = s.length() - 1; i >= 0; i -= base) {
      long long tmpVal = 0;
      for (int j = std::max(i - base + 1, 0); j <= i; j++) {
        tmpVal = tmpVal * 10ll + (s[j] - '0');
      }
      Array.push_back(tmpVal);
    }
  }
  int2048::int2048(const int2048& a) {
    Array.clear();
    op_minus = a.op_minus;
    Array = a.Array;
  }
  int2048::int2048(int2048&& a) {
    Array.swap(a.Array);
    op_minus = a.op_minus;
  }
  int2048& int2048::operator=(const int2048& a) {
    Array.clear();
    op_minus = a.op_minus;
    Array = a.Array;
    return (*this);
  }//initialize
  int2048& int2048::operator=(int2048&& a) {
    op_minus = a.op_minus;
    Array.swap(a.Array);
    return (*this);
  }
  void int2048::read(const std::string& s) {
    (*this) = int2048(s);
  }
  void int2048::print()const {
    std::cout << (*this);
  }
}//basic function for input and output

namespace sjtu {
  long long int2048::operator()(int x)const {
    return x >= Array.size() ? 0 : Array[x];
  }
}//function for safe visit to vectors

namespace sjtu {
  bool operator==(const int2048& a, const int2048& b) {
    if (a.Array.size() == 1 && b.Array.size() == 1 && a.Array[0] == 0 && b.Array[0] == 0)
      return true;
    //possible p-zero and n-zero
    if (a.op_minus != b.op_minus) return false;
    if (a.Array.size() != b.Array.size()) return false;
    for (int i = 0; i < a.Array.size(); i++) {
      if (a.Array[i] != b.Array[i]) return false;
    }
    return true;
  }
  bool operator!=(const int2048& a, const int2048& b) {
    return !(a == b);
  }
  bool operator<(const int2048& a, const int2048& b) {
    if (a.op_minus != b.op_minus) return a.op_minus < b.op_minus;
    if (a.op_minus == -1) {
      if (a.Array.size() != b.Array.size()) return a.Array.size() > b.Array.size();
      for (int i = a.Array.size() - 1; i >= 0; i--) {
        if (a.Array[i] != b.Array[i]) return a.Array[i] > b.Array[i];
      }
    }
    else {
      if (a.Array.size() != b.Array.size()) return a.Array.size() < b.Array.size();
      for (int i = a.Array.size() - 1; i >= 0; i--) {
        if (a.Array[i] != b.Array[i]) return a.Array[i] < b.Array[i];
      }
    }
    return false;
  }
  bool operator>(const int2048& a, const int2048& b) {
    return b < a;
  }
  bool operator<=(const int2048& a, const int2048& b) {
    return (a < b) || (a == b);
  }
  bool operator>=(const int2048& a, const int2048& b) {
    return (a > b) || (a == b);
  }
}//inequality and equality for int2048

namespace sjtu {
  complex operator +(const complex& p, const complex& q) {
    return complex(p.x + q.x, p.y + q.y);
  }
  complex operator -(const complex& p, const complex& q) {
    return complex(p.x - q.x, p.y - q.y);
  }
  complex operator *(const complex& p, const complex& q) {
    return complex(p.x * q.x - p.y * q.y, p.x * q.y + p.y * q.x);
  }
  void FFT(int type) {
    for (int i = 0; i < limit; i++) {
      if (i < rev[i]) std::swap(A[i], A[rev[i]]);
    }
    for (int mid = 1; mid < limit; mid <<= 1) {
      complex wn = complex(cos(Pi / mid), type * sin(Pi / mid));
      for (int j = 0, R = (mid << 1); j < limit; j += R) {
        complex w = complex(1.0, 0.0);
        for (int k = 0; k < mid; k++, w = w * wn) {
          complex x = A[j | k], y = w * A[j | mid | k];
          A[j | k] = x + y;
          A[j | mid | k] = x - y;
        }
      }
    }
  }
}//FFT calculation

namespace sjtu {
  int2048 get_abs(const int2048& a) {
    int2048 t = a;
    t.op_minus = 1;
    return t;
  }
  int2048 add(const int2048& a, const int2048& b) {
    if (a.op_minus == b.op_minus) {
      int2048 t;
      t.op_minus = a.op_minus;
      int len = std::max(a.Array.size(), b.Array.size());
      t.Array.resize(len + 1);
      for (int i = 0; i < len; i++) {
        t.Array[i] += a(i) + b(i);
        if (t.Array[i] >= base_) {
          t.Array[i + 1] += (t.Array[i] / base_);
          t.Array[i] %= base_;
        }
      }
      if (!t.Array[len]) t.Array.pop_back();
      return t;
    }
    else if (a.op_minus == -1) {
      return minus(b, get_abs(a));
    }
    else {
      return minus(a, get_abs(b));
    }
  }
  int2048& int2048::add(const int2048& b) {
    (*this) = sjtu::add((*this), b);
    return *this;
  }
  int2048 minus(const int2048& a, const int2048& b) {
    if (a.op_minus == b.op_minus) {
      int2048 t;
      t.op_minus = a.op_minus;
      if (get_abs(a) < get_abs(b)) {
        t = minus(b, a); t.op_minus *= -1;
        return t;
      }
      t.Array.resize(a.Array.size());
      for (int i = 0; i < a.Array.size(); i++) {
        t.Array[i] += a(i) - b(i);
        if (t.Array[i] < 0) {
          t.Array[i] += base_; t.Array[i + 1]--;
        }
      }
      while (!t.Array.back() && t.Array.size() != 1) {
        t.Array.pop_back();
      }
      return t;
    }
    else if (a.op_minus == -1) {
      int2048 t = add(get_abs(a), b);
      t.op_minus = -1;
      return t;
    }
    else {
      return std::move(add(a, get_abs(b)));
    }
  }
  int2048& int2048::minus(const int2048& b) {
    (*this) = sjtu::minus((*this), b);
    return *this;
  }
  //Integer1

  int2048 operator+(const int2048& a, const int2048& b) {
    return std::move(add(a, b));
  }
  int2048 operator-(const int2048& a, const int2048& b) {
    return std::move(minus(a, b));
  }
  int2048& int2048::operator+=(const int2048& a) {
    (*this).add(a);
    return (*this);
  }
  int2048& int2048::operator-=(const int2048& a) {
    (*this).minus(a);
    return (*this);
  }

  int2048 operator*(const int2048& a, const int2048& b) {
    int2048 t;
    t.op_minus = a.op_minus * b.op_minus;
    limit = 1, limit_ = 0;
    while (limit <= a.Array.size() + b.Array.size() - 2) limit <<= 1, limit_++;
    A.clear();
    A.resize(limit);
    for (int i = 0; i < a.Array.size(); i++) A[i].x = a.Array[i];
    for (int i = 0; i < b.Array.size(); i++) A[i].y = b.Array[i];
    t.Array.resize(limit + 1);
    rev.clear();
    rev.resize(limit);
    for (int i = 0; i < limit; i++) {
      rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (limit_ - 1));
    }
    FFT(1);
    for (int i = 0; i < limit; i++) A[i] = A[i] * A[i];
    FFT(-1);//optimizing FFT from three times to two times
    for (int i = 0; i < limit; i++) {
      t.Array[i] += (long long)(A[i].y / (limit * 2.0) + 0.5);
      if (t.Array[i] >= base_) {
        t.Array[i + 1] += t.Array[i] / base_;
        t.Array[i] %= base_;
      }
    }
    while (!t.Array.back() && t.Array.size() != 1) {
      t.Array.pop_back();
    }
    return std::move(t);
  }
  int2048& int2048::operator*=(const int2048& a) {
    (*this) = (*this) * a;
    return (*this);
  }
  int2048 div(int2048 a, const int2048& b) {
    int2048 ans, now = b;
    ans.op_minus = a.op_minus * b.op_minus;
    ans.Array.resize(a.Array.size());
    v.clear();
    while (now <= a) {
      v.push_back(now);
      now *= base_;
    }//prepare diffrent base times b for division
    while (a > b) {
      while (v.back() > a) v.pop_back();//won't be used anymore
      now = v.back();
      long long l = a.Array.back() / (now.Array.back() + 1), r = base_ - 1, val = 0;
      //give a approximate range of the quotient to accelerate division
      while (l <= r) {
        long long mid = (l + r) >> 1;
        if (now * mid <= a) {
          l = mid + 1; val = mid;
        }
        else r = mid - 1;
      }
      ans.Array[v.size() - 1] = val; a -= now * val;
    }
    if (a == b) ans += int2048(1);
    while (!ans.Array.back() && ans.Array.size() != 1) {
      ans.Array.pop_back();
    }
    return std::move(ans);
  }
  void shift(int2048& a, int shift) {
    if (shift >= 0) {
      int pos = a.Array.size();
      a.Array.resize(a.Array.size() + shift);
      for (int i = pos - 1; i >= 0; i--) {
        a.Array[i + shift] = a.Array[i];
      }
      for (int i = 0; i < shift; i++) {
        a.Array[i] = 0;
      }
    }
    else {
      shift *= -1;
      int pos = a.Array.size() - shift - 1;
      for (int i = 0; i <= pos; i++) {
        a.Array[i] = a.Array[i + shift];
      }
      a.Array.resize(pos + 1);
    }
    if (!a.Array.size()) a.Array.push_back(0);
  }
  int2048 get_inverse(const int2048& a) {
    if (a.Array.size() <= 8) {
      int2048 one;
      one.Array.resize(a.Array.size() * 2 + 1);
      one.Array[one.Array.size() - 1] = 1;
      return div(one, a);
    }//brute force division when number is small enough
    int2048 t, rem, tmpa, ans;
    int m = a.Array.size(), k = (m + 5) >> 1;
    tmpa.Array.resize(k);
    for (int i = tmpa.Array.size() - 1, j = a.Array.size() - 1; i >= 0; i--, j--) {
      tmpa.Array[i] = a.Array[j];
    }//copy the first k digit of a to do the calculations
    int2048 one;
    one.Array.resize(2 * m + 1);
    one.Array[2 * m] = 1;//the one for adjusting the error,it has to be biased itself
    int2048 inv = get_inverse(tmpa);
    t = a * inv * inv;//B*x^2
    shift(t, -2 * k);
    inv = inv + inv;//2x
    shift(inv, m - k);
    ans = inv - t;//2x-B*x^2
    rem = one - ans * a;
    if (rem < 0) ans -= int2048(1);
    //adjust the possible error in calculation which is limited to one
    return std::move(ans);
  }
  int2048 operator/(const int2048& a, const int2048& b) {
    int2048 ans, rem, inv, one = int2048(1);
    int2048 tmpa = get_abs(a), tmpb = get_abs(b);
    if (tmpa < tmpb) {
      if (a.op_minus != b.op_minus) return int2048(-1);
      else return ans;
    }
    if (a.Array.size() > (b.Array.size() << 1)) {
      shift(tmpa, a.Array.size() - (b.Array.size() << 1));
      shift(tmpb, a.Array.size() - (b.Array.size() << 1));
    }//to maintain the error below one we have to shift to make the digits of a smaller than two times that of b
    inv = get_inverse(tmpb);
    ans = tmpa * inv;
    shift(ans, -tmpb.Array.size() * 2);//removing the biase which makes decimals integer
    rem = tmpa - ans * tmpb;//adjusting the possible error
    if (rem >= tmpb) ans += one, rem -= tmpb;
    if (rem < 0) ans -= one, rem += tmpb;
    //注意调整的时候要对rem(余数)进行修改
    ans.op_minus = a.op_minus * b.op_minus;
    if (rem == 0) return ans;
    if (ans.op_minus == -1) return ans - one;
    return ans;//注意负数除法的处理
  }
  int2048& int2048::operator/=(const int2048& a) {
    (*this) = (*this) / a;
    return (*this);
  }
}//overload operators