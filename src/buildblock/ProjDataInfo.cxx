//
// $Id$
//
/*!
  \file
  \ingroup projdata

  \brief Implementation of non-inline functions of class ProjDataInfo

  \author Sanida Mustafovic
  \author Kris Thielemans
  \author PARAPET project

  $Date$
  $Revision$
*/
/*
    Copyright (C) 2000 PARAPET partners
    Copyright (C) 2000- $Date$, IRSL
    See STIR/LICENSE.txt for details
*/
#include "stir/ProjDataInfo.h"
#include "stir/ProjDataInfoCylindricalArcCorr.h"
#include "stir/ProjDataInfoCylindricalNoArcCorr.h"
#include "stir/Scanner.h"
#include "stir/Viewgram.h"
#include "stir/Sinogram.h"
#include "stir/SegmentBySinogram.h"
#include "stir/SegmentByView.h"
#include "stir/RelatedViewgrams.h"
#include "stir/ViewSegmentNumbers.h"
#include "stir/Coordinate2D.h"
#include "stir/Coordinate3D.h"
#include "stir/IndexRange2D.h"
#include "stir/IndexRange3D.h"
#include "stir/Bin.h"
// include for ask and ask_num
#include "stir/utilities.h"

#include <iostream>
#include <typeinfo>
#include <vector>
#include <algorithm>
#ifdef BOOST_NO_STRINGSTREAM
#include <strstream.h>
#else
#include <sstream>
#endif


#ifndef STIR_NO_NAMESPACES
using std::vector;
using std::cerr;
using std::cout;
using std::endl;
using std::ends;
using std::equal;
#endif

START_NAMESPACE_STIR


float 
ProjDataInfo::get_sampling_in_t(const Bin& bin) const
{
  return
    (get_t(Bin(bin.segment_num(), bin.view_num(), bin.axial_pos_num()+1,bin.tangential_pos_num())) -
     get_t(Bin(bin.segment_num(), bin.view_num(), bin.axial_pos_num()-1,bin.tangential_pos_num()))
     )/2;
}

float 
ProjDataInfo::get_sampling_in_m(const Bin& bin) const
{
  return
    (get_m(Bin(bin.segment_num(), bin.view_num(), bin.axial_pos_num()+1,bin.tangential_pos_num())) -
     get_m(Bin(bin.segment_num(), bin.view_num(), bin.axial_pos_num()-1,bin.tangential_pos_num()))
     )/2;
}


float 
ProjDataInfo::get_sampling_in_s(const Bin& bin) const
{
  return
    (get_s(Bin(bin.segment_num(), bin.view_num(), bin.axial_pos_num(),bin.tangential_pos_num()+1)) -
     get_s(Bin(bin.segment_num(), bin.view_num(), bin.axial_pos_num(),bin.tangential_pos_num()-1))
     )/2;
}

void 
ProjDataInfo::set_num_views(const int num_views)
{
  min_view_num = 0;
  max_view_num = num_views-1;
}

void 
ProjDataInfo::set_num_tangential_poss(const int num_tang_poss)
{

  min_tangential_pos_num = -(num_tang_poss/2);
  max_tangential_pos_num = min_tangential_pos_num + num_tang_poss-1;
}

/*! Uses as min_axial_pos_per_seg 0 */
void 
ProjDataInfo::set_num_axial_poss_per_segment(const VectorWithOffset<int>& num_axial_pos_per_segment)
{
  // first do assignments to make the members the correct size 
  // (data will be overwritten)
  min_axial_pos_per_seg= num_axial_pos_per_segment;
  max_axial_pos_per_seg= num_axial_pos_per_segment;
  
  for (int i=num_axial_pos_per_segment.get_min_index(); 
       i<=num_axial_pos_per_segment.get_max_index();
       i++)
  {
    min_axial_pos_per_seg[i]=0;
    max_axial_pos_per_seg[i]=num_axial_pos_per_segment[i]-1;
  }
    
}

/*! No checks are done on validity of the min_ax_pos_num argument */
void 
ProjDataInfo::set_min_axial_pos_num(const int min_ax_pos_num, const int segment_num)
{
  min_axial_pos_per_seg[segment_num] = min_ax_pos_num;
}
/*! No checks are done on validity of the max_ax_pos_num argument */
void 
ProjDataInfo::set_max_axial_pos_num(const int max_ax_pos_num, const int segment_num)
{
  max_axial_pos_per_seg[segment_num] = max_ax_pos_num;
}


void
ProjDataInfo::set_min_tangential_pos_num(const int min_tang_poss)
{
  min_tangential_pos_num = min_tang_poss;
}

void
ProjDataInfo::set_max_tangential_pos_num(const int max_tang_poss)
{
  max_tangential_pos_num = max_tang_poss;
}


ProjDataInfo::ProjDataInfo()
{}




ProjDataInfo::ProjDataInfo(const shared_ptr<Scanner>& scanner_ptr_v,
                           const VectorWithOffset<int>& num_axial_pos_per_segment_v, 
                           const int num_views_v, 
                           const int num_tangential_poss_v)
			   :scanner_ptr(scanner_ptr_v)

{ 
  set_num_views(num_views_v);
  set_num_tangential_poss(num_tangential_poss_v);
  set_num_axial_poss_per_segment(num_axial_pos_per_segment_v);
}

string
ProjDataInfo::parameter_info()  const
{

#ifdef BOOST_NO_STRINGSTREAM
  // dangerous for out-of-range, but 'old-style' ostrstream seems to need this
  char str[30000];
  ostrstream s(str, 30000);
#else
  std::ostringstream s;
#endif
  s << scanner_ptr->parameter_info();

  s << "Parameters relating to this data :\n";

  s << "\nSegment_num range:           ("
      << get_min_segment_num()
      << ", " <<  get_max_segment_num() << ")\n";
  s << "Number of Views:                " << get_num_views() << endl;
  s << "Number of axial positions per seg: {";
  for (int seg_num=get_min_segment_num(); seg_num <= get_max_segment_num(); ++seg_num)
    s << get_num_axial_poss(seg_num) << " ";
  s << "}\n";
  s << "Number of tangential positions: " << get_num_tangential_poss() << endl;

  s << ends;
  return s.str();

}


void
ProjDataInfo::
reduce_segment_range(const int min_segment_num, const int max_segment_num)
{
  assert(min_segment_num >= get_min_segment_num());
  assert(max_segment_num <= get_max_segment_num());

  VectorWithOffset<int> new_min_axial_pos_per_seg(min_segment_num, max_segment_num); 
  VectorWithOffset<int> new_max_axial_pos_per_seg(min_segment_num, max_segment_num);

  for (int segment_num = min_segment_num; segment_num<= max_segment_num; ++segment_num)
  {
    new_min_axial_pos_per_seg[segment_num] =
      min_axial_pos_per_seg[segment_num];
    new_max_axial_pos_per_seg[segment_num] =
      max_axial_pos_per_seg[segment_num];
  }

  min_axial_pos_per_seg = new_min_axial_pos_per_seg;
  max_axial_pos_per_seg = new_max_axial_pos_per_seg;
  
}


Viewgram<float> 
ProjDataInfo::get_empty_viewgram(const int view_num, 
				 const int segment_num, 
				 const bool make_num_tangential_poss_odd) const
{  
  // we can't access the shared ptr, so we have to clone 'this'.
  ProjDataInfo *  proj_data_info_ptr = this->clone();
  
  if (make_num_tangential_poss_odd && (get_num_tangential_poss()%2==0))
    proj_data_info_ptr->set_max_tangential_pos_num(get_max_tangential_pos_num() + 1);
  
  Viewgram<float> v(proj_data_info_ptr, view_num, segment_num);
   
  return v;
}


Sinogram<float>
ProjDataInfo::get_empty_sinogram(const int axial_pos_num, const int segment_num,
                                      const bool make_num_tangential_poss_odd) const
{
  // we can't access the shared ptr, so we have to clone 'this'.
  ProjDataInfo *  proj_data_info_ptr = this->clone();
  
  if (make_num_tangential_poss_odd && (get_num_tangential_poss()%2==0))
    proj_data_info_ptr->set_max_tangential_pos_num(get_max_tangential_pos_num() + 1);

  Sinogram<float> s(proj_data_info_ptr, axial_pos_num, segment_num);
   
  return s;
}

SegmentBySinogram<float>
ProjDataInfo::get_empty_segment_by_sinogram(const int segment_num, 
      const bool make_num_tangential_poss_odd) const
{
  assert(segment_num >= get_min_segment_num());
  assert(segment_num <= get_max_segment_num());
 
  // we can't access the shared ptr, so we have to clone 'this'.
  ProjDataInfo *  proj_data_info_ptr = this->clone();
  
  if (make_num_tangential_poss_odd && (get_num_tangential_poss()%2==0))
    proj_data_info_ptr->set_max_tangential_pos_num(get_max_tangential_pos_num() + 1);

  SegmentBySinogram<float> s(proj_data_info_ptr, segment_num);

  return s;
}  


SegmentByView<float>
ProjDataInfo::get_empty_segment_by_view(const int segment_num, 
				   const bool make_num_tangential_poss_odd) const
{
  assert(segment_num >= get_min_segment_num());
  assert(segment_num <= get_max_segment_num());

    // we can't access the shared ptr, so we have to clone 'this'.
  ProjDataInfo *  proj_data_info_ptr = this->clone();
  
  if (make_num_tangential_poss_odd && (get_num_tangential_poss()%2==0))
    proj_data_info_ptr->set_max_tangential_pos_num(get_max_tangential_pos_num() + 1);
  
  SegmentByView<float> s(proj_data_info_ptr, segment_num);

  return s;
}

RelatedViewgrams<float> 
ProjDataInfo::get_empty_related_viewgrams(const ViewSegmentNumbers& view_segmnet_num,
                   //const int view_num, const int segment_num,
		   const shared_ptr<DataSymmetriesForViewSegmentNumbers>& symmetries_used,
		   const bool make_num_tangential_poss_odd) const
{
  vector<ViewSegmentNumbers> pairs;
  symmetries_used->get_related_view_segment_numbers(
                                                    pairs, 
                                                    ViewSegmentNumbers(view_segmnet_num.view_num(),view_segmnet_num.segment_num())
    );

  vector<Viewgram<float> > viewgrams;
  viewgrams.reserve(pairs.size());

  for (unsigned int i=0; i<pairs.size(); i++)
  {
    // TODO optimise to get shared proj_data_info_ptr
    viewgrams.push_back(get_empty_viewgram(pairs[i].view_num(),
                                          pairs[i].segment_num(), make_num_tangential_poss_odd));
  }

  return RelatedViewgrams<float>(viewgrams, symmetries_used);
}


/****************** static members **********************/

ProjDataInfo*
ProjDataInfo::ProjDataInfoCTI(const shared_ptr<Scanner>& scanner,
			      const int span, const int max_delta, 
			      const int num_views, const int num_tangential_poss,
                              const bool arc_corrected)
{
  if (span < 1)
    error("ProjDataInfoCTI: span %d has to be larger than 0\n", span);
  if (span%2 != 1)
    error("ProjDataInfoCTI: span %d has to be odd\n", span);
  if (max_delta<(span-1)/2)
    error("ProjDataInfoCTI: max_ring_difference %d has to be at least (span-1)/2, span is %d\n",
	  max_delta, span);
  
  const int num_ring = scanner->get_num_rings();
 // Construct first a temporary list of min and max ring diff per segment (0,1,2,3...)
  
  // KT changed dimension to num_ring
  vector <int> RDmintmp(num_ring);
  vector <int> RDmaxtmp(num_ring);
  
  // KT avoid float stuff 
  // RDmintmp[0]= (int) ceil(-span/2);
  RDmintmp[0] = -(span-1)/2;
  RDmaxtmp[0] = RDmintmp[0] + span - 1;

  int seg_num =0;
  while (RDmaxtmp[seg_num] < max_delta)
  {
    seg_num++;
    RDmintmp[seg_num] = span + RDmintmp[seg_num-1];
    RDmaxtmp[seg_num] = RDmintmp[seg_num] + span - 1;
  }
  // check if we went one too far
  if (RDmaxtmp[seg_num] > max_delta)
    seg_num--;

  const int max_seg_num = seg_num;
  // KT possible modifications
  // RDmintmp[i] = (i)*span - (span-1)/2;
  // RDmaxtmp[i] = (i+1)*span - (span-1)/2 -1;
  
  
  
  VectorWithOffset<int> num_axial_pos_per_segment(-max_seg_num,max_seg_num);
  VectorWithOffset<int> min_ring_difference(-max_seg_num,max_seg_num);
  VectorWithOffset<int> max_ring_difference(-max_seg_num,max_seg_num);
  
  min_ring_difference[0] = RDmintmp[0];
  max_ring_difference[0]= RDmaxtmp[0];
  
  for(int i=1; i<=max_seg_num; i++)
  {
    // KT 28/06/2001 make sure max_ring_diff>min_ring_diff for negative segments
    max_ring_difference[-i]= -RDmintmp[i];
    max_ring_difference[i]= RDmaxtmp[i];
    min_ring_difference[-i]= -RDmaxtmp[i];
    min_ring_difference[i]= RDmintmp[i];    
  }
  
  if (span==1)
  {
    num_axial_pos_per_segment[0] = num_ring;
    for(int i=1; i<=max_seg_num; i++)
    {
      num_axial_pos_per_segment[i]= 
        num_axial_pos_per_segment[-i] = num_ring -i;
    }
  }
  else
  {
    num_axial_pos_per_segment[0] = 2*num_ring -1 ; 
    for( int i=1; i<=max_seg_num; i++)
    {
      num_axial_pos_per_segment[i] = 
        num_axial_pos_per_segment[-i] = (2*num_ring -1 - 2*RDmintmp[i]); 
    }    
  }
  
  const float bin_size = scanner->get_default_bin_size();
  
  
  if (arc_corrected)
    return
    new ProjDataInfoCylindricalArcCorr(scanner,bin_size,
                                       num_axial_pos_per_segment,
                                       min_ring_difference, 
                                       max_ring_difference,
                                       num_views,num_tangential_poss);
  else
    return
    new ProjDataInfoCylindricalNoArcCorr(scanner,
                                       num_axial_pos_per_segment,
                                       min_ring_difference, 
                                       max_ring_difference,
                                       num_views,num_tangential_poss);

}

// KT 28/06/2001 added arc_corrected flag
ProjDataInfo*
ProjDataInfo::ProjDataInfoGE(const shared_ptr<Scanner>& scanner,
			     const int max_delta,
			     const int num_views, const int num_tangential_poss,
                             const bool arc_corrected)
	       
	       
{
  const int num_rings = scanner->get_num_rings();
  const float bin_size = scanner->get_default_bin_size();
  
  if(max_delta<=0)
    error("ProjDataInfo::ProjDataInfoGE: can only handle max_delta>0\n");

  const int max_segment_num = max_delta-1;
  
  VectorWithOffset<int> num_axial_pos_per_segment(-max_segment_num,max_segment_num);
  
  VectorWithOffset<int> min_ring_difference(-max_segment_num,max_segment_num); 
  VectorWithOffset<int> max_ring_difference(-max_segment_num,max_segment_num);
  
  num_axial_pos_per_segment[0] = 2*num_rings-1;
  min_ring_difference[0] = -1; 
  max_ring_difference[0] = 1;
  
  for (int i=1; i<=max_segment_num; i++)
    
  { 
    num_axial_pos_per_segment[i]=
      num_axial_pos_per_segment[-i]=num_rings-i-1;
    
    max_ring_difference[i]= 
      min_ring_difference[i]= i+1;
    max_ring_difference[-i]= 
      min_ring_difference[-i]= -i-1;
    
  }
  
  
  
  if (arc_corrected)
    return
    new ProjDataInfoCylindricalArcCorr(scanner,bin_size,
                                       num_axial_pos_per_segment,
                                       min_ring_difference, 
                                       max_ring_difference,
                                       num_views,num_tangential_poss);
  else
    return
    new ProjDataInfoCylindricalNoArcCorr(scanner,
                                       num_axial_pos_per_segment,
                                       min_ring_difference, 
                                       max_ring_difference,
                                       num_views,num_tangential_poss);
}


ProjDataInfo* ProjDataInfo::ask_parameters()
{

  Scanner * scanner_ptr = 
    Scanner::ask_parameters();
  
   const int num_views = scanner_ptr->get_max_num_views()/
     ask_num("Mash factor for views",1,16,1);

   const int num_tangential_poss=ask_num("Number of tangential positions",1,
     scanner_ptr->get_max_num_non_arccorrected_bins()+1,
     scanner_ptr->get_default_num_arccorrected_bins());
  
   int span = 0;
   while(span%2==0)
   {
     span = 
       scanner_ptr->get_type() == Scanner::Advance 
       ? 1 
       : ask_num("Span value (must be odd) : ", 1,11,1);
   }
  
   const int max_delta = ask_num("Max. ring difference acquired : ",
    0,
    scanner_ptr->get_num_rings()-1,
    scanner_ptr->get_type() == Scanner::Advance 
    ? 11 : scanner_ptr->get_num_rings()-1);
 
  const bool arc_corrected =
    ask("Is the data arc-corrected?",true);

  ProjDataInfo * pdi_ptr =
    scanner_ptr->get_type() == Scanner::Advance 
    ? ProjDataInfoGE(scanner_ptr,max_delta,num_views,num_tangential_poss,arc_corrected)
    : ProjDataInfoCTI(scanner_ptr,span,max_delta,num_views,num_tangential_poss,arc_corrected);

  cout << pdi_ptr->parameter_info() <<endl;

  return pdi_ptr;
    
}

bool 
ProjDataInfo::operator ==(const ProjDataInfo& proj) const
{  
  return
    (typeid (*this) == typeid(proj)) &&
   (get_min_view_num()==proj.get_min_view_num()) &&
   (get_max_view_num()==proj.get_max_view_num()) &&
   (get_min_tangential_pos_num() ==proj.get_min_tangential_pos_num())&&
   (get_max_tangential_pos_num() ==proj.get_max_tangential_pos_num())&&
   equal(min_axial_pos_per_seg.begin(), min_axial_pos_per_seg.end(), proj.min_axial_pos_per_seg.begin())&&
   equal(max_axial_pos_per_seg.begin(), max_axial_pos_per_seg.end(), proj.max_axial_pos_per_seg.begin())&&
   (*get_scanner_ptr()== *(proj.get_scanner_ptr()));

}



END_NAMESPACE_STIR

