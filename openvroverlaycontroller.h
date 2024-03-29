//====== Copyright Valve Corporation, All rights reserved. =======

#ifndef OPENVROVERLAYCONTROLLER_H
#define OPENVROVERLAYCONTROLLER_H

#ifdef _WIN32
#pragma once
#endif

#include "openvr.h"

#include <QtCore/QtCore>
// because of incompatibilities with QtOpenGL and GLEW we need to cherry pick includes
#include <QtGui/QVector2D>
#include <QtGui/QMatrix4x4>
#include <QtCore/QVector>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtWidgets/QGraphicsScene>
#include <QtGui/QOffscreenSurface>

class COpenVROverlayController : public QObject
{
	Q_OBJECT
	typedef QObject BaseClass;

public:
    static COpenVROverlayController *SharedInstance();

public:
    COpenVROverlayController();
    virtual ~COpenVROverlayController();

	bool Init();
	void Shutdown();
	void EnableRestart();
	void setOverlayTransform(float, float, float, float, float, float);

	void SetOverlayPosX(float);
	void SetOverlayPosY(float);
	void SetOverlayPosZ(float);
	void SetOverlayRotX(float);
	void SetOverlayRotY(float);
	void SetOverlayRotZ(float);
	void OpenTwitchDashboard(QString,QString);

	bool BHMDAvailable();
    vr::IVRSystem *GetVRSystem();
	vr::HmdError GetLastHmdError();

	QString GetVRDriverString();
	QString GetVRDisplayString();
	QString GetName() { return m_strName; }

	void SetWidget( QWidget *pWidget );

public slots:
	void OnSceneChanged( const QList<QRectF>& );
	void OnTimeoutPumpEvents();

protected:

private:
	bool ConnectToVRRuntime();
	void DisconnectFromVRRuntime();
	void CheckViewAngle();

	vr::TrackedDevicePose_t m_rTrackedDevicePose[ vr::k_unMaxTrackedDeviceCount ];
	QString m_strVRDriver;
	QString m_strVRDisplay;
	QString m_strName;

	vr::HmdError m_eLastHmdError;

private:
	vr::HmdError m_eCompositorError;
	vr::HmdError m_eOverlayError;
	vr::VROverlayHandle_t m_ulOverlayHandle;
    vr::VROverlayHandle_t m_ulOverlayThumbnailHandle;
	vr::TrackedDeviceIndex_t m_VRControllerRight;
	vr::TrackedDeviceIndex_t m_VRControllerLeft;
	vr::IVRSystem *pVRSystem;

	float xRot;
	float yRot;
	float zRot;
	float xPos;
	float yPos;
	float zPos;

	QOpenGLContext *m_pOpenGLContext;
	QGraphicsScene *m_pScene;
	QOpenGLFramebufferObject *m_pFbo;
	QOffscreenSurface *m_pOffscreenSurface;

	QTimer *m_pPumpEventsTimer;

	// the widget we're drawing into the texture
	QWidget *m_pWidget;

	QPointF m_ptLastMouse;
	Qt::MouseButtons m_lastMouseButtons;
};


#endif // OPENVROVERLAYCONTROLLER_H
