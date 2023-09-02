//====== Copyright Valve Corporation, All rights reserved. =======


#include "openvroverlaycontroller.h"
#include "overlaywidget.h"


#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QtWidgets/QWidget>
#include <QMouseEvent>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsEllipseItem>
#include <QCursor>
#include <QFile>
#include <iostream>
#include <cmath>

using namespace vr;

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
COpenVROverlayController *s_pSharedVRController = NULL;

COpenVROverlayController *COpenVROverlayController::SharedInstance()
{
	if ( !s_pSharedVRController )
	{
        s_pSharedVRController = new COpenVROverlayController();
	}
	return s_pSharedVRController;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
COpenVROverlayController::COpenVROverlayController()
	: BaseClass()
	, m_strVRDriver( "No Driver" )
	, m_strVRDisplay( "No Display" )
	, m_VRControllerRight( -1 )
	, m_VRControllerLeft( -1 )
	, pVRSystem ( NULL )
    , m_eLastHmdError( vr::VRInitError_None )
    , m_eCompositorError( vr::VRInitError_None )
    , m_eOverlayError( vr::VRInitError_None )
	, m_ulOverlayHandle( vr::k_ulOverlayHandleInvalid )
	, m_pOpenGLContext( NULL )
	, m_pScene( NULL )
	, m_pFbo( NULL )
	, m_pOffscreenSurface ( NULL )
	, m_pPumpEventsTimer( NULL )
	, m_pWidget( NULL )
	, m_lastMouseButtons( 0 )
	, xRot ( 0 )
	, yRot ( 0 )
	, zRot ( 0 )
	, xPos ( 0 )
	, yPos ( 0 )
	, zPos ( 0 )
{
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
COpenVROverlayController::~COpenVROverlayController()
{
}


//-----------------------------------------------------------------------------
// Purpose: Helper to get a string from a tracked device property and turn it
//			into a QString
//-----------------------------------------------------------------------------
QString GetTrackedDeviceString( vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop )
{
	char buf[128];
	vr::TrackedPropertyError err;
	pHmd->GetStringTrackedDeviceProperty( unDevice, prop, buf, sizeof( buf ), &err );
	if( err != vr::TrackedProp_Success )
	{
		return QString( "Error Getting String: " ) + pHmd->GetPropErrorNameFromEnum( err );
	}
	else
	{
		return buf;
	}
}

void COpenVROverlayController::CheckViewAngle(){
	
}

void COpenVROverlayController::OpenTwitchDashboard(QString Username, QString YoutubeChatLink){
	((OverlayWidget*)m_pWidget)->openDashboard(Username, YoutubeChatLink);
}
void COpenVROverlayController::SetOverlayPosX(float newpos){
	setOverlayTransform(newpos, yPos, zPos, xRot, yRot, zRot);
}
void COpenVROverlayController::SetOverlayPosY(float newpos){
	setOverlayTransform(xPos, newpos, zPos, xRot, yRot, zRot);
}
void COpenVROverlayController::SetOverlayPosZ(float newpos){
	setOverlayTransform(xPos, yPos, newpos, xRot, yRot, zRot);
}
void COpenVROverlayController::SetOverlayRotX(float newrot){
	setOverlayTransform(xPos, yPos, zPos, newrot, yRot, zRot);
}
void COpenVROverlayController::SetOverlayRotY(float newrot){
	setOverlayTransform(xPos, yPos, zPos, xRot, newrot, zRot);
}
void COpenVROverlayController::SetOverlayRotZ(float newrot){
	setOverlayTransform(xPos, yPos, zPos, xRot, yRot, newrot);
}

void COpenVROverlayController::setOverlayTransform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ){
		
		vr::HmdMatrix34_t transform;

		double xAngle = rotX*(M_PI / 180.0); // 0 degree angle around X-axis
		double yAngle = rotY*(M_PI / 180.0); // 90 degree angle around Y-axis
		double zAngle = rotZ*(M_PI / 180.0); // 30 degree angle around Z-axis

		xRot = rotX;
		yRot = rotY;
		zRot = rotZ;

		xPos = posX;
		yPos = posY;
		zPos = posZ;

		float cosX = cos(xAngle);
		float sinX = sin(xAngle);
		float cosY = cos(yAngle);
		float sinY = sin(yAngle);
		float cosZ = cos(zAngle);
		float sinZ = sin(zAngle);

		transform.m[0][0] = cosY * cosZ;
		transform.m[0][1] = sinX * sinY * cosZ - cosX * sinZ;
		transform.m[0][2] = cosX * sinY * cosZ + sinX * sinZ;
		transform.m[1][0] = cosY * sinZ;
		transform.m[1][1] = sinX * sinY * sinZ + cosX * cosZ;
		transform.m[1][2] = cosX * sinY * sinZ - sinX * cosZ;
		transform.m[2][0] = -sinY;
		transform.m[2][1] = sinX * cosY;
		transform.m[2][2] = cosX * cosY;

		// Position
		transform.m[0][3] = posX;
		transform.m[1][3] = posY;
		transform.m[2][3] = posZ;
		
		vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(m_ulOverlayHandle, m_VRControllerRight, &transform);
}
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool COpenVROverlayController::Init()
{
	bool bSuccess = true;

    m_strName = "systemoverlay";

	QStringList arguments = qApp->arguments();

	int nNameArg = arguments.indexOf( "-name" );
	if( nNameArg != -1 && nNameArg + 2 <= arguments.size() )
	{
		m_strName = arguments.at( nNameArg + 1 );
	}

	QSurfaceFormat format;
	format.setMajorVersion( 4 );
	format.setMinorVersion( 1 );
	format.setProfile( QSurfaceFormat::CompatibilityProfile );

	m_pOpenGLContext = new QOpenGLContext();
	m_pOpenGLContext->setFormat( format );
	bSuccess = m_pOpenGLContext->create();
	if( !bSuccess )
		return false;

	// create an offscreen surface to attach the context and FBO to
	m_pOffscreenSurface = new QOffscreenSurface();
	m_pOffscreenSurface->create();
	m_pOpenGLContext->makeCurrent( m_pOffscreenSurface );

	m_pScene = new QGraphicsScene();
	connect( m_pScene, SIGNAL(changed(const QList<QRectF>&)), this, SLOT( OnSceneChanged(const QList<QRectF>&)) );
	
	// Loading the OpenVR Runtime
	bSuccess = ConnectToVRRuntime();

    bSuccess = bSuccess && vr::VRCompositor() != NULL;

    if( vr::VROverlay() )
	{
        std::string sKey = std::string( "sample." ) + m_strName.toStdString();
        vr::VROverlayError overlayError = vr::VROverlay()->CreateOverlay( sKey.c_str(), m_strName.toStdString().c_str(), &m_ulOverlayHandle);
		bSuccess = bSuccess && overlayError == vr::VROverlayError_None;
	}

	if( bSuccess )
	{
		
        setOverlayTransform(0.2, 0.2, 0.1, 0.0f, -90.0f, -30.0f);
		vr::VROverlay()->SetOverlayWidthInMeters( m_ulOverlayHandle, .2f );
        vr::VROverlay()->SetOverlayInputMethod( m_ulOverlayHandle, vr::VROverlayInputMethod_Mouse );
		vr::VROverlay()->ShowOverlay( m_ulOverlayHandle);

		m_pPumpEventsTimer = new QTimer( this );
		connect(m_pPumpEventsTimer, SIGNAL( timeout() ), this, SLOT( OnTimeoutPumpEvents() ) );
		m_pPumpEventsTimer->setInterval( 20 );
		m_pPumpEventsTimer->start();

	}
	return true;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COpenVROverlayController::Shutdown()
{
	DisconnectFromVRRuntime();

	delete m_pScene;
	delete m_pFbo;
	delete m_pOffscreenSurface;

	if( m_pOpenGLContext )
	{
//		m_pOpenGLContext->destroy();
		delete m_pOpenGLContext;
		m_pOpenGLContext = NULL;
	}
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COpenVROverlayController::OnSceneChanged( const QList<QRectF>& )
{
	CheckViewAngle();
	// skip rendering if the overlay isn't visible
    if( ( m_ulOverlayHandle == k_ulOverlayHandleInvalid ) || !vr::VROverlay() ||
        ( !vr::VROverlay()->IsOverlayVisible( m_ulOverlayHandle ) ) )
        return;

	m_pOpenGLContext->makeCurrent( m_pOffscreenSurface );
	m_pFbo->bind();
	
	QOpenGLPaintDevice device( m_pFbo->size() );
	QPainter painter( &device );

	m_pScene->render( &painter );
	
	//painter.drawPixmap(QRectF(), m_pWidget->grab(),QRectF());
	
	//QFile file("yourFile.png");
	//file.open(QIODevice::WriteOnly);
	//m_pFbo->toImage().save(&file,"PNG");
	//m_pWidget->grab().save(&file, "PNG");


	GLuint unTexture = m_pFbo->texture();
	m_pFbo->release();
	if( unTexture != 0 )
	{
        vr::Texture_t texture = {(void*)(uintptr_t)unTexture, vr::TextureType_OpenGL, vr::ColorSpace_Auto };
        vr::VROverlay()->SetOverlayTexture( m_ulOverlayHandle, &texture );
	}
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COpenVROverlayController::OnTimeoutPumpEvents()
{
    if( !vr::VRSystem() )
		return;

	vr::VREvent_t vrEvent;
    while( vr::VROverlay()->PollNextOverlayEvent( m_ulOverlayHandle, &vrEvent, sizeof( vrEvent )  ) )
	{
		switch( vrEvent.eventType )
		{
		case vr::VREvent_MouseMove:
			{
				QPointF ptNewMouse( vrEvent.data.mouse.x, vrEvent.data.mouse.y );
				QPoint ptGlobal = ptNewMouse.toPoint();
				QGraphicsSceneMouseEvent mouseEvent( QEvent::GraphicsSceneMouseMove );
				mouseEvent.setWidget( NULL );
				mouseEvent.setPos( ptNewMouse );
				mouseEvent.setScenePos( ptGlobal );
				mouseEvent.setScreenPos( ptGlobal );
				mouseEvent.setLastPos( m_ptLastMouse );
				mouseEvent.setLastScenePos( m_pWidget->mapToGlobal( m_ptLastMouse.toPoint() ) );
				mouseEvent.setLastScreenPos( m_pWidget->mapToGlobal( m_ptLastMouse.toPoint() ) );
				mouseEvent.setButtons( m_lastMouseButtons );
				mouseEvent.setButton( Qt::NoButton );
				mouseEvent.setModifiers( 0 );
				mouseEvent.setAccepted( false );

				m_ptLastMouse = ptNewMouse;
				QApplication::sendEvent( m_pScene, &mouseEvent );

				OnSceneChanged( QList<QRectF>() );
			}
			break;

		case vr::VREvent_MouseButtonDown:
			{
				Qt::MouseButton button = vrEvent.data.mouse.button == vr::VRMouseButton_Right ? Qt::RightButton : Qt::LeftButton;

				m_lastMouseButtons |= button;

				QPoint ptGlobal = m_ptLastMouse.toPoint();
				QGraphicsSceneMouseEvent mouseEvent( QEvent::GraphicsSceneMousePress );
				mouseEvent.setWidget( NULL );
				mouseEvent.setPos( m_ptLastMouse );
				mouseEvent.setButtonDownPos( button, m_ptLastMouse );
				mouseEvent.setButtonDownScenePos( button, ptGlobal);
				mouseEvent.setButtonDownScreenPos( button, ptGlobal );
				mouseEvent.setScenePos( ptGlobal );
				mouseEvent.setScreenPos( ptGlobal );
				mouseEvent.setLastPos( m_ptLastMouse );
				mouseEvent.setLastScenePos( ptGlobal );
				mouseEvent.setLastScreenPos( ptGlobal );
				mouseEvent.setButtons( m_lastMouseButtons );
				mouseEvent.setButton( button );
				mouseEvent.setModifiers( 0 );
				mouseEvent.setAccepted( false );

				QApplication::sendEvent( m_pScene, &mouseEvent );
			}
			break;

		case vr::VREvent_MouseButtonUp:
			{
				Qt::MouseButton button = vrEvent.data.mouse.button == vr::VRMouseButton_Right ? Qt::RightButton : Qt::LeftButton;
				m_lastMouseButtons &= ~button;

				QPoint ptGlobal = m_ptLastMouse.toPoint();
				QGraphicsSceneMouseEvent mouseEvent( QEvent::GraphicsSceneMouseRelease );
				mouseEvent.setWidget( NULL );
				mouseEvent.setPos( m_ptLastMouse );
				mouseEvent.setScenePos( ptGlobal );
				mouseEvent.setScreenPos( ptGlobal );
				mouseEvent.setLastPos( m_ptLastMouse );
				mouseEvent.setLastScenePos( ptGlobal );
				mouseEvent.setLastScreenPos( ptGlobal );
				mouseEvent.setButtons( m_lastMouseButtons );
				mouseEvent.setButton( button );
				mouseEvent.setModifiers( 0 );
				mouseEvent.setAccepted( false );

				QApplication::sendEvent(  m_pScene, &mouseEvent );
			}
			break;

		case vr::VREvent_OverlayShown:
			{
				m_pWidget->repaint();
			}
			break;

        case vr::VREvent_Quit:
            QApplication::exit();
            break;
		}
	}

    if( m_ulOverlayThumbnailHandle != vr::k_ulOverlayHandleInvalid )
    {
        while( vr::VROverlay()->PollNextOverlayEvent( m_ulOverlayThumbnailHandle, &vrEvent, sizeof( vrEvent)  ) )
        {
            switch( vrEvent.eventType )
            {
            case vr::VREvent_OverlayShown:
                {
                    m_pWidget->repaint();
                }
                break;
            }
        }
    }

}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COpenVROverlayController::SetWidget( QWidget *pWidget )
{
	if( m_pScene )
	{
		// all of the mouse handling stuff requires that the widget be at 0,0
		pWidget->move( 0, 0 );
		m_pScene->addWidget( pWidget );
	}
	m_pWidget = pWidget;

	m_pFbo = new QOpenGLFramebufferObject( pWidget->width(), pWidget->height(), GL_TEXTURE_2D );

    if( vr::VROverlay() )
    {
        vr::HmdVector2_t vecWindowSize =
        {
            (float)pWidget->width(),
            (float)pWidget->height()
        };
        vr::VROverlay()->SetOverlayMouseScale( m_ulOverlayHandle, &vecWindowSize );
    }

}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool COpenVROverlayController::ConnectToVRRuntime()
{
    m_eLastHmdError = vr::VRInitError_None;
    vr::IVRSystem *pVRSystem = vr::VR_Init( &m_eLastHmdError, vr::VRApplication_Overlay );

    if ( m_eLastHmdError != vr::VRInitError_None )
	{
		m_strVRDriver = "No Driver";
		m_strVRDisplay = "No Display";
		return false;
	}

	for (vr::TrackedDeviceIndex_t i=0; i<=vr::k_unMaxTrackedDeviceCount;i++){
		if(pVRSystem->GetTrackedDeviceClass(i)==vr::TrackedControllerRole_LeftHand){
			m_VRControllerLeft = i;
		}else if (pVRSystem->GetTrackedDeviceClass(i)==vr::TrackedControllerRole_RightHand)
		{
			m_VRControllerRight = i;
		}
		
	}

    m_strVRDriver = GetTrackedDeviceString(pVRSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
    m_strVRDisplay = GetTrackedDeviceString(pVRSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);
	return true;
}


void COpenVROverlayController::DisconnectFromVRRuntime()
{
	vr::VR_Shutdown();
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
QString COpenVROverlayController::GetVRDriverString()
{
	return m_strVRDriver;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
QString COpenVROverlayController::GetVRDisplayString()
{
	return m_strVRDisplay;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool COpenVROverlayController::BHMDAvailable()
{
    return vr::VRSystem() != NULL;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

vr::HmdError COpenVROverlayController::GetLastHmdError()
{
	return m_eLastHmdError;
}


