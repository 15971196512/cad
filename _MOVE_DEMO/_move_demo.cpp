#include "pch.h"
#include <vector>
#include "string"
#include <stdarg.h>
#include <wchar.h>
#include <cstdlib>
#include <codecvt>
#include <sstream>
#include "acarray.h"
#include <stdio.h>
#include "_Func_.h"
#include "Arx_Func.h"
#define RKTDEVICE _T("RKTDEVICE")
#define RKTLEGEND _T("RKTLEGEND")
_TEMP_ TEMP;
AcDbObjectIdArray tttttttt = NULL;
std::vector<CString> deviceTypes;
ACRX_CONS_DEFINE_MEMBERS(RKTDatabaseReactor, AcDbDatabaseReactor, 1)
RKTDatabaseReactor::RKTDatabaseReactor(AcDbDatabase* pDb) : AcDbDatabaseReactor(), mpDatabase(pDb)
{
	if (pDb)
	{
		pDb->addReactor(this);
	}
}
RKTDatabaseReactor::~RKTDatabaseReactor() {
	Detach();
}
void RKTDatabaseReactor::Attach(AcDbDatabase* pDb)
{
	Detach();
	if (mpDatabase == NULL)
	{
		if ((mpDatabase = pDb) != NULL)
			pDb->addReactor(this);
	}
}
void RKTDatabaseReactor::Detach()
{
	if (mpDatabase)
	{
		mpDatabase->removeReactor(this);
		mpDatabase = NULL;
	}
}
AcDbDatabase* RKTDatabaseReactor::Subject() const
{
	return (mpDatabase);
}
bool RKTDatabaseReactor::IsAttached() const
{
	return (mpDatabase != NULL);
}
void RKTDatabaseReactor::objectAppended(const AcDbDatabase* dwg, const AcDbObject* dbObj)
{
	//AcDbObjectId Temp_Id = NULL;
	if (TEMP.isRecordAppendedObject == true && dbObj->objectId() != NULL) {
		if (dbObj->isA() == AcDbBlockReference::desc()) {
			TEMP.appendedObjectId = dbObj->objectId();
			tttttttt.append(TEMP.appendedObjectId);
			acutPrintf(_T(""));
		}
	}
}
void RKTDatabaseReactor::objectUnAppended(const AcDbDatabase* dwg, const AcDbObject* dbObj)
{

}
void RKTDatabaseReactor::objectReAppended(const AcDbDatabase* dwg, const AcDbObject* dbObj)
{

}
void RKTDatabaseReactor::objectOpenedForModify(const AcDbDatabase* dwg, const AcDbObject* dbObj)
{

}
void RKTDatabaseReactor::headerSysVarWillChange(const AcDbDatabase* dwg, const ACHAR* name)
{

}
void RKTDatabaseReactor::headerSysVarChanged(const AcDbDatabase* dwg, const ACHAR* name, Adesk::Boolean bSuccess)
{

}
void RKTDatabaseReactor::proxyResurrectionCompleted(const AcDbDatabase* dwg, const ACHAR* appname, AcDbObjectIdArray& objects)
{

}
void RKTDatabaseReactor::goodbye(const AcDbDatabase* dwg)
{

}
void RKTDatabaseReactor::objectModified(const AcDbDatabase* dwg, const AcDbObject* dbObj)
{

}
void RKTDatabaseReactor::objectErased(const AcDbDatabase* dwg, const AcDbObject* dbObj, Adesk::Boolean pErased)
{

}
RKTInputContextReactor::RKTInputContextReactor(AcApDocument* pDoc):AcEdInputContextReactor(), mpDocument(pDoc)
{
	acutPrintf(_T("进行初始化"));
}
RKTInputContextReactor::~RKTInputContextReactor()
{
	Detach();
}
void RKTInputContextReactor::Attach(AcApDocument* pDoc)
{
	Detach();
	if (mpDocument == NULL)
	{
		if ((mpDocument = pDoc) != NULL)
		{
			pDoc->inputPointManager()->addInputContextReactor(this);
		}
	}
}
void RKTInputContextReactor::Detach()
{
	if (mpDocument)
	{
		mpDocument->inputPointManager()->removeInputContextReactor(this);
		mpDocument = NULL;
	}
}
AcApDocument* RKTInputContextReactor::Subject() const
{
	return (mpDocument);
}
bool RKTInputContextReactor::IsAttached() const
{
	return (mpDocument != NULL);
}
void RKTInputContextReactor::endGetPoint(Acad::PromptStatus returnStatus, const AcGePoint3d& pointOut, const ACHAR*& pKeyword)
{
	if (wcscmp(TEMP.currentCommand, _T("ROTATE")) == 0 && TEMP.isRotate) {
		TEMP.rotateBasePoint2 = pointOut;
		TEMP.isRotate = false;
		return;
	}
}
//选择结束时
void RKTInputContextReactor::endSSGet(Acad::PromptStatus returnStatus, const AcArray<AcDbObjectId>& ss)
{
	if (TEMP.currentCommand == _T("MOVE") && TEMP.isInputContextReactor == true) {
		// 判断所选择的对象是否是设备,如果存在则获得设备的最小点作
		// 为移动基点
		for (int i = 0; i < ss.length(); i++) {
			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, ss[i], AcDb::kForRead) == Acad::eOk)
			{
				// 判断是否存在扩展字典
				AcDbObjectId dictionaryId = pBlockReference->extensionDictionary();
				if (dictionaryId == NULL)
				{
					pBlockReference->close();
					continue;
				}
				//判断扩展词典中是否存在设备记录
				AcDbObjectId xrecordId = NULL;
				AcDbDictionary* pDictionary = NULL;
				if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForRead) == 0)
				{
					pDictionary->getAt(RKTDEVICE, xrecordId);
					pDictionary->close();
				}
				// 如果存在设备记录则设置移动基点和移动基本对象
				if (xrecordId != NULL) {
					TEMP.moveBasePointArray.append(pBlockReference->position());
					TEMP.moveBaseEntityIdArray.append(ss[i]);

				}
				pBlockReference->close();
			}
		}
		TEMP.isInputContextReactor = false;
		
	}

	else if (TEMP.currentCommand == _T("ROTATE") && TEMP.isInputContextReactor == true) {


		// 判断所选择的对象是否是设备,如果存在则获得设备的最小点作
		// 为移动基点
		if (ss.length() == 0)
		{
			return;
		}
		
		for (int i = 0; i < ss.length(); i++) {


			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, ss[i], AcDb::kForRead) == Acad::eOk)
			{
				// 判断是否存在扩展字典
				AcDbObjectId dictionaryId = pBlockReference->extensionDictionary();
				if (dictionaryId == NULL)
				{
					pBlockReference->close();
					continue;
				}


				//判断扩展词典中是否存在设备记录
				AcDbObjectId xrecordId = NULL;
				AcDbDictionary* pDictionary = NULL;
				if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForRead) == 0)
				{
					pDictionary->getAt(RKTDEVICE, xrecordId);
					pDictionary->close();
				}


				// 如果存在设备记录则设置移动基点和移动基本对象
				if (xrecordId != NULL) {
					TEMP.rotateBasePoint1 = pBlockReference->position();
					TEMP.moveBaseEntityIdArray.append(ss[i]);
					TEMP.isRotate = true;
				}

				TEMP.Old_angle = GetBlockAngle(ss[i]) * PI / 180;
				pBlockReference->close();


				if (TEMP.moveBaseEntityIdArray_Rotate.length() == 0) {
					break;
				}
			}
		}


		TEMP.isInputContextReactor = false;
	}
	else if (wcscmp(TEMP.currentCommand, _T("COPY")) == 0 && TEMP.isInputContextReactor == true)
	{
		if (ss.length() == 0)
		{
			return;
		}
		for (int i = 0; i < ss.length(); i++) {
			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, ss[i], AcDb::kForRead) == Acad::eOk)
			{
				// 判断是否存在扩展字典
				AcDbObjectId dictionaryId = pBlockReference->extensionDictionary();
				if (dictionaryId == NULL)
				{
					pBlockReference->close();
					continue;
				}
				//判断扩展词典中是否存在设备记录
				AcDbObjectId xrecordId = NULL;
				AcDbDictionary* pDictionary = NULL;
				if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForRead) == 0)
				{
					pDictionary->getAt(RKTDEVICE, xrecordId);
					pDictionary->close();
				}
				// 如果存在设备记录则设置移动基点和移动基本对象
				if (xrecordId != NULL) {
					TEMP.moveBasePointArray.append(pBlockReference->position());
					TEMP.moveBaseEntityIdArray.append(ss[i]);
				}
				pBlockReference->close();
				if (TEMP.moveBaseEntityIdArray.length() != NULL) {
					break;
				}
			}
		}
		TEMP.isInputContextReactor = false;
	}
}
FRKTEditReactor::FRKTEditReactor(const bool autoInitAndRelease)
	: AcEditorReactor(), mbAutoInitAndRelease(autoInitAndRelease)
{
	if (autoInitAndRelease)
	{
		if (acedEditor)
		{
			acedEditor->addReactor(this);
		}
		else
		{
			mbAutoInitAndRelease = false;
		}
	}
}
FRKTEditReactor::~FRKTEditReactor()
{


}
void FRKTEditReactor::beginDoubleClick(const AcGePoint3d& clickPoint)
{


}
//命令开始时
void FRKTEditReactor::commandWillStart(const ACHAR* cmdStr)
{
	acutPrintf(_T("\nthere is command start %s \n"), cmdStr);
	TEMP.currentCommand = cmdStr;
	TEMP.isInputContextReactor = false;
	if (wcscmp(cmdStr, _T("MOVE")) == 0)
	{
		TEMP.moveBaseEntityIdArray = NULL;
		// 获得当前被选择对象
		ads_name adsName;
		AcDbObjectIdArray entityIds;
		if (acedSSGet(_T("I"), NULL, NULL, NULL, adsName) == 5100)
		{
			int len = 0;
			acedSSLength(adsName, &len);
			for (int i = 0; i < len; i++)
			{
				ads_name ename;
				acedSSName(adsName, i, ename);
				AcDbObjectId ObjectId = NULL;
				acdbGetObjectId(ObjectId, ename);
				entityIds.append(ObjectId);
			}
			acedSSFree(adsName);
		}
		// 如果没有选择对象使用输入反应器等待选择完成
		if (entityIds.length() <= 0)
		{
			TEMP.isInputContextReactor = true;
		}
		else
		{
			// 判断所选择的对象是否是设备,如果存在则获得设备的最小点作为移动基点
			for (int i = 0; i < entityIds.length(); i++) {
				AcDbBlockReference* pBlockReference = NULL;
				if (acdbOpenObject(pBlockReference, entityIds[i], AcDb::kForRead) == Acad::eOk)
				{
					// 判断是否存在扩展字典
					AcDbObjectId dictionaryId = pBlockReference->extensionDictionary();
					if (dictionaryId == NULL)
					{
						pBlockReference->close();
						continue;
					}
					//判断扩展词典中是否存在设备记录
					AcDbObjectId xrecordId = NULL;
					AcDbDictionary* pDictionary = NULL;
					if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForRead) == 0)
					{
						pDictionary->getAt(RKTDEVICE, xrecordId);
						pDictionary->close();
					}
					// 如果存在设备记录则设置移动基点和移动基本对象
					if (xrecordId != NULL) {
						TEMP.moveBasePointArray.append(pBlockReference->position());
						TEMP.moveBaseEntityIdArray.append(entityIds[i]);
					}
					pBlockReference->close();
				}
			}
		}
	}
	else if (wcscmp(cmdStr, _T("ROTATE")) == 0) {
		TEMP.moveBaseEntityId = NULL;

		
		// 获得当前被选择对象
		ads_name adsName;
		AcDbObjectIdArray entityIds;
		if (acedSSGet(_T("I"), NULL, NULL, NULL, adsName) == 5100)
		{
			int len = 0;
			acedSSLength(adsName, &len);


			for (int i = 0; i < len; i++)
			{
				ads_name ename;
				acedSSName(adsName, i, ename);


				AcDbObjectId ObjectId = NULL;
				acdbGetObjectId(ObjectId, ename);
				entityIds.append(ObjectId);
			}
			acedSSFree(adsName);
		}
		// 如果没有选择对象使用输入反应器等待选择完成
		if (entityIds.length() <= 0)
		{
			TEMP.isInputContextReactor = true;
		}
		else
		{
			// 判断所选择的对象是否是设备,如果存在则获得设备的最小点作为移动基点
			for (int i = 0; i < entityIds.length(); i++) {
				AcDbBlockReference* pBlockReference = NULL;
				if (acdbOpenObject(pBlockReference, entityIds[i], AcDb::kForRead) == Acad::eOk)
				{
					// 判断是否存在扩展字典
					AcDbObjectId dictionaryId = pBlockReference->extensionDictionary();
					if (dictionaryId == NULL)
					{
						pBlockReference->close();
						continue;
					}


					//判断扩展词典中是否存在设备记录
					AcDbObjectId xrecordId = NULL;
					AcDbDictionary* pDictionary = NULL;
					if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForRead) == 0)
					{
						pDictionary->getAt(RKTDEVICE, xrecordId);
						pDictionary->close();
					}


					// 如果存在设备记录则设置移动基点和移动基本对象
					if (xrecordId != NULL) {
						TEMP.rotateBasePoint1 = pBlockReference->position();
						TEMP.moveBaseEntityId = entityIds[i];
						TEMP.Old_angle = GetBlockAngle(entityIds[i]) * PI / 180;
					}
					TEMP.isRotate = true;
					pBlockReference->close();


					if (TEMP.moveBaseEntityId != NULL) {
						break;
					}
				}
			}
		}
		}
	else if (wcscmp(cmdStr, _T("GRIP_STRETCH")) == 0)
	{
		TEMP.moveBaseEntityId = NULL;
		// 获得当前被选择对象
		ads_name adsName;
		AcDbObjectIdArray entityIds;
		if (acedSSGet(_T("I"), NULL, NULL, NULL, adsName) == 5100)
		{
			int len = 0;
			acedSSLength(adsName, &len);
			for (int i = 0; i < len; i++)
			{
				ads_name ename;
				acedSSName(adsName, i, ename);
				AcDbObjectId ObjectId = NULL;
				acdbGetObjectId(ObjectId, ename);
				entityIds.append(ObjectId);
			}
			acedSSFree(adsName);
		}
		// 如果没有选择对象使用输入反应器等待选择完成
		if (entityIds.length() <= 0)
		{
			TEMP.isInputContextReactor = true;
		}
		else
		{
			// 判断所选择的对象是否是设备,如果存在则获得设备的最小点作为移动基点
			for (int i = 0; i < entityIds.length(); i++) {
				AcDbBlockReference* pBlockReference = NULL;
				bool isLegend = false;
				if (acdbOpenObject(pBlockReference, entityIds[i], AcDb::kForRead) == Acad::eOk)
				{
					// 判断是否存在扩展字典
					AcDbObjectId dictionaryId = pBlockReference->extensionDictionary();
					if (dictionaryId == NULL)
					{
						pBlockReference->close();
						continue;
					}
					//判断扩展词典中是否存在设备记录
					AcDbObjectId xrecordId = NULL;
					AcDbDictionary* pDictionary = NULL;
					if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForRead) == 0)
					{
						pDictionary->getAt(RKTDEVICE, xrecordId);
						pDictionary->close();
					}
					// 如果存在设备记录则设置移动基点和移动基本对象
					if (xrecordId != NULL) {
						TEMP.moveBasePoint = pBlockReference->position();
						TEMP.moveBaseEntityId = entityIds[i];
					}
					pBlockReference->close();
					if (TEMP.moveBaseEntityId != NULL) {
						break;
					}
				}
			}
		}
		}
	else if (wcscmp(cmdStr, _T("COPY")) == 0)
	{
		TEMP.isRecordAppendedObject = true;
		//--------------------------------------------
		//--------------------------------------------
		TEMP.appendedObjectId = NULL;
	}
	else if (wcscmp(cmdStr, _T("COPYCLIP")) == 0)
	{
	}
	else if (wcscmp(cmdStr, _T("PASTECLIP")) == 0)
	{
		TEMP.isRecordAppendedObject = true;
		//--------------------------------------------
		//--------------------------------------------
		TEMP.appendedObjectId = NULL;
	}
}
//命令结束时 
void FRKTEditReactor::commandEnded(const ACHAR* cmdStr)
{
	acutPrintf(_T("\nthere is command end %s \n"), cmdStr);
	if (wcscmp(cmdStr, _T("MOVE")) == 0 && TEMP.moveBaseEntityIdArray.length() != NULL) {
			// 获得最近选择集
		for (int i = 0; i < TEMP.moveBaseEntityIdArray.length(); i++) {
			AcDbObjectId moveBaseEntityId_temp = TEMP.moveBaseEntityIdArray[i];
			// 获得最近选择集
			ads_name adsName;
			AcDbObjectIdArray entityIds;
			int error = acedSSGet(_T("P"), NULL, NULL, NULL, adsName);
			if (error != 5100)
			{
				return;
			}
			int len = 0;
			acedSSLength(adsName, &len);
			for (int i = 0; i < len; i++)
			{
				ads_name ename;
				acedSSName(adsName, i, ename);
				AcDbObjectId ObjectId = NULL;
				acdbGetObjectId(ObjectId, ename);
				entityIds.append(ObjectId);
			}
			acedSSFree(adsName);
			if (entityIds.length() == 0) {
				break;
			}
			// 获得位移
			AcGeVector3d displacement(0, 0, 0);
			for (int j = 0; j < entityIds.length(); j++) {
				if (entityIds[j] != moveBaseEntityId_temp) {   //这里是循环遍历所有的块ID，找出父块
					continue;
				}
				AcDbBlockReference* pBlockReference = NULL;
				if (acdbOpenObject(pBlockReference, entityIds[j], AcDb::kForRead) == Acad::eOk)
				{
					displacement = pBlockReference->position() - TEMP.moveBasePointArray[i];  // 获取到父块移动的向量距离
					pBlockReference->close();
				}
				break;
			}
			//// 获得设备对象
			RKTdevice* pDevice = new RKTdevice();
			pDevice->m_entityHandle = moveBaseEntityId_temp.handle();
			pDevice->read(moveBaseEntityId_temp);   //读取到父块里面的扩展信息
			CString str = _T("");
			str.Format(_T("%d"), pDevice->m_legends.size());
			acutPrintf(str);
			// 移动设备对象(注意如果本身机电已经被选中则不需要再次移动了)
			for (int i = 0; i < pDevice->m_legends.size(); i++)  //这里是循环遍历所有的被选中的子块句柄
			{
				AcDbObjectId legendId = NULL;

				//这里是通过m_legends[i]->m_entityHandle 中的子块的句柄 获得块对象ID 
				acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(legendId, false, pDevice->m_legends[i]->m_entityHandle);
				bool isMove = true;

				//判断是否为MOVE
				for (int u = 0; u < entityIds.length(); u++) {
					if (entityIds[u] == legendId) {
						isMove = false;
						break;
					}
				}
				if (isMove == false) {
					continue;
				}
				//判断是否为MOVE   //isMove  ???????


				AcDbBlockReference* pBlockReference = NULL;
				if (acdbOpenObject(pBlockReference, legendId, AcDb::kForWrite) != Acad::eOk)
				{
					continue;
				}
				pBlockReference->setPosition(pBlockReference->position() + displacement);
				pBlockReference->close();
			}
		}
		TEMP.moveBaseEntityIdArray.removeAll();
		TEMP.moveBasePointArray.removeAll();
	}
	else if (wcscmp(cmdStr, _T("ROTATE")) == 0 && TEMP.moveBaseEntityIdArray.length() != NULL) {
		while (true) {
			//获得最近选择集
			ads_name adsName;
			AcDbObjectIdArray entityIds;
			AcGeMatrix3d matrix;
			int error = acedSSGet(_T("P"), NULL, NULL, NULL, adsName);
			if (error != 5100)
			{
				return;
			}
			int len = 0;
			acedSSLength(adsName, &len);
			for (int i = 0; i < len; i++)
			{
				ads_name ename;
				acedSSName(adsName, i, ename);


				AcDbObjectId ObjectId = NULL;
				acdbGetObjectId(ObjectId, ename);
				entityIds.append(ObjectId);
			}
			acedSSFree(adsName);
			if (entityIds.length() == 0) {
				break;
			}
			// 获得偏移后位置
			for (int i = 0; i < entityIds.length(); i++) {
				if (entityIds[i] != TEMP.moveBaseEntityId) {
					continue;
				}
				AcDbBlockReference* pBlockReference = NULL;
				if (acdbOpenObject(pBlockReference, entityIds[i], AcDb::kForRead) == Acad::eOk)
				{
					TEMP.rotateBasePoint3 = pBlockReference->position();
					// 计算偏转角度
					TEMP.Vector1 = TEMP.rotateBasePoint1 - TEMP.rotateBasePoint2;
					TEMP.Vector2 = TEMP.rotateBasePoint3 - TEMP.rotateBasePoint2;
					TEMP.Vector1.normalize();
					TEMP.Vector2.normalize();
					TEMP.angle = TEMP.Vector2.angleTo(TEMP.Vector1);
					if (isClockwiseRotation(TEMP.Vector1, TEMP.Vector2, AcGeVector3d(0, 0, 0))) {
						TEMP.angle = (-TEMP.angle);
					}
					pBlockReference->close();
				}
				break;
			}
			// 获得设备对象
			RKTdevice* pDevice = new RKTdevice();
			pDevice->m_entityHandle = TEMP.moveBaseEntityId.handle();
			pDevice->read(TEMP.moveBaseEntityId);
			TEMP.New_angle = GetBlockAngle(entityIds[0]) * PI / 180;
			// 旋转设备对象(注意如果本身机电已经被选中则不需要再次移动了)
			RKTdevice* pDevice_temp = new RKTdevice();
			for (int i = 0; i < pDevice->m_legends.size(); i++)
			{
				AcDbObjectId legendId = NULL;
				acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(legendId, false, pDevice->m_legends[i]->m_entityHandle);
				// 排除已经选中的机电
				bool isMove = true;
				for (int u = 0; u < entityIds.length(); u++) {
					if (entityIds[u] == legendId) {
						isMove = false;
						break;
					}
				}
				if (isMove == false) {
					continue;
				}
				AcDbBlockReference* pBlockReference = NULL;
				if (acdbOpenObject(pBlockReference, legendId, AcDb::kForWrite) != Acad::eOk)
				{
					continue;
				}
				if (TEMP.rotateBasePoint1 == TEMP.rotateBasePoint2 && TEMP.rotateBasePoint2 == TEMP.rotateBasePoint3 && TEMP.angle == 0)
				{
					matrix.setToRotation(TEMP.New_angle - TEMP.Old_angle, AcGeVector3d::kZAxis, TEMP.rotateBasePoint2);
					pBlockReference->transformBy(matrix);
				}
				else
				{
					matrix.setToRotation(TEMP.angle, AcGeVector3d::kZAxis, TEMP.rotateBasePoint2);
					pBlockReference->transformBy(matrix);
				}
				
				AcGePoint3d Base_dian(0, 0, 0);
				AcGePoint3d Base_legend(0, 0, 0);
				Base_dian = GetBlockPosition(TEMP.moveBaseEntityId);
				Base_legend = pBlockReference->position();
				//加入向量值-xyz-  存入扩展数据 
				pDevice->m_legends[i]->m_x = Base_legend.x - Base_dian.x;
				pDevice->m_legends[i]->m_y = Base_legend.y - Base_dian.y;
				pDevice->m_legends[i]->m_z = Base_legend.z - Base_dian.z;
				pBlockReference->close();
			}
			pDevice->save(TEMP.moveBaseEntityId);
			delete pDevice_temp;
			delete pDevice;

			break;
		};
		TEMP.isRotate = false;
		}
	else if (wcscmp(cmdStr, _T("GRIP_STRETCH")) == 0 && TEMP.moveBaseEntityIdArray.length() != NULL) {
			while (true) {
				// 获得最近选择集
				ads_name adsName;
				AcDbObjectIdArray entityIds;
				int error = acedSSGet(_T("P"), NULL, NULL, NULL, adsName);
				if (error != 5100)
				{
					return;
				}
				int len = 0;
				acedSSLength(adsName, &len);
				for (int i = 0; i < len; i++)
				{
					ads_name ename;
					acedSSName(adsName, i, ename);
					AcDbObjectId ObjectId = NULL;
					acdbGetObjectId(ObjectId, ename);
					entityIds.append(ObjectId);
				}
				acedSSFree(adsName);
				if (entityIds.length() == 0) {
					break;
				}
				// 获得位移
				AcGeVector3d displacement(0, 0, 0);
				for (int i = 0; i < entityIds.length(); i++) {
					if (entityIds[i] != TEMP.moveBaseEntityId) {
						continue;
					}
					AcDbBlockReference* pBlockReference = NULL;
					if (acdbOpenObject(pBlockReference, entityIds[i], AcDb::kForRead) == Acad::eOk)
					{
						displacement = pBlockReference->position() - TEMP.moveBasePoint;
						pBlockReference->close();
					}
					break;
				}
				// 获得设备对象
				RKTdevice* pDevice = new RKTdevice();
				pDevice->m_entityHandle = TEMP.moveBaseEntityId.handle();
				pDevice->read(TEMP.moveBaseEntityId);
				// 移动设备对象(注意如果本身机电已经被选中则不需要再次移动了)
				for (int i = 0; i < pDevice->m_legends.size(); i++)
				{
					AcDbObjectId legendId = NULL;
					acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(legendId, false, pDevice->m_legends[i]->m_entityHandle);
					
					AcDbBlockReference* pBlockReference = NULL;
					if (acdbOpenObject(pBlockReference, legendId, AcDb::kForWrite) != Acad::eOk)
					{
						continue;
					}
					pBlockReference->setPosition(pBlockReference->position() + displacement);
					pBlockReference->close();
				}
				break;
			};
			TEMP.moveBaseEntityId = NULL;
			}
	else if (wcscmp(cmdStr, _T("GRIP_STRETCH")) == 0 && TEMP.moveBaseEntityIdArray.length() == NULL)
	{
		while (true) {
			// 获得最近选择集
			ads_name adsName;
			AcDbObjectIdArray entityIds;
			int error = acedSSGet(_T("P"), NULL, NULL, NULL, adsName);
			if (error != 5100)
			{
				return;
			}
			int len = 0;
			acedSSLength(adsName, &len);
			for (int i = 0; i < len; i++)
			{
				ads_name ename;
				acedSSName(adsName, i, ename);
				AcDbObjectId ObjectId = NULL;
				acdbGetObjectId(ObjectId, ename);
				entityIds.append(ObjectId);
			}
			acedSSFree(adsName);
			RKTLegend* pLegend = new RKTLegend();
			pLegend->m_entityHandle = entityIds[0].handle();
			pLegend->m_deviceHandle = entityIds[0].handle();
			pLegend->read(entityIds[0]);
			AcDbObjectId pDeviceId = NULL;
			acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(pDeviceId, false, pLegend->m_deviceHandle);



			RKTdevice* pDevice = new RKTdevice();
			pDevice->m_entityHandle = pDeviceId.handle();
			pDevice->read(pDeviceId);
			// 移动设备对象(注意如果本身机电已经被选中则不需要再次移动了)
			for (int i = 0; i < pDevice->m_legends.size(); i++)
			{
				AcDbObjectId legendId = NULL;
				acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(legendId, false, pDevice->m_legends[i]->m_entityHandle);
				if (pLegend->m_entityHandle == pDevice->m_legends[i]->m_entityHandle)
				{
					pDevice->m_legends[i]->m_x = GetBlockPosition(legendId).x - GetBlockPosition(pDeviceId).x;
					pDevice->m_legends[i]->m_y = GetBlockPosition(legendId).y - GetBlockPosition(pDeviceId).y;
					pDevice->m_legends[i]->m_z = GetBlockPosition(legendId).z - GetBlockPosition(pDeviceId).z;
					pLegend->save(legendId);
					break;
				}
			}
			
			pDevice->save(pDeviceId);
			delete pDevice;
			break;
		};
	}
	else if (wcscmp(cmdStr, _T("COPY")) == 0)
	{
		for (int i = 0;i < tttttttt.length();i++)
		{
			RKTdevice* pDevice = new RKTdevice();
			pDevice->read(tttttttt[i]);
			if (pDevice->m_legends.size() == 0)
			{
				ObjectArx::DeleteObject(tttttttt[i]);
			}
			if (pDevice->m_legends.size() != 0)
			{
				TEMP.appendedObjectId = tttttttt[i];
			}
		}
		TEMP.isRecordAppendedObject = false;
		RKTdevice* pDevice = new RKTdevice();
		AcDbObjectIdArray Legdend_Ids;
		AcDbObjectIdArray entityIds;
		
		pDevice->read(TEMP.appendedObjectId);
		pDevice->m_entityHandle = TEMP.appendedObjectId.handle();
		//pDevice->read(TEMP.appendedObjectId);
		// 移动设备对象(注意如果本身机电已经被选中则不需要再次移动了)
		for (int i = 0; i < pDevice->m_legends.size(); i++)
		{
			AcDbObjectId legendId = NULL;
			AcDbObjectId legendId_new = NULL;
			acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(legendId, false, pDevice->m_legends[i]->m_entityHandle);

			//设备父块是CAD自动复制的，子块需要手动复制
			legendId_new = ObjectArx::CopyEntity(legendId);
			//拿到新块的legend的ID
			Legdend_Ids.append(legendId_new);
			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, legendId_new, AcDb::kForWrite) != Acad::eOk)
			{
				continue;
			}
			//设置新复制出的子块相对于基点的位置（向量）
			acutPrintf(_T(""));
			
			pBlockReference->setPosition(GetBlockPosition(TEMP.appendedObjectId) + AcGeVector3d(pDevice->m_legends[i]->m_x, pDevice->m_legends[i]->m_y, pDevice->m_legends[i]->m_z));
			//将块的句柄存放到pLegend->m_entityHandle中去
			pBlockReference->close();
		}
		for (int i = 0; i < pDevice->m_legends.size(); i++)
		{
			//拿到父块句柄
			pDevice->m_legends[i]->m_deviceHandle = TEMP.appendedObjectId.handle();
			//拿到自己的句柄
			pDevice->m_legends[i]->m_entityHandle = Legdend_Ids[i].handle();

			AcGePoint3d Base_dian(0, 0, 0);
			AcGePoint3d Base_legend(0, 0, 0);
			Base_dian = GetBlockPosition(TEMP.appendedObjectId);
			Base_legend = GetBlockPosition(Legdend_Ids[i]);
			//加入向量值-xyz-  存入扩展数据 
			pDevice->m_legends[i]->m_x = Base_legend.x - Base_dian.x;
			pDevice->m_legends[i]->m_y = Base_legend.y - Base_dian.y;
			pDevice->m_legends[i]->m_z = Base_legend.z - Base_dian.z;
		}
		//保存
		pDevice->save(TEMP.appendedObjectId);
		delete pDevice;
	}
	else if (wcscmp(cmdStr, _T("PASTECLIP")) == 0)
	{
		for (int i = 0; i < tttttttt.length(); i++)
		{
			RKTdevice* pDevice = new RKTdevice();
			pDevice->read(tttttttt[i]);
			if (pDevice->m_legends.size() == 0)
			{
				ObjectArx::DeleteObject(tttttttt[i]);
			}
			if (pDevice->m_legends.size() != 0)
			{
				TEMP.appendedObjectId = tttttttt[i];
			}
		}
		TEMP.isRecordAppendedObject = false;
		RKTdevice* pDevice = new RKTdevice();
		AcDbObjectIdArray Legdend_Ids;
		AcDbObjectIdArray entityIds;
		pDevice->read(TEMP.appendedObjectId);
		pDevice->m_entityHandle = TEMP.appendedObjectId.handle();
		for (int i = 0; i < pDevice->m_legends.size(); i++)
		{
			AcDbObjectId legendId_new = NULL;
			AcDbObjectId legendId = NULL;
			acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(legendId, false, pDevice->m_legends[i]->m_entityHandle);
			//设备父块是CAD自动复制的，子块需要手动复制
			legendId_new = ObjectArx::CopyEntity(legendId);
			//拿到新块的legend的ID
			Legdend_Ids.append(legendId_new);
			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, legendId_new, AcDb::kForWrite) != Acad::eOk)
			{
				continue;
			}
			//设置新复制出的子块相对于基点的位置（向量）
			pBlockReference->setPosition(GetBlockPosition(TEMP.appendedObjectId) + AcGeVector3d(pDevice->m_legends[i]->m_x, pDevice->m_legends[i]->m_y, pDevice->m_legends[i]->m_z));
			//将块的句柄存放到pLegend->m_entityHandle中去
			pBlockReference->close();
		}
		for (int i = 0; i < pDevice->m_legends.size(); i++)
		{
			//拿到父块句柄
			pDevice->m_legends[i]->m_deviceHandle = TEMP.appendedObjectId.handle();
			//拿到自己的句柄
			pDevice->m_legends[i]->m_entityHandle = Legdend_Ids[i].handle();

			AcGePoint3d Base_dian(0, 0, 0);
			AcGePoint3d Base_legend(0, 0, 0);
			Base_dian = GetBlockPosition(TEMP.appendedObjectId);
			Base_legend = GetBlockPosition(Legdend_Ids[i]);
			//加入向量值-xyz-  存入扩展数据 
			pDevice->m_legends[i]->m_x = Base_legend.x - Base_dian.x;
			pDevice->m_legends[i]->m_y = Base_legend.y - Base_dian.y;
			pDevice->m_legends[i]->m_z = Base_legend.z - Base_dian.z;
		}
		//保存
		pDevice->save(TEMP.appendedObjectId);
		delete pDevice;
	}
}
	
void FRKTEditReactor::beginRightClick(const AcGePoint3d& clickPoint)
{


}
void FRKTEditReactor::viewChanged() {


}
void FRKTEditReactor::commandFailed(const ACHAR* cmdStr)
{
}
void FRKTEditReactor::commandCancelled(const ACHAR* cmdStr)
{
	if (wcscmp(cmdStr, _T("COPY")) == 0)
	{
		for (int i = 0; i < tttttttt.length(); i++)
		{
			RKTdevice* pDevice = new RKTdevice();
			pDevice->read(tttttttt[i]);
			if (pDevice->m_legends.size() == 0)
			{
				ObjectArx::DeleteObject(tttttttt[i]);
			}
			if (pDevice->m_legends.size() != 0)
			{
				TEMP.appendedObjectId = tttttttt[i];
			}
		}
		TEMP.isRecordAppendedObject = false;
		RKTdevice* pDevice = new RKTdevice();
		AcDbObjectIdArray Legdend_Ids;
		AcDbObjectIdArray entityIds;

		pDevice->read(TEMP.appendedObjectId);
		pDevice->m_entityHandle = TEMP.appendedObjectId.handle();
		//pDevice->read(TEMP.appendedObjectId);
		// 移动设备对象(注意如果本身机电已经被选中则不需要再次移动了)
		for (int i = 0; i < pDevice->m_legends.size(); i++)
		{
			AcDbObjectId legendId = NULL;
			AcDbObjectId legendId_new = NULL;
			acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(legendId, false, pDevice->m_legends[i]->m_entityHandle);

			//设备父块是CAD自动复制的，子块需要手动复制
			legendId_new = ObjectArx::CopyEntity(legendId);
			//拿到新块的legend的ID
			Legdend_Ids.append(legendId_new);
			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, legendId_new, AcDb::kForWrite) != Acad::eOk)
			{
				continue;
			}
			//设置新复制出的子块相对于基点的位置（向量）
			acutPrintf(_T(""));

			pBlockReference->setPosition(GetBlockPosition(TEMP.appendedObjectId) + AcGeVector3d(pDevice->m_legends[i]->m_x, pDevice->m_legends[i]->m_y, pDevice->m_legends[i]->m_z));
			//将块的句柄存放到pLegend->m_entityHandle中去
			pBlockReference->close();
		}
		for (int i = 0; i < pDevice->m_legends.size(); i++)
		{
			//拿到父块句柄
			pDevice->m_legends[i]->m_deviceHandle = TEMP.appendedObjectId.handle();
			//拿到自己的句柄
			pDevice->m_legends[i]->m_entityHandle = Legdend_Ids[i].handle();

			AcGePoint3d Base_dian(0, 0, 0);
			AcGePoint3d Base_legend(0, 0, 0);
			Base_dian = GetBlockPosition(TEMP.appendedObjectId);
			Base_legend = GetBlockPosition(Legdend_Ids[i]);
			//加入向量值-xyz-  存入扩展数据 
			pDevice->m_legends[i]->m_x = Base_legend.x - Base_dian.x;
			pDevice->m_legends[i]->m_y = Base_legend.y - Base_dian.y;
			pDevice->m_legends[i]->m_z = Base_legend.z - Base_dian.z;
		}
		//保存
		pDevice->save(TEMP.appendedObjectId);
		delete pDevice;
	}

}
void _move_demo::command() {
	do {
		ads_point pt;
		ads_name entName;
		if (acedEntSel(_T("\n请选择一个设备对象："), entName, pt) != RTNORM)
		{
			break;
		}
		AcDbObjectId deviceId = NULL;
		acdbGetObjectId(deviceId, entName);

		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, deviceId, AcDb::kForRead) != Acad::eOk)
		{
			break;
		}
		pBlockReference->close();
		// 设置用到的机电对象
		
		deviceTypes.push_back(_T("排水"));
		deviceTypes.push_back(_T("冷水墙出"));
		deviceTypes.push_back(_T("热水地出"));
		deviceTypes.push_back(_T("冷凝排水"));
		deviceTypes.push_back(_T("热水墙出")); 
		deviceTypes.push_back(_T("单相接线位"));
		deviceTypes.push_back(_T("单相接线位"));
		deviceTypes.push_back(_T("燃气接点"));

		ads_name adsName;
		if (acedSSGet(NULL, NULL, NULL, NULL, adsName) != 5100)
		{
			break;
		}
		int len = 0;
		//这里是获取选择的对象的长度，用于下面循环遍历
		if (acedSSLength(adsName, &len) != 5100)
		{
			break;
		}
		AcDbObjectIdArray entityIds;
		for (int i = 0; i < len; i++)
		{
			ads_name entName;
			acedSSName(adsName, i, entName);
			AcDbObjectId objId;
			acdbGetObjectId(objId, entName);
			entityIds.append(objId);
		}

		acedSSFree(adsName);
		RKTdevice* pDevice = new RKTdevice();
		pDevice->m_entityHandle = deviceId.handle();
		// 获得里面的机电
		for (int i = 0; i < entityIds.length(); i++)
		{
			AcDbBlockReference* pBlockReference = NULL;

			//这里是获取块的参数 
			if (acdbOpenObject(pBlockReference, entityIds[i], AcDb::kForRead) != Acad::eOk)
			{
				continue;
			}
			// 获得块名称
			AcDbObjectId blockTableRecordId = NULL;
			AcDbDynBlockReference* pDynBlockReference = new AcDbDynBlockReference(pBlockReference);
			if (pDynBlockReference == NULL)
			{
				blockTableRecordId = pBlockReference->blockTableRecord();
			}
			else {
				blockTableRecordId = pDynBlockReference->dynamicBlockTableRecord();
			}
			wchar_t* blockName = NULL;
			AcDbBlockTableRecord* pBlockTableRecord = NULL;
			if (acdbOpenObject(pBlockTableRecord, blockTableRecordId, AcDb::OpenMode::kForRead) == Acad::eOk)
			{
				pBlockTableRecord->getName(blockName);
				//获取块名,用于下面判断是否为机电
				pBlockTableRecord->close();
			}
			// 判断是否是机电
			bool isLegend = false;
			for (int u = 0; u < deviceTypes.size(); u++)
			{
				if (wcscmp(blockName, deviceTypes[u]) == 0)
				{
					isLegend = true;
					break;
				}
			}
			if (isLegend == false)
			{
				continue;
			}
			//deviceType是个数组 存放了五种需要的机电类型  
			AcGePoint3d Base_dian(0, 0, 0);
			AcGePoint3d Base_legend(0, 0, 0);
			Base_dian = GetBlockPosition(deviceId);
			Base_legend = GetBlockPosition(entityIds[i]);

			RKTLegend* pLegend = new RKTLegend();
			
			//将块的句柄存放到pLegend->m_entityHandle中去
			pLegend->m_entityHandle = entityIds[i].handle();
			/////////-----------------------------------//
			pLegend->m_x = Base_legend.x - Base_dian.x;
			pLegend->m_y = Base_legend.y - Base_dian.y;
			pLegend->m_z = Base_legend.z - Base_dian.z;
			/////////-----------------------------------//
			AcGePoint3d teMp(pLegend->m_x, pLegend->m_y, pLegend->m_z);
			TEMP.disaPlace.append(teMp);
			/////////-----------------------------------//
			//然后将块的句柄存放到m_legends 数组中去 
			pDevice->m_legends.push_back(pLegend);
			pBlockReference->close();
		}

		//将pDevice存放到父块的扩展参数中去  pDevice中放是自己的句柄，和其他子块的一个句柄数组 
		pDevice->save(deviceId);
		delete pDevice;


	} while (false);

}