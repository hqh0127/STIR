//
// $Id$
//
/*!

  \file
  \ingroup symmetries

  \brief Declaration of class RelatedDensels

  \author Sanida Mustafovic
  \author Kris Thielemans
  \author PARAPET project

  $Date$
  $Revision$
*/
/*
    Copyright (C) 2000 PARAPET partners
    Copyright (C) 2000- $Date$, Hammersmith Imanet Ltd
    See STIR/LICENSE.txt for details
*/
#ifndef __RelatedDensels_H__
#define __RelatedDensels_H__

#include "stir/shared_ptr.h"
#include "stir/Densel.h"
#include <vector>
#include <iterator>

#ifndef STIR_NO_NAMESPACES
using std::size_t;
using std::ptrdiff_t;
using std::random_access_iterator_tag;
using std::vector;
#endif

START_NAMESPACE_STIR

class DataSymmetriesForDensels;
/*! 
  \ingroup symmetries
  \brief This class contains all information about a set of densels related 
  by symmetry.
*/
class RelatedDensels 
{
public:
 //! typedefs for iterator support


  typedef random_access_iterator_tag iterator_category;  
  typedef Densel value_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;

  //! typedefs to make it partly comply with STL requirements
#ifndef STIR_NO_NAMESPACES
  typedef std::vector<Densel>::iterator iterator;
  typedef std::vector<Densel>::const_iterator const_iterator;
#else
  typedef vector<Densel>::iterator iterator;
  typedef vector<Densel>::const_iterator const_iterator;
#endif
   //!Default constructor: creates no densels, no symmetries  
  inline  RelatedDensels();

  //! get the number of related densels
  inline int get_num_related_densels() const;

  //! get 'basic' densel coordinates
  inline Densel get_basic_densel() const;

  // get the pointer to a ProjDataInfo class 
  // inline const ProjDataInfo * get_proj_data_info_ptr() const;

  //! return the symmetries used
  inline const DataSymmetriesForDensels* get_symmetries_ptr() const ;
 
  //! get an empty copy
  RelatedDensels get_empty_copy() const;

 // basic iterator support

  //! use to initialise an iterator to the first element of the vector
   inline iterator begin();
   //! iterator 'past' the last element of the vector
   inline iterator end();
    //! use to initialise an iterator to the first element of the (const) vector
   inline const_iterator begin() const;
   //! iterator 'past' the last element of the (const) vector
   inline const_iterator end() const;



private:
   vector<Densel> related_densels;
   shared_ptr<DataSymmetriesForDensels> symmetries;
     //! a private constructor which sets the members
  inline RelatedDensels(const vector<Densel>& related_densels,
                     const shared_ptr<DataSymmetriesForDensels>& symmetries_used);
 

};

END_NAMESPACE_STIR

#include "stir/recon_buildblock/RelatedDensels.inl"

#endif //__RelatedDensels_H__


