//
// $Id$
//
/*!

  \file
  \ingroup listmode
  \brief Class for rebinning listmode files with motion correction
    
  \author Sanida Mustafovic
  \author Kris Thielemans
      
  $Date$
  $Revision$
*/
/*
    Copyright (C) 2003- $Date$, IRSL
    See STIR/LICENSE.txt for details
*/

#ifndef __stir_listmode_LmToProjDataWithMC_H__
#define __stir_listmode_LmToProjDataWithMC_H__


#include "local/stir/listmode/LmToProjData.h"
#include "stir/CartesianCoordinate3D.h"

#include "local/stir/motion/Polaris_MT_File.h"
#include "local/stir/motion/RigidObject3DMotionFromPolaris.h"

START_NAMESPACE_STIR

class LmToProjDataWithMC : public LmToProjData
{
public:
     
  LmToProjDataWithMC(const char * const par_filename);

  virtual void get_bin_from_event(Bin& bin, const CListEvent&) const;


//private:

  void find_ref_pos_from_att_file (float& att_start_time, float& att_end_time, 
	const float transmission_duration, const string attnuation_filename);
  /*
  void find_cartesian_coordinates_given_scanner_coordinates (CartesianCoordinate3D<float>& coord_1,
				 CartesianCoordinate3D<float>& coord_2,
				 const int Ring_A,const int Ring_B, 
				 const int det1, const int det2, 
				 const Scanner& scanner) const;


  Succeeded find_scanner_coordinates_given_cartesian_coordinates(int& det1, int& det2, int& ring1, int& ring2,
							  const CartesianCoordinate3D<float>& coord_1_in,
							  const CartesianCoordinate3D<float>& coord_2_in, 
							  const Scanner& scanner) const;*/

  void transform_detector_pair_into_view_bin (int& view,int& bin, 
					    const int det1,const int det2, 
					    const Scanner& scanner) const;

  shared_ptr<RigidObject3DMotion> ro3d_ptr;
  string attenuation_filename; 
  float transmission_duration;
  float polaris_time_offset;
   
  virtual void set_defaults();
  virtual void initialise_keymap();
  virtual bool post_processing();

  mutable RigidObject3DTransformation ro3d_move_to_reference_position;
  RigidObject3DTransformation move_to_scanner;
  RigidObject3DTransformation move_from_scanner;

  
};

END_NAMESPACE_STIR


#endif
