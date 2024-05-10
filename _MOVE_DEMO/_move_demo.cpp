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
	acutPrintf(_T("���г�ʼ��"));
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
//ѡ�����ʱ
void RKTInputContextReactor::endSSGet(Acad::PromptStatus returnStatus, const AcArray<AcDbObjectId>& ss)
{
	if (TEMP.currentCommand == _T("MOVE") && TEMP.isInputContextReactor == true) {
		// �ж���ѡ��Ķ����Ƿ����豸,������������豸����С����
		// Ϊ�ƶ�����
		for (int i = 0; i < ss.length(); i++) {
			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, ss[i], AcDb::kForRead) == Acad::eOk)
			{
				// �ж��Ƿ������չ�ֵ�
				AcDbObjectId dictionaryId = pBlockReference->extensionDictionary();
				if (dictionaryId == NULL)
				{
					pBlockReference->close();
					continue;
				}
				//�ж���չ�ʵ����Ƿ�����豸��¼
				AcDbObjectId xrecordId = NULL;
				AcDbDictionary* pDictionary = NULL;
				if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForRead) == 0)
				{
					pDictionary->getAt(RKTDEVICE, xrecordId);
					pDictionary->close();
				}
				// ��������豸��¼�������ƶ�������ƶ���������
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


		// �ж���ѡ��Ķ����Ƿ����豸,������������豸����С����
		// Ϊ�ƶ�����
		if (ss.length() == 0)
		{
			return;
		}
		
		for (int i = 0; i < ss.length(); i++) {


			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, ss[i], AcDb::kForRead) == Acad::eOk)
			{
				// �ж��Ƿ������չ�ֵ�
				AcDbObjectId dictionaryId = pBlockReference->extensionDictionary();
				if (dictionaryId == NULL)
				{
					pBlockReference->close();
					continue;
				}


				//�ж���չ�ʵ����Ƿ�����豸��¼
				AcDbObjectId xrecordId = NULL;
				AcDbDictionary* pDictionary = NULL;
				if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForRead) == 0)
				{
					pDictionary->getAt(RKTDEVICE, xrecordId);
					pDictionary->close();
				}


				// ��������豸��¼�������ƶ�������ƶ���������
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
				// �ж��Ƿ������չ�ֵ�
				AcDbObjectId dictionaryId = pBlockReference->extensionDictionary();
				if (dictionaryId == NULL)
				{
					pBlockReference->close();
					continue;
				}
				//�ж���չ�ʵ����Ƿ�����豸��¼
				AcDbObjectId xrecordId = NULL;
				AcDbDictionary* pDictionary = NULL;
				if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForRead) == 0)
				{
					pDictionary->getAt(RKTDEVICE, xrecordId);
					pDictionary->close();
				}
				// ��������豸��¼�������ƶ�������ƶ���������
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
//���ʼʱ
void FRKTEditReactor::commandWillStart(const ACHAR* cmdStr)
{
	acutPrintf(_T("\nthere is command start %s \n"), cmdStr);
	TEMP.currentCommand = cmdStr;
	TEMP.isInputContextReactor = false;
	if (wcscmp(cmdStr, _T("MOVE")) == 0)
	{
		TEMP.moveBaseEntityIdArray = NULL;
		// ��õ�ǰ��ѡ�����
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
		// ���û��ѡ�����ʹ�����뷴Ӧ���ȴ�ѡ�����
		if (entityIds.length() <= 0)
		{
			TEMP.isInputContextReactor = true;
		}
		else
		{
			// �ж���ѡ��Ķ����Ƿ����豸,������������豸����С����Ϊ�ƶ�����
			for (int i = 0; i < entityIds.length(); i++) {
				AcDbBlockReference* pBlockReference = NULL;
				if (acdbOpenObject(pBlockReference, entityIds[i], AcDb::kForRead) == Acad::eOk)
				{
					// �ж��Ƿ������չ�ֵ�
					AcDbObjectId dictionaryId = pBlockReference->extensionDictionary();
					if (dictionaryId == NULL)
					{
						pBlockReference->close();
						continue;
					}
					//�ж���չ�ʵ����Ƿ�����豸��¼
					AcDbObjectId xrecordId = NULL;
					AcDbDictionary* pDictionary = NULL;
					if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForRead) == 0)
					{
						pDictionary->getAt(RKTDEVICE, xrecordId);
						pDictionary->close();
					}
					// ��������豸��¼�������ƶ�������ƶ���������
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

		
		// ��õ�ǰ��ѡ�����
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
		// ���û��ѡ�����ʹ�����뷴Ӧ���ȴ�ѡ�����
		if (entityIds.length() <= 0)
		{
			TEMP.isInputContextReactor = true;
		}
		else
		{
			// �ж���ѡ��Ķ����Ƿ����豸,������������豸����С����Ϊ�ƶ�����
			for (int i = 0; i < entityIds.length(); i++) {
				AcDbBlockReference* pBlockReference = NULL;
				if (acdbOpenObject(pBlockReference, entityIds[i], AcDb::kForRead) == Acad::eOk)
				{
					// �ж��Ƿ������չ�ֵ�
					AcDbObjectId dictionaryId = pBlockReference->extensionDictionary();
					if (dictionaryId == NULL)
					{
						pBlockReference->close();
						continue;
					}


					//�ж���չ�ʵ����Ƿ�����豸��¼
					AcDbObjectId xrecordId = NULL;
					AcDbDictionary* pDictionary = NULL;
					if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForRead) == 0)
					{
						pDictionary->getAt(RKTDEVICE, xrecordId);
						pDictionary->close();
					}


					// ��������豸��¼�������ƶ�������ƶ���������
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
		// ��õ�ǰ��ѡ�����
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
		// ���û��ѡ�����ʹ�����뷴Ӧ���ȴ�ѡ�����
		if (entityIds.length() <= 0)
		{
			TEMP.isInputContextReactor = true;
		}
		else
		{
			// �ж���ѡ��Ķ����Ƿ����豸,������������豸����С����Ϊ�ƶ�����
			for (int i = 0; i < entityIds.length(); i++) {
				AcDbBlockReference* pBlockReference = NULL;
				bool isLegend = false;
				if (acdbOpenObject(pBlockReference, entityIds[i], AcDb::kForRead) == Acad::eOk)
				{
					// �ж��Ƿ������չ�ֵ�
					AcDbObjectId dictionaryId = pBlockReference->extensionDictionary();
					if (dictionaryId == NULL)
					{
						pBlockReference->close();
						continue;
					}
					//�ж���չ�ʵ����Ƿ�����豸��¼
					AcDbObjectId xrecordId = NULL;
					AcDbDictionary* pDictionary = NULL;
					if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForRead) == 0)
					{
						pDictionary->getAt(RKTDEVICE, xrecordId);
						pDictionary->close();
					}
					// ��������豸��¼�������ƶ�������ƶ���������
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
//�������ʱ 
void FRKTEditReactor::commandEnded(const ACHAR* cmdStr)
{
	acutPrintf(_T("\nthere is command end %s \n"), cmdStr);
	if (wcscmp(cmdStr, _T("MOVE")) == 0 && TEMP.moveBaseEntityIdArray.length() != NULL) {
			// ������ѡ��
		for (int i = 0; i < TEMP.moveBaseEntityIdArray.length(); i++) {
			AcDbObjectId moveBaseEntityId_temp = TEMP.moveBaseEntityIdArray[i];
			// ������ѡ��
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
			// ���λ��
			AcGeVector3d displacement(0, 0, 0);
			for (int j = 0; j < entityIds.length(); j++) {
				if (entityIds[j] != moveBaseEntityId_temp) {   //������ѭ���������еĿ�ID���ҳ�����
					continue;
				}
				AcDbBlockReference* pBlockReference = NULL;
				if (acdbOpenObject(pBlockReference, entityIds[j], AcDb::kForRead) == Acad::eOk)
				{
					displacement = pBlockReference->position() - TEMP.moveBasePointArray[i];  // ��ȡ�������ƶ�����������
					pBlockReference->close();
				}
				break;
			}
			//// ����豸����
			RKTdevice* pDevice = new RKTdevice();
			pDevice->m_entityHandle = moveBaseEntityId_temp.handle();
			pDevice->read(moveBaseEntityId_temp);   //��ȡ�������������չ��Ϣ
			CString str = _T("");
			str.Format(_T("%d"), pDevice->m_legends.size());
			acutPrintf(str);
			// �ƶ��豸����(ע�������������Ѿ���ѡ������Ҫ�ٴ��ƶ���)
			for (int i = 0; i < pDevice->m_legends.size(); i++)  //������ѭ���������еı�ѡ�е��ӿ���
			{
				AcDbObjectId legendId = NULL;

				//������ͨ��m_legends[i]->m_entityHandle �е��ӿ�ľ�� ��ÿ����ID 
				acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(legendId, false, pDevice->m_legends[i]->m_entityHandle);
				bool isMove = true;

				//�ж��Ƿ�ΪMOVE
				for (int u = 0; u < entityIds.length(); u++) {
					if (entityIds[u] == legendId) {
						isMove = false;
						break;
					}
				}
				if (isMove == false) {
					continue;
				}
				//�ж��Ƿ�ΪMOVE   //isMove  ???????


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
			//������ѡ��
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
			// ���ƫ�ƺ�λ��
			for (int i = 0; i < entityIds.length(); i++) {
				if (entityIds[i] != TEMP.moveBaseEntityId) {
					continue;
				}
				AcDbBlockReference* pBlockReference = NULL;
				if (acdbOpenObject(pBlockReference, entityIds[i], AcDb::kForRead) == Acad::eOk)
				{
					TEMP.rotateBasePoint3 = pBlockReference->position();
					// ����ƫת�Ƕ�
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
			// ����豸����
			RKTdevice* pDevice = new RKTdevice();
			pDevice->m_entityHandle = TEMP.moveBaseEntityId.handle();
			pDevice->read(TEMP.moveBaseEntityId);
			TEMP.New_angle = GetBlockAngle(entityIds[0]) * PI / 180;
			// ��ת�豸����(ע�������������Ѿ���ѡ������Ҫ�ٴ��ƶ���)
			RKTdevice* pDevice_temp = new RKTdevice();
			for (int i = 0; i < pDevice->m_legends.size(); i++)
			{
				AcDbObjectId legendId = NULL;
				acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(legendId, false, pDevice->m_legends[i]->m_entityHandle);
				// �ų��Ѿ�ѡ�еĻ���
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
				//��������ֵ-xyz-  ������չ���� 
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
				// ������ѡ��
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
				// ���λ��
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
				// ����豸����
				RKTdevice* pDevice = new RKTdevice();
				pDevice->m_entityHandle = TEMP.moveBaseEntityId.handle();
				pDevice->read(TEMP.moveBaseEntityId);
				// �ƶ��豸����(ע�������������Ѿ���ѡ������Ҫ�ٴ��ƶ���)
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
			// ������ѡ��
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
			// �ƶ��豸����(ע�������������Ѿ���ѡ������Ҫ�ٴ��ƶ���)
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
		// �ƶ��豸����(ע�������������Ѿ���ѡ������Ҫ�ٴ��ƶ���)
		for (int i = 0; i < pDevice->m_legends.size(); i++)
		{
			AcDbObjectId legendId = NULL;
			AcDbObjectId legendId_new = NULL;
			acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(legendId, false, pDevice->m_legends[i]->m_entityHandle);

			//�豸������CAD�Զ����Ƶģ��ӿ���Ҫ�ֶ�����
			legendId_new = ObjectArx::CopyEntity(legendId);
			//�õ��¿��legend��ID
			Legdend_Ids.append(legendId_new);
			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, legendId_new, AcDb::kForWrite) != Acad::eOk)
			{
				continue;
			}
			//�����¸��Ƴ����ӿ�����ڻ����λ�ã�������
			acutPrintf(_T(""));
			
			pBlockReference->setPosition(GetBlockPosition(TEMP.appendedObjectId) + AcGeVector3d(pDevice->m_legends[i]->m_x, pDevice->m_legends[i]->m_y, pDevice->m_legends[i]->m_z));
			//����ľ����ŵ�pLegend->m_entityHandle��ȥ
			pBlockReference->close();
		}
		for (int i = 0; i < pDevice->m_legends.size(); i++)
		{
			//�õ�������
			pDevice->m_legends[i]->m_deviceHandle = TEMP.appendedObjectId.handle();
			//�õ��Լ��ľ��
			pDevice->m_legends[i]->m_entityHandle = Legdend_Ids[i].handle();

			AcGePoint3d Base_dian(0, 0, 0);
			AcGePoint3d Base_legend(0, 0, 0);
			Base_dian = GetBlockPosition(TEMP.appendedObjectId);
			Base_legend = GetBlockPosition(Legdend_Ids[i]);
			//��������ֵ-xyz-  ������չ���� 
			pDevice->m_legends[i]->m_x = Base_legend.x - Base_dian.x;
			pDevice->m_legends[i]->m_y = Base_legend.y - Base_dian.y;
			pDevice->m_legends[i]->m_z = Base_legend.z - Base_dian.z;
		}
		//����
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
			//�豸������CAD�Զ����Ƶģ��ӿ���Ҫ�ֶ�����
			legendId_new = ObjectArx::CopyEntity(legendId);
			//�õ��¿��legend��ID
			Legdend_Ids.append(legendId_new);
			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, legendId_new, AcDb::kForWrite) != Acad::eOk)
			{
				continue;
			}
			//�����¸��Ƴ����ӿ�����ڻ����λ�ã�������
			pBlockReference->setPosition(GetBlockPosition(TEMP.appendedObjectId) + AcGeVector3d(pDevice->m_legends[i]->m_x, pDevice->m_legends[i]->m_y, pDevice->m_legends[i]->m_z));
			//����ľ����ŵ�pLegend->m_entityHandle��ȥ
			pBlockReference->close();
		}
		for (int i = 0; i < pDevice->m_legends.size(); i++)
		{
			//�õ�������
			pDevice->m_legends[i]->m_deviceHandle = TEMP.appendedObjectId.handle();
			//�õ��Լ��ľ��
			pDevice->m_legends[i]->m_entityHandle = Legdend_Ids[i].handle();

			AcGePoint3d Base_dian(0, 0, 0);
			AcGePoint3d Base_legend(0, 0, 0);
			Base_dian = GetBlockPosition(TEMP.appendedObjectId);
			Base_legend = GetBlockPosition(Legdend_Ids[i]);
			//��������ֵ-xyz-  ������չ���� 
			pDevice->m_legends[i]->m_x = Base_legend.x - Base_dian.x;
			pDevice->m_legends[i]->m_y = Base_legend.y - Base_dian.y;
			pDevice->m_legends[i]->m_z = Base_legend.z - Base_dian.z;
		}
		//����
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
		// �ƶ��豸����(ע�������������Ѿ���ѡ������Ҫ�ٴ��ƶ���)
		for (int i = 0; i < pDevice->m_legends.size(); i++)
		{
			AcDbObjectId legendId = NULL;
			AcDbObjectId legendId_new = NULL;
			acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(legendId, false, pDevice->m_legends[i]->m_entityHandle);

			//�豸������CAD�Զ����Ƶģ��ӿ���Ҫ�ֶ�����
			legendId_new = ObjectArx::CopyEntity(legendId);
			//�õ��¿��legend��ID
			Legdend_Ids.append(legendId_new);
			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, legendId_new, AcDb::kForWrite) != Acad::eOk)
			{
				continue;
			}
			//�����¸��Ƴ����ӿ�����ڻ����λ�ã�������
			acutPrintf(_T(""));

			pBlockReference->setPosition(GetBlockPosition(TEMP.appendedObjectId) + AcGeVector3d(pDevice->m_legends[i]->m_x, pDevice->m_legends[i]->m_y, pDevice->m_legends[i]->m_z));
			//����ľ����ŵ�pLegend->m_entityHandle��ȥ
			pBlockReference->close();
		}
		for (int i = 0; i < pDevice->m_legends.size(); i++)
		{
			//�õ�������
			pDevice->m_legends[i]->m_deviceHandle = TEMP.appendedObjectId.handle();
			//�õ��Լ��ľ��
			pDevice->m_legends[i]->m_entityHandle = Legdend_Ids[i].handle();

			AcGePoint3d Base_dian(0, 0, 0);
			AcGePoint3d Base_legend(0, 0, 0);
			Base_dian = GetBlockPosition(TEMP.appendedObjectId);
			Base_legend = GetBlockPosition(Legdend_Ids[i]);
			//��������ֵ-xyz-  ������չ���� 
			pDevice->m_legends[i]->m_x = Base_legend.x - Base_dian.x;
			pDevice->m_legends[i]->m_y = Base_legend.y - Base_dian.y;
			pDevice->m_legends[i]->m_z = Base_legend.z - Base_dian.z;
		}
		//����
		pDevice->save(TEMP.appendedObjectId);
		delete pDevice;
	}

}
void _move_demo::command() {
	do {
		ads_point pt;
		ads_name entName;
		if (acedEntSel(_T("\n��ѡ��һ���豸����"), entName, pt) != RTNORM)
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
		// �����õ��Ļ������
		
		deviceTypes.push_back(_T("��ˮ"));
		deviceTypes.push_back(_T("��ˮǽ��"));
		deviceTypes.push_back(_T("��ˮ�س�"));
		deviceTypes.push_back(_T("������ˮ"));
		deviceTypes.push_back(_T("��ˮǽ��")); 
		deviceTypes.push_back(_T("�������λ"));
		deviceTypes.push_back(_T("�������λ"));
		deviceTypes.push_back(_T("ȼ���ӵ�"));

		ads_name adsName;
		if (acedSSGet(NULL, NULL, NULL, NULL, adsName) != 5100)
		{
			break;
		}
		int len = 0;
		//�����ǻ�ȡѡ��Ķ���ĳ��ȣ���������ѭ������
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
		// �������Ļ���
		for (int i = 0; i < entityIds.length(); i++)
		{
			AcDbBlockReference* pBlockReference = NULL;

			//�����ǻ�ȡ��Ĳ��� 
			if (acdbOpenObject(pBlockReference, entityIds[i], AcDb::kForRead) != Acad::eOk)
			{
				continue;
			}
			// ��ÿ�����
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
				//��ȡ����,���������ж��Ƿ�Ϊ����
				pBlockTableRecord->close();
			}
			// �ж��Ƿ��ǻ���
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
			//deviceType�Ǹ����� �����������Ҫ�Ļ�������  
			AcGePoint3d Base_dian(0, 0, 0);
			AcGePoint3d Base_legend(0, 0, 0);
			Base_dian = GetBlockPosition(deviceId);
			Base_legend = GetBlockPosition(entityIds[i]);

			RKTLegend* pLegend = new RKTLegend();
			
			//����ľ����ŵ�pLegend->m_entityHandle��ȥ
			pLegend->m_entityHandle = entityIds[i].handle();
			/////////-----------------------------------//
			pLegend->m_x = Base_legend.x - Base_dian.x;
			pLegend->m_y = Base_legend.y - Base_dian.y;
			pLegend->m_z = Base_legend.z - Base_dian.z;
			/////////-----------------------------------//
			AcGePoint3d teMp(pLegend->m_x, pLegend->m_y, pLegend->m_z);
			TEMP.disaPlace.append(teMp);
			/////////-----------------------------------//
			//Ȼ�󽫿�ľ����ŵ�m_legends ������ȥ 
			pDevice->m_legends.push_back(pLegend);
			pBlockReference->close();
		}

		//��pDevice��ŵ��������չ������ȥ  pDevice�з����Լ��ľ�����������ӿ��һ��������� 
		pDevice->save(deviceId);
		delete pDevice;


	} while (false);

}