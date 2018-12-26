/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCTIFFImageReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

	 This software is distributed WITHOUT ANY WARRANTY; without even
	 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
	 PURPOSE.  See the above copyright notice for more information.

  Changed by Hyun-Jin Won, Pemtron.Corp
  2018-12-26
=========================================================================*/
/**
 * @class   vtkCTIFFImageReader
 * @brief   Reads some DICOM images
 *
 * DICOM (stands for Digital Imaging in COmmunications and Medicine)
 * is a medical image file format widely used to exchange data, provided
 * by various modalities.
 * @warning
 * This reader might eventually handle ACR-NEMA file (predecessor of the DICOM
 * format for medical images).
 * This reader does not handle encapsulated format, only plain raw file are
 * handled. This reader also does not handle multi-frames DICOM datasets.
 * @warning
 * Internally DICOMParser assumes the x,y pixel spacing is stored in 0028,0030 and
 * that z spacing is stored in Slice Thickness (correct only when slice were acquired
 * contiguous): 0018,0050. Which means this is only valid for some rare
 * MR Image Storage
 *
 * @sa
 * vtkBMPReader vtkPNMReader vtkTIFFReader
*/

#ifndef vtkCTIFFImageReader_h
#define vtkCTIFFImageReader_h

#include "vtkIOImageModule.h" // For export macro
#include <vtkTIFFReader.h>

class vtkCTIFFImageReaderVector;

class vtkCTIFFImageReader : public vtkTIFFReader
{

private:
	char* DirectoryName;
	vtkCTIFFImageReaderVector * TIFFFileNames;

public:
static vtkCTIFFImageReader *New();
vtkTypeMacro(vtkCTIFFImageReader, vtkTIFFReader)
void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
	vtkCTIFFImageReader();
	~vtkCTIFFImageReader() override;

public:
	void SetDirectoryName(const char * dn);
	vtkGetStringMacro(DirectoryName);

	void vtkCTIFFImageReader::ExecuteInformation() override;
	void vtkCTIFFImageReader::ExecuteDataWithInformation(vtkDataObject *output, vtkInformation *outInfo) override;


private:
	vtkCTIFFImageReader(const vtkCTIFFImageReader&) = delete;
	void operator=(const vtkCTIFFImageReader&) = delete;
};



#endif
