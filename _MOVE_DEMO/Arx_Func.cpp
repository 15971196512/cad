#include "pch.h"
#include "Arx_Func.h"









//���CAD����
HWND ObjectArx::GetAutoCADWindow()
{
	CWnd* Cwhd = acedGetAcadFrame();
	return Cwhd->GetSafeHwnd();
}



//����CAD����(��ģ̬)
int ObjectArx::SendMessageAutoCAD(CString StrCommand)
{
	acDocManager->sendStringToExecute(curDoc(), StrCommand);
	return 0;
}


//�������·��
CString ObjectArx::GetWindowPath()
{
	CString StrPath = _T("");


	wchar_t* pPath = new wchar_t[1024];
	SHGetSpecialFolderPath(0, pPath, CSIDL_DESKTOPDIRECTORY, 0);
	StrPath = pPath;
	delete pPath;


	return StrPath;
}

//���acad��װ·��
CString ObjectArx::GetInstallationPath()
{
	CString strPath;
	HKEY hKey;
	DWORD dwType = REG_SZ;
	DWORD dwSize = MAX_PATH;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Autodesk\\AutoCAD"), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, _T("LastestVersion"), NULL, &dwType, (LPBYTE)strPath.GetBuffer(dwSize), &dwSize) == ERROR_SUCCESS)
		{
			strPath.ReleaseBuffer();
			strPath += _T("\\ApplicationPlugins");
		}
		RegCloseKey(hKey);
	}
	return strPath;
}

//ȥ��������
bool ObjectArx::RemoveEduPlotStamp(AcDbDatabase* _pDatabase)
{
	bool IsErr = false;


#if OBJECTARX2014 || OBJECTARX2015 || OBJECTARX2016 || OBJECTARX2017


	do
	{
		//����ǿ�ָ������ǽ����棬�򷵻�
		if (_pDatabase == NULL || !_pDatabase->isEMR())
		{
			break;
		}


		INT_PTR d1 = (INT_PTR)_pDatabase;
		//�Դ����ݿ�ָ�����һ����ַ����mpImpDb��ַ,ʵ������һ������ָ��
#ifdef _WIN64
		d1 += 0x00000008;
#else
		d1 += 0x00000004;
#endif

		INT_PTR d2 = (INT_PTR)(*((void**)d1));			//ȡ���ڲ���mpImpDb��ַ
		for (int i = 0x600; i < 0x1000; i++)
		{
			INT_PTR* pIsEMR = (INT_PTR*)(d2 + i);		//ȡ�ô˵�ַ������
			if ((*pIsEMR) == 0x01)						//����˵�ַ��������0x1:������ 
			{
				*pIsEMR = 0x00;							//תΪ0x0:�ǽ�����,
				if (!_pDatabase->isEMR())
				{
					IsErr = true;
					break;
				}
				else
				{
					*pIsEMR = 0x01;						//���ɹ�������ԭ����
				}
			}
		}

	} while (false);


#endif

	return IsErr;
}





/*--------------------------------------------------------------------------------------------------------------------------------------------*/












//��������
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



//������
AcDbObjectId ObjectArx::CreatePoint(AcGePoint3d _PointPos)
{
	AcDbPoint* pPoint = new AcDbPoint(_PointPos);
	return AddObjectToModl(pPoint);
}



/*
_StartPoint:���
_EndPoint:�˵�
*/
//����ֱ��
AcDbObjectId ObjectArx::CreateLine(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint)
{
	AcDbLine* pLine = new AcDbLine(_StartPoint, _EndPoint);
	return AddObjectToModl(pLine);
}


//��������
AcDbObjectId ObjectArx::CreateRay(AcGePoint3d _BasePoint, AcGeVector3d _Vector) {

	AcDbObjectId RayId = NULL;

	do
	{
		AcDbRay* pLine = new AcDbRay();
		pLine->setBasePoint(_BasePoint);
		pLine->setUnitDir(_Vector);
		RayId = AddObjectToModl(pLine);

	} while (false);

	return RayId;

}
AcDbObjectId ObjectArx::CreateRay(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint) {
	return CreateRay(_StartPoint, AcGeVector3d(_EndPoint.x - _StartPoint.x, _EndPoint.y - _StartPoint.y, _EndPoint.z - _StartPoint.z));
}


//����������
AcDbObjectId ObjectArx::CreateXline(AcGePoint3d _BasePoint, AcGeVector3d _Vector)
{
	AcDbObjectId XlineId = NULL;

	do
	{
		AcDbXline* pLine = new AcDbXline();
		pLine->setBasePoint(_BasePoint);
		pLine->setUnitDir(_Vector);
		XlineId = AddObjectToModl(pLine);

	} while (false);

	return XlineId;
}
AcDbObjectId ObjectArx::CreateXline(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint)
{
	return CreateXline(_StartPoint, AcGeVector3d(_EndPoint.x - _StartPoint.x, _EndPoint.y - _StartPoint.y, _EndPoint.z - _StartPoint.z));
}



/*
_Center:Բ��
_Radius:�뾶
_StartAngle:��ʼ�Ƕ�
_EndAngle:��ֹ�Ƕ�
*/
//����Բ��
AcDbObjectId ObjectArx::CreateArc(AcGePoint3d _Center, double _Radius, double _StartAngle, double _EndAngle)
{
	if (_EndAngle == 360.0 && _StartAngle == 0.0)
	{
		AcGeVector3d Vector(0.0, 0.0, 1.0);
		AcDbCircle* pCircle = new AcDbCircle(_Center, Vector, _Radius);
		return AddObjectToModl(pCircle);
	}
	else
	{
		double ArcStart = _StartAngle * 3.14159265358 / 180;
		double ArcEnd = _EndAngle * 3.14159265358 / 180;
		AcDbArc* pArc = new AcDbArc(_Center, _Radius, ArcStart, ArcEnd);
		return AddObjectToModl(pArc);
	}
	return 0;
}

//���������
AcDbObjectId ObjectArx::CreatePolyline(AcGePoint2dArray _ListPoins) {

	AcGePoint3dArray Points;
	for (int i = 0; i < _ListPoins.length(); i++) {
		Points.append(AcGePoint3d(_ListPoins[i].x, _ListPoins[i].y, 0));
	}
	return ObjectArx::CreatePolyline(Points);
}
AcDbObjectId ObjectArx::CreatePolyline(AcGePoint3dArray ListPoins)
{
	AcDbPolyline* pPolyline = new AcDbPolyline(ListPoins.length());
	for (int i = 0; i < ListPoins.length(); i++)
	{
		pPolyline->addVertexAt(i, AcGePoint2d(ListPoins.at(i).x, ListPoins.at(i).y), 0, 0.0, 0.0);
	}
	return AddObjectToModl(pPolyline);
}

//������������
AcDbObjectId ObjectArx::CreateSpline(AcGePoint3dArray _points)
{
	AcDbSpline* pSpline = new AcDbSpline(_points);
	return AddObjectToModl(pSpline);
}


//��������
AcDbObjectIdArray ObjectArx::CreateRegion(AcDbObjectIdArray _ListObjects)
{
	AcDbObjectIdArray ListRegionIds;

	AcDbEntity* pEnt;


	//������ʵ��ת��������
	AcDbVoidPtrArray ListCurvePointers;
	for (int i = 0; i < _ListObjects.length(); i++)
	{
		if (acdbOpenAcDbEntity(pEnt, _ListObjects.at(i), AcDb::kForRead) == 0)
		{
			if (pEnt->isKindOf(AcDbCurve::desc()))
			{
				ListCurvePointers.append(static_cast<void*>(pEnt));
			}
		}
	}


	//��������
	AcDbObjectId RegionId;
	AcDbRegion* pRegion;
	AcDbVoidPtrArray ListRegionPointers;
	int Err = AcDbRegion::createFromCurves(ListCurvePointers, ListRegionPointers);
	if (Err == 0)
	{
		//�����ɵ�������ӵ�ģ�Ϳռ�
		for (int i = 0; i < ListRegionPointers.length(); i++)
		{
			//����ָ��(��ָ���κ�����)ת��Ϊָ�������ָ��
			pRegion = static_cast<AcDbRegion*>(ListRegionPointers[i]);
			pRegion->setDatabaseDefaults();
			RegionId = AddObjectToModl(pRegion);
			ListRegionIds.append(RegionId);
		}
	}
	else //����������ɹ���ҲҪɾ���Ѿ����ɵ�����
	{
		for (int i = 0; i < ListRegionPointers.length(); i++)
		{
			delete (AcRxObject*)ListRegionPointers[i];
		}
	}
	//�ر���Ϊ�߽�Ķ���
	for (int i = 0; i < ListCurvePointers.length(); i++)
	{
		pEnt = static_cast<AcDbEntity*>(ListCurvePointers[i]);
		pEnt->close();
	}
	return ListRegionIds;
}
AcDbObjectIdArray ObjectArx::CreateRegion(AcDbObjectId _PolylineId)
{
	AcDbObjectIdArray ListObjects = NULL;
	ListObjects.append(_PolylineId);
	return CreateRegion(ListObjects);
}



//��������
AcDbObjectId ObjectArx::CreateExtrude(AcDbObjectId _RegionId, double _Height, double _TaperAngle)
{
	AcDbObjectId ObjectId = NULL;
	AcDbRegion* pRegion = NULL;
	if (acdbOpenObject(pRegion, _RegionId, AcDb::kForRead) == 0)
	{
		AcDb3dSolid* pSolid = new AcDb3dSolid();
		pSolid->extrude(pRegion, _Height, _TaperAngle);
		ObjectId = AddObjectToModl(pSolid);
		pRegion->close();
	}
	return ObjectId;
}




//��������
AcDbObjectId ObjectArx::CreateText(AcGePoint3d _PointPos, CString _StrText, double _Height, double _Rotation, CString _TextStyle)
{
	AcDbObjectId TextId = NULL;

	do
	{
		//���������ʽ
		AcDbObjectId TextStyleId = GetTextStyleId(_TextStyle);


		//��������
		AcDbText* pText = new AcDbText(_PointPos, _StrText, TextStyleId, _Height, 3.14159265358 / 180 * _Rotation);
		if (pText == NULL)
		{
			break;
		}


		//����������ʽ
		pText->setTextStyle(TextStyleId);


		//��ӵ�ģ�Ϳռ�
		TextId = AddObjectToModl(pText);


	} while (false);


	return TextId;
}


AcDbObjectId ObjectArx::CreateMText(AcGePoint3d _PointPos, CString _StrText, double _Height, double _Rotation, CString _TextStyle)
{
	AcDbObjectId TextId = NULL;

	do
	{
		//���������ʽ
		AcDbObjectId TextStyleId = GetTextStyleId(_TextStyle);


		//��������
		AcDbMText* pText = new AcDbMText();
		if (pText == NULL)
		{
			break;
		}

		pText->setLocation(_PointPos);
		pText->setContents(_StrText);
		pText->setTextHeight(_Height);
		pText->setRotation(3.1415926 / 180 * _Rotation);


		//����������ʽ
		pText->setTextStyle(TextStyleId);


		//��ӵ�ģ�Ϳռ�
		TextId = AddObjectToModl(pText);


	} while (false);


	return TextId;
}


//��������
AcDbObjectId ObjectArx::CreateLeader(AcGePoint3d StartPoint, AcGePoint3d EndPoint, AcGePoint3d LeadPoint)
{
	AcDbObjectId LeaderId = NULL;


	do
	{
		AcDbLeader* pLeader = new AcDbLeader();
		pLeader->appendVertex(StartPoint);
		pLeader->appendVertex(EndPoint);
		if (EndPoint.x == LeadPoint.x && EndPoint.y == LeadPoint.y && EndPoint.z == LeadPoint.z)
		{
			LeaderId = AddObjectToModl(pLeader);
		}
		else
		{
			pLeader->appendVertex(LeadPoint);

			LeaderId = AddObjectToModl(pLeader);
		}

	} while (false);


	return LeaderId;
}



//����������
void ObjectArx::SetHatchPatternScale(AcDbObjectId _HatchId, double _Scale)
{
	do
	{
		AcDbHatch* pHatch = NULL;
		if (acdbOpenObject(pHatch, _HatchId, AcDb::OpenMode::kForWrite) != 0)
		{
			break;
		}


		pHatch->setPatternScale(_Scale);


		pHatch->close();

	} while (false);
}



//�������߼�ͷ�ߴ�
void ObjectArx::SetLeaderArrowSize(AcDbObjectId _ObjectId, double _Size)
{
	AcDbLeader* pLeader = NULL;
	if (acdbOpenObject(pLeader, _ObjectId, AcDb::kForWrite) == 0)
	{
		pLeader->setDimasz(_Size);
		pLeader->close();
	}
}



/*
"":ʵ�ıպ�
"_DOT":��
"_DOTSMALL":С��
"_DOTBLANK":���ĵ�
"_ORIGIN":ԭ����
"_ORIGIN2":ԭ���� 2
"_OPEN":��
"_OPEN90":ֱ��
"_OPEN30":30 �Ƚ�
"_CLOSED":�պ�
"_SMALL":����С��
"_NONE":��
"_OBLIQUE":��б
"_BOXFILLED":ʵ�Ŀ�
"_BOXBLANK":��
"_CLOSEDBLANK":���ıպ�
"_DATUMFILLED":ʵ�Ļ�׼������
"_DATUMBLANK":��׼������
"_INTEGRAL":�������
"_ARCHTICK":�������
*/
//�������߼�ͷ
void ObjectArx::SetLeaderArrowStyle(AcDbObjectId _ObjectId, CString _StyleName)
{
	AcDbLeader* pLeader = NULL;
	if (acdbOpenObject(pLeader, _ObjectId, AcDb::kForWrite) == 0)
	{
		if (_StyleName == _T(""))
		{
			_StyleName = _T("");
		}
		else if (_StyleName == _T("��"))
		{
			_StyleName = _T("_DOT");
		}
		else if (_StyleName == _T("С��"))
		{
			_StyleName = _T("_DOTSMALL");
		}
		else if (_StyleName == _T("���ĵ�"))
		{
			_StyleName = _T("_DOTBLANK");
		}
		else if (_StyleName == _T("ԭ����"))
		{
			_StyleName = _T("_ORIGIN");
		}
		else if (_StyleName == _T("ԭ����2"))
		{
			_StyleName = _T("_ORIGIN2");
		}
		else if (_StyleName == _T("��"))
		{
			_StyleName = _T("_OPEN");
		}
		else if (_StyleName == _T("ֱ��"))
		{
			_StyleName = _T("_OPEN90");
		}
		else if (_StyleName == _T("30�Ƚ�"))
		{
			_StyleName = _T("_OPEN30");
		}
		else if (_StyleName == _T("�պ�"))
		{
			_StyleName = _T("_CLOSED");
		}
		else if (_StyleName == _T("����С��"))
		{
			_StyleName = _T("_SMALL");
		}
		else if (_StyleName == _T("��"))
		{
			_StyleName = _T("_NONE");
		}
		else if (_StyleName == _T("��б"))
		{
			_StyleName = _T("_OBLIQUE");
		}
		else if (_StyleName == _T("ʵ�Ŀ�"))
		{
			_StyleName = _T("_BOXFILLED");
		}
		else if (_StyleName == _T("��"))
		{
			_StyleName = _T("_BOXBLANK");
		}
		else if (_StyleName == _T("���ıպ�"))
		{
			_StyleName = _T("_CLOSEDBLANK");
		}
		else if (_StyleName == _T("ʵ�Ļ�׼������"))
		{
			_StyleName = _T("_DATUMFILLED");
		}
		else if (_StyleName == _T("��׼������"))
		{
			_StyleName = _T("_DATUMBLANK");
		}
		else if (_StyleName == _T("�������"))
		{
			_StyleName = _T("_INTEGRAL");
		}
		else if (_StyleName == _T("�������"))
		{
			_StyleName = _T("_ARCHTICK");
		}

		pLeader->setDimldrblk(_StyleName);
		pLeader->close();
	}
}



//������ֻ���
void ObjectArx::SetTextPoint(AcDbObjectId _ObjectId, AcGePoint3d _PointPos)
{
	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		AcDbText* pText = AcDbText::cast(pEnt);
		if (pText != NULL)
		{
			pText->setPosition(_PointPos);
		}


		AcDbMText* pMText = AcDbMText::cast(pEnt);
		if (pMText != NULL)
		{
			pMText->setLocation(_PointPos);
		}


		pEnt->close();


	} while (false);
}


//�������ָ߶�
void ObjectArx::SetTextHeight(AcDbObjectId _ObjectId, double _Height)
{
	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		AcDbText* pText = AcDbText::cast(pEnt);
		if (pText != NULL)
		{
			pText->setHeight(_Height);
		}


		AcDbMText* pMText = AcDbMText::cast(pEnt);
		if (pMText != NULL)
		{
			pMText->setTextHeight(_Height);
		}


		pEnt->close();


	} while (false);
}


//�������ֿ��
void ObjectArx::SetTextWidth(AcDbObjectId _ObjectId, double _Width)
{
	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		AcDbMText* pMText = AcDbMText::cast(pEnt);
		if (pMText != NULL)
		{
			pMText->setWidth(_Width);
		}


		pEnt->close();


	} while (false);
}

//����������ʽ
void ObjectArx::SetTextStyle(AcDbObjectId _ObjectId, CString _TextStyle)
{
	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		AcDbText* pText = AcDbText::cast(pEnt);
		if (pText != NULL)
		{
			pText->setTextStyle(GetTextStyleId(_TextStyle));
		}


		AcDbMText* pMText = AcDbMText::cast(pEnt);
		if (pMText != NULL)
		{
			pMText->setTextStyle(GetTextStyleId(_TextStyle));
		}


		pEnt->close();


	} while (false);
}


//������������
void ObjectArx::SetTextContent(AcDbObjectId _ObjectId, CString _Content)
{
	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		AcDbText* pText = AcDbText::cast(pEnt);
		if (pText != NULL)
		{
			pText->setTextString(_Content);
		}


		AcDbMText* pMText = AcDbMText::cast(pEnt);
		if (pMText != NULL)
		{
			pMText->setContents(_Content);
		}


		pEnt->close();


	} while (false);
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
















//������
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


//�ж��Ƿ���ʵ��
bool ObjectArx::IsEntity(AcDbObjectId _ObjectId)
{
	int Is = false;


	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
		{
			break;
		}


		Is = true;


		pEnt->close();


	} while (false);


	return Is;
}




//���ƶ���
AcDbObjectId ObjectArx::CopyEntity(AcDbObjectId _ObjectId)
{
	AcDbObjectId EntityId = NULL;


	do
	{

		AcDbEntity* pEntity = NULL;
		if (acdbOpenObject(pEntity, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}
		AcDbEntity* pTempEntity = AcDbEntity::cast(pEntity->clone());
		if (pTempEntity != NULL)
		{
			EntityId = AddObjectToModl(pTempEntity);
		}
		pEntity->close();


	} while (false);


	return EntityId;
}



//���ƶ���
AcDbObjectIdArray ObjectArx::CopyEntity(AcDbObjectIdArray _ListEntitys)
{
	AcDbObjectIdArray ListEntitys = NULL;
	for (int i = 0; i < _ListEntitys.length(); i++)
	{
		ListEntitys.append(CopyEntity(_ListEntitys[i]));
	}
	return ListEntitys;
}



//��������ƫ��
AcDbObjectIdArray ObjectArx::CreateOffset(AcDbObjectId _ObjectId, double _VarDeviation, AcDbDatabase* _pDatabase)
{
	AcDbObjectIdArray ListEntitys = NULL;


	do
	{
		//�򿪶���
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		if (pEnt->isKindOf(AcDbCurve::desc()) == true)
		{
			AcDbCurve* pCurve = (AcDbCurve*)pEnt;
			if (pCurve == NULL)
			{
				pEnt->close();
				break;
			}


			AcDbEntity* pTempEnt = NULL;
			AcDbVoidPtrArray tArr = NULL;
			pCurve->getOffsetCurves(_VarDeviation, tArr);
			for (int i = 0; i < tArr.length(); i++)
			{
				pTempEnt = (AcDbEntity*)tArr[i];
				ListEntitys.append(AddObjectToModl(pTempEnt));
			}


			//AcDbEntity *pTempEnt = NULL;
			//AcDbVoidPtrArray *ptArr = new AcDbVoidPtrArray();
			//pCurve->getOffsetCurves(_VarDeviation, *ptArr);
			//for (int i = 0; i < ptArr->length(); i++)
			//{
			//	pTempEnt = (AcDbEntity*)ptArr->at(i);
			//	ListEntitys.append(AddObjectToModl(pTempEnt));
			//}
		}
		pEnt->close();
	} while (false);


	return ListEntitys;
}



/*
_ObjectId:Ҫɾ���Ķ���Id
_ListObjects:��������
*/
//ɾ������
int ObjectArx::DeleteObject(AcDbObjectId _ObjectId)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
	{
		return -1;
	}
	if (pEnt->erase() != 0)
	{
		pEnt->close();
		return -2;
	}
	pEnt->close();
	return 0;
}
int ObjectArx::DeleteObject(AcDbObjectIdArray _ListObjects)
{
	AcDbEntity* pEnt = NULL;
	for (int i = 0; i < _ListObjects.length(); i++)
	{
		//�򿪶���
		if (acdbOpenObject(pEnt, _ListObjects[i], AcDb::kForWrite) != 0)
		{
			continue;
		}


		pEnt->erase();


		pEnt->close();
	}
	return 0;
}



//��ȡ������ɫ
int ObjectArx::GetObjectColor(AcDbObjectId _ObjectId)
{
	int Color = 0;


	do
	{
		//ע����ɫΪ1����256��1����飬256�����
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
		{
			break;
		}


		Color = pEnt->colorIndex();


		pEnt->close();


		if (Color == 256)
		{
			Color = GetLayerColor(GetObjectLayer(_ObjectId));
		}

	} while (false);


	return Color;
}



/*
_ObjectId:����Id
_Color:��ɫ���
_ListObjects:��������
*/
//���ö�����ɫ
int ObjectArx::SetObjectColor(AcDbObjectId _ObjectId, int _Color)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
	{
		return -1;
	}


	pEnt->setColorIndex(_Color);


	pEnt->close();
	return 0;
}
int ObjectArx::SetObjectColor(AcDbObjectIdArray _ListObjects, int _Color)
{
	AcDbEntity* pEnt = NULL;
	for (int i = 0; i < _ListObjects.length(); i++)
	{
		//�򿪶���
		if (acdbOpenObject(pEnt, _ListObjects[i], AcDb::kForWrite) != 0)
		{
			continue;
		}


		pEnt->setColorIndex(_Color);


		pEnt->close();
	}
	return 0;
}


void ObjectArx::SetViewportViewCenter(AcDbObjectId _ObjectId, AcGePoint2d _Center)
{
	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		pViewport->setViewCenter(_Center);


		pViewport->close();

	} while (false);
}



//�����ӿ���ͼ�߶�
void ObjectArx::SetViewportViewHeight(AcDbObjectId _ObjectId, double _Height)
{
	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		pViewport->setViewHeight(_Height);


		pViewport->close();

	} while (false);
}



//�����ӿ���ͼ���
void ObjectArx::SetViewportWidth(AcDbObjectId _ObjectId, double _Width)
{
	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		pViewport->setWidth(_Width);


		pViewport->close();

	} while (false);
}



//�����ӿ���ͼ�߶�
void ObjectArx::SetViewportHeight(AcDbObjectId _ObjectId, double _Height)
{
	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		pViewport->setHeight(_Height);


		pViewport->close();

	} while (false);
}



//�����ӿ���ͼ����
void ObjectArx::SetViewportStandardScale(AcDbObjectId _ObjectId, AcDbViewport::StandardScaleType _Scale)
{
	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		pViewport->setStandardScale(_Scale);


		pViewport->close();

	} while (false);
}


//�����ӿ���ͼ�Զ������
void ObjectArx::SetViewportCustomScale(AcDbObjectId _ObjectId, double _Scale)
{
	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		pViewport->setCustomScale(_Scale);


		pViewport->close();

	} while (false);
}


//���ö�����ʾ��������
int ObjectArx::SetObjectShow(AcDbObjectId ObjectId, bool BolShow)
{
	int Return = 0;


	do
	{
		//�򿪶���
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, ObjectId, AcDb::kForWrite) != 0)
		{
			Return = -1;
			break;
		}


		if (BolShow == 0)
		{
			pEnt->setVisibility(AcDb::kInvisible);
		}
		else
		{
			pEnt->setVisibility(AcDb::kVisible);
		}
		pEnt->close();

	} while (false);


	return Return;
}


//����ֱ�����
int ObjectArx::SetLineStartPoint(AcDbObjectId _LineId, AcGePoint3d _StartPoint)
{
	int Err = 0;


	do
	{
		//�򿪶���
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _LineId, AcDb::kForWrite) != 0)
		{
			Err = -1;
			break;
		}
		AcDbLine* pLine = AcDbLine::cast(pEnt);
		if (pLine == NULL)
		{
			pEnt->close();
			Err = -2;
			break;
		}


		pLine->setStartPoint(_StartPoint);


		pEnt->close();

	} while (false);


	return Err;
}


//����ֱ���յ�
int ObjectArx::SetLineEndPoint(AcDbObjectId _LineId, AcGePoint3d _EndPoint)
{
	int Err = 0;


	do
	{
		//�򿪶���
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _LineId, AcDb::kForWrite) != 0)
		{
			Err = -1;
			break;
		}
		AcDbLine* pLine = AcDbLine::cast(pEnt);
		if (pLine == NULL)
		{
			pEnt->close();
			Err = -2;
			break;
		}


		pLine->setEndPoint(_EndPoint);


		pEnt->close();

	} while (false);


	return Err;
}


//���ù����߻�׼��
bool ObjectArx::SetXlineBasePoint(AcDbObjectId _XlineId, AcGePoint3d _BasePoint)
{
	int IsReturn = true;


	do
	{
		AcDbXline* pXline = NULL;
		if (acdbOpenObject(pXline, _XlineId, AcDb::kForWrite) != 0)
		{
			IsReturn = false;
			break;
		}


		pXline->setBasePoint(_BasePoint);


		pXline->close();

	} while (false);


	return IsReturn;
}


//���ö���ͼ��
int ObjectArx::SetObjectLayer(AcDbObjectId _ObjectId, CString _Layer)
{
	int Err = 0;


	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		Err = pEnt->setLayer(_Layer);


		pEnt->close();

	} while (false);


	return Err;
}
int ObjectArx::SetObjectLayer(AcDbObjectIdArray _ListEntitys, CString _Layer)
{
	int Err = 0;


	for (int i = 0; i < _ListEntitys.length(); i++)
	{
		SetObjectLayer(_ListEntitys[i], _Layer);
	}


	return Err;
}



//��ö�������ͼ��
CString ObjectArx::GetObjectLayer(AcDbObjectId _ObjectId)
{
	CString StrLayer = _T("");


	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForRead) != Acad::eOk)
		{
			break;
		}


		StrLayer = pEnt->layer();


		pEnt->close();


	} while (false);


	return StrLayer;
}


//����ӿ�����
AcGePoint2d ObjectArx::GetViewportViewCenter(AcDbObjectId _ObjectId)
{
	AcGePoint2d Center(0, 0);


	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForRead) != 0)
		{
			break;
		}


		Center = pViewport->viewCenter();


		pViewport->close();

	} while (false);


	return Center;
}



//����ӿ���ͼ�߶�
double ObjectArx::GetViewportViewHeight(AcDbObjectId _ObjectId)
{
	double Height = 0;


	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForRead) != 0)
		{
			break;
		}


		Height = pViewport->viewHeight();


		pViewport->close();

	} while (false);


	return Height;
}


//����ӿڿ��
double ObjectArx::GetViewportWidth(AcDbObjectId _ObjectId)
{
	double Width = 0;


	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForRead) != 0)
		{
			break;
		}


		Width = pViewport->width();


		pViewport->close();

	} while (false);


	return Width;
}


//����ӿڸ߶�
double ObjectArx::GetViewportHeight(AcDbObjectId _ObjectId)
{
	double Width = 0;


	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForRead) != 0)
		{
			break;
		}


		Width = pViewport->height();


		pViewport->close();

	} while (false);


	return Width;
}


//����ӿ���ͼ����
AcDbViewport::StandardScaleType ObjectArx::SetViewportStandardScale(AcDbObjectId _ObjectId)
{
	AcDbViewport::StandardScaleType StandardScale;


	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForRead) != 0)
		{
			break;
		}


		StandardScale = pViewport->standardScale();


		pViewport->close();

	} while (false);


	return StandardScale;
}


//����ӿ���ͼ�Զ������
double ObjectArx::GetViewportCustomScale(AcDbObjectId _ObjectId)
{
	double Scale = 1;


	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForRead) != 0)
		{
			break;
		}


		Scale = pViewport->customScale();


		pViewport->close();

	} while (false);


	return Scale;
}


//��ö�����ʾ��������
bool ObjectArx::GetObjectShow(AcDbObjectId ObjectId)
{
	bool IsErr = false;


	do
	{
		//�򿪶���
		AcDbEntity* pEnt;
		if (acdbOpenAcDbEntity(pEnt, ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		AcDb::Visibility BolShow = pEnt->visibility();
		if (BolShow == AcDb::kVisible)
		{
			IsErr = true;
		}


		pEnt->close();


	} while (false);


	return IsErr;
}


//������ַ�Χ
AcGePoint2d ObjectArx::GetTextExtents(CString _String, CString _TextStyleName, double _Size)
{
	do
	{
		AcGiTextStyle AsdkStyle;
		AsdkStyle.setTextSize(_Size);
		AsdkStyle.setStyleName(_TextStyleName);
		AsdkStyle.extents(_String, Adesk::kFalse, _tcslen(_String), 1);
	} while (false);



	AcGiTextStyle AsdkStyle;
	AsdkStyle.setTextSize(_Size);
	AsdkStyle.setStyleName(_TextStyleName);
	return AsdkStyle.extents(_String, Adesk::kFalse, _tcslen(_String), 1);
}


//���Բ��Բ��
AcGePoint3d ObjectArx::GetArcCenter(AcDbObjectId _EntityId)
{
	AcGePoint3d Center(0, 0, 0);


	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _EntityId, AcDb::OpenMode::kForRead) != 0)
		{
			break;
		}


		AcDbCircle* pCircle = AcDbCircle::cast(pEnt);
		if (pCircle != NULL)
		{
			Center = pCircle->center();
		}
		else
		{
			AcDbArc* pArc = AcDbArc::cast(pEnt);
			if (pArc != NULL)
			{
				Center = pArc->center();
			}
		}


		pEnt->close();


	} while (false);


	return Center;
}


//���Բ������
AcGeVector3d ObjectArx::GetArcNormal(AcDbObjectId _EntityId)
{
	AcGeVector3d Normal(0, 0, 1);


	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _EntityId, AcDb::OpenMode::kForRead) != 0)
		{
			break;
		}


		AcDbCircle* pCircle = AcDbCircle::cast(pEnt);
		if (pCircle != NULL)
		{
			Normal = pCircle->normal();
		}
		else
		{
			AcDbArc* pArc = AcDbArc::cast(pEnt);
			if (pArc != NULL)
			{
				Normal = pArc->normal();
			}
		}


		pEnt->close();


	} while (false);


	return Normal;
}
bool ObjectArx::SetArcNormal(AcDbObjectId _EntityId, AcGeVector3d _Normal) {

	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _EntityId, AcDb::OpenMode::kForWrite) != 0)
		{
			break;
		}

		AcDbCircle* pCircle = AcDbCircle::cast(pEnt);
		if (pCircle != NULL)
		{
			pCircle->setNormal(_Normal);
		}
		else
		{
			AcDbArc* pArc = AcDbArc::cast(pEnt);
			if (pArc != NULL)
			{
				pArc->setNormal(_Normal);
			}
		}

		pEnt->close();

	} while (false);

	return true;
}


//���Բ���뾶
double ObjectArx::GetArcRadius(AcDbObjectId _EntityId)
{
	double Radius = 0;


	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _EntityId, AcDb::OpenMode::kForRead) != 0)
		{
			break;
		}


		AcDbCircle* pCircle = AcDbCircle::cast(pEnt);
		if (pCircle != NULL)
		{
			Radius = pCircle->radius();
		}
		else
		{
			AcDbArc* pArc = AcDbArc::cast(pEnt);
			if (pArc != NULL)
			{
				Radius = pArc->radius();
			}
		}


		pEnt->close();


	} while (false);


	return Radius;
}


//���Բ����ʼ�Ƕ�
double ObjectArx::GetArcStartAngle(AcDbObjectId _EntityId)
{
	double Angle = 0;


	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _EntityId, AcDb::OpenMode::kForRead) != 0)
		{
			break;
		}


		AcDbCircle* pCircle = AcDbCircle::cast(pEnt);
		if (pCircle != NULL)
		{
			Angle = 0;
		}
		else
		{
			AcDbArc* pArc = AcDbArc::cast(pEnt);
			if (pArc != NULL)
			{
				Angle = pArc->startAngle() * 180 / 3.1415926;
			}
		}


		pEnt->close();


	} while (false);


	return Angle;
}

//���Բ����ֹ�Ƕ�
double ObjectArx::GetArcEndAngle(AcDbObjectId _EntityId)
{
	double Angle = 0;


	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _EntityId, AcDb::OpenMode::kForRead) != 0)
		{
			break;
		}


		AcDbCircle* pCircle = AcDbCircle::cast(pEnt);
		if (pCircle != NULL)
		{
			Angle = 3.1415926 * 2 * 180 / 3.14159265358;
		}
		else
		{
			AcDbArc* pArc = AcDbArc::cast(pEnt);
			if (pArc != NULL)
			{
				Angle = pArc->endAngle() * 180 / 3.1415926;
			}
		}


		pEnt->close();


	} while (false);


	return Angle;
}



//�ֽ�һ������
AcDbVoidPtrArray ObjectArx::ExplodeObject(AcDbObjectId _ObjectId, bool BolDelete)
{
	//�򿪱��ֽ��ʵ��
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForWrite) != 0)
	{
		return -1;
	}


	//�ֽ����
	AcDbVoidPtrArray* pEnts = new AcDbVoidPtrArray();
	if (pEnt->explode(*pEnts) != 0)
	{
		pEnt->close();
		return -2;
	}

	if (BolDelete == 1)
	{
		pEnt->erase();
	}
	pEnt->close();
	return *pEnts;
}
AcDbObjectIdArray ObjectArx::ExplodeObjectToModel(AcDbObjectId _ObjectId, bool BolDelete, AcDbDatabase* _pDatabase)
{
	AcDbObjectIdArray ListObjects = NULL;


	do
	{
		//�򿪱��ֽ��ʵ��
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		//�ֽ����
		AcDbVoidPtrArray* pEnts = new AcDbVoidPtrArray();
		if (pEnt->explode(*pEnts) != 0)
		{
			pEnt->close();
			break;
		}


		//�Ƿ�ɾ��ԭ����
		if (BolDelete == 1)
		{
			pEnt->erase();
		}
		pEnt->close();


		//��ӵ�ģ�Ϳռ�
		for (int i = 0; i < pEnts->length(); i++)
		{
			ListObjects.append(AddObjectToModl((AcDbEntity*)pEnts->at(i), _pDatabase));
		}

	} while (false);


	return ListObjects;
}




AcDbObjectId ObjectArx::HandleToObjectId(CString _Handle, AcDbDatabase* _pDataBase)
{
	AcDbObjectId ObjectId = NULL;


	do
	{
		AcDbHandle Handle = _Handle.GetBuffer();

		if (_pDataBase == NULL)
		{
			_pDataBase = acdbHostApplicationServices()->workingDatabase();
		}
		if (_pDataBase->getAcDbObjectId(ObjectId, false, Handle) != 0)
		{
			break;
		}

	} while (false);


	return ObjectId;
}


//����ת���
CString ObjectArx::ObjectIdToHandle(AcDbObjectId _ObjectId)
{
	TCHAR pBuffer[256] = { 0 };
	_ObjectId.handle().getIntoAsciiBuffer(pBuffer);
	return pBuffer;
}



/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/













/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


//���ļ�
CString ObjectArx::OpenDialog(CString _StrPrompt, CString _StrSuffix, CString _StrDefaultPath)
{
	CString StrPath = _T("");
	struct resbuf* FileName;
	FileName = acutNewRb(RTSTR);
	if (acedGetFileD(_StrPrompt, _StrDefaultPath, _StrSuffix, 0, FileName) == RTNORM)
	{
		StrPath = FileName->resval.rstring;
	}
	acutRelRb(FileName);
	return StrPath;
}



//���Ϊ�Ի���
CString ObjectArx::SaveDialog(CString _StrPrompt, CString _StrSuffix, CString _StrDefaultPath)
{
	CString StrPath = _T("");
	struct resbuf* FileName;
	FileName = acutNewRb(RTSTR);
	if (acedGetFileD(_StrPrompt, _StrDefaultPath, _StrSuffix, 1, FileName) == RTNORM)
	{
		StrPath = FileName->resval.rstring;
	}
	acutRelRb(FileName);
	return StrPath;
}




/*
StrPrompt:ѡ�����ʱ����ʾ
PointCoor:���ص�ѡ�����ʱ����Ӧ�ĵ�

������ѡ��Ķ���
*/
//ѡ��һ�����󷵻�ʵ��ID
AcDbObjectId ObjectArx::SelectObject(CString StrPrompt)
{
	ads_name entName;
	ads_point pt;
	if (acedEntSel(StrPrompt, entName, pt) != RTNORM)
	{
		return NULL;
	}

	AcDbObjectId ObjectId = NULL;
	acdbGetObjectId(ObjectId, entName);
	return ObjectId;
}
AcDbObjectId ObjectArx::SelectObject(CString StrPrompt, AcGePoint3d* PointCoor)
{
	ads_name entName;
	ads_point pt;
	if (acedEntSel(StrPrompt, entName, pt) != RTNORM)
	{
		return NULL;
	}

	AcDbObjectId ObjectId = NULL;
	acdbGetObjectId(ObjectId, entName);
	PointCoor->x = pt[0];
	PointCoor->y = pt[1];
	PointCoor->z = pt[2];
	return ObjectId;
}



//��ѡʵ�巵��ʵ�弯
//AcDbObjectIdArray ObjectArx::SelectObjects(CString StrPrompt)
//{
//	AcDbObjectIdArray ListEntitys = NULL;
//
//
//	do
//	{
//		ads_name adsName, Ename;
//		if (acedSSGet(NULL, NULL, NULL, NULL, adsName) != RTNORM)
//		{
//			break;
//		}
//
//
//		AcDbObjectId ObjectId;
//#if OBJECTARX2010 || OBJECTARX2011 || OBJECTARX2012 || OBJECTARX2013 || OBJECTARX2014 || OBJECTARX2015 || OBJECTARX2016
//		long Len = 0;
//#elif OBJECTARX2017 || OBJECTARX2018 || OBJECTARX2019 || OBJECTARX2020
//		int Len = 0;
//#endif
//		if (acedSSLength(adsName, &Len) == RTNORM)
//		{
//			for (int i = 0; i < Len; i++)
//			{
//				acedSSName(adsName, i, Ename);
//				acdbGetObjectId(ObjectId, Ename);
//				ListEntitys.append(ObjectId);
//			}
//		}
//
//		acedSSFree(adsName);
//
//	} while (false);
//
//
//	return ListEntitys;
//}



//ѡ���
int ObjectArx::SelectPoint(CString _StrPrompt, AcGePoint3d* _PointPos)
{
	//����û�����Ŀ���յĲ����
	ads_point ptStart;
	if (acedGetPoint(NULL, _StrPrompt, ptStart) != RTNORM)
	{
		return -1;
	}
	_PointPos->x = ptStart[0];
	_PointPos->y = ptStart[1];
	_PointPos->z = ptStart[2];
	*_PointPos = UCS(*_PointPos);
	return 0;
}
int ObjectArx::SelectPoint(CString _StrPrompt, AcGePoint3d _PointDatum, AcGePoint3d* _PointPos)
{
	int Err = 0;


	do
	{
		_PointDatum = WCS(_PointDatum);


		ads_point ptStart, ptEnt;
		ptStart[0] = _PointDatum.x;
		ptStart[1] = _PointDatum.y;
		ptStart[2] = _PointDatum.z;
		if (acedGetPoint(ptStart, _T("��������һ�㣺\n"), ptEnt) != RTNORM)
		{
			Err = -1;
			break;
		}

		_PointPos->x = ptEnt[0];
		_PointPos->y = ptEnt[1];
		_PointPos->z = ptEnt[2];
		*_PointPos = UCS(*_PointPos);


	} while (false);


	return Err;
}



//����ѡ�ж���
int ObjectArx::SetSelectObject(AcDbObjectId _ObjectId)
{
	//����ѡ�ж���
	ads_name AdsSelects, AdsName;

	//����ѡ��
	acedSSAdd(NULL, NULL, AdsSelects);

	//���ʵ�嵽ѡ��
	acdbGetAdsName(AdsName, _ObjectId);
	acedSSAdd(AdsName, AdsSelects, AdsSelects);

	//��������
	ads_sssetfirst(AdsSelects, NULL);

	//�ͷ��ڴ�
	acedSSFree(AdsName);
	acedSSFree(AdsSelects);
	return 0;
}



//����ѡ�ж���
int ObjectArx::SetSelectObject(AcDbObjectIdArray _ListObjects)
{
	//����ѡ�ж���
	ads_name AdsSelects, AdsName;
	acedSSAdd(NULL, NULL, AdsSelects);
	for (int i = 0; i < _ListObjects.length(); i++)
	{
		acdbGetAdsName(AdsName, _ListObjects[i]);
		acedSSAdd(AdsName, AdsSelects, AdsSelects);
	}
	ads_sssetfirst(AdsSelects, NULL);
	acedSSFree(AdsName);
	acedSSFree(AdsSelects);
	return 0;
}


//ȡ��ѡ��
int ObjectArx::CancelSelect()
{
	ads_sssetfirst(NULL, NULL);
	return 0;
}


//���ģ�Ϳռ������һ��ʵ��
AcDbObjectId ObjectArx::GetEndEntity()
{
	AcDbObjectId EntityId = NULL;


	do
	{
		ads_name adsEnt;
		if (acdbEntLast(adsEnt) != RTNORM)
		{
			break;
		}


		acdbGetObjectId(EntityId, adsEnt);

	} while (false);


	return EntityId;
}

//��õ��µ�ʵ��
//AcDbObjectIdArray ObjectArx::GetPointInEntitys(AcGePoint3d _point)
//{
//	AcDbObjectIdArray entitys = NULL;
//
//	do
//	{
//		_point = WCS(_point);
//
//
//		ads_name adsName;
//		if (acedSSGet(NULL, asDblArray(_point), NULL, NULL, adsName) != RTNORM)  //���˫����ѡ��
//		{
//			acedSSFree(adsName);
//			break;
//		}
//
//
//
//#if OBJECTARX2010 || OBJECTARX2011 || OBJECTARX2012 || OBJECTARX2013 || OBJECTARX2014 || OBJECTARX2015 || OBJECTARX2016
//		long Len = 0;
//#elif OBJECTARX2017 || OBJECTARX2018 || OBJECTARX2019 || OBJECTARX2020
//		int Len = 0;
//#endif
//		AcDbObjectId ObjectId = NULL;
//		if (acedSSLength(adsName, &Len) == RTNORM)
//		{
//			for (int i = 0; i < Len; i++)
//			{
//				ads_name Ename;
//				acedSSName(adsName, i, Ename);
//				acdbGetObjectId(ObjectId, Ename);
//				entitys.append(ObjectId);
//			}
//		}
//		acedSSFree(adsName);
//
//	} while (false);
//
//	return entitys;
//}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

















/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/





//��������ĵ�תΪ��������ĵ�
AcGePoint3d ObjectArx::WCS(AcGePoint3d _PointPos)
{
	ads_point Pt, Result;
	struct resbuf Fromrb, Torb;
	Pt[0] = _PointPos.x;
	Pt[1] = _PointPos.y;
	Pt[2] = _PointPos.z;
	Fromrb.restype = RTSHORT;
	Fromrb.resval.rint = 0;


	//WCS
	Torb.restype = RTSHORT;
	Torb.resval.rint = 1;


	//UCS
	AcGePoint3d PointPos(0.0, 0.0, 0.0);
	acedTrans(Pt, &Fromrb, &Torb, FALSE, Result);
	PointPos.x = Result[0];
	PointPos.y = Result[1];
	PointPos.z = Result[2];
	return PointPos;

}


//��������ĵ�תΪ��������ĵ�
AcGePoint3d ObjectArx::UCS(AcGePoint3d _PointPos)
{
	ads_point Pt, Result;
	struct resbuf Fromrb, Torb;
	Pt[0] = _PointPos.x;
	Pt[1] = _PointPos.y;
	Pt[2] = _PointPos.z;
	Fromrb.restype = RTSHORT;
	Fromrb.resval.rint = 1;


	//WCS
	Torb.restype = RTSHORT;
	Torb.resval.rint = 0;


	//UCS
	AcGePoint3d PointPos(0.0, 0.0, 0.0);
	acedTrans(Pt, &Fromrb, &Torb, FALSE, Result);
	PointPos.x = Result[0];
	PointPos.y = Result[1];
	PointPos.z = Result[2];
	return PointPos;
}



//ָ�������ƶ�����
int ObjectArx::MoveObject(AcDbObjectId ObjectId, AcGeVector3d _Vector)
{
	AcGeMatrix3d Matrix;
	Matrix.setToTranslation(_Vector);


	AcDbEntity* pEnt = NULL;
	if (acdbOpenObject(pEnt, ObjectId, AcDb::kForWrite) != 0)
	{
		return -1;
	}


	pEnt->transformBy(Matrix);
	pEnt->close();
	return 0;
}
int ObjectArx::MoveObject(AcDbEntity* pEnt, AcGeVector3d _Vector)
{
	AcGeMatrix3d Matrix;
	Matrix.setToTranslation(_Vector);
	if (pEnt->transformBy(Matrix) != 0)
	{
		return -1;
	}
	return 0;
}
int ObjectArx::MoveObject(AcDbObjectIdArray _ListObjects, AcGeVector3d _Vector)
{
	AcGeMatrix3d Matrix;
	Matrix.setToTranslation(_Vector);


	AcDbEntity* pEnt = NULL;
	for (int i = 0; i < _ListObjects.length(); i++)
	{
		if (acdbOpenObject(pEnt, _ListObjects[i], AcDb::kForWrite) != 0)
		{
			continue;
		}


		pEnt->transformBy(Matrix);
		pEnt->close();
	}
	return 0;
}



//�㵽���ƶ�
int ObjectArx::MoveObject(AcDbObjectId _ObjectId, AcGePoint3d _PointA, AcGePoint3d _PointB)
{
	AcGeVector3d Vector(_PointB.x - _PointA.x, _PointB.y - _PointA.y, _PointB.z - _PointA.z);
	AcGeMatrix3d Matrix;
	Matrix.setToTranslation(Vector);


	AcDbEntity* pEnt = NULL;
	if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
	{
		return -1;
	}


	pEnt->transformBy(Matrix);
	pEnt->close();
	return 0;
}
int ObjectArx::MoveObject(AcDbEntity* _pEnt, AcGePoint3d _PointA, AcGePoint3d _PointB)
{
	AcGeMatrix3d Matrix;
	AcGeVector3d Vector(_PointB.x - _PointA.x, _PointB.y - _PointA.y, _PointB.z - _PointA.z);
	Matrix.setToTranslation(Vector);
	_pEnt->transformBy(Matrix);
	return 0;
}
int ObjectArx::MoveObject(AcDbObjectIdArray _ListObjects, AcGePoint3d _PointA, AcGePoint3d _PointB)
{
	AcGeMatrix3d Matrix;
	AcGeVector3d Vector(_PointB.x - _PointA.x, _PointB.y - _PointA.y, _PointB.z - _PointA.z);
	Matrix.setToTranslation(Vector);


	AcDbEntity* pEnt = NULL;
	for (int i = 0; i < _ListObjects.length(); i++)
	{
		if (acdbOpenObject(pEnt, _ListObjects[i], AcDb::kForWrite) != 0)
		{
			continue;
		}


		pEnt->transformBy(Matrix);
		pEnt->close();
	}
	return 0;
}



//��ת����
int ObjectArx::RotateObject(AcDbObjectId _ObjectId, AcGePoint3d _PointPos, double _Angle, AcGeVector3d _Vector)
{
	int Err = 0;


	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			Err = -1;
			break;
		}


		AcGeMatrix3d Matrix;
		Matrix.setToRotation(3.1415926 / 180.0 * _Angle, _Vector, _PointPos);
		Err = pEnt->transformBy(Matrix);


		pEnt->close();

	} while (false);


	return Err;
}


int ObjectArx::RotateObject(AcDbObjectIdArray _ListEntitys, AcGePoint3d _PointPos, double _Angle, AcGeVector3d _Vector)
{
	for (int i = 0; i < _ListEntitys.length(); i++)
	{
		ObjectArx::RotateObject(_ListEntitys[i], _PointPos, _Angle, _Vector);
	}
	return 0;
}


//���Ŷ���
int ObjectArx::ScaleEntity(AcDbObjectId _ObjectId, AcGePoint3d _PointPos, double _Var)
{
	int Err = 0;


	do
	{
		AcDbEntity* pEnt;
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			Err = -1;
			break;
		}
		AcGeMatrix3d Matrix3d;
		Matrix3d.setToScaling(_Var, _PointPos);
		pEnt->transformBy(Matrix3d);
		pEnt->close();

	} while (false);


	return Err;
}
int ObjectArx::ScaleEntity(AcDbObjectIdArray ListEntitys, AcGePoint3d _PointPos, double _Var)
{
	for (int i = 0; i < ListEntitys.length(); i++)
	{
		ScaleEntity(ListEntitys[i], _PointPos, _Var);
	}
	return 0;
}





//����
void ObjectArx::Mirroring(AcDbObjectId _EntityId, AcGePoint3d _StartPoint, AcGePoint3d _EndPoint)
{
	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _EntityId, AcDb::kForWrite) != 0)
		{
			break;
		}


		//���þ���
		AcGeMatrix3d Matrix;
		Matrix.setToMirroring(AcGeLine3d(_StartPoint, _EndPoint));


		pEnt->transformBy(Matrix);


		pEnt->close();

	} while (false);
}
void ObjectArx::Mirroring(AcDbObjectIdArray ListEntitys, AcGePoint3d _StartPoint, AcGePoint3d _EndPoint)
{
	for (int i = 0; i < ListEntitys.length(); i++)
	{
		Mirroring(ListEntitys[i], _StartPoint, _EndPoint);
	}
}


/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/






















/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


//��ö���ߵ�����
int ObjectArx::GetPolyVertexCount(AcDbObjectId _PolylineId)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _PolylineId, AcDb::kForWrite) != 0)
	{
		return 0;
	}
	AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
	if (pPolyline == NULL)
	{
		pEnt->close();
		return 0;
	}

	int Count = pPolyline->numVerts();
	pPolyline->close();


	return Count;
}


//���ö���ߵ�
int ObjectArx::SetPolyAtVertex(AcDbObjectId _PolylineId, int _nIndex, AcGePoint3d _PointPos)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _PolylineId, AcDb::kForWrite) != 0)
	{
		return 0;
	}
	AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
	if (pPolyline == NULL)
	{
		pEnt->close();
		return 0;
	}

	if (_nIndex < pPolyline->numVerts())
	{
		pPolyline->setPointAt(_nIndex, AcGePoint2d(_PointPos.x, _PointPos.y));
	}

	pPolyline->close();
	return 0;
}



//��Ӷ���ߵĵ�
int ObjectArx::AddPolyVertex(AcDbObjectId _PolylineId, AcGePoint3d _PointPos, int _nIndex)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _PolylineId, AcDb::kForWrite) != 0)
	{
		return 0;
	}
	AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
	if (pPolyline == NULL)
	{
		pEnt->close();
		return 0;
	}

	if (_nIndex == -1 && _nIndex > pPolyline->numVerts())
	{
		pPolyline->addVertexAt(pPolyline->numVerts(), AcGePoint2d(_PointPos.x, _PointPos.y));
	}
	else
	{
		pPolyline->addVertexAt(_nIndex, AcGePoint2d(_PointPos.x, _PointPos.y));
	}


	pPolyline->close();
	return 0;
}



//��ö���ߵ�
AcGePoint3d ObjectArx::GetPolyVertex(AcDbObjectId _PolylineId, int _nIndex)
{
	AcGePoint2d PointPos(0.0, 0.0);

	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _PolylineId, AcDb::kForRead) != 0)
	{
		return AcGePoint3d(PointPos.x, PointPos.y, 0.0);
	}
	AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
	if (pPolyline == NULL)
	{
		pEnt->close();
		return AcGePoint3d(PointPos.x, PointPos.y, 0.0);
	}

	if (_nIndex < pPolyline->numVerts())
	{
		pPolyline->getPointAt(_nIndex, PointPos);
	}

	pPolyline->close();
	return AcGePoint3d(PointPos.x, PointPos.y, 0.0);
}



//��ö���ߵ�
AcGePoint3dArray ObjectArx::GetPolyVertex(AcDbObjectId _PolylineId)
{
	//�򿪶���
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _PolylineId, AcDb::kForRead) != 0)
	{
		return NULL;
	}
	AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
	if (pPolyline == NULL)
	{
		pEnt->close();
		return NULL;
	}

	AcGePoint3d PointPos(0.0, 0.0, 0.0);
	AcGePoint3dArray ListPoints = NULL;
	for (int i = 0; i < pPolyline->numVerts(); i++)
	{
		pPolyline->getPointAt(i, PointPos);
		ListPoints.append(PointPos);
	}
	pPolyline->close();
	return ListPoints;
}



//�Ƴ�����ߵ�
int ObjectArx::RemovePolyAtVertex(AcDbObjectId _PolylineId, int _nIndex)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _PolylineId, AcDb::kForWrite) != 0)
	{
		return -1;
	}
	AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
	if (pPolyline == NULL)
	{
		pEnt->close();
		return -2;
	}


	pPolyline->removeVertexAt(_nIndex);


	pPolyline->close();
	return 0;
}



//�������л������ĳ���
double ObjectArx::GetCurvePointInLength(AcDbObjectId _CurveId, AcGePoint3d _PointA, AcGePoint3d _PointB)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _CurveId, AcDb::kForRead) != 0)
	{
		return -1;
	}
	AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
	if (pCurve == NULL)
	{
		pEnt->close();
		return -1;
	}


	double ParamA = 0.0, ParamB = 0.0;
	pCurve->getParamAtPoint(_PointA, ParamA);
	pCurve->getParamAtPoint(_PointB, ParamB);


	double LengthA = 0.0, LengthB = 0.0;
	pCurve->getDistAtParam(ParamA, LengthA);
	pCurve->getDistAtParam(ParamB, LengthB);


	pCurve->close();
	return abs(LengthB - LengthA);
}


double ObjectArx::GetCurvePointInParam(AcDbObjectId _CurveId, AcGePoint3d _Point)
{
	double Param = -1.0;

	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _CurveId, AcDb::kForRead) != 0)
		{
			break;
		}
		AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
		if (pCurve == NULL)
		{
			pEnt->close();
			break;
		}


		pCurve->getParamAtPoint(_Point, Param);

	} while (false);


	return Param;
}


//�ж϶����Ƿ�������
bool ObjectArx::IsObjectCurve(AcDbObjectId _ObjectId)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
	{
		return 0;
	}
	AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
	if (pCurve == NULL)
	{
		pEnt->close();
		return 0;
	}
	pEnt->close();
	return 1;
}



/*
_CurveId:����Id
_Param:�����ٷֱ�0����ʼ��,1�������
*/
//������ߵ�
AcGePoint3d ObjectArx::GetCurvePoint(AcDbObjectId _CurveId, double _Param)
{
	AcGePoint3d PointPos(0.0, 0.0, 0.0);


	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _CurveId, AcDb::kForRead) != 0)
	{
		return PointPos;
	}

	AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
	if (pCurve == NULL)
	{
		pEnt->close();
		return PointPos;
	}

	if (_Param == 0.0)
	{
		pCurve->getStartPoint(PointPos);
		pEnt->close();
	}
	else if (_Param == 1.0)
	{
		pCurve->getEndPoint(PointPos);
		pEnt->close();
	}
	else
	{
		double StartValue = 0.0, EndValue = 0.0;
		pCurve->getStartParam(StartValue);
		pCurve->getEndParam(EndValue);
		if (pCurve->getPointAtParam((EndValue - StartValue) * _Param, PointPos) != 0) {
			PointPos.set(0, 0, 0);
		}
		pEnt->close();
	}
	return PointPos;
}
bool ObjectArx::GetCurvePoint(AcDbObjectId _CurveId, double _Param, AcGePoint3d& _positoin) {

	bool isValue = false;

	do {

		AcDbCurve* pCurve = NULL;
		if (acdbOpenObject(pCurve, _CurveId, AcDb::kForRead) != 0)
		{
			break;
		}

		isValue = true;

		if (_Param == 0.0)
		{
			pCurve->getStartPoint(_positoin);
		}
		else if (_Param == 1.0)
		{
			pCurve->getEndPoint(_positoin);
		}
		else
		{
			double StartValue = 0.0, EndValue = 0.0;
			pCurve->getStartParam(StartValue);
			pCurve->getEndParam(EndValue);
			if (pCurve->getPointAtParam((EndValue - StartValue) * _Param, _positoin) != 0) {
				isValue = false;
			}
		}

		pCurve->close();

	} while (false);

	return isValue;
}



//��ó���
double ObjectArx::GetCurveLength(AcDbObjectId _CurveId, double _Param)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _CurveId, AcDb::kForRead) != 0)
	{
		return 0.0;
	}

	AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
	if (pCurve == NULL)
	{
		pEnt->close();
		return 0.0;
	}

	double StartValue = 0.0, EndValue = 0.0, Length = 0.0;
	pCurve->getStartParam(StartValue);
	pCurve->getEndParam(EndValue);
	pCurve->getDistAtParam((EndValue - StartValue) * _Param, Length);
	pEnt->close();
	return Length;
}



//��õ��������ϵ�λ��(��ֵ)
double ObjectArx::GetPointInCurve(AcDbObjectId _CurveId, AcGePoint3d _PointPos)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _CurveId, AcDb::kForRead) != 0)
	{
		return 0.0;
	}

	AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
	if (pCurve == NULL)
	{
		pEnt->close();
		return 0.0;
	}


	double StartValue = 0.0, EndValue = 0.0;
	pCurve->getStartParam(StartValue);
	pCurve->getEndParam(EndValue);


	//������߳���
	double LengthCurve = 0.0;
	pCurve->getDistAtParam(EndValue, LengthCurve);


	//�����Գ���
	double Length = 0.0;
	pCurve->getDistAtPoint(_PointPos, Length);

	pEnt->close();
	return Length / LengthCurve;
}



//�ж������Ƿ�պ�
bool ObjectArx::IsCurveClosed(AcDbObjectId _CurveId)
{
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _CurveId, AcDb::kForRead) != 0)
	{
		return 0;
	}

	AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
	if (pCurve == NULL)
	{
		pEnt->close();
		return 0;
	}


	bool Bol = pCurve->isClosed();
	pCurve->close();


	return Bol;
}



//���ö�����Ƿ�պ�
void ObjectArx::SetPolylineClosed(AcDbObjectId _PolylineId, bool _IsClose)
{
	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _PolylineId, AcDb::kForWrite) != 0)
		{
			break;
		}
		AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
		if (pPolyline == NULL)
		{
			pEnt->close();
			break;
		}


		pPolyline->setClosed(_IsClose);


		pPolyline->close();


	} while (false);
}



//�жϵ��Ƿ��ڶ������
bool ObjectArx::IsPointInCurve(AcDbObjectId _PolylineId, AcGePoint3d _PointPos, double _Tolerance)
{
	//��ʵ��
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _PolylineId, AcDb::kForRead) != 0)
	{
		return 0;
	}
	AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
	if (pPolyline == NULL)
	{
		pEnt->close();
		return 0;
	}


	AcDbRay* pRay = new AcDbRay();
	pRay->setBasePoint(_PointPos);
	pRay->setUnitDir(AcGeVector3d(1, 0, 0));


	//�ù��߶�����������ͼ����
	AcGePoint3dArray ListPoints = NULL;
	ListPoints.setLogicalLength(0);
	if (pRay->intersectWith(pPolyline, AcDb::kOnBothOperands, ListPoints) != 0)
	{
		delete pRay;
		pPolyline->close();
		return 0;
	}


	//���������Ϊ����,˵������ͼ���ڲ�
	if (ListPoints.length() % 2 != 0)
	{
		delete pRay;
		pPolyline->close();
		return 1;
	}

	delete pRay;
	pPolyline->close();
	return 0;
}



double BulgeFromArc(AcDbCurve* pCurve, bool Clockwise)
{
	double Bulge = 0.0;
	AcDbArc* pArc = AcDbArc::cast(pCurve);
	if (pArc == NULL)
	{
		return Bulge;
	}

	double NewStart = (pArc->startAngle() > pArc->endAngle()) ? (pArc->startAngle() - 8 * atan(1.0)) : (pArc->startAngle());

	Bulge = tan((pArc->endAngle() - NewStart) / 4.0);
	if (Clockwise)
	{
		Bulge = -Bulge;
	}
	return Bulge;
}



//���������
double ObjectArx::GetCurveArea(AcDbObjectId _CurveId)
{
	//�򿪶���ָ��
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _CurveId, AcDb::kForRead) != 0)
	{
		return 0.0;
	}
	AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
	if (pCurve == NULL)
	{
		pEnt->close();
		return 0.0;
	}


	double Area = 0.0;
	pCurve->getArea(Area);


	pCurve->close();
	return Area;
}



//���ö���߷���
int ObjectArx::SetPolylineNormal(AcDbObjectId _PolylineId, AcGeVector3d _Normal)
{
	int Err = 0;


	do
	{
		//�򿪶���ָ��
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _PolylineId, AcDb::kForRead) != 0)
		{
			Err = -1;
			break;
		}
		AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
		if (pPolyline == NULL)
		{
			pEnt->close();
			Err = -2;
			break;
		}


		//���÷���
		pPolyline->setNormal(_Normal);
		pPolyline->close();


	} while (false);


	return Err;
}


//�ж�Ŀ�������Ƿ��ڶ������
bool ObjectArx::IsInCurveInside(AcDbObjectId _targetId, AcDbObjectId _polylineId, double _Tolerance)
{
	bool isInside = false;

	do {

		//������߶����������
		AcGePoint3dArray curvePoints;
		for (int i = 0; i < 10; i++) {
			AcGePoint3d position;
			if (ObjectArx::GetCurvePoint(_targetId, 0.1 * i, position) == true) {
				curvePoints.append(position);
			}

			if (curvePoints.length() > 0) {
				isInside = true;
			}

			for (int i = 0; i < curvePoints.length(); i++) {

				//�������������Ҳ����
				if (ObjectArx::GetPointToCurveDist(curvePoints[i], _polylineId) < _Tolerance) {
					continue;
				}

				if (ObjectArx::IsPointInCurve(_polylineId, curvePoints[i]) == false) {
					isInside = false;
					break;
				}
			}

		} while (false);
	} while (false);

	return isInside;
}

//���ö����ȫ�ֿ��
int ObjectArx::SetPloylineConstantWidth(AcDbObjectId _ObjectId, double _Width)
{
	int Err = 0;


	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			Err = -1;
			break;
		}

		AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
		if (pPolyline == NULL)
		{
			pEnt->close();
			break;
		}

		pPolyline->setConstantWidth(_Width);

		pEnt->close();
	} while (false);


	return Err;
}

/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/














//�������
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


//��������
AcDbObjectIdArray ObjectArx::CreateObjectArray(AcDbObjectId _ObjectId, int _xCount, double _xDist, int _yCount, double _yDist)
{
	AcDbObjectIdArray ListObjects = NULL;
	ListObjects.append(_ObjectId);


	//��������ʵ���ƶ�ʵ��ľ���
	AcGeMatrix3d Matrix;
	AcGeVector3d Vector(0.0, 0.0, 0.0);
	Matrix.setToTranslation(Vector);


	//�򿪶���
	AcDbEntity* pEnt = NULL;
	if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
	{
		return NULL;
	}


	//�����ƶ�����
	AcDbEntity* pCopyEnt = NULL;
	for (int i = 0; i < _xCount; i++)
	{
		for (int u = 0; u < _yCount; u++)
		{
			if (i == 0 && u == 0)
			{
				continue;
			}

			//��������
			Vector.x = i * _xDist;
			Vector.y = u * _yDist;
			Vector.z = 0.0;
			Matrix.setToTranslation(Vector);

			//���ƶ���
			pCopyEnt = AcDbEntity::cast(pEnt->clone());

			//ʵ�־���任
			pCopyEnt->transformBy(Matrix);

			//��ӵ�ģ�Ϳռ�
			ListObjects.append(AddObjectToModl(pCopyEnt));
		}
	}
	pEnt->close();
	return ListObjects;
}


/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/











//��չ����
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



/*
_pEnt:����ָ��
_ObjectId:����Id
_StrName:��չ��������
*/
//�жϸö����Ƿ����ָ������չ��¼
bool ObjectArx::IsExtendData(AcDbEntity* _pEnt, CString _StrName)
{
	AcDbObjectId DictionaryId = _pEnt->extensionDictionary();


	//�鿴ʵ���Ƿ������չ�ֵ�
	if (DictionaryId == NULL)
	{
		return 0;
	}


	//����չ�ֵ�,���ָ�����Ƶ���չ��¼
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


	pDictionary->close();
	pXrec->close();
	return 1;
}
bool ObjectArx::IsExtendData(AcDbObjectId _ObjectId, CString _StrName)
{
	bool IsErr = false;


	do
	{
		//��ͼ�ζ���,���ʵ����չ�ֵ��ObjectId
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}
		AcDbObjectId DictionaryId = pEnt->extensionDictionary();
		pEnt->close();


		//�鿴ʵ���Ƿ������չ�ֵ�
		if (DictionaryId == NULL)
		{
			break;
		}


		//����չ�ֵ�,���ָ�����Ƶ���չ��¼
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



/*
_pEnt:����ָ��
_StrName:��չ������
_ObjectId:����Id
pRb:��չ����ֵ
_StrText:��չ�����ַ���
_DataVarArray:������
*/
//Ϊ��չ�ʵ��������չ����
int ObjectArx::AddExtendData(AcDbObjectId _ObjectId, CString _StrName, struct resbuf* (&pRb))
{
	int Err = -1;


	do
	{
		//��ͼ�ζ���,���ʵ����չ�ֵ��ObjectId
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			Err = -2;
			break;
		}
		AcDbObjectId DictionaryId = pEnt->extensionDictionary();
		if (DictionaryId == NULL)
		{
			//������չ�ʵ�
			pEnt->createExtensionDictionary();
			DictionaryId = pEnt->extensionDictionary();
		}
		pEnt->close();


		//�鿴ʵ���Ƿ������չ�ֵ�,������������½���չ�ʵ�
		if (DictionaryId == AcDbObjectId::kNull)
		{
			AcDbObject* pObject = NULL;
			if (acdbOpenObject(pObject, _ObjectId, AcDb::kForWrite) != 0)
			{
				Err = -3;
				break;
			}


			if (pObject->createExtensionDictionary() != 0)
			{
				pObject->close();
				Err = -4;
				break;
			}


			DictionaryId = pObject->extensionDictionary();
			if (DictionaryId == NULL)
			{
				pObject->close();
				Err = -5;
				break;
			}
			pObject->close();
		}


		//����չ�ֵ������һ����¼  
		AcDbObjectId RecObjectId = NULL;
		AcDbDictionary* pDict;
		AcDbXrecord* pXrec = new AcDbXrecord;
		if (acdbOpenObject(pDict, DictionaryId, AcDb::kForWrite) != 0)
		{
			pDict->close();
			Err = -6;
			break;
		}
		pDict->setAt(_StrName, pXrec, RecObjectId);
		pDict->close();


		//����չ��¼���������
		pXrec->setFromRbChain(*pRb);
		pXrec->close();

	} while (false);


	return Err;
}
int ObjectArx::AddExtendData(AcDbEntity* _pEnt, CString _StrName, struct resbuf* (&pRb))
{
	int Err = 0;


	do
	{
		AcDbObjectId DictionaryId = _pEnt->extensionDictionary();
		if (DictionaryId == NULL)
		{
			//������չ�ʵ�
			_pEnt->createExtensionDictionary();
			DictionaryId = _pEnt->extensionDictionary();
		}
		_pEnt->close();


		//�鿴ʵ���Ƿ������չ�ֵ�,������������½���չ�ʵ�
		if (DictionaryId == AcDbObjectId::kNull)
		{
			AcDbObject* pObject = AcDbObject::cast(_pEnt);
			//if (acdbOpenObject(pObject, _ObjectId, AcDb::kForWrite) != 0)
			//{
			//	Err = -3;
			//	break;
			//}


			if (pObject->createExtensionDictionary() != 0)
			{
				//pObject->close();
				Err = -4;
				break;
			}


			DictionaryId = pObject->extensionDictionary();
			if (DictionaryId == NULL)
			{
				//pObject->close();
				Err = -5;
				break;
			}
			//pObject->close();
		}


		//����չ�ֵ������һ����¼  
		AcDbObjectId RecObjectId = NULL;
		AcDbDictionary* pDict;
		AcDbXrecord* pXrec = new AcDbXrecord;
		if (acdbOpenObject(pDict, DictionaryId, AcDb::kForWrite) != 0)
		{
			pDict->close();
			Err = -6;
			break;
		}
		pDict->setAt(_StrName, pXrec, RecObjectId);
		pDict->close();


		//����չ��¼���������
		pXrec->setFromRbChain(*pRb);
		pXrec->close();

	} while (false);


	return Err;
}
int ObjectArx::AddExtendData(AcDbObjectId _ObjectId, CString _StrName, CString _StrText)
{
	int Error = -1;


	do
	{
		if (_ObjectId == NULL)
		{
			break;
		}

		struct resbuf* pRb;
		pRb = acutBuildList(AcDb::kDxfRegAppName, _StrName, AcDb::kDxfXdAsciiString, _StrText, RTNONE);
		Error = AddExtendData(_ObjectId, _StrName, pRb);
		acutRelRb(pRb);

	} while (false);


	return Error;
}
int ObjectArx::AddExtendData(AcDbEntity* _pEnt, CString _StrName, CString _StrText)
{
	struct resbuf* pRb;
	pRb = acutBuildList(AcDb::kDxfRegAppName, _StrName, AcDb::kDxfXdAsciiString, _StrText, RTNONE);
	int Err = AddExtendData(_pEnt, _StrName, pRb);
	acutRelRb(pRb);
	return Err;
}


/*
_pEnt:����ָ��
_StrName:��չ������
_ObjectId:����Id
pRb:��չ����ֵ
_StrText:��չ�����ַ���
_DataVarArray:������
*/
//�����չ�ʵ��е���չ����
int ObjectArx::GetExtendData(AcDbObjectId _ObjectId, CString _StrName, struct resbuf* (&pRb))
{
	int Err = -1;


	//��ͼ�ζ���,���ʵ����չ�ֵ��ObjectId
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
	{
		return -1;
	}
	AcDbObjectId DictionaryId = pEnt->extensionDictionary();
	pEnt->close();


	//�鿴ʵ���Ƿ������չ�ֵ�
	if (DictionaryId == AcDbObjectId::kNull)
	{
		return -2;
	}


	//����չ�ֵ�,���ָ�����Ƶ���չ��¼
	AcDbDictionary* pDictionary = NULL;
	if (acdbOpenObject(pDictionary, DictionaryId, AcDb::kForRead) != 0)
	{
		return -3;
	}
	AcDbXrecord* pXrec = NULL;
	Err = pDictionary->getAt(_StrName, (AcDbObject*&)pXrec, AcDb::kForRead);
	if (Err != 0)
	{
		pDictionary->close();
		return -4;
	}
	pDictionary->close();


	//�����չ��¼�����������ر���չ���ݶ���
	Err = pXrec->rbChain(&pRb);
	pXrec->close();
	return 0;
}
int ObjectArx::GetExtendData(AcDbEntity* _pEnt, CString _StrName, struct resbuf* (&pRb))
{
	int Err = -1;


	//��ͼ�ζ���,���ʵ����չ�ֵ��ObjectId
	AcDbObjectId DictionaryId = _pEnt->extensionDictionary();


	//�鿴ʵ���Ƿ������չ�ֵ�
	if (DictionaryId == AcDbObjectId::kNull)
	{
		return -2;
	}


	//����չ�ֵ�,���ָ�����Ƶ���չ��¼
	AcDbDictionary* pDictionary = NULL;
	if (acdbOpenObject(pDictionary, DictionaryId, AcDb::kForRead) != 0)
	{
		return -3;
	}
	AcDbXrecord* pXrec = NULL;
	Err = pDictionary->getAt(_StrName, (AcDbObject*&)pXrec, AcDb::kForRead);
	if (Err != 0)
	{
		pDictionary->close();
		return -4;
	}
	pDictionary->close();


	//�����չ��¼�����������ر���չ���ݶ���
	Err = pXrec->rbChain(&pRb);
	pXrec->close();
	return 0;
}
int ObjectArx::GetExtendData(AcDbObjectId _ObjectId, CString _StrName, CString* _StrText)
{
	int Err = -1;


	//��ѯ��չ����
	struct resbuf* pRb;
	Err = GetExtendData(_ObjectId, _StrName, pRb);
	if (Err != 0)
	{
		return -1;
	}


	//�����չ����
	CString StrText;
	while (1)
	{
		if (pRb != NULL)
		{
			if (pRb->restype == AcDb::kDxfXdAsciiString)
			{
				if (pRb->resval.rstring != _StrName)
				{
					StrText = pRb->resval.rstring;
				}
			}
		}
		else
		{
			break;
		}
		pRb = pRb->rbnext;
	}
	acutRelRb(pRb);


	*_StrText = StrText;
	return 0;
}
int ObjectArx::GetExtendData(AcDbEntity* _pEnt, CString _StrName, CString* _StrText)
{
	int Err = -1;


	//��ѯ��չ����
	struct resbuf* pRb;
	Err = GetExtendData(_pEnt, _StrName, pRb);
	if (Err != 0)
	{
		return -1;
	}


	//�����չ����
	CString StrText;
	while (1)
	{
		if (pRb != NULL)
		{
			if (pRb->restype == AcDb::kDxfXdAsciiString)
			{
				if (pRb->resval.rstring != _StrName)
				{
					StrText = pRb->resval.rstring;
				}
			}
		}
		else
		{
			break;
		}
		pRb = pRb->rbnext;
	}
	acutRelRb(pRb);


	*_StrText = StrText;
	return 0;
}



/*
_pEnt:����ָ��
_ObjectId:����Id
_StrName:��չ������
*/
//ɾ����չ����
int ObjectArx::DeleteExtendData(AcDbObjectId _ObjectId, CString _StrName)
{
	int Err = -1;


	//��ͼ�ζ���,���ʵ����չ�ֵ��ObjectId
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
	{
		return -1;
	}
	AcDbObjectId DictionaryId = pEnt->extensionDictionary();
	pEnt->close();


	//�鿴ʵ���Ƿ������չ�ֵ�
	if (DictionaryId == AcDbObjectId::kNull)
	{
		return -2;
	}


	//����չ�ֵ�,���ָ�����Ƶ���չ��¼
	AcDbDictionary* pDictionary = NULL;
	if (acdbOpenObject(pDictionary, DictionaryId, AcDb::kForWrite) != 0)
	{
		return -3;
	}


	if (pDictionary->remove(_StrName) != 0)
	{
		pDictionary->close();
		return -4;
	}


	pDictionary->close();
	return 0;
}
int ObjectArx::DeleteExtendData(AcDbEntity* _pEnt, CString _StrName)
{
	int Err = -1;


	//��ͼ�ζ���,���ʵ����չ�ֵ��ObjectId
	AcDbObjectId DictionaryId = _pEnt->extensionDictionary();


	//�鿴ʵ���Ƿ������չ�ֵ�
	if (DictionaryId == AcDbObjectId::kNull)
	{
		return -2;
	}


	//����չ�ֵ�,���ָ�����Ƶ���չ��¼
	AcDbDictionary* pDictionary = NULL;
	if (acdbOpenObject(pDictionary, DictionaryId, AcDb::kForWrite) != 0)
	{
		return -3;
	}


	if (pDictionary->remove(_StrName) != 0)
	{
		pDictionary->close();
		return -4;
	}


	pDictionary->close();
	return 0;
}



//ɾ��ͼֽ��չ�ʵ��е���չ����
int ObjectArx::DeleteDatabaseExtendData(CString _StrName, AcDbDatabase* _pDatabase)
{
	int Err = 0;


	do
	{
		//������ݿ���չ�ʵ�
		AcDbDictionary* pDictionary = NULL;
		if (_pDatabase->getNamedObjectsDictionary(pDictionary, AcDb::kForWrite) != 0)
		{
			Err = -1;
			break;
		}
		if (pDictionary->has(_StrName) == 0)
		{
			pDictionary->close();
			Err = -2;
			break;
		}


		//�Ƴ���չ����
		if (pDictionary->remove(_StrName) != 0)
		{
			pDictionary->close();
			Err = -3;
			break;
		}


		pDictionary->close();
	} while (0);


	return Err;
}



//�����ݿ��������չ�ʵ��������չ����
int ObjectArx::AddDatabaseExtendData(CString _StrName, struct resbuf* (&pRb), AcDbDatabase* _pDatabase)
{
	int Err = 0;


	do
	{
		//������ݿ���չ�ʵ�
		AcDbDictionary* pDictionary = NULL;
		if (_pDatabase->getNamedObjectsDictionary(pDictionary, AcDb::kForWrite) != 0)
		{
			Err = -1;
			break;
		}


		//�ж��Ƿ���ڸ�
		if (pDictionary->has(_StrName) == 1)
		{
			pDictionary->close();
			Err = -2;
			break;
		}


		//����չ�ֵ������һ����¼  
		AcDbObjectId RecObjId = NULL;
		AcDbXrecord* pXrec = new AcDbXrecord();
		pDictionary->setAt(_StrName, pXrec, RecObjId);
		pDictionary->close();


		//����չ��¼���������
		Err = pXrec->setFromRbChain(*pRb);
		pXrec->close();
	} while (0);


	return Err;
}
int ObjectArx::AddDatabaseExtendData(CString _StrName, CString _StrText, AcDbDatabase* _pDatabase)
{
	struct resbuf* pRb;
	pRb = acutBuildList(AcDb::kDxfRegAppName, _StrName, AcDb::kDxfXdAsciiString, _StrText, RTNONE);
	int Err = AddDatabaseExtendData(_StrName, pRb, _pDatabase);
	acutRelRb(pRb);
	return Err;
}


//������ݿ���ָ�����Ƶ���չ����
int ObjectArx::GetDatabaseExtendData(CString _StrName, struct resbuf* (&pRb), AcDbDatabase* _pDatabase)
{
	int Err = 0;


	do
	{
		//������ݿ���չ�ʵ�
		AcDbDictionary* pDictionary = NULL;
		if (_pDatabase->getNamedObjectsDictionary(pDictionary, AcDb::kForRead) != 0)
		{
			Err = -1;
			break;
		}
		if (pDictionary->has(_StrName) == 0)
		{
			pDictionary->close();
			Err = -2;
			break;
		}


		//����չ�ֵ������һ����¼  
		AcDbXrecord* pXrec = NULL;
		if (pDictionary->getAt(_StrName, (AcDbObject*&)pXrec, AcDb::kForRead) != 0)
		{
			pDictionary->close();
			Err = -3;
			break;
		}
		pDictionary->close();


		//�����չ��¼�����������ر���չ���ݶ���
		Err = pXrec->rbChain(&pRb);
		pXrec->close();


	} while (0);


	return Err;
}
int ObjectArx::GetDatabaseExtendData(CString _StrName, CString* _StrText, AcDbDatabase* _pDatabase)
{
	int Err = -1;
	CString StrText = _T("");


	do
	{
		//��ѯ��չ����
		struct resbuf* pRb;
		Err = GetDatabaseExtendData(_StrName, pRb, _pDatabase);
		if (Err != 0)
		{
			return -1;
		}


		//�����չ����
		while (1)
		{
			if (pRb != NULL)
			{
				if (pRb->restype == AcDb::kDxfXdAsciiString)
				{
					if (pRb->resval.rstring != _StrName)
					{
						StrText = pRb->resval.rstring;
					}
				}
			}
			else
			{
				break;
			}
			pRb = pRb->rbnext;
		}
		acutRelRb(pRb);


	} while (0);


	*_StrText = StrText;


	return Err;
}


/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/




















//��̬�����
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


//��ÿ�����
CString ObjectArx::GetBlockName(AcDbBlockReference* _pBlockReference)
{
	CString BlockName = _T("");


	do
	{
		AcDbDynBlockReference* pDynBlockReference = new AcDbDynBlockReference(_pBlockReference);
		if (pDynBlockReference == NULL)
		{
			//�򿪿���¼
			AcDbBlockTableRecord* pBlockTableRecord = new AcDbBlockTableRecord();
			if (acdbOpenObject(pBlockTableRecord, _pBlockReference->blockTableRecord(), AcDb::kForWrite) != 0)
			{
				_pBlockReference->close();
				break;
			}


			//��ÿ���
			wchar_t* pBlockName = NULL;
			pBlockTableRecord->getName(pBlockName);
			BlockName = pBlockName;


			pBlockTableRecord->close();
		}
		else
		{
			//��ÿ���¼
			AcDbBlockTableRecord* pBlockTableRecord = NULL;
			if (acdbOpenObject(pBlockTableRecord, pDynBlockReference->dynamicBlockTableRecord(), AcDb::kForRead) != 0)
			{
				delete[]pDynBlockReference;
				break;
			}


			//��ÿ�����
			wchar_t* pBlockName = NULL;
			pBlockTableRecord->getName(pBlockName);
			BlockName = pBlockName;


			pBlockTableRecord->close();
			delete[]pDynBlockReference;
		}
	} while (false);


	return BlockName;
}


//��������¼
int nRandomBlockName = 0;
AcDbObjectId ObjectArx::CreateBlockTableRecord(CString _StrName)
{
	AcDbObjectId BlockTableRecordId = NULL;


	do
	{
		//��õ�ǰͼ�����ݿ�Ŀ��
		AcDbBlockTable* pBlockTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForWrite);


		//�����µĿ���¼
		AcDbBlockTableRecord* pBlockTableRecord = new AcDbBlockTableRecord();


		//�ж��Ƿ���ڸÿ�
		if (_StrName != _T(""))
		{
			if (pBlockTable->has(_StrName) == true)
			{
				pBlockTable->close();
				break;
			}
		}
		else
		{
			//����һ������
			CTime Time = CTime::GetCurrentTime();
			while (true)
			{
				_StrName.Format(_T("%d%d%d%d%d%d%d%d"), Time.GetYear(), Time.GetMonth(), Time.GetDay(), Time.GetHour(),
					Time.GetMinute(), Time.GetSecond(), rand() % 10 * Time.GetMinute(), nRandomBlockName++);


				//�ж��Ƿ���ڸÿ�
				if (pBlockTable->has(_StrName) == false)
				{
					break;
				}
			}
		}


		//�����û����������ÿ���¼������
		if (pBlockTableRecord->setName(_StrName) != 0)
		{
			pBlockTable->close();
			break;
		}


		//������¼��ӵ������
		if (pBlockTable->add(BlockTableRecordId, pBlockTableRecord) != 0)
		{
			pBlockTableRecord->close();
			pBlockTable->close();
			break;
		}


		//�ͷ��ڴ�
		pBlockTableRecord->close();
		pBlockTable->close();

	} while (false);

	return BlockTableRecordId;
}


AcGeScale3d ObjectArx::GetBlockScale(AcDbObjectId _ObjectId)
{
	AcGeScale3d Scale(0, 0, 0);

	do
	{
		//�򿪿���¼���ָ��
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _ObjectId, AcDb::kForRead) != 0)
		{
			break;
		}


		//���ÿ����
		Scale = pBlockReference->scaleFactors();;


		//�ͷ��ڴ�
		pBlockReference->close();

	} while (false);

	return Scale;
}



void ObjectArx::SetBlockScale(AcDbObjectId _ObjectId, AcGeScale3d _Scale)
{
	do
	{
		//�򿪿���¼���ָ��
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _ObjectId, AcDb::OpenMode::kForWrite) != 0)
		{
			break;
		}


		//���ÿ����
		pBlockReference->setScaleFactors(_Scale);;


		//�ͷ��ڴ�
		pBlockReference->close();

	} while (false);

}


//���ͼ�鷨��
AcGeVector3d ObjectArx::GetBlockNormal(AcDbObjectId _BlockId)
{
	AcGeVector3d normal = AcGeVector3d::kZAxis;

	do
	{
		//�򿪿���¼���ָ��
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _BlockId, AcDb::OpenMode::kForRead) != 0)
		{
			break;
		}


		//���ÿ����
		normal = pBlockReference->normal();


		//�ͷ��ڴ�
		pBlockReference->close();

	} while (false);

	return normal;
}


//��ÿ�����
CString ObjectArx::GetBlockName(AcDbObjectId _ObjectId)
{
	CString StrBlockName = _T("");


	do
	{
		AcDbDynBlockReference* pDynBlockTableReference = new AcDbDynBlockReference(_ObjectId);
		if (pDynBlockTableReference == NULL)
		{
			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, _ObjectId, AcDb::kForWrite) != 0)
			{
				break;
			}


			AcDbBlockTableRecord* pBlockTableRecord = new AcDbBlockTableRecord();
			if (acdbOpenObject(pBlockTableRecord, pBlockReference->blockTableRecord(), AcDb::kForWrite) != 0)
			{
				pBlockReference->close();
				break;
			}


			//��ÿ���
			wchar_t* Tchar = NULL;
			pBlockTableRecord->getName(Tchar);
			StrBlockName = Tchar;


			pBlockTableRecord->close();
			pBlockReference->close();
		}
		else
		{
			//��ÿ���¼
			AcDbBlockTableRecord* pBlockTableRecord = NULL;
			if (acdbOpenObject(pBlockTableRecord, pDynBlockTableReference->dynamicBlockTableRecord(), AcDb::kForRead) != 0)
			{
				delete[]pDynBlockTableReference;
				break;
			}


			//��ÿ�����
			wchar_t* Tchar = NULL;
			pBlockTableRecord->getName(Tchar);
			StrBlockName = Tchar;


			pBlockTableRecord->close();
			delete[]pDynBlockTableReference;
		}
	} while (false);


	return StrBlockName;
}


//��ÿ���¼����
CString ObjectArx::GetBlockTableRecordName(AcDbObjectId _ObjectId)
{
	CString Name;

	do
	{
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		if (acdbOpenObject(pBlockTableRecord, _ObjectId, AcDb::OpenMode::kForRead) == 0)
		{
			wchar_t* pBlockName = NULL;
			pBlockTableRecord->getName(pBlockName);
			Name = pBlockName;
			pBlockTableRecord->close();
		}


	} while (false);

	return Name;
}


//���ƶ�̬��
AcDbObjectId ObjectArx::CopyDynBlockReference(AcDbObjectId _EntityId) {
	AcDbObjectId EntityId = NULL;

	if (ObjectArx::IsDynBolck(_EntityId) == false) {
		EntityId = ObjectArx::CopyEntity(_EntityId);
	}
	else {
		//���ģ�Ϳռ����һ��ʵ��
		AcDbObjectId EndId = GetEndEntity();

		//��ʵ��ŵ�������
		AcDbObjectIdArray Entitys;
		Entitys.append(_EntityId);

		//д�뵽��ʱ���ݿ�
		AcDbDatabase* pTempDatabase = NULL;
		acdbHostApplicationServices()->workingDatabase()->wblock(pTempDatabase, Entitys, AcGePoint3d(0, 0, 0));

		//���뵽��ǰͼֽ
		acdbHostApplicationServices()->workingDatabase()->insert(AcGeMatrix3d::kIdentity, pTempDatabase);

		//ɾ����ʱ���ݿ�
		delete pTempDatabase;

		//���ʵ��Id
		AcDbObjectId TempEndId = GetEndEntity();
		if (EndId != TempEndId) {
			EntityId = TempEndId;
		}
		else {
			EntityId = NULL;
		}
	}

	return EntityId;
}
AcDbObjectIdArray ObjectArx::CopyDynBlockReference(AcDbObjectIdArray _Entitys) {

	AcDbObjectIdArray EntityIds;

	do
	{
		//��õ�ǰģ�Ϳռ������ʵ��
		AcDbObjectIdArray OldEntitys = ObjectArx::IterateBlockTableRecord();


		//����̬��ͷǶ�̬��ֿ���
		AcDbObjectIdArray CopyEntitys = NULL;
		for (int i = 0; i < _Entitys.length(); i++)
		{
			if (ObjectArx::IsDynBolck(_Entitys[i]) == false) {
				EntityIds.append(ObjectArx::CopyEntity(_Entitys[i]));
			}
			else {
				CopyEntitys.append(_Entitys[i]);
			}
		}


		//���ƶ�̬��
		if (CopyEntitys.length() > 0) {

			//д�뵽��ʱ���ݿ�
			AcDbDatabase* pTempDatabase = NULL;
			acdbHostApplicationServices()->workingDatabase()->wblock(pTempDatabase, CopyEntitys, AcGePoint3d(0, 0, 0));

			//���뵽��ǰͼֽ
			acdbHostApplicationServices()->workingDatabase()->insert(AcGeMatrix3d::kIdentity, pTempDatabase);

			//ɾ����ʱ���ݿ�
			delete pTempDatabase;

			//��õ�ǰģ�Ϳռ������ʵ��
			AcDbObjectIdArray NewEntitys = ObjectArx::IterateBlockTableRecord();

			//����µ�ʵ��
			for (int i = OldEntitys.length(); i < NewEntitys.length(); i++) {
				EntityIds.append(NewEntitys[i]);
			}
		}

	} while (false);

	return EntityIds;
}

//���ÿ����
void ObjectArx::ResetReference(AcDbObjectId _ObjectId)
{
	AcDbDynBlockReference* pBlockReference = new AcDbDynBlockReference(_ObjectId);
	if (pBlockReference != NULL)
	{
		pBlockReference->resetBlock();
	}
}


//��������¼
AcDbObjectIdArray ObjectArx::IterateBlockTableRecord(CString _BlockName, AcDbDatabase* _pDatabase)
{
	AcDbObjectIdArray ListEntitys = NULL;

	do
	{
		//�򿪿��
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::OpenMode::kForRead);


		//���ģ�Ϳռ�Ŀ���¼
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		if (pBlockTable->getAt(_BlockName, pBlockTableRecord, AcDb::OpenMode::kForRead) != 0)
		{
			pBlockTable->close();
			break;
		}
		pBlockTable->close();


		//��������¼
		ListEntitys = IterateBlockTableRecord(pBlockTableRecord);


		//�رտ���¼
		pBlockTableRecord->close();


	} while (false);

	return ListEntitys;
}
AcDbObjectIdArray ObjectArx::IterateBlockTableRecord(AcDbObjectId _BlockTableRecordId, AcDbDatabase* _pDatabase)
{
	AcDbObjectIdArray ListEntitys = NULL;


	do
	{
		if (_BlockTableRecordId == NULL)
		{
			//�򿪿��
			AcDbBlockTable* pBlockTable = NULL;
			_pDatabase->getBlockTable(pBlockTable, AcDb::OpenMode::kForRead);


			//���ģ�Ϳռ�Ŀ���¼
			pBlockTable->getAt(ACDB_MODEL_SPACE, _BlockTableRecordId);
			pBlockTable->close();
			if (_BlockTableRecordId == NULL)
			{
				break;
			}
		}


		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		if (acdbOpenObject(pBlockTableRecord, _BlockTableRecordId, AcDb::kForRead) != 0)
		{
			break;
		}


		ListEntitys = IterateBlockTableRecord(pBlockTableRecord);


		pBlockTableRecord->close();


	} while (false);


	return ListEntitys;
}
AcDbObjectIdArray ObjectArx::IterateBlockTableRecord(AcDbBlockTableRecord* _pBlockTableRecord)
{
	AcDbObjectIdArray ListEntitys = NULL;

	do
	{
		//����������
		AcDbBlockTableRecordIterator* pBlockTableRecordIterator = NULL;
		if (_pBlockTableRecord->newIterator(pBlockTableRecordIterator) != Acad::eOk)
		{
			break;
		}


		//������¼��ʵ��
		AcDbEntity* pEntity = NULL;
		for (pBlockTableRecordIterator->start(); !pBlockTableRecordIterator->done(); pBlockTableRecordIterator->step())
		{
			if (pBlockTableRecordIterator->getEntity(pEntity, AcDb::kForRead) != Acad::eOk)
			{
				continue;
			}


			ListEntitys.append(pEntity->objectId());
			pEntity->close();
		}


		//ɾ������������
		delete pBlockTableRecordIterator;

	} while (false);


	return ListEntitys;
}



//���ָ�����ƵĿ���¼
AcDbObjectId ObjectArx::GetBlockTableRecord(CString _BlockName, AcDbDatabase* _pDatabase)
{
	AcDbObjectId BlockTableRecordId = NULL;


	do
	{
		//�򿪿��
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::OpenMode::kForRead);


		//����û�ָ���Ŀ���¼
		if (pBlockTable->getAt(_BlockName, BlockTableRecordId) != 0)
		{
			pBlockTable->close();
			break;
		}


		//�رտ��
		pBlockTable->close();


	} while (false);


	return BlockTableRecordId;
}


//���ģ�Ϳռ�����п���
void ObjectArx::GetModelSpaceBlockNames(CString _Path, CStringArray& _ListBlockNames)
{
	do
	{
		//��ȡDWG�ļ�������ݿ�
		AcDbDatabase* pDatabase = new AcDbDatabase(false);
#if OBJECTARX2010 || OBJECTARX2011 || OBJECTARX2012 || OBJECTARX2013 || OBJECTARX2014 || OBJECTARX2015 || OBJECTARX2016
		if (pDatabase->readDwgFile(_Path, _SH_DENYNO) != 0)
#elif OBJECTARX2017 || OBJECTARX2018 || OBJECTARX2019 || OBJECTARX2020
		if (pDatabase->readDwgFile(_Path) != 0)
#endif
		{
			break;
		}


		//����ͼ��
		GetModelSpaceBlockNames(_ListBlockNames, pDatabase);


		//�ͷ��ڴ�
		delete pDatabase;

	} while (false);
}
void ObjectArx::GetModelSpaceBlockNames(CStringArray& _ListBlockNames, AcDbDatabase* _pDatabase)
{
	do
	{
		//�򿪿��
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::OpenMode::kForRead);


		//���ģ�Ϳռ�Ŀ���¼
		AcDbObjectId BlockTableRecordId = NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE, BlockTableRecordId);


		//�رտ��
		pBlockTable->close();


		//����ģ�Ϳռ�
		AcDbObjectIdArray ListEntitys = IterateBlockTableRecord(BlockTableRecordId, _pDatabase);


		//��ÿ�����
		bool IsAdd = false;
		CString StrBlockName = _T("");
		for (int i = 0; i < ListEntitys.length(); i++)
		{
			AcDbBlockReference* pBlockReference = NULL;
			if (acdbOpenObject(pBlockReference, ListEntitys[i], AcDb::kForRead) != 0)
			{
				continue;
			}
			pBlockReference->close();


			//��ÿ���
			StrBlockName = ObjectArx::GetBlockName(ListEntitys[i]);


			IsAdd = true;
			for (int u = 0; u < _ListBlockNames.GetCount(); u++)
			{
				if (StrBlockName == _ListBlockNames[u])
				{
					IsAdd = false;
					break;
				}
			}
			if (IsAdd == true)
			{
				_ListBlockNames.Add(StrBlockName);
			}
		}


	} while (false);
}



//���ͼֽ�е����п���
void ObjectArx::GetBlockNames(CString _Path, CStringArray& _ListBlockNames)
{
	do
	{
		//��ȡDWG�ļ�������ݿ�
		AcDbDatabase* pDatabase = new AcDbDatabase(false);
#if OBJECTARX2010 || OBJECTARX2011 || OBJECTARX2012 || OBJECTARX2013 || OBJECTARX2014 || OBJECTARX2015 || OBJECTARX2016
		if (pDatabase->readDwgFile(_Path, _SH_DENYNO) != 0)
#elif OBJECTARX2017 || OBJECTARX2018 || OBJECTARX2019 || OBJECTARX2020
		if (pDatabase->readDwgFile(_Path) != 0)
#endif
		{
			break;
		}


		//����ͼ��
		GetBlockNames(_ListBlockNames, pDatabase);


		//�ͷ��ڴ�
		delete pDatabase;

	} while (false);
}
void ObjectArx::GetBlockNames(CStringArray& _ListBlockNames, AcDbDatabase* _pDatabase)
{
	do
	{
		//��ÿ��ָ��
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead);


		//������������
		AcDbBlockTableIterator* pBlockTableIterator = NULL;
		pBlockTable->newIterator(pBlockTableIterator);


		//������п�����
		wchar_t* pBlockName = NULL;
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		for (pBlockTableIterator->start(); !pBlockTableIterator->done(); pBlockTableIterator->step())
		{
			if (pBlockTableIterator->getRecord(pBlockTableRecord, AcDb::kForRead) != 0)
			{
				continue;
			}

			if (pBlockTableRecord->getName(pBlockName) == 0)
			{
				_ListBlockNames.Add(pBlockName);
			}
			pBlockTableRecord->close();
		}


		//ɾ���������رտ��
		delete pBlockTableIterator;
		pBlockTable->close();

	} while (false);
}



//���ʵ�嵽����¼
AcDbObjectId ObjectArx::AddEntityToBlockTableRecord(AcDbEntity* _pEntiy, AcDbObjectId _BlockTableRecordId, AcDbDatabase* _pDatabase)
{
	AcDbObjectId ObjectId = NULL;


	do
	{
		if (_pDatabase == NULL)
		{
			_pDatabase = acdbHostApplicationServices()->workingDatabase();
		}


		//������ݿ��еĿ�����
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead);


		//���ģ�Ϳռ�Ŀ���¼
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		if (_BlockTableRecordId == NULL)
		{
			pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
		}
		else
		{
			if (acdbOpenObject(pBlockTableRecord, _BlockTableRecordId, AcDb::kForWrite) != 0)
			{
				pBlockTable->close();
				break;
			}
		}

		//��ʬ����ӵ�����¼��
		pBlockTableRecord->appendAcDbEntity(ObjectId, _pEntiy);


		//�رն���
		pBlockTable->close();
		pBlockTableRecord->close();
		_pEntiy->close();

	} while (false);


	return ObjectId;
}
AcDbObjectIdArray ObjectArx::AddEntityToBlockTableRecord(AcDbObjectIdArray _ListEntitys, AcDbObjectId _BlockTableRecordId, AcDbDatabase* _pDatabase)
{
	AcDbObjectIdArray ListEntitys = NULL;


	do
	{
		if (_pDatabase == NULL)
		{
			_pDatabase = acdbHostApplicationServices()->workingDatabase();
		}


		//������ݿ��еĿ�����
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead);


		//���ģ�Ϳռ�Ŀ���¼
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		if (_BlockTableRecordId == NULL)
		{
			pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
		}
		else
		{
			if (acdbOpenObject(pBlockTableRecord, _BlockTableRecordId, AcDb::kForWrite) != 0)
			{
				pBlockTable->close();
				break;
			}
		}

		//��ʬ����ӵ�����¼��
		AcDbObjectId ObjectId = NULL;
		AcDbEntity* pEntity = NULL, * pTempEntity = NULL;;
		for (int i = 0; i < _ListEntitys.length(); i++)
		{
			if (acdbOpenObject(pEntity, _ListEntitys[i], AcDb::OpenMode::kForWrite) == 0)
			{
				//���ƶ���
				pTempEntity = AcDbEntity::cast(pEntity->clone());

				pBlockTableRecord->appendAcDbEntity(ObjectId, pTempEntity);
				pTempEntity->close();
				ListEntitys.append(ObjectId);

				pEntity->close();
			}
		}


		//�رն���
		pBlockTable->close();
		pBlockTableRecord->close();

	} while (false);


	return ListEntitys;
}


//ɾ������¼
void ObjectArx::DeleteBlockTableRecord(CString _BlockName)
{
	do
	{
		//��õ�ǰ��ͼ���б�  
		AcDbBlockTable* pBlockTable;
		acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);


		//�ж��Ƿ����ָ�����ƵĿ���¼  
		if (pBlockTable->has(_BlockName) == 0)
		{
			pBlockTable->close();
		}


		//���ָ������¼��ָ��  
		AcDbBlockTableRecord* pBlockTableRcd = NULL;
		if (pBlockTable->getAt(_BlockName, pBlockTableRcd, AcDb::kForWrite) == 0)
		{
			pBlockTableRcd->erase();
			pBlockTableRcd->close();
			pBlockTable->close();
		}


		pBlockTableRcd->close();
		pBlockTable->close();

	} while (false);
}



//������ݿ�����ͼ��
int ObjectArx::GetDatabaseLayers(CStringArray* _StrLayerArray, AcDbDatabase* _pDatabase)
{
	//��ò��ָ��
	AcDbLayerTable* pLayerTable = NULL;
	AcDbLayerTableRecord* pLayerTableRcd = NULL;
	_pDatabase->getLayerTable(pLayerTable, AcDb::kForRead);


	//ʹ�ñ���������ÿһ������¼
	AcDbLayerTableIterator* pItr;
	pLayerTable->newIterator(pItr);
	ACHAR* Tchar;
	for (pItr->start(); !pItr->done(); pItr->step())
	{
		if (pItr->getRecord(pLayerTableRcd, AcDb::kForRead) != 0)
		{
			continue;
		}

		if (pLayerTableRcd->getName(Tchar) == 0)
		{
			_StrLayerArray->Add(Tchar);
		}
		pLayerTableRcd->close();
	}
	delete pItr;
	pLayerTable->close();
	return 0;
}



//����ͼ��
int ObjectArx::CreateLayer(CString _Layer, int _Color)
{
	do
	{
		//�ж��Ƿ��������Ϊ StrLayerName ��ͼ��
		AcDbLayerTable* pLayerTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
		if (pLayerTable->has(_Layer) == 1)
		{
			pLayerTable->close();
			break;
		}


		//�½�����¼
		AcDbLayerTableRecord* pLayerTableRecord = new AcDbLayerTableRecord();
		pLayerTableRecord->setName(_Layer);


		//����ͼ����ɫ
		AcCmColor Color;
		Color.setColorIndex(_Color);
		pLayerTableRecord->setColor(Color);


		AcDbObjectId LayerTableRecordId = NULL;
		pLayerTable->add(LayerTableRecordId, pLayerTableRecord);


		//���½��Ĳ���¼��ӵ������
		pLayerTableRecord->close();
		pLayerTable->close();

	} while (false);

	return 0;
}



//�ж�ͼ���Ƿ����
bool ObjectArx::IsLayer(CString _Layer, AcDbDatabase* _pDatabase)
{
	bool IsErr = false;


	do
	{
		AcDbLayerTable* pLayerTable = NULL;
		_pDatabase->getLayerTable(pLayerTable, AcDb::kForWrite);
		IsErr = pLayerTable->has(_Layer);
		pLayerTable->close();
	} while (false);


	return IsErr;
}


//��õ�ǰͼ��
CString ObjectArx::GetCurrentLayer(AcDbDatabase* _pDatabase)
{
	CString LayerName = _T("");


	do
	{
		AcDbObjectId LayerId = _pDatabase->clayer();


		AcDbLayerTableRecord* pLayerTableRecord = NULL;
		if (acdbOpenObject(pLayerTableRecord, LayerId, AcDb::OpenMode::kForRead) == 0)
		{
			wchar_t* pLayerName = NULL;
			pLayerTableRecord->getName(pLayerName);
			LayerName = pLayerName;

			pLayerTableRecord->close();
		}

	} while (false);


	return LayerName;
}

bool ObjectArx::IsLayerOff(CString _StrLayer)
{
	bool IsReturn = false;


	do
	{
		//�򿪲��,�ж��Ƿ��������Ϊ StrLayerName ��ͼ��
		AcDbLayerTable* pLayerTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
		if (pLayerTable->has(_StrLayer) == 0)
		{
			pLayerTable->close();
			break;
		}


		// ���ָ������¼��ָ��
		AcDbLayerTableRecord* pLayerTableRcd = NULL;
		if (pLayerTable->getAt(_StrLayer, pLayerTableRcd, AcDb::kForWrite) != 0)
		{
			pLayerTable->close();
			break;
		}


		//����ͼ���Ƿ񶳽�
		IsReturn = pLayerTableRcd->isOff();


		//�رն���
		pLayerTableRcd->close();
		pLayerTable->close();

	} while (false);


	return IsReturn;
}

int ObjectArx::SetLayerOff(CString _StrLayer, bool _IsOff) {

	int Return = -1;


	do
	{
		//�򿪲��,�ж��Ƿ��������Ϊ StrLayerName ��ͼ��
		AcDbLayerTable* pLayerTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
		if (pLayerTable->has(_StrLayer) == 0)
		{
			pLayerTable->close();
			break;
		}


		// ���ָ������¼��ָ��
		AcDbLayerTableRecord* pLayerTableRcd = NULL;
		if (pLayerTable->getAt(_StrLayer, pLayerTableRcd, AcDb::kForWrite) != 0)
		{
			pLayerTable->close();
			break;
		}


		//����ͼ���Ƿ񶳽�
		pLayerTableRcd->setIsOff(_IsOff);


		//�رն���
		pLayerTableRcd->close();
		pLayerTable->close();


		Return = 0;

	} while (false);


	return Return;
}


//�ж�ͼ���Ƿ�����
bool ObjectArx::IsLayerLock(CString _StrLayer)
{
	//�򿪲��,�ж��Ƿ��������Ϊ StrLayerName ��ͼ��
	AcDbLayerTable* pLayerTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
	if (pLayerTable->has(_StrLayer) == 0)
	{
		pLayerTable->close();
		return 0;
	}


	// ���ָ������¼��ָ��
	AcDbLayerTableRecord* pLayerTableRcd = NULL;
	if (pLayerTable->getAt(_StrLayer, pLayerTableRcd, AcDb::kForWrite) != 0)
	{
		pLayerTable->close();
		return 0;
	}


	//����ͼ���Ƿ񶳽�
	bool BolLock = pLayerTableRcd->isLocked();


	//�رն���
	pLayerTableRcd->close();
	pLayerTable->close();

	return BolLock;
}


//����ͼ���Ƿ�����
int ObjectArx::SetLayerLock(CString _StrLayer, bool _BolLock)
{
	//�򿪲��,�ж��Ƿ��������Ϊ StrLayerName ��ͼ��
	AcDbLayerTable* pLayerTbl;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (pLayerTbl->has(_StrLayer) == 0)
	{
		pLayerTbl->close();
		return 0;
	}


	//���ָ������¼��ָ��
	AcDbLayerTableRecord* pLayerTblRcd;
	if (pLayerTbl->getAt(_StrLayer, pLayerTblRcd, AcDb::kForWrite) != 0)
	{
		pLayerTbl->close();
		return 0;
	}


	//����ͼ���Ƿ񶳽�
	pLayerTblRcd->setIsLocked(_BolLock);


	//�رն���
	pLayerTblRcd->close();
	pLayerTbl->close();
	return 0;
}



//�ж�ͼ���Ƿ�����
bool ObjectArx::IsLayerFrozen(CString _StrLayer)
{
	//�򿪲��,�ж��Ƿ��������Ϊ StrLayerName ��ͼ��
	AcDbLayerTable* pLayerTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
	if (pLayerTable->has(_StrLayer) == 0)
	{
		pLayerTable->close();
		return 0;
	}


	// ���ָ������¼��ָ��
	AcDbLayerTableRecord* pLayerTableRcd = NULL;
	if (pLayerTable->getAt(_StrLayer, pLayerTableRcd, AcDb::kForWrite) != 0)
	{
		pLayerTable->close();
		return 0;
	}


	//����ͼ���Ƿ�����
	bool BolHide = pLayerTableRcd->isFrozen();


	//�رն���
	pLayerTableRcd->close();
	pLayerTable->close();
	return BolHide;
}


//����ͼ���Ƿ�����
int ObjectArx::SetLayerFrozen(CString _StrLayer, bool _BolHide)
{
	//�򿪲��,�ж��Ƿ��������Ϊ StrLayerName ��ͼ��
	AcDbLayerTable* pLayerTbl;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (pLayerTbl->has(_StrLayer) == 0)
	{
		pLayerTbl->close();
		return 0;
	}


	//���ָ������¼��ָ��
	AcDbLayerTableRecord* pLayerTblRcd;
	if (pLayerTbl->getAt(_StrLayer, pLayerTblRcd, AcDb::kForWrite) != 0)
	{
		pLayerTbl->close();
		return 0;
	}


	//����ͼ���Ƿ񶳽�
	pLayerTblRcd->setIsFrozen(_BolHide);


	//�رն���
	pLayerTblRcd->close();
	pLayerTbl->close();
	return 0;
}



//���ͼ����ɫ
int ObjectArx::GetLayerColor(CString _LayerName)
{
	int Color = 0;


	do
	{
		//��õ�ǰͼ�εĲ��
		AcDbLayerTable* pLayerTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForRead);


		//�ж��Ƿ����ָ�����ƵĲ���¼
		if (pLayerTable->has(_LayerName) == false)
		{
			pLayerTable->close();
			break;
		}


		// ���ָ������¼��ָ��
		AcDbLayerTableRecord* pLayerTableRecord = NULL;
		pLayerTable->getAt(_LayerName, pLayerTableRecord, AcDb::kForRead);


		//���ͼ����ɫ 
		AcCmColor CmColor = pLayerTableRecord->color();
		Color = CmColor.colorIndex();


		//�رն���
		pLayerTableRecord->close();
		pLayerTable->close();


	} while (false);


	return Color;
}



//����ͼ����ɫ
int ObjectArx::SetLayerColor(CString _LayerName, int _Color)
{
	int Err = 0;


	do
	{
		//�򿪲��,�ж��Ƿ��������Ϊ StrLayerName ��ͼ��
		AcDbLayerTable* pLayerTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForRead);
		if (pLayerTable->has(_LayerName) == false)
		{
			pLayerTable->close();
			Err = -1;
			break;
		}


		//���ָ������¼��ָ��
		AcDbLayerTableRecord* pLayerTableRecord = NULL;
		pLayerTable->getAt(_LayerName, pLayerTableRecord, AcDb::kForWrite);


		//����ͼ����ɫ
		AcCmColor color;
		color.setColorIndex(_Color);
		pLayerTableRecord->setColor(color);


		//�رն���
		pLayerTableRecord->close();
		pLayerTable->close();

	} while (false);


	return Err;
}


//���õ�ǰͼ��
int ObjectArx::SetCurrentLayer(CString _LayerName)
{
	int Err = 0;


	do
	{
		//��ȡ��ǰͼ�β��
		AcDbLayerTable* pLayerTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
		if (pLayerTable->has(_LayerName) == false)
		{
			pLayerTable->close();
			Err = -1;
			break;
		}


		//����ͼ��
		AcDbObjectId LayerId = NULL;
		pLayerTable->getAt(_LayerName, LayerId);
		acdbHostApplicationServices()->workingDatabase()->setClayer(LayerId);
		pLayerTable->close();

	} while (false);


	return Err;
}



//�޸Ŀ�����
int ObjectArx::SetBlockName(CString _StrOldName, CString _StrNewName, AcDbDatabase* _pDatabase)
{
	do
	{
		//��õ�ǰͼ�εĿ��
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead);


		//�ж��Ƿ����ָ�����ƵĿ���¼
		if (!pBlockTable->has(_StrOldName))
		{
			pBlockTable->close();
			break;
		}


		//���ָ������¼��ָ��
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		pBlockTable->getAt(_StrOldName, pBlockTableRecord, AcDb::kForWrite);


		//�޸Ŀ���
		pBlockTableRecord->setName(_StrNewName);


		//�رն���
		pBlockTableRecord->close();
		pBlockTable->close();


	} while (false);

	return 0;
}


//���ͼ��Ļ���
AcGePoint3d ObjectArx::GetBlockPoint(AcDbObjectId _BlockId)
{
	AcGePoint3d PointPos(0.0, 0.0, 0.0);


	do
	{
		//�򿪿���¼���ָ��
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _BlockId, AcDb::kForRead) != 0)
		{
			break;
		}


		//��õ�
		PointPos = pBlockReference->position();


		//�ͷ��ڴ�
		pBlockReference->close();

	} while (false);


	return PointPos;
}



//����ͼ��Ļ���
void ObjectArx::SetBlockPoint(AcDbObjectId _BlockId, AcGePoint3d _Point)
{
	do
	{
		//�򿪿���¼���ָ��
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _BlockId, AcDb::kForWrite) != 0)
		{
			break;
		}


		//��õ�
		pBlockReference->setPosition(_Point);


		//�ͷ��ڴ�
		pBlockReference->close();

	} while (false);

}



//���ͼ��ĽǶ�
double ObjectArx::GetBlockAngle(AcDbObjectId _BlockId)
{
	double Angle = 0.0;


	do
	{
		//�򿪿���¼���ָ��
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _BlockId, AcDb::kForRead) != 0)
		{
			break;
		}


		//��õ�
		Angle = pBlockReference->rotation();
		Angle = 180.0 / 3.14159265358 * Angle;


		//�ͷ��ڴ�
		pBlockReference->close();

	} while (false);


	return Angle;
}



//����ͼ��ĽǶ�
void ObjectArx::SetBlockAngle(AcDbObjectId _BlockId, double _Angle)
{
	do
	{
		//�򿪿���¼���ָ��
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _BlockId, AcDb::kForWrite) != 0)
		{
			break;
		}


		//��õ�
		pBlockReference->setRotation(_Angle * 3.14159265358 / 180.0);


		//�ͷ��ڴ�
		pBlockReference->close();

	} while (false);
}



/*
_ObjectId:����
*/
//�ж�һ�������Ƿ��Ƕ�̬��
bool ObjectArx::IsDynBolck(AcDbObjectId _ObjectId)
{
	bool IsRuturn = false;

	do
	{
		AcDbDynBlockReference* pDynBlock = new AcDbDynBlockReference(_ObjectId);
		if (pDynBlock != NULL)
		{
			IsRuturn = pDynBlock->isDynamicBlock();
		}
		delete pDynBlock;
	} while (false);

	return IsRuturn;
}




/*
_ObjectId:�����
*/
//�ж�һ�������Ƿ������Կ�
bool ObjectArx::IsAttrBolck(AcDbObjectId _ObjectId)
{
	//�򿪿���¼���ָ��
	AcDbBlockReference* pBlockRef = NULL;
	if (acdbOpenObject(pBlockRef, _ObjectId, AcDb::kForRead) != 0)
	{
		return 0;
	}


	//��ÿ���¼
	AcDbBlockTableRecord* pBlockDefRcd = NULL;
	if (acdbOpenObject(pBlockDefRcd, pBlockRef->blockTableRecord(), AcDb::kForRead) != 0)
	{
		pBlockRef->close();
		return 0;
	}


	//�ж��Ƿ������Կ�
	if (pBlockDefRcd->hasAttributeDefinitions() == 0)
	{
		pBlockDefRcd->close();
		pBlockRef->close();
		return 0;
	}
	pBlockDefRcd->close();
	return 1;
}




/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/







/*-------------------------------------------------------------------------------------------------------------------------------------------*/


//���õ�ǰ����
int ObjectArx::SetCurrentLayout(CString _StrName)
{
	int Err = 0;


	do
	{
		AcApLayoutManager* pLayMan = (AcApLayoutManager*)acdbHostApplicationServices()->layoutManager();
		if (pLayMan == NULL)
		{
			Err = -1;
			break;
		}

		pLayMan->updateLayoutTabs();
		Err = pLayMan->setCurrentLayout(_StrName);
		pLayMan->updateCurrentPaper(Adesk::kTrue);

	} while (false);


	return Err;
}


//��õ�ǰ����
CString ObjectArx::GetCurrentLayout()
{
	return acdbHostApplicationServices()->layoutManager()->findActiveLayout(1);
}


//��������
void ObjectArx::CreateLayout(CString _StrLayoutName, AcDbDatabase* _pDatabase)
{
	do
	{
		//��ò��ֱ�
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//�ж����ݿ����Ƿ���ڸò���
		bool IsExist = false;
		AcDbDictionaryIterator* pDictionaryIterator = pDictionary->newIterator();
		for (; !pDictionaryIterator->done(); pDictionaryIterator->next())
		{
			if (_StrLayoutName == pDictionaryIterator->name())
			{
				IsExist = true;
				break;
			}
		}
		delete pDictionaryIterator;
		pDictionary->close();
		if (IsExist == true)
		{
			break;
		}


		//������ݿ�Ŀ��
		AcDbBlockTable* pBlockTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForWrite);


		//�½�����¼
		AcDbObjectId BlockTableRecordId = NULL;
		AcDbBlockTableRecord* pBlockTableRecord = new AcDbBlockTableRecord();
		pBlockTableRecord->setName(_T("*Paper_Space"));
		pBlockTable->add(BlockTableRecordId, pBlockTableRecord);
		pBlockTableRecord->close();


		//�رտ��
		pBlockTable->close();


		//�½����ָ��
		AcDbLayout* pLayout = new AcDbLayout();
		if (pLayout == NULL)
		{
			break;
		}


		//��ʾ��ӡ��ʽ
		pLayout->setShowPlotStyles(true);
		pLayout->setPlotPlotStyles(true);

		//pLayout->setPlotSettingsName(_T("monochrome.ctb"));



		//���ò�������
		if (pLayout->setLayoutName(_StrLayoutName) != 0)
		{
			pLayout->close();
			break;
		}


		//��ӿ���¼����ǰ���ݿ�
		pLayout->addToLayoutDict(acdbHostApplicationServices()->workingDatabase(), BlockTableRecordId);


		//�رղ���
		pLayout->close();

	} while (false);
}



//������в�������
void ObjectArx::GetDatabaseLayouts(CStringArray* pListLayoutNames, AcDbDatabase* _pDatabase)
{
	do
	{
		//��ò��ֱ�
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//������в�������
		AcDbDictionaryIterator* pDictionaryIterator = pDictionary->newIterator();
		for (; !pDictionaryIterator->done(); pDictionaryIterator->next())
		{
			pListLayoutNames->Add(pDictionaryIterator->name());
		}
		delete pDictionaryIterator;
		pDictionary->close();

	} while (false);
}



//��ò��ֵĿ���
CString ObjectArx::GetLayoutBlockName(CString _StrLayoutName, AcDbDatabase* _pDatabase)
{
	CString StrBlockName = _T("");


	do
	{
		//��ò��ֱ�
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//��ò���Id
		AcDbObjectId LayoutId = NULL;
		pDictionary->getAt(_StrLayoutName, LayoutId);
		pDictionary->close();
		if (LayoutId == NULL)
		{
			break;
		}


		//��ÿ���¼Id
		AcDbObjectId BlockTableRecordId = NULL;
		AcDbLayout* pLayout = NULL;
		if (acdbOpenObject(pLayout, LayoutId, AcDb::kForWrite) == 0)
		{
			BlockTableRecordId = pLayout->getBlockTableRecordId();
			pLayout->close();
		}


		//��ÿ���
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		if (acdbOpenObject(pBlockTableRecord, BlockTableRecordId, AcDb::kForWrite) == 0)
		{
			wchar_t* pBlockName = NULL;
			pBlockTableRecord->getName(pBlockName);
			StrBlockName = pBlockName;
			pBlockTableRecord->close();
		}


	} while (false);


	return StrBlockName;
}



//��ò��ֵĿ���¼
AcDbObjectId ObjectArx::GetLayoutBlockTableRecord(CString _StrLayoutName, AcDbDatabase* _pDatabase)
{
	AcDbObjectId BlockTableRecordId = NULL;


	do
	{
		//��ò��ֱ�
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//��ò���Id
		AcDbObjectId LayoutId = NULL;
		pDictionary->getAt(_StrLayoutName, LayoutId);
		pDictionary->close();
		if (LayoutId == NULL)
		{
			break;
		}


		//��ÿ���¼Id
		AcDbLayout* pLayout = NULL;
		if (acdbOpenObject(pLayout, LayoutId, AcDb::kForWrite) == 0)
		{
			BlockTableRecordId = pLayout->getBlockTableRecordId();
			pLayout->close();
		}

	} while (false);


	return BlockTableRecordId;
}



//����ӿڶ���ͼ��
void ObjectArx::GetViewportFrozenLayer(AcDbObjectId _ViewPortId, CStringArray* pListLayoutNames)
{
	do
	{
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ViewPortId, AcDb::kForRead) != 0)
		{
			break;
		}


		AcDbObjectIdArray ListFrozenLayers = NULL;
		pViewport->getFrozenLayerList(ListFrozenLayers);


		pViewport->close();


		wchar_t* pLayerName = NULL;
		AcDbLayerTableRecord* pLayerTableRecord = NULL;
		for (int i = 0; i < ListFrozenLayers.length(); i++)
		{
			if (acdbOpenObject(pLayerTableRecord, ListFrozenLayers[i], AcDb::kForRead) != 0)
			{
				continue;
			}


			pLayerTableRecord->getName(pLayerName);
			pListLayoutNames->Add(pLayerName);


			pLayerTableRecord->close();
		}


	} while (false);
}


//�����ӿڶ���ͼ��
void ObjectArx::SetViewportFrozenLayer(AcDbViewport* _pViewport, CStringArray& ListLayoutNames, AcDbDatabase* _pDatabase)
{
	do
	{
		//��ò��
		AcDbLayerTable* pLayerTable = NULL;
		_pDatabase->getLayerTable(pLayerTable, AcDb::OpenMode::kForRead);


		//��ÿ���¼Id
		AcDbObjectIdArray ListFrozenLayers = NULL;
		AcDbLayerTableRecord* pLayerTableRecord = NULL;
		for (int i = 0; i < ListLayoutNames.GetCount(); i++)
		{
			pLayerTable->getAt(ListLayoutNames.GetAt(i), pLayerTableRecord, AcDb::OpenMode::kForRead);
			if (pLayerTableRecord == NULL)
			{
				continue;
			}


			ListFrozenLayers.append(pLayerTableRecord->objectId());
			pLayerTableRecord->close();
		}


		//�رտ���¼
		pLayerTable->close();


		//�����ӿڶ���ͼ��
		_pViewport->thawAllLayersInViewport();
		_pViewport->freezeLayersInViewport(ListFrozenLayers);
		_pViewport->setOn();

	} while (false);
}
void ObjectArx::SetViewportFrozenLayer(AcDbObjectId _ViewPortId, CStringArray& ListLayoutNames, AcDbDatabase* _pDatabase)
{
	do
	{
		//�����ӿڶ���ͼ��
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ViewPortId, AcDb::kForWrite) != 0)
		{
			break;
		}
		SetViewportFrozenLayer(pViewport, ListLayoutNames, _pDatabase);


		pViewport->close();

	} while (false);
}



//�����ӿ�����
void ObjectArx::SetViewportLocked(AcDbObjectId _ViewPortId, bool _IsLocked)
{
	AcDbViewport* pViewport = NULL;
	if (acdbOpenObject(pViewport, _ViewPortId, AcDb::kForWrite) == 0)
	{
		pViewport->setLocked(_IsLocked);
		pViewport->close();
	}
}


//ɾ������
int ObjectArx::DeleteLayout(CString _StrName)
{
	int Err = 0;


	do
	{
		Err = acdbHostApplicationServices()->layoutManager()->deleteLayout(_StrName);
	} while (false);


	return Err;
}


//��ò����д�ӡpc3�ļ�
CString ObjectArx::GetPlotCfgName(CString _LayoutName, AcDbDatabase* _pDatabase)
{
	CString PlotCfgName;


	do
	{
		//��ò��ֱ�
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//��ò���Id
		AcDbObjectId LayoutId = NULL;
		pDictionary->getAt(_LayoutName, LayoutId);
		pDictionary->close();
		if (LayoutId == NULL)
		{
			break;
		}


		AcDbLayout* pLayout = NULL;
		if (acdbOpenObject(pLayout, LayoutId, AcDb::kForRead) == 0)
		{
			const wchar_t* pPlotCfgName = NULL;
			pLayout->getPlotCfgName(pPlotCfgName);
			PlotCfgName = pPlotCfgName;
			pLayout->close();
		}
	} while (false);


	return PlotCfgName;
}


//��ò������Ƿ���ʾ��ӡ��ʽ
bool ObjectArx::GetShowPlotStyles(CString _LayoutName, AcDbDatabase* _pDatabase)
{
	bool IsShowPlotStyles = false;

	do
	{
		//��ò��ֱ�
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//��ò���Id
		AcDbObjectId LayoutId = NULL;
		pDictionary->getAt(_LayoutName, LayoutId);
		pDictionary->close();
		if (LayoutId == NULL)
		{
			break;
		}


		AcDbLayout* pLayout = NULL;
		if (acdbOpenObject(pLayout, LayoutId, AcDb::kForRead) == 0)
		{
			IsShowPlotStyles = pLayout->showPlotStyles();
			pLayout->close();
		}
	} while (false);

	return IsShowPlotStyles;
}

//���ò������Ƿ���ʾ��ӡ��ʽ
void ObjectArx::SetShowPlotStyles(CString _LayoutName, bool _IsShowPlotStyles, AcDbDatabase* _pDatabase)
{
	do
	{
		//��ò��ֱ�
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//��ò���Id
		AcDbObjectId LayoutId = NULL;
		pDictionary->getAt(_LayoutName, LayoutId);
		pDictionary->close();
		if (LayoutId == NULL)
		{
			break;
		}


		AcDbLayout* pLayout = NULL;
		if (acdbOpenObject(pLayout, LayoutId, AcDb::OpenMode::kForWrite) == 0)
		{
			pLayout->setShowPlotStyles(_IsShowPlotStyles);
			pLayout->close();
		}
	} while (false);
}

AcDbViewTableRecord* ObjectArx::GetCurrentView()
{
	AcDbViewTableRecord view;
	struct resbuf rb;
	struct resbuf wcs, ucs, dcs; // ת������ʱʹ�õ�����ϵͳ���
	wcs.restype = RTSHORT;
	wcs.resval.rint = 0;
	ucs.restype = RTSHORT;
	ucs.resval.rint = 1;
	dcs.restype = RTSHORT;
	dcs.resval.rint = 2;

	// ��õ�ǰ�ӿڵ�"�鿴"ģʽ
	acedGetVar(_T("VIEWMODE"), &rb);
	view.setPerspectiveEnabled(rb.resval.rint & 1);
	view.setFrontClipEnabled(rb.resval.rint & 1);
	view.setBackClipEnabled(1);
	view.setFrontClipAtEye(1);


	// ��ǰ�ӿ�����ͼ�����ĵ㣨UCS���꣩
	acedGetVar(_T("VIEWCTR"), &rb);
	acedTrans(rb.resval.rpoint, &ucs, &dcs, 0, rb.resval.rpoint);
	view.setCenterPoint(AcGePoint2d(rb.resval.rpoint[X],
		rb.resval.rpoint[Y]));


	// ��ǰ�ӿ�͸��ͼ�еľ�ͷ���೤�ȣ���λΪ���ף�
	acedGetVar(_T("LENSLENGTH"), &rb);
	view.setLensLength(rb.resval.rreal);


	// ��ǰ�ӿ���Ŀ����λ�ã���UCS �����ʾ��
	acedGetVar(_T("TARGET"), &rb);
	acedTrans(rb.resval.rpoint, &ucs, &wcs, 0, rb.resval.rpoint);
	view.setTarget(AcGePoint3d(rb.resval.rpoint[X],
		rb.resval.rpoint[Y], rb.resval.rpoint[Z]));


	// ��ǰ�ӿڵĹ۲췽��UCS��
	acedGetVar(_T("VIEWDIR"), &rb);
	acedTrans(rb.resval.rpoint, &ucs, &wcs, 1, rb.resval.rpoint);
	view.setViewDirection(AcGeVector3d(rb.resval.rpoint[X],
		rb.resval.rpoint[Y], rb.resval.rpoint[Z]));


	// ��ǰ�ӿڵ���ͼ�߶ȣ�ͼ�ε�λ��
	acedGetVar(_T("VIEWSIZE"), &rb);
	view.setHeight(rb.resval.rreal);
	double height = rb.resval.rreal;


	// ������Ϊ��λ�ĵ�ǰ�ӿڵĴ�С��X ��Y ֵ��
	acedGetVar(_T("SCREENSIZE"), &rb);
	view.setWidth(rb.resval.rpoint[X] / rb.resval.rpoint[Y] * height);


	// ��ǰ�ӿڵ���ͼŤת��
	acedGetVar(_T("VIEWTWIST"), &rb);
	view.setViewTwist(rb.resval.rreal);


	// ��ģ��ѡ������һ������ѡ���Ϊ��ǰ
	acedGetVar(_T("TILEMODE"), &rb);
	int tileMode = rb.resval.rint;


	// ���õ�ǰ�ӿڵı�ʶ��
	acedGetVar(_T("CVPORT"), &rb);
	int cvport = rb.resval.rint;


	// �Ƿ���ģ�Ϳռ����ͼ
	bool paperspace = ((tileMode == 0) && (cvport == 1)) ? true : false;
	view.setIsPaperspaceView(paperspace);
	if (!paperspace)
	{
		// ��ǰ�ӿ���ǰ�����ƽ�浽Ŀ��ƽ���ƫ����
		acedGetVar(_T("FRONTZ"), &rb);
		view.setFrontClipDistance(rb.resval.rreal);


		// ��õ�ǰ�ӿں������ƽ�浽Ŀ��ƽ���ƫ��ֵ
		acedGetVar(_T("BACKZ"), &rb);
		view.setBackClipDistance(rb.resval.rreal);
	}
	else
	{
		view.setFrontClipDistance(0.0);
		view.setBackClipDistance(0.0);
	}
	return &view;
}




/*-------------------------------------------------------------------------------------------------------------------------------------------*/












/*-------------------------------------------------------------------------------------------------------------------------------------------*/


//�ж�������ʽ�Ƿ����
bool ObjectArx::IsTextStyle(CString _StrStyle)
{
	bool Err = 1;


	//��ñ�ע��ʽId
	AcDbTextStyleTable* pTextStyleTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyleTable, AcDb::kForWrite);
	if (pTextStyleTable->has(_StrStyle) == 0)
	{
		Err = 0;
	}


	pTextStyleTable->close();
	return Err;
}


//�����������
CString ObjectArx::GetText(AcDbObjectId _ObjectId)
{
	AcDbEntity* pEnt = NULL;
	CString StrText = _T("");


	do
	{
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		AcDbText* pText = AcDbText::cast(pEnt);
		if (pText != NULL)
		{
			StrText = pText->textString();
			break;
		}


		AcDbMText* pMText = AcDbMText::cast(pEnt);
		if (pMText != NULL)
		{
			StrText = pMText->text();
			break;
		}

	} while (false);


	if (pEnt != NULL)
	{
		pEnt->close();
	}


	return StrText;
}

//�����������
CString ObjectArx::GetMTextContents(AcDbObjectId _ObjectId)
{
	CString StrText = _T("");


	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		AcDbText* pText = AcDbText::cast(pEnt);
		if (pText != NULL)
		{
			StrText = pText->textString();
		}


		AcDbMText* pMText = AcDbMText::cast(pEnt);
		if (pMText != NULL)
		{
			StrText = pMText->contents();
		}


		pEnt->close();

	} while (false);


	return StrText;
}

//�жϱ�ע��ʽ�Ƿ����
bool ObjectArx::IsDimensionStyle(CString _StrStyle)
{
	bool Err = 1;


	//��ñ�ע��ʽId
	AcDbTextStyleTable* pTextStyleTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyleTable, AcDb::kForWrite);
	if (pTextStyleTable->has(_StrStyle) == 0)
	{
		Err = 0;
	}


	pTextStyleTable->close();
	return Err;
}


//ͨ��������ʽ�����ʽId
AcDbObjectId ObjectArx::GetDimensionStyleId(CString _StrStyle)
{
	int Err = -1;


	//��ñ�ע��ʽId
	AcDbDimStyleTable* pDimensionStyle = NULL;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimensionStyle, AcDb::kForWrite);
	if (!pDimensionStyle->has(_StrStyle))
	{
		pDimensionStyle->close();
		return NULL;
	}


	AcDbObjectId StyleId = NULL;
	Err = pDimensionStyle->getAt(_StrStyle, StyleId);
	if (Err != 0)
	{
		pDimensionStyle->close();
		return NULL;
	}


	pDimensionStyle->close();
	return StyleId;
}



//ͨ��������ʽId�����ʽ��
CString ObjectArx::GetDimensionStyleName(AcDbObjectId _StyleId)
{
	//�򿪶���
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _StyleId, AcDb::kForWrite) != 0)
	{
		return _T("");
	}



	//�����ʽָ��
	AcDbDimStyleTableRecord* pDimStyleTableRecord = AcDbDimStyleTableRecord::cast(pEnt);
	if (pDimStyleTableRecord == NULL)
	{
		pEnt->close();
		return _T("");
	}


	//�����ʽ����
	ACHAR* pName = NULL;
	CString StrName = _T("");
	pDimStyleTableRecord->getName(pName);
	StrName = pName;


	pDimStyleTableRecord->close();
	return StrName;
}



//ͨ��������ʽ�����ʽId
AcDbObjectId ObjectArx::GetTextStyleId(CString _StrStyle)
{
	AcDbObjectId StyleId = NULL;


	do
	{
		//��ñ�ע��ʽId
		AcDbTextStyleTable* pTextStyleTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyleTable, AcDb::kForWrite);
		if (pTextStyleTable->has(_StrStyle) == 0)
		{
			pTextStyleTable->close();
			break;
		}


		if (pTextStyleTable->getAt(_StrStyle, StyleId) != 0)
		{
			pTextStyleTable->close();
			break;
		}


		pTextStyleTable->close();


	} while (false);


	return StyleId;
}



//���������ʽ
AcDbObjectId ObjectArx::GetTextStyle(AcDbObjectId _ObjectId)
{
	AcDbEntity* pEnt = NULL;
	AcDbObjectId StyleId = NULL;


	do
	{
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		AcDbText* pText = AcDbText::cast(pEnt);
		if (pText != NULL)
		{
			StyleId = pText->textStyle();
			break;
		}


		AcDbMText* pMText = AcDbMText::cast(pEnt);
		if (pMText != NULL)
		{
			StyleId = pMText->textStyle();
			break;
		}

	} while (false);


	if (pEnt != NULL)
	{
		pEnt->close();
	}


	return StyleId;
}



//ͨ��������ʽId�����ʽ��
CString ObjectArx::GetTextStyleName(AcDbObjectId _StyleId)
{
	CString StrName = _T("");


	do
	{
		//�򿪶���
		AcDbObject* pObject = NULL;
		if (acdbOpenObject(pObject, _StyleId, AcDb::OpenMode::kForRead) != 0)
		{
			break;
		}

		//���������ʽָ��
		AcDbTextStyleTableRecord* pTextStyleTableRecord = AcDbTextStyleTableRecord::cast(pObject);
		if (pTextStyleTableRecord == NULL)
		{
			pObject->close();
			break;
		}
		pObject->close();


		//�����ʽ����
		ACHAR* pName = NULL;
		pTextStyleTableRecord->getName(pName);
		StrName = pName;


		pTextStyleTableRecord->close();


	} while (false);

	return StrName;
}


//������ֳߴ�
AcGePoint3d ObjectArx::GetTextSize(CString _StrText, CString _StrFont, double _Size)
{
	AcGiTextStyle AsdkStyle;
	AsdkStyle.setTextSize(_Size);
	AsdkStyle.setStyleName(_StrFont);
	AcGePoint2d PointText = AsdkStyle.extents(_StrText, Adesk::kFalse, _tcslen(_StrText), 1);
	return AcGePoint3d(PointText.x, PointText.y, 0.0);
}


//������ֳߴ�
double ObjectArx::GetTextHeight(AcDbObjectId _ObjectId)
{
	double Height = 0.0;
	AcDbEntity* pEnt = NULL;


	do
	{
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		AcDbText* pText = AcDbText::cast(pEnt);
		if (pText != NULL)
		{
			Height = pText->height();
			break;
		}


		AcDbMText* pMText = AcDbMText::cast(pEnt);
		if (pMText != NULL)
		{
			Height = pMText->textHeight();
			break;
		}

	} while (false);


	if (pEnt != NULL)
	{
		pEnt->close();
	}


	return Height;
}

//������ֿ��
double ObjectArx::GetTextWidth(AcDbObjectId _ObjectId)
{
	double Width = 0.0;
	AcDbEntity* pEnt = NULL;

	do
	{
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}



		AcDbMText* pMText = AcDbMText::cast(pEnt);
		if (pMText != NULL)
		{
			Width = pMText->width();
			break;
		}

	} while (false);


	if (pEnt != NULL)
	{
		pEnt->close();
	}


	return Width;
}

//������ֻ���
AcGePoint3d ObjectArx::GetTextPoint(AcDbObjectId _ObjectId)
{
	AcGePoint3d PointPos(0.0, 0.0, 0.0);
	AcDbEntity* pEnt = NULL;


	do
	{
		if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		AcDbText* pText = AcDbText::cast(pEnt);
		if (pText != NULL)
		{
			PointPos = pText->position();
			break;
		}


		AcDbMText* pMText = AcDbMText::cast(pEnt);
		if (pMText != NULL)
		{
			PointPos = pMText->location();
			break;
		}

	} while (false);


	if (pEnt != NULL)
	{
		pEnt->close();
	}


	return PointPos;
}



//����������ʼ��
int ObjectArx::SetLeaderPoint(AcDbObjectId _ObjectId, AcGePoint3d _PointPos, int _nInde)
{
	AcDbLeader* pLeader = NULL;
	if (acdbOpenObject(pLeader, _ObjectId, AcDb::kForWrite) == 0)
	{
		pLeader->setVertexAt(_nInde, _PointPos);
		pLeader->close();
	}
	return 0;
}



//������ߵ�
AcGePoint3d ObjectArx::GetLeaderPoint(AcDbObjectId _ObjectId, int _nInde)
{
	AcGePoint3d PointPos = AcGePoint3d(0.0, 0.0, 0.0);
	AcDbLeader* pLeader = NULL;
	if (acdbOpenObject(pLeader, _ObjectId, AcDb::kForWrite) == 0)
	{
		PointPos = pLeader->vertexAt(_nInde);
		pLeader->close();
	}
	return PointPos;
}



//���ö��������Ƿ���������
int ObjectArx::SetMLeaderEnableDogleg(AcDbObjectId _MLeaderId, bool _BolDogleg)
{
	AcDbMLeader* pMLeader = NULL;
	if (acdbOpenObject(pMLeader, _MLeaderId, AcDb::kForWrite) != 0)
	{
		return -1;
	}


	pMLeader->setEnableDogleg(_BolDogleg);


	pMLeader->close();
	return 0;
}



//���ñ�ע��ͷ�ߴ�
int ObjectArx::SetDimensionArrowSize(AcDbObjectId _ObjectId, double _Size)
{
	AcDbLeader* pLeader = NULL;
	if (acdbOpenObject(pLeader, _ObjectId, AcDb::kForWrite) == 0)
	{
		pLeader->setDimasz(_Size);
		pLeader->close();
	}
	return 0;
}



//���óߴ����ָ߶�
int ObjectArx::SetDimensionTextHeight(AcDbObjectId _ObjectId, double _Height)
{
	AcDbLeader* pDimension = NULL;
	if (acdbOpenObject(pDimension, _ObjectId, AcDb::kForWrite) == 0)
	{
		pDimension->setDimtxt(_Height);
		pDimension->close();
	}
	return 0;
}



//ָ���ߴ���߳����ߴ��ߵľ���
int ObjectArx::SetDimensionExcDimline(AcDbObjectId _ObjectId, double _Var)
{
	AcDbDimension* pDimension = NULL;
	if (acdbOpenObject(pDimension, _ObjectId, AcDb::kForWrite) == 0)
	{
		pDimension->setDimexe(_Var);
		pDimension->close();
	}
	return 0;
}



//���ñ�ע��ʽ
int ObjectArx::SetDimensionStyle(AcDbObjectId _ObjectId, CString _StrStyle)
{
	AcDbLeader* pDimension = NULL;
	if (acdbOpenObject(pDimension, _ObjectId, AcDb::kForWrite) == 0)
	{
		pDimension->setDimensionStyle(GetDimensionStyleId(_StrStyle));
		pDimension->close();
	}
	return 0;
}



/*
_StrName:��ʽ����
_Color:��ɫ
_StrFont:����
_StrBigFont:������
_FontHeight:���ָ߶�
_XScale:��߱�
_ValueObliquingAngle:��б�Ƕ�
_BolVertical:�Ƿ�����
*/
//����������ʽ
int ObjectArx::CreateTextStyle(CString _StrName, int _Color, CString _StrFont, CString _StrBigFont, double _FontHeight, double _XScale,
	double _ValueObliquingAngle, bool _BolVertical)
{
	//���������ʽ��
	AcDbTextStyleTable* pTextStyleTbl;
	acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyleTbl, AcDb::kForWrite);



	//�ж��Ƿ���ڸ���ʽ
	if (pTextStyleTbl->has(_StrName) == 1)
	{
		pTextStyleTbl->close();
		return -1;
	}



	//�����µ�������ʽ���¼
	AcDbTextStyleTableRecord* pTextStyleTblRcd;
	pTextStyleTblRcd = new AcDbTextStyleTableRecord();


	//����������ʽ���¼������
	pTextStyleTblRcd->setName(_StrName);

	//���������ļ�����
	pTextStyleTblRcd->setFileName(_StrFont);

	//����������ʽ
	pTextStyleTblRcd->setBigFontFileName(_StrBigFont);

	//��������߶�
	pTextStyleTblRcd->setTextSize(_FontHeight);

	//���ø߿����
	pTextStyleTblRcd->setXScale(_XScale);

	//������б����
	double ValueArc = 3.14159265358 / 180 * _ValueObliquingAngle;
	pTextStyleTblRcd->setObliquingAngle(ValueArc);

	//��������
	pTextStyleTblRcd->setIsVertical(_BolVertical);

	//���µļ�¼��ӵ�������ʽ��
	pTextStyleTbl->add(pTextStyleTblRcd);
	pTextStyleTblRcd->close();
	pTextStyleTbl->close();
	return 0;

}



/*
_StrName:������ʽ��
_StrTextStyle:������ʽ
_HeightText:��ע���ָ߶�
_ColorDimText:��ע������ɫ
_VarDimMove:��ע���ֵ��ƶ�����
0:�ߴ��ߺͱ�ע����һ���ƶ�;
1:���ƶ���ע����ʱ���һ������;
2:�����ע���������ƶ��������������
__StrTypeArrow:��ͷ��״
"":ʵ�ıպ�
"_DOT":��
"_DOTSMALL":С��
"_DOTBLANK":���ĵ�
"_ORIGIN":ԭ����
"_ORIGIN2":ԭ���� 2
"_OPEN":��
"_OPEN90":ֱ��
"_OPEN30":30 �Ƚ�
"_CLOSED":�պ�
"_SMALL":����С��
"_NONE":��
"_OBLIQUE":��б
"_BOXFILLED":ʵ�Ŀ�
"_BOXBLANK":��
"_CLOSEDBLANK":���ıպ�
"_DATUMFILLED":ʵ�Ļ�׼������
"_DATUMBLANK":��׼������
"_INTEGRAL":�������
"_ARCHTICK":�������
_VarArrow:��ͷ�ߴ�(�Ƽ�Ϊ_HeightText / 2.0)
_BolTextInExtlineUp:���ñ�ע����ʼ�ջ����ڳߴ����֮��
_VarTextToDimline:�����ڱ�ע���Ϸ��ľ���(�Ƽ�Ϊ_HeightText / 10.0)
_VarExtlineOffOriginDist:�ߴ����ƫ��ԭ��ľ���
_VarDimJust:���Ʊ�ע���ֵ�ˮƽλ��
0:���������ڳߴ���֮��,���ڳߴ����֮�����ж���;
1:���ڵ�һ���ߴ���߷��ñ�ע����;
2:���ڵڶ����ߴ���߷��ñ�ע����;
3:����ע���ַ��ڵ�һ���ߴ��������,����֮����;
4:����ע���ַ��ڵڶ����ߴ��������,����֮����)
_VarExcDimline:ָ���ߴ���߳����ߴ��ߵľ���(�Ƽ�Ϊ_HeightText / 2.0)
_VarScale:��������
_ColorDimline:�ߴ�����ɫ(�м�ĳߴ���)
_ColorExtline:�ߴ������ɫ(���ߵĳߴ���)
_VarDimdec:��ע����λ��ʾ��С��λλ��,0Ϊ��ͼ��
_VarTextOffDimline:���ִӳߴ���ƫ��,���ߴ��߷ֳɶ���������֮����ñ�ע����ʱ,���ñ�ע������Χ�ľ���
*/
//����������ʽ
int ObjectArx::CreateDimensionStyle(CString _StrName, CString _StrTextStyle, double _HeightText, int _ColorDimText, int _VarDimMove,
	CString _StrTypeArrow, double _VarArrow, bool _BolTextInExtlineUp, double _VarTextToDimline, double _VarExtlineOffOriginDist,
	int _VarDimJust, double _VarExcDimline, double  _VarScale, int _ColorDimline, int _ColorExtline, int _VarDimdec,
	double _VarTextOffDimline)
{
	//��õ�ǰͼ�εı�ע��ʽ��  
	AcDbDimStyleTable* pDimStyleTable;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTable, AcDb::kForWrite);


	//�Ƿ��Ѱ���  
	if (pDimStyleTable->has(_StrName))
	{
		pDimStyleTable->close();
		return -1;
	}


	//�����µı�ע��ʽ��  
	AcDbDimStyleTableRecord* pDimStyleTableRcd;
	pDimStyleTableRcd = new AcDbDimStyleTableRecord();



	//���������ʽId
	AcDbTextStyleTable* pTextStyle = NULL;
	acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyle, AcDb::kForRead);
	AcDbObjectId TextStyleId = NULL;
	pTextStyle->getAt(_StrTextStyle, TextStyleId);
	pTextStyle->close();



	AcCmColor Color;



	//���ñ�ע��ʽ������  
	pDimStyleTableRcd->setName(_StrName);										//��ע��ʽ����

	pDimStyleTableRcd->setDimasz(_VarArrow);									//��ͷ����

	pDimStyleTableRcd->setDimblk(_StrTypeArrow);								//���ü�ͷ����״

	pDimStyleTableRcd->setDimexe(_VarExcDimline);								//ָ���ߴ���߳����ߴ��ߵľ���

	pDimStyleTableRcd->setDimlfac(_VarScale);									//��������

	Color.setColorIndex(_ColorDimline);
	pDimStyleTableRcd->setDimclrd(Color);										//Ϊ�ߴ���,��ͷ�ͱ�ע����ָ����ɫ

	Color.setColorIndex(_ColorExtline);
	pDimStyleTableRcd->setDimclre(Color);										//Ϊ�ߴ����ָ����ɫ

	Color.setColorIndex(_ColorDimText);
	pDimStyleTableRcd->setDimclrt(Color);										//Ϊ��ע����ָ����ɫ

	pDimStyleTableRcd->setDimdec(_VarDimdec);									//���ñ�ע����λ��ʾ��С��λλ��,0Ϊ��ͼ��

	pDimStyleTableRcd->setDimexo(_VarExtlineOffOriginDist);						//ָ���ߴ����ƫ��ԭ��ľ���

	pDimStyleTableRcd->setDimgap(_VarTextOffDimline);							//���ִӳߴ���ƫ��,���ߴ��߷ֳɶ���������֮����ñ�ע����ʱ,���ñ�ע������Χ�ľ���

	pDimStyleTableRcd->setDimjust(_VarDimJust);									//���Ʊ�ע���ֵ�ˮƽλ��

	pDimStyleTableRcd->setDimtix(_BolTextInExtlineUp);							//���ñ�ע����ʼ�ջ����ڳߴ����֮��

	pDimStyleTableRcd->setDimtmove(_VarDimMove);								//���ñ�ע���ֵ��ƶ�����

	pDimStyleTableRcd->setDimtxsty(TextStyleId);								//ָ����ע��������ʽ

	pDimStyleTableRcd->setDimtxt(_HeightText);									//ָ����ע���ֵĸ߶�,���ǵ�ǰ������ʽ���й̶��ĸ߶�

	pDimStyleTableRcd->setDimtad(_VarTextToDimline);							//����λ�ڱ�ע�ߵ��Ϸ�


	//����ע��ʽ���¼��ӵ���ע��ʽ����  
	pDimStyleTable->add(pDimStyleTableRcd);


	pDimStyleTableRcd->close();
	pDimStyleTable->close();
	return 0;
}



//���������ע
AcDbObjectId ObjectArx::CreateAligDimension(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint, AcGePoint3d _PointPos, CString _StrStyle, CString _StrText)
{
	AcDbAlignedDimension* pDimension = new AcDbAlignedDimension(_StartPoint, _EndPoint, _PointPos, _StrText, GetDimensionStyleId(_StrStyle));
	return AddObjectToModl(pDimension);
}



//�������Ա�ע
AcDbObjectId ObjectArx::CreateLinearDimension(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint, AcGePoint3d _PointPos, double _Angle, CString _StrStyle, CString _StrText)
{
	AcDbRotatedDimension* pDimension = new AcDbRotatedDimension(3.14159265358 / 180.0 * _Angle, _StartPoint, _EndPoint, _PointPos, _StrText, GetDimensionStyleId(_StrStyle));
	return AddObjectToModl(pDimension);
}


/*-------------------------------------------------------------------------------------------------------------------------------------------*/



















//����
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/




/*
_ObjectId:�󽻶���
_ListObjects:Ҫ�ԱȵĶ���
_Mode:��ģʽ
*/
//����ཻ����
AcDbObjectIdArray ObjectArx::GetInterObject(AcDbObjectId _ObjectId, AcDbObjectIdArray _ListObjects, AcDb::Intersect _Mode)
{
	AcDbEntity* pEnt = NULL, * pEntTemp = NULL;



	//�򿪵�һ������
	if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
	{
		return -1;
	}



	AcGePoint3dArray ListTempPoints = NULL;
	AcDbObjectIdArray ListInterObjects = NULL;
	for (int i = 0; i < _ListObjects.length(); i++)
	{
		//�ų���ͬ����
		if (_ListObjects[i] == _ObjectId)
		{
			continue;
		}


		//�򿪶���
		pEntTemp = NULL;
		if (acdbOpenAcDbEntity(pEntTemp, _ListObjects[i], AcDb::kForRead) != 0)
		{
			continue;
		}


		//�󽻵�
		ListTempPoints.removeAll();
		pEnt->intersectWith(pEntTemp, _Mode, ListTempPoints);
		if (ListTempPoints.length() > 0)
		{
			ListInterObjects.append(_ListObjects[i]);
		}


		//�رն���
		pEntTemp->close();
	}


	pEnt->close();
	return ListInterObjects;
}




/*
_ObjectId:�󽻶���
_ListObjects:Ҫ�󽻵Ķ���
_Mode:��ģʽ
*/
//����ཻ��
AcGePoint3dArray ObjectArx::GetInterPoint(AcDbObjectId _ObjectId, AcDbObjectIdArray _ListObjects, AcDb::Intersect _Mode)
{
	AcDbEntity* pEnt = NULL, * pEntTemp = NULL;



	//�򿪵�һ������
	if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
	{
		return -1;
	}



	AcGePoint3dArray ListTempPoints = NULL;
	AcGePoint3dArray ListInterPoints = NULL;
	for (int i = 0; i < _ListObjects.length(); i++)
	{
		//�ų���ͬ����
		if (_ListObjects[i] == _ObjectId)
		{
			continue;
		}


		//�򿪶���
		pEntTemp = NULL;
		if (acdbOpenAcDbEntity(pEntTemp, _ListObjects[i], AcDb::kForRead) != 0)
		{
			continue;
		}


		//�󽻵�
		ListTempPoints.removeAll();
		pEnt->intersectWith(pEntTemp, _Mode, ListTempPoints);
		ListInterPoints.append(ListTempPoints);


		//�رն���
		pEntTemp->close();
	}


	pEnt->close();
	return ListInterPoints;
}



//����ཻ��
AcGePoint3dArray ObjectArx::GetInterPoint(AcDbObjectId _ObjectAId, AcDbObjectId _ObjectBId, AcDb::Intersect _Mode)
{
	AcDbEntity* pEnt1, * pEnt2;
	if (acdbOpenAcDbEntity(pEnt1, _ObjectAId, AcDb::kForRead) != 0)
	{
		return NULL;
	}
	if (acdbOpenAcDbEntity(pEnt2, _ObjectBId, AcDb::kForRead) != 0)
	{
		pEnt1->close();
		return NULL;
	}


	AcGePoint3dArray* ListObjects = new AcGePoint3dArray();
	int Err = pEnt1->intersectWith(pEnt2, _Mode, *ListObjects);


	pEnt1->close();
	pEnt2->close();
	return *ListObjects;




	//AcDbEntity *pEnt1 = NULL, *pEnt2 = NULL;



	////�򿪵�һ������
	//if (acdbOpenAcDbEntity(pEnt1, _ObjectAId, AcDb::kForRead) != 0)
	//{
	//	return NULL;
	//}


	////�򿪵ڶ�������
	//if (acdbOpenAcDbEntity(pEnt2, _ObjectBId, AcDb::kForRead) != 0)
	//{
	//	pEnt1->close();
	//	return NULL;
	//}



	////��ý���
	//AcGePoint3dArray *pListInterPoints = new AcGePoint3dArray();
	//pEnt1->intersectWith(pEnt2, _Mode, *pListInterPoints);



	//pEnt1->close();
	//pEnt2->close();
	//return *pListInterPoints;
}



/*
_ObjectId:Ҫ��������Ķ���
_ListObjects::Ҫ��������Ķ���
_MinPoint:���ص���С��
_MaxPoint:���ص�����
*/
//��ö�������
int ObjectArx::GetEntityExtents(AcDbObjectId _ObjectId, AcGePoint3d* _MinPoint, AcGePoint3d* _MaxPoint)
{
	int Error = 0;


	do
	{
		//��ʵ��
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
		{
			Error = -2;
			break;
		}


		//��ȡ����
		AcDbExtents Extents;
		if (Acad::eInvalidExtents != pEnt->getGeomExtents(Extents))
		{
			*_MinPoint = Extents.minPoint();
			*_MaxPoint = Extents.maxPoint();
			Error = 0;
		}
		pEnt->close();

	} while (false);


	return Error;
}
int ObjectArx::GetEntityExtents(AcDbObjectIdArray _ListObjects, AcGePoint3d* _MinPoint, AcGePoint3d* _MaxPoint)
{
	AcGePoint3d* MinPoints = new AcGePoint3d[_ListObjects.length()];
	AcGePoint3d* MaxPoints = new AcGePoint3d[_ListObjects.length()];


	int NumPoint = 0;
	AcDbExtents Extents;
	AcDbObjectId ObjectId = NULL;
	AcDbEntity* pEnt = NULL;


	//ѭ����ȡ��������
	for (int i = 0; i < _ListObjects.length(); i++)
	{
		ObjectId = _ListObjects.at(i);

		//�򿨶���
		if (acdbOpenAcDbEntity(pEnt, ObjectId, AcDb::kForRead) != 0)
		{
			continue;
		}

		//��ȡ����
		if (Acad::eInvalidExtents != pEnt->getGeomExtents(Extents))
		{
			MinPoints[NumPoint] = Extents.minPoint();
			MaxPoints[NumPoint] = Extents.maxPoint();
			NumPoint++;
			pEnt->close();
		}
	}

	//�Ƚϻ�����е�������С����
	for (int i = 0; i < NumPoint; i++)
	{
		if (i == 0)
		{
			*_MinPoint = MinPoints[0];
			*_MaxPoint = MaxPoints[0];
		}
		else
		{
			if (MinPoints[i].x < _MinPoint->x)
			{
				_MinPoint->x = MinPoints[i].x;
			}
			if (MinPoints[i].y < _MinPoint->y)
			{
				_MinPoint->y = MinPoints[i].y;
			}
			if (MinPoints[i].z < _MinPoint->z)
			{
				_MinPoint->z = MinPoints[i].z;
			}

			if (MaxPoints[i].x > _MaxPoint->x)
			{
				_MaxPoint->x = MaxPoints[i].x;
			}
			if (MaxPoints[i].y > _MaxPoint->y)
			{
				_MaxPoint->y = MaxPoints[i].y;
			}
			if (MaxPoints[i].z > _MaxPoint->z)
			{
				_MaxPoint->z = MaxPoints[i].z;
			}
		}
	}


	return 0;
}




/*
_PointPos:�����
_CurveId:���߶���
*/
//��õ㵽���ߵ���С����
double ObjectArx::GetPointToCurveDist(AcGePoint3d _PointPos, AcDbObjectId _CurveId)
{
	AcGePoint3d PointSearch;


	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _CurveId, AcDb::kForRead) != 0)
	{
		return 0.0;
	}
	AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
	if (pCurve == NULL)
	{
		pEnt->close();
		return 0.0;
	}
	pCurve->getClosestPointTo(_PointPos, PointSearch, Adesk::kFalse);
	pCurve->close();


	return _PointPos.distanceTo(PointSearch);
}



//��õ㵽���ߵ������
AcGePoint3d ObjectArx::GetPointToCurveMidPoint(AcGePoint3d _PointPos, AcDbObjectId _CurveId)
{
	AcGePoint3d PointSearch(0.0, 0.0, 0.0);


	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _CurveId, AcDb::kForRead) != 0)
	{
		return PointSearch;
	}
	AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
	if (pCurve == NULL)
	{
		pEnt->close();
		return PointSearch;
	}
	pCurve->getClosestPointTo(_PointPos, PointSearch, Adesk::kFalse);
	pCurve->close();

	return PointSearch;
}


//�����������Ľ���
AcGePoint3dArray ObjectArx::GetObjectInter(AcDbObjectId _ObjectA, AcDbObjectId _ObjectB, AcDb::Intersect _Type)
{
	AcGePoint3dArray ListPoints = NULL;


	do
	{
		AcDbEntity* pEnt1, * pEnt2;
		if (acdbOpenAcDbEntity(pEnt1, _ObjectA, AcDb::kForWrite) != 0)
		{
			break;
		}
		if (acdbOpenAcDbEntity(pEnt2, _ObjectB, AcDb::kForWrite) != 0)
		{
			pEnt1->close();
			break;
		}


		int Err = pEnt1->intersectWith(pEnt2, _Type, ListPoints);


		pEnt1->close();
		pEnt2->close();

	} while (false);


	return ListPoints;
}


/*
_PointPos:�����
_CurveId:���߶���
_Tolerance:����
*/
//�жϵ��Ƿ���������
bool ObjectArx::IsPointInCurve(AcGePoint3d _PointPos, AcDbObjectId _CurveId, double _Tolerance)
{
	AcGePoint3d PointSearch;


	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _CurveId, AcDb::kForRead) != 0)
	{
		return 0;
	}
	AcDbCurve* pCurve = AcDbCurve::cast(pEnt);
	if (pCurve == NULL)
	{
		pEnt->close();
		return 0;
	}
	pCurve->getClosestPointTo(_PointPos, PointSearch, Adesk::kFalse);


	if (PointSearch.isEqualTo(_PointPos) == true)
	{
		pEnt->close();
		return 1;
	}
	pEnt->close();
	return 0;
}
bool ObjectArx::IsPointInCurve(AcGePoint3d _PointPos, AcDbObjectIdArray _ListCurves, double _Tolerance)
{
	for (int i = 0; i < _ListCurves.length(); i++)
	{
		if (IsPointInCurve(_PointPos, _ListCurves[i], _Tolerance) == 1)
		{
			return 1;
		}
	}
	return 0;
}



/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


































/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


//���ʵ�嵽��ǰ����
AcDbObjectId ObjectArx::AddObjectToCurSpace(AcDbEntity* _pEnt, AcDbDatabase* _pDatabase)
{
	AcDbObjectId ObjectId = NULL;

	do
	{
		//������ݿ��еĿ�����
		AcDbBlockTable* pBlockTable = NULL;
		if (_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead) != NULL)
		{
			break;
		}

		//��õ�ǰ����
		CString LayoutName = ObjectArx::GetCurrentLayout();

		//���ģ�Ϳռ�Ŀ���¼
		AcDbBlockTableRecord* pBlockTableRecord;
		if (pBlockTable->getAt(LayoutName, pBlockTableRecord, AcDb::kForWrite) != NULL)
		{
			pBlockTable->close();
			break;
		}

		//��������ӵ�����¼��
		pBlockTableRecord->appendAcDbEntity(ObjectId, _pEnt);

		//�رն���
		pBlockTable->close();
		pBlockTableRecord->close();
		_pEnt->close();

	} while (false);

	return ObjectId;
}


/*
pEnt:Ҫ��ӵ�ָ�����
_pEnts:ָ������
_pDatabase:Ҫ��ӵ����ݿ����
*/
//���ʵ�嵽ģ�Ϳռ�
AcDbObjectId ObjectArx::AddObjectToModl(AcDbEntity* _pEnt, AcDbDatabase* _pDatabase)
{
	AcDbObjectId ObjectId = NULL;


	//������ݿ��еĿ�����
	AcDbBlockTable* pBlockTable = NULL;
	_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead);


	//���ģ�Ϳռ�Ŀ���¼
	AcDbBlockTableRecord* pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);

	//��ʬ����ӵ�����¼��
	pBlockTableRecord->appendAcDbEntity(ObjectId, _pEnt);


	//�رն���
	pBlockTable->close();
	pBlockTableRecord->close();
	_pEnt->close();
	return ObjectId;
}
AcDbObjectIdArray ObjectArx::AddObjectToModl(AcDbVoidPtrArray* _pEnts, AcDbDatabase* _pDatabase)
{
	AcDbObjectIdArray ListObjects = NULL;
	for (int i = 0; i < _pEnts->length(); i++)
	{
		ListObjects.append(AddObjectToModl((AcDbEntity*)_pEnts->at(i)));
	}
	return ListObjects;
}



//��ӿ鵽ģ�Ϳռ�
AcDbObjectId ObjectArx::AddBlockToModl(CString _StrBlockName, AcGePoint3d _PointDatum)
{
	//��õ�ǰ���ݿ�Ŀ��
	AcDbBlockTable* pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForWrite);


	//�����û�ָ���Ŀ鶨���Ƿ����
	if (pBlockTable->has(_StrBlockName) == 0)
	{
		pBlockTable->close();
		return NULL;
	}


	//����û�ָ���Ŀ���¼
	AcDbObjectId BlockDefId;
	if (pBlockTable->getAt(_StrBlockName, BlockDefId) != 0)
	{
		pBlockTable->close();
		return NULL;
	}


	//��������ն���
	AcDbBlockReference* pBlockRef = new AcDbBlockReference(_PointDatum, BlockDefId);


	//���������ӵ�ģ�Ϳռ�
	AcDbBlockTableRecord* pBlockTableRcd;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRcd, AcDb::kForWrite);
	AcDbObjectId ObjectId;
	if (pBlockTableRcd->appendAcDbEntity(ObjectId, pBlockRef) != 0)
	{
		pBlockTableRcd->close();
		pBlockRef->close();
		pBlockTable->close();
		return NULL;
	}


	//�ر����ݿ�Ķ���
	pBlockRef->close();
	pBlockTableRcd->close();
	pBlockTable->close();


	return ObjectId;
}


//��ӿ鵽ģ�Ϳռ�
AcDbObjectId ObjectArx::AddBlockToModl(AcDbObjectId _BlockTableRecordId, AcGePoint3d _PointDatum)
{
	AcDbObjectId EntityId = NULL;


	do
	{
		//��õ�ǰ���ݿ�Ŀ��
		AcDbBlockTable* pBlockTable;
		acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForWrite);


		//��������ն���
		AcDbBlockReference* pBlockReference = new AcDbBlockReference(_PointDatum, _BlockTableRecordId);


		//���������ӵ�ģ�Ϳռ�
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
		pBlockTableRecord->appendAcDbEntity(EntityId, pBlockReference);


		//�ر����ݿ�Ķ���
		pBlockReference->close();
		pBlockTableRecord->close();
		pBlockTable->close();

	} while (false);


	return EntityId;
}



//��ָ��·���µĿ���뵽��ǰ���ݿ�
int ObjectArx::ImportBlockTableRecord(CString _StrPath, CString _StrBlockName)
{
	int Err = 0;


	do
	{
		//��õ�ǰ���ݿ�Ŀ��
		AcDbBlockTable* pBlockTable = NULL;
		if (acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead) != 0)
		{
			Err = -1;
			break;
		}


		//������ݿ����Ƿ����ָ������¼
		AcDbObjectId BlockTableRecordId = NULL;
		if (pBlockTable->getAt(_StrBlockName, BlockTableRecordId) == 0)
		{
			pBlockTable->close();
			Err = -2;
			break;
		}


		//�رտ��
		pBlockTable->close();


		//��ȡDWG�ļ�������ݿ�
		AcDbDatabase* pExternal = new AcDbDatabase(false);
#if OBJECTARX2010 || OBJECTARX2011 || OBJECTARX2012 || OBJECTARX2013 || OBJECTARX2014 || OBJECTARX2015 || OBJECTARX2016
		if (pExternal->readDwgFile(_StrPath, _SH_DENYNO) != 0)
#elif OBJECTARX2017 || OBJECTARX2018 || OBJECTARX2019 || OBJECTARX2020
		if (pExternal->readDwgFile(_StrPath) != 0)
#endif 
		{
			delete pExternal;
			Err = -3;
			break;
		}


		//����ͼ��
		Err = ImportBlockTableRecord(pExternal, _StrBlockName);


		//�ͷ��ڴ�
		delete pExternal;


	} while (false);


	return Err;
}
int ObjectArx::ImportBlockTableRecord(AcDbDatabase* _pDatabase, CString _StrBlockName)
{
	int Err = 0;


	do
	{
		//���ⲿͼ�����ݿ�Ŀ��
		AcDbBlockTable* pBlockTable = NULL;
		if (_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead) != 0)
		{
			Err = -1;
			break;
		}


		//�ж��ⲿͼ�����ݿ��Ƿ����ָ�����ƵĿ�
		if (pBlockTable->has(_StrBlockName) != 1)
		{
			Err = -2;
			pBlockTable->close();
			break;
		}


		//���ָ�����ƵĿ���¼Id
		AcDbObjectId BlockTableRecordId = NULL;
		if (pBlockTable->getAt(_StrBlockName, BlockTableRecordId) != 0)
		{
			pBlockTable->close();
			Err = -3;
			break;
		}


		//��ָ����ͼ�������һ����ʱͼ�����ݿ�
		AcDbDatabase* pTempDb = NULL;
		_pDatabase->wblock(pTempDb, BlockTableRecordId);


		//�Ѹ���ʱͼ�����ݿ���Ϊ����뵽��ǰdwg
		acdbHostApplicationServices()->workingDatabase()->insert(BlockTableRecordId, _StrBlockName, pTempDb);


		//�ͷ��ڴ�
		delete pTempDb;
		pBlockTable->close();

	} while (false);


	return Err;
}


AcDbObjectIdArray ObjectArx::splitCurves(AcDbObjectId _entityId, AcDbObjectId _cuttingId) {

	AcDbObjectIdArray segments;

	do {

		//�󽻵�
		AcGePoint3dArray intersection = ObjectArx::GetObjectInter(_entityId, _cuttingId);
		if (intersection.length() <= 0) {
			continue;
		}

		//������
		AcDbCurve* pCurve = NULL;
		if (::acdbOpenObject(pCurve, _entityId, AcDb::OpenMode::kForWrite) != 0) {
			continue;
		}

		//���ʵ��
		AcDbVoidPtrArray curveSegments;
		pCurve->getSplitCurves(intersection, curveSegments);

		//��ӵ�ģ�Ϳռ�
		for (int i = 0; i < curveSegments.length(); i++) {
			AcDbEntity* pEntity = (AcDbEntity*)curveSegments.at(i);
			if (pEntity == NULL) {
				continue;
			}

			segments.append(ObjectArx::AddObjectToModl(pEntity));
		}

		//�رն���
		pCurve->close();

	} while (false);

	return segments;
}


bool ObjectArx::RemovePolylineRepeatVertex(AcDbObjectId _polylineId) {

	bool isReturn = false;

	do {

		AcDbPolyline* pPolyline = NULL;
		if (acdbOpenObject(pPolyline, _polylineId, AcDb::kForWrite) != 0) {
			break;
		}

		AcGePoint3d previousPoint;
		for (int i = 1; i < pPolyline->numVerts(); i++) {
			AcGePoint3d point1;
			if (pPolyline->getPointAt(i - 1, point1) != 0) {
				continue;
			}
			AcGePoint3d point2;
			if (pPolyline->getPointAt(i, point2) != 0) {
				continue;
			}

			if (point1.isEqualTo(point2) == true) {
				pPolyline->removeVertexAt(i--);
			}
		}

		pPolyline->close();

	} while (false);

	return isReturn;
}

//��������
AcDbObjectId ObjectArx::ImportBlockReference(CString _StrPath, CString _StrBlockName)
{
	AcDbObjectId EntityId = NULL;


	do
	{
		//���ģ�Ϳռ����һ������
		AcDbObjectId EndEntityId = GetEndEntity();


		//�����ݿ�ģ�Ϳռ����¼
		AcDbDatabase* pExternalDatabase = new AcDbDatabase(Adesk::kFalse);
		if (Acad::eOk != pExternalDatabase->readDwgFile(_StrPath))
		{
			break;
		}


		//�򿪿��
		AcDbBlockTable* pBlockTable = NULL;
		pExternalDatabase->getSymbolTable(pBlockTable, AcDb::kForRead);


		//���ģ�Ϳռ�Ŀ���¼
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
		pBlockTable->close();


		//��������¼������
		AcDbBlockTableRecordIterator* pBlockTableRecordIterator = NULL;
		pBlockTableRecord->newIterator(pBlockTableRecordIterator);


		//��������¼
		bool IsFind = false;
		bool IsErase = false;
		AcDbEntity* pEnt = NULL;
		AcDbBlockReference* pBlockReference = NULL;
		for (pBlockTableRecordIterator->start(); !pBlockTableRecordIterator->done(); pBlockTableRecordIterator->step())
		{
			if (pBlockTableRecordIterator->getEntity(pEnt, AcDb::kForWrite) != 0)
			{
				continue;
			}


			//�ж��Ƿ���ָ����
			IsErase = true;
			pBlockReference = AcDbBlockReference::cast(pEnt);
			if (pBlockReference == NULL)
			{
				IsErase = true;
			}
			else if (GetBlockName(pBlockReference) == _StrBlockName)
			{
				if (IsFind == false)
				{
					IsErase = false;
					IsFind = true;
				}
			}


			//ɾ��ʵ��
			if (IsErase == true)
			{
				pEnt->erase();
			}


			pEnt->close();
		}


		//�رտ��
		pBlockTableRecord->close();
		delete pBlockTableRecordIterator;


		//�����ݿ��е����ݵ���һ����ʱ���ݿ�
		AcDbDatabase* pTempDatabase = NULL;
		if (Acad::eOk != pExternalDatabase->wblock(pTempDatabase))
		{
			delete pExternalDatabase;
			break;
		}


		//����ʱ���ݿ��е����ݵ������ݿ�
		acdbHostApplicationServices()->workingDatabase()->insert(AcGeMatrix3d::kIdentity, pTempDatabase);


		//�ر����ݿ�
		delete pTempDatabase;
		delete pExternalDatabase;


		//���ʵ�����
		EntityId = GetEndEntity();
		if (EndEntityId == EntityId)
		{
			EntityId = NULL;
		}

	} while (false);


	return EntityId;
}



/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/







//����
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

//��õ���ֱ�ߵĴ���
AcGePoint3d ObjectArx::GetVerticalPoint(AcGePoint3d _PointOver, AcGePoint3d _PointLineA, AcGePoint3d _PointLineB)
{
	double k = 0.0;
	AcGePoint3d PointVertical(0.0, 0.0, 0.0);
	if (abs(_PointLineB.x - _PointLineA.x) < 0.002)
	{
		PointVertical.x = _PointLineA.x;
		PointVertical.y = _PointOver.y;
	}
	else
	{
		k = (_PointLineB.y - _PointLineA.y) / (_PointLineB.x - _PointLineA.x);
		PointVertical.x = (k * k * _PointLineA.x + k * (_PointOver.y - _PointLineA.y) + _PointOver.x) / (k * k + 1);
		PointVertical.y = k * (PointVertical.x - _PointLineA.x) + _PointLineA.y;
	}
	return PointVertical;
}


/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/