/*  $Id$
    Copyright (C) 2006- $Date$, Hammersmith Imanet Ltd
    This file is part of STIR.
  
    This file is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.
    
    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
  
    See STIR/LICENSE.txt for details
*/
/*! 
 
\file
\ingroup test
 
\brief testing ParametricDiscretisedDensity class
 
\author T. Borgeaud
\author Charalampos Tsoumpas 
 
$Date$
 
$Revision$ 
 
*/

#include <iostream>
#include "stir/RunTests.h"
#include "stir/IndexRange2D.h"
#include "stir/ProjDataInfoCylindricalNoArcCorr.h"
#include "stir/stream.h"
#include "stir/Succeeded.h"
#include <fstream>
#include "stir/VoxelsOnCartesianGrid.h"
#include "stir/DiscretisedDensity.h"
#include "stir/shared_ptr.h"
#include "stir/TimeFrameDefinitions.h"
#include "stir/Scanner.h"
#include <utility>
#include <vector>
#include <string>

#include <algorithm>
#include <iomanip>
//-----------
#include "local/stir/modelling/ParametricDiscretisedDensity.h"
#include "stir/is_null_ptr.h"
#include "stir/IO/OutputFileFormat.h"

#ifndef STIR_NO_NAMESPACES
using std::cerr;
using std::ifstream;
using std::istream;
using std::setw;
#endif

START_NAMESPACE_STIR

class ParametricDiscretisedDensityTests : public RunTests
{
public:
  ParametricDiscretisedDensityTests() 
  {}
  void run_tests();
  //private:
};

void ParametricDiscretisedDensityTests::run_tests()
{
  set_tolerance(0.000000000000001);

  //! Setup the scanner details first
  const Scanner::Type test_scanner=Scanner::E966;
  const shared_ptr<Scanner> scanner_sptr = new Scanner(test_scanner);
  VectorWithOffset<int> num_axial_pos_per_segment;  num_axial_pos_per_segment.resize(0,0);  num_axial_pos_per_segment[0]=48;
  VectorWithOffset<int> min_ring_diff;  min_ring_diff.resize(0,0);  min_ring_diff[0]=0;
  VectorWithOffset<int> max_ring_diff; max_ring_diff.resize(0,0);  max_ring_diff[0]=0;
  const int num_views=144; const int num_tangential_poss=144; 
  ProjDataInfoCylindricalNoArcCorr proj_data_info(scanner_sptr,num_axial_pos_per_segment,min_ring_diff,max_ring_diff,num_views,num_tangential_poss);
  //! Setup some of the image details
  const CartesianCoordinate3D< float > origin (0.F,0.F,0.F);
  const CartesianCoordinate3D<float> grid_spacing (1.F,1.F,1.F);
  const float zoom=1.F;
  {
    cerr << "Testing ParametricDiscretisedDensity class for one voxel..." << endl;
    const CartesianCoordinate3D<int> sizes (1,1,1);
    
    const shared_ptr<ParametricVoxelsOnCartesianGrid> parametric_image_sptr = new 
      ParametricVoxelsOnCartesianGrid(
				      ParametricVoxelsOnCartesianGridBaseType(proj_data_info,
									      zoom,grid_spacing,sizes));
    ParametricVoxelsOnCartesianGrid & parametric_image = *parametric_image_sptr;
    parametric_image[0][0][0][1]=1.F;  parametric_image[0][0][0][2]=2.F;

    check_if_equal(parametric_image[0][0][0][1],1.F,"check ParametricVoxelsOnCartesianGrid class implementation");
    check_if_equal(parametric_image[0][0][0][2],2.F,"check ParametricVoxelsOnCartesianGrid class implementation");
  }

  {
    //  Test of two frame images, read voxel  
    cerr << "Writing ParametricDiscretisedDensity class for more voxels: 63x128x128..." << endl;

    const CartesianCoordinate3D<int> sizes (63,128,128);
  
    const shared_ptr<ParametricVoxelsOnCartesianGrid> parametric_image_sptr = 
      new ParametricVoxelsOnCartesianGrid(ParametricVoxelsOnCartesianGridBaseType(proj_data_info,zoom,grid_spacing,sizes));
    ParametricVoxelsOnCartesianGrid & parametric_image = *parametric_image_sptr;

    for(int k=0;k<63;++k)
      for(int j=-64;j<63;++j)  
	for(int i=-64;i<63;++i)
	  for(unsigned int par_num=1; par_num<=2; ++par_num)
	    parametric_image[k][j][i][par_num] = static_cast<float> (par_num*(i*1.F+j*5.F-k*10.F));

    cerr << "- Checking the [] operator. " << endl;
    for(int k=0;k<63;++k)
      for(int j=-64;j<63;++j)  
	for(int i=-64;i<63;++i)
	    for(unsigned int par_num=1; par_num<=2; ++par_num)
	      check_if_equal(parametric_image[k][j][i][par_num],static_cast<float> (par_num*(i*1.F+j*5.F-k*10.F)),
			     "Please, check the [] operator implementation");   


    cerr << "- Checking construct_single_density(param_num) implementation." << endl;
    ParametricVoxelsOnCartesianGrid::SingleDiscretisedDensityType single_density_1 = parametric_image_sptr->construct_single_density(1);
    ParametricVoxelsOnCartesianGrid::SingleDiscretisedDensityType single_density_2 = parametric_image_sptr->construct_single_density(2);

    for(int k=0;k<63;++k)
      for(int j=-64;j<63;++j)  
	for(int i=-64;i<63;++i)
	  {
	    check_if_equal(parametric_image[k][j][i][1],single_density_1[k][j][i],
			   "Please, check construct_single_density(param_num) implementation");   
	    check_if_equal(parametric_image[k][j][i][2],single_density_2[k][j][i],
			   "Please, check construct_single_density(param_num) implementation");   
	    for(unsigned int par_num=1; par_num<=2; ++par_num)
	      check_if_equal(parametric_image[k][j][i][par_num],static_cast<float> (par_num*(i*1.F+j*5.F-k*10.F)),
			     "Please, check the construct_single_density(param_num) implementation");   
	  }

    ParametricVoxelsOnCartesianGrid::SingleDiscretisedDensityType::full_iterator cur_iter_1 =
      single_density_1.begin_all();
    ParametricVoxelsOnCartesianGrid::SingleDiscretisedDensityType::full_iterator cur_iter_2 =
      single_density_2.begin_all();
    for (; cur_iter_1!=single_density_1.end_all() &&
	   cur_iter_2!=single_density_2.end_all();	  
	 ++cur_iter_1, ++cur_iter_2)			  
      check_if_equal(*cur_iter_1*2.F, *cur_iter_2,
		     "Please, check construct_single_density(param_num) implementation");   

    cerr << "- Checking create_parametric_image(VectorWithOffset single_denisties) implementation." << endl;
    std::fill(single_density_1.begin_all(), single_density_1.end_all(), 1.F);
    std::fill(single_density_2.begin_all(), single_density_2.end_all(), 2.F);
    VectorWithOffset<shared_ptr<ParametricVoxelsOnCartesianGrid::SingleDiscretisedDensityType> > v_test;
    v_test.resize(1,2); v_test[1]=single_density_1.clone(); v_test[2]=single_density_2.clone();
    ParametricVoxelsOnCartesianGrid test_create_par = parametric_image_sptr->create_parametric_image(v_test);

    for(int k=0;k<63;++k)
      for(int j=-64;j<63;++j)  
	for(int i=-64;i<63;++i)
	  for(unsigned int par_num=1;par_num<=2;++par_num)
	    {
	      check_if_equal(test_create_par[k][j][i][par_num],static_cast<float> (par_num),
			     "Please, check create_parametric_image(VectorWithOffset single_denisties) implementation");   
	      check_if_equal(parametric_image[k][j][i][par_num],static_cast<double> (par_num*(i*1.F+j*5.F-k*10.F)),
			     "Please, check the create_parametric_image(VectorWithOffset single_denisties)");   
	    }
    cerr << "- Checking update_parametric_image(VectorWithOffset) implementation." << endl;
    ParametricVoxelsOnCartesianGrid test_update_par = parametric_image_sptr->update_parametric_image(v_test);
    for(int k=0;k<63;++k)
      for(int j=-64;j<63;++j)  
	for(int i=-64;i<63;++i)
	  for(unsigned int par_num=1;par_num<=2;++par_num)
	    check_if_equal(parametric_image[k][j][i][par_num],static_cast<float> (par_num),
			   "Please, check update_parametric_image(VectorWithOffset single_densities) implementation");   

    cerr << "- Checking update_parametric_image(single_density,param_num) implementation." << endl;

    ParametricVoxelsOnCartesianGrid test_update_1 = parametric_image_sptr->update_parametric_image(single_density_1.clone(),2);
    ParametricVoxelsOnCartesianGrid test_update_2 = parametric_image_sptr->update_parametric_image(single_density_2.clone(),1);

    // Check if the result has been reversed.
    for(int k=0;k<63;++k)
      for(int j=-64;j<63;++j)  
	for(int i=-64;i<63;++i)
	  for(unsigned int par_num=1;par_num<=2;++par_num)
	    check_if_equal(parametric_image[k][j][i][par_num],static_cast<float> (2+1-par_num),
			   "Please, check update_parametric_image(VectorWithOffset) implementation");   

  }
}

END_NAMESPACE_STIR
USING_NAMESPACE_STIR

int main(int argc, char **argv)
{
  if (argc != 1)
    {
      cerr << "Usage : " << argv[0] << " \n";
      return EXIT_FAILURE;
    }
  ParametricDiscretisedDensityTests tests;
  tests.run_tests();
  return tests.main_return_value();
}