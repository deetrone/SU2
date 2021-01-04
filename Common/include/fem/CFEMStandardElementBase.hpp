/*!
 * \file CFEMStandardElementBase.hpp
 * \brief Base class for the FEM standard elements.
 *        The functions are in the <i>CFEMStandardElementBase.cpp</i> file.
 * \author E. van der Weide
 * \version 7.0.8 "Blackbird"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2020, SU2 Contributors (cf. AUTHORS.md)
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <iostream>
#include <vector>
#include <cstdlib>

#include "../CConfig.hpp"
#include "../containers/C2DContainer.hpp"
#include "../parallelization/vectorization.hpp"

#if defined(HAVE_MKL)
#include "mkl.h"
#else
#define dgemm_jit_kernel_t int
#endif

#if !(defined(PRIMAL_SOLVER) && defined(HAVE_MKL))
#include "../linear_algebra/blas_structure.hpp"
#endif

using namespace std;

/*!
 * \class CFEMStandardElementBase
 * \brief Base class for a FEM standard element.
 * \author E. van der Weide
 * \version 7.0.8 "Blackbird"
 */
class CFEMStandardElementBase {
public:
  static const size_t baseVectorLen = simd::preferredLen<su2double>();   /*!< \brief Vector length must be a multiple of
                                                                                     basevectorLen for good performance. */
protected:
  unsigned short VTK_Type;         /*!< \brief Element type using the VTK convention. */
  unsigned short nPoly;            /*!< \brief Polynomial order of the element. */
  unsigned short orderExact;       /*!< \brief Polynomial order that must be integrated exactly by the integration rule. */
  unsigned short nDOFs;            /*!< \brief Total number of DOFs. */
  unsigned short nDOFsPad;         /*!< \brief Padded version of nDOFs. */
  unsigned short nIntegration;     /*!< \brief Total number of points used in the numerical integration. */
  unsigned short nIntegrationPad;  /*!< \brief Padded version of nIntegration. */

  su2passivevector wIntegration;    /*!< \brief The weights of the integration points for this standard element. */

  unsigned short VTK_SubType1;    /*!< \brief VTK type for elements of type 1 in subConn1ForPlotting. */
  unsigned short VTK_SubType2;    /*!< \brief VTK type for elements of type 2 in subConn2ForPlotting. */

  vector<unsigned short> subConn1ForPlotting; /*!< \brief Local subconnectivity of element type 1 of the high order element.
                                                          Used for plotting. */
  vector<unsigned short> subConn2ForPlotting; /*!< \brief Local subconnectivity of element type 2 of the high order element.
                                                          Used for plotting. */

  vector<vector<unsigned short> > gridConnFaces; /*!< \brief Local grid connectivities of the faces of the element.
                                                             The numbering of the DOFs is such that the element
                                                             is to the left of the face. */

#if !(defined(PRIMAL_SOLVER) && defined(HAVE_MKL))
  CBlasStructure blasFunctions; /*!< \brief  The object to carry out the BLAS functionalities. */
#endif

public:
  /*-----------------------------------------------------------------------------------*/
  /*---                     Constructors and destructors.                           ---*/
  /*-----------------------------------------------------------------------------------*/

  /*!
  * \brief Constructor.
  */
  CFEMStandardElementBase() = default;

  /*!
  * \brief Destructor.
  */
  virtual ~CFEMStandardElementBase() = default;

public:
  /*-----------------------------------------------------------------------------------*/
  /*---                     Public static member functions.                         ---*/
  /*-----------------------------------------------------------------------------------*/

  /*!
   * \brief Static function, which makes available the number of integration
   *        points for an element corresponding to the arguments.
   * \param[in] VTK_Type   - Type of the element using the VTK convention.
   * \param[in] orderExact - Polynomial degree that must be integrated exactly.
   * \return The number of integration points.
   */
  static unsigned short GetNIntStatic(unsigned short VTK_Type,
                                      unsigned short orderExact);

  /*!
   * \brief Static function, which makes available the number of DOFs for an element
   *        corresponding to the arguments.
   * \param[in] VTK_Type   - Type of the element using the VTK convention.
   * \param[in] nPoly      - Polynomial degree of the element.
   * \return The number of DOFs
   */
  static unsigned short GetNDOFsStatic(unsigned short VTK_Type,
                                       unsigned short nPoly);

  /*!
   * \brief Static function, which computes the padded value of the given input value.
   * \param[in] val - Value to be padded.
   * \return          The padded value of val.
   */
  static constexpr size_t PaddedValue(size_t val) {
    return ((val+baseVectorLen-1)/baseVectorLen)*baseVectorLen;
  }

  /*-----------------------------------------------------------------------------------*/
  /*--- Virtual functions that must be overwritten by the derived classs when used. ---*/
  /*-----------------------------------------------------------------------------------*/

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \param[in]  LGLDistribution - Whether or not the LGL node distribution must be used.
   * \param[in]  matCoorDOF - Matrix that contains the coordinates of the grid DOFs.
   * \param[out] matCoorInt - Matrix that contains the coordinates of the integration
   *                          points.
   */
  virtual void CoorIntPoints(const bool                LGLDistribution,
                             ColMajorMatrix<su2double> &matCoorDOF,
                             ColMajorMatrix<su2double> &matCoorInt) {

    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \param[in]  LGLDistribution - Whether or not the LGL node distribution must be used.
   * \param[in]  matCoor         - Matrix that contains the coordinates of the grid DOFs.
   * \param[out] matDerCoor      - Vector of matrices to store the derivatives of the coordinates.
   */
  virtual void DerivativesCoorIntPoints(const bool                         LGLDistribution,
                                        ColMajorMatrix<su2double>          &matCoor,
                                        vector<ColMajorMatrix<su2double> > &matDerCoor) {

    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \param[in]  matCoor       - Matrix that contains the coordinates of the grid DOFs.
   * \param[out] matDer2ndCoor - Vector of matrices to store the 2nd derivatives of the coordinates.
   */
  virtual void Derivatives2ndCoorIntPoints(ColMajorMatrix<su2double>          &matCoor,
                                           vector<ColMajorMatrix<su2double> > &matDer2ndCoor) {
    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \param[in]  matCoor    - Matrix that contains the coordinates of the grid DOFs.
   * \param[out] matDerCoor - Vector of matrices to store the derivatives of the coordinates.
   */
  virtual void DerivativesCoorSolDOFs(ColMajorMatrix<su2double>          &matCoor,
                                      vector<ColMajorMatrix<su2double> > &matDerCoor) {

    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \return The number of faces of the volume element.
   */
  virtual unsigned short GetNFaces(void) const {
    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
    return 0;
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \return The number of solution DOFs of the volume element.
   */
  virtual unsigned short GetNSolDOFs(void) const {
    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
    return 0;
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \return The padded number of solution DOFs of the volume element.
   */
  virtual unsigned short GetNSolDOFsPad(void) const {
    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
    return 0;
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \param[in] ind - Index of the face for which the VTK type must be returned.
   * \return The VTK type of the given face id of the element.
   */
  virtual unsigned short GetVTK_Face(unsigned short ind) const {
    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
    return 0;
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \return The number of different face types of the volume element.
   */
  virtual unsigned short GetnFaceTypes(void) const {
    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
    return 0;
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \param[in] ind - Index of the face type for which the VTK type must be returned.
   * \return The VTK type of the given face type.
   */
  virtual unsigned short GetVTK_TypeFace(unsigned short ind) const {
    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
    return 0;
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \param[in] config   - Object, which contains the input parameters.
   * \param[in] elemType - Type of the volume element adjacent to this boundary face.
   * \return The work estimate for a boundary face of this type.
   */
  virtual passivedouble WorkEstimateBoundaryFace(CConfig              *config,
                                                 const unsigned short elemType) {

    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
    return 0;
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \param[in] config    - Object, which contains the input parameters.
   * \param[in] elemType0 - Type of the volume element adjacent to side 0 of this face.
   * \param[in] nPoly0    - Polynomial degree used in elemType0.
   * \param[in] elemType1 - Type of the volume element adjacent to side 1 of this face.
   * \param[in] nPoly1    - Polynomial degree used in elemType1.
   * \return The work estimate for an internal face of this type.
   */
  virtual passivedouble WorkEstimateInternalFace(CConfig              *config,
                                                 const unsigned short elemType0,
                                                 const unsigned short nPoly0,
                                                 const unsigned short elemType1,
                                                 const unsigned short nPoly1) {

    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
    return 0;
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \param[in] config - Object, which contains the input parameters.
   * \return The work estimate for the volume for this type of element.
   */
  virtual passivedouble WorkEstimateVolume(CConfig *config) {

    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
    return 0;
  }

  /*!
   * \brief Virtual function, that, if used, must be overwritten by the derived class.
   * \param[in] config    - Object, which contains the input parameters.
   * \param[in] nPointsWF - Number of points in wall function treatment.
   * \param[in] elemType  - Type of the volume element adjacent to this boundary face.
   * \return The work estimate for a boundary face of this type.
   */
  virtual passivedouble WorkEstimateWallFunctions(CConfig              *config,
                                                  const unsigned short nPointsWF,
                                                  const unsigned short elemType) {

    SU2_MPI::Error(string("This function must be overwritten by the derived class"),
                   CURRENT_FUNCTION);
    return 0;
  }

  /*-----------------------------------------------------------------------------------*/
  /*---                  Inline public member functions.                            ---*/
  /*-----------------------------------------------------------------------------------*/

  /*!
   * \brief Function, which returns the const pointer to the grid connectivity
   *        of the requested face of the volume element.
   * \param[in] ind - Index of the face for which the connectivity is requested.
   * \return The pointer to the connectivity of the requested face of the volume element.
   */
  inline const unsigned short *GetGridConnFace(const unsigned short ind) const {
    return gridConnFaces[ind].data();
  }

  /*!
   * \brief Function, which makes available the number of total DOFs of the element.
   * \return  The number of total DOFs.
   */
  inline unsigned short GetNDOFs(void) const {return nDOFs;}

  /*!
   * \brief Function, which makes available the padded number of total DOFs of the element.
   * \return  The padded number of total DOFs.
   */
  inline unsigned short GetNDOFsPad(void) const {return nDOFsPad;}

  /*!
   * \brief Function, which makes available the number of total integration points of the element.
   * \return  The number of total integration points.
   */
  inline unsigned short GetNIntegration(void) const {return nIntegration;}

  /*!
   * \brief Function, which makes available the padded number of total integration points of the element.
   * \return  The padded number of total integration points.
   */
  inline unsigned short GetNIntegrationPad(void) const {return nIntegrationPad;}

  /*!
   * \brief Function, which makes available the order of the polynomial
   *        that is integrated exactly by the integration rule.
   * \return  The order of polynomial that is integrated exactly.
   */
  inline unsigned short GetOrderExact(void) const {return orderExact;}

  /*!
   * \brief Function, which makes available the polynomial degree of the element.
   * \return  The polynomial degree of the element.
   */
  inline unsigned short GetPolyDegree(void) const {return nPoly;}

  /*!
   * \brief Function, which makes available the type of the element.
   * \return  The type of the element using the VTK convention.
   */
  inline unsigned short GetVTK_Type(void) const {return VTK_Type;}

  /*!
   * \brief Function, which makes available the type of the element in subConn1ForPlotting.
   * \return  The type of the elements in subConn1ForPlotting using the VTK convention.
   */
  inline unsigned short GetVTK_SubType1(void) const {return VTK_SubType1;}

  /*!
   * \brief Function, which makes available the type of the element in subConn2ForPlotting.
   * \return  The type of the elements in subConn1ForPlotting using the VTK convention.
   */
  inline unsigned short GetVTK_SubType2(void) const {return VTK_SubType2;}

  /*!
   * \brief Function, which makes available the number of sub-elements of type 1 for plotting.
   * \return  The number of sub-elements of type 1 for plotting.
   */
  inline unsigned short GetNSubElemsType1(void) const {return subConn1ForPlotting.size()/GetNDOFsPerSubElem(VTK_SubType1);}

  /*!
   * \brief Function, which makes available the number of sub-elements of type 2 for plotting.
   * \return  The number of sub-elements of type 2 for plotting.
   */
  inline unsigned short GetNSubElemsType2(void) const {return subConn2ForPlotting.size()/GetNDOFsPerSubElem(VTK_SubType2);}

  /*!
   * \brief Function, which makes available the the connectivity of the linear elements of type 1 as a const pointer.
   * \return  The pointer to the local connectivity of the linear elements of type 1.
   */
  inline const unsigned short *GetSubConnType1(void) const {return subConn1ForPlotting.data();}

  /*!
   * \brief Function, which makes available the connectivity of the linear elements of type 2 as a const pointer.
   * \return  The pointer to the local connectivity of the linear elements of type 2.
   */
  inline const unsigned short *GetSubConnType2(void) const {return subConn2ForPlotting.data();}

  /*!
   * \brief Function, which makes available the integration weights as a const pointer.
   * \return  The pointer to the integration weights.
   */
  inline const passivedouble *GetIntegrationWeights(void) const {return wIntegration.data();}

  /*!
   * \brief Function, which checks if the function arguments correspond to this standard element.
   * \param[in] val_VTK_Type   - Type of the element using the VTK convention.
   * \param[in] val_nPoly      - Polynomial degree of the element.
   * \param[in] val_orderExact - Order of the polynomial that is integrated exactly.
   * \return Whether or not the function arguments correspond to this standard element.
   */
  inline bool SameStandardElement(unsigned short val_VTK_Type,
                                  unsigned short val_nPoly,
                                  unsigned short val_orderExact) {
    if(val_VTK_Type   != VTK_Type)   return false;
    if(val_nPoly      != nPoly)      return false;
    if(val_orderExact != orderExact) return false;
    return true;
  }

  /*-----------------------------------------------------------------------------------*/
  /*---                 Regular public member functions.                            ---*/
  /*-----------------------------------------------------------------------------------*/

  /*!
   * \brief Function, which computes the data and/or derivatives in the
   *        integration points from the known data in the DOFs.
   * \param[in]  matB    - Matrix that contains the input data.
   * \param[in]  ldb     - Leading dimension of matB (gemm convention).
   * \param[in]  ldc     - Leading dimension of matC (gemm convention).
   * \param[in]  n       - Second dimension of matB and matC (gemm convention).
   * \param[out] matC    - Result of the multiplication C = A*B.
   * \param[out] matDerC - Result of the multiplication CDer = ADer*B.
   * \param[in]  config  - Pointer to the configuration. Used for the timings.
   */
  void DataIntegrationPoints(const ColMajorMatrix<su2double>    &matB,
                             const unsigned short               ldb,
                             const unsigned short               ldc,
                             const unsigned short               n,
                             ColMajorMatrix<su2double>          *matC,
                             vector<ColMajorMatrix<su2double> > *matDerC,
                             const CConfig                      *config);
  /*!
   * \brief Function, which makes available the number of DOFs of a linear element, used for plotting.
   * \return  The number of DOFs of the linear element.
   */
  unsigned short GetNDOFsPerSubElem(unsigned short val_VTK_Type) const;

  /*!
   * \brief Function, which computes the actual volume metric points.
   * \param[in,out] matMetricTerms - On input the derivatives of the coordinates w.r.t.
   *                                 the parametric coordinates. On output the metric terms.
   * \param[out] Jacobians         - Vector to store the Jacobians of the transformation.
   */
  void MetricTermsVolume(vector<ColMajorMatrix<su2double> > &matMetricTerms,
                         su2activevector                    &Jacobians);

  /*!
   * \brief Function, which computes the metric terms in the volume integration points.
   * \param[in]  LGLDistribution - Whether or not the LGL node distribution must be used.
   * \param[in]  matCoor         - Matrix that contains the coordinates of the grid DOFs.
   * \param[out] matMetricTerms  - Vector of matrices to store the metric terms.
   * \param[out] Jacobians       - Vector to store the Jacobians of the transformation.
   */
  void MetricTermsVolumeIntPoints(const bool                         LGLDistribution,
                                  ColMajorMatrix<su2double>          &matCoor,
                                  vector<ColMajorMatrix<su2double> > &matMetricTerms,
                                  su2activevector                    &Jacobians);

  /*!
   * \brief Function, which computes the metric terms needed to compute the 2nd derivatives
   *        in the volume integration points.
   * \param[in]  matCoor              - Matrix that contains the coordinates of the grid DOFs.
   * \param[in]  matMetricTerms       - Vector of matrices, which contains the standard metric terms.
   * \param[in]  Jacobians            - Vector, which contains the Jacobians of the transformation.
   * \param[out] matMetricTerms2ndDer - Vector of matrices to store the metric terms for the
   *                                    2nd derivatives.
   */
  void MetricTerms2ndDerVolumeIntPoints(ColMajorMatrix<su2double>          &matCoor,
                                        vector<ColMajorMatrix<su2double> > &matMetricTerms,
                                        su2activevector                    &Jacobians,
                                        vector<ColMajorMatrix<su2double> > &matMetricTerms2ndDer);

  /*!
   * \brief Function, which computes the metric terms in the nodal solution DOFs.
   * \param[in]  matCoor         - Matrix that contains the coordinates of the grid DOFs.
   * \param[out] matMetricTerms  - Vector of matrices to store the metric terms.
   * \param[out] Jacobians       - Vector to store the Jacobians of the transformation.
   */
  void MetricTermsSolDOFs(ColMajorMatrix<su2double>          &matCoor,
                          vector<ColMajorMatrix<su2double> > &matMetricTerms,
                          su2activevector                    &Jacobians);

  /*!
   * \brief Function, which computes the mininum and maximum value of the face Jacobians
   *        of the transformation to the standard element as well as the minimum value
   *        of the cosine of the angles between the unit outward normals.
   * \param[in]  LGLDistribution - Whether or not the LGL node distribution must be used.
   * \param[in]  matCoor         - Matrix that contains the coordinates of the grid DOFs.
   * \param[out] matDerCoor      - Vector of matrices to store the derivatives of
   *                               the coordinates in the integration point of the face.
   * \param[out] unitNormals     - Matrix to store the unit normals in the integration
   *                               points of the face.
   * \param[out] Jacobians       - Vector to store the Jacobians of the transformation
   *                               in the integration points of the face.
   * \param[out] jacMin          - Minimum value of the Jacobian.
   * \param[out] jacMax          - Maximum value of the Jacobian.
   * \param[out] cosAngleMin     - Minimum value of the cosine of the angles between
   *                               the unit outward normals.
   */
  void MinMaxFaceJacobians(const bool                         LGLDistribution,
                           ColMajorMatrix<su2double>          &matCoor,
                           vector<ColMajorMatrix<su2double> > &matDerCoor,
                           ColMajorMatrix<su2double>          &unitNormals,
                           su2activevector                    &Jacobians,
                           su2double                          &jacMin,
                           su2double                          &jacMax,
                           su2double                          &cosAngleMin);

  /*!
   * \brief Function, which computes the mininum and maximum value of the Jacobian of
   *        the transformation to the standard element.
   * \param[in]  LGLDistribution - Whether or not the LGL node distribution must be used.
   * \param[in]  matCoor         - Matrix that contains the coordinates of the grid DOFs.
   * \param[out] matMetricTerms  - Vector of matrices to store the metric terms.
   * \param[out] Jacobians       - Vector to store the Jacobians of the transformation.
   * \param[out] jacMin          - Minimum value of the Jacobian.
   * \param[out] jacMax          - Maximum value of the Jacobian.
   */
  void MinMaxJacobians(const bool                         LGLDistribution,
                       ColMajorMatrix<su2double>          &matCoor,
                       vector<ColMajorMatrix<su2double> > &matMetricTerms,
                       su2activevector                    &Jacobians,
                       su2double                          &jacMin,
                       su2double                          &jacMax);

  /*!
   * \brief Function, which computes the face normals and the Jacobian.
   * \param[in]  LGLDistribution - Whether or not the LGL node distribution must be used.
   * \param[in]  matCoor         - Matrix that contains the coordinates of the grid DOFs.
   * \param[out] matDerCoor      - Vector of matrices to store the derivatives of
   *                               the coordinates in the integration point of the face.
   * \param[out] unitNormals     - Matrix to store the unit normals in the integration
   *                               points of the face.
   * \param[out] Jacobians       - Vector to store the Jacobians of the transformation
   *                               in the integration points of the face.
   */
  void UnitFaceNormals(const bool                         LGLDistribution,
                       ColMajorMatrix<su2double>          &matCoor,
                       vector<ColMajorMatrix<su2double> > &matDerCoor,
                       ColMajorMatrix<su2double>          &unitNormals,
                       su2activevector                    &Jacobians);

protected:

  /*-----------------------------------------------------------------------------------*/
  /*---                         Protected member functions.                         ---*/
  /*-----------------------------------------------------------------------------------*/

  /*!
   * \brief Function, which checks the sum of the elements of a row of a matrix
   *        in column major order.
   * \param[in] nRows          - Number of rows of the matrix.
   * \param[in] nCols          - Number of columns of the matrix.
   * \param[in] sumRowElements - Required value of the sum of the row elements.
   * \param[in] mat            - Matrix for which the row sums must be determined.
   */
  void CheckRowSum(const unsigned short                nRows,
                   const unsigned short                nCols,
                   const passivedouble                 sumRowElements,
                   const ColMajorMatrix<passivedouble> &mat);

  /*!
   * \brief Function, which computes the value of the gradient of the
   *        Jacobi polynomial for the given x-coordinate.
   * \param[in] n     - Order of the Jacobi polynomial.
   * \param[in] alpha - Alpha coefficient of the Jacobi polynomial.
   * \param[in] beta  - Beta coefficient of the Jacobi polynomial.
   * \param[in] x     - Coordinate (-1 <= x <= 1) for which the gradient of
   *                    the Jacobi polynomial must be evaluated.
   * \return            The value of the gradient of the normalized Jacobi polynomial
   *                    of order n for the given value of x.
   */
  passivedouble GradNormJacobi(unsigned short n,
                               unsigned short alpha,
                               unsigned short beta,
                               passivedouble  x);

  /*!
   * \brief Function, which computes the value of the Hessian (2nd derivative) of the
   *        Jacobi polynomial for the given x-coordinate.
   * \param[in] n     - Order of the Jacobi polynomial.
   * \param[in] alpha - Alpha coefficient of the Jacobi polynomial.
   * \param[in] beta  - Beta coefficient of the Jacobi polynomial.
   * \param[in] x     - Coordinate (-1 <= x <= 1) for which the gradient of the
   *                    Jacobi polynomial must be evaluated.
   * \return            The value of the 2nd derivative of the normalized Jacobi polynomial
   *                    of order n for the given value of x.
   */
  passivedouble HesNormJacobi(unsigned short n,
                              unsigned short alpha,
                              unsigned short beta,
                              passivedouble  x);

  /*!
   * \brief Function, which computes the value of the Jacobi polynomial for the given x-coordinate.
   * \param[in] n     - Order of the Jacobi polynomial.
   * \param[in] alpha - Alpha coefficient of the Jacobi polynomial.
   * \param[in] beta  - Beta coefficient of the Jacobi polynomial.
   * \param[in] x     - Coordinate (-1 <= x <= 1) for which the Jacobi polynomial must be evaluated.
   * \return            The value of the normalized Jacobi polynomial of order n for the given value of x.
   */
  passivedouble NormJacobi(unsigned short n,
                           unsigned short alpha,
                           unsigned short beta,
                           passivedouble  x);

  /*!
   * \brief Function, which is an interface to the actual gemm functionality.
   * \param[in]  gemm   - MKL jitted gemm kernel, if used.
   * \param[in]  jitter - Pointer with internal data for the MKL jitted gemm kernel.
   * \param[in]  M      - First matrix dimension of A and C in the gemm call.
   * \param[in]  N      - Second matrix dimension of B and C in the gemm call.
   * \param[in]  K      - First matrix dimension of B and second matrix dimension
   *                      of A in the gemm call.
   * \param[in]  LDA    - Leading dimension of A.
   * \param[in]  LDB    - Leading dimension of B.
   * \param[in]  LDC    - Leading dimension of C.
   * \param[in]  A      - Matrix A in the gemm call.
   * \param[in]  B      - Matrix B in the gemm call.
   * \param[out] C      - Matrix C in the gemm call.
   * \param[out] config - Object used for the timing of the gemm call.
   */
  void OwnGemm(dgemm_jit_kernel_t            &gemm,
               void                          *&jitter,
               const int                     M,
               const int                     N,
               const int                     K,
               const int                     LDA,
               const int                     LDB,
               const int                     LDC,
               ColMajorMatrix<passivedouble> &A,
               ColMajorMatrix<su2double>     &B,
               ColMajorMatrix<su2double>     &C,
               const CConfig                 *config);

  /*!
   * \brief Function, which sets up the jitted GEMM call when MKL is used.
   * \param[in]  M          - First matrix dimension of A and C in the gemm call.
   * \param[in]  N          - Second matrix dimension of B and C in the gemm call.
   * \param[in]  K          - First matrix dimension of B and second matrix dimension
   *                          of A in the gemm call.
   * \param[in]  LDA        - Leading dimension of A.
   * \param[in]  LDB        - Leading dimension of B.
   * \param[in]  LDC        - Leading dimension of C.
   * \param[out] val_jitter - Pointer with internal data for the MKL jitted gemm kernel.
   * \param[out] val_gemm   - MKL jitted gemm kernel to be created.
   */
  void SetUpJittedGEMM(const int          M,
                       const int          N,
                       const int          K,
                       const int          LDA,
                       const int          LDB,
                       const int          LDC,
                       void               *&val_jitter,
                       dgemm_jit_kernel_t &val_gemm);
};
