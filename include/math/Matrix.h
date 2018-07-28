 /*************************************************************************
 * 
 *  [2017] - [2018] Automy Inc. 
 *  All Rights Reserved.
 * 
 * NOTICE:  All information contained herein is, and remains
 * the property of Automy Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Automy Incorporated
 * and its suppliers and may be covered by U.S. and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Automy Incorporated.
 */

#ifndef MATH_INCLUDE_MATRIX_H_
#define MATH_INCLUDE_MATRIX_H_

#include <math/package.hxx>

#include <vnx/Input.h>
#include <vnx/Output.h>
#include <vnx/Visitor.h>

#include <cmath>
#include <cstdint>
#include <ostream>
#include <array>
#include <stdexcept>
#include <initializer_list>


namespace math {

struct NO_INIT {};

template<typename T, size_t Rows, size_t Cols>
class Matrix;

template<typename T, size_t Rows, size_t Cols>
std::ostream& operator<<(std::ostream& out, const Matrix<T, Rows, Cols>& value);

template<typename T, size_t Rows, size_t Cols>
std::istream& operator>>(std::istream& in, Matrix<T, Rows, Cols>& value);

template<typename T, size_t Rows, size_t Cols>
Matrix<T, Rows, Cols> inverse(const Matrix<T, Rows, Cols>& mat);

template<typename T>
Matrix<T, 1, 1> inverse(const Matrix<T, 1, 1>& m);

template<typename T>
Matrix<T, 2, 2> inverse(const Matrix<T, 2, 2>& m);

template<typename T>
Matrix<T, 3, 3> inverse(const Matrix<T, 3, 3>& m);

template<typename T>
Matrix<T, 4, 4> inverse(const Matrix<T, 4, 4>& m);


template<typename T, size_t Rows, size_t Cols>
class Matrix {
public:
	typedef T Scalar;
	
	/*
	 * Default constructor initializes with all zeros.
	 */
	Matrix() : data({}) {}
	
	/*
	 * Special constructor, does not initialize data.
	 */
	Matrix(NO_INIT) {}
	
	/*
	 * Generic copy constructor.
	 */
	template<typename S>
	Matrix(const Matrix<S, Rows, Cols>& mat) {
		for(size_t i = 0; i < size(); ++i) {
			data[i] = mat.data[i];
		}
	}
	
	/*
	 * Initialize with column major list (ie. human readable form).
	 */
	Matrix(const std::initializer_list<T>& list) {
		if(list.size() != Rows * Cols) {
			throw std::logic_error("list.size() != Rows * Cols");
		}
		size_t i = 0;
		for(const T& v : list) {
			(*this)(i / Cols, i % Cols) = v;
			i++;
		}
	}
	
	/*
	 * Returns identity matrix.
	 */
	static Matrix Identity() {
		if(Rows != Cols) {
			throw std::logic_error("Rows != Cols");
		}
		Matrix res;
		for(size_t i = 0; i < Rows; ++i) {
			res(i, i) = 1;
		}
		return res;
	}
	
	T* get_data() {
		return &data[0];
	}
	
	const T* get_data() const {
		return &data[0];
	}
	
	T& operator()(size_t i, size_t j) {
		return data[j * Rows + i];
	}
	
	const T& operator()(size_t i, size_t j) const {
		return data[j * Rows + i];
	}
	
	T& operator[](size_t i) {
		return data[i];
	}
	
	const T& operator[](size_t i) const {
		return data[i];
	}
	
	Matrix<T, Cols, Rows> transpose() const {
		Matrix<T, Cols, Rows> res;
		for(size_t j = 0; j < Cols; ++j) {
			for(size_t i = 0; i < Rows; ++i) {
				res(j, i) = (*this)(i, j);
			}
		}
		return res;
	}
	
	T squared_norm() const {
		T sum = 0;
		for(T v : data) {
			sum += v*v;
		}
		return sum;
	}
	
	T norm() const {
		return std::sqrt(squared_norm());
	}
	
	size_t size() const {
		return Rows * Cols;
	}
	
	Matrix inverse() const {
		if(Rows != Cols) {
			throw std::logic_error("Rows != Cols");
		}
		return math::inverse<T>(*this);
	}
	
	template<size_t N, size_t M = 1>
	Matrix<T, N, M> get(size_t i_0 = 0, size_t j_0 = 0) const {
		Matrix<T, N, M> res;
		for(size_t j = 0; j < M; ++j) {
			for(size_t i = 0; i < N; ++i) {
				res(i, j) = (*this)(i + i_0, j + j_0);
			}
		}
		return res;
	}
	
	Matrix<T, Rows+1, 1> extend() const {
		if(Cols != 1) {
			throw std::logic_error("Cols != 1");
		}
		Matrix<T, Rows+1, 1> res;
		for(size_t i = 0; i < Rows; ++i) {
			res[i] = data[i];
		}
		res[Rows] = 1;
		return res;
	}
	
	Matrix<T, Rows-1, 1> project() const {
		if(Cols != 1) {
			throw std::logic_error("Cols != 1");
		}
		return get<Rows-1>() * (T(1) / data[Rows-1]);
	}
	
	T dot(const Matrix& B) const {
		T res = 0;
		for(size_t i = 0; i < size(); ++i) {
			res += data[i] * B[i];
		}
		return res;
	}
	
	template<size_t N>
	Matrix<T, Rows, N> operator*(const Matrix<T, Cols, N>& B) const {
		Matrix<T, Rows, N> C;
		for(size_t i = 0; i < Rows; ++i) {
			for(size_t j = 0; j < N; ++j) {
				for(size_t k = 0; k < Cols; ++k) {
					C(i, j) += (*this)(i, k) * B(k, j);
				}
			}
		}
		return C;
	}
	
	template<size_t N>
	Matrix& operator*=(const Matrix<T, Cols, N>& B) {
		*this = *this * B;
		return *this;
	}
	
	Matrix& operator*=(const T& value) {
		for(size_t i = 0; i < Rows * Cols; ++i) {
			(*this)[i] *= value;
		}
		return *this;
	}
	
	Matrix operator*(const T& value) const {
		Matrix C = *this;
		C *= value;
		return C;
	}
	
	Matrix& operator/=(const T& value) {
		for(size_t i = 0; i < Rows * Cols; ++i) {
			(*this)[i] /= value;
		}
		return *this;
	}
	
	Matrix operator/(const T& value) const {
		Matrix C = *this;
		C /= value;
		return C;
	}
	
	Matrix& operator+=(const Matrix& B) {
		for(size_t i = 0; i < Rows * Cols; ++i) {
			(*this)[i] += B[i];
		}
		return *this;
	}
	
	Matrix operator+(const Matrix& B) const {
		Matrix C = *this;
		C += B;
		return C;
	}
	
	Matrix& operator-=(const Matrix& B) {
		for(size_t i = 0; i < Rows * Cols; ++i) {
			(*this)[i] -= B[i];
		}
		return *this;
	}
	
	Matrix operator-(const Matrix& B) const {
		Matrix C = *this;
		C -= B;
		return C;
	}
	
	bool operator==(const Matrix& B) const {
		return data == B.data;
	}
	
	bool operator!=(const Matrix& B) const {
		return data != B.data;
	}
	
	std::ostream& print(std::ostream& out, const std::string& name) {
		out << name << " = [" << std::endl;
		for(size_t i = 0; i < Rows; ++i) {
			if(i > 0) {
				out << "," << std::endl;
			}
			out << "[";
			for(size_t j = 0; j < Cols; ++j) {
				if(j > 0) {
					out << ", ";
				}
				out << (*this)(i, j);
			}
			out << "]";
		}
		out << "]" << std::endl;
		return out;
	}
	
	void read(vnx::TypeInput& in, const vnx::TypeCode* type_code, const uint16_t* code) {
		if(!code) {
			throw std::logic_error("read(math::Matrix<T>): code == 0");
		}
		if(code[0] == vnx::CODE_MATRIX) {		// new format since vnx-1.2.0
			vnx::read_matrix<T, 2>(in, data.data(), {Rows, Cols}, code);
		} else if(code[0] == vnx::CODE_ARRAY) {		// old format
			vnx::read(in, data, type_code, code);
		} else {
			data = {};
			vnx::skip(in, type_code, code);
		}
	}
	
	void write(vnx::TypeOutput& out, const vnx::TypeCode* type_code, const uint16_t* code) const {
		if(!type_code) {
			throw std::logic_error("write(math::Matrix<T>): type_code == 0");	// only allowed inside another class
		}
		vnx::write_matrix<T, 2>(out, data.data(), {Rows, Cols}, code);
	}
	
	void read(std::istream& in) {
		vnx::read_matrix<T, 2>(in, data.data(), {Rows, Cols});
	}
	
	void write(std::ostream& out) const {
		vnx::write_matrix<T, 2>(out, data.data(), {Rows, Cols});
	}
	
	void accept(vnx::Visitor& visitor) const {
		vnx::accept_matrix<T, 2>(visitor, data.data(), {Rows, Cols});
	}
	
	std::array<T, Rows * Cols> data;
	
};


template<typename T, size_t Rows, size_t Cols>
std::ostream& operator<<(std::ostream& out, const Matrix<T, Rows, Cols>& value) {
	vnx::write(out, value.data);
	return out;
}

template<typename T, size_t Rows, size_t Cols>
std::istream& operator>>(std::istream& in, Matrix<T, Rows, Cols>& value) {
	vnx::read(in, value.data);
	return in;
}

template<typename T, size_t Rows, size_t Cols>
Matrix<T, Rows, Cols> inverse(const Matrix<T, Rows, Cols>& mat) {
	throw std::logic_error("not implemented");
}

template<typename T>
Matrix<T, 1, 1> inverse(const Matrix<T, 1, 1>& m) {
	return m * (T(1) / m[0]);
}

template<typename T>
Matrix<T, 2, 2> inverse(const Matrix<T, 2, 2>& m) {
	const T det = (m(0,0) * m(1, 1)) - (m(0, 1) * m(1, 0));
	if(det == 0) {
		throw std::runtime_error("inverse(): determinant = 0");
	}
	Matrix<T, 2, 2> tmp;
	tmp(0, 0) = m(1, 1);
	tmp(1, 0) = -m(1, 0);
	tmp(0, 1) = -m(0, 1);
	tmp(1, 1) = m(0, 0);
	return tmp * (T(1) / det);
}

template<typename T>
Matrix<T, 3, 3> inverse(const Matrix<T, 3, 3>& m) {
	const T det = m(0, 0) * (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) -
				  m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
				  m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));
	if(det == 0) {
		throw std::runtime_error("inverse(): determinant = 0");
	}
	Matrix<T, 3, 3> tmp;
	tmp(0, 0) = m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2);
	tmp(0, 1) = m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2);
	tmp(0, 2) = m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1);
	tmp(1, 0) = m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2);
	tmp(1, 1) = m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0);
	tmp(1, 2) = m(1, 0) * m(0, 2) - m(0, 0) * m(1, 2);
	tmp(2, 0) = m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1);
	tmp(2, 1) = m(2, 0) * m(0, 1) - m(0, 0) * m(2, 1);
	tmp(2, 2) = m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1);
	return tmp * (T(1) / det);
}

template<typename T>
Matrix<T, 4, 4> inverse(const Matrix<T, 4, 4>& m) {
	Matrix<T, 4, 4> tmp;
	tmp[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15]
			+ m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
	
	tmp[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15]
			- m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
	
	tmp[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15]
			+ m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
	
	tmp[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14]
			- m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
	
	tmp[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15]
			- m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
	
	tmp[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15]
			+ m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
	
	tmp[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15]
			- m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
	
	tmp[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14]
			+ m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
	
	tmp[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15]
			+ m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
	
	tmp[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15]
			- m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
	
	tmp[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15]
			+ m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
	
	tmp[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14]
			- m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
	
	tmp[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11]
			- m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
	
	tmp[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11]
			+ m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
	
	tmp[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11]
			- m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
	
	tmp[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10]
			+ m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];
	
	const T det = m[0] * tmp[0] + m[1] * tmp[4] + m[2] * tmp[8] + m[3] * tmp[12];
	if(det == 0) {
		throw std::runtime_error("inverse(): determinant = 0");
	}
	return tmp * (T(1) / det);
}


} // math

#endif /* MATH_INCLUDE_MATRIX_H_ */