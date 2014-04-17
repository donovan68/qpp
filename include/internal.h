/*
 * internal.h
 *
 *  Created on: Mar 24, 2014
 *      Author: vlad
 */

#ifndef INTERNAL_H_
#define INTERNAL_H_

#include <iostream>
#include <utility>
#include <vector>

#include "types.h"
#include "classes/exception.h"

// internal functions, do not modify

namespace qpp
{
namespace internal
{

// integer index to multi-index, use C-style array for speed
// standard lexicographical order, e.g. 00, 01, 10, 11
void _n2multiidx(size_t n, size_t numdims, const size_t* dims, size_t* result)
{
// no error checks to improve speed
	/*	size_t maxn = 1;
	 for (size_t i = 0; i < numdims; i++)
	 maxn *= dims[i];
	 if (n >= maxn)
	 throw Exception("_n2multiidx", Exception::Type::OUT_OF_RANGE);
	 */
	size_t _n = n;
	for (size_t i = 0; i < numdims; i++)
	{
		result[numdims - i - 1] = _n % static_cast<int>(dims[numdims - i - 1]);
		_n = _n / static_cast<int>(dims[numdims - i - 1]);
	}
}

// multi-index to integer index, use C-style array for speed,
// standard lexicographical order, e.g. 00->0, 01->1, 10->2, 11->3
size_t _multiidx2n(const size_t* midx, size_t numdims, const size_t* dims)
{
// no error checks to improve speed
	/*
	 for (size_t i = 0; i < numdims; i++)
	 if (midx[i] >= dims[i])
	 throw Exception("_multiidx2n", Exception::Type::OUT_OF_RANGE);
	 */

// Static allocation for speed!
	// double the size for matrices reshaped as vectors
	size_t part_prod[2 * ct::maxn];

	part_prod[numdims - 1] = 1;
	for (size_t j = 1; j < numdims; j++)
		part_prod[numdims - j - 1] = part_prod[numdims - j] * dims[numdims - j];

	size_t result = 0;
	for (size_t i = 0; i < numdims; i++)
		result += midx[i] * part_prod[i];

	return result;
}

// check square matrix
template<typename Derived>
bool _check_square_mat(const Eigen::MatrixBase<Derived>& A)
{
	const types::DynMat<typename Derived::Scalar> & rA = A;

	if (rA.rows() != rA.cols())
		return false;
	return true;
}

// check whether input is a vector or not
template<typename Derived>
bool _check_vector(const Eigen::MatrixBase<Derived>& A)
{
	const types::DynMat<typename Derived::Scalar> & rA = A;

	if (rA.rows() != 1 && rA.cols() != 1)
		return false;
	return true;
}

// check whether input is a row vector or not
template<typename Derived>
bool _check_row_vector(const Eigen::MatrixBase<Derived>& A)
{
	const types::DynMat<typename Derived::Scalar> & rA = A;

	if (rA.rows() != 1)
		return false;
	return true;
}

// check whether input is a column vector or not
template<typename Derived>
bool _check_col_vector(const Eigen::MatrixBase<Derived>& A)
{
	const types::DynMat<typename Derived::Scalar> & rA = A;

	if (rA.cols() != 1)
		return false;
	return true;
}

// check non-zero size of object that supports size() function
template<typename T>
bool _check_nonzero_size(const T& x)
{
	if (x.size() == 0)
		return false;
	return true;
}

// check that dims is a valid dimension vector
bool _check_dims(const std::vector<size_t>& dims)
{
	if (dims.size() == 0)
		return false;

	if (std::find_if(std::begin(dims), std::end(dims), [&dims](size_t i) -> bool
	{	if(i==0) return true;
		else return false;}) != std::end(dims))
		return false;
	return true;
}

// check that valid dims match the dimensions
// of valid (non-zero sized) quare matrix
template<typename Derived>
bool _check_dims_match_mat(const std::vector<size_t>& dims,
		const Eigen::MatrixBase<Derived>& A)
{
	const types::DynMat<typename Derived::Scalar> & rA = A;

	size_t proddim = 1;
	for (size_t i : dims)
		proddim *= i;
	if (proddim != static_cast<size_t>(rA.rows()))
		return false;
	return true;
}

// check that valid dims match the dimensions of valid row vector
template<typename Derived>
bool _check_dims_match_cvect(const std::vector<size_t>& dims,
		const Eigen::MatrixBase<Derived>& V)
{
	const types::DynMat<typename Derived::Scalar> & rV = V;

	size_t proddim = 1;
	for (size_t i : dims)
		proddim *= i;
	if (proddim != static_cast<size_t>(rV.rows()))
		return false;
	return true;
}

// check that valid dims match the dimensions of valid row vector
template<typename Derived>
bool _check_dims_match_rvect(const std::vector<size_t>& dims,
		const Eigen::MatrixBase<Derived>& V)
{
	const types::DynMat<typename Derived::Scalar> & rV = V;

	size_t proddim = 1;
	for (size_t i : dims)
		proddim *= i;
	if (proddim != static_cast<size_t>(rV.cols()))
		return false;
	return true;
}

// check that all elements in valid dims equal to dim
bool _check_eq_dims(const std::vector<size_t> &dims, size_t dim)
{
	for (size_t i : dims)
		if (i != dim)
			return false;
	return true;
}

// check that subsys is valid with respect to valid dims
bool _check_subsys_match_dims(const std::vector<size_t>& subsys,
		const std::vector<size_t>& dims)
{
//	// check non-zero sized subsystems
//	if (subsys.size() == 0)
//		return false;

// check valid number of subsystems
	if (subsys.size() > dims.size())
		return false;

	// sort the subsystems
	std::vector<size_t> subsyssort = subsys;
	std::sort(std::begin(subsyssort), std::end(subsyssort));

	// check duplicates
	if (std::unique(std::begin(subsyssort), std::end(subsyssort))
			!= std::end(subsyssort))
		return false;

	// check range of subsystems
	if (std::find_if(std::begin(subsyssort), std::end(subsyssort),
			[&dims](size_t i) -> bool
			{	if(i>dims.size()-1) return true;
				else return false;}) != std::end(subsyssort))
		return false;

	return true;
}

// check that the permutation is valid with respect to valid dims
bool _check_perm_match_dims(const std::vector<size_t>& perm,
		const std::vector<size_t>& dims)
{
	if (perm.size() != dims.size())
		return false;

	std::vector<size_t> sort_perm = perm;
	std::sort(std::begin(sort_perm), std::end(sort_perm));
	for (size_t i = 0; i < dims.size(); i++)
		if (sort_perm[i] != i)
			return false;
	return true;
}

} /* namespace internal */
} /* namespace qpp */

#endif /* INTERNAL_H_ */
