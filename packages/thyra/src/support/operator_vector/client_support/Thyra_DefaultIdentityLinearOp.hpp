// @HEADER
// ***********************************************************************
// 
//    Thyra: Interfaces and Support for Abstract Numerical Algorithms
//                 Copyright (2004) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#ifndef THYRA_DEFAULT_IDENTITY_LINEAR_OP_HPP
#define THYRA_DEFAULT_IDENTITY_LINEAR_OP_HPP

#include "Thyra_DefaultIdentityLinearOpDecl.hpp"
#include "Thyra_MultiVectorStdOps.hpp"
#include "Teuchos_arrayArg.hpp"

namespace Thyra {

// Constructors/initializers/accessors

template<class Scalar>
DefaultIdentityLinearOp<Scalar>::DefaultIdentityLinearOp()
{}

template<class Scalar>
DefaultIdentityLinearOp<Scalar>::DefaultIdentityLinearOp(
  const Teuchos::RCP<const VectorSpaceBase<Scalar> >   &space
  )
{
  initialize(space);
}

template<class Scalar>
void DefaultIdentityLinearOp<Scalar>::initialize(
  const Teuchos::RCP<const VectorSpaceBase<Scalar> >   &space
  )
{
  space_ = space.assert_not_null();
}

template<class Scalar>
void DefaultIdentityLinearOp<Scalar>::uninitialize()
{
  space_ = Teuchos::null;
}

// Overridden from LinearOpBase
  
template<class Scalar>
Teuchos::RCP< const VectorSpaceBase<Scalar> >
DefaultIdentityLinearOp<Scalar>::range() const
{
  return space_;
}

template<class Scalar>
Teuchos::RCP< const VectorSpaceBase<Scalar> >
DefaultIdentityLinearOp<Scalar>::domain() const
{
  return space_;
}
  
template<class Scalar>
Teuchos::RCP<const LinearOpBase<Scalar> >
DefaultIdentityLinearOp<Scalar>::clone() const
{
  typedef DefaultIdentityLinearOp<Scalar> this_t;
  if(space_.get())
    return Teuchos::rcp(new this_t(space_));
  return Teuchos::rcp(new this_t());
}
  
// Overridden from Teuchos::Describable

template<class Scalar>
std::string DefaultIdentityLinearOp<Scalar>::description() const
{
  typedef Teuchos::ScalarTraits<Scalar>  ST;
  std::ostringstream oss;
  oss
    << "Thyra::DefaultIdentityLinearOp<" << ST::name() << ">{"
    << "space="<<(space_.get()?space_->description():"NULL")
    << "}";
  return oss.str();
}

// protected

// Overridden from SingleScalarLinearOpBase

template<class Scalar>
bool DefaultIdentityLinearOp<Scalar>::opSupported(ETransp M_trans) const
{
  return true;
}

template<class Scalar>
void DefaultIdentityLinearOp<Scalar>::apply(
  const ETransp                     M_trans
  ,const MultiVectorBase<Scalar>    &X
  ,MultiVectorBase<Scalar>          *Y
  ,const Scalar                     alpha
  ,const Scalar                     beta
  ) const
{
  using Teuchos::arrayArg;
#ifdef TEUCHOS_DEBUG
  THYRA_ASSERT_LINEAR_OP_MULTIVEC_APPLY_SPACES(
    "DefaultIdentityLinearOp<Scalar>::apply(...)",*this,M_trans,X,Y
    );
#endif // TEUCHOS_DEBUG  
  Thyra::linear_combination<Scalar>(
    1,arrayArg<Scalar>(alpha),arrayArg<const MultiVectorBase<Scalar>*>(&X)()
    ,beta,Y
    );
}

}	// end namespace Thyra

template<class Scalar>
Teuchos::RCP<const Thyra::LinearOpBase<Scalar> >
Thyra::identity(
  const Teuchos::RCP<const VectorSpaceBase<Scalar> >   &space
  )
{
  return Teuchos::rcp(new DefaultIdentityLinearOp<Scalar>(space));
}

#endif	// THYRA_DEFAULT_IDENTITY_LINEAR_OP_HPP
