#include "stdafx.h"
#include "pxikVTKUIWidgetRepresentation.h"
#include "vtkBalloonRepresentation.h"
#include "vtkImageMapper.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"

vtkStandardNewMacro(pxikVTKUIWidgetRepresentation);

//----------------------------------------------------------------------
pxikVTKUIWidgetRepresentation::pxikVTKUIWidgetRepresentation()
{
	// Initially we are not visible
	this->Visibility = 0;
	this->FrameVisible = 0;

	//// Balloon related
	//this->BalloonText = nullptr;
	//this->BalloonImage = nullptr;
	//this->BalloonLayout = ImageTop;

	//// Displaying the image in the balloon using texture. Create a quad polygon
	//// and apply the texture on top of it.
	//this->ImageSize[0] = 50;
	//this->ImageSize[1] = 50;
	//this->Texture = vtkTexture::New();
	//this->TexturePolyData = vtkPolyData::New();
	//this->TexturePoints = vtkPoints::New();
	//this->TexturePoints->SetNumberOfPoints(4);
	//this->TexturePolyData->SetPoints(this->TexturePoints);
	//vtkCellArray* polys = vtkCellArray::New();
	//polys->InsertNextCell(4);
	//polys->InsertCellPoint(0);
	//polys->InsertCellPoint(1);
	//polys->InsertCellPoint(2);
	//polys->InsertCellPoint(3);
	//this->TexturePolyData->SetPolys(polys);
	//polys->Delete();
	//vtkFloatArray* tc = vtkFloatArray::New();
	//tc->SetNumberOfComponents(2);
	//tc->SetNumberOfTuples(4);
	//tc->InsertComponent(0, 0, 0.0);  tc->InsertComponent(0, 1, 0.0);
	//tc->InsertComponent(1, 0, 1.0);  tc->InsertComponent(1, 1, 0.0);
	//tc->InsertComponent(2, 0, 1.0);  tc->InsertComponent(2, 1, 1.0);
	//tc->InsertComponent(3, 0, 0.0);  tc->InsertComponent(3, 1, 1.0);
	//this->TexturePolyData->GetPointData()->SetTCoords(tc);
	//tc->Delete();
	//this->TextureMapper = vtkPolyDataMapper2D::New();
	//this->TextureMapper->SetInputData(this->TexturePolyData);
	//this->TextureActor = vtkTexturedActor2D::New();
	//this->TextureActor->SetMapper(this->TextureMapper);
	//this->TextureActor->SetTexture(this->Texture);
	//this->ImageProperty = vtkProperty2D::New();
	//this->ImageProperty->SetOpacity(1.0);
	//this->TextureActor->SetProperty(this->ImageProperty);

	// Controlling layout
	this->Offset[0] = 10;
	this->Offset[1] = 10;

	// Controlling Color
	this->Color[0] = 0.949;
	this->Color[1] = 0.949;
	this->Color[2] = 0.949;
	this->Opacity = 0.5;

	// The frame
	this->FramePoints = vtkPoints::New();
	this->FramePoints->SetNumberOfPoints(4);
	this->FramePolygon = vtkCellArray::New();
	this->FramePolygon->Allocate(this->FramePolygon->EstimateSize(1, 5));
	this->FramePolygon->InsertNextCell(4);
	this->FramePolygon->InsertCellPoint(0);
	this->FramePolygon->InsertCellPoint(1);
	this->FramePolygon->InsertCellPoint(2);
	this->FramePolygon->InsertCellPoint(3);
	this->FramePolyData = vtkPolyData::New();
	this->FramePolyData->SetPoints(this->FramePoints);
	this->FramePolyData->SetPolys(this->FramePolygon);
	this->FrameMapper = vtkPolyDataMapper2D::New();
	this->FrameMapper->SetInputData(this->FramePolyData);
	this->FrameActor = vtkActor2D::New();
	this->FrameActor->SetMapper(this->FrameMapper);
	this->FrameProperty = vtkProperty2D::New();
	this->FrameProperty->SetColor(this->Color);
	this->FrameProperty->SetOpacity(this->Opacity);
	this->FrameActor->SetProperty(this->FrameProperty);
}

//----------------------------------------------------------------------
pxikVTKUIWidgetRepresentation::~pxikVTKUIWidgetRepresentation()
{
	// The frame
	this->FramePoints->Delete();
	this->FramePolygon->Delete();
	this->FramePolyData->Delete();
	this->FrameMapper->Delete();
	this->FrameActor->Delete();
	this->FrameProperty->Delete();
}


//----------------------------------------------------------------------
void pxikVTKUIWidgetRepresentation::StartWidgetInteraction(double e[2])
{
	this->StartEventPosition[0] = e[0];
	this->StartEventPosition[1] = e[1];
	this->VisibilityOn();
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetRepresentation::EndWidgetInteraction(double e[2])
{
	this->VisibilityOff();
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetRepresentation::BuildRepresentation()
{
	if (this->GetMTime() > this->BuildTime ||
		(this->Renderer && this->Renderer->GetVTKWindow() &&
			this->Renderer->GetVTKWindow()->GetMTime() > this->BuildTime))
	{
		this->FrameVisible = 1;

		int size[2];
		size[0] = (this->Renderer->GetSize())[0];
		size[1] = (this->Renderer->GetSize())[1];
		int stringSize[2]; stringSize[0] = stringSize[1] = 0;
		double imageSize[2]; imageSize[0] = imageSize[1] = 0.0;
		double frameSize[2]; frameSize[0] = frameSize[1] = 0.0;
		double io[2], so[2], fo[2];
		io[0] = 0.0;
		io[1] = 0.0;
		so[0] = 0.0;
		so[1] = 0.0;
		fo[0] = 0.0;
		fo[1] = 0.0;
		double e[2];
		e[0] = static_cast<double>(this->StartEventPosition[0] + this->Offset[0]);
		e[1] = static_cast<double>(this->StartEventPosition[1] + this->Offset[1]);

		
		// Layout the text and image
		if (this->FrameVisible)
		{
			io[0] = 0.0;
			io[1] = 0.0;
			
			//this->AdjustImageSize(imageSize);
				//if (this->BalloonLayout == ImageTop)
				//{
				//	frameSize[1] = stringSize[1] + 2 * this->Padding;
				//	double length = (imageSize[0] > (stringSize[0] + 2 * this->Padding) ?
				//		imageSize[0] : (stringSize[0] + 2 * this->Padding));
				//
				//	frameSize[0] = length;
				//	double scale = length / imageSize[0];
				//	this->ScaleImage(imageSize, scale);
				//	io[0] = 0.0;
				//	io[1] = frameSize[1];
				//	fo[0] = 0.0;
				//	fo[1] = 0.0;
				//	so[0] = length / 2.0 - stringSize[0] / 2.0;
				//	so[1] = this->Padding;
				//}
				//else if (this->BalloonLayout == ImageBottom)
				//{
				//	frameSize[1] = stringSize[1] + 2 * this->Padding;
				//	double length = (imageSize[0] > (stringSize[0] + 2 * this->Padding) ?
				//		imageSize[0] : (stringSize[0] + 2 * this->Padding));
				//	frameSize[0] = length;
				//	double scale = length / imageSize[0];
				//	this->ScaleImage(imageSize, scale);
				//	io[0] = 0.0;
				//	io[1] = 0.0;
				//	fo[0] = 0.0;
				//	fo[1] = imageSize[1];
				//	so[0] = length / 2.0 - stringSize[0] / 2.0;
				//	so[1] = imageSize[1] + this->Padding;
				//}
				//else if (this->BalloonLayout == ImageLeft)
				//{
				//	frameSize[0] = stringSize[0] + 2 * this->Padding;
				//	double length = (imageSize[1] > (stringSize[1] + 2 * this->Padding) ?
				//		imageSize[1] : (stringSize[1] + 2 * this->Padding));
				//	frameSize[1] = length;
				//	double scale = length / imageSize[1];
				//	this->ScaleImage(imageSize, scale);
				//	io[0] = 0.0;
				//	io[1] = 0.0;
				//	fo[0] = imageSize[0];
				//	fo[1] = 0.0;
				//	so[0] = imageSize[0] + this->Padding;
				//	so[1] = length / 2.0 - stringSize[1] / 2.0;
				//}
				//else if (this->BalloonLayout == ImageRight)
				//{
				//	frameSize[0] = stringSize[0] + 2 * this->Padding;
				//	double length = (imageSize[1] > (stringSize[1] + 2 * this->Padding) ?
				//		imageSize[1] : (stringSize[1] + 2 * this->Padding));
				//	frameSize[1] = length;
				//	double scale = length / imageSize[1];
				//	this->ScaleImage(imageSize, scale);
				//	io[0] = frameSize[0];
				//	io[1] = 0.0;
				//	fo[0] = 0.0;
				//	fo[1] = 0.0;
				//	so[0] = this->Padding;
				//	so[1] = length / 2.0 - stringSize[1] / 2.0;
				//}
			

			

			// Reposition the origin of the balloon if it's off the renderer
			if (e[0] < 0)
			{
				e[0] = 0.0;
			}
			if (e[1] < 0)
			{
				e[1] = 0.0;
			}
			if ((e[0] + frameSize[0] + imageSize[0]) > size[0])
			{
				e[0] = size[0] - (frameSize[0] + imageSize[0]);
			}
			if ((e[1] + frameSize[1] + imageSize[1]) > size[1])
			{
				e[1] = size[1] - (frameSize[1] + imageSize[1]);
			}

			// Draw the text if visible
			this->FramePoints->SetPoint(0, e[0] + fo[0], e[1] + fo[1], 0.0);
			this->FramePoints->SetPoint(1, e[0] + fo[0] + frameSize[0], e[1] + fo[1], 0.0);
			this->FramePoints->SetPoint(2, e[0] + fo[0] + frameSize[0], e[1] + fo[1] + frameSize[1], 0.0);
			this->FramePoints->SetPoint(3, e[0] + fo[0], e[1] + fo[1] + frameSize[1], 0.0);
			this->FramePoints->Modified();


		}//if something visible

		// Update the properties
		this->FrameActor->SetProperty(this->FrameProperty);

		this->BuildTime.Modified();
	}
}

//----------------------------------------------------------------------
int pxikVTKUIWidgetRepresentation::ComputeInteractionState(int X, int Y, int modify)
{
	// Is it in the text region or the image region?
	double x0[3], x2[3];
	int origin[2] = { 0, 0 };
	if (this->Renderer)
	{
		origin[0] = (this->Renderer->GetOrigin())[0];
		origin[1] = (this->Renderer->GetOrigin())[1];
	}

	if (this->FrameVisible)
	{
		this->FramePoints->GetPoint(0, x0);
		this->FramePoints->GetPoint(2, x2);
		for (int i = 0; i < 2; ++i)
		{
			x0[i] += origin[i];
			x2[i] += origin[i];
		}
		if ((x0[0] <= X && X <= x2[0]) && (x0[1] <= Y && Y <= x2[1]))
		{
			return pxikVTKUIWidgetRepresentation::Onside;
		}
	}

	return pxikVTKUIWidgetRepresentation::Outside;
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetRepresentation::ReleaseGraphicsResources(vtkWindow * w)
{
	this->FrameActor->ReleaseGraphicsResources(w);
}

//----------------------------------------------------------------------
int pxikVTKUIWidgetRepresentation::RenderOverlay(vtkViewport * v)
{
	int count = 0;
	this->BuildRepresentation();

	if (this->FrameVisible)
	{
		count += this->FrameActor->RenderOverlay(v);
	}

	return count;
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetRepresentation::setMargin(int value, int margin)
{
	int cmpMargin = 0;

	if (margin <= 0)
	{
		return;
	}

	m_margin = margin;
	for (cmpMargin = 1; cmpMargin&cmpMargin; cmpMargin *= 2)
	{
		switch (cmpMargin)
		{
		case marginTop:
			m_marginTopValue = value;
			break;
		case marginBtm:
			m_marginBtmValue = value;
			break;
		case marginLeft:
			m_marginLeftValue = value;
			break;
		case marginRight:
			m_marginRightValue = value;
			break;
		default:
			break;
		}
	}
}

//----------------------------------------------------------------------
int pxikVTKUIWidgetRepresentation::getMargin(int margin)
{
	return m_margin;
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetRepresentation::removeAllMargin()
{
	m_margin = 0;
	m_marginTopValue = 0;
	m_marginBtmValue = 0;
	m_marginLeftValue = 0;
	m_marginRightValue = 0;

	this->Modified();
}
//----------------------------------------------------------------------
void pxikVTKUIWidgetRepresentation::setPosition(int x, int y)
{

	if (x < 0 || y < 0)
	{
		return;
	}

	m_PositionX = x;
	m_PositionY = y;

	this->Modified();
}

//----------------------------------------------------------------------
int * pxikVTKUIWidgetRepresentation::getPosition()
{
	int* position = new int[2];
	position[0] = m_PositionX;
	position[1] = m_PositionY;

	return position;
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetRepresentation::setSize(int width, int height)
{
	m_width = width;
	m_height = height;

	this->Modified();
}

//----------------------------------------------------------------------
int * pxikVTKUIWidgetRepresentation::getSize()
{
	int* size = new int[2];
	size[0] = m_width;
	size[1] = m_height;

	return size;
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetRepresentation::AdjustFrameSize(double frameSize[2])
{
	frameSize[0] = FrameSize[0];
	frameSize[1] = FrameSize[1];

	int parentSize[2] = { 0,0 };
	int *tmp = nullptr;
	if (m_parent != nullptr)
	{ 
		tmp = m_parent->getSize();
		parentSize[0] = tmp[0];
		parentSize[1] = tmp[1];
	}
	else
	{ 
		//꼬인다 시.....
		//render연결 전에 미리 생성되기 때문임
		//render연결 후 한번 업데이트를 시도해야함

		//아님 연결되고 시작할때 Bulid되는 걸 확인함
		tmp = this->Renderer->GetSize();
		parentSize[0] = tmp[0];
		parentSize[1] = tmp[1];
	}
	delete tmp;

	if (m_margin & marginLeft && m_margin & marginRight)
		frameSize[0] = parentSize[0] - (m_marginTopValue + m_marginBtmValue);

	if (m_margin & marginTop && m_margin & marginBtm)
		frameSize[1] = parentSize[1] - (m_marginTopValue + m_marginBtmValue);
}

void pxikVTKUIWidgetRepresentation::AdjustFramePosition(double * framePosition)
{
}

//----------------------------------------------------------------------
int * pxikVTKUIWidgetRepresentation::getUIPosition()
{
	return nullptr;
}

//----------------------------------------------------------------------
int * pxikVTKUIWidgetRepresentation::getUISize()
{
	////TODO: 해당값이 실제 이미지의 Bound룰 가져오는지 애매함 
	////      확인할 것
	//double* tmp = this->WidgetRep->getImageSize();
	//int * rt = new int[2];
	//
	//rt[0] = static_cast<int>(tmp[0]);
	//rt[1] = static_cast<int>(tmp[1]);
	//
	//return rt;
	return nullptr;
}

//----------------------------------------------------------------------
void pxikVTKUIWidgetRepresentation::ScaleImage(double imageSize[2], double scale)
{
}

void pxikVTKUIWidgetRepresentation::PrintSelf(ostream & os, vtkIndent indent)
{
}

void pxikVTKUIWidgetRepresentation::SetFrameProperty(vtkProperty2D * p)
{
}
