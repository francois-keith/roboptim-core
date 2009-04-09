// Copyright (C) 2009 by Thomas Moulard, FIXME.
//
// This file is part of the liboptimization.
//
// liboptimization is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// liboptimization is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with liboptimization.  If not, see <http://www.gnu.org/licenses/>.

/**
 * \brief Declaration of the Solver class.
 */

#ifndef OPTIMIZATION_SOLVER_HH
# define OPTIMIZATION_SOLVER_HH
# include <liboptimization/fwd.hh>
# include <liboptimization/problem.hh>
# include <liboptimization/generic-solver.hh>

namespace optimization
{
  template <typename F, typename C>
  class Solver : public GenericSolver
  {
  public:
    typedef Problem<F, C> problem_t;

    explicit Solver (const problem_t&) throw ();
    ~Solver () throw ();

    const problem_t& getProblem () const throw ();
  protected:
    const problem_t& problem_;
  };



}; // end of namespace optimization

# include <liboptimization/solver.hxx>
#endif //! OPTIMIZATION_SOLVER_HH