//获取块的名称 
#include "pch.h"
#include <dbidar.h>
#include <arxHeaders.h>
#include "_Func_.h"
#include "3rdparty/rapidJson/document.h"
#include "3rdparty/rapidJson/writer.h"
#include "3rdparty/rapidJson/prettywriter.h"
#define RKTDEVICE _T("RKTDEVICE")
#define RKTLEGEND _T("RKTLEGEND")
CString GetBlockName(AcDbObjectId entityId)
{
	AcDbBlockReference* pBlockReference = NULL;
	if (acdbOpenObject(pBlockReference, entityId, AcDb::OpenMode::kForRead) != Acad::eOk)
	{
		return _T("");
	}
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
		pBlockTableRecord->close();
	}
	pBlockReference->close();
	return CString(blockName);
}

//通过ID获取基点位置
AcGePoint3d GetBlockPosition(AcDbObjectId entityId) {
	AcDbBlockReference* pBlockReference = NULL;
	if (acdbOpenObject(pBlockReference, entityId, AcDb::OpenMode::kForRead) != Acad::eOk)
	{
		return AcGePoint3d();
	}
	AcGePoint3d position = pBlockReference->position();
	pBlockReference->close();
	return position;
}


//遍历获取实体
AcDbObjectIdArray GetEntity_temp(AcDbObjectIdArray& entityIds)
{
	do {
		int length = 0;
		ads_name adsName;

		if (acedSSGet(NULL, NULL, NULL, NULL, adsName) == 5100) {
			acedSSLength(adsName, &length);

			CString str = _T("");
			str.Format(_T("选择集个数：%d"), length);
			acutPrintf(str);
			acutPrintf(_T("\n测试命令\n"));
			//创建 entityIds的组
			for (int i = 0; i < length; i++)
			{
				// 获得一个实体
				ads_name entName;
				acedSSName(adsName, i, entName);
				// 获得实体的AcDbObjectId
				AcDbObjectId objId;
				acdbGetObjectId(objId, entName);
				entityIds.append(objId);
			}
		}
		acedSSFree(adsName);
	} while (false);
	return entityIds;
}


//_T(p)模式 遍历获取实体
AcDbObjectIdArray _T_GetEntity_temp(AcDbObjectIdArray& entityIds)
{
	// 获得当前被选择对象
	ads_name adsName;
	if (acedSSGet(_T("P"), NULL, NULL, NULL, adsName) == 5100)
	{
		int len = 0;

		//获取被选中对象的len 
		acedSSLength(adsName, &len);
		for (int i = 0; i < len; i++)
		{
			ads_name ename;
			acedSSName(adsName, i, ename);
			AcDbObjectId ObjectId = NULL;
			acdbGetObjectId(ObjectId, ename);

			//被选中的对象添加到数组entityIds 中去
			entityIds.append(ObjectId);
		}
		acedSSFree(adsName);
	}
	return entityIds;
}


//_T(I)模式 遍历获取实体
AcDbObjectIdArray _I_GetEntity_temp(AcDbObjectIdArray& entityIds)
{
	ads_name adsName;
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
	return entityIds;
}

//判断扩展数据是否存在
bool IsExtendData(AcDbEntity* _pEnt, CString _StrName)
{
	AcDbObjectId DictionaryId = _pEnt->extensionDictionary();
	//查看实体是否包含扩展字典
	if (DictionaryId == NULL)
	{
		return 0;
	}
	//打开扩展字典,获得指定名称的扩展记录
	AcDbDictionary* pDictionary = NULL;
	if (acdbOpenObject(pDictionary, DictionaryId, AcDb::kForRead) != 0)
	{
		return 0;
	}
	AcDbXrecord* pXrec = NULL;
	if (pDictionary->getAt(_StrName, (AcDbObject*&)pXrec, AcDb::kForRead) != 0)
	{
		pDictionary->close();
		return 0;
	}
}


bool IsExtendData(AcDbObjectId _ObjectId, CString _StrName)
{
	{
		bool IsErr = false;


		do
		{
			//打开图形对象,获得实体扩展字典的ObjectId
			AcDbEntity* pEnt = NULL;
			if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForWrite) != 0)
			{
				break;
			}
			AcDbObjectId DictionaryId = pEnt->extensionDictionary();
			pEnt->close();


			//查看实体是否包含扩展字典
			if (DictionaryId == NULL)
			{
				break;
			}


			//打开扩展字典,获得指定名称的扩展记录
			AcDbDictionary* pDictionary = NULL;
			if (acdbOpenObject(pDictionary, DictionaryId, AcDb::kForRead) != 0)
			{
				break;
			}


			AcDbXrecord* pXrec = NULL;
			if (pDictionary->getAt(_StrName, (AcDbObject*&)pXrec, AcDb::kForRead) != 0)
			{
				pDictionary->close();
				break;
			}


			pDictionary->close();
			pXrec->close();

			IsErr = true;
		} while (false);


		return IsErr;
	}
}

double GetBlockAngle(AcDbObjectId ObjectId)
{
	//打开块表记录获得指针
	AcDbBlockReference* pBlkRef = NULL;
	if (acdbOpenObject(pBlkRef, ObjectId, AcDb::kForRead) != 0)
	{
		return 0.0;
	}
	//获得点
	double Angle = pBlkRef->rotation();
	Angle = 180.0 / PI * Angle;
	//释放内存
	pBlkRef->close();
	return Angle;
}


bool isClockwiseRotation(AcGeVector3d v1, AcGeVector3d v2, AcGeVector3d v3) {
	double crossProduct = v1.x * v2.y - v1.y * v2.x;
	return crossProduct < 0;
}


std::string wstring_to_string(const std::wstring& ws)
{
	std::string strLocale = setlocale(LC_ALL, "");
	const wchar_t* wchSrc = ws.c_str();
	size_t nDestSize = wcstombs(NULL, wchSrc, (size_t)0) + 1;
	char* chDest = new char[nDestSize];
	memset(chDest, 0, nDestSize);
	wcstombs(chDest, wchSrc, (size_t)nDestSize);
	std::string strResult = chDest;
	delete[] chDest;
	setlocale(LC_ALL, strLocale.c_str());
	return strResult;
}

RKTLegend::RKTLegend() {
	this->m_entityHandle = 0;
	this->m_deviceHandle = 0;
	this->m_x = 0.0;
	this->m_y = 0.0;
	this->m_z = 0.0;
}
//析构函数
RKTLegend::~RKTLegend() {

}

RKTdevice::RKTdevice()
{
	this->m_entityHandle = 0;
}

RKTdevice::~RKTdevice()
{
	for (int u = this->m_legends.size() - 1; u >= 0; u--) {
		delete this->m_legends[u];
	}
	this->m_legends.clear();
}


void RKTLegend::save(AcDbObjectId id) {
	do {

		rapidjson::GenericStringBuffer<rapidjson::UTF8<char>> StrJsonBuff;
		rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF8<char>>>* pJson = new rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF8<char>>>(StrJsonBuff);

		pJson->StartObject();
		pJson->Key("handle");
		pJson->Int64(this->m_entityHandle);
		pJson->Key("deviceHandle");
		pJson->Int64(this->m_deviceHandle);
		pJson->Key("pos_x");
		pJson->Int64(this->m_x);
		pJson->Key("pos_y");
		pJson->Int64(this->m_y);
		pJson->Key("pos_z");
		pJson->Int64(this->m_z);



		pJson->EndObject();

		CString data(StrJsonBuff.GetString());
		delete pJson;


		AcDbEntity* pEntity = NULL;
		if (acdbOpenObject(pEntity, id, AcDb::kForWrite) != Acad::eOk)
		{
			break;
		}

		// 判断是否存在扩展字典,如果不存在则创建
		AcDbObjectId dictionaryId = pEntity->extensionDictionary();
		if (dictionaryId == NULL)
		{
			pEntity->createExtensionDictionary();
			dictionaryId = pEntity->extensionDictionary();
		}
		pEntity->close();

		AcDbObjectId RecObjectId = NULL;
		AcDbDictionary* pDictionary;
		if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForWrite) != 0)
		{
			break;
		}

		// 判断是否存在扩展记录,如果不存在则创建
		AcDbObjectId xrecordId = NULL;
		pDictionary->getAt(RKTDEVICE, xrecordId);

		AcDbXrecord* pXrec = NULL;
		if (xrecordId == NULL) {
			pXrec = new AcDbXrecord();
		}
		else {
			acdbOpenObject(pXrec, xrecordId, AcDb::kForWrite);
		}
		struct resbuf* pRb;
		pRb = acutBuildList(AcDb::kDxfRegAppName, RKTLEGEND, AcDb::kDxfXdAsciiString, data, RTNONE);
		pXrec->setFromRbChain(*pRb);
		pDictionary->setAt(RKTLEGEND, pXrec, xrecordId);
		pXrec->close();

		pDictionary->close();

	} while (false);
}

void RKTLegend::read(AcDbObjectId id) {
	do {

		AcDbEntity* pEntity = NULL;
		if (acdbOpenObject(pEntity, id, AcDb::kForWrite) != Acad::eOk)
		{
			break;
		}

		// 判断是否存在扩展字典
		AcDbObjectId dictionaryId = pEntity->extensionDictionary();
		pEntity->close();
		if (dictionaryId == NULL)
		{
			break;
		}

		AcDbObjectId RecObjectId = NULL;
		AcDbDictionary* pDictionary;
		if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForWrite) != 0)
		{
			break;
		}

		// 获得扩展记录
		AcDbObjectId xrecordId = NULL;
		pDictionary->getAt(RKTLEGEND, xrecordId);
		pDictionary->close();
		if (xrecordId == NULL)
		{
			break;
		}

		// 读取扩展记录
		AcDbXrecord* pXrec = NULL;
		if (acdbOpenObject(pXrec, xrecordId, AcDb::kForWrite) != 0)
		{
			break;
		}

		// 获得数据
		struct resbuf* pRb;
		pXrec->rbChain(&pRb);
		CString data;
		while (true)
		{
			if (pRb == NULL)
			{
				break;
			}
			if (pRb->restype == AcDb::kDxfXdAsciiString)
			{
				if (pRb->resval.rstring != RKTDEVICE)
				{
					data = pRb->resval.rstring;
				}
			}
			pRb = pRb->rbnext;
		}
		acutRelRb(pRb);

		pXrec->close();
		if (data.GetLength() == 0) {
			break;
		}

		// wstring转换为string
		std::string dataUtf8 = wstring_to_string(data.GetBuffer());
		data.ReleaseBuffer();

		// 解析JSON数据
		rapidjson::GenericDocument<rapidjson::UTF8<char>>* pDocument = new rapidjson::GenericDocument<rapidjson::UTF8<char>>();
		pDocument->Parse(dataUtf8.c_str());
		if (pDocument->IsObject() == true)
		{
			for (rapidjson::GenericValue<rapidjson::UTF8<char>>::ConstMemberIterator obj = pDocument->MemberBegin(); obj != pDocument->MemberEnd(); ++obj)
			{
				if (obj->name == "handle") {
					this->m_entityHandle = obj->value.GetInt64();
				}
				else if (obj->name == "deviceHandle") {
					this->m_deviceHandle = obj->value.GetInt64();
				}
				else if (obj->name == "pos_x"){
					this->m_x = obj->value.GetInt64();
				}
				else if (obj->name == "pos_y") {
					this->m_y = obj->value.GetInt64();
				}
				else if (obj->name == "pos_z") {
					this->m_z = obj->value.GetInt64();
				}
			}
		}
		delete pDocument;

	} while (false);
}

void RKTdevice::save(AcDbObjectId id)
{
	do {

		for (int i = 0; i < this->m_legends.size(); i++) {
			this->m_legends[i]->m_deviceHandle = this->m_entityHandle;

			AcDbObjectId legendId;
			acdbHostApplicationServices()->workingDatabase()->getAcDbObjectId(legendId, false, this->m_legends[i]->m_entityHandle);
			this->m_legends[i]->save(legendId);
		}

		rapidjson::GenericStringBuffer<rapidjson::UTF8<char>> StrJsonBuff;
		rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF8<char>>>* pJson = new rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF8<char>>>(StrJsonBuff);

		pJson->StartObject();
		pJson->Key("legends");
		pJson->StartArray();
		for (int i = 0; i < this->m_legends.size(); i++)
		{
			unsigned __int64 handle = this->m_legends[i]->m_entityHandle;

			pJson->StartObject();
			pJson->Key("handle");
			pJson->Int64(handle);
			pJson->Key("deviceHandle");
			pJson->Int64(this->m_legends[i]->m_deviceHandle);
			pJson->Key("pos_x");
			pJson->Int64(this->m_legends[i]->m_x);
			pJson->Key("pos_y");
			pJson->Int64(this->m_legends[i]->m_y);
			pJson->Key("pos_z");
			pJson->Int64(this->m_legends[i]->m_z);
			pJson->EndObject();
		}
		pJson->EndArray();
		pJson->EndObject();

		CString data(StrJsonBuff.GetString());
		delete pJson;


		AcDbEntity* pEntity = NULL;
		if (acdbOpenObject(pEntity, id, AcDb::kForWrite) != Acad::eOk)
		{
			break;
		}

		// 判断是否存在扩展字典,如果不存在则创建
		AcDbObjectId dictionaryId = pEntity->extensionDictionary();
		if (dictionaryId == NULL)
		{
			pEntity->createExtensionDictionary();
			dictionaryId = pEntity->extensionDictionary();
		}
		pEntity->close();

		AcDbObjectId RecObjectId = NULL;
		AcDbDictionary* pDictionary;
		if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForWrite) != 0)
		{
			break;
		}

		// 判断是否存在扩展记录,如果不存在则创建
		AcDbObjectId xrecordId = NULL;
		pDictionary->getAt(RKTDEVICE, xrecordId);

		AcDbXrecord* pXrec = NULL;
		if (xrecordId == NULL) {
			pXrec = new AcDbXrecord();
		}
		else {
			acdbOpenObject(pXrec, xrecordId, AcDb::kForWrite);
		}
		struct resbuf* pRb;
		pRb = acutBuildList(AcDb::kDxfRegAppName, RKTDEVICE, AcDb::kDxfXdAsciiString, data, RTNONE);
		pXrec->setFromRbChain(*pRb);
		pDictionary->remove(RKTDEVICE);
		pDictionary->setAt(RKTDEVICE, pXrec, xrecordId);
		pXrec->close();

		pDictionary->close();

	} while (false);
}

void RKTdevice::read(AcDbObjectId id)
{
	do {

		AcDbEntity* pEntity = NULL;
		if (acdbOpenObject(pEntity, id, AcDb::kForWrite) != Acad::eOk)
		{
			break;
		}

		// 判断是否存在扩展字典
		AcDbObjectId dictionaryId = pEntity->extensionDictionary();
		pEntity->close();
		if (dictionaryId == NULL)
		{
			break;
		}

		AcDbObjectId RecObjectId = NULL;
		AcDbDictionary* pDictionary;
		if (acdbOpenObject(pDictionary, dictionaryId, AcDb::kForWrite) != 0)
		{
			break;
		}

		// 获得扩展记录
		AcDbObjectId xrecordId = NULL;
		pDictionary->getAt(RKTDEVICE, xrecordId);
		pDictionary->close();
		if (xrecordId == NULL)
		{
			break;
		}

		// 读取扩展记录
		AcDbXrecord* pXrec = NULL;
		if (acdbOpenObject(pXrec, xrecordId, AcDb::kForWrite) != 0)
		{
			break;
		}

		// 获得数据
		struct resbuf* pRb;
		pXrec->rbChain(&pRb);
		CString data;
		while (true)
		{
			if (pRb == NULL)
			{
				break;
			}
			if (pRb->restype == AcDb::kDxfXdAsciiString)
			{
				if (pRb->resval.rstring != RKTDEVICE)
				{
					data = pRb->resval.rstring;
				}
			}
			pRb = pRb->rbnext;
		}
		acutRelRb(pRb);

		pXrec->close();
		if (data.GetLength() == 0) {
			break;
		}

		// wstring转换为string
		std::string dataUtf8 = wstring_to_string(data.GetBuffer());
		data.ReleaseBuffer();

		// 解析JSON数据
		rapidjson::GenericDocument<rapidjson::UTF8<char>>* pDocument = new rapidjson::GenericDocument<rapidjson::UTF8<char>>();
		pDocument->Parse(dataUtf8.c_str());
		if (pDocument->IsObject() == true)
		{
			for (rapidjson::GenericValue<rapidjson::UTF8<char>>::ConstMemberIterator obj = pDocument->MemberBegin(); obj != pDocument->MemberEnd(); ++obj)
			{
				if (obj->name == "legends") {
					for (rapidjson::GenericValue<rapidjson::UTF8<char>>::ConstValueIterator objLegend = obj->value.GetArray().Begin(); objLegend != obj->value.GetArray().End(); ++objLegend)
					{
						RKTLegend* pLegend = new RKTLegend();
						for (rapidjson::GenericValue<rapidjson::UTF8<char>>::ConstMemberIterator objLegendItem = objLegend->MemberBegin(); objLegendItem != objLegend->MemberEnd(); ++objLegendItem)
						{
							if (objLegendItem->name == "handle") {
								pLegend->m_entityHandle = objLegendItem->value.GetInt64();
							}
							else if (objLegendItem->name == "deviceHandle") {
								pLegend->m_deviceHandle = objLegendItem->value.GetInt64();
							}
							else if (objLegendItem->name == "pos_x") {
								pLegend->m_x = objLegendItem->value.GetInt64();
							}
							else if (objLegendItem->name == "pos_y") {
								pLegend->m_y = objLegendItem->value.GetInt64();
							}
							else if (objLegendItem->name == "pos_z") {
								pLegend->m_z = objLegendItem->value.GetInt64();
							}
						}
						this->m_legends.push_back(pLegend);
					}
				}
			}
		}
		delete pDocument;

	} while (false);
}