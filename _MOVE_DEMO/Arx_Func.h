#pragma once
using namespace std;
#include "arxHeaders.h"								//ObjectARX��ͷ�ļ��Լ�lib����
#include "rxmfcapi.h"								//һЩAPI
#include "dbSubD.h"
#include "dbobjptr2.h"		 		
#include "dbmain.h"
#include <acgs.h>
#include <dbsymtb.h>
#include <afxmenubar.h>
#include "dbmleader.h"
#include <geassign.h>
#include <axlock.h >
#include <AcDbAssocAction.h>


//Arx����������
class ObjectArx
{

	//����
public:
	static HWND GetAutoCADWindow();																																		//���CAD����
	static int SendMessageAutoCAD(CString StrCommand);																													//����CAD����(��ģ̬)
	static CString GetWindowPath();																																		//�������·��
	static bool RemoveEduPlotStamp(AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());														//ȥ��������
	static CString GetInstallationPath();																																//���acad��װ·��


	//����ʵ��
public:
	static AcDbObjectId CreatePoint(AcGePoint3d _PointPos);																														//������
	static AcDbObjectId CreateLine(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint);																								//����ֱ��
	static AcDbObjectId CreateRay(AcGePoint3d _StartPoint, AcGeVector3d _Vector);																								//��������
	static AcDbObjectId CreateRay(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint);																								//��������
	static AcDbObjectId CreateXline(AcGePoint3d _StartPoint, AcGeVector3d _Vector);																								//����������
	static AcDbObjectId CreateXline(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint);																							//����������
	static AcDbObjectId CreateArc(AcGePoint3d _Center, double _Radius, double _StartAngle, double _EndAngle);																	//��Բ��
	static AcDbObjectId CreatePolyline(AcGePoint2dArray _ListPoins);																											//���������
	static AcDbObjectId CreatePolyline(AcGePoint3dArray _ListPoins);																											//���������
	static AcDbObjectId CreateSpline(AcGePoint3dArray _points);																													//������������
	static AcDbObjectIdArray CreateRegion(AcDbObjectId _PolylineId);																											//��������
	static AcDbObjectIdArray CreateRegion(AcDbObjectIdArray _ListObjects);																										//��������
	static AcDbObjectId CreateExtrude(AcDbObjectId _RegionId, double _Height, double _TaperAngle);																				//�����������
	static AcDbObjectId CreateText(AcGePoint3d _PointPos, CString _StrText, double _Height = 3.5, double _Rotation = 0.0, CString _TextStyle = _T(""));							//��������
	static AcDbObjectId CreateMText(AcGePoint3d _PointPos, CString _StrText, double _Height = 3.5, double _Rotation = 0.0, CString _TextStyle = _T(""));						//������������
	static AcDbObjectId CreateLeader(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint, AcGePoint3d _LeadPoint);																	//��������


	//ʵ��
public:
	static bool IsEntity(AcDbObjectId _ObjectId);																																//�ж��Ƿ���ʵ��

	static AcDbObjectId CopyEntity(AcDbObjectId _ObjectId);																														//���ƶ���
	static int DeleteObject(AcDbObjectId _ObjectId);																															//ɾ������
	static int GetObjectColor(AcDbObjectId _ObjectId);																															//��ö�����ɫ
	static CString GetObjectLayer(AcDbObjectId _ObjectId);																														//��ö�������ͼ��
	static AcGePoint2d GetViewportViewCenter(AcDbObjectId _ObjectId);																											//����ӿ���ͼ����
	static double GetViewportViewHeight(AcDbObjectId _ObjectId);																												//����ӿ���ͼ�߶�
	static double GetViewportWidth(AcDbObjectId _ObjectId);																														//����ӿڿ��
	static double GetViewportHeight(AcDbObjectId _ObjectId);																													//����ӿڸ߶�
	static AcDbViewport::StandardScaleType SetViewportStandardScale(AcDbObjectId _ObjectId);																					//����ӿ���ͼ����
	static double GetViewportCustomScale(AcDbObjectId _ObjectId);																												//����ӿ���ͼ�Զ������
	static bool GetObjectShow(AcDbObjectId _ObjectId);																															//��ö�����ʾ��������
	static AcGePoint2d GetTextExtents(CString _String, CString _TextStyleName, double _Size);																					//������ַ�Χ
	static AcGePoint3d GetArcCenter(AcDbObjectId _EntityId);																													//���Բ��Բ��
	static AcGeVector3d GetArcNormal(AcDbObjectId _EntityId);																													//���Բ������
	static bool SetArcNormal(AcDbObjectId _EntityId, AcGeVector3d _Normal);																										//���Բ������
	static double GetArcRadius(AcDbObjectId _EntityId);																															//���Բ���뾶
	static double GetArcStartAngle(AcDbObjectId _EntityId);																														//���Բ����ʼ�Ƕ�
	static double GetArcEndAngle(AcDbObjectId _EntityId);																														//���Բ����ֹ�Ƕ�

	static AcDbObjectIdArray CopyEntity(AcDbObjectIdArray _ListEntitys);																										//���ƶ���
	static AcDbObjectIdArray CreateOffset(AcDbObjectId _ObjectId, double _VarDeviation, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());			//��������ƫ��
	static int DeleteObject(AcDbObjectIdArray _ListObjects);																													//ɾ������
	static int SetObjectColor(AcDbObjectId _ObjectId, int _Color);																												//���ö�����ɫ
	static int SetObjectLayer(AcDbObjectId _ObjectId, CString _Layer);																											//���ö���ͼ��
	static int SetObjectLayer(AcDbObjectIdArray _ListEntitys, CString _Layer);																									//���ö���ͼ��
	static int SetObjectColor(AcDbObjectIdArray _ListObjects, int _Color);																										//���ö�����ɫ
	static void SetViewportViewCenter(AcDbObjectId _ObjectId, AcGePoint2d _Center);																								//�����ӿ���ͼ����
	static void SetViewportViewHeight(AcDbObjectId _ObjectId, double _Height);																									//�����ӿ���ͼ�߶�
	static void SetViewportWidth(AcDbObjectId _ObjectId, double _Width);																										//�����ӿ���ͼ���
	static void SetViewportHeight(AcDbObjectId _ObjectId, double _Height);																										//�����ӿ���ͼ�߶�
	static void SetViewportStandardScale(AcDbObjectId _ObjectId, AcDbViewport::StandardScaleType _Scale);																		//�����ӿ���ͼ����
	static void SetViewportCustomScale(AcDbObjectId _ObjectId, double _Scale);																									//�����ӿ���ͼ�Զ������
	static int SetObjectShow(AcDbObjectId ObjectId, bool BolShow);																												//���ö�����ʾ��������
	static int SetLineStartPoint(AcDbObjectId _LineId, AcGePoint3d _StartPoint);																								//����ֱ�����
	static int SetLineEndPoint(AcDbObjectId _LineId, AcGePoint3d _EndPoint);																									//����ֱ���յ�
	static bool SetXlineBasePoint(AcDbObjectId _XlineId, AcGePoint3d _BasePoint);																								//���ù����߻�׼��
	static void SetHatchPatternScale(AcDbObjectId _HatchId, double _Scale);																										//�������ͼ������
	static void SetLeaderArrowSize(AcDbObjectId _ObjectId, double _Size);																										//�������߼�ͷ�ߴ�
	static void SetLeaderArrowStyle(AcDbObjectId _ObjectId, CString _StyleName);																								//�������߼�ͷ
	static void SetTextPoint(AcDbObjectId _ObjectId, AcGePoint3d _PointPos);																									//�������ֻ���
	static void SetTextHeight(AcDbObjectId _ObjectId, double _Height);																											//�������ָ߶�
	static void SetTextWidth(AcDbObjectId _ObjectId, double _Width);																											//�������ֿ��
	static void SetTextStyle(AcDbObjectId _ObjectId, CString _TextStyle);																										//����������ʽ
	static void SetTextContent(AcDbObjectId _ObjectId, CString _Content);																										//������������

	static AcDbVoidPtrArray ExplodeObject(AcDbObjectId _ObjectId, bool BolDelete = 0);																							//�ֽ�һ������
	static AcDbObjectIdArray ExplodeObjectToModel(AcDbObjectId _ObjectId, bool BolDelete = 0, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());		//�ֽ�һ������

	static AcDbObjectId HandleToObjectId(CString _Handle, AcDbDatabase* _pDataBase = acdbHostApplicationServices()->workingDatabase());											//���ת����
	static CString ObjectIdToHandle(AcDbObjectId _ObjectId);																													//����ת���


	//����
public:
	static CString OpenDialog(CString _StrPrompt = _T("������Ҫѡ����ļ�����"), CString _StrSuffix = _T("dwg"), CString _StrDefaultPath = _T(""));								//���ļ�
	static CString SaveDialog(CString _StrPrompt = _T("������Ҫѡ����ļ�����"), CString _StrSuffix = _T("dwg"), CString _StrDefaultPath = _T(""));								//���Ϊ�Ի���
	static AcDbObjectId SelectObject(CString StrPrompt = _T(""));																												//ѡ��ʵ�����
	static AcDbObjectId SelectObject(CString StrPrompt, AcGePoint3d* _PointPos);																								//ѡ��ʵ�����
	static AcDbObjectIdArray SelectObjects(CString StrPrompt = _T(""));																											//��ѡʵ�巵��ʵ�弯
	static int SelectPoint(CString _StrPrompt, AcGePoint3d* _PointPos);																											//ѡ���
	static int SelectPoint(CString _StrPrompt, AcGePoint3d _PointDatum, AcGePoint3d* _PointPos);																				//ѡ���
	static int SetSelectObject(AcDbObjectId _ObjectId);																															//����ѡ�ж���
	static int SetSelectObject(AcDbObjectIdArray _ListObjects);																													//����ѡ�ж���
	static int CancelSelect();																																					//ȡ��ѡ��
	static AcDbObjectId GetEndEntity();																																			//���ģ�Ϳռ������һ��ʵ��
	static AcDbObjectIdArray GetPointInEntitys(AcGePoint3d _point);																												//��õ��µ�ʵ��


	//�������
public:
	static AcGePoint3d WCS(AcGePoint3d _PoinPos);																																//��������ĵ�תΪ��������ĵ�
	static AcGePoint3d UCS(AcGePoint3d _PoinPos);																																//��������ĵ�תΪ��������ĵ�

	static int MoveObject(AcDbObjectId _ObjectId, AcGePoint3d _PointA, AcGePoint3d _PointB);																					//�㵽���ƶ�
	static int MoveObject(AcDbEntity* _pEnt, AcGePoint3d _PointA, AcGePoint3d _PointB);																							//�㵽���ƶ�
	static int MoveObject(AcDbObjectId _ObjectId, AcGeVector3d _Vector);																										//ָ�������ƶ�����
	static int MoveObject(AcDbEntity* _pEnt, AcGeVector3d _Vector);																												//ָ�������ƶ�����
	static int MoveObject(AcDbObjectIdArray _ListObjects, AcGePoint3d _PointA, AcGePoint3d _PointB);																			//�㵽���ƶ�
	static int MoveObject(AcDbObjectIdArray _ListObjects, AcGeVector3d _Vector);																								//ָ�������ƶ�����
	static int RotateObject(AcDbObjectId _ObjectId, AcGePoint3d _PointPos, double _Angle, AcGeVector3d _Vector = AcGeVector3d(0.0, 0.0, 1.0));									//��ת����
	static int RotateObject(AcDbObjectIdArray _ListEntitys, AcGePoint3d __PointPos, double _Angle, AcGeVector3d _Vector = AcGeVector3d(0.0, 0.0, 1.0));							//��ת����
	static int ScaleEntity(AcDbObjectId _ObjectId, AcGePoint3d _PointPos, double _Var);																							//����ʵ��
	static int ScaleEntity(AcDbObjectIdArray ListEntitys, AcGePoint3d _PointPos, double _Var);																					//����ʵ��
	static void Mirroring(AcDbObjectId _EntityId, AcGePoint3d _StartPoint, AcGePoint3d _EndPoint);																				//����
	static void Mirroring(AcDbObjectIdArray ListEntitys, AcGePoint3d _StartPoint, AcGePoint3d _EndPoint);																		//����


	//�������
public:
	static int GetPolyVertexCount(AcDbObjectId _PolylineId);																											//��ö���ߵ�����
	static int SetPolyAtVertex(AcDbObjectId _PolylineId, int _nIndex, AcGePoint3d _PointPos);																			//���ö���ߵ�
	static int AddPolyVertex(AcDbObjectId _PolylineId, AcGePoint3d _PointPos, int _Index = -1);																			//��Ӷ���ߵĵ�
	static AcGePoint3d GetPolyVertex(AcDbObjectId _PolylineId, int _nIndex);																							//��ö���ߵ�
	static AcGePoint3dArray GetPolyVertex(AcDbObjectId _PolylineId);																									//��ö���ߵ�
	static int RemovePolyAtVertex(AcDbObjectId _PolylineId, int _nIndex);																								//�Ƴ�����ߵ�
	static double GetCurvePointInLength(AcDbObjectId _CurveId, AcGePoint3d _PointA, AcGePoint3d _PointB);																//�������л������ĳ���
	static double GetCurvePointInParam(AcDbObjectId _CurveId, AcGePoint3d _Point);
	static bool IsObjectCurve(AcDbObjectId _ObjectId);																													//�ж϶����Ƿ�������
	static AcGePoint3d GetCurvePoint(AcDbObjectId _CurveId, double _Param);																								//������ߵ�
	static bool GetCurvePoint(AcDbObjectId _CurveId, double _Param, AcGePoint3d& _positoin);																			//������ߵ�
	static double GetCurveLength(AcDbObjectId _CurveId, double _Param);																									//������߳���
	static double GetPointInCurve(AcDbObjectId _CurveId, AcGePoint3d _PointPos);																						//��õ��������ϵ�λ��(��ֵ)
	static bool IsCurveClosed(AcDbObjectId _CurveId);																													//�ж������Ƿ�պ�
	static void SetPolylineClosed(AcDbObjectId _PolylineId, bool _IsClose);																								//���ö�����Ƿ�պ�
	static bool IsPointInCurve(AcDbObjectId _PolylineId, AcGePoint3d _PointPos, double _Tolerance = 0.02);																//�жϵ��Ƿ��ڶ������
	static double GetCurveArea(AcDbObjectId _CurveId);																													//���������
	static int SetPolylineNormal(AcDbObjectId _PolylineId, AcGeVector3d _Normal);																						//���ö���߷���
	static int SetPloylineConstantWidth(AcDbObjectId _ObjectId, double _Width);																							//���ö����ȫ�ֿ��
	static bool IsInCurveInside(AcDbObjectId _targetId, AcDbObjectId _polylineId, double _Tolerance = 0.02);															//�ж�Ŀ�������Ƿ��ڶ������
	static AcDbObjectIdArray splitCurves(AcDbObjectId _entityId, AcDbObjectId _cuttingId);																				//�ָ�����
	static bool RemovePolylineRepeatVertex(AcDbObjectId _polylineId);																									//�Ƴ�������ظ�����


	//���߾������
public:
	static AcDbObjectIdArray CreateObjectArray(AcDbObjectId _ObjectId, int _xCount, double _xDist, int _yCount, double _yDist);											//��������


	//��չ����
public:
	static bool IsExtendData(AcDbEntity* _pEnt, CString _StrName);																												//�жϸö����Ƿ����ָ������չ��¼
	static bool IsExtendData(AcDbObjectId _ObjectId, CString _StrName);																											//�жϸö����Ƿ����ָ������չ��¼

	static int AddExtendData(AcDbObjectId _ObjectId, CString _StrName, struct resbuf* (&pRb));																					//Ϊ��չ�ʵ��������չ����
	static int AddExtendData(AcDbEntity* _pEnt, CString _StrName, struct resbuf* (&pRb));																						//Ϊ��չ�ʵ��������չ����
	static int AddExtendData(AcDbObjectId _ObjectId, CString _StrName, CString _StrText);																						//Ϊ��չ�ʵ��������չ����
	static int AddExtendData(AcDbEntity* _pEnt, CString _StrName, CString _StrText);																							//Ϊ��չ�ʵ��������չ����

	static int GetExtendData(AcDbObjectId _ObjectId, CString _StrName, struct resbuf* (&pRb));																					//�����չ�ʵ��е���չ����
	static int GetExtendData(AcDbEntity* _pEnt, CString _StrName, struct resbuf* (&pRb));																						//�����չ�ʵ��е���չ����
	static int GetExtendData(AcDbObjectId _ObjectId, CString _StrName, CString* _StrText);																						//�����չ�ʵ��е���չ����
	static int GetExtendData(AcDbEntity* _pEnt, CString _StrName, CString* _StrText);																							//�����չ�ʵ��е���չ����

	static int DeleteExtendData(AcDbObjectId _ObjectId, CString _StrName);																										//ɾ����չ����
	static int DeleteExtendData(AcDbEntity* _pEnt, CString _StrName);																											//ɾ����չ����

	static int DeleteDatabaseExtendData(CString _StrName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());											//ɾ��ͼֽ��չ�ʵ��е���չ����

	static int AddDatabaseExtendData(CString _StrName, struct resbuf* (&pRb), AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());						//�����ݿ��������չ�ʵ��������չ����
	static int AddDatabaseExtendData(CString _StrName, CString _StrText, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());							//�����ݿ��������չ�ʵ��������չ����

	static int GetDatabaseExtendData(CString _StrName, struct resbuf* (&pRb), AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());						//������ݿ���ָ�����Ƶ���չ����
	static int GetDatabaseExtendData(CString _StrName, CString* _StrText, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());							//������ݿ���ָ�����Ƶ���չ����


	//ͼ��
public:
	static CString GetCurrentLayer(AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());														//��õ�ǰͼ��
	static bool IsLayer(CString _Layer, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());													//�ж�ͼ���Ƿ����
	static int CreateLayer(CString _Layer, int _Color = 0);																												//����ͼ��
	static int GetDatabaseLayers(CStringArray* _StrLayerArray, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());							//������ݿ�����ͼ��
	static bool IsLayerOff(CString _StrLayer);																															//�ж�ͼ���Ƿ��
	static int SetLayerOff(CString _StrLayer, bool _IsOff);																												//����ͼ���Ƿ��
	static bool IsLayerLock(CString _StrLayer);																															//�ж�ͼ���Ƿ�����
	static int SetLayerLock(CString _StrLayer, bool _BolHide);																											//����ͼ���Ƿ�����
	static bool IsLayerFrozen(CString _StrLayer);																														//�ж�ͼ���Ƿ�����
	static int SetLayerFrozen(CString _StrLayer, bool _BolHide);																										//����ͼ���Ƿ�����
	static int GetLayerColor(CString _LayerName);																														//���ͼ����ɫ
	static int SetLayerColor(CString _LayerName, int _Color);																											//����ͼ����ɫ
	static int SetCurrentLayer(CString _LayerName);																														//���õ�ǰͼ��


	//ͼ��
public:
	static AcDbObjectId CopyDynBlockReference(AcDbObjectId _EntityId);																										//���ƶ�̬��
	static AcDbObjectIdArray CopyDynBlockReference(AcDbObjectIdArray _Entitys);																								//���ƶ�̬��
	static void ResetReference(AcDbObjectId _ObjectId);																														//���ÿ����
	static AcDbObjectIdArray IterateBlockTableRecord(CString _BlockName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());						//��������¼
	static AcDbObjectIdArray IterateBlockTableRecord(AcDbObjectId _BlockTableRecordId = NULL, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());	//��������¼
	static AcDbObjectIdArray IterateBlockTableRecord(AcDbBlockTableRecord* _pBlockTableRecord);																				//��������¼
	static AcDbObjectId GetBlockTableRecord(CString _BlockName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());								//���ָ�����ƵĿ���¼
	static void GetModelSpaceBlockNames(CString _Path, CStringArray& _ListBlockNames);																						//���ģ�Ϳռ�����п���
	static void GetModelSpaceBlockNames(CStringArray& _ListBlockNames, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());						//���ģ�Ϳռ�����п���
	static void GetBlockNames(CString _Path, CStringArray& _ListBlockNames);																								//���ͼֽ�е����п���
	static void GetBlockNames(CStringArray& _ListBlockNames, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());									//���ͼֽ�е����п���
	static AcDbObjectId AddEntityToBlockTableRecord(AcDbEntity* _pEntiy, AcDbObjectId _BlockTableRecordId = NULL, AcDbDatabase* _pDatabase = NULL);							//���ʵ�嵽����¼
	static AcDbObjectIdArray AddEntityToBlockTableRecord(AcDbObjectIdArray _ListEntitys, AcDbObjectId _BlockTableRecordId = NULL, AcDbDatabase* _pDatabase = NULL);			//���ʵ�嵽����¼
	static void DeleteBlockTableRecord(CString _BlockName);																													//ɾ������¼

	static AcDbObjectId CreateBlockTableRecord(CString _StrName = _T(""));																								//��������¼
	static CString GetBlockName(AcDbBlockReference* _pBlockReference);																									//��ÿ�����
	static CString GetBlockName(AcDbObjectId _ObjectId);																												//��ÿ�����
	static CString GetBlockTableRecordName(AcDbObjectId _ObjectId);																										//��ÿ���¼����
	static AcGeScale3d GetBlockScale(AcDbObjectId _ObjectId);																											//�������
	static int SetBlockName(CString _StrOldName, CString _StrNewName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());						//�޸Ŀ�����
	static AcGePoint3d GetBlockPoint(AcDbObjectId _BlockId);																											//���ͼ��Ļ���
	static void SetBlockPoint(AcDbObjectId _BlockId, AcGePoint3d _Point);																								//����ͼ��Ļ���
	static double GetBlockAngle(AcDbObjectId _BlockId);																													//���ͼ��ĽǶ�
	static void SetBlockAngle(AcDbObjectId _BlockId, double _Angle);																									//����ͼ��ĽǶ�
	static void SetBlockScale(AcDbObjectId _ObjectId, AcGeScale3d _Scale);																								//���ÿ�����
	static AcGeVector3d GetBlockNormal(AcDbObjectId _BlockId);																											//���ͼ�鷨��

	static bool IsDynBolck(AcDbObjectId _ObjectId);																														//�ж�һ�������Ƿ��Ƕ�̬��

	static bool IsAttrBolck(AcDbObjectId _ObjectId);																													//�ж�һ�������Ƿ������Կ�

	//����/�ӿ�
public:
	static CString GetCurrentLayout();																																				//��õ�ǰ����
	static int SetCurrentLayout(CString _StrName);																																	//���õ�ǰͼֽ����
	static void CreateLayout(CString _StrLayoutName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());													//��������
	static void GetDatabaseLayouts(CStringArray* pListLayoutNames, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());									//������в�������
	static CString GetLayoutBlockName(CString _StrLayoutName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());											//��ò��ֵĿ���
	static AcDbObjectId GetLayoutBlockTableRecord(CString _StrLayoutName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());								//��ò��ֵĿ���¼
	static void GetViewportFrozenLayer(AcDbObjectId _ViewPortId, CStringArray* _pListLayoutNames);																					//����ӿڶ���ͼ��
	static void SetViewportFrozenLayer(AcDbViewport* _pViewport, CStringArray& ListLayoutNames, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());		//�����ӿڶ���ͼ��
	static void SetViewportFrozenLayer(AcDbObjectId _ViewPortId, CStringArray& ListLayoutNames, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());		//�����ӿڶ���ͼ��
	static void SetViewportLocked(AcDbObjectId _ViewPortId, bool _IsLocked);																										//�����ӿ�����
	static int DeleteLayout(CString _StrName);																																		//ɾ������
	static AcDbViewTableRecord* GetCurrentView();																																	//��õ�ǰ�ӿ�
	static CString GetPlotCfgName(CString _LayoutName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());												//��ò����д�ӡpc3�ļ�
	static bool GetShowPlotStyles(CString _LayoutName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());												//��ò������Ƿ���ʾ��ӡ��ʽ
	static void SetShowPlotStyles(CString _LayoutName, bool _IsShowPlotStyles, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());						//���ò������Ƿ���ʾ��ӡ��ʽ


	//��ע/����/����
public:
	static bool IsTextStyle(CString _StrStyle);																																	//�ж�������ʽ�Ƿ����
	static CString GetText(AcDbObjectId _ObjectId);																																//�����������
	static CString GetMTextContents(AcDbObjectId _ObjectId);																													//�����������
	static bool IsDimensionStyle(CString _StrStyle);																															//�жϱ�ע��ʽ�Ƿ����
	static AcDbObjectId GetDimensionStyleId(CString _StrStyle);																													//ͨ��������ʽ�����ʽId
	static CString GetDimensionStyleName(AcDbObjectId _StyleId);																												//ͨ��������ʽId�����ʽ��
	static AcDbObjectId GetTextStyleId(CString _StrStyle);																														//ͨ��������ʽ�����ʽId
	static AcDbObjectId GetTextStyle(AcDbObjectId _ObjectId);																													//���������ʽ
	static CString GetTextStyleName(AcDbObjectId _StyleId);																														//ͨ��������ʽId�����ʽ��
	static AcGePoint3d GetTextSize(CString _StrText, CString _StrFont, double _Size);																							//������ֳߴ�
	static double GetTextHeight(AcDbObjectId _ObjectId);																														//������ֳߴ�
	static double GetTextWidth(AcDbObjectId _ObjectId);																															//������ֿ��
	static AcGePoint3d GetTextPoint(AcDbObjectId _ObjectId);																													//������ֻ���


	static int CreateTextStyle(CString _StrName, int _Color = 0, CString _StrFont = _T("����"), CString _StrBigFont = _T(""), double _FontHeight = 3.5,
		double _XScale = 1, double _ValueObliquingAngle = 0.0, bool _BolVertical = 0);																							//����������ʽ
	static int CreateDimensionStyle(CString _StrName, CString _StrTextStyle = _T(""), double _HeightText = 3.5, int _ColorDimText = 0, int _VarDimMove = 0,
		CString _StrTypeArrow = _T(""), double _VarArrow = 1.75, bool _BolTextInExtlineUp = 0, double _VarTextToDimline = 0.35, double _VarExtlineOffOriginDist = 3.5,
		int _VarDimJust = 0, double _VarExcDimline = 1.75, double  _VarScale = 1.0, int _ColorDimline = 0, int ColorExtline = 0, int _VarDimdec = 0,
		double _VarTextOffDimline = 3.5);																																		//����������ʽ

	static int SetDimensionStyle(AcDbObjectId _ObjectId, CString _StrStyle);																									//���ñ�ע��ʽ
	static int SetDimensionArrowSize(AcDbObjectId _ObjectId, double _Size);																										//���ñ�ע��ͷ�ߴ�
	static int SetDimensionTextHeight(AcDbObjectId _ObjectId, double _Height);																									//���óߴ����ָ߶�
	static int SetDimensionExcDimline(AcDbObjectId _ObjectId, double _Var);																										//ָ���ߴ���߳����ߴ��ߵľ���

	static int SetLeaderPoint(AcDbObjectId _ObjectId, AcGePoint3d _PointPos, int _nInde = 0);																					//�������ߵ�
	static AcGePoint3d GetLeaderPoint(AcDbObjectId _ObjectId, int _nInde = 0);																									//������ߵ�
	static int SetMLeaderEnableDogleg(AcDbObjectId _MLeaderId, bool _BolDogleg = 0);																							//���ö��������Ƿ���������

	static AcDbObjectId CreateAligDimension(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint, AcGePoint3d _PointPos, CString _StrStyle = _T(""), CString _StrText = _T(""));		//���������ע
	static AcDbObjectId CreateLinearDimension(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint, AcGePoint3d _PointPos, double _Angle, CString _StrStyle = _T(""),
		CString _StrText = _T(""));																																				//�������Ա�ע


	//����
public:
	static AcDbObjectIdArray GetInterObject(AcDbObjectId _ObjectId, AcDbObjectIdArray _ListObjects, AcDb::Intersect _Mode = AcDb::kOnBothOperands);						//����ཻ����
	static AcGePoint3dArray GetInterPoint(AcDbObjectId _ObjectId, AcDbObjectIdArray _ListObjects, AcDb::Intersect _Mode = AcDb::kOnBothOperands);						//����ཻ��
	static AcGePoint3dArray GetInterPoint(AcDbObjectId _ObjectAId, AcDbObjectId _ObjectBId, AcDb::Intersect _Mode = AcDb::kOnBothOperands);								//����ཻ��
	static int GetEntityExtents(AcDbObjectId _ObjectAId, AcGePoint3d* _MinPoint, AcGePoint3d* _MaxPoint);																//��ö�������
	static int GetEntityExtents(AcDbObjectIdArray _ListObjects, AcGePoint3d* _MinPoint, AcGePoint3d* _MaxPoint);														//��ö�������
	static bool IsPointInCurve(AcGePoint3d _PointPos, AcDbObjectId _CurveId, double _Tolerance = 0.02);																	//�жϵ��Ƿ���������
	static bool IsPointInCurve(AcGePoint3d _PointPos, AcDbObjectIdArray _ListCurves, double _Tolerance = 0.02);															//�жϵ��Ƿ���������
	static double GetPointToCurveDist(AcGePoint3d _PointPos, AcDbObjectId _CurveId);																					//��õ㵽���ߵ���С����
	static AcGePoint3d GetPointToCurveMidPoint(AcGePoint3d _PointPos, AcDbObjectId _CurveId);																			//��õ㵽���ߵ������
	static AcGePoint3dArray GetObjectInter(AcDbObjectId _ObjectA, AcDbObjectId _ObjectB, AcDb::Intersect _Type = AcDb::kOnBothOperands);								//�����������Ľ���

	//���ݿ�
public:
	static AcDbObjectId AddObjectToCurSpace(AcDbEntity* _pEnt, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());								//���ʵ�嵽��ǰ����
	static AcDbObjectId AddObjectToModl(AcDbEntity* _pEnt, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());								//���ʵ�嵽ģ�Ϳռ�
	static AcDbObjectIdArray AddObjectToModl(AcDbVoidPtrArray* _pEnts, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());					//���ʵ�嵽ģ�Ϳռ�
	static AcDbObjectId AddBlockToModl(CString _StrBlockName, AcGePoint3d _PointDatum = AcGePoint3d(0.0, 0.0, 0.0));													//��ӿ鵽ģ�Ϳռ�
	static AcDbObjectId AddBlockToModl(AcDbObjectId _BlockTableRecordId, AcGePoint3d _PointDatum = AcGePoint3d(0.0, 0.0, 0.0));											//��ӿ鵽ģ�Ϳռ�
	static int ImportBlockTableRecord(CString _StrPath, CString _StrBlockName);																							//�������¼
	static int ImportBlockTableRecord(AcDbDatabase* _pDatabase, CString _StrBlockName);																					//�������¼
	static AcDbObjectId ImportBlockReference(CString _StrPath, CString _StrBlockName);																					//��������

	//����
public:
	static AcGePoint3d GetVerticalPoint(AcGePoint3d _PointOver, AcGePoint3d _PointLineA, AcGePoint3d _PointLineB);														//��õ㵽�ߵĴ���
};
