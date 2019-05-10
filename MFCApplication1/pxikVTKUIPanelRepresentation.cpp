#include "stdafx.h"

#include "vtkProperty2D.h"


#include "pxikVTKUIWidgetAbstractRepresentation.h"
#include "pxikVTKUIPanelRepresentation.h"


#include <list>

vtkStandardNewMacro(pxikVTKUIPanelRepresentation);

void pxikVTKUIPanelRepresentation::updateFrameColor(double red, double green, double blue)
{
	if (FrameProperty == nullptr)
		return;
	FrameProperty->SetColor(red, green, blue);
	this->Modified();
}

//----------------------------------------------------------------------
pxikVTKUIPanelRepresentation::pxikVTKUIPanelRepresentation()
{
	// Initially we are not visible
	this->Visibility = 1;
	this->FrameVisible = 1;


	m_Geometry.X = -1;
	m_Geometry.Y = -1;
	m_Geometry.Width = -1;
	m_Geometry.Height = -1;

	// Controlling Color
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
	this->FrameProperty->SetColor(0.949, 0.949, 0.949);
	this->FrameProperty->SetOpacity(this->Opacity);
	this->FrameActor->SetProperty(this->FrameProperty);

	this->StartEventPosition[0] = 0;
	this->StartEventPosition[1] = 1;
}

//----------------------------------------------------------------------
pxikVTKUIPanelRepresentation::~pxikVTKUIPanelRepresentation()
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
void pxikVTKUIPanelRepresentation::StartWidgetInteraction(double e[2])
{
	//this->StartEventPosition[0] = e[0];
	//this->StartEventPosition[1] = e[1];
	this->VisibilityOn();
}

//----------------------------------------------------------------------
void pxikVTKUIPanelRepresentation::EndWidgetInteraction(double e[2])
{
	this->VisibilityOff();
}

//----------------------------------------------------------------------
void pxikVTKUIPanelRepresentation::BuildRepresentation()
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

			this->AdjustFrameSize(frameSize);

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
int pxikVTKUIPanelRepresentation::ComputeInteractionState(int X, int Y, int modify)
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
			return pxikVTKUIPanelRepresentation::Onside;
		}
	}

	return pxikVTKUIPanelRepresentation::Outside;
}

//----------------------------------------------------------------------
void pxikVTKUIPanelRepresentation::ReleaseGraphicsResources(vtkWindow * w)
{
	this->FrameActor->ReleaseGraphicsResources(w);
}

//----------------------------------------------------------------------
int pxikVTKUIPanelRepresentation::RenderOverlay(vtkViewport * v)
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
void pxikVTKUIPanelRepresentation::setMargin(int value, int margin)
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
int pxikVTKUIPanelRepresentation::getMargin(int margin)
{
	return m_margin;
}

//----------------------------------------------------------------------
void pxikVTKUIPanelRepresentation::removeAllMargin()
{
	m_margin = 0;
	m_marginTopValue = 0;
	m_marginBtmValue = 0;
	m_marginLeftValue = 0;
	m_marginRightValue = 0;

	this->Modified();
}
//----------------------------------------------------------------------
void pxikVTKUIPanelRepresentation::setPosition(int x, int y)
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
int * pxikVTKUIPanelRepresentation::getPosition()
{
	int* position = new int[2];
	position[0] = m_PositionX;
	position[1] = m_PositionY;

	return position;
}

//----------------------------------------------------------------------
void pxikVTKUIPanelRepresentation::setSize(int width, int height)
{
	m_width = width;
	m_height = height;

	FrameSize[0] = width;
	FrameSize[1] = height;

	this->Modified();
}

//----------------------------------------------------------------------
int * pxikVTKUIPanelRepresentation::getSize()
{
	int* size = new int[2];
	size[0] = m_width;
	size[1] = m_height;

	this->FrameSize[0] = m_width;
	this->FrameSize[1] = m_height;

	return size;
}

//----------------------------------------------------------------------
void pxikVTKUIPanelRepresentation::AdjustFrameSize(double frameSize[2])
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
		delete tmp;
	}
	else
	{

		//아님 연결되고 시작할때 Bulid되는 걸 확인함
		tmp = this->Renderer->GetSize();
		parentSize[0] = tmp[0];
		parentSize[1] = tmp[1];
	}


	if (m_margin & marginLeft && m_margin & marginRight)
	{
		frameSize[0] = parentSize[0] - (m_marginLeftValue + m_marginRightValue);
		this->StartEventPosition[0] = (m_marginLeftValue);
	}

	if (m_margin & marginTop && m_margin & marginBtm)
	{
		frameSize[1] = parentSize[1] - (m_marginTopValue + m_marginBtmValue);
		this->StartEventPosition[1] = m_marginBtmValue;
	}
}

void pxikVTKUIPanelRepresentation::AdjustFramePosition(double * framePosition)
{
}

//----------------------------------------------------------------------
int * pxikVTKUIPanelRepresentation::getUIPosition()
{
	return nullptr;
}

//----------------------------------------------------------------------
int * pxikVTKUIPanelRepresentation::getUISize()
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
void pxikVTKUIPanelRepresentation::ScaleImage(double imageSize[2], double scale)
{
}

void pxikVTKUIPanelRepresentation::PrintSelf(ostream & os, vtkIndent indent)
{
}

void pxikVTKUIPanelRepresentation::SetFrameProperty(vtkProperty2D * p)
{
}
