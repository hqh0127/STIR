//
// $Id: CListModeDataECAT.cxx,v 1.25 2012-01-09 09:04:55 kris Exp $
//
/*
    Copyright (C) 2003-2012 Hammersmith Imanet Ltd
    Copyright (C) 2013 University College London

*/
/*!
  \file
  \ingroup listmode  
  \brief Implementation of class stir::UCL::CListModeDataECAT8_32bit

  \author Kris Thielemans
      
  $Date: 2012-01-09 09:04:55 $
  $Revision: 1.25 $
*/


#include "UCL/listmode/CListModeDataECAT8_32bit.h"
#include "UCL/listmode/CListRecordECAT8_32bit.h"
#include "stir/Succeeded.h"
#include "stir/is_null_ptr.h"
#include "boost/static_assert.hpp"
#include <iostream>
#include <fstream>
#include <typeinfo>
#ifndef STIR_NO_NAMESPACES
using std::cerr;
using std::endl;
using std::ios;
using std::fstream;
using std::ifstream;
#endif

START_NAMESPACE_STIR
namespace UCL {


CListModeDataECAT8_32bit::
CListModeDataECAT8_32bit(const std::string& listmode_filename)
  : listmode_filename(listmode_filename)
{
  // initialise scanner_ptr before calling open_lm_file, as it is used in that function
  this->interfile_parser.add_key("%axial_compression", &this->axial_compression);
  this->interfile_parser.add_key("%maximum_ring_difference", &this->maximum_ring_difference);
  this->interfile_parser.add_key("%number_of_projections", &this->number_of_projections);
  this->interfile_parser.add_key("%number_of_views", &this->number_of_views);
  this->interfile_parser.add_key("%number_of_segments", &this->number_of_segments);
  // TODO cannot do this yet
  //this->interfile_parser.add_key("segment_table", &this->segment_table);
  
  // We need to set num_time_frames to 1 as the Siemens header doesn't have the keyword
  {
    const int num_time_frames=1;
    this->interfile_parser.num_time_frames=1;
    this->interfile_parser.image_scaling_factors.resize(num_time_frames);
    for (int i=0; i<num_time_frames; i++)
      this->interfile_parser.image_scaling_factors[i].resize(1, 1.);
    this->interfile_parser.data_offset.resize(num_time_frames, 0UL);
    this->interfile_parser.image_relative_start_times.resize(num_time_frames, 0.);
    this->interfile_parser.image_durations.resize(num_time_frames, 0.);
  }

  this->interfile_parser.parse(listmode_filename.c_str());
  if (this->interfile_parser.originating_system == "2008")
    this->scanner_sptr.reset(new Scanner(Scanner::Siemens_mMR));
  else
    error("Unknown originating_system");

  this->proj_data_info_sptr.reset(ProjDataInfo::ProjDataInfoCTI(this->scanner_sptr, 
								this->axial_compression,
								this->maximum_ring_difference,
								this->number_of_views,
								this->number_of_projections,
								/* arc_correction*/false));

  if (this->open_lm_file() == Succeeded::no)
    error("CListModeDataECAT8_32bit: error opening the first listmode file for filename %s\n",
	  listmode_filename.c_str());
}

std::string
CListModeDataECAT8_32bit::
get_name() const
{
  return listmode_filename;
}

std::time_t 
CListModeDataECAT8_32bit::
get_scan_start_time_in_secs_since_1970() const
{
  // TODO
  return std::time_t(-1);
}



shared_ptr <CListRecord> 
CListModeDataECAT8_32bit::
get_empty_record_sptr() const
{
  shared_ptr<CListRecord> sptr(new CListRecordT(this->proj_data_info_sptr));
  return sptr;
}


Succeeded
CListModeDataECAT8_32bit::
open_lm_file()
{
    {

      // now open new file
      std::string filename = interfile_parser.data_file_name;
      cerr << "CListModeDataECAT8_32bit: opening file " << filename << endl;
      shared_ptr<istream> stream_ptr(new fstream(filename.c_str(), ios::in | ios::binary));
      if (!(*stream_ptr))
      {
	warning("CListModeDataECAT8_32bit: cannot open file '%s'", filename.c_str());
        return Succeeded::no;
      }
      current_lm_data_ptr.reset(
				new InputStreamWithRecords<CListRecordT, bool>(stream_ptr,  4, 4,
                                                       ByteOrder::little_endian != ByteOrder::get_native_order()));

      return Succeeded::yes;
    }
}



Succeeded
CListModeDataECAT8_32bit::
get_next_record(CListRecord& record_of_general_type) const
{
  CListRecordT& record = static_cast<CListRecordT&>(record_of_general_type);
  return current_lm_data_ptr->get_next_record(record);
 }




Succeeded
CListModeDataECAT8_32bit::
reset()
{  
  return current_lm_data_ptr->reset();
}



CListModeData::SavedPosition
CListModeDataECAT8_32bit::
save_get_position() 
{
  return current_lm_data_ptr->save_get_position();
} 


Succeeded
CListModeDataECAT8_32bit::
set_get_position(const typename CListModeDataECAT8_32bit::SavedPosition& pos)
{
  return
    current_lm_data_ptr->set_get_position(pos);
}


} // namespace UCL 

END_NAMESPACE_STIR
