!INTERFILE  :=
name of data file := ECAT_931_projdata_template.s
!imaging modality := PT
originating system := ECAT 931
!GENERAL DATA :=
!GENERAL IMAGE DATA :=
!type of data := PET
imagedata byte order := LITTLEENDIAN
!PET STUDY (General) :=
!PET data type := Emission
;applied corrections := {arc correction}
!number format := float
!number of bytes per pixel := 4
number of dimensions := 4
matrix axis label [4] := segment
!matrix size [4] := 15
matrix axis label [3] := view
!matrix size [3] := 64
matrix axis label [2] := axial coordinate
!matrix size [2] := { 1,2,3,4,5,6,7,8,7,6,5,4,3,2,1}
matrix axis label [1] := tangential coordinate
!matrix size [1] := 48
minimum ring difference per segment := { -7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7}
maximum ring difference per segment := { -7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7}
Scanner parameters:= 
Scanner type := ECAT 931
Number of rings                          := 8
Number of detectors per ring             := 512
Inner ring diameter (cm)                 := 102
Average depth of interaction (cm)        := 0.7
Distance between rings (cm)              := 1.35
Default bin size (cm)                    := 0.3129
View offset (degrees)                    := 0
Maximum number of non-arc-corrected bins := 192
Default number of arc-corrected bins     := 192
Number of blocks per bucket in transaxial direction         := 4
Number of blocks per bucket in axial direction              := 2
Number of crystals per block in axial direction             := 4
Number of crystals per block in transaxial direction        := 8
Number of detector layers                                   := 1
Number of crystals per singles unit in axial direction      := 4
Number of crystals per singles unit in transaxial direction := 32
end scanner parameters:=
effective central bin size (cm) := 0.3129
image scaling factor[1] := 1
data offset in bytes[1] := 0
number of time frames := 1
!END OF INTERFILE :=
