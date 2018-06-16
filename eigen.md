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
<pre>
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
</pre>

other style declaration:
- Eigen::Vector2d (= VectorXd v(2))
- Eigen::Vector3d (= VectorXd v(3))
- Eigen::Matrix3d (= MatrixXd m(3,3))

## basic operation
vector operation
<pre>
 auto v = v1 + v2;
 auto v = v1 - v2;
 auto v = 5 * v1;
 auto v = v1.dot(v2);  // v1 * v2
</pre>

matrix operation
<pre>
 auto m = m1 + m2;
 auto m = m1 - m2;
 auto m = 5 * m1;
 auto m = m1 * m2;  // inner product
</pre>
