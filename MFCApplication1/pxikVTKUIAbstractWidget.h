
#ifndef pxikVTKUIAbstractWidget_h
#define pxikVTKUIAbstractWidget_h

#include <vtkAbstractWidget.h>
#include <list>

class pxikVTKUIWidgetAbstractRepresentation;
class vtkImageMapper;

class pxikVTKUIAbstractWidget : public vtkAbstractWidget
{
public:

	/*!
	 * Instantiate the object.
	 */
	vtkTypeMacro(pxikVTKUIAbstractWidget, vtkAbstractWidget);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	virtual void CreateDefaultRepresentation() = 0;

	void SetWidgetRepresentation(pxikVTKUIWidgetAbstractRepresentation *r)
	{
		this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));
	}



	//UI끼리의 연결을 위한 Parts
	std::list<pxikVTKUIAbstractWidget*>		 m_uiList;
	pxikVTKUIAbstractWidget					*m_parentUI;
	void addUI(pxikVTKUIAbstractWidget *);
	void removeUI(pxikVTKUIAbstractWidget * );
	
	//@{
	//please set this functions protected
	void setParentUI(pxikVTKUIAbstractWidget *);
	void removeParentUI();
	//@}

	//배경 관련 part
	vtkImageData *m_backgroundImage;
	void setBackgroundImage(vtkImageData * src);
	void removeBackgroundImage();

	struct color_t {
		double red, green, blue;

		color_t()
		{
			red		= 0;
			green	= 0;
			blue	= 0;
		}

		void setcolor(double _red, double _green, double _blue)
		{
			red		= _red;
			green	= _green;
			blue	= _blue;
		}

	};

	struct color_t m_bactgroundColor;
	void setBackgroundColor(color_t);
	void getBackgroundColor();
	
	void createImageData(vtkImageData *);


	vtkActor2D *m_UIActor;
	vtkImageMapper *m_UIMapper;

	int m_margin = 0;
	int m_marginTopValue = 0;
	int m_marginBtmValue = 0;
	int m_marginLeftValue = 0;
	int m_marginRightValue = 0;
	enum : int
	{
		marginTop = 1,
		marginBtm = 2,
		marginLeft = 4,
		marginRight = 8
	};

	void setMargin(int value, int margin);
	//please use single margin (2^n, 0 <= n <= 3)
	int getMargin(int margin);
	void removeAllMargin();
	
	int m_PositionX, m_PositionY;
	void setPosition(int x, int y);
	int* getPosition();

	int m_width, m_height;
	void setSize(int width, int height);
	int* getSize();

	/*struct color_t m_borderColor;
	void setBorder(int);
	void setBorderColor();
	void getBorderColor();*/
	
	//please Call, When UI Layout geometry or marign is Changed
	void UpdateUI();

	//{@
	/**
	* UI의 실제 그려지는 위치,
	* 해당 값을 사용하여 자식 UI의 location을 결정할 것
	*/
	int* getUIPosition();
	int* getUISize();
	//@}
	
	
	
	int m_lastButtonPressed;
	enum
	{
		VTK_NO_BUTTON = 0,
		VTK_LEFT_BUTTON = 1,
		VTK_MIDDLE_BUTTON = 2,
		VTK_RIGHT_BUTTON = 3
	};
	
	int m_state;
	enum WidgetState
	{
		Outside = 0,
		Click,
		Moving,
		Hovering
	};

	void SetEnabled(int) override;

protected:
	pxikVTKUIAbstractWidget();
	~pxikVTKUIAbstractWidget();
	
private:
	pxikVTKUIAbstractWidget(const pxikVTKUIAbstractWidget&) = delete;
	void operator=(const pxikVTKUIAbstractWidget&) = delete;
};

#endif