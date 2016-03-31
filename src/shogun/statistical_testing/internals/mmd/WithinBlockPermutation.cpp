/*
 * Restructuring Shogun's statistical hypothesis testing framework.
 * Copyright (C) 2014  Soumyajit De
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http:/www.gnu.org/licenses/>.
 */

#include <shogun/lib/SGMatrix.h>
#include <shogun/lib/SGVector.h>
#include <shogun/lib/GPUMatrix.h>
#include <shogun/mathematics/eigen3.h>
#include <shogun/mathematics/Math.h>
#include <shogun/statistical_testing/internals/mmd/WithinBlockPermutation.h>
#include <shogun/statistical_testing/internals/mmd/BiasedFull.h>
#include <shogun/statistical_testing/internals/mmd/UnbiasedFull.h>
#include <shogun/statistical_testing/internals/mmd/UnbiasedIncomplete.h>

using namespace shogun;
using namespace internal;
using namespace mmd;

template <class T>
WithinBlockPermutation<T>::WithinBlockPermutation(index_t n) : n_x(n)
{
}

template <class T>
typename T::return_type WithinBlockPermutation<T>::operator()(SGMatrix<float64_t> km)
{
	// http:/stackoverflow.com/questions/15858569/randomly-permute-rows-columns-of-a-matrix-with-eigen

	Eigen::Map<Eigen::MatrixXd> map(km.matrix, km.num_rows, km.num_cols);

	Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> perm(km.num_rows);
	perm.setIdentity();
	SGVector<int> inds(perm.indices().data(), perm.indices().size(), false);
	CMath::permute(inds);

	Eigen::MatrixXd permuted = perm.transpose() * map * perm;
	SGMatrix<float64_t> permuted_km(permuted.data(), permuted.rows(), permuted.cols(), false);

	T statistic(n_x);
	return statistic(permuted_km);
}

template class WithinBlockPermutation<BiasedFull>;
template class WithinBlockPermutation<UnbiasedFull>;
template class WithinBlockPermutation<UnbiasedIncomplete>;
