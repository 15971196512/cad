#pragma once
using namespace std;
#include "arxHeaders.h"								//ObjectARX总头文件以及lib引用
#include "rxmfcapi.h"								//一些API
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


//Arx函数操作类
class ObjectArx
{

	//其他
public:
	static HWND GetAutoCADWindow();																																		//获得CAD窗口
	static int SendMessageAutoCAD(CString StrCommand);																													//发送CAD命令(非模态)
	static CString GetWindowPath();																																		//获得桌面路径
	static bool RemoveEduPlotStamp(AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());														//去除教育版
	static CString GetInstallationPath();																																//获得acad安装路径


	//创建实体
public:
	static AcDbObjectId CreatePoint(AcGePoint3d _PointPos);																														//创建点
	static AcDbObjectId CreateLine(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint);																								//创建直线
	static AcDbObjectId CreateRay(AcGePoint3d _StartPoint, AcGeVector3d _Vector);																								//创建射线
	static AcDbObjectId CreateRay(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint);																								//创建射线
	static AcDbObjectId CreateXline(AcGePoint3d _StartPoint, AcGeVector3d _Vector);																								//创建构造线
	static AcDbObjectId CreateXline(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint);																							//创建构造线
	static AcDbObjectId CreateArc(AcGePoint3d _Center, double _Radius, double _StartAngle, double _EndAngle);																	//画圆弧
	static AcDbObjectId CreatePolyline(AcGePoint2dArray _ListPoins);																											//创建多段线
	static AcDbObjectId CreatePolyline(AcGePoint3dArray _ListPoins);																											//创建多段线
	static AcDbObjectId CreateSpline(AcGePoint3dArray _points);																													//创建样条曲线
	static AcDbObjectIdArray CreateRegion(AcDbObjectId _PolylineId);																											//创建面域
	static AcDbObjectIdArray CreateRegion(AcDbObjectIdArray _ListObjects);																										//创建面域
	static AcDbObjectId CreateExtrude(AcDbObjectId _RegionId, double _Height, double _TaperAngle);																				//创建拉伸对象
	static AcDbObjectId CreateText(AcGePoint3d _PointPos, CString _StrText, double _Height = 3.5, double _Rotation = 0.0, CString _TextStyle = _T(""));							//创建文字
	static AcDbObjectId CreateMText(AcGePoint3d _PointPos, CString _StrText, double _Height = 3.5, double _Rotation = 0.0, CString _TextStyle = _T(""));						//创建多行文字
	static AcDbObjectId CreateLeader(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint, AcGePoint3d _LeadPoint);																	//创建引线


	//实体
public:
	static bool IsEntity(AcDbObjectId _ObjectId);																																//判断是否是实体

	static AcDbObjectId CopyEntity(AcDbObjectId _ObjectId);																														//复制对象
	static int DeleteObject(AcDbObjectId _ObjectId);																															//删除对象
	static int GetObjectColor(AcDbObjectId _ObjectId);																															//获得对象颜色
	static CString GetObjectLayer(AcDbObjectId _ObjectId);																														//获得对象所在图层
	static AcGePoint2d GetViewportViewCenter(AcDbObjectId _ObjectId);																											//获得视口视图中心
	static double GetViewportViewHeight(AcDbObjectId _ObjectId);																												//获得视口视图高度
	static double GetViewportWidth(AcDbObjectId _ObjectId);																														//获得视口宽度
	static double GetViewportHeight(AcDbObjectId _ObjectId);																													//获得视口高度
	static AcDbViewport::StandardScaleType SetViewportStandardScale(AcDbObjectId _ObjectId);																					//获得视口视图比例
	static double GetViewportCustomScale(AcDbObjectId _ObjectId);																												//获得视口视图自定义比例
	static bool GetObjectShow(AcDbObjectId _ObjectId);																															//获得对象显示或者隐藏
	static AcGePoint2d GetTextExtents(CString _String, CString _TextStyleName, double _Size);																					//获得文字范围
	static AcGePoint3d GetArcCenter(AcDbObjectId _EntityId);																													//获得圆弧圆心
	static AcGeVector3d GetArcNormal(AcDbObjectId _EntityId);																													//获得圆弧法线
	static bool SetArcNormal(AcDbObjectId _EntityId, AcGeVector3d _Normal);																										//获得圆弧法线
	static double GetArcRadius(AcDbObjectId _EntityId);																															//获得圆弧半径
	static double GetArcStartAngle(AcDbObjectId _EntityId);																														//获得圆弧起始角度
	static double GetArcEndAngle(AcDbObjectId _EntityId);																														//获得圆弧终止角度

	static AcDbObjectIdArray CopyEntity(AcDbObjectIdArray _ListEntitys);																										//复制对象
	static AcDbObjectIdArray CreateOffset(AcDbObjectId _ObjectId, double _VarDeviation, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());			//创建对象偏移
	static int DeleteObject(AcDbObjectIdArray _ListObjects);																													//删除对象
	static int SetObjectColor(AcDbObjectId _ObjectId, int _Color);																												//设置对象颜色
	static int SetObjectLayer(AcDbObjectId _ObjectId, CString _Layer);																											//设置对象图层
	static int SetObjectLayer(AcDbObjectIdArray _ListEntitys, CString _Layer);																									//设置对象图层
	static int SetObjectColor(AcDbObjectIdArray _ListObjects, int _Color);																										//设置对象颜色
	static void SetViewportViewCenter(AcDbObjectId _ObjectId, AcGePoint2d _Center);																								//设置视口视图中心
	static void SetViewportViewHeight(AcDbObjectId _ObjectId, double _Height);																									//设置视口视图高度
	static void SetViewportWidth(AcDbObjectId _ObjectId, double _Width);																										//设置视口视图宽度
	static void SetViewportHeight(AcDbObjectId _ObjectId, double _Height);																										//设置视口视图高度
	static void SetViewportStandardScale(AcDbObjectId _ObjectId, AcDbViewport::StandardScaleType _Scale);																		//设置视口视图比例
	static void SetViewportCustomScale(AcDbObjectId _ObjectId, double _Scale);																									//设置视口视图自定义比例
	static int SetObjectShow(AcDbObjectId ObjectId, bool BolShow);																												//设置对象显示或者隐藏
	static int SetLineStartPoint(AcDbObjectId _LineId, AcGePoint3d _StartPoint);																								//设置直线起点
	static int SetLineEndPoint(AcDbObjectId _LineId, AcGePoint3d _EndPoint);																									//设置直线终点
	static bool SetXlineBasePoint(AcDbObjectId _XlineId, AcGePoint3d _BasePoint);																								//设置构造线基准点
	static void SetHatchPatternScale(AcDbObjectId _HatchId, double _Scale);																										//设置填充图案比例
	static void SetLeaderArrowSize(AcDbObjectId _ObjectId, double _Size);																										//设置引线箭头尺寸
	static void SetLeaderArrowStyle(AcDbObjectId _ObjectId, CString _StyleName);																								//设置引线箭头
	static void SetTextPoint(AcDbObjectId _ObjectId, AcGePoint3d _PointPos);																									//设置文字基点
	static void SetTextHeight(AcDbObjectId _ObjectId, double _Height);																											//设置文字高度
	static void SetTextWidth(AcDbObjectId _ObjectId, double _Width);																											//设置文字宽度
	static void SetTextStyle(AcDbObjectId _ObjectId, CString _TextStyle);																										//设置文字样式
	static void SetTextContent(AcDbObjectId _ObjectId, CString _Content);																										//设置文字内容

	static AcDbVoidPtrArray ExplodeObject(AcDbObjectId _ObjectId, bool BolDelete = 0);																							//分解一个对象
	static AcDbObjectIdArray ExplodeObjectToModel(AcDbObjectId _ObjectId, bool BolDelete = 0, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());		//分解一个对象

	static AcDbObjectId HandleToObjectId(CString _Handle, AcDbDatabase* _pDataBase = acdbHostApplicationServices()->workingDatabase());											//句柄转对象
	static CString ObjectIdToHandle(AcDbObjectId _ObjectId);																													//对象转句柄


	//交互
public:
	static CString OpenDialog(CString _StrPrompt = _T("请输入要选择的文件名称"), CString _StrSuffix = _T("dwg"), CString _StrDefaultPath = _T(""));								//打开文件
	static CString SaveDialog(CString _StrPrompt = _T("请输入要选择的文件名称"), CString _StrSuffix = _T("dwg"), CString _StrDefaultPath = _T(""));								//另存为对话框
	static AcDbObjectId SelectObject(CString StrPrompt = _T(""));																												//选择实体对象
	static AcDbObjectId SelectObject(CString StrPrompt, AcGePoint3d* _PointPos);																								//选择实体对象
	static AcDbObjectIdArray SelectObjects(CString StrPrompt = _T(""));																											//框选实体返回实体集
	static int SelectPoint(CString _StrPrompt, AcGePoint3d* _PointPos);																											//选择点
	static int SelectPoint(CString _StrPrompt, AcGePoint3d _PointDatum, AcGePoint3d* _PointPos);																				//选择点
	static int SetSelectObject(AcDbObjectId _ObjectId);																															//设置选中对象
	static int SetSelectObject(AcDbObjectIdArray _ListObjects);																													//设置选中对象
	static int CancelSelect();																																					//取消选择
	static AcDbObjectId GetEndEntity();																																			//获得模型空间中最后一个实体
	static AcDbObjectIdArray GetPointInEntitys(AcGePoint3d _point);																												//获得点下的实体


	//坐标矩阵
public:
	static AcGePoint3d WCS(AcGePoint3d _PoinPos);																																//绝对坐标的点转为工作坐标的点
	static AcGePoint3d UCS(AcGePoint3d _PoinPos);																																//工作坐标的点转为绝对坐标的点

	static int MoveObject(AcDbObjectId _ObjectId, AcGePoint3d _PointA, AcGePoint3d _PointB);																					//点到点移动
	static int MoveObject(AcDbEntity* _pEnt, AcGePoint3d _PointA, AcGePoint3d _PointB);																							//点到点移动
	static int MoveObject(AcDbObjectId _ObjectId, AcGeVector3d _Vector);																										//指定矩阵移动对象
	static int MoveObject(AcDbEntity* _pEnt, AcGeVector3d _Vector);																												//指定矩阵移动对象
	static int MoveObject(AcDbObjectIdArray _ListObjects, AcGePoint3d _PointA, AcGePoint3d _PointB);																			//点到点移动
	static int MoveObject(AcDbObjectIdArray _ListObjects, AcGeVector3d _Vector);																								//指定矩阵移动对象
	static int RotateObject(AcDbObjectId _ObjectId, AcGePoint3d _PointPos, double _Angle, AcGeVector3d _Vector = AcGeVector3d(0.0, 0.0, 1.0));									//旋转对象
	static int RotateObject(AcDbObjectIdArray _ListEntitys, AcGePoint3d __PointPos, double _Angle, AcGeVector3d _Vector = AcGeVector3d(0.0, 0.0, 1.0));							//旋转对象
	static int ScaleEntity(AcDbObjectId _ObjectId, AcGePoint3d _PointPos, double _Var);																							//缩放实体
	static int ScaleEntity(AcDbObjectIdArray ListEntitys, AcGePoint3d _PointPos, double _Var);																					//缩放实体
	static void Mirroring(AcDbObjectId _EntityId, AcGePoint3d _StartPoint, AcGePoint3d _EndPoint);																				//镜像
	static void Mirroring(AcDbObjectIdArray ListEntitys, AcGePoint3d _StartPoint, AcGePoint3d _EndPoint);																		//镜像


	//曲线相关
public:
	static int GetPolyVertexCount(AcDbObjectId _PolylineId);																											//获得多段线点数量
	static int SetPolyAtVertex(AcDbObjectId _PolylineId, int _nIndex, AcGePoint3d _PointPos);																			//设置多段线点
	static int AddPolyVertex(AcDbObjectId _PolylineId, AcGePoint3d _PointPos, int _Index = -1);																			//添加多段线的点
	static AcGePoint3d GetPolyVertex(AcDbObjectId _PolylineId, int _nIndex);																							//获得多段线点
	static AcGePoint3dArray GetPolyVertex(AcDbObjectId _PolylineId);																									//获得多段线点
	static int RemovePolyAtVertex(AcDbObjectId _PolylineId, int _nIndex);																								//移除多段线点
	static double GetCurvePointInLength(AcDbObjectId _CurveId, AcGePoint3d _PointA, AcGePoint3d _PointB);																//在曲线中获得两点的长度
	static double GetCurvePointInParam(AcDbObjectId _CurveId, AcGePoint3d _Point);
	static bool IsObjectCurve(AcDbObjectId _ObjectId);																													//判断对象是否是曲线
	static AcGePoint3d GetCurvePoint(AcDbObjectId _CurveId, double _Param);																								//获得曲线点
	static bool GetCurvePoint(AcDbObjectId _CurveId, double _Param, AcGePoint3d& _positoin);																			//获得曲线点
	static double GetCurveLength(AcDbObjectId _CurveId, double _Param);																									//获得曲线长度
	static double GetPointInCurve(AcDbObjectId _CurveId, AcGePoint3d _PointPos);																						//获得点在曲线上的位置(比值)
	static bool IsCurveClosed(AcDbObjectId _CurveId);																													//判断曲线是否闭合
	static void SetPolylineClosed(AcDbObjectId _PolylineId, bool _IsClose);																								//设置多段线是否闭合
	static bool IsPointInCurve(AcDbObjectId _PolylineId, AcGePoint3d _PointPos, double _Tolerance = 0.02);																//判断点是否在多段线内
	static double GetCurveArea(AcDbObjectId _CurveId);																													//求曲线面积
	static int SetPolylineNormal(AcDbObjectId _PolylineId, AcGeVector3d _Normal);																						//设置多段线法线
	static int SetPloylineConstantWidth(AcDbObjectId _ObjectId, double _Width);																							//设置多段线全局宽度
	static bool IsInCurveInside(AcDbObjectId _targetId, AcDbObjectId _polylineId, double _Tolerance = 0.02);															//判断目标曲线是否在多段线内
	static AcDbObjectIdArray splitCurves(AcDbObjectId _entityId, AcDbObjectId _cuttingId);																				//分割曲线
	static bool RemovePolylineRepeatVertex(AcDbObjectId _polylineId);																									//移除多段线重复顶点


	//曲线距阵相关
public:
	static AcDbObjectIdArray CreateObjectArray(AcDbObjectId _ObjectId, int _xCount, double _xDist, int _yCount, double _yDist);											//创建阵列


	//扩展属性
public:
	static bool IsExtendData(AcDbEntity* _pEnt, CString _StrName);																												//判断该对象是否存在指定的扩展记录
	static bool IsExtendData(AcDbObjectId _ObjectId, CString _StrName);																											//判断该对象是否存在指定的扩展记录

	static int AddExtendData(AcDbObjectId _ObjectId, CString _StrName, struct resbuf* (&pRb));																					//为扩展词典中添加扩展数据
	static int AddExtendData(AcDbEntity* _pEnt, CString _StrName, struct resbuf* (&pRb));																						//为扩展词典中添加扩展数据
	static int AddExtendData(AcDbObjectId _ObjectId, CString _StrName, CString _StrText);																						//为扩展词典中添加扩展数据
	static int AddExtendData(AcDbEntity* _pEnt, CString _StrName, CString _StrText);																							//为扩展词典中添加扩展数据

	static int GetExtendData(AcDbObjectId _ObjectId, CString _StrName, struct resbuf* (&pRb));																					//获得扩展词典中的扩展数据
	static int GetExtendData(AcDbEntity* _pEnt, CString _StrName, struct resbuf* (&pRb));																						//获得扩展词典中的扩展数据
	static int GetExtendData(AcDbObjectId _ObjectId, CString _StrName, CString* _StrText);																						//获得扩展词典中的扩展数据
	static int GetExtendData(AcDbEntity* _pEnt, CString _StrName, CString* _StrText);																							//获得扩展词典中的扩展数据

	static int DeleteExtendData(AcDbObjectId _ObjectId, CString _StrName);																										//删除扩展数据
	static int DeleteExtendData(AcDbEntity* _pEnt, CString _StrName);																											//删除扩展数据

	static int DeleteDatabaseExtendData(CString _StrName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());											//删除图纸扩展词典中的扩展数据

	static int AddDatabaseExtendData(CString _StrName, struct resbuf* (&pRb), AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());						//在数据库中添加扩展词典中添加扩展数据
	static int AddDatabaseExtendData(CString _StrName, CString _StrText, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());							//在数据库中添加扩展词典中添加扩展数据

	static int GetDatabaseExtendData(CString _StrName, struct resbuf* (&pRb), AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());						//获得数据库中指定名称的扩展数据
	static int GetDatabaseExtendData(CString _StrName, CString* _StrText, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());							//获得数据库中指定名称的扩展数据


	//图层
public:
	static CString GetCurrentLayer(AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());														//获得当前图层
	static bool IsLayer(CString _Layer, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());													//判断图层是否存在
	static int CreateLayer(CString _Layer, int _Color = 0);																												//创建图层
	static int GetDatabaseLayers(CStringArray* _StrLayerArray, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());							//获得数据库所有图层
	static bool IsLayerOff(CString _StrLayer);																															//判断图层是否打开
	static int SetLayerOff(CString _StrLayer, bool _IsOff);																												//设置图层是否打开
	static bool IsLayerLock(CString _StrLayer);																															//判断图层是否锁定
	static int SetLayerLock(CString _StrLayer, bool _BolHide);																											//设置图层是否锁定
	static bool IsLayerFrozen(CString _StrLayer);																														//判断图层是否隐藏
	static int SetLayerFrozen(CString _StrLayer, bool _BolHide);																										//设置图层是否隐藏
	static int GetLayerColor(CString _LayerName);																														//获得图层颜色
	static int SetLayerColor(CString _LayerName, int _Color);																											//设置图层颜色
	static int SetCurrentLayer(CString _LayerName);																														//设置当前图层


	//图块
public:
	static AcDbObjectId CopyDynBlockReference(AcDbObjectId _EntityId);																										//复制动态块
	static AcDbObjectIdArray CopyDynBlockReference(AcDbObjectIdArray _Entitys);																								//复制动态块
	static void ResetReference(AcDbObjectId _ObjectId);																														//重置块参照
	static AcDbObjectIdArray IterateBlockTableRecord(CString _BlockName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());						//遍历块表记录
	static AcDbObjectIdArray IterateBlockTableRecord(AcDbObjectId _BlockTableRecordId = NULL, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());	//遍历块表记录
	static AcDbObjectIdArray IterateBlockTableRecord(AcDbBlockTableRecord* _pBlockTableRecord);																				//遍历块表记录
	static AcDbObjectId GetBlockTableRecord(CString _BlockName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());								//获得指定名称的块表记录
	static void GetModelSpaceBlockNames(CString _Path, CStringArray& _ListBlockNames);																						//获得模型空间的所有块名
	static void GetModelSpaceBlockNames(CStringArray& _ListBlockNames, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());						//获得模型空间的所有块名
	static void GetBlockNames(CString _Path, CStringArray& _ListBlockNames);																								//获得图纸中的所有块名
	static void GetBlockNames(CStringArray& _ListBlockNames, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());									//获得图纸中的所有块名
	static AcDbObjectId AddEntityToBlockTableRecord(AcDbEntity* _pEntiy, AcDbObjectId _BlockTableRecordId = NULL, AcDbDatabase* _pDatabase = NULL);							//添加实体到块表记录
	static AcDbObjectIdArray AddEntityToBlockTableRecord(AcDbObjectIdArray _ListEntitys, AcDbObjectId _BlockTableRecordId = NULL, AcDbDatabase* _pDatabase = NULL);			//添加实体到块表记录
	static void DeleteBlockTableRecord(CString _BlockName);																													//删除块表记录

	static AcDbObjectId CreateBlockTableRecord(CString _StrName = _T(""));																								//创建块表记录
	static CString GetBlockName(AcDbBlockReference* _pBlockReference);																									//获得块名称
	static CString GetBlockName(AcDbObjectId _ObjectId);																												//获得块名称
	static CString GetBlockTableRecordName(AcDbObjectId _ObjectId);																										//获得块表记录名称
	static AcGeScale3d GetBlockScale(AcDbObjectId _ObjectId);																											//获得缩放
	static int SetBlockName(CString _StrOldName, CString _StrNewName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());						//修改块名称
	static AcGePoint3d GetBlockPoint(AcDbObjectId _BlockId);																											//获得图块的基点
	static void SetBlockPoint(AcDbObjectId _BlockId, AcGePoint3d _Point);																								//设置图块的基点
	static double GetBlockAngle(AcDbObjectId _BlockId);																													//获得图块的角度
	static void SetBlockAngle(AcDbObjectId _BlockId, double _Angle);																									//设置图块的角度
	static void SetBlockScale(AcDbObjectId _ObjectId, AcGeScale3d _Scale);																								//设置块缩放
	static AcGeVector3d GetBlockNormal(AcDbObjectId _BlockId);																											//获得图块法线

	static bool IsDynBolck(AcDbObjectId _ObjectId);																														//判断一个对象是否是动态块

	static bool IsAttrBolck(AcDbObjectId _ObjectId);																													//判断一个对象是否是属性块

	//布局/视口
public:
	static CString GetCurrentLayout();																																				//获得当前布局
	static int SetCurrentLayout(CString _StrName);																																	//设置当前图纸布局
	static void CreateLayout(CString _StrLayoutName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());													//创建布局
	static void GetDatabaseLayouts(CStringArray* pListLayoutNames, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());									//获得所有布局名称
	static CString GetLayoutBlockName(CString _StrLayoutName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());											//获得布局的块名
	static AcDbObjectId GetLayoutBlockTableRecord(CString _StrLayoutName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());								//获得布局的块表记录
	static void GetViewportFrozenLayer(AcDbObjectId _ViewPortId, CStringArray* _pListLayoutNames);																					//获得视口冻结图层
	static void SetViewportFrozenLayer(AcDbViewport* _pViewport, CStringArray& ListLayoutNames, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());		//设置视口冻结图层
	static void SetViewportFrozenLayer(AcDbObjectId _ViewPortId, CStringArray& ListLayoutNames, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());		//设置视口冻结图层
	static void SetViewportLocked(AcDbObjectId _ViewPortId, bool _IsLocked);																										//设置视口锁定
	static int DeleteLayout(CString _StrName);																																		//删除布局
	static AcDbViewTableRecord* GetCurrentView();																																	//获得当前视口
	static CString GetPlotCfgName(CString _LayoutName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());												//获得布局中打印pc3文件
	static bool GetShowPlotStyles(CString _LayoutName, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());												//获得布局中是否显示打印样式
	static void SetShowPlotStyles(CString _LayoutName, bool _IsShowPlotStyles, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());						//设置布局中是否显示打印样式


	//标注/文字/引线
public:
	static bool IsTextStyle(CString _StrStyle);																																	//判断文字样式是否存在
	static CString GetText(AcDbObjectId _ObjectId);																																//获得文字内容
	static CString GetMTextContents(AcDbObjectId _ObjectId);																													//获得文字内容
	static bool IsDimensionStyle(CString _StrStyle);																															//判断标注样式是否存在
	static AcDbObjectId GetDimensionStyleId(CString _StrStyle);																													//通过引线样式获得样式Id
	static CString GetDimensionStyleName(AcDbObjectId _StyleId);																												//通过引线样式Id获得样式名
	static AcDbObjectId GetTextStyleId(CString _StrStyle);																														//通过文字样式获得样式Id
	static AcDbObjectId GetTextStyle(AcDbObjectId _ObjectId);																													//获得文字样式
	static CString GetTextStyleName(AcDbObjectId _StyleId);																														//通过文字样式Id获得样式名
	static AcGePoint3d GetTextSize(CString _StrText, CString _StrFont, double _Size);																							//获得文字尺寸
	static double GetTextHeight(AcDbObjectId _ObjectId);																														//获得文字尺寸
	static double GetTextWidth(AcDbObjectId _ObjectId);																															//获得文字宽度
	static AcGePoint3d GetTextPoint(AcDbObjectId _ObjectId);																													//获得文字基点


	static int CreateTextStyle(CString _StrName, int _Color = 0, CString _StrFont = _T("宋体"), CString _StrBigFont = _T(""), double _FontHeight = 3.5,
		double _XScale = 1, double _ValueObliquingAngle = 0.0, bool _BolVertical = 0);																							//创建文字样式
	static int CreateDimensionStyle(CString _StrName, CString _StrTextStyle = _T(""), double _HeightText = 3.5, int _ColorDimText = 0, int _VarDimMove = 0,
		CString _StrTypeArrow = _T(""), double _VarArrow = 1.75, bool _BolTextInExtlineUp = 0, double _VarTextToDimline = 0.35, double _VarExtlineOffOriginDist = 3.5,
		int _VarDimJust = 0, double _VarExcDimline = 1.75, double  _VarScale = 1.0, int _ColorDimline = 0, int ColorExtline = 0, int _VarDimdec = 0,
		double _VarTextOffDimline = 3.5);																																		//创建引线样式

	static int SetDimensionStyle(AcDbObjectId _ObjectId, CString _StrStyle);																									//设置标注样式
	static int SetDimensionArrowSize(AcDbObjectId _ObjectId, double _Size);																										//设置标注箭头尺寸
	static int SetDimensionTextHeight(AcDbObjectId _ObjectId, double _Height);																									//设置尺寸文字高度
	static int SetDimensionExcDimline(AcDbObjectId _ObjectId, double _Var);																										//指定尺寸界线超出尺寸线的距离

	static int SetLeaderPoint(AcDbObjectId _ObjectId, AcGePoint3d _PointPos, int _nInde = 0);																					//设置引线点
	static AcGePoint3d GetLeaderPoint(AcDbObjectId _ObjectId, int _nInde = 0);																									//获得引线点
	static int SetMLeaderEnableDogleg(AcDbObjectId _MLeaderId, bool _BolDogleg = 0);																							//设置多重引线是否启动基线

	static AcDbObjectId CreateAligDimension(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint, AcGePoint3d _PointPos, CString _StrStyle = _T(""), CString _StrText = _T(""));		//创建对齐标注
	static AcDbObjectId CreateLinearDimension(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint, AcGePoint3d _PointPos, double _Angle, CString _StrStyle = _T(""),
		CString _StrText = _T(""));																																				//创建线性标注


	//分析
public:
	static AcDbObjectIdArray GetInterObject(AcDbObjectId _ObjectId, AcDbObjectIdArray _ListObjects, AcDb::Intersect _Mode = AcDb::kOnBothOperands);						//获得相交对象
	static AcGePoint3dArray GetInterPoint(AcDbObjectId _ObjectId, AcDbObjectIdArray _ListObjects, AcDb::Intersect _Mode = AcDb::kOnBothOperands);						//获得相交点
	static AcGePoint3dArray GetInterPoint(AcDbObjectId _ObjectAId, AcDbObjectId _ObjectBId, AcDb::Intersect _Mode = AcDb::kOnBothOperands);								//获得相交点
	static int GetEntityExtents(AcDbObjectId _ObjectAId, AcGePoint3d* _MinPoint, AcGePoint3d* _MaxPoint);																//获得对象轮廓
	static int GetEntityExtents(AcDbObjectIdArray _ListObjects, AcGePoint3d* _MinPoint, AcGePoint3d* _MaxPoint);														//获得对象轮廓
	static bool IsPointInCurve(AcGePoint3d _PointPos, AcDbObjectId _CurveId, double _Tolerance = 0.02);																	//判断点是否在曲线上
	static bool IsPointInCurve(AcGePoint3d _PointPos, AcDbObjectIdArray _ListCurves, double _Tolerance = 0.02);															//判断点是否在曲线上
	static double GetPointToCurveDist(AcGePoint3d _PointPos, AcDbObjectId _CurveId);																					//获得点到曲线的最小距离
	static AcGePoint3d GetPointToCurveMidPoint(AcGePoint3d _PointPos, AcDbObjectId _CurveId);																			//获得点到曲线的最近点
	static AcGePoint3dArray GetObjectInter(AcDbObjectId _ObjectA, AcDbObjectId _ObjectB, AcDb::Intersect _Type = AcDb::kOnBothOperands);								//获得两个对象的交点

	//数据库
public:
	static AcDbObjectId AddObjectToCurSpace(AcDbEntity* _pEnt, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());								//添加实体到当前布局
	static AcDbObjectId AddObjectToModl(AcDbEntity* _pEnt, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());								//添加实体到模型空间
	static AcDbObjectIdArray AddObjectToModl(AcDbVoidPtrArray* _pEnts, AcDbDatabase* _pDatabase = acdbHostApplicationServices()->workingDatabase());					//添加实体到模型空间
	static AcDbObjectId AddBlockToModl(CString _StrBlockName, AcGePoint3d _PointDatum = AcGePoint3d(0.0, 0.0, 0.0));													//添加块到模型空间
	static AcDbObjectId AddBlockToModl(AcDbObjectId _BlockTableRecordId, AcGePoint3d _PointDatum = AcGePoint3d(0.0, 0.0, 0.0));											//添加块到模型空间
	static int ImportBlockTableRecord(CString _StrPath, CString _StrBlockName);																							//导入块表记录
	static int ImportBlockTableRecord(AcDbDatabase* _pDatabase, CString _StrBlockName);																					//导入块表记录
	static AcDbObjectId ImportBlockReference(CString _StrPath, CString _StrBlockName);																					//导入块参照

	//其他
public:
	static AcGePoint3d GetVerticalPoint(AcGePoint3d _PointOver, AcGePoint3d _PointLineA, AcGePoint3d _PointLineB);														//获得点到线的垂点
};
