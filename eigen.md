## Install & Compile
Install:
<pre>
 $ wget http://bitbucket.org/eigen/eigen/get/3.3.1.tar.bz2
 $ tar jxvf 3.3.1.tar.bz2
 $ cd eigen-eigen-f562a193118d/
 $ mkdir build
 $ cd build/
 $ cmake ../ -DCMAKE_INSTALL_PREFIX=/usr
 $ sudo make install
</pre>
Note: As for latest version, check eigen.tuxfamily.org/index.php

Compile:
<pre>
 $ g++ xx.cpp -I/usr/include/eigen3
</pre>

## Basic
template:
```
#include <Eigen/Dense>
#include <iostream>

int main() {
  Eigen::VectorXd v(3);
  Eigen::MatrixXd m(2,3);  // 2 x 3 matrix

  v << 1, 2, 3;

  m << 1, 2,
       2, 3,
       3, 4;

  std::cout << v << std::endl;
  std::cout << m << std::endl;
  return 0;
}
```

declaration:
- Eigen::Matrix<type, row, col> m
- Eigen::VectorXi v(num), Eigen::VectorXf w(num), Eigen::VectorXd x(num) = (int vec, float vec, double vec)
- Eigen::MatrixXi a(row, col), Eigen::MatrixXf b(row, col), Eigen::MatrixXd c(row, col) (= int mat, float mat, double mat)
- Eigen::Vector2i (= Eigen::Matrix<int, 2, 1>)
- Eigen::Matrix3f (= Eigen::Matrix<float, 3, 3>)

init:
- Eigen::MatrixXd::Identity(row, col), Eigen::Matrix3d::Identity()
- Eigen::MatrixXd::Zero(row, col), Eigen::Matrix3d::Zero()
- Eigen::MatrixXd::Ones(row, col), Eigen::Matrix3d::Ones()
- Eigen::MatrixXd::Constant(row, col, value), Eigen::Matrix3d::Contant(value)
- Eigen::MatrixXd::Random(row, col), Eigen::Matrix3::Random()
- Eigen::Vector3d::UnitX(), UnitY(), UnitZ(), Eigen::VectorXd::Unit(4,2) (= [0 0 1 0])
- Eigen::VectorXd::Zero(num), Eigenn::VectorXd::Ones(num), Eigen::VectorXd::Constant(num, value), Eigen::VectorXd::Random(num)

matrix info:
- m.size() (= row x col)
- m.rows(), m.cols()
- m.norm()

## basic operation
vector operation
- add: v1 + v2
- sub: v1 - v2
- mul: 5 * v1
- inner product: v1.dot(v2) 

matrix operation
- add: m1 + m2
- sub: m1 - m2
- mul: 5 * m1
- inner product: m1 * m2

## other type matrix
- inverse: m.inverse()
- tranpose: m.transpose(), v.transpose()
- diagonal:
  - Eigen::DiagonalMatrix<float, 3> m = v.asDiagonal();
  - Eigen::DiagonalMatrix<float, 3> m(3, 8, 9);
  - m = Eigen::Matrix3f::Zero; m.diagonal() << 3, 8, 9;

## Space transformation / Affine transformation
- 2D translation / rotation

code:
```
Eigen::Translation2f trans(1,2);
Eigen::Rotation2Df rot(3.14/2);
Eigen::Affine2f affine = trans * rot;
std::cout << affine.matrix() << std::endl;
std::cout << affine.translation() << std::endl;
std::cout << affine.rotation() << std::endl;
```

output:
<pre>
0.000796274          -1           1
          1 0.000796274           2
          0           0           1
1
2
0.000796274          -1
          1 0.000796274
</pre>

- set affine parameter with matrix

code:
```
Eigen::Matrix3f m;
m << 1, 0, 5,
     0, 1, 9,
     0, 0, 1;
affine = m;
std::cout << affine.matrix() << std::endl;
std::cout << affine.translation() << std::endl;
std::cout << affine.rotation() << std::endl;
```

output:
<pre>
1 0 5
0 1 9
0 0 1
5
9
1 0
0 1
</pre>

- 3D translation / rotation

code:
```
#include <Eigen/Dense>
#include <Eigen/Geometry>
...

int main() {
  Eigen::Vector3f v;
  v  <<  1,  2,  3;

  Eigen::Translation3f trans(1,2,3);
  std::cout << trans * v  << std::endl << std::endl;

  Eigen::Matrix3f m;
  m = Eigen::AngleAxisf(M_PI/4, Eigen::Vector3f::UnitZ()); // Z-axis rotation (XY-Plane)
  std::cout << m * v << std::endl << std::endl;

  m = Eigen::AngleAxisf(M_PI/4, Eigen::Vector3f::UnitX()); // X-axis rotation (YZ-Plane)
  std::cout << m * v << std::endl << std::endl;

  m = Eigen::AngleAxisf(M_PI/4, Eigen::Vector3f::UnitY()); // Y-axis rotation (ZX-Plane)
  std::cout << m * v << std::endl << std::endl;
  
  return 0;
}
```

output:
<pre>
2
4
6

-0.707107
  2.12132
        3

        1
-0.707107
  3.53553

2.82843
      2
1.41421
</pre>

## Eigen Applications
### Group Theory C++ library (https://github.com/strasdat/Sophus)
- Install
<pre>
$ git clone https://github.com/strasdat/Sophus.git
$ cd Sophus
$ mkdir build; cd build
$ cmake ../
$ make; sudo make install
</pre>

- Example implementation
```
#include <iomanip>
#include <iostream>

#include <Eigen/Dense>
#include <sophus/se2.hpp>

int main() {
  Eigen::Matrix2d R1, R2;
  R1  << cos(M_PI/2), -sin(M_PI/2),
         sin(M_PI/2), cos(M_PI/2);
  R2 = R1.inverse();

  Eigen::Vector2d t1, t2;
  t1 << 2, 1;
  t2 << -1, 2;

  Sophus::SE2d m1(R1, t1), m2(R2, t2);
  std::cout << std::fixed << std::setprecision(2);
  std::cout << "rotation::" << std::endl;
  std::cout << m1.rotationMatrix() << std::endl << std::endl;
  std::cout << "translation:" << std::endl;
  std::cout << m1.translation() << std::endl << std::endl;
  std::cout << "SE2 matrix:" << std::endl;
  std::cout << m1.matrix() << std::endl << std::endl;

  Sophus::SE2d m3 = m1 * m2;
  std::cout << "m1 * m2:" << std::endl;
  std::cout << m3.matrix() << std::endl;

  return 0;
}
```

output:
<pre>
rotation::
 0.00 -1.00
 1.00  0.00

translation:
2.00
1.00

SE2 matrix:
 0.00 -1.00  2.00
 1.00  0.00  1.00
 0.00  0.00  1.00

m1 * m2:
 1.00 -0.00  0.00
 0.00  1.00  0.00
 0.00  0.00  1.00
</pre>
