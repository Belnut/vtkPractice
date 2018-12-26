///*=========================================================================
//
//  Program:   Visualization Toolkit
//  Module:    vtkCTIFFImageReader.cxx
//
//  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
//  All rights reserved.
//  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
//
//	 This software is distributed WITHOUT ANY WARRANTY; without even
//	 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//	 PURPOSE.  See the above copyright notice for more information.
//
//
//  Changed by Hyun-Jin Won, Pemtron.Corp
//  2018-12-26
//=========================================================================*/
#include"stdafx.h"
#include "vtkCTIFFImageReader.h"

#include <vtkDirectory.h>
#include <vtkDataArray.h>
#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkErrorCode.h>

#include <vtksys/SystemTools.hxx>

#include <vector>
#include <string>

#include <DICOMAppHelper.h>
#include <DICOMParser.h>
#include <vtktiff/tiffio.h>

#pragma warning(disable:4996)
vtkStandardNewMacro(vtkCTIFFImageReader);

class vtkCTIFFImageReaderVector : public std::vector<std::string>
{

};


void vtkCTIFFImageReader::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
vtkCTIFFImageReader::vtkCTIFFImageReader() : vtkTIFFReader()
{
	DirectoryName = nullptr;
	this->TIFFFileNames = new vtkCTIFFImageReaderVector();
}

vtkCTIFFImageReader::~vtkCTIFFImageReader()
{
}

void vtkCTIFFImageReader::SetDirectoryName(const char* dn)
{
	vtkDebugMacro(<< this->GetClassName() << " (" << this <<
		"): setting DirectoryName to " << (dn ? dn : "(null)"));
	if (this->DirectoryName == nullptr && dn == nullptr)
	{
		return;
	}
	delete[] this->FileName;
	this->FileName = nullptr;
	if (this->DirectoryName && dn && (!strcmp(this->DirectoryName, dn)))
	{
		return;
	}
	delete[] this->DirectoryName;
	if (dn)
	{
		this->DirectoryName = new char[strlen(dn) + 1];
		strcpy(this->DirectoryName, dn);
	}
	else
	{
		this->DirectoryName = nullptr;
	}
	//this->Modified();
}

void vtkCTIFFImageReader::ExecuteInformation()
{
	if (this->DirectoryName == nullptr)
	{
	 	this->vtkTIFFReader::ExecuteInformation();
	}
	else if(this->DirectoryName)
	{
		vtkDirectory* dir = vtkDirectory::New();
		int opened = dir->Open(this->DirectoryName);
		if (!opened)
		{
			vtkErrorMacro("Couldn't open " << this->DirectoryName);
			dir->Delete();
			return;
		}
		vtkIdType numFiles = dir->GetNumberOfFiles();

		vtkDebugMacro(<< "There are " << numFiles << " files in the directory.");

		this->TIFFFileNames->clear();
		//this->AppHelper->Clear();

		for (vtkIdType i = 0; i < numFiles; i++)
		{
			if (strcmp(dir->GetFile(i), ".") == 0 ||
				strcmp(dir->GetFile(i), "..") == 0)
			{
				continue;
			}

			std::string fileString = this->DirectoryName;
			fileString += "/";
			fileString += dir->GetFile(i);

			int val = this->CanReadFile(fileString.c_str());

			//open 확인시 TURE = 3 FALSE = 0 리턴;
			if (val == 3)
			{
				vtkDebugMacro(<< "Adding " << fileString.c_str() << " to DICOMFileNames.");
				this->TIFFFileNames->push_back(fileString);
			}
			else
			{
				vtkDebugMacro(<< fileString.c_str() << " - DICOMParser CanReadFile returned : " << val);
			}

		}
		std::vector<std::string>::iterator iter;

		//for (iter = this->TIFFFileNames->begin();
		//	iter != this->TIFFFileNames->end();
		//	++iter)
		//{
		//	const char* fn = iter->c_str();
		//	vtkDebugMacro(<< "Trying : " << fn);

		//	//bool couldOpen = this->Parser->OpenFile(fn);
		//	if (!couldOpen)
		//	{
		//		dir->Delete();
		//		return;
		//	}

		//	//
		//	this->Parser->ClearAllDICOMTagCallbacks();
		//	this->AppHelper->RegisterCallbacks(this->Parser);

		//	this->Parser->ReadHeader();
		//	this->Parser->CloseFile();

		//	vtkDebugMacro(<< "File name : " << fn);
		//	vtkDebugMacro(<< "Slice number : " << this->AppHelper->GetSliceNumber());
		//}

		std::vector<std::pair<float, std::string> > sortedFiles;

		//this->AppHelper->GetImagePositionPatientFilenamePairs(sortedFiles, false);
		//this->SetupOutputInformation(static_cast<int>(sortedFiles.size()));

		//this->AppHelper->OutputSeries();

		//정렬 음.... 필요할까 모르겠음 확인나중에 해보자
		//if (!sortedFiles.empty())
		//{
		//	this->TIFFFileNames->clear();
		//	std::vector<std::pair<float, std::string> >::iterator siter;
		//	for (siter = sortedFiles.begin();
		//		siter != sortedFiles.end();
		//		++siter)
		//	{
		//		vtkDebugMacro(<< "Sorted filename : " << (*siter).second.c_str());
		//		vtkDebugMacro(<< "Adding file " << (*siter).second.c_str() << " at slice : " << (*siter).first);
		//		this->TIFFFileNames->push_back((*siter).second);
		//	}
		//}
		
		/////////////////////////////////////////////////////////////////////////////////////////
		//output Setting

		TIFF * image;

		//this->Initialize();
		//this->ComputeInternalFileName(this->DataExtent[4]);
		//if (this->InternalFileName == nullptr)
		//{
		//	vtkErrorMacro("Need to specify a filename");
		//	this->SetErrorCode(vtkErrorCode::NoFileNameError);
		//	return;
		//}

		//if (!this->InternalImage->Open(this->InternalFileName))
		//{
		//	vtkErrorMacro("Unable to open file "
		//		<< this->InternalFileName
		//		<< " Reason: "
		//		<< vtksys::SystemTools::GetLastSystemError());
		//	this->SetErrorCode(vtkErrorCode::CannotOpenFileError);
		//	this->DataExtent[0] = 0;
		//	this->DataExtent[1] = 0;
		//	this->DataExtent[2] = 0;
		//	this->DataExtent[3] = 0;
		//	this->DataExtent[4] = 0;
		//	this->DataExtent[5] = 0;
		//	this->SetNumberOfScalarComponents(1);
		//	this->vtkImageReader2::ExecuteInformation();
		//	return;
		//}

		//// If orientation information is provided, overwrite the value
		//// read from the tiff image
		//if (this->OrientationTypeSpecifiedFlag)
		//{
		//	this->InternalImage->Orientation = OrientationType;
		//}

		//if (!SpacingSpecifiedFlag)
		//{
		//	this->DataSpacing[0] = 1.0;
		//	this->DataSpacing[1] = 1.0;

		//	// If we have some spacing information we use it
		//	if (this->InternalImage->ResolutionUnit > 0 &&
		//		this->InternalImage->XResolution > 0 &&
		//		this->InternalImage->YResolution > 0)
		//	{
		//		if (this->InternalImage->ResolutionUnit == 2) // inches
		//		{
		//			this->DataSpacing[0] = 25.4 / this->InternalImage->XResolution;
		//			this->DataSpacing[1] = 25.4 / this->InternalImage->YResolution;
		//		}
		//		else if (this->InternalImage->ResolutionUnit == 3) // cm
		//		{
		//			this->DataSpacing[0] = 10.0 / this->InternalImage->XResolution;
		//			this->DataSpacing[1] = 10.0 / this->InternalImage->YResolution;
		//		}
		//		// Somewhat arbitrary choice of using the X spacing as also the
		//		// Z spacing. Used only with image stacks.
		//		this->DataSpacing[2] = this->DataSpacing[0];
		//	}
		//}

		//if (!OriginSpecifiedFlag)
		//{
		//	this->DataOrigin[0] = 0.0;
		//	this->DataOrigin[1] = 0.0;
		//	this->DataOrigin[2] = 0.0;
		//}

		//// Pull out the width/height, etc.
		//this->DataExtent[0] = 0;
		//this->DataExtent[1] = this->InternalImage->Width - 1;
		//this->DataExtent[2] = 0;
		//this->DataExtent[3] = this->InternalImage->Height - 1;

		//switch (this->GetFormat())
		//{
		//case vtkTIFFReader::GRAYSCALE:
		//case vtkTIFFReader::PALETTE_GRAYSCALE:
		//	this->SetNumberOfScalarComponents(1);
		//	break;
		//case vtkTIFFReader::RGB:
		//	this->SetNumberOfScalarComponents(this->InternalImage->SamplesPerPixel);
		//	break;
		//case vtkTIFFReader::PALETTE_RGB:
		//	this->SetNumberOfScalarComponents(3);
		//	break;
		//default:
		//	this->SetNumberOfScalarComponents(4);
		//}

		//if (!this->InternalImage->CanRead())
		//{
		//	this->SetNumberOfScalarComponents(4);
		//}

		//// Figure out the appropriate scalar type for the data.
		//int scalarType = VTK_SIGNED_CHAR;
		//short sampleFormat = this->InternalImage->SampleFormat;
		//if (this->InternalImage->BitsPerSample <= 8)
		//{
		//	scalarType = sampleFormat == 2 ? VTK_SIGNED_CHAR : VTK_UNSIGNED_CHAR;
		//}
		//else if (this->InternalImage->BitsPerSample <= 16)
		//{
		//	scalarType = sampleFormat == 2 ? VTK_SHORT : VTK_UNSIGNED_SHORT;
		//}
		//else if (this->InternalImage->BitsPerSample <= 32 && sampleFormat <= 2)
		//{
		//	scalarType = sampleFormat == 2 ? VTK_INT : VTK_UNSIGNED_INT;
		//}
		//else if (this->InternalImage->BitsPerSample <= 32 && sampleFormat == 3)
		//{
		//	scalarType = VTK_FLOAT;
		//}
		//else
		//{
		//	vtkErrorMacro("Unhandled Bit Per Sample: " << this->InternalImage->BitsPerSample);
		//	return;
		//}
		//this->SetDataScalarType(scalarType);

		//// We check if we have a Zeiss image.
		//// Meaning that the SamplesPerPixel is 2 but the image should be treated as
		//// an RGB image.
		//if (this->InternalImage->SamplesPerPixel == 2)
		//{
		//	this->SetNumberOfScalarComponents(3);
		//}

		//// If the tiff file is multi-pages series of tiff images (3D volume)
		//if (this->InternalImage->NumberOfPages > 1)
		//{
		//	if (this->InternalImage->SubFiles > 0)
		//	{
		//		this->DataExtent[4] = 0;
		//		this->DataExtent[5] = this->InternalImage->SubFiles - 1;
		//	}
		//	else
		//	{
		//		this->DataExtent[4] = 0;
		//		this->DataExtent[5] = this->InternalImage->NumberOfPages - 1;
		//	}
		//}

		//// If the tiff is tiled
		//if (this->InternalImage->NumberOfTiles > 1)
		//{
		//	this->DataExtent[0] = 0;
		//	this->DataExtent[1] = this->InternalImage->Width - 1;
		//	this->DataExtent[2] = 0;
		//	this->DataExtent[3] = this->InternalImage->Height - 1;
		//	this->DataExtent[4] = 0;
		//	this->DataExtent[5] = 0;
		//	if (!SpacingSpecifiedFlag)
		//	{
		//		this->DataSpacing[2] = 1.0;
		//	}
		//	if (!OriginSpecifiedFlag)
		//	{
		//		this->DataOrigin[2] = 0.0;
		//	}
		//}



		dir->Delete();
	}
		
	else
	{
			vtkErrorMacro(<< "Couldn't get sorted files. Slices may be in wrong order!");
	}
	
}


void vtkCTIFFImageReader::ExecuteDataWithInformation(vtkDataObject * output, vtkInformation * outInfo)
{
	//TestChecking
	int a = 10;

	vtkImageData *data = this->AllocateOutputData(output, outInfo);

	if (!this->FileName && this->TIFFFileNames->empty())
	{
		vtkErrorMacro(<< "Either a filename was not specified or the specified directory does not contain any DICOM images.");
		this->SetErrorCode(vtkErrorCode::NoFileNameError);
		return;
	}

	data->GetPointData()->GetScalars()->SetName("TIFFImage");

	this->ComputeDataIncrements();

	if (this->FileName)
	{
		vtkDebugMacro(<< "Single file : " << this->FileName);
		//this->Parser->ClearAllDICOMTagCallbacks();
		//this->Parser->OpenFile(this->FileName);
		//this->AppHelper->Clear();
		//this->AppHelper->RegisterCallbacks(this->Parser);
		//this->AppHelper->RegisterPixelDataCallback(this->Parser);

		//this->Parser->ReadHeader();

		//void* imgData = nullptr;
		//DICOMParser::VRTypes dataType;
		//unsigned long imageDataLength;

		//this->AppHelper->GetImageData(imgData, dataType, imageDataLength);
		//if (!imageDataLength)
		//{
		//	vtkErrorMacro(<< "There was a problem retrieving data from: " << this->FileName);
		//	this->SetErrorCode(vtkErrorCode::FileFormatError);
		//	return;
		//}

		//void* buffer = data->GetScalarPointer();
		//if (buffer == nullptr)
		//{
		//	vtkErrorMacro(<< "No memory allocated for image data!");
		//	return;
		//}
		//// DICOM stores the upper left pixel as the first pixel in an
		//// image. VTK stores the lower left pixel as the first pixel in
		//// an image.  Need to flip the data.
		//vtkIdType rowLength;
		//rowLength = this->DataIncrements[1];
		//unsigned char *b = (unsigned char *)buffer;
		//unsigned char *iData = (unsigned char *)imgData;
		//iData += (imageDataLength - rowLength); // beginning of last row
		//for (int i = 0; i < this->AppHelper->GetHeight(); ++i)
		//{
		//	memcpy(b, iData, rowLength);
		//	b += rowLength;
		//	iData -= rowLength;
		//}
	}
	else if (!this->TIFFFileNames->empty())
	{
		vtkDebugMacro(<< "Multiple files (" << static_cast<int>(this->TIFFFileNames->size()) << ")");
		//this->Parser->ClearAllDICOMTagCallbacks();
		//this->AppHelper->Clear();
		//this->AppHelper->RegisterCallbacks(this->Parser);
		//this->AppHelper->RegisterPixelDataCallback(this->Parser);


		data->GetExtent();
		data->GetScalarType();
		data->GetScalarPointer();


		void* buffer = data->GetScalarPointer();
		if (buffer == nullptr)
		{
			vtkErrorMacro(<< "No memory allocated for image data!");
			return;
		}

		std::vector<std::string>::iterator fiter;

		int count = 0;
		vtkIdType numFiles = static_cast<int>(this->TIFFFileNames->size());

		//for (fiter = this->TIFFFileNames->begin();
		//	fiter != this->TIFFFileNames->end();
		//	++fiter)
		//{
		//	count++;
		//	const char *file = fiter->c_str();
		//	vtkDebugMacro(<< "File : " << file);
		//	this->Parser->OpenFile(file);
		//	this->Parser->ReadHeader();

		//	void* imgData = nullptr;
		//	DICOMParser::VRTypes dataType;
		//	unsigned long imageDataLengthInBytes;

		//	this->AppHelper->GetImageData(imgData, dataType, imageDataLengthInBytes);
		//	if (!imageDataLengthInBytes)
		//	{
		//		vtkErrorMacro(<< "There was a problem retrieving data from: " << file);
		//		this->SetErrorCode(vtkErrorCode::FileFormatError);
		//		return;
		//	}

		//	// DICOM stores the upper left pixel as the first pixel in an
		//	// image. VTK stores the lower left pixel as the first pixel in
		//	// an image.  Need to flip the data.
		//	vtkIdType rowLength;
		//	rowLength = this->DataIncrements[1];
		//	unsigned char *b = (unsigned char *)buffer;
		//	unsigned char *iData = (unsigned char *)imgData;
		//	iData += (imageDataLengthInBytes - rowLength); // beginning of last row
		//	for (int i = 0; i < this->AppHelper->GetHeight(); ++i)
		//	{
		//		memcpy(b, iData, rowLength);
		//		b += rowLength;
		//		iData -= rowLength;
		//	}
		//	buffer = ((char*)buffer) + imageDataLengthInBytes;

		//	this->UpdateProgress(float(count) / float(numFiles));
		//	int len = static_cast<int> (strlen((const char*)(*fiter).c_str()));
		//	char* filename = new char[len + 1];
		//	strcpy(filename, (const char*)(*fiter).c_str());
		//	this->SetProgressText(filename);
		//	delete[] filename;
		//}
	}
}
