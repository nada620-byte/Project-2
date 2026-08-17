#include <iostream>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <vector>
using namespace std;

class BigInt;
BigInt operator+(BigInt lhs, const BigInt& rhs);
BigInt operator-(BigInt lhs, const BigInt& rhs);
BigInt operator*(BigInt lhs, const BigInt& rhs);
BigInt operator/(BigInt lhs, const BigInt& rhs);
BigInt operator%(BigInt lhs, const BigInt& rhs);

class BigInt {
    string number;    // Stores the number as a string
    bool isNegative;  // True if number is negative

    // Remove unnecessary leading zeros from the number string
    void removeLeadingZeros() {
        // Find the first non-zero digit
        size_t firstNonZero = 0;
        while (firstNonZero < number.length() && number[firstNonZero] == '0') {
            firstNonZero++;
        }

        // If the whole string was zeros (or empty), keep a single "0"
        if (firstNonZero == number.length()) {
            number = "0";
            isNegative = false;   // negative zero is not allowed
            return;
        }

        // Erase the leading zeros
        number.erase(0, firstNonZero);
    }


    // Compare absolute values of two BigInts (ignore signs)
    // Returns: 1 if |this| > |other|, 0 if equal, -1 if |this| < |other|
    int compareMagnitude(const BigInt& other) const {
        // Different lengths → longer one is bigger
        if (number.length() > other.number.length()) {
            return 1;
        }
        if (number.length() < other.number.length()) {
            return -1;
        }

        // Same length → lexicographical compare
        if (number > other.number) {
            return 1;
        }
        if (number < other.number) {
            return -1;
        }

        // Completely equal
        return 0;
    }


    static string divideStringByTwo(const string& num) {
        string result;
        int carry = 0;
        for (char c : num) {
            int digit = carry * 10 + (c - '0');
            result.push_back(char((digit / 2) + '0'));
            carry = digit % 2;
        }
        size_t nonZero = result.find_first_not_of('0');
        if (nonZero == string::npos) return "0";
        return result.substr(nonZero);
    }

public:
    // Default constructor - initialize to zero
    BigInt() {
        number = "0";
        isNegative = false;
    }
    // Constructor from 64-bit integer
    BigInt(int64_t value) {
        if (value < 0)
    {
        isNegative = true;

        string temp = to_string(value);

        number = temp.substr(1);
    }
    else
    {
        isNegative = false;

        number = to_string(value);
    }
}
    // Constructor from string representation
    BigInt(const string& str) {
        // Empty string safety (treat as zero)
        if (str.empty()) {
            number = "0";
            isNegative = false;
            return;
        }

        // Check for leading minus sign
        if (str[0] == '-') {
            isNegative = true;
            // Take everything after the '-'
            if (str.length() == 1) {
                // Just "-" → treat as zero
                number = "0";
                isNegative = false;
            } else {
                number = str.substr(1);
            }
        } else {
            isNegative = false;
            number = str;
        }

        // Normalize (remove leading zeros and fix -0)
        removeLeadingZeros();
    }

    // Copy constructor
    BigInt(const BigInt& other) {
        number = other.number;
        isNegative = other.isNegative;
    }

    // Destructor
    ~BigInt() {
        // No dynamic memory owned by this class - nothing to release
    }

    // Assignment operator
    BigInt& operator=(const BigInt& other) {
        // Self-assignment check
        if (this == &other) {
            return *this;
        }

        number = other.number;
        isNegative = other.isNegative;
        return *this;
    }

    // Unary negation operator (-x) philip
    BigInt operator-() const {
        BigInt result = *this;
        if (result.number != "0") {
            result.isNegative = !result.isNegative;
        } else {
            result.isNegative = false;
        }
        return result;
    }

    // Unary plus operator (+x) philip
    BigInt operator+() const {
        BigInt result = *this;
        return result;
    }

    // Addition assignment operator (x += y) philip
    BigInt& operator+=(const BigInt& other) {
        if (this->isNegative != other.isNegative) {
            BigInt temp = other;
            temp.isNegative = this->isNegative;
            *this -= temp;
            return *this;
        }

        string result = "";
        int i = this->number.length() - 1;
        int j = other.number.length() - 1;
        int carry = 0;

        while (i >= 0 || j >= 0 || carry > 0) {
            // Get digit 1 using normal if/else
            int digit1;
            if (i >= 0) {
                digit1 = this->number[i] - '0';
            } else {
                digit1 = 0;
            }

            // Get digit 2 using normal if/else
            int digit2;
            if (j >= 0) {
                digit2 = other.number[j] - '0';
            } else {
                digit2 = 0;
            }

            int sum = digit1 + digit2 + carry;
            carry = sum / 10;

            result.push_back((sum % 10) + '0');
            i--;
            j--;
        }

        int start = 0, end = result.length() - 1;
        while (start < end) {
            char temp_char = result[start];
            result[start] = result[end];
            result[end] = temp_char;
            start++;
            end--;
        }

        this->number = result;
        removeLeadingZeros();
        return *this;
    }

    // Subtraction assignment operator (x -= y) philip
    BigInt& operator-=(const BigInt& other) {
        if (this->isNegative != other.isNegative) {
            BigInt temp = other;
            temp.isNegative = this->isNegative;
            *this += temp;
            return *this;
        }

        int cmp = compareMagnitude(other);

        // Setup maxStr and minStr using standard if/else
        string maxStr;
        string minStr;
        if (cmp >= 0) {
            maxStr = this->number;
            minStr = other.number;
        } else {
            maxStr = other.number;
            minStr = this->number;
        }

        string result = "";
        int i = maxStr.length() - 1;
        int j = minStr.length() - 1;
        int borrow = 0;

        while (i >= 0) {
            // maxStr is guaranteed to have digits as long as i >= 0
            int digit1 = maxStr[i] - '0';

            // Get digit 2 using normal if/else
            int digit2;
            if (j >= 0) {
                digit2 = minStr[j] - '0';
            } else {
                digit2 = 0;
            }

            int diff = digit1 - digit2 - borrow;
            if (diff < 0) {
                diff += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }

            result.push_back(diff + '0');
            i--;
            j--;
        }

        int start = 0, end = result.length() - 1;
        while (start < end) {
            char temp_char = result[start];
            result[start] = result[end];
            result[end] = temp_char;
            start++;
            end--;
        }

        this->number = result;

        if (cmp < 0) {
            this->isNegative = !this->isNegative;
        }

        removeLeadingZeros();
        return *this;
    }

   // Multiplication assignment operator (x *= y) Sara
    BigInt& operator*=(const BigInt& other) {
        //If either operand is zero, the result is zero:
        if (number == "0" || other.number == "0") {
        number = "0";
        isNegative = false;
        return *this;
    }
    //XOR rule:
    bool resultNegative = (isNegative != other.isNegative);

    int len1 = number.length();
    int len2 = other.number.length();


    vector<int> result(len1 + len2, 0);

     for (int i = len1 - 1; i >= 0; i--) {
            for (int j = len2 - 1; j >= 0; j--) {

            int digit1 = number[i] - '0';
            int digit2 = other.number[j] - '0';
            result[i + j + 1] += digit1 * digit2;
        }

    }
    for (int i = len1 + len2 - 1; i > 0; i--) {
        result[i - 1] += result[i] / 10;
        result[i] %= 10;
    }

    string newNumber = "";

    int start = 0;

    while (start < len1 + len2 - 1 && result[start] == 0) {
        start++;
    }

    for (int i = start; i < len1 + len2; i++) {
        newNumber += char(result[i] + '0');
    }

    number = newNumber;
    isNegative = resultNegative;

    removeLeadingZeros();
   return *this;
    }

    // Division assignment operator (x /= y)
BigInt& operator/=(const BigInt& other) {
    // Throw exception if divisor is zero
    if (other.number == "0") {
        throw std::runtime_error("Division by zero");
    }

    // If dividend is zero, result is zero
    if (number == "0") {
        return *this;
    }

    // Store the sign of the result (XOR rule)
    bool resultNegative = (isNegative != other.isNegative);

    // Work with absolute values for division
    BigInt dividend = *this;
    dividend.isNegative = false;
    BigInt divisor = other;
    divisor.isNegative = false;

    // If dividend < divisor, result is zero
    if (dividend.compareMagnitude(divisor) == -1) {
        number = "0";
        isNegative = false;
        return *this;
    }

    // Binary search for quotient
    BigInt low(0);
    BigInt high = dividend; // Since dividend is positive now
    BigInt result(0);

    // Binary search: find largest q such that q * divisor <= dividend
    while (low.compareMagnitude(high) == -1 || low.compareMagnitude(high) == 0) {
        // Calculate mid = (low + high) / 2
        BigInt mid = low + high;

        mid.number = divideStringByTwo(mid.number);

        // Calculate mid * divisor
        BigInt product = mid * divisor;

        // If product <= dividend, search upper half
        if (product.compareMagnitude(dividend) == 0 || product.compareMagnitude(dividend) == -1) {
            result = mid;
            low = mid + BigInt(1);
        }
        // Else search lower half
        else {
            high = mid - BigInt(1);
        }

        // Safety check to avoid infinite loops
        if (low == high && low == mid) {
            break;
        }
    }

    // Apply the sign to the result
    number = result.number;
    isNegative = resultNegative;

    // Ensure zero is positive
    removeLeadingZeros();
    return *this;
}

// Modulus assignment operator (x %= y)
BigInt& operator%=(const BigInt& other) {
    // Throw exception if divisor is zero
    if (other.number == "0") {
        throw std::runtime_error("Division by zero");
    }

    // If dividend is zero, result is zero
    if (number == "0") {
        return *this;
    }

    // Store the sign of the dividend (this) for the result
    bool originalSign = isNegative;

    // Calculate quotient = this / other
    BigInt quotient = *this / other;

    // Calculate remainder = this - (quotient * other)
    BigInt remainder = *this - (quotient * other);

    // The sign of the remainder matches the dividend
    remainder.isNegative = originalSign;

    // Copy the result
    number = remainder.number;
    isNegative = remainder.isNegative;

    // Ensure zero is positive
    removeLeadingZeros();
    return *this;
}

   // Pre-increment operator (++x)
    BigInt& operator++() {
        *this += BigInt(1);
        return *this;
    }

    // Post-increment operator (x++)
    BigInt operator++(int) {
        BigInt temp = *this;
        ++(*this);
        return temp;
    }

    // Pre-decrement operator (--x)
    BigInt& operator--() {
        *this -= BigInt(1);
        return *this;
    }

    // Post-decrement operator (x--)
    BigInt operator--(int) {
        BigInt temp = *this;
        --(*this);
        return temp;
    }

    // Convert BigInt to string representation
    string toString() const {
        if (isNegative && number != "0") {
            return "-" + number;
        }

        return number;
    }

    // Output stream operator (for printing)
    friend ostream& operator<<(ostream& os, const BigInt& num) {
        os << num.toString();
        return os;
    }

    // Input stream operator (for reading from input)
    friend istream& operator>>(istream& is, BigInt& num) {
        string input;
        is >> input;

        BigInt temp(input);
        num = temp;

        return is;
    }

    // Friend declarations for comparison operators
    friend bool operator==(const BigInt& lhs, const BigInt& rhs);
    friend bool operator<(const BigInt& lhs, const BigInt& rhs);
};

// Binary addition operator (x + y) philip
BigInt operator+(BigInt lhs, const BigInt& rhs) {
    lhs += rhs;
    return lhs;
}

// Binary subtraction operator (x - y) philip
BigInt operator-(BigInt lhs, const BigInt& rhs) {
    lhs -= rhs;
    return lhs;
}

// Binary multiplication operator (x * y)
BigInt operator*(BigInt lhs, const BigInt& rhs) {
     lhs *= rhs;
    return lhs;
}

// Binary division operator (x / y)
BigInt operator/(BigInt lhs, const BigInt& rhs) {
    lhs /= rhs;
    return lhs;
}

// Binary modulus operator (x % y)
BigInt operator%(BigInt lhs, const BigInt& rhs) {
    lhs %= rhs;
    return lhs;
}

// Equality comparison operator (x == y) Sara
bool operator==(const BigInt& lhs, const BigInt& rhs) {
 if (lhs.isNegative != rhs.isNegative) {
        return false;
    }

    return lhs.number == rhs.number;
}

// Inequality comparison operator (x != y)
bool operator!=(const BigInt& lhs, const BigInt& rhs) {
 return !(lhs == rhs);
}

// Less-than comparison operator (x < y)
bool operator<(const BigInt& lhs, const BigInt& rhs) {
  if (lhs.isNegative && !rhs.isNegative) {
        return true;
    }
    if (!lhs.isNegative && rhs.isNegative) {
        return false;
    }
    if (!lhs.isNegative && !rhs.isNegative) {
        return lhs.compareMagnitude(rhs) == -1;
    }
    return lhs.compareMagnitude(rhs) == 1;
}


// Less-than-or-equal comparison operator (x <= y)
bool operator<=(const BigInt& lhs, const BigInt& rhs) {
  return (lhs < rhs) || (lhs == rhs);
}

// Greater-than comparison operator (x > y)
bool operator>(const BigInt& lhs, const BigInt& rhs) {
 return !(lhs <= rhs);
}

// Greater-than-or-equal comparison operator (x >= y)
bool operator>=(const BigInt& lhs, const BigInt& rhs) {
    return !(lhs < rhs);
}


int main() {
    cout << "=== BigInt Class Test Program ===" << endl << endl;

    //Test 1: Constructors and basic output
    cout << "1. Constructors and output:" << endl;
    BigInt a(12345);              // Should create BigInt from integer
    BigInt b("-67890");           // Should create BigInt from string
    BigInt c("0");                // Should handle zero correctly
    BigInt d = a;                 // Should use copy constructor
    cout << "a (from int): " << a << endl;        // Should print "12345"
    cout << "b (from string): " << b << endl;     // Should print "-67890"
    cout << "c (zero): " << c << endl;            // Should print "0"
    cout << "d (copy of a): " << d << endl << endl; // Should print "12345"

    // Test 2: Arithmetic operations
    cout << "2. Arithmetic operations:" << endl;
    cout << "a + b = " << a + b << endl;          // Should calculate 12345 + (-67890)
    cout << "a - b = " << a - b << endl;          // Should calculate 12345 - (-67890)
    cout << "a * b = " << a * b << endl;          // Should calculate 12345 * (-67890)
    cout << "b / a = " << b / a << endl;          // Should calculate (-67890) / 12345
    cout << "a % 100 = " << a % BigInt(100) << endl << endl; // Should calculate 12345 % 100

    // Test 3: Relational operators
    cout << "3. Relational operators:" << endl;
    cout << "a == d: " << (a == d) << endl;       // Should be true (12345 == 12345)
    cout << "a != b: " << (a != b) << endl;       // Should be true (12345 != -67890)
    cout << "a < b: " << (a < b) << endl;         // Should be false (12345 < -67890)
    cout << "a > b: " << (a > b) << endl;         // Should be true (12345 > -67890)
    cout << "c == 0: " << (c == BigInt(0)) << endl << endl; // Should be true (0 == 0)

    // Test 4: Unary operators and increments
    cout << "4. Unary operators and increments:" << endl;
    cout << "-a: " << -a << endl;                 // Should print "-12345"
    cout << "++a: " << ++a << endl;               // Should increment and print "12346"
    cout << "a--: " << a-- << endl;               // Should print "12346" then decrement
    cout << "a after decrement: " << a << endl << endl; // Should print "12345"

    // Test 5: Large number operations
    cout << "5. Large number operations:" << endl;
    BigInt num1("12345678901234567890");
    BigInt num2("98765432109876543210");
    cout << "Very large addition: " << num1 + num2 << endl;
    cout << "Very large multiplication: " << num1 * num2 << endl << endl;

    // Test 6: Edge cases and error handling
    cout << "6. Edge cases:" << endl;
    BigInt zero(0);
    BigInt one(1);
    try {
        BigInt result = one / zero;               // Should throw division by zero error
        cout << "Division by zero succeeded (unexpected)" << endl;
    } catch (const runtime_error& e) {
        cout << "Division by zero correctly threw error: " << e.what() << endl;
    }
    cout << "Multiplication by zero: " << one * zero << endl;        // Should be "0"
    cout << "Negative multiplication: " << BigInt(-5) * BigInt(3) << endl;  // Should be "-15"
    cout << "Negative division: " << BigInt(-10) / BigInt(3) << endl;       // Should be "-3"
    cout << "Negative modulus: " << BigInt(-10) % BigInt(3) << endl;        // Should be "-1"


    return 0;
}
