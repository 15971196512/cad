#pragma once
#include "pch.h"
#include <vector>
#include <algorithm>
#define SIZE_CHAR _T("size")
#define PI 3.1415926

//通过ID获取基点位置
AcGePoint3d GetBlockPosition(AcDbObjectId entityId);

CString GetBlockName(AcDbObjectId entityId);
AcDbObjectIdArray GetEntity_temp(AcDbObjectIdArray& entityIds);
AcDbObjectIdArray _T_GetEntity_temp(AcDbObjectIdArray& entityIds);
double GetBlockAngle(AcDbObjectId ObjectId);
AcDbObjectIdArray _I_GetEntity_temp(AcDbObjectIdArray& entityIds);
bool IsExtendData(AcDbEntity* _pEnt, CString _StrName);
bool IsExtendData(AcDbObjectId _ObjectId, CString _StrName);

bool isClockwiseRotation(AcGeVector3d v1, AcGeVector3d v2, AcGeVector3d v3);

class RKTHandleIndex {
public:
	/* 0表设备,1表机电 */
	int type;

	/* 句柄 */
	int handle;

	/* 实体id */
	AcDbObjectId entityId;
};

class RKTLegend
{
public:
	RKTLegend();
	virtual ~RKTLegend();
	unsigned __int64 m_entityHandle;
	unsigned __int64 m_deviceHandle;
	double m_x;
	double m_y;
	double m_z;
public:
	void save(AcDbObjectId id);
	void read(AcDbObjectId id);
};
class RKTdevice
{
public:
	RKTdevice();
	virtual ~RKTdevice();
	unsigned __int64 m_entityHandle;
	std::vector<RKTLegend*> m_legends;

public:
	void save(AcDbObjectId id);
	void read(AcDbObjectId id);
};
class FRKTEditReactor : public AcEditorReactor
{
protected:
	bool mbAutoInitAndRelease;




public:
	FRKTEditReactor(const bool autoInitAndRelease = true);
	virtual ~FRKTEditReactor();


	virtual void beginDoubleClick(const AcGePoint3d& clickPoint);										//双击实体反应
	virtual void commandWillStart(const ACHAR* cmdStr);													//当命令开始时调用
	virtual void commandEnded(const ACHAR* cmdStr);														//当命令结束时调用
	virtual void commandFailed(const ACHAR* cmdStr);													//命令失败
	virtual void commandCancelled(const ACHAR* cmdStr);													//命令取消
	virtual void beginRightClick(const AcGePoint3d& clickPoint);										//右击


	virtual void viewChanged();
};
class RKTInputContextReactor : public AcEdInputContextReactor
{
protected:
	AcApDocument* mpDocument;


public:


	RKTInputContextReactor(AcApDocument* pDoc = NULL);
	virtual ~RKTInputContextReactor();


	virtual void Attach(AcApDocument* pDoc);
	virtual void Detach();
	virtual AcApDocument* Subject() const;
	virtual bool IsAttached() const;


	void endGetPoint(Acad::PromptStatus returnStatus, const AcGePoint3d& pointOut, const ACHAR*& pKeyword);




	virtual void endSSGet(Acad::PromptStatus returnStatus, const AcArray<AcDbObjectId>& ss);
};
class _move_demo {
public:
	static void command();
};

class _TEMP_ {
public:
	CString currentCommand;
	bool isInputContextReactor;
	AcDbObjectIdArray moveBaseEntityIdArray;
	AcGePoint3dArray moveBasePointArray;
	bool isRotate;
	AcGePoint3d rotateBasePoint1;
	AcGePoint3d rotateBasePoint2;
	AcGePoint3d rotateBasePoint3;
	AcGeVector3d Vector1;
	AcGeVector3d Vector2;
	double angle;
	AcDbObjectIdArray DataBaseLegend;
	double Old_angle;
	double New_angle;

	//--------------------------------
	AcDbObjectId moveBaseEntityId;
	AcGePoint3d moveBasePoint;
	//--------------------------------
	AcGePoint3d moveBaseEntityPosit;
	bool isRecordAppendedObject = false;
	AcDbObjectId appendedObjectId;
	AcGePoint3dArray disaPlace;
	bool IsMore;
	bool flag_IsPaste;
	AcDbObjectIdArray moveBaseEntityIdArray_Rotate;
public:
	_TEMP_()
	{
		//-------------------------------
		this->moveBasePoint = AcGePoint3d(0, 0, 0);
		this->moveBaseEntityId = NULL;
		//-------------------------------
		this->DataBaseLegend = NULL;
		this->currentCommand = _T("NULL");
		this->isInputContextReactor = false;
		this->moveBaseEntityIdArray = NULL;
		this->moveBasePointArray = NULL;
		this->moveBaseEntityIdArray_Rotate = NULL;
		this->isRotate = false;
		this->rotateBasePoint1 = AcGePoint3d(0, 0, 0);
		this->rotateBasePoint2 = AcGePoint3d(0, 0, 0);
		this->rotateBasePoint3 = AcGePoint3d(0, 0, 0);
		this->Vector1 = AcGePoint3d(0, 0, 0) - AcGePoint3d(0, 0, 0);
		this->Vector2 = AcGePoint3d(0, 0, 0) - AcGePoint3d(0, 0, 0);
		this->angle = 0;
		this->Old_angle = 0;
		this->flag_IsPaste = false;
		this->New_angle = 0;
		this->isRecordAppendedObject = false;
		this->appendedObjectId = NULL;
		this->disaPlace = NULL;
		this->IsMore = false;
	}
};

class RKTDatabaseReactor : public AcDbDatabaseReactor
{

public:
	ACRX_DECLARE_MEMBERS(RKTDatabaseReactor);

protected:
	AcDbDatabase* mpDatabase;

public:
	RKTDatabaseReactor(AcDbDatabase* pDb = NULL);
	virtual ~RKTDatabaseReactor();

	virtual void Attach(AcDbDatabase* pDb);
	virtual void Detach();
	virtual AcDbDatabase* Subject() const;
	virtual bool IsAttached() const;
	virtual void objectAppended(const AcDbDatabase* dwg, const AcDbObject* dbObj);
	virtual void objectModified(const AcDbDatabase* dwg, const AcDbObject* dbObj);
	virtual void objectErased(const AcDbDatabase* dwg, const AcDbObject* dbObj, Adesk::Boolean pErased = true);
	virtual void objectUnAppended(const AcDbDatabase* dwg, const AcDbObject* dbObj);
	virtual void objectReAppended(const AcDbDatabase* dwg, const AcDbObject* dbObj);
	virtual void objectOpenedForModify(const AcDbDatabase* dwg, const AcDbObject* dbObj);
	virtual void headerSysVarWillChange(const AcDbDatabase* dwg, const ACHAR* name);
	virtual void headerSysVarChanged(const AcDbDatabase* dwg, const ACHAR* name, Adesk::Boolean bSuccess);
	virtual void proxyResurrectionCompleted(const AcDbDatabase* dwg, const ACHAR* appname, AcDbObjectIdArray& objects);
	virtual void goodbye(const AcDbDatabase* dwg);
};

