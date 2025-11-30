#pragma once

#include <iostream>
#include <string>
#include <vector>

class BigInt
{
public:
  BigInt();
  BigInt(int);
  BigInt(const std::string&);

  friend std::ostream& operator<<(std::ostream&, const BigInt&);

  BigInt abs() const;
  BigInt operator-() const;
  const BigInt& operator+=(const BigInt&);
  const BigInt& operator-=(const BigInt&);
  const BigInt& operator*=(const BigInt&);
  const BigInt& operator%=(const BigInt&);

  BigInt operator+(const BigInt&);
  BigInt operator-(const BigInt&);
  BigInt operator*(const BigInt&);
  BigInt operator%(const BigInt&);

  std::strong_ordering operator<=>(const BigInt& other) const;
  bool operator==(const BigInt& other) const = default;

private:
  void remove_leading_zeros();
  void abs_add(const BigInt&);
  void abs_sub(const BigInt&);

public:
  bool mIsNegative{false};
  std::vector<char> mDigits; // least significant bit first
};
