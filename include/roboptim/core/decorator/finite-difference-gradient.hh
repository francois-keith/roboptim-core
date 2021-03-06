// Copyright (C) 2009 by Thomas Moulard, AIST, CNRS, INRIA.
//
// This file is part of the roboptim.
//
// roboptim is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// roboptim is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with roboptim.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ROBOPTIM_CORE_DECORATOR_FINITE_DIFFERENCE_GRADIENT_HH
# define ROBOPTIM_CORE_DECORATOR_FINITE_DIFFERENCE_GRADIENT_HH

# include <stdexcept>
# include <string>
# include <ostream>

# include <boost/shared_ptr.hpp>

# include <roboptim/core/fwd.hh>
# include <roboptim/core/differentiable-function.hh>
# include <roboptim/core/portability.hh>
# include <roboptim/core/deprecated.hh>

namespace roboptim
{
  /// \addtogroup roboptim_decorator
  /// @{

  /// \brief Default threshold for checkGradient.
  static const double finiteDifferenceThreshold = 1e-4;
  /// \brief Default epsilon for finite difference class.
  static const double finiteDifferenceEpsilon = 1e-8;

  /// \brief Exception thrown when a gradient check fails.
  template <typename T>
  class BadGradient : public std::runtime_error
  {
  public:
    ROBOPTIM_DIFFERENTIABLE_FUNCTION_FWD_TYPEDEFS_
    (GenericDifferentiableFunction<T>);

    /// \brief Default constructor.
    BadGradient (const_argument_ref x,
		 const_gradient_ref analyticalGradient,
		 const_gradient_ref finiteDifferenceGradient,
		 const value_type& threshold);

    virtual ~BadGradient () throw ();

    /// \brief Display the exception on the specified output stream.
    ///
    /// \param o output stream used for display
    /// \return output stream
    virtual std::ostream& print (std::ostream& o) const;

    /// \brief Gradient has been computed for this point.
    argument_t x_;

    /// \brief Analytical gradient.
    gradient_t analyticalGradient_;

    /// \brief Gradient computed through finite differences.
    gradient_t finiteDifferenceGradient_;

    /// \brief Maximum error.
    value_type maxDelta_;

    /// \brief Component containing the maximum error.
    size_type maxDeltaComponent_;

    /// \brief Allowed threshold.
    value_type threshold_;
  };

  /// \brief Override operator<< to handle exception display.
  ///
  /// \param o output stream used for display
  /// \param f function to be displayed
  /// \return output stream
  template <typename T>
  std::ostream& operator<< (std::ostream& o,
			    const BadGradient<T>& f);

  /// \brief Exception thrown when a Jacobian check fails.
  template <typename T>
  class BadJacobian : public std::runtime_error
  {
  public:
    ROBOPTIM_DIFFERENTIABLE_FUNCTION_FWD_TYPEDEFS_
    (GenericDifferentiableFunction<T>);

    /// \brief Default constructor.
    BadJacobian (const_argument_ref x,
		 const_jacobian_ref analyticalJacobian,
		 const_jacobian_ref finiteDifferenceJacobian,
		 const value_type& threshold);

    virtual ~BadJacobian () throw ();

    /// \brief Display the exception on the specified output stream.
    ///
    /// \param o output stream used for display
    /// \return output stream
    virtual std::ostream& print (std::ostream& o) const;

    /// \brief Jacobian has been computed for this point.
    argument_t x_;

    /// \brief Analytical Jacobian.
    gradient_t analyticalJacobian_;

    /// \brief Jacobian computed through finite differences.
    gradient_t finiteDifferenceJacobian_;

    /// \brief Maximum error.
    value_type maxDelta_;

    /// \brief Components containing the maximum error.
    size_type maxDeltaRow_;
    size_type maxDeltaCol_;

    /// \brief Allowed threshold.
    value_type threshold_;
  };

  /// \brief Override operator<< to handle exception display.
  ///
  /// \param o output stream used for display
  /// \param f function to be displayed
  /// \return output stream
  template <typename T>
  std::ostream& operator<< (std::ostream& o,
			    const BadJacobian<T>& f);

  /// \brief Contains finite difference gradients policies.
  ///
  /// Each class of this algorithm implements a finite difference
  /// gradient computation algorithm.
  namespace finiteDifferenceGradientPolicies
  {
    /// \brief Interface for the finite difference gradient policies.
    template <typename T>
    class Policy
    {
    public:
      ROBOPTIM_DIFFERENTIABLE_FUNCTION_FWD_TYPEDEFS_
      (GenericDifferentiableFunction<T>);

      explicit Policy (const GenericFunction<T>& adaptee)
	: adaptee_ (adaptee),
	  column_ (adaptee.outputSize ()),
	  gradient_ (adaptee.inputSize ())
      {}

      /// \brief Virtual destructor.
      virtual ~Policy () {}

      virtual void computeGradient
      (value_type epsilon,
       gradient_ref gradient,
       const_argument_ref argument,
       size_type idFunction,
       argument_ref xEps) const = 0;

      virtual void computeColumn
      (value_type epsilon,
       gradient_ref column,
       const_argument_ref argument,
       size_type colIdx,
       argument_ref xEps) const = 0;

      virtual void computeJacobian
      (value_type epsilon,
       jacobian_ref jacobian,
       const_argument_ref argument,
       argument_ref xEps) const;

    protected:
      /// \brief Wrapped function.
      const GenericFunction<T>& adaptee_;

      /// \brief Vector storing temporary Jacobian column.
      mutable vector_t column_;

      /// \brief Vector storing temporary Jacobian row.
      mutable gradient_t gradient_;
    };

    /// \brief Fast finite difference gradient computation.
    ///
    /// Finite difference is computed using forward difference.
    template <typename T>
    class Simple : public Policy<T>
    {
    public:
      ROBOPTIM_DIFFERENTIABLE_FUNCTION_FWD_TYPEDEFS_
      (GenericDifferentiableFunction<T>);

      typedef Policy<T> policy_t;

      explicit Simple (const GenericFunction<T>& adaptee)
	: Policy<T> (adaptee),
	result_ (adaptee.outputSize ()),
	resultEps_ (adaptee.outputSize ())
      {}

      void computeColumn
      (value_type epsilon,
       gradient_ref column,
       const_argument_ref argument,
       size_type colIdx,
       argument_ref xEps) const;

      void computeGradient
      (value_type epsilon,
       gradient_ref gradient,
       const_argument_ref argument,
       size_type idFunction,
       argument_ref xEps) const;

      void computeJacobian
      (value_type epsilon,
       jacobian_ref jacobian,
       const_argument_ref argument,
       argument_ref xEps) const;

    private:
      mutable result_t result_;
      mutable result_t resultEps_;
    };

    /// \brief Precise finite difference gradient computation.
    ///
    /// Finite difference is computed using five-points stencil
    /// (i.e. \f$\{x-2h, x-h, x, x+h, x+2h\}\f$).
    template <typename T>
    class FivePointsRule : public Policy<T>
    {
    public:
      ROBOPTIM_DIFFERENTIABLE_FUNCTION_FWD_TYPEDEFS_
      (GenericDifferentiableFunction<T>);

      explicit FivePointsRule (const GenericFunction<T>& adaptee)
	: Policy<T> (adaptee),
	tmpResult_ (adaptee.outputSize ())
      {}

      void computeColumn
      (value_type epsilon,
       gradient_ref column,
       const_argument_ref argument,
       size_type colIdx,
       argument_ref xEps) const;

      void computeGradient
      (value_type epsilon,
       gradient_ref gradient,
       const_argument_ref argument,
       size_type idFunction,
       argument_ref xEps) const;

      void computeJacobian
      (value_type epsilon,
       jacobian_ref jacobian,
       const_argument_ref argument,
       argument_ref xEps) const;

      void
      compute_deriv (typename GenericFunction<T>::size_type j,
		     double h,
		     double& result,
		     double& round,
		     double& trunc,
		     typename GenericFunction<T>::const_argument_ref argument,
		     typename GenericFunction<T>::size_type idFunction,
		     typename GenericFunction<T>::argument_ref xEps)
	const;

    private:
      mutable result_t tmpResult_;

    };
  } // end of namespace policy.

  /// \brief Compute automatically a gradient with finite differences.
  ///
  /// Finite difference gradient is a method to approximate a function's
  /// gradient. It is particularly useful in RobOptim to avoid the need
  /// to compute the analytical gradient manually.
  ///
  /// This class takes a Function as its input and wraps it into a derivable
  /// function.
  ///
  /// The one dimensional formula is:
  /// \f[f'(x)\approx {f(x+\epsilon)-f(x)\over \epsilon}\f]
  /// where \f$\epsilon\f$ is a constant given when calling the class
  /// constructor.
  template <typename T, typename FdgPolicy>
  class GenericFiniteDifferenceGradient
    : public GenericDifferentiableFunction<T>,
      protected FdgPolicy
  {
  public:
    ROBOPTIM_DIFFERENTIABLE_FUNCTION_FWD_TYPEDEFS_
    (GenericDifferentiableFunction<T>);

    /// \brief Instantiate a finite differences gradient.
    ///
    /// Instantiate a derivable function that will wrap a non
    /// derivable function and compute automatically its gradient
    /// using finite differences.
    /// \param f shared pointer to the function that will be wrapped.
    /// \param e epsilon used in finite difference computation
    GenericFiniteDifferenceGradient
    (const boost::shared_ptr<const GenericFunction<T> >& f,
     value_type e = finiteDifferenceEpsilon);

    /// \brief Instantiate a finite differences gradient.
    /// WARNING: deprecated version. Prefer the shared_ptr alternative.
    ///
    /// Instantiate a derivable function that will wrap a non
    /// derivable function and compute automatically its gradient
    /// using finite differences.
    /// \param f function that will e wrapped
    /// \param e epsilon used in finite difference computation
    // TODO: remove after enough releases (deprecated in 3.3).
    ROBOPTIM_CORE_DEPRECATED GenericFiniteDifferenceGradient
    (const GenericFunction<T>& f,
     value_type e = finiteDifferenceEpsilon);

    ~GenericFiniteDifferenceGradient ();

    /// \brief Display the function on the specified output stream.
    ///
    /// \param o output stream used for display
    /// \return output stream
    virtual std::ostream& print (std::ostream& o) const;

  protected:
    virtual void impl_compute (result_ref, const_argument_ref) const;
    virtual void impl_gradient (gradient_ref,
                                const_argument_ref argument,
                                size_type = 0) const;
    virtual void impl_jacobian (jacobian_ref jacobian,
                                const_argument_ref argument) const;

    std::string generateName (const GenericFunction<T>& adaptee) const;

    /// \brief Shared pointer to the wrapped function.
    const boost::shared_ptr<const GenericFunction<T> > adaptee_;

    //// \brief Epsilon used in finite differences computation.
    const value_type epsilon_;

    mutable argument_t xEps_;
  };

  /// \brief Check if a gradient is valid.
  ///
  /// Check if a gradient is valid by comparing the distance between its
  /// gradient and an automatically computed finite differences gradient.
  /// \param function function that will be checked
  /// \param functionId function id in split representation
  /// \param x point where the gradient will be evaluated
  /// \param threshold maximum tolerated error
  /// \return true if valid, false if not
  template <typename T>
  bool
  checkGradient
  (const GenericDifferentiableFunction<T>& function,
   typename GenericDifferentiableFunction<T>::size_type functionId,
   typename GenericDifferentiableFunction<T>::const_argument_ref x,
   typename GenericDifferentiableFunction<T>::value_type threshold =
   finiteDifferenceThreshold);

  template <typename T>
  void
  checkGradientAndThrow
  (const GenericDifferentiableFunction<T>& function,
   typename GenericDifferentiableFunction<T>::size_type functionId,
   typename GenericDifferentiableFunction<T>::const_argument_ref x,
   typename GenericDifferentiableFunction<T>::value_type threshold =
   finiteDifferenceThreshold)
    throw (BadGradient<T>);

  /// \brief Check if a Jacobian is valid.
  ///
  /// Check if a Jacobian is valid by comparing the distance between the
  /// matrix and an automatically computed finite differences Jacobian.
  /// \param function function that will be checked
  /// \param x point where the Jacobian will be evaluated
  /// \param threshold maximum tolerated error
  /// \return true if valid, false if not
  template <typename T>
  bool
  checkJacobian
  (const GenericDifferentiableFunction<T>& function,
   typename GenericDifferentiableFunction<T>::const_argument_ref x,
   typename GenericDifferentiableFunction<T>::value_type threshold =
   finiteDifferenceThreshold);

  template <typename T>
  void
  checkJacobianAndThrow
  (const GenericDifferentiableFunction<T>& function,
   typename GenericDifferentiableFunction<T>::const_argument_ref x,
   typename GenericDifferentiableFunction<T>::value_type threshold =
   finiteDifferenceThreshold)
    throw (BadJacobian<T>);

  /// Example shows finite differences gradient use.
  /// \example finite-difference-gradient.cc

  /// @}

} // end of namespace roboptim

# include <roboptim/core/decorator/finite-difference-gradient.hxx>
#endif //! ROBOPTIM_CORE_DECORATOR_FINITE_DIFFERENCE_GRADIENT_HH
