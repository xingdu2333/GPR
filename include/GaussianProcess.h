/*
 * Copyright 2015 Christoph Jud (christoph.jud@unibas.ch)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef GaussianProcess_h
#define GaussianProcess_h

#include <string>
#include <vector>
#include <memory>

#include <Eigen/Dense>

#include "Kernel.h"

namespace gpr{

template< class TScalarType >
class GaussianProcess
{
public:
	typedef GaussianProcess Self;
	typedef Kernel<TScalarType> KernelType;
	typedef std::shared_ptr<KernelType> KernelTypePointer;
	
    typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, 1> VectorType;
    typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixType;
    typedef Eigen::DiagonalMatrix<TScalarType, Eigen::Dynamic> DiagMatrixType;

	typedef std::vector<VectorType> VectorListType;
	typedef std::vector<MatrixType> MatrixListType;
	
	/*
	 * Add a new sample lable pair to the gaussian process
	 *  x is the input vector
	 *  y the corresponding label vector
	 */
    void AddSample(const VectorType &x, const VectorType &y);

	/*
	 * Predict new data point
	 */
    VectorType Predict(const VectorType &x);

	/*
	 * Predict new point (return value) and its derivative input parameter D
	 */
    VectorType PredictDerivative(const VectorType &x, MatrixType &D);


    /*
     * Returns the scalar product between x and y in the RKHS of this GP
     */
    TScalarType operator()(const VectorType & x, const VectorType & y);

    /*
     * Returns the positive credible interval at point x
     */
    TScalarType GetCredibleInterval(const VectorType&x);
	
	/*
	 * If sample data has changed perform learning step
	 */
    void Initialize();


    // Constructors
    GaussianProcess(KernelTypePointer kernel) : m_Sigma(0),
                                                m_Initialized(false),
                                                m_InputDimension(0),
                                                m_OutputDimension(0),
                                                debug(false) {
        m_Kernel = kernel;
	}

    virtual ~GaussianProcess() {}


    // Some get / set methods
	void DebugOn(){
		debug = true;
	}

    unsigned GetNumberOfSamples() const{
		return m_SampleVectors.size();
	}

    TScalarType GetSigma() const{
        return m_Sigma;
    }

    void SetSigma(TScalarType sigma){
        m_Sigma = sigma;
        m_Initialized = false;
    }

    unsigned GetNumberOfInputDimensions() const{ return m_InputDimension; }


    // IO methods
    void Save(std::string prefix);
    void Load(std::string prefix);

    void ToString() const;


    // Comparison operator
    bool operator ==(const GaussianProcess<TScalarType> &b) const;
    bool operator !=(const GaussianProcess<TScalarType> &b) const{
        return ! operator ==(b);
    }

private:

	/*
	 * Computation of kernel matrix K_ij = k(x_i, x_j)
	 * 	- it is symmetric therefore only half of the kernel evaluations
	 * 	  has to be performed
	 */
    void ComputeKernelMatrix(MatrixType &M) const;

	/*
	 * Bring the label vectors in a matrix form Y,
	 * where the rows are the labels.
	 */
    void ComputeLabelMatrix(MatrixType &Y) const;

	/*
	 * Lerning is performed.
	 */
    void ComputeRegressionVectors();

	/*
	 * Computation of the kernel vector V_i = k(x, x_i)
	 */
    void ComputeKernelVector(const VectorType &x, VectorType &Kx) const;

	/*
	 * Compute difference matrix X = [x-x_0, x-x_1, ... x-x_n]^T
	 */
    void ComputeDifferenceMatrix(const VectorType &x, MatrixType &X) const;

	/*
	 * Assertion functions to check input and output dimensions of the vectors
	 */
    void CheckInputDimension(const VectorType &x, std::string msg_prefix) const;
    void CheckOutputDimension(const VectorType &y, std::string msg_prefix) const;


    KernelTypePointer m_Kernel; // pointer to kernel
	
    TScalarType m_Sigma; // noise on sample data

	VectorListType m_SampleVectors;  // Dimensionality: TInputDimension
	VectorListType m_LabelVectors;   // Dimensionality: TOutputDimension
	MatrixType m_RegressionVectors; // for each output dimension there is one regression vector
    MatrixType m_CoreMatrix;
	
	bool m_Initialized;
	unsigned m_InputDimension;
	unsigned m_OutputDimension;

	bool debug;

	GaussianProcess(const Self &); //purposely not implemented
	void operator=(const Self &); //purposely not implemented
};

}

#endif
