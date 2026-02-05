#include <compare>
#include <ostream>
#include <utils/BigInt.h>
#include <utils/MoreMath.h>

template <typename Int>
concept FractionCompatible = std::copyable<Int> && requires(Int a, Int b) {
  { a + b } -> std::convertible_to<Int>;
  { a * b } -> std::convertible_to<Int>;
  { a - b } -> std::convertible_to<Int>;
  { a / b } -> std::convertible_to<Int>;
  { a % b } -> std::convertible_to<Int>;
  { a <=> b } -> std::same_as<std::strong_ordering>;
  { std::declval<std::ostream&>() << a } -> std::same_as<std::ostream&>;
};

template <FractionCompatible Int> class Fraction
{
public:
  Fraction(Int top = 0, Int bot = 1) : mTop(top), mBot(bot) {}

  Fraction& operator*=(const Fraction& o)
  {
    mTop *= o.mTop;
    mBot *= o.mBot;
    return *this;
  }

  Fraction& operator/=(const Fraction& o)
  {
    mTop *= o.mBot;
    mBot *= o.mTop;
    return *this;
  }

  Fraction& operator+=(const Fraction& o)
  {
    mTop = mTop * o.mBot + o.mTop * mBot;
    mBot *= o.mBot;
    return *this;
  }

  Fraction& operator-=(const Fraction& o)
  {
    mTop = mTop * o.mBot - o.mTop * mBot;
    mBot *= o.mBot;
    return *this;
  }

  friend Fraction operator*(const Fraction& a, const Fraction& b)
  {
    Fraction o = a;
    o *= b;
    return o;
  }
  friend Fraction operator/(const Fraction& a, const Fraction& b)
  {
    Fraction o = a;
    o /= b;
    return o;
  }
  friend Fraction operator+(const Fraction& a, const Fraction& b)
  {
    Fraction o = a;
    o += b;
    return o;
  }
  friend Fraction operator-(const Fraction& a, const Fraction& b)
  {
    Fraction o = a;
    o -= b;
    return o;
  }

  friend std::ostream& operator<<(std::ostream& os, const Fraction& b)
  {
    return os << '(' << b.mTop << '/' << b.mBot << ')';
  }

  std::strong_ordering operator<=>(const Fraction& o) const
  {
    Int lhs = mTop * o.mBot;
    Int rhs = o.mTop * mBot;
    return lhs <=> rhs;
  }

  bool operator==(const Fraction& o) const = default;

  double estimate() const { return (double)mTop / mBot; }

  // returns BigInt with #digits after floating point (returned digits can > digits)
  template <slow_bigint::isBigInt BigIntT> BigIntT expansion(size_t digits) const
  {
    Int top = mTop;
    top *= math::pow(slow_bigint::DenseBigInt(BigIntT::Base), digits);
    return top /= slow_bigint::DenseBigInt(mBot);
  }

  Int numerator() const { return mTop; }
  Int denominator() const { return mBot; }

  template <typename Func> void normalize(Func gcd)
  {
    Int g = gcd(mTop, mBot);
    mTop /= g;
    mBot /= g;
  }

private:
  Int mTop;
  Int mBot;
};
