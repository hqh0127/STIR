//
// $Id$: $Date$
//
/*!
  \file
  \ingroup recon_buildblock

  \brief non-inline implementations for BackProjectorByBinUsingInterpolation

  \author Kris Thielemans
  \author Claire Labbe
  \author PARAPET project
  
  \date $Date$

  \version $Revision$
*/

#include "VoxelsOnCartesianGrid.h"
#include "recon_buildblock/BackProjectorByBinUsingInterpolation.h"
#include "IndexRange4D.h"
//#include "ProjDataInfoCylindricalArcCorr.h"


START_NAMESPACE_TOMO


const DataSymmetriesForViewSegmentNumbers *
 BackProjectorByBinUsingInterpolation::get_symmetries_used() const
{ return &symmetries; }

BackProjectorByBinUsingInterpolation::
BackProjectorByBinUsingInterpolation(shared_ptr<ProjDataInfo> const& proj_data_info_ptr,
				     shared_ptr<DiscretisedDensity<3,float> > const& image_info_ptr,
				     const bool use_piecewise_linear_interpolation,
                                     const bool use_exact_Jacobian)			   
  :
  symmetries(proj_data_info_ptr,image_info_ptr),
  use_piecewise_linear_interpolation_now(use_piecewise_linear_interpolation),
  use_exact_Jacobian_now(use_exact_Jacobian)
{}

void
BackProjectorByBinUsingInterpolation::
use_exact_Jacobian(const bool use_exact_Jacobian)
{
  use_exact_Jacobian_now = use_exact_Jacobian;
}


void
BackProjectorByBinUsingInterpolation::
use_piecewise_linear_interpolation(const bool use_piecewise_linear_interpolation)
{
  use_piecewise_linear_interpolation_now = use_piecewise_linear_interpolation;
}

void BackProjectorByBinUsingInterpolation::
actual_back_project(DiscretisedDensity<3,float>& density,
		    const RelatedViewgrams<float>& viewgrams,
		    const int min_axial_pos_num, const int max_axial_pos_num,
		    const int min_tangential_pos_num, const int max_tangential_pos_num)

{
  // this will throw an exception when the cast does not work
  VoxelsOnCartesianGrid<float>& image = 
    dynamic_cast<VoxelsOnCartesianGrid<float>&>(density);
  // TODO somehow check symmetry object in RelatedViewgrams

  const int num_views = viewgrams.get_proj_data_info_ptr()->get_num_views();
  RelatedViewgrams<float>::const_iterator r_viewgrams_iter = viewgrams.begin();
  if (viewgrams.get_basic_segment_num() == 0)
  {
    // no segment symmetry
    const Viewgram<float> & pos_view =*r_viewgrams_iter;
    r_viewgrams_iter++;
    const Viewgram<float> & pos_plus90 =*r_viewgrams_iter;

    //const Viewgram<float> & pos_view = viewgrams.get_viewgram_reference(0); 
   // const Viewgram<float> & pos_plus90 = viewgrams.get_viewgram_reference(1); 
    const Viewgram<float> neg_view = pos_view.get_empty_copy(); 
    const Viewgram<float> neg_plus90 = pos_plus90.get_empty_copy(); 
    if (viewgrams.get_num_viewgrams() == 2)
    {
      back_project_view_plus_90_and_delta(
	image,
	pos_view, neg_view, pos_plus90, neg_plus90, 
	min_axial_pos_num, max_axial_pos_num,
	min_tangential_pos_num, max_tangential_pos_num);

    }
    else
    {
      assert(viewgrams.get_basic_view_num() != 0);
      assert(viewgrams.get_basic_view_num() != num_views/4);
      r_viewgrams_iter++;//2 
       const Viewgram<float> & pos_min180 =*r_viewgrams_iter;
       r_viewgrams_iter++;//3
      const Viewgram<float> & pos_min90 =*r_viewgrams_iter;
     // const Viewgram<float> & pos_min180 = viewgrams.get_viewgram_reference(2); 
      //const Viewgram<float> & pos_min90 = viewgrams.get_viewgram_reference(3); 
      const Viewgram<float> neg_min180 = pos_min180.get_empty_copy(); 
      const Viewgram<float> neg_min90 = pos_min90.get_empty_copy();     

      back_project_all_symmetries(
	image,
	pos_view, neg_view, pos_plus90, neg_plus90, 
	pos_min180, neg_min180, pos_min90, neg_min90,
	min_axial_pos_num, max_axial_pos_num,
	min_tangential_pos_num, max_tangential_pos_num);

    }
  }
  else
  {
    // segment symmetry

    const Viewgram<float> & pos_view = *r_viewgrams_iter;//0
    r_viewgrams_iter++;
    const Viewgram<float> & neg_view = *r_viewgrams_iter;//1
      r_viewgrams_iter++;
    const Viewgram<float> & pos_plus90 =*r_viewgrams_iter;//2
	r_viewgrams_iter++;
    const Viewgram<float> & neg_plus90 =*r_viewgrams_iter;//3
    //const Viewgram<float> & pos_view = viewgrams.get_viewgram_reference(0); 
    //const Viewgram<float> & neg_view = viewgrams.get_viewgram_reference(1); 
    //const Viewgram<float> & pos_plus90 = viewgrams.get_viewgram_reference(2); 
    //const Viewgram<float> & neg_plus90 = viewgrams.get_viewgram_reference(3); 
    if (viewgrams.get_num_viewgrams() == 4)
    {
      back_project_view_plus_90_and_delta(
	image,
	pos_view, neg_view, pos_plus90, neg_plus90, 
	min_axial_pos_num, max_axial_pos_num,
	min_tangential_pos_num, max_tangential_pos_num);

    }
    else
    {
      assert(viewgrams.get_basic_view_num() != 0);
      assert(viewgrams.get_basic_view_num() != num_views/4);
     r_viewgrams_iter++;//4
      const Viewgram<float> & pos_min180 =*r_viewgrams_iter;
     r_viewgrams_iter++;//5
      const Viewgram<float> & neg_min180 =*r_viewgrams_iter;
     r_viewgrams_iter++;//6
      const Viewgram<float> & pos_min90 =*r_viewgrams_iter;
     r_viewgrams_iter++;//7
       const Viewgram<float> & neg_min90 =*r_viewgrams_iter;

    //  const Viewgram<float> & pos_min180 = viewgrams.get_viewgram_reference(4); 
    //  const Viewgram<float> & neg_min180 = viewgrams.get_viewgram_reference(5); 
    //  const Viewgram<float> & pos_min90 = viewgrams.get_viewgram_reference(6); 
    //  const Viewgram<float> & neg_min90 = viewgrams.get_viewgram_reference(7);     

      back_project_all_symmetries(
	image,
	pos_view, neg_view, pos_plus90, neg_plus90, 
	pos_min180, neg_min180, pos_min90, neg_min90,
	min_axial_pos_num, max_axial_pos_num,
	min_tangential_pos_num, max_tangential_pos_num);

    }
  }


}





#if 0
/******************************************************************************
 2D
 ******************************************************************************/

// TODO rounding errors...

void 
BackProjectorByBinUsingInterpolation::
  back_project_2D_all_symmetries(const Sinogram<float> &sino, PETPlane &image, int view,
                                    const int min_bin_num, const int max_bin_num)
{
  start_timers();

  assert(sino.get_min_bin() == - sino.get_max_bin());
  assert(min_bin_num == -max_bin_num);
  assert(image.get_min_x() == - image.get_max_x());
  assert(image.get_min_y() == - image.get_max_y());
  assert(view < sino.get_num_views() / 4);
  
  assert(view != 0);
  
  const int nviews = sino.get_num_views();
  const int view90=(int)(nviews/2);
  const JacobianForIntBP jacobian(sino.scan_info, use_exact_Jacobian_now);
  
  
  const int        min90 = view90 - view;
  const int        plus90 = view90 + view;
  const int        min180 = nviews - view;
  
  const double        phi = _PI * view / nviews;
  // sadly has to be float, otherwise rounding errors...
  const float        cphi = cos(phi);
  const float        sphi = sin(phi);
  
  
  ProjDataForIntBP projs;
  //TODO loop is wrong
  for (int s = 0; s <= max_bin_num - 2; s++)
  {
    const float jac = jacobian(0, s+ 0.5);
    
    projs.view__pos_s = sino[view][s] * jac;
    projs.view__pos_sp1 =sino[view][s+1] * jac;
    projs.view__neg_s = sino[view][-s] * jac; 
    projs.view__neg_sp1 = sino[view][-s-1] * jac;
    
    projs.min90__pos_s = sino[min90][s] * jac; 
    projs.min90__pos_sp1 =sino[min90][s+1] * jac;
    projs.min90__neg_s = sino[min90][-s] * jac; 
    projs.min90__neg_sp1 = sino[min90][-s-1] * jac;
    
    projs.plus90__pos_s = sino[plus90][s] * jac; 
    projs.plus90__pos_sp1 =sino[plus90][s+1] * jac;
    projs.plus90__neg_s = sino[plus90][-s] * jac; 
    projs.plus90__neg_sp1 = sino[plus90][-s-1] * jac;
    
    projs.min180__pos_s = sino[min180][s] * jac; 
    projs.min180__pos_sp1 =sino[min180][s+1] * jac;
    projs.min180__neg_s = sino[min180][-s] * jac; 
    projs.min180__neg_sp1 = sino[min180][-s-1] * jac;
    
    backproj2D_Cho_view_viewplus90_180minview_90minview(image, projs, cphi, sphi, s);
  }
  
  stop_timers();
}

void 
BackProjectorByBinUsingInterpolation::
  back_project_2D_view_plus_90(const Sinogram<float> &sino, PETPlane &image, int view,
                               const int min_bin_num, const int max_bin_num)
{
  start_timers();

  assert(sino.get_min_bin() == - sino.get_max_bin());
  assert(min_bin_num == -max_bin_num);
  assert(image.get_min_x() == - image.get_max_x());
  assert(image.get_min_y() == - image.get_max_y());
  assert(view < sino.get_num_views() / 2);
  
  assert(view >= 0);
  assert(view < sino.get_num_views()/2);
  
  const int nviews = sino.get_num_views();
  const int view90 = nviews/2;
  const JacobianForIntBP jacobian(sino.scan_info, use_exact_Jacobian_now);
  
  
  const int        min90 = view90 - view;
  const int        plus90 = view90 + view;
  const int        min180 = nviews - view;
  
  const double        phi = _PI * view / nviews;
  const float        cphi = cos(phi);
  const float        sphi = sin(phi);
  
  
  ProjDataForIntBP projs;
  // TODO loop is wrong
  for (int s = 0; s <= max_bin_num - 2; s++)
  {
    const float jac = jacobian(0, s+ 0.5);
    
    projs.view__pos_s = sino[view][s] * jac;
    projs.view__pos_sp1 =sino[view][s+1] * jac;
    projs.view__neg_s = sino[view][-s] * jac; 
    projs.view__neg_sp1 = sino[view][-s-1] * jac;
    
    projs.plus90__pos_s = sino[plus90][s] * jac; 
    projs.plus90__pos_sp1 =sino[plus90][s+1] * jac;
    projs.plus90__neg_s = sino[plus90][-s] * jac; 
    projs.plus90__neg_sp1 = sino[plus90][-s-1] * jac;
        
    backproj2D_Cho_view_viewplus90(image, projs, cphi, sphi, s);
  }
 
  stop_timers();
}

#endif



/****************************************************************************
 real work
 ****************************************************************************/
   /*
    The version which uses all possible symmetries.
    Here 0<=view < num_views/4 (= 45 degrees)
    */

void 
BackProjectorByBinUsingInterpolation::back_project_all_symmetries(
				 VoxelsOnCartesianGrid<float>& image,
			    	 const Viewgram<float> & pos_view, 
				 const Viewgram<float> & neg_view, 
				 const Viewgram<float> & pos_plus90, 
				 const Viewgram<float> & neg_plus90, 
				 const Viewgram<float> & pos_min180, 
				 const Viewgram<float> & neg_min180, 
				 const Viewgram<float> & pos_min90, 
				 const Viewgram<float> & neg_min90,
				 const int min_axial_pos_num, const int max_axial_pos_num,
				 const int min_tangential_pos_num, const int max_tangential_pos_num)
{
  const ProjDataInfoCylindricalArcCorr* proj_data_info_cyl_ptr = 
    dynamic_cast<const ProjDataInfoCylindricalArcCorr*> (pos_view.get_proj_data_info_ptr());
 

  if ( proj_data_info_cyl_ptr==NULL)
  {
    error("\nBackProjectorByBinUsingInterpolation,\n\
     Casting failed!");
  }

  assert(min_axial_pos_num >= pos_view. get_min_axial_pos_num());
  assert(max_axial_pos_num <= pos_view. get_max_axial_pos_num());
  assert(min_tangential_pos_num >= pos_view.get_min_tangential_pos_num());
  assert(max_tangential_pos_num <= pos_view.get_max_tangential_pos_num());

  assert(min_tangential_pos_num == - max_tangential_pos_num);

  int segment_num = pos_view.get_segment_num();
  
  assert(proj_data_info_cyl_ptr ->get_average_ring_difference(segment_num) >= 0);
  assert(pos_view.get_view_num() > 0);
  assert(pos_view.get_view_num() < pos_view.get_proj_data_info_ptr()->get_num_views()/4);

  const int nviews = pos_view.get_proj_data_info_ptr()->get_num_views();

  assert(pos_plus90.get_view_num() == nviews / 2 + pos_view.get_view_num());
  assert(pos_min90.get_view_num() == nviews / 2 - pos_view.get_view_num());
  assert(pos_min180.get_view_num() == nviews - pos_view.get_view_num());
  assert(neg_view.get_view_num() == pos_view.get_view_num());
  assert(neg_plus90.get_view_num() == pos_plus90.get_view_num());
  assert(neg_min90.get_view_num() == pos_min90.get_view_num());
  assert(neg_min180.get_view_num() == pos_min180.get_view_num());

  //KTxxx not necessary anymore assert(pos_view.get_num_bins() ==image.get_x_size());
  assert(image.get_min_x() == -image.get_max_x());
  // CL&KT 05/11/98 use scan_info, enable assert
  assert(image.get_voxel_size().x() ==(proj_data_info_cyl_ptr)->get_tangential_sampling());
  // TODO rewrite code a bit to remove this assertion
  assert(image.get_min_z() == 0);

  if (pos_view.get_view_num() == 0 || pos_view.get_view_num() == nviews/4)
    {
      error("BackProjectorByBinUsingInterpolation: back_project_all_symmetries called with view 0 or 45 degrees.\n");
      //back_project_view_plus_90_and_delta(pos_view, neg_view, pos_plus90, neg_plus90, image,
      //  min_ring_num, max_ring_num, min_bin_num, max_bin_num);
      //return;
    }

  const int fovrad = (int) (pos_view.get_num_tangential_poss() / 2) - 1;/* FOV radius in voxel units */
  // TODO remove -1, it's there because otherwise a crazy value is used. Don't know why yet...
  const int max_bin_num_to_use =
    std::min(max_tangential_pos_num, image.get_max_x()-1);

  start_timers();

  // KT&MJ 07/08/98 new
  const JacobianForIntBP jacobian(proj_data_info_cyl_ptr, use_exact_Jacobian_now);

  Array<4, float > Proj2424(IndexRange4D(0, 1, 0, 3, 0, 1, 1, 4));
  const float cphi = cos(pos_view.get_view_num() * _PI / nviews);
  const float sphi = sin(pos_view.get_view_num() * _PI / nviews);
 

  // Do a loop over all rings. However, because we use interpolation of
  // a 'beam', each step takes elements from ring0 and ring0+1. So, data in
  // a ring influences beam ring0-1 and ring0. All this means that we
  // have to let ring0 run from min_ring_num-1 to max_ring_num.
  // KT 02/06/98 changed boundaries from min_ring_num, max_ring_num-1 to min_ring_num-1, max_ring_num
  for (int ring0 = min_axial_pos_num-1; ring0 <= max_axial_pos_num; ring0++)
    {
      const int r0plus = ring0 + 1; 

      // We have to fill with 0, as not all elements are set in the lines below
      if (ring0==min_axial_pos_num-1 || ring0==max_axial_pos_num)
	Proj2424.fill(0);
      
      for (int s = 0; s <= max_bin_num_to_use; s++) {
	const int splus = s + 1;
	const int ms = -s;
	const int msplus = -splus;

	// now I have to check if ring0 is in allowable range
        if (ring0 >= min_axial_pos_num)
	{
	Proj2424[0][0][0][1] = pos_view[ring0][s];
	Proj2424[0][0][0][2] = splus>max_bin_num_to_use ? 0 : pos_view[ring0][splus];
	Proj2424[0][1][0][3] = pos_min90[ring0][s];
	Proj2424[0][1][0][4] = splus>max_bin_num_to_use ? 0 : pos_min90[ring0][splus];
	Proj2424[0][2][0][1] = pos_plus90[ring0][s];
	Proj2424[0][2][0][2] = splus>max_bin_num_to_use ? 0 : pos_plus90[ring0][splus];
	Proj2424[0][3][0][3] = pos_min180[ring0][s];
	Proj2424[0][3][0][4] = splus>max_bin_num_to_use ? 0 : pos_min180[ring0][splus];
	Proj2424[1][0][0][3] = neg_view[ring0][s];
	Proj2424[1][0][0][4] = splus>max_bin_num_to_use ? 0 : neg_view[ring0][splus];
	Proj2424[1][1][0][1] = neg_min90[ring0][s];
	Proj2424[1][1][0][2] = splus>max_bin_num_to_use ? 0 : neg_min90[ring0][splus];
	Proj2424[1][2][0][3] = neg_plus90[ring0][s];
	Proj2424[1][2][0][4] = splus>max_bin_num_to_use ? 0 : neg_plus90[ring0][splus];
	Proj2424[1][3][0][1] = neg_min180[ring0][s];
	Proj2424[1][3][0][2] = splus>max_bin_num_to_use ? 0 : neg_min180[ring0][splus];

	Proj2424[0][0][1][3] = pos_view[ring0][ms];
	Proj2424[0][0][1][4] = splus>max_bin_num_to_use ? 0 : pos_view[ring0][msplus];
	Proj2424[0][1][1][1] = pos_min90[ring0][ms];
	Proj2424[0][1][1][2] = splus>max_bin_num_to_use ? 0 : pos_min90[ring0][msplus];
	Proj2424[0][2][1][3] = pos_plus90[ring0][ms];
	Proj2424[0][2][1][4] = splus>max_bin_num_to_use ? 0 : pos_plus90[ring0][msplus];
	Proj2424[0][3][1][1] = pos_min180[ring0][ms];
	Proj2424[0][3][1][2] = splus>max_bin_num_to_use ? 0 : pos_min180[ring0][msplus];
	Proj2424[1][0][1][1] = neg_view[ring0][ms];
	Proj2424[1][0][1][2] = splus>max_bin_num_to_use ? 0 : neg_view[ring0][msplus];
	Proj2424[1][1][1][3] = neg_min90[ring0][ms];
	Proj2424[1][1][1][4] = splus>max_bin_num_to_use ? 0 : neg_min90[ring0][msplus];
	Proj2424[1][2][1][1] = neg_plus90[ring0][ms];
	Proj2424[1][2][1][2] = splus>max_bin_num_to_use ? 0 : neg_plus90[ring0][msplus];
	Proj2424[1][3][1][3] = neg_min180[ring0][ms];
	Proj2424[1][3][1][4] = splus>max_bin_num_to_use ? 0 : neg_min180[ring0][msplus];
	}

	if (r0plus <= max_axial_pos_num)
	{
	Proj2424[0][0][0][3] = pos_view[r0plus][s];
	Proj2424[0][0][0][4] = splus>max_bin_num_to_use ? 0 : pos_view[r0plus][splus];
	Proj2424[0][1][0][1] = pos_min90[r0plus][s];
	Proj2424[0][1][0][2] = splus>max_bin_num_to_use ? 0 : pos_min90[r0plus][splus];
        Proj2424[0][2][0][3] = pos_plus90[r0plus][s];
	Proj2424[0][2][0][4] = splus>max_bin_num_to_use ? 0 : pos_plus90[r0plus][splus];
	Proj2424[0][3][0][1] = pos_min180[r0plus][s];
	Proj2424[0][3][0][2] = splus>max_bin_num_to_use ? 0 : pos_min180[r0plus][splus];
	Proj2424[1][0][0][1] = neg_view[r0plus][s];
	Proj2424[1][0][0][2] = splus>max_bin_num_to_use ? 0 : neg_view[r0plus][splus];
	Proj2424[1][1][0][3] = neg_min90[r0plus][s];
	Proj2424[1][1][0][4] = splus>max_bin_num_to_use ? 0 : neg_min90[r0plus][splus];
	Proj2424[1][2][0][1] = neg_plus90[r0plus][s];
	Proj2424[1][2][0][2] = splus>max_bin_num_to_use ? 0 : neg_plus90[r0plus][splus];
	Proj2424[1][3][0][3] = neg_min180[r0plus][s];
	Proj2424[1][3][0][4] = splus>max_bin_num_to_use ? 0 : neg_min180[r0plus][splus];

	Proj2424[0][0][1][1] = pos_view[r0plus][ms];
	Proj2424[0][0][1][2] = splus>max_bin_num_to_use ? 0 : pos_view[r0plus][msplus];
	Proj2424[0][1][1][3] = pos_min90[r0plus][ms];
	Proj2424[0][1][1][4] = splus>max_bin_num_to_use ? 0 : pos_min90[r0plus][msplus];
	Proj2424[0][2][1][1] = pos_plus90[r0plus][ms];
	Proj2424[0][2][1][2] = splus>max_bin_num_to_use ? 0 : pos_plus90[r0plus][msplus];
	Proj2424[0][3][1][3] = pos_min180[r0plus][ms];
	Proj2424[0][3][1][4] = splus>max_bin_num_to_use ? 0 : pos_min180[r0plus][msplus];
	Proj2424[1][0][1][3] = neg_view[r0plus][ms];
	Proj2424[1][0][1][4] = splus>max_bin_num_to_use ? 0 : neg_view[r0plus][msplus];
	Proj2424[1][1][1][1] = neg_min90[r0plus][ms];
	Proj2424[1][1][1][2] = splus>max_bin_num_to_use ? 0 : neg_min90[r0plus][msplus];
	Proj2424[1][2][1][3] = neg_plus90[r0plus][ms];
	Proj2424[1][2][1][4] = splus>max_bin_num_to_use ? 0 : neg_plus90[r0plus][msplus];
	Proj2424[1][3][1][1] = neg_min180[r0plus][ms];
	Proj2424[1][3][1][2] = splus>max_bin_num_to_use ? 0 : neg_min180[r0plus][msplus];
	}
	const int segment_num = pos_view.get_segment_num();

        const float delta=proj_data_info_cyl_ptr->get_average_ring_difference(segment_num);
	Proj2424 *= jacobian(delta, s+ 0.5);

	// find correspondence between ring coordinates and image coordinates:
	// z = num_planes_per_virtual_ring * ring + virtual_ring_offset
	// compute the offset by matching up the centre of the scanner 
	// in the 2 coordinate systems
	const int num_planes_per_virtual_ring =
           proj_data_info_cyl_ptr->get_axial_sampling(segment_num)/image.get_voxel_size().z();
	const int num_virtual_rings_per_physical_ring =
           proj_data_info_cyl_ptr->get_ring_spacing()/proj_data_info_cyl_ptr->get_axial_sampling(segment_num);
	const float virtual_ring_offset = 
	  (image.get_max_z() + image.get_min_z())/2.F
	  - num_planes_per_virtual_ring
	    *(pos_view.get_max_axial_pos_num()+ num_virtual_rings_per_physical_ring*delta 
	      + pos_view.get_min_axial_pos_num())/2;

        if (use_piecewise_linear_interpolation_now && num_planes_per_virtual_ring>1)
          piecewise_linear_interpolation_backproj3D_Cho_view_viewplus90_180minview_90minview
          (Proj2424,
          image,
          proj_data_info_cyl_ptr, 
          delta, 
          cphi, sphi, s, ring0, 
          num_planes_per_virtual_ring,
          virtual_ring_offset);
        else
          linear_interpolation_backproj3D_Cho_view_viewplus90_180minview_90minview
          (Proj2424,
          image,
          proj_data_info_cyl_ptr, 
          delta, 
          cphi, sphi, s, ring0, 
          num_planes_per_virtual_ring,
          virtual_ring_offset);
      }
    }
  stop_timers();
}

/*
This function projects 4 viewgrams related by symmetry.
It will be used for view=0 or 45 degrees 
(or all others if the above version is not implemented in 
the derived class)
Here 0<=view < num_views/2 (= 90 degrees)
*/

void 
BackProjectorByBinUsingInterpolation::back_project_view_plus_90_and_delta(
				         VoxelsOnCartesianGrid<float>& image,
					 const Viewgram<float> & pos_view, 
					 const Viewgram<float> & neg_view, 
					 const Viewgram<float> & pos_plus90, 
					 const Viewgram<float> & neg_plus90,
					 const int min_axial_pos_num, const int max_axial_pos_num,
				         const int min_tangential_pos_num, const int max_tangential_pos_num)				   
{
 const ProjDataInfoCylindricalArcCorr* proj_data_info_cyl_ptr = 
    dynamic_cast<const ProjDataInfoCylindricalArcCorr*> (pos_view.get_proj_data_info_ptr());

  if ( proj_data_info_cyl_ptr==NULL)
  {
    error("\nBackProjectorByBinUsingInterpolation,\n\
     Casting failed!");
  }

  assert(min_axial_pos_num >= pos_view. get_min_axial_pos_num());
  assert(max_axial_pos_num <= pos_view. get_max_axial_pos_num());
  assert(min_tangential_pos_num >= pos_view.get_min_tangential_pos_num());
  assert(max_tangential_pos_num <= pos_view.get_max_tangential_pos_num());

  assert(min_tangential_pos_num == - max_tangential_pos_num);
  const int segment_num = pos_view.get_segment_num();

  assert(proj_data_info_cyl_ptr ->get_average_ring_difference(segment_num) >= 0);

  assert(pos_view.get_view_num()>=0);
  assert(pos_view.get_view_num() < pos_view.get_proj_data_info_ptr()->get_num_views()/2);

 const int nviews = pos_view.get_proj_data_info_ptr()->get_num_views();

  assert(pos_plus90.get_view_num() == nviews / 2 + pos_view.get_view_num());
  assert(neg_view.get_view_num() == pos_view.get_view_num());
  assert(neg_plus90.get_view_num() == pos_plus90.get_view_num());


  assert(image.get_min_x() == -image.get_max_x());
  assert(image.get_voxel_size().x() == proj_data_info_cyl_ptr ->get_tangential_sampling());
  // TODO rewrite code a bit to remove this assertion
  assert(image.get_min_z() == 0);
  // TODO remove -1, it's there because otherwise a crazy value is used. Don't know why yet...
  const int max_bin_num_to_use =
    std::min(max_tangential_pos_num, image.get_max_x()-1);

  start_timers();

  const JacobianForIntBP jacobian(proj_data_info_cyl_ptr, use_exact_Jacobian_now);

  Array<4, float > Proj2424(IndexRange4D(0, 1, 0, 3, 0, 1, 1, 4));

  const float cphi = cos(pos_view.get_view_num() * _PI / nviews);
  const float sphi = sin(pos_view.get_view_num() * _PI / nviews);
 

  // Do a loop over all rings. However, because we use interpolation of
  // a 'beam', each step takes elements from ring0 and ring0+1. So, data in
  // a ring influences beam ring0-1 and ring0. All this means that we
  // have to let ring0 run from min_ring_num-1 to max_ring_num.
  for (int ring0 = min_axial_pos_num-1; ring0 <= max_axial_pos_num; ring0++)
    {
      const int r0plus = ring0 + 1; 
        
      // We have to fill with 0, as not all elements are set in the lines below
      if (ring0==min_axial_pos_num-1 || ring0==max_axial_pos_num)
	Proj2424.fill(0);
      for (int s = 0; s <= max_bin_num_to_use; s++) {
	const int splus = s + 1;
	const int ms = -s;
	const int msplus = -splus;

	// now I have to check if ring0 is in allowable range
	if (ring0 >= min_axial_pos_num)
	{
	  Proj2424[0][0][0][1] = pos_view[ring0][s];
	Proj2424[0][0][0][2] = splus>max_bin_num_to_use ? 0 : pos_view[ring0][splus];
	Proj2424[0][2][0][1] = pos_plus90[ring0][s];
	Proj2424[0][2][0][2] = splus>max_bin_num_to_use ? 0 : pos_plus90[ring0][splus];
	Proj2424[1][0][0][3] = neg_view[ring0][s];
	Proj2424[1][0][0][4] = splus>max_bin_num_to_use ? 0 : neg_view[ring0][splus];
	Proj2424[1][2][0][3] = neg_plus90[ring0][s];
	Proj2424[1][2][0][4] = splus>max_bin_num_to_use ? 0 : neg_plus90[ring0][splus];

	Proj2424[0][0][1][3] = pos_view[ring0][ms];
	Proj2424[0][0][1][4] = splus>max_bin_num_to_use ? 0 : pos_view[ring0][msplus];
	Proj2424[0][2][1][3] = pos_plus90[ring0][ms];
	Proj2424[0][2][1][4] = splus>max_bin_num_to_use ? 0 : pos_plus90[ring0][msplus];
	Proj2424[1][0][1][1] = neg_view[ring0][ms];
	Proj2424[1][0][1][2] = splus>max_bin_num_to_use ? 0 : neg_view[ring0][msplus];
	Proj2424[1][2][1][1] = neg_plus90[ring0][ms];
	Proj2424[1][2][1][2] = splus>max_bin_num_to_use ? 0 : neg_plus90[ring0][msplus];
	}

        if (r0plus <= max_axial_pos_num)
	{
	Proj2424[0][0][0][3] = pos_view[r0plus][s];
	Proj2424[0][0][0][4] = splus>max_bin_num_to_use ? 0 : pos_view[r0plus][splus];
	Proj2424[0][2][0][3] = pos_plus90[r0plus][s];
	Proj2424[0][2][0][4] = splus>max_bin_num_to_use ? 0 : pos_plus90[r0plus][splus];
	Proj2424[1][0][0][1] = neg_view[r0plus][s];
	Proj2424[1][0][0][2] = splus>max_bin_num_to_use ? 0 : neg_view[r0plus][splus];
	Proj2424[1][2][0][1] = neg_plus90[r0plus][s];
	Proj2424[1][2][0][2] = splus>max_bin_num_to_use ? 0 : neg_plus90[r0plus][splus];

	Proj2424[0][0][1][1] = pos_view[r0plus][ms];
	Proj2424[0][0][1][2] = splus>max_bin_num_to_use ? 0 : pos_view[r0plus][msplus];
	Proj2424[0][2][1][1] = pos_plus90[r0plus][ms];
	Proj2424[0][2][1][2] = splus>max_bin_num_to_use ? 0 : pos_plus90[r0plus][msplus];
	Proj2424[1][0][1][3] = neg_view[r0plus][ms];
	Proj2424[1][0][1][4] = splus>max_bin_num_to_use ? 0 : neg_view[r0plus][msplus];
	Proj2424[1][2][1][3] = neg_plus90[r0plus][ms];
	Proj2424[1][2][1][4] = splus>max_bin_num_to_use ? 0 : neg_plus90[r0plus][msplus];
	}

	const int segment_num = pos_view.get_segment_num();
	Proj2424 *= jacobian(proj_data_info_cyl_ptr->get_average_ring_difference(segment_num), s+ 0.5);
        
	
	const float delta=proj_data_info_cyl_ptr->get_average_ring_difference(segment_num);

	// find correspondence between ring coordinates and image coordinates:
	// z = num_planes_per_virtual_ring * ring + virtual_ring_offset
	// compute the offset by matching up the centre of the scanner 
	// in the 2 coordinate systems
	const int num_planes_per_virtual_ring =
          proj_data_info_cyl_ptr->get_axial_sampling(segment_num)/image.get_voxel_size().z();
        const int num_virtual_rings_per_physical_ring =
          proj_data_info_cyl_ptr->get_ring_spacing()/proj_data_info_cyl_ptr->get_axial_sampling(segment_num);
        
        const float virtual_ring_offset = 
          (image.get_max_z() + image.get_min_z())/2.F
          - num_planes_per_virtual_ring
          *(pos_view.get_max_axial_pos_num()+ num_virtual_rings_per_physical_ring*delta 
          + pos_view.get_min_axial_pos_num())/2;

        if (use_piecewise_linear_interpolation_now && num_planes_per_virtual_ring>1)
          piecewise_linear_interpolation_backproj3D_Cho_view_viewplus90( Proj2424, image, 
				       proj_data_info_cyl_ptr, 
				       delta, 
				       cphi, sphi, s, ring0, 
				       num_planes_per_virtual_ring,
				       virtual_ring_offset);
        else
          linear_interpolation_backproj3D_Cho_view_viewplus90( Proj2424, image, 
				       proj_data_info_cyl_ptr, 
				       delta, 
				       cphi, sphi, s, ring0, 
				       num_planes_per_virtual_ring,
				       virtual_ring_offset);
      }
    }
  stop_timers();
}


END_NAMESPACE_TOMO
