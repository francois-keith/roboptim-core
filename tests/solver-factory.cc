// Copyright (C) 2014 by Benjamin Chrétien, CNRS.
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

#include "shared-tests/fixture.hh"

#include <iostream>

#include <roboptim/core/io.hh>
#include <roboptim/core/plugin/dummy.hh>
#include <roboptim/core/twice-differentiable-function.hh>

using namespace roboptim;

// Specify the solver that will be used.
typedef Solver<TwiceDifferentiableFunction,
               boost::mpl::vector<TwiceDifferentiableFunction> > solver_t;

// Unconstrained solver
typedef Solver<TwiceDifferentiableFunction,
               boost::mpl::vector<> > solver_uc_t;

// Output stream
boost::shared_ptr<boost::test_tools::output_test_stream> output;

struct F : public TwiceDifferentiableFunction
{
  F () : TwiceDifferentiableFunction (1, 1, "0")
  {
  }

  void
  impl_compute (result_t&, const argument_t&) const throw ()
  {
  }

  void
  impl_gradient (gradient_t&, const argument_t&, size_type) const throw ()
  {
  }

  void
  impl_hessian (hessian_t&, const argument_t&, size_type) const throw ()
  {
  }
};

BOOST_FIXTURE_TEST_SUITE (core, TestSuiteConfiguration)

BOOST_AUTO_TEST_CASE (solver_factory)
{
  boost::shared_ptr<boost::test_tools::output_test_stream>
    output = retrievePattern ("solver-factory");

  // Create cost function.
  F f;

  // Create problem.
  solver_t::problem_t pb (f);

  // Set bounds for all optimization parameters.
  for (std::size_t i = 0;
       static_cast<Function::size_type> (i) < pb.function ().inputSize (); ++i)
    pb.argumentBounds ()[i] = Function::makeInterval (0., 1.);

  // Set the starting point.
  Function::vector_t start (pb.function ().inputSize ());
  start.setZero ();

  // Initialize solver.
  SolverFactory<solver_t> factory ("dummy-td", pb);
  solver_t& solver = factory ();
  (*output) << solver << std::endl;

  BOOST_CHECK_THROW (SolverFactory<solver_t> factory_size ("dummy", pb);
                     solver_t& solver_size = factory_size (),
                     std::runtime_error);

  BOOST_CHECK_THROW (SolverFactory<solver_t> factory_plugin ("dummy-foo", pb);
                     solver_t& solver_plugin = factory_plugin (),
                     std::runtime_error);

  solver_uc_t::problem_t pb_uc (f);
  BOOST_CHECK_THROW (SolverFactory<solver_uc_t> factory_type ("dummy-td", pb_uc);
                     solver_uc_t& solver_type = factory_type (),
                     std::runtime_error);

  std::cout << output->str () << std::endl;
  BOOST_CHECK (output->match_pattern ());
}

BOOST_AUTO_TEST_SUITE_END ()
