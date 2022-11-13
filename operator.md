# Operator overloading

### Arithmetic operator

```
T operator+(const T& x) const {}  // T y = (T)a + (T)x
T operator+(const U& x) const {}  // T y = (T)a + (U)x

T operator+(const T& x) const {}  // T y = (T)a - (T)x
T operator*(const T& x) const {}  // T y = (T)a * (T)x
T operator/(const T& x) const {}  // T y = (T)a / (T)x
T operator%(const T& x) const {}  // T y = (T)a % (T)x
```



### Assignment operator

```
// (1) Copy Constructor.
T(const T& x) {}

// (2) '=' operator.
T& operator=(const T& x) {
  /* processing */
  return *this;
}

/////////////////////////////////////
T y = (T)x;     // Copy constructor is called. (1)
T y; y = (T)x;  // '='operator is called. (2)
```

```
T& operator+=(const T& x) { /*processing*/; return *this}  // y += (T)x;
T& operator-=(const T& x) { /*processing*/; return *this}  // y -= (T)x;
T& operator*=(const T& x) { /*processing*/; return *this}  // y *= (T)x;
T& operator/=(const T& x) { /*processing*/; return *this}  // y /= (T)x;
T& operator%=(const T& x) { /*processing*/; return *this}  // y %= (T)x;
```



### Comparison operator

```
bool operator==(const T& x) const {}  // if ((T)a == (T)x) {}
bool operator!=(const T& x) const {}  // if ((T)a != (T)x) {}
bool operator<(const T& x) const {}   // if ((T)a < (T)x) {}
bool operator>(const T& x) const {}   // if ((T)a > (T)x) {}
```



### Increment / Decrement

```
T operator++() { /* processing */; return *this; }     //++x;
T operator--() { /* processing */; return *this; }     //--x;
T operator++(int) { T t = *this; ++(*this); return t; }  //x++;
T operator--(int) { T t = *this; --(*this); return t; }  //x--;
```



### Accessor

```
const U& operator[](const int i) const {}  // x[i]
U& operator[](const int i) const {} 
```

Example:
If you implement matrix class and []operator, you can do like here

```c++
class Matrix {
  public:
    const vector<T> operator[](const int i) const { return m[i]; }
    vector<T>& operator[](const int i) const { return m[i]; }
  private:
    vector<vector<T>> m;
};
```



### Stream Insertion Operator

```
class A {
 public:
  friend std::istream& operator>>(istream& is, A& a);
  friend std::ostream& operator<<(ostream& os, const A& a);
 private:
  int m_value = 0;
};

std::istream& operator>>(istream& is, A& a) {
  is >> a.m_value;
  return is;
}

std::ostream& operator<<(ostream& os, const A& a) {
  os << a.m_value;
  return os;
}
```
