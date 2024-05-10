#include "pch.h"
#include "Arx_Func.h"









//获得CAD窗口
HWND ObjectArx::GetAutoCADWindow()
{
	CWnd* Cwhd = acedGetAcadFrame();
	return Cwhd->GetSafeHwnd();
}



//发送CAD命令(非模态)
int ObjectArx::SendMessageAutoCAD(CString StrCommand)
{
	acDocManager->sendStringToExecute(curDoc(), StrCommand);
	return 0;
}


//获得桌面路径
CString ObjectArx::GetWindowPath()
{
	CString StrPath = _T("");


	wchar_t* pPath = new wchar_t[1024];
	SHGetSpecialFolderPath(0, pPath, CSIDL_DESKTOPDIRECTORY, 0);
	StrPath = pPath;
	delete pPath;


	return StrPath;
}

//获得acad安装路径
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

//去除教育版
bool ObjectArx::RemoveEduPlotStamp(AcDbDatabase* _pDatabase)
{
	bool IsErr = false;


#if OBJECTARX2014 || OBJECTARX2015 || OBJECTARX2016 || OBJECTARX2017


	do
	{
		//如果是空指针或者是教育版，则返回
		if (_pDatabase == NULL || !_pDatabase->isEMR())
		{
			break;
		}


		INT_PTR d1 = (INT_PTR)_pDatabase;
		//对从数据库指针的下一个地址就是mpImpDb地址,实际上是一个二级指针
#ifdef _WIN64
		d1 += 0x00000008;
#else
		d1 += 0x00000004;
#endif

		INT_PTR d2 = (INT_PTR)(*((void**)d1));			//取到内部的mpImpDb地址
		for (int i = 0x600; i < 0x1000; i++)
		{
			INT_PTR* pIsEMR = (INT_PTR*)(d2 + i);		//取得此地址的内容
			if ((*pIsEMR) == 0x01)						//如果此地址的内容是0x1:教育版 
			{
				*pIsEMR = 0x00;							//转为0x0:非教育版,
				if (!_pDatabase->isEMR())
				{
					IsErr = true;
					break;
				}
				else
				{
					*pIsEMR = 0x01;						//不成功，则保留原内容
				}
			}
		}

	} while (false);


#endif

	return IsErr;
}





/*--------------------------------------------------------------------------------------------------------------------------------------------*/












//创建对象
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



//创建点
AcDbObjectId ObjectArx::CreatePoint(AcGePoint3d _PointPos)
{
	AcDbPoint* pPoint = new AcDbPoint(_PointPos);
	return AddObjectToModl(pPoint);
}



/*
_StartPoint:起点
_EndPoint:端点
*/
//创建直线
AcDbObjectId ObjectArx::CreateLine(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint)
{
	AcDbLine* pLine = new AcDbLine(_StartPoint, _EndPoint);
	return AddObjectToModl(pLine);
}


//创建射线
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


//创建构造线
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
_Center:圆心
_Radius:半径
_StartAngle:起始角度
_EndAngle:终止角度
*/
//创建圆弧
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

//创建多段线
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

//创建样条曲线
AcDbObjectId ObjectArx::CreateSpline(AcGePoint3dArray _points)
{
	AcDbSpline* pSpline = new AcDbSpline(_points);
	return AddObjectToModl(pSpline);
}


//创建面域
AcDbObjectIdArray ObjectArx::CreateRegion(AcDbObjectIdArray _ListObjects)
{
	AcDbObjectIdArray ListRegionIds;

	AcDbEntity* pEnt;


	//将所有实体转换成曲线
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


	//创建面域
	AcDbObjectId RegionId;
	AcDbRegion* pRegion;
	AcDbVoidPtrArray ListRegionPointers;
	int Err = AcDbRegion::createFromCurves(ListCurvePointers, ListRegionPointers);
	if (Err == 0)
	{
		//将生成的面域添加到模型空间
		for (int i = 0; i < ListRegionPointers.length(); i++)
		{
			//将空指针(可指向任何类型)转化为指向面域的指针
			pRegion = static_cast<AcDbRegion*>(ListRegionPointers[i]);
			pRegion->setDatabaseDefaults();
			RegionId = AddObjectToModl(pRegion);
			ListRegionIds.append(RegionId);
		}
	}
	else //如果创建不成功，也要删除已经生成的面域
	{
		for (int i = 0; i < ListRegionPointers.length(); i++)
		{
			delete (AcRxObject*)ListRegionPointers[i];
		}
	}
	//关闭作为边界的对象
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



//创建拉伸
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




//创建文字
AcDbObjectId ObjectArx::CreateText(AcGePoint3d _PointPos, CString _StrText, double _Height, double _Rotation, CString _TextStyle)
{
	AcDbObjectId TextId = NULL;

	do
	{
		//获得文字样式
		AcDbObjectId TextStyleId = GetTextStyleId(_TextStyle);


		//创建文字
		AcDbText* pText = new AcDbText(_PointPos, _StrText, TextStyleId, _Height, 3.14159265358 / 180 * _Rotation);
		if (pText == NULL)
		{
			break;
		}


		//设置文字样式
		pText->setTextStyle(TextStyleId);


		//添加到模型空间
		TextId = AddObjectToModl(pText);


	} while (false);


	return TextId;
}


AcDbObjectId ObjectArx::CreateMText(AcGePoint3d _PointPos, CString _StrText, double _Height, double _Rotation, CString _TextStyle)
{
	AcDbObjectId TextId = NULL;

	do
	{
		//获得文字样式
		AcDbObjectId TextStyleId = GetTextStyleId(_TextStyle);


		//创建文字
		AcDbMText* pText = new AcDbMText();
		if (pText == NULL)
		{
			break;
		}

		pText->setLocation(_PointPos);
		pText->setContents(_StrText);
		pText->setTextHeight(_Height);
		pText->setRotation(3.1415926 / 180 * _Rotation);


		//设置文字样式
		pText->setTextStyle(TextStyleId);


		//添加到模型空间
		TextId = AddObjectToModl(pText);


	} while (false);


	return TextId;
}


//创建引线
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



//设置填充比例
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



//设置引线箭头尺寸
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
"":实心闭合
"_DOT":点
"_DOTSMALL":小点
"_DOTBLANK":空心点
"_ORIGIN":原点标记
"_ORIGIN2":原点标记 2
"_OPEN":打开
"_OPEN90":直角
"_OPEN30":30 度角
"_CLOSED":闭合
"_SMALL":空心小点
"_NONE":无
"_OBLIQUE":倾斜
"_BOXFILLED":实心框
"_BOXBLANK":框
"_CLOSEDBLANK":空心闭合
"_DATUMFILLED":实心基准三角形
"_DATUMBLANK":基准三角形
"_INTEGRAL":完整标记
"_ARCHTICK":建筑标记
*/
//设置引线箭头
void ObjectArx::SetLeaderArrowStyle(AcDbObjectId _ObjectId, CString _StyleName)
{
	AcDbLeader* pLeader = NULL;
	if (acdbOpenObject(pLeader, _ObjectId, AcDb::kForWrite) == 0)
	{
		if (_StyleName == _T(""))
		{
			_StyleName = _T("");
		}
		else if (_StyleName == _T("点"))
		{
			_StyleName = _T("_DOT");
		}
		else if (_StyleName == _T("小点"))
		{
			_StyleName = _T("_DOTSMALL");
		}
		else if (_StyleName == _T("空心点"))
		{
			_StyleName = _T("_DOTBLANK");
		}
		else if (_StyleName == _T("原点标记"))
		{
			_StyleName = _T("_ORIGIN");
		}
		else if (_StyleName == _T("原点标记2"))
		{
			_StyleName = _T("_ORIGIN2");
		}
		else if (_StyleName == _T("打开"))
		{
			_StyleName = _T("_OPEN");
		}
		else if (_StyleName == _T("直角"))
		{
			_StyleName = _T("_OPEN90");
		}
		else if (_StyleName == _T("30度角"))
		{
			_StyleName = _T("_OPEN30");
		}
		else if (_StyleName == _T("闭合"))
		{
			_StyleName = _T("_CLOSED");
		}
		else if (_StyleName == _T("空心小点"))
		{
			_StyleName = _T("_SMALL");
		}
		else if (_StyleName == _T("无"))
		{
			_StyleName = _T("_NONE");
		}
		else if (_StyleName == _T("倾斜"))
		{
			_StyleName = _T("_OBLIQUE");
		}
		else if (_StyleName == _T("实心框"))
		{
			_StyleName = _T("_BOXFILLED");
		}
		else if (_StyleName == _T("框"))
		{
			_StyleName = _T("_BOXBLANK");
		}
		else if (_StyleName == _T("空心闭合"))
		{
			_StyleName = _T("_CLOSEDBLANK");
		}
		else if (_StyleName == _T("实心基准三角形"))
		{
			_StyleName = _T("_DATUMFILLED");
		}
		else if (_StyleName == _T("基准三角形"))
		{
			_StyleName = _T("_DATUMBLANK");
		}
		else if (_StyleName == _T("完整标记"))
		{
			_StyleName = _T("_INTEGRAL");
		}
		else if (_StyleName == _T("建筑标记"))
		{
			_StyleName = _T("_ARCHTICK");
		}

		pLeader->setDimldrblk(_StyleName);
		pLeader->close();
	}
}



//获得文字基点
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


//设置文字高度
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


//设置文字宽度
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

//设置文字样式
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


//设置文字内容
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
















//对象处理
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


//判断是否是实体
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




//复制对象
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



//复制对象
AcDbObjectIdArray ObjectArx::CopyEntity(AcDbObjectIdArray _ListEntitys)
{
	AcDbObjectIdArray ListEntitys = NULL;
	for (int i = 0; i < _ListEntitys.length(); i++)
	{
		ListEntitys.append(CopyEntity(_ListEntitys[i]));
	}
	return ListEntitys;
}



//创建对象偏移
AcDbObjectIdArray ObjectArx::CreateOffset(AcDbObjectId _ObjectId, double _VarDeviation, AcDbDatabase* _pDatabase)
{
	AcDbObjectIdArray ListEntitys = NULL;


	do
	{
		//打开对象
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
_ObjectId:要删除的对象Id
_ListObjects:对象链表
*/
//删除对象
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
		//打开对象
		if (acdbOpenObject(pEnt, _ListObjects[i], AcDb::kForWrite) != 0)
		{
			continue;
		}


		pEnt->erase();


		pEnt->close();
	}
	return 0;
}



//获取对象颜色
int ObjectArx::GetObjectColor(AcDbObjectId _ObjectId)
{
	int Color = 0;


	do
	{
		//注：颜色为1——256，1表随块，256表随层
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
_ObjectId:对象Id
_Color:颜色编号
_ListObjects:对象链表
*/
//设置对象颜色
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
		//打开对象
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
		//打开被分解的实体
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		pViewport->setViewCenter(_Center);


		pViewport->close();

	} while (false);
}



//设置视口视图高度
void ObjectArx::SetViewportViewHeight(AcDbObjectId _ObjectId, double _Height)
{
	do
	{
		//打开被分解的实体
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		pViewport->setViewHeight(_Height);


		pViewport->close();

	} while (false);
}



//设置视口视图宽度
void ObjectArx::SetViewportWidth(AcDbObjectId _ObjectId, double _Width)
{
	do
	{
		//打开被分解的实体
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		pViewport->setWidth(_Width);


		pViewport->close();

	} while (false);
}



//设置视口视图高度
void ObjectArx::SetViewportHeight(AcDbObjectId _ObjectId, double _Height)
{
	do
	{
		//打开被分解的实体
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		pViewport->setHeight(_Height);


		pViewport->close();

	} while (false);
}



//设置视口视图比例
void ObjectArx::SetViewportStandardScale(AcDbObjectId _ObjectId, AcDbViewport::StandardScaleType _Scale)
{
	do
	{
		//打开被分解的实体
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		pViewport->setStandardScale(_Scale);


		pViewport->close();

	} while (false);
}


//设置视口视图自定义比例
void ObjectArx::SetViewportCustomScale(AcDbObjectId _ObjectId, double _Scale)
{
	do
	{
		//打开被分解的实体
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		pViewport->setCustomScale(_Scale);


		pViewport->close();

	} while (false);
}


//设置对象显示或者隐藏
int ObjectArx::SetObjectShow(AcDbObjectId ObjectId, bool BolShow)
{
	int Return = 0;


	do
	{
		//打开对象
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


//设置直线起点
int ObjectArx::SetLineStartPoint(AcDbObjectId _LineId, AcGePoint3d _StartPoint)
{
	int Err = 0;


	do
	{
		//打开对象
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


//设置直线终点
int ObjectArx::SetLineEndPoint(AcDbObjectId _LineId, AcGePoint3d _EndPoint)
{
	int Err = 0;


	do
	{
		//打开对象
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


//设置构造线基准点
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


//设置对象图层
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



//获得对象所在图层
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


//获得视口中心
AcGePoint2d ObjectArx::GetViewportViewCenter(AcDbObjectId _ObjectId)
{
	AcGePoint2d Center(0, 0);


	do
	{
		//打开被分解的实体
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



//获得视口视图高度
double ObjectArx::GetViewportViewHeight(AcDbObjectId _ObjectId)
{
	double Height = 0;


	do
	{
		//打开被分解的实体
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


//获得视口宽度
double ObjectArx::GetViewportWidth(AcDbObjectId _ObjectId)
{
	double Width = 0;


	do
	{
		//打开被分解的实体
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


//获得视口高度
double ObjectArx::GetViewportHeight(AcDbObjectId _ObjectId)
{
	double Width = 0;


	do
	{
		//打开被分解的实体
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


//获得视口视图比例
AcDbViewport::StandardScaleType ObjectArx::SetViewportStandardScale(AcDbObjectId _ObjectId)
{
	AcDbViewport::StandardScaleType StandardScale;


	do
	{
		//打开被分解的实体
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


//获得视口视图自定义比例
double ObjectArx::GetViewportCustomScale(AcDbObjectId _ObjectId)
{
	double Scale = 1;


	do
	{
		//打开被分解的实体
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


//获得对象显示或者隐藏
bool ObjectArx::GetObjectShow(AcDbObjectId ObjectId)
{
	bool IsErr = false;


	do
	{
		//打开对象
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


//获得文字范围
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


//获得圆弧圆心
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


//获得圆弧法线
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


//获得圆弧半径
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


//获得圆弧起始角度
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

//获得圆弧终止角度
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



//分解一个对象
AcDbVoidPtrArray ObjectArx::ExplodeObject(AcDbObjectId _ObjectId, bool BolDelete)
{
	//打开被分解的实体
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForWrite) != 0)
	{
		return -1;
	}


	//分解对象
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
		//打开被分解的实体
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			break;
		}


		//分解对象
		AcDbVoidPtrArray* pEnts = new AcDbVoidPtrArray();
		if (pEnt->explode(*pEnts) != 0)
		{
			pEnt->close();
			break;
		}


		//是否删除原对象
		if (BolDelete == 1)
		{
			pEnt->erase();
		}
		pEnt->close();


		//添加到模型空间
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


//对象转句柄
CString ObjectArx::ObjectIdToHandle(AcDbObjectId _ObjectId)
{
	TCHAR pBuffer[256] = { 0 };
	_ObjectId.handle().getIntoAsciiBuffer(pBuffer);
	return pBuffer;
}



/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/













/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


//打开文件
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



//另存为对话框
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
StrPrompt:选择对象时的提示
PointCoor:返回的选择对象时鼠标对应的点

返回所选择的对象
*/
//选择一个对象返回实体ID
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



//框选实体返回实体集
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



//选择点
int ObjectArx::SelectPoint(CString _StrPrompt, AcGePoint3d* _PointPos)
{
	//获得用户输入的块参照的插入点
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
		if (acedGetPoint(ptStart, _T("请输入下一点：\n"), ptEnt) != RTNORM)
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



//设置选中对象
int ObjectArx::SetSelectObject(AcDbObjectId _ObjectId)
{
	//高亮选中对象
	ads_name AdsSelects, AdsName;

	//创建选择集
	acedSSAdd(NULL, NULL, AdsSelects);

	//添加实体到选择集
	acdbGetAdsName(AdsName, _ObjectId);
	acedSSAdd(AdsName, AdsSelects, AdsSelects);

	//高亮对象
	ads_sssetfirst(AdsSelects, NULL);

	//释放内存
	acedSSFree(AdsName);
	acedSSFree(AdsSelects);
	return 0;
}



//设置选中对象
int ObjectArx::SetSelectObject(AcDbObjectIdArray _ListObjects)
{
	//高亮选中对象
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


//取消选择
int ObjectArx::CancelSelect()
{
	ads_sssetfirst(NULL, NULL);
	return 0;
}


//获得模型空间中最后一个实体
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

//获得点下的实体
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
//		if (acedSSGet(NULL, asDblArray(_point), NULL, NULL, adsName) != RTNORM)  //获得双击的选择集
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





//绝对坐标的点转为工作坐标的点
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


//工作坐标的点转为绝对坐标的点
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



//指定矩阵移动对象
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



//点到点移动
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



//旋转对象
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


//缩放对象
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





//镜像
void ObjectArx::Mirroring(AcDbObjectId _EntityId, AcGePoint3d _StartPoint, AcGePoint3d _EndPoint)
{
	do
	{
		AcDbEntity* pEnt = NULL;
		if (acdbOpenObject(pEnt, _EntityId, AcDb::kForWrite) != 0)
		{
			break;
		}


		//设置矩阵
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


//获得多段线点数量
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


//设置多段线点
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



//添加多段线的点
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



//获得多段线点
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



//获得多段线点
AcGePoint3dArray ObjectArx::GetPolyVertex(AcDbObjectId _PolylineId)
{
	//打开对象
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



//移除多段线点
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



//在曲线中获得两点的长度
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


//判断对象是否是曲线
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
_CurveId:曲线Id
_Param:参数百分比0表起始点,1表结束点
*/
//获得曲线点
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



//获得长度
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



//获得点在曲线上的位置(比值)
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


	//获得曲线长度
	double LengthCurve = 0.0;
	pCurve->getDistAtParam(EndValue, LengthCurve);


	//获得相对长度
	double Length = 0.0;
	pCurve->getDistAtPoint(_PointPos, Length);

	pEnt->close();
	return Length / LengthCurve;
}



//判断曲线是否闭合
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



//设置多段线是否闭合
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



//判断点是否在多段线内
bool ObjectArx::IsPointInCurve(AcDbObjectId _PolylineId, AcGePoint3d _PointPos, double _Tolerance)
{
	//打开实体
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


	//用过线段起点的射线与图框求交
	AcGePoint3dArray ListPoints = NULL;
	ListPoints.setLogicalLength(0);
	if (pRay->intersectWith(pPolyline, AcDb::kOnBothOperands, ListPoints) != 0)
	{
		delete pRay;
		pPolyline->close();
		return 0;
	}


	//如果交点数为奇数,说明点在图框内部
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



//求曲线面积
double ObjectArx::GetCurveArea(AcDbObjectId _CurveId)
{
	//打开对象指针
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



//设置多段线法线
int ObjectArx::SetPolylineNormal(AcDbObjectId _PolylineId, AcGeVector3d _Normal)
{
	int Err = 0;


	do
	{
		//打开对象指针
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


		//设置法线
		pPolyline->setNormal(_Normal);
		pPolyline->close();


	} while (false);


	return Err;
}


//判断目标曲线是否在多段线内
bool ObjectArx::IsInCurveInside(AcDbObjectId _targetId, AcDbObjectId _polylineId, double _Tolerance)
{
	bool isInside = false;

	do {

		//获得曲线对象的三个点
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

				//如果点在曲线内也算是
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

//设置多段线全局宽度
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














//矩阵相关
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


//创建阵列
AcDbObjectIdArray ObjectArx::CreateObjectArray(AcDbObjectId _ObjectId, int _xCount, double _xDist, int _yCount, double _yDist)
{
	AcDbObjectIdArray ListObjects = NULL;
	ListObjects.append(_ObjectId);


	//构建用于实现移动实体的矩阵
	AcGeMatrix3d Matrix;
	AcGeVector3d Vector(0.0, 0.0, 0.0);
	Matrix.setToTranslation(Vector);


	//打开对象
	AcDbEntity* pEnt = NULL;
	if (acdbOpenObject(pEnt, _ObjectId, AcDb::kForWrite) != 0)
	{
		return NULL;
	}


	//复制移动对象
	AcDbEntity* pCopyEnt = NULL;
	for (int i = 0; i < _xCount; i++)
	{
		for (int u = 0; u < _yCount; u++)
		{
			if (i == 0 && u == 0)
			{
				continue;
			}

			//构建矩阵
			Vector.x = i * _xDist;
			Vector.y = u * _yDist;
			Vector.z = 0.0;
			Matrix.setToTranslation(Vector);

			//复制对象
			pCopyEnt = AcDbEntity::cast(pEnt->clone());

			//实现矩阵变换
			pCopyEnt->transformBy(Matrix);

			//添加到模型空间
			ListObjects.append(AddObjectToModl(pCopyEnt));
		}
	}
	pEnt->close();
	return ListObjects;
}


/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/











//扩展数据
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/



/*
_pEnt:对象指针
_ObjectId:对象Id
_StrName:扩展数据名称
*/
//判断该对象是否存在指定的扩展记录
bool ObjectArx::IsExtendData(AcDbEntity* _pEnt, CString _StrName)
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


	pDictionary->close();
	pXrec->close();
	return 1;
}
bool ObjectArx::IsExtendData(AcDbObjectId _ObjectId, CString _StrName)
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



/*
_pEnt:对象指针
_StrName:扩展数据名
_ObjectId:对象Id
pRb:扩展数据值
_StrText:扩展数据字符串
_DataVarArray:数据组
*/
//为扩展词典中添加扩展数据
int ObjectArx::AddExtendData(AcDbObjectId _ObjectId, CString _StrName, struct resbuf* (&pRb))
{
	int Err = -1;


	do
	{
		//打开图形对象,获得实体扩展字典的ObjectId
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForWrite) != 0)
		{
			Err = -2;
			break;
		}
		AcDbObjectId DictionaryId = pEnt->extensionDictionary();
		if (DictionaryId == NULL)
		{
			//创建扩展词典
			pEnt->createExtensionDictionary();
			DictionaryId = pEnt->extensionDictionary();
		}
		pEnt->close();


		//查看实体是否包含扩展字典,如果不包含则新建扩展词典
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


		//向扩展字典中添加一条记录  
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


		//像扩展记录上添加数据
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
			//创建扩展词典
			_pEnt->createExtensionDictionary();
			DictionaryId = _pEnt->extensionDictionary();
		}
		_pEnt->close();


		//查看实体是否包含扩展字典,如果不包含则新建扩展词典
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


		//向扩展字典中添加一条记录  
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


		//像扩展记录上添加数据
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
_pEnt:对象指针
_StrName:扩展数据名
_ObjectId:对象Id
pRb:扩展数据值
_StrText:扩展数据字符串
_DataVarArray:数据组
*/
//获得扩展词典中的扩展数据
int ObjectArx::GetExtendData(AcDbObjectId _ObjectId, CString _StrName, struct resbuf* (&pRb))
{
	int Err = -1;


	//打开图形对象,获得实体扩展字典的ObjectId
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
	{
		return -1;
	}
	AcDbObjectId DictionaryId = pEnt->extensionDictionary();
	pEnt->close();


	//查看实体是否包含扩展字典
	if (DictionaryId == AcDbObjectId::kNull)
	{
		return -2;
	}


	//打开扩展字典,获得指定名称的扩展记录
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


	//获得扩展记录的数据链表并关闭扩展数据对象
	Err = pXrec->rbChain(&pRb);
	pXrec->close();
	return 0;
}
int ObjectArx::GetExtendData(AcDbEntity* _pEnt, CString _StrName, struct resbuf* (&pRb))
{
	int Err = -1;


	//打开图形对象,获得实体扩展字典的ObjectId
	AcDbObjectId DictionaryId = _pEnt->extensionDictionary();


	//查看实体是否包含扩展字典
	if (DictionaryId == AcDbObjectId::kNull)
	{
		return -2;
	}


	//打开扩展字典,获得指定名称的扩展记录
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


	//获得扩展记录的数据链表并关闭扩展数据对象
	Err = pXrec->rbChain(&pRb);
	pXrec->close();
	return 0;
}
int ObjectArx::GetExtendData(AcDbObjectId _ObjectId, CString _StrName, CString* _StrText)
{
	int Err = -1;


	//查询扩展数据
	struct resbuf* pRb;
	Err = GetExtendData(_ObjectId, _StrName, pRb);
	if (Err != 0)
	{
		return -1;
	}


	//获得扩展数据
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


	//查询扩展数据
	struct resbuf* pRb;
	Err = GetExtendData(_pEnt, _StrName, pRb);
	if (Err != 0)
	{
		return -1;
	}


	//获得扩展数据
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
_pEnt:对象指针
_ObjectId:对象Id
_StrName:扩展数据名
*/
//删除扩展数据
int ObjectArx::DeleteExtendData(AcDbObjectId _ObjectId, CString _StrName)
{
	int Err = -1;


	//打开图形对象,获得实体扩展字典的ObjectId
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
	{
		return -1;
	}
	AcDbObjectId DictionaryId = pEnt->extensionDictionary();
	pEnt->close();


	//查看实体是否包含扩展字典
	if (DictionaryId == AcDbObjectId::kNull)
	{
		return -2;
	}


	//打开扩展字典,获得指定名称的扩展记录
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


	//打开图形对象,获得实体扩展字典的ObjectId
	AcDbObjectId DictionaryId = _pEnt->extensionDictionary();


	//查看实体是否包含扩展字典
	if (DictionaryId == AcDbObjectId::kNull)
	{
		return -2;
	}


	//打开扩展字典,获得指定名称的扩展记录
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



//删除图纸扩展词典中的扩展数据
int ObjectArx::DeleteDatabaseExtendData(CString _StrName, AcDbDatabase* _pDatabase)
{
	int Err = 0;


	do
	{
		//获得数据库扩展词典
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


		//移除扩展数据
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



//在数据库中添加扩展词典中添加扩展数据
int ObjectArx::AddDatabaseExtendData(CString _StrName, struct resbuf* (&pRb), AcDbDatabase* _pDatabase)
{
	int Err = 0;


	do
	{
		//获得数据库扩展词典
		AcDbDictionary* pDictionary = NULL;
		if (_pDatabase->getNamedObjectsDictionary(pDictionary, AcDb::kForWrite) != 0)
		{
			Err = -1;
			break;
		}


		//判断是否存在该
		if (pDictionary->has(_StrName) == 1)
		{
			pDictionary->close();
			Err = -2;
			break;
		}


		//向扩展字典中添加一条记录  
		AcDbObjectId RecObjId = NULL;
		AcDbXrecord* pXrec = new AcDbXrecord();
		pDictionary->setAt(_StrName, pXrec, RecObjId);
		pDictionary->close();


		//向扩展记录上添加数据
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


//获得数据库中指定名称的扩展数据
int ObjectArx::GetDatabaseExtendData(CString _StrName, struct resbuf* (&pRb), AcDbDatabase* _pDatabase)
{
	int Err = 0;


	do
	{
		//获得数据库扩展词典
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


		//向扩展字典中添加一条记录  
		AcDbXrecord* pXrec = NULL;
		if (pDictionary->getAt(_StrName, (AcDbObject*&)pXrec, AcDb::kForRead) != 0)
		{
			pDictionary->close();
			Err = -3;
			break;
		}
		pDictionary->close();


		//获得扩展记录的数据链表并关闭扩展数据对象
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
		//查询扩展数据
		struct resbuf* pRb;
		Err = GetDatabaseExtendData(_StrName, pRb, _pDatabase);
		if (Err != 0)
		{
			return -1;
		}


		//获得扩展数据
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




















//动态块相关
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


//获得块名称
CString ObjectArx::GetBlockName(AcDbBlockReference* _pBlockReference)
{
	CString BlockName = _T("");


	do
	{
		AcDbDynBlockReference* pDynBlockReference = new AcDbDynBlockReference(_pBlockReference);
		if (pDynBlockReference == NULL)
		{
			//打开块表记录
			AcDbBlockTableRecord* pBlockTableRecord = new AcDbBlockTableRecord();
			if (acdbOpenObject(pBlockTableRecord, _pBlockReference->blockTableRecord(), AcDb::kForWrite) != 0)
			{
				_pBlockReference->close();
				break;
			}


			//获得块名
			wchar_t* pBlockName = NULL;
			pBlockTableRecord->getName(pBlockName);
			BlockName = pBlockName;


			pBlockTableRecord->close();
		}
		else
		{
			//获得块表记录
			AcDbBlockTableRecord* pBlockTableRecord = NULL;
			if (acdbOpenObject(pBlockTableRecord, pDynBlockReference->dynamicBlockTableRecord(), AcDb::kForRead) != 0)
			{
				delete[]pDynBlockReference;
				break;
			}


			//获得块名称
			wchar_t* pBlockName = NULL;
			pBlockTableRecord->getName(pBlockName);
			BlockName = pBlockName;


			pBlockTableRecord->close();
			delete[]pDynBlockReference;
		}
	} while (false);


	return BlockName;
}


//创建块表记录
int nRandomBlockName = 0;
AcDbObjectId ObjectArx::CreateBlockTableRecord(CString _StrName)
{
	AcDbObjectId BlockTableRecordId = NULL;


	do
	{
		//获得当前图形数据库的块表
		AcDbBlockTable* pBlockTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForWrite);


		//创建新的块表记录
		AcDbBlockTableRecord* pBlockTableRecord = new AcDbBlockTableRecord();


		//判断是否存在该块
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
			//生成一个块名
			CTime Time = CTime::GetCurrentTime();
			while (true)
			{
				_StrName.Format(_T("%d%d%d%d%d%d%d%d"), Time.GetYear(), Time.GetMonth(), Time.GetDay(), Time.GetHour(),
					Time.GetMinute(), Time.GetSecond(), rand() % 10 * Time.GetMinute(), nRandomBlockName++);


				//判断是否存在该块
				if (pBlockTable->has(_StrName) == false)
				{
					break;
				}
			}
		}


		//根据用户的输入设置块表记录的名称
		if (pBlockTableRecord->setName(_StrName) != 0)
		{
			pBlockTable->close();
			break;
		}


		//将块表记录添加到块表中
		if (pBlockTable->add(BlockTableRecordId, pBlockTableRecord) != 0)
		{
			pBlockTableRecord->close();
			pBlockTable->close();
			break;
		}


		//释放内存
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
		//打开块表记录获得指针
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _ObjectId, AcDb::kForRead) != 0)
		{
			break;
		}


		//设置块比例
		Scale = pBlockReference->scaleFactors();;


		//释放内存
		pBlockReference->close();

	} while (false);

	return Scale;
}



void ObjectArx::SetBlockScale(AcDbObjectId _ObjectId, AcGeScale3d _Scale)
{
	do
	{
		//打开块表记录获得指针
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _ObjectId, AcDb::OpenMode::kForWrite) != 0)
		{
			break;
		}


		//设置块比例
		pBlockReference->setScaleFactors(_Scale);;


		//释放内存
		pBlockReference->close();

	} while (false);

}


//获得图块法线
AcGeVector3d ObjectArx::GetBlockNormal(AcDbObjectId _BlockId)
{
	AcGeVector3d normal = AcGeVector3d::kZAxis;

	do
	{
		//打开块表记录获得指针
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _BlockId, AcDb::OpenMode::kForRead) != 0)
		{
			break;
		}


		//设置块比例
		normal = pBlockReference->normal();


		//释放内存
		pBlockReference->close();

	} while (false);

	return normal;
}


//获得块名称
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


			//获得块名
			wchar_t* Tchar = NULL;
			pBlockTableRecord->getName(Tchar);
			StrBlockName = Tchar;


			pBlockTableRecord->close();
			pBlockReference->close();
		}
		else
		{
			//获得块表记录
			AcDbBlockTableRecord* pBlockTableRecord = NULL;
			if (acdbOpenObject(pBlockTableRecord, pDynBlockTableReference->dynamicBlockTableRecord(), AcDb::kForRead) != 0)
			{
				delete[]pDynBlockTableReference;
				break;
			}


			//获得块名称
			wchar_t* Tchar = NULL;
			pBlockTableRecord->getName(Tchar);
			StrBlockName = Tchar;


			pBlockTableRecord->close();
			delete[]pDynBlockTableReference;
		}
	} while (false);


	return StrBlockName;
}


//获得块表记录名称
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


//复制动态块
AcDbObjectId ObjectArx::CopyDynBlockReference(AcDbObjectId _EntityId) {
	AcDbObjectId EntityId = NULL;

	if (ObjectArx::IsDynBolck(_EntityId) == false) {
		EntityId = ObjectArx::CopyEntity(_EntityId);
	}
	else {
		//获得模型空间最后一个实体
		AcDbObjectId EndId = GetEndEntity();

		//将实体放到链表中
		AcDbObjectIdArray Entitys;
		Entitys.append(_EntityId);

		//写入到临时数据库
		AcDbDatabase* pTempDatabase = NULL;
		acdbHostApplicationServices()->workingDatabase()->wblock(pTempDatabase, Entitys, AcGePoint3d(0, 0, 0));

		//插入到当前图纸
		acdbHostApplicationServices()->workingDatabase()->insert(AcGeMatrix3d::kIdentity, pTempDatabase);

		//删除临时数据库
		delete pTempDatabase;

		//获得实体Id
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
		//获得当前模型空间的所有实体
		AcDbObjectIdArray OldEntitys = ObjectArx::IterateBlockTableRecord();


		//将动态块和非动态块分开来
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


		//复制动态块
		if (CopyEntitys.length() > 0) {

			//写入到临时数据库
			AcDbDatabase* pTempDatabase = NULL;
			acdbHostApplicationServices()->workingDatabase()->wblock(pTempDatabase, CopyEntitys, AcGePoint3d(0, 0, 0));

			//插入到当前图纸
			acdbHostApplicationServices()->workingDatabase()->insert(AcGeMatrix3d::kIdentity, pTempDatabase);

			//删除临时数据库
			delete pTempDatabase;

			//获得当前模型空间的所有实体
			AcDbObjectIdArray NewEntitys = ObjectArx::IterateBlockTableRecord();

			//获得新的实体
			for (int i = OldEntitys.length(); i < NewEntitys.length(); i++) {
				EntityIds.append(NewEntitys[i]);
			}
		}

	} while (false);

	return EntityIds;
}

//重置块参照
void ObjectArx::ResetReference(AcDbObjectId _ObjectId)
{
	AcDbDynBlockReference* pBlockReference = new AcDbDynBlockReference(_ObjectId);
	if (pBlockReference != NULL)
	{
		pBlockReference->resetBlock();
	}
}


//遍历块表记录
AcDbObjectIdArray ObjectArx::IterateBlockTableRecord(CString _BlockName, AcDbDatabase* _pDatabase)
{
	AcDbObjectIdArray ListEntitys = NULL;

	do
	{
		//打开块表
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::OpenMode::kForRead);


		//获得模型空间的块表记录
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		if (pBlockTable->getAt(_BlockName, pBlockTableRecord, AcDb::OpenMode::kForRead) != 0)
		{
			pBlockTable->close();
			break;
		}
		pBlockTable->close();


		//遍历块表记录
		ListEntitys = IterateBlockTableRecord(pBlockTableRecord);


		//关闭块表记录
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
			//打开块表
			AcDbBlockTable* pBlockTable = NULL;
			_pDatabase->getBlockTable(pBlockTable, AcDb::OpenMode::kForRead);


			//获得模型空间的块表记录
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
		//创建遍历器
		AcDbBlockTableRecordIterator* pBlockTableRecordIterator = NULL;
		if (_pBlockTableRecord->newIterator(pBlockTableRecordIterator) != Acad::eOk)
		{
			break;
		}


		//遍历记录内实体
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


		//删除遍历器对象
		delete pBlockTableRecordIterator;

	} while (false);


	return ListEntitys;
}



//获得指定名称的块表记录
AcDbObjectId ObjectArx::GetBlockTableRecord(CString _BlockName, AcDbDatabase* _pDatabase)
{
	AcDbObjectId BlockTableRecordId = NULL;


	do
	{
		//打开块表
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::OpenMode::kForRead);


		//获得用户指定的块表记录
		if (pBlockTable->getAt(_BlockName, BlockTableRecordId) != 0)
		{
			pBlockTable->close();
			break;
		}


		//关闭块表
		pBlockTable->close();


	} while (false);


	return BlockTableRecordId;
}


//获得模型空间的所有块名
void ObjectArx::GetModelSpaceBlockNames(CString _Path, CStringArray& _ListBlockNames)
{
	do
	{
		//读取DWG文件获得数据库
		AcDbDatabase* pDatabase = new AcDbDatabase(false);
#if OBJECTARX2010 || OBJECTARX2011 || OBJECTARX2012 || OBJECTARX2013 || OBJECTARX2014 || OBJECTARX2015 || OBJECTARX2016
		if (pDatabase->readDwgFile(_Path, _SH_DENYNO) != 0)
#elif OBJECTARX2017 || OBJECTARX2018 || OBJECTARX2019 || OBJECTARX2020
		if (pDatabase->readDwgFile(_Path) != 0)
#endif
		{
			break;
		}


		//导入图块
		GetModelSpaceBlockNames(_ListBlockNames, pDatabase);


		//释放内存
		delete pDatabase;

	} while (false);
}
void ObjectArx::GetModelSpaceBlockNames(CStringArray& _ListBlockNames, AcDbDatabase* _pDatabase)
{
	do
	{
		//打开块表
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::OpenMode::kForRead);


		//获得模型空间的块表记录
		AcDbObjectId BlockTableRecordId = NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE, BlockTableRecordId);


		//关闭块表
		pBlockTable->close();


		//遍历模型空间
		AcDbObjectIdArray ListEntitys = IterateBlockTableRecord(BlockTableRecordId, _pDatabase);


		//获得块名称
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


			//获得块名
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



//获得图纸中的所有块名
void ObjectArx::GetBlockNames(CString _Path, CStringArray& _ListBlockNames)
{
	do
	{
		//读取DWG文件获得数据库
		AcDbDatabase* pDatabase = new AcDbDatabase(false);
#if OBJECTARX2010 || OBJECTARX2011 || OBJECTARX2012 || OBJECTARX2013 || OBJECTARX2014 || OBJECTARX2015 || OBJECTARX2016
		if (pDatabase->readDwgFile(_Path, _SH_DENYNO) != 0)
#elif OBJECTARX2017 || OBJECTARX2018 || OBJECTARX2019 || OBJECTARX2020
		if (pDatabase->readDwgFile(_Path) != 0)
#endif
		{
			break;
		}


		//导入图块
		GetBlockNames(_ListBlockNames, pDatabase);


		//释放内存
		delete pDatabase;

	} while (false);
}
void ObjectArx::GetBlockNames(CStringArray& _ListBlockNames, AcDbDatabase* _pDatabase)
{
	do
	{
		//获得块表指针
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead);


		//创建块表遍历器
		AcDbBlockTableIterator* pBlockTableIterator = NULL;
		pBlockTable->newIterator(pBlockTableIterator);


		//获得所有块名称
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


		//删除遍历器关闭块表
		delete pBlockTableIterator;
		pBlockTable->close();

	} while (false);
}



//添加实体到块表记录
AcDbObjectId ObjectArx::AddEntityToBlockTableRecord(AcDbEntity* _pEntiy, AcDbObjectId _BlockTableRecordId, AcDbDatabase* _pDatabase)
{
	AcDbObjectId ObjectId = NULL;


	do
	{
		if (_pDatabase == NULL)
		{
			_pDatabase = acdbHostApplicationServices()->workingDatabase();
		}


		//获得数据库中的块表对象
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead);


		//获得模型空间的块表记录
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

		//将尸体添加到块表记录中
		pBlockTableRecord->appendAcDbEntity(ObjectId, _pEntiy);


		//关闭对象
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


		//获得数据库中的块表对象
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead);


		//获得模型空间的块表记录
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

		//将尸体添加到块表记录中
		AcDbObjectId ObjectId = NULL;
		AcDbEntity* pEntity = NULL, * pTempEntity = NULL;;
		for (int i = 0; i < _ListEntitys.length(); i++)
		{
			if (acdbOpenObject(pEntity, _ListEntitys[i], AcDb::OpenMode::kForWrite) == 0)
			{
				//复制对象
				pTempEntity = AcDbEntity::cast(pEntity->clone());

				pBlockTableRecord->appendAcDbEntity(ObjectId, pTempEntity);
				pTempEntity->close();
				ListEntitys.append(ObjectId);

				pEntity->close();
			}
		}


		//关闭对象
		pBlockTable->close();
		pBlockTableRecord->close();

	} while (false);


	return ListEntitys;
}


//删除块表记录
void ObjectArx::DeleteBlockTableRecord(CString _BlockName)
{
	do
	{
		//获得当前的图层列表  
		AcDbBlockTable* pBlockTable;
		acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);


		//判断是否包含指定名称的块表记录  
		if (pBlockTable->has(_BlockName) == 0)
		{
			pBlockTable->close();
		}


		//获得指定层表记录的指针  
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



//获得数据库所有图层
int ObjectArx::GetDatabaseLayers(CStringArray* _StrLayerArray, AcDbDatabase* _pDatabase)
{
	//获得层表指针
	AcDbLayerTable* pLayerTable = NULL;
	AcDbLayerTableRecord* pLayerTableRcd = NULL;
	_pDatabase->getLayerTable(pLayerTable, AcDb::kForRead);


	//使用遍历器访问每一条层表记录
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



//创建图层
int ObjectArx::CreateLayer(CString _Layer, int _Color)
{
	do
	{
		//判断是否存在名称为 StrLayerName 的图层
		AcDbLayerTable* pLayerTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
		if (pLayerTable->has(_Layer) == 1)
		{
			pLayerTable->close();
			break;
		}


		//新建层表记录
		AcDbLayerTableRecord* pLayerTableRecord = new AcDbLayerTableRecord();
		pLayerTableRecord->setName(_Layer);


		//设置图层颜色
		AcCmColor Color;
		Color.setColorIndex(_Color);
		pLayerTableRecord->setColor(Color);


		AcDbObjectId LayerTableRecordId = NULL;
		pLayerTable->add(LayerTableRecordId, pLayerTableRecord);


		//将新建的层表记录添加到层表中
		pLayerTableRecord->close();
		pLayerTable->close();

	} while (false);

	return 0;
}



//判断图层是否存在
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


//获得当前图层
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
		//打开层表,判断是否存在名称为 StrLayerName 的图层
		AcDbLayerTable* pLayerTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
		if (pLayerTable->has(_StrLayer) == 0)
		{
			pLayerTable->close();
			break;
		}


		// 获得指定层表记录的指针
		AcDbLayerTableRecord* pLayerTableRcd = NULL;
		if (pLayerTable->getAt(_StrLayer, pLayerTableRcd, AcDb::kForWrite) != 0)
		{
			pLayerTable->close();
			break;
		}


		//设置图层是否冻结
		IsReturn = pLayerTableRcd->isOff();


		//关闭对象
		pLayerTableRcd->close();
		pLayerTable->close();

	} while (false);


	return IsReturn;
}

int ObjectArx::SetLayerOff(CString _StrLayer, bool _IsOff) {

	int Return = -1;


	do
	{
		//打开层表,判断是否存在名称为 StrLayerName 的图层
		AcDbLayerTable* pLayerTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
		if (pLayerTable->has(_StrLayer) == 0)
		{
			pLayerTable->close();
			break;
		}


		// 获得指定层表记录的指针
		AcDbLayerTableRecord* pLayerTableRcd = NULL;
		if (pLayerTable->getAt(_StrLayer, pLayerTableRcd, AcDb::kForWrite) != 0)
		{
			pLayerTable->close();
			break;
		}


		//设置图层是否冻结
		pLayerTableRcd->setIsOff(_IsOff);


		//关闭对象
		pLayerTableRcd->close();
		pLayerTable->close();


		Return = 0;

	} while (false);


	return Return;
}


//判断图层是否锁定
bool ObjectArx::IsLayerLock(CString _StrLayer)
{
	//打开层表,判断是否存在名称为 StrLayerName 的图层
	AcDbLayerTable* pLayerTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
	if (pLayerTable->has(_StrLayer) == 0)
	{
		pLayerTable->close();
		return 0;
	}


	// 获得指定层表记录的指针
	AcDbLayerTableRecord* pLayerTableRcd = NULL;
	if (pLayerTable->getAt(_StrLayer, pLayerTableRcd, AcDb::kForWrite) != 0)
	{
		pLayerTable->close();
		return 0;
	}


	//设置图层是否冻结
	bool BolLock = pLayerTableRcd->isLocked();


	//关闭对象
	pLayerTableRcd->close();
	pLayerTable->close();

	return BolLock;
}


//设置图层是否锁定
int ObjectArx::SetLayerLock(CString _StrLayer, bool _BolLock)
{
	//打开层表,判断是否存在名称为 StrLayerName 的图层
	AcDbLayerTable* pLayerTbl;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (pLayerTbl->has(_StrLayer) == 0)
	{
		pLayerTbl->close();
		return 0;
	}


	//获得指定层表记录的指针
	AcDbLayerTableRecord* pLayerTblRcd;
	if (pLayerTbl->getAt(_StrLayer, pLayerTblRcd, AcDb::kForWrite) != 0)
	{
		pLayerTbl->close();
		return 0;
	}


	//设置图层是否冻结
	pLayerTblRcd->setIsLocked(_BolLock);


	//关闭对象
	pLayerTblRcd->close();
	pLayerTbl->close();
	return 0;
}



//判断图层是否隐藏
bool ObjectArx::IsLayerFrozen(CString _StrLayer)
{
	//打开层表,判断是否存在名称为 StrLayerName 的图层
	AcDbLayerTable* pLayerTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
	if (pLayerTable->has(_StrLayer) == 0)
	{
		pLayerTable->close();
		return 0;
	}


	// 获得指定层表记录的指针
	AcDbLayerTableRecord* pLayerTableRcd = NULL;
	if (pLayerTable->getAt(_StrLayer, pLayerTableRcd, AcDb::kForWrite) != 0)
	{
		pLayerTable->close();
		return 0;
	}


	//设置图层是否隐藏
	bool BolHide = pLayerTableRcd->isFrozen();


	//关闭对象
	pLayerTableRcd->close();
	pLayerTable->close();
	return BolHide;
}


//设置图层是否隐藏
int ObjectArx::SetLayerFrozen(CString _StrLayer, bool _BolHide)
{
	//打开层表,判断是否存在名称为 StrLayerName 的图层
	AcDbLayerTable* pLayerTbl;
	acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTbl, AcDb::kForWrite);
	if (pLayerTbl->has(_StrLayer) == 0)
	{
		pLayerTbl->close();
		return 0;
	}


	//获得指定层表记录的指针
	AcDbLayerTableRecord* pLayerTblRcd;
	if (pLayerTbl->getAt(_StrLayer, pLayerTblRcd, AcDb::kForWrite) != 0)
	{
		pLayerTbl->close();
		return 0;
	}


	//设置图层是否冻结
	pLayerTblRcd->setIsFrozen(_BolHide);


	//关闭对象
	pLayerTblRcd->close();
	pLayerTbl->close();
	return 0;
}



//获得图层颜色
int ObjectArx::GetLayerColor(CString _LayerName)
{
	int Color = 0;


	do
	{
		//获得当前图形的层表
		AcDbLayerTable* pLayerTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForRead);


		//判断是否包含指定名称的层表记录
		if (pLayerTable->has(_LayerName) == false)
		{
			pLayerTable->close();
			break;
		}


		// 获得指定层表记录的指针
		AcDbLayerTableRecord* pLayerTableRecord = NULL;
		pLayerTable->getAt(_LayerName, pLayerTableRecord, AcDb::kForRead);


		//获得图层颜色 
		AcCmColor CmColor = pLayerTableRecord->color();
		Color = CmColor.colorIndex();


		//关闭对象
		pLayerTableRecord->close();
		pLayerTable->close();


	} while (false);


	return Color;
}



//设置图层颜色
int ObjectArx::SetLayerColor(CString _LayerName, int _Color)
{
	int Err = 0;


	do
	{
		//打开层表,判断是否存在名称为 StrLayerName 的图层
		AcDbLayerTable* pLayerTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForRead);
		if (pLayerTable->has(_LayerName) == false)
		{
			pLayerTable->close();
			Err = -1;
			break;
		}


		//获得指定层表记录的指针
		AcDbLayerTableRecord* pLayerTableRecord = NULL;
		pLayerTable->getAt(_LayerName, pLayerTableRecord, AcDb::kForWrite);


		//设置图层颜色
		AcCmColor color;
		color.setColorIndex(_Color);
		pLayerTableRecord->setColor(color);


		//关闭对象
		pLayerTableRecord->close();
		pLayerTable->close();

	} while (false);


	return Err;
}


//设置当前图层
int ObjectArx::SetCurrentLayer(CString _LayerName)
{
	int Err = 0;


	do
	{
		//获取当前图形层表
		AcDbLayerTable* pLayerTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getLayerTable(pLayerTable, AcDb::kForWrite);
		if (pLayerTable->has(_LayerName) == false)
		{
			pLayerTable->close();
			Err = -1;
			break;
		}


		//设置图层
		AcDbObjectId LayerId = NULL;
		pLayerTable->getAt(_LayerName, LayerId);
		acdbHostApplicationServices()->workingDatabase()->setClayer(LayerId);
		pLayerTable->close();

	} while (false);


	return Err;
}



//修改块名称
int ObjectArx::SetBlockName(CString _StrOldName, CString _StrNewName, AcDbDatabase* _pDatabase)
{
	do
	{
		//获得当前图形的块表
		AcDbBlockTable* pBlockTable = NULL;
		_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead);


		//判断是否包含指定名称的块表记录
		if (!pBlockTable->has(_StrOldName))
		{
			pBlockTable->close();
			break;
		}


		//获得指定块表记录的指针
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		pBlockTable->getAt(_StrOldName, pBlockTableRecord, AcDb::kForWrite);


		//修改块名
		pBlockTableRecord->setName(_StrNewName);


		//关闭对象
		pBlockTableRecord->close();
		pBlockTable->close();


	} while (false);

	return 0;
}


//获得图块的基点
AcGePoint3d ObjectArx::GetBlockPoint(AcDbObjectId _BlockId)
{
	AcGePoint3d PointPos(0.0, 0.0, 0.0);


	do
	{
		//打开块表记录获得指针
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _BlockId, AcDb::kForRead) != 0)
		{
			break;
		}


		//获得点
		PointPos = pBlockReference->position();


		//释放内存
		pBlockReference->close();

	} while (false);


	return PointPos;
}



//设置图块的基点
void ObjectArx::SetBlockPoint(AcDbObjectId _BlockId, AcGePoint3d _Point)
{
	do
	{
		//打开块表记录获得指针
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _BlockId, AcDb::kForWrite) != 0)
		{
			break;
		}


		//获得点
		pBlockReference->setPosition(_Point);


		//释放内存
		pBlockReference->close();

	} while (false);

}



//获得图块的角度
double ObjectArx::GetBlockAngle(AcDbObjectId _BlockId)
{
	double Angle = 0.0;


	do
	{
		//打开块表记录获得指针
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _BlockId, AcDb::kForRead) != 0)
		{
			break;
		}


		//获得点
		Angle = pBlockReference->rotation();
		Angle = 180.0 / 3.14159265358 * Angle;


		//释放内存
		pBlockReference->close();

	} while (false);


	return Angle;
}



//设置图块的角度
void ObjectArx::SetBlockAngle(AcDbObjectId _BlockId, double _Angle)
{
	do
	{
		//打开块表记录获得指针
		AcDbBlockReference* pBlockReference = NULL;
		if (acdbOpenObject(pBlockReference, _BlockId, AcDb::kForWrite) != 0)
		{
			break;
		}


		//获得点
		pBlockReference->setRotation(_Angle * 3.14159265358 / 180.0);


		//释放内存
		pBlockReference->close();

	} while (false);
}



/*
_ObjectId:对象
*/
//判断一个对象是否是动态块
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
_ObjectId:块对象
*/
//判断一个对象是否是属性块
bool ObjectArx::IsAttrBolck(AcDbObjectId _ObjectId)
{
	//打开块表记录获得指针
	AcDbBlockReference* pBlockRef = NULL;
	if (acdbOpenObject(pBlockRef, _ObjectId, AcDb::kForRead) != 0)
	{
		return 0;
	}


	//获得块表记录
	AcDbBlockTableRecord* pBlockDefRcd = NULL;
	if (acdbOpenObject(pBlockDefRcd, pBlockRef->blockTableRecord(), AcDb::kForRead) != 0)
	{
		pBlockRef->close();
		return 0;
	}


	//判断是否是属性块
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


//设置当前布局
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


//获得当前布局
CString ObjectArx::GetCurrentLayout()
{
	return acdbHostApplicationServices()->layoutManager()->findActiveLayout(1);
}


//创建布局
void ObjectArx::CreateLayout(CString _StrLayoutName, AcDbDatabase* _pDatabase)
{
	do
	{
		//获得布局表
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//判断数据库中是否存在该布局
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


		//获得数据库的块表
		AcDbBlockTable* pBlockTable = NULL;
		acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForWrite);


		//新建块表记录
		AcDbObjectId BlockTableRecordId = NULL;
		AcDbBlockTableRecord* pBlockTableRecord = new AcDbBlockTableRecord();
		pBlockTableRecord->setName(_T("*Paper_Space"));
		pBlockTable->add(BlockTableRecordId, pBlockTableRecord);
		pBlockTableRecord->close();


		//关闭块表
		pBlockTable->close();


		//新建块表指针
		AcDbLayout* pLayout = new AcDbLayout();
		if (pLayout == NULL)
		{
			break;
		}


		//显示打印样式
		pLayout->setShowPlotStyles(true);
		pLayout->setPlotPlotStyles(true);

		//pLayout->setPlotSettingsName(_T("monochrome.ctb"));



		//设置布局名称
		if (pLayout->setLayoutName(_StrLayoutName) != 0)
		{
			pLayout->close();
			break;
		}


		//添加块表记录到当前数据库
		pLayout->addToLayoutDict(acdbHostApplicationServices()->workingDatabase(), BlockTableRecordId);


		//关闭布局
		pLayout->close();

	} while (false);
}



//获得所有布局名称
void ObjectArx::GetDatabaseLayouts(CStringArray* pListLayoutNames, AcDbDatabase* _pDatabase)
{
	do
	{
		//获得布局表
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//获得所有布局名称
		AcDbDictionaryIterator* pDictionaryIterator = pDictionary->newIterator();
		for (; !pDictionaryIterator->done(); pDictionaryIterator->next())
		{
			pListLayoutNames->Add(pDictionaryIterator->name());
		}
		delete pDictionaryIterator;
		pDictionary->close();

	} while (false);
}



//获得布局的块名
CString ObjectArx::GetLayoutBlockName(CString _StrLayoutName, AcDbDatabase* _pDatabase)
{
	CString StrBlockName = _T("");


	do
	{
		//获得布局表
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//获得布局Id
		AcDbObjectId LayoutId = NULL;
		pDictionary->getAt(_StrLayoutName, LayoutId);
		pDictionary->close();
		if (LayoutId == NULL)
		{
			break;
		}


		//获得块表记录Id
		AcDbObjectId BlockTableRecordId = NULL;
		AcDbLayout* pLayout = NULL;
		if (acdbOpenObject(pLayout, LayoutId, AcDb::kForWrite) == 0)
		{
			BlockTableRecordId = pLayout->getBlockTableRecordId();
			pLayout->close();
		}


		//获得块名
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



//获得布局的块表记录
AcDbObjectId ObjectArx::GetLayoutBlockTableRecord(CString _StrLayoutName, AcDbDatabase* _pDatabase)
{
	AcDbObjectId BlockTableRecordId = NULL;


	do
	{
		//获得布局表
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//获得布局Id
		AcDbObjectId LayoutId = NULL;
		pDictionary->getAt(_StrLayoutName, LayoutId);
		pDictionary->close();
		if (LayoutId == NULL)
		{
			break;
		}


		//获得块表记录Id
		AcDbLayout* pLayout = NULL;
		if (acdbOpenObject(pLayout, LayoutId, AcDb::kForWrite) == 0)
		{
			BlockTableRecordId = pLayout->getBlockTableRecordId();
			pLayout->close();
		}

	} while (false);


	return BlockTableRecordId;
}



//获得视口冻结图层
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


//设置视口冻结图层
void ObjectArx::SetViewportFrozenLayer(AcDbViewport* _pViewport, CStringArray& ListLayoutNames, AcDbDatabase* _pDatabase)
{
	do
	{
		//获得层表
		AcDbLayerTable* pLayerTable = NULL;
		_pDatabase->getLayerTable(pLayerTable, AcDb::OpenMode::kForRead);


		//获得块表记录Id
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


		//关闭块表记录
		pLayerTable->close();


		//设置视口冻结图层
		_pViewport->thawAllLayersInViewport();
		_pViewport->freezeLayersInViewport(ListFrozenLayers);
		_pViewport->setOn();

	} while (false);
}
void ObjectArx::SetViewportFrozenLayer(AcDbObjectId _ViewPortId, CStringArray& ListLayoutNames, AcDbDatabase* _pDatabase)
{
	do
	{
		//设置视口冻结图层
		AcDbViewport* pViewport = NULL;
		if (acdbOpenObject(pViewport, _ViewPortId, AcDb::kForWrite) != 0)
		{
			break;
		}
		SetViewportFrozenLayer(pViewport, ListLayoutNames, _pDatabase);


		pViewport->close();

	} while (false);
}



//设置视口锁定
void ObjectArx::SetViewportLocked(AcDbObjectId _ViewPortId, bool _IsLocked)
{
	AcDbViewport* pViewport = NULL;
	if (acdbOpenObject(pViewport, _ViewPortId, AcDb::kForWrite) == 0)
	{
		pViewport->setLocked(_IsLocked);
		pViewport->close();
	}
}


//删除布局
int ObjectArx::DeleteLayout(CString _StrName)
{
	int Err = 0;


	do
	{
		Err = acdbHostApplicationServices()->layoutManager()->deleteLayout(_StrName);
	} while (false);


	return Err;
}


//获得布局中打印pc3文件
CString ObjectArx::GetPlotCfgName(CString _LayoutName, AcDbDatabase* _pDatabase)
{
	CString PlotCfgName;


	do
	{
		//获得布局表
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//获得布局Id
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


//获得布局中是否显示打印样式
bool ObjectArx::GetShowPlotStyles(CString _LayoutName, AcDbDatabase* _pDatabase)
{
	bool IsShowPlotStyles = false;

	do
	{
		//获得布局表
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//获得布局Id
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

//设置布局中是否显示打印样式
void ObjectArx::SetShowPlotStyles(CString _LayoutName, bool _IsShowPlotStyles, AcDbDatabase* _pDatabase)
{
	do
	{
		//获得布局表
		AcDbDictionary* pDictionary = NULL;
		_pDatabase->getLayoutDictionary(pDictionary, AcDb::kForRead);


		//获得布局Id
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
	struct resbuf wcs, ucs, dcs; // 转换坐标时使用的坐标系统标记
	wcs.restype = RTSHORT;
	wcs.resval.rint = 0;
	ucs.restype = RTSHORT;
	ucs.resval.rint = 1;
	dcs.restype = RTSHORT;
	dcs.resval.rint = 2;

	// 获得当前视口的"查看"模式
	acedGetVar(_T("VIEWMODE"), &rb);
	view.setPerspectiveEnabled(rb.resval.rint & 1);
	view.setFrontClipEnabled(rb.resval.rint & 1);
	view.setBackClipEnabled(1);
	view.setFrontClipAtEye(1);


	// 当前视口中视图的中心点（UCS坐标）
	acedGetVar(_T("VIEWCTR"), &rb);
	acedTrans(rb.resval.rpoint, &ucs, &dcs, 0, rb.resval.rpoint);
	view.setCenterPoint(AcGePoint2d(rb.resval.rpoint[X],
		rb.resval.rpoint[Y]));


	// 当前视口透视图中的镜头焦距长度（单位为毫米）
	acedGetVar(_T("LENSLENGTH"), &rb);
	view.setLensLength(rb.resval.rreal);


	// 当前视口中目标点的位置（以UCS 坐标表示）
	acedGetVar(_T("TARGET"), &rb);
	acedTrans(rb.resval.rpoint, &ucs, &wcs, 0, rb.resval.rpoint);
	view.setTarget(AcGePoint3d(rb.resval.rpoint[X],
		rb.resval.rpoint[Y], rb.resval.rpoint[Z]));


	// 当前视口的观察方向（UCS）
	acedGetVar(_T("VIEWDIR"), &rb);
	acedTrans(rb.resval.rpoint, &ucs, &wcs, 1, rb.resval.rpoint);
	view.setViewDirection(AcGeVector3d(rb.resval.rpoint[X],
		rb.resval.rpoint[Y], rb.resval.rpoint[Z]));


	// 当前视口的视图高度（图形单位）
	acedGetVar(_T("VIEWSIZE"), &rb);
	view.setHeight(rb.resval.rreal);
	double height = rb.resval.rreal;


	// 以像素为单位的当前视口的大小（X 和Y 值）
	acedGetVar(_T("SCREENSIZE"), &rb);
	view.setWidth(rb.resval.rpoint[X] / rb.resval.rpoint[Y] * height);


	// 当前视口的视图扭转角
	acedGetVar(_T("VIEWTWIST"), &rb);
	view.setViewTwist(rb.resval.rreal);


	// 将模型选项卡或最后一个布局选项卡置为当前
	acedGetVar(_T("TILEMODE"), &rb);
	int tileMode = rb.resval.rint;


	// 设置当前视口的标识码
	acedGetVar(_T("CVPORT"), &rb);
	int cvport = rb.resval.rint;


	// 是否是模型空间的视图
	bool paperspace = ((tileMode == 0) && (cvport == 1)) ? true : false;
	view.setIsPaperspaceView(paperspace);
	if (!paperspace)
	{
		// 当前视口中前向剪裁平面到目标平面的偏移量
		acedGetVar(_T("FRONTZ"), &rb);
		view.setFrontClipDistance(rb.resval.rreal);


		// 获得当前视口后向剪裁平面到目标平面的偏移值
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


//判断文字样式是否存在
bool ObjectArx::IsTextStyle(CString _StrStyle)
{
	bool Err = 1;


	//获得标注样式Id
	AcDbTextStyleTable* pTextStyleTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyleTable, AcDb::kForWrite);
	if (pTextStyleTable->has(_StrStyle) == 0)
	{
		Err = 0;
	}


	pTextStyleTable->close();
	return Err;
}


//获得文字内容
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

//获得文字内容
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

//判断标注样式是否存在
bool ObjectArx::IsDimensionStyle(CString _StrStyle)
{
	bool Err = 1;


	//获得标注样式Id
	AcDbTextStyleTable* pTextStyleTable = NULL;
	acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyleTable, AcDb::kForWrite);
	if (pTextStyleTable->has(_StrStyle) == 0)
	{
		Err = 0;
	}


	pTextStyleTable->close();
	return Err;
}


//通过引线样式获得样式Id
AcDbObjectId ObjectArx::GetDimensionStyleId(CString _StrStyle)
{
	int Err = -1;


	//获得标注样式Id
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



//通过引线样式Id获得样式名
CString ObjectArx::GetDimensionStyleName(AcDbObjectId _StyleId)
{
	//打开对象
	AcDbEntity* pEnt = NULL;
	if (acdbOpenAcDbEntity(pEnt, _StyleId, AcDb::kForWrite) != 0)
	{
		return _T("");
	}



	//获得样式指针
	AcDbDimStyleTableRecord* pDimStyleTableRecord = AcDbDimStyleTableRecord::cast(pEnt);
	if (pDimStyleTableRecord == NULL)
	{
		pEnt->close();
		return _T("");
	}


	//获得样式名称
	ACHAR* pName = NULL;
	CString StrName = _T("");
	pDimStyleTableRecord->getName(pName);
	StrName = pName;


	pDimStyleTableRecord->close();
	return StrName;
}



//通过文字样式获得样式Id
AcDbObjectId ObjectArx::GetTextStyleId(CString _StrStyle)
{
	AcDbObjectId StyleId = NULL;


	do
	{
		//获得标注样式Id
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



//获得文字样式
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



//通过文字样式Id获得样式名
CString ObjectArx::GetTextStyleName(AcDbObjectId _StyleId)
{
	CString StrName = _T("");


	do
	{
		//打开对象
		AcDbObject* pObject = NULL;
		if (acdbOpenObject(pObject, _StyleId, AcDb::OpenMode::kForRead) != 0)
		{
			break;
		}

		//获得文字样式指针
		AcDbTextStyleTableRecord* pTextStyleTableRecord = AcDbTextStyleTableRecord::cast(pObject);
		if (pTextStyleTableRecord == NULL)
		{
			pObject->close();
			break;
		}
		pObject->close();


		//获得样式名称
		ACHAR* pName = NULL;
		pTextStyleTableRecord->getName(pName);
		StrName = pName;


		pTextStyleTableRecord->close();


	} while (false);

	return StrName;
}


//获得文字尺寸
AcGePoint3d ObjectArx::GetTextSize(CString _StrText, CString _StrFont, double _Size)
{
	AcGiTextStyle AsdkStyle;
	AsdkStyle.setTextSize(_Size);
	AsdkStyle.setStyleName(_StrFont);
	AcGePoint2d PointText = AsdkStyle.extents(_StrText, Adesk::kFalse, _tcslen(_StrText), 1);
	return AcGePoint3d(PointText.x, PointText.y, 0.0);
}


//获得文字尺寸
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

//获得文字宽度
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

//获得文字基点
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



//设置引线起始点
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



//获得引线点
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



//设置多重引线是否启动基线
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



//设置标注箭头尺寸
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



//设置尺寸文字高度
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



//指定尺寸界线超出尺寸线的距离
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



//设置标注样式
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
_StrName:样式名称
_Color:颜色
_StrFont:字体
_StrBigFont:大字体
_FontHeight:文字高度
_XScale:宽高比
_ValueObliquingAngle:倾斜角度
_BolVertical:是否竖向
*/
//创建文字样式
int ObjectArx::CreateTextStyle(CString _StrName, int _Color, CString _StrFont, CString _StrBigFont, double _FontHeight, double _XScale,
	double _ValueObliquingAngle, bool _BolVertical)
{
	//获得字体样式表
	AcDbTextStyleTable* pTextStyleTbl;
	acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyleTbl, AcDb::kForWrite);



	//判断是否存在该样式
	if (pTextStyleTbl->has(_StrName) == 1)
	{
		pTextStyleTbl->close();
		return -1;
	}



	//创建新的字体样式表记录
	AcDbTextStyleTableRecord* pTextStyleTblRcd;
	pTextStyleTblRcd = new AcDbTextStyleTableRecord();


	//设置字体样式表记录的名称
	pTextStyleTblRcd->setName(_StrName);

	//设置字体文件名称
	pTextStyleTblRcd->setFileName(_StrFont);

	//设置字体样式
	pTextStyleTblRcd->setBigFontFileName(_StrBigFont);

	//设置字体高度
	pTextStyleTblRcd->setTextSize(_FontHeight);

	//设置高宽比例
	pTextStyleTblRcd->setXScale(_XScale);

	//设置倾斜弧度
	double ValueArc = 3.14159265358 / 180 * _ValueObliquingAngle;
	pTextStyleTblRcd->setObliquingAngle(ValueArc);

	//设置竖立
	pTextStyleTblRcd->setIsVertical(_BolVertical);

	//将新的记录添加到字体样式表
	pTextStyleTbl->add(pTextStyleTblRcd);
	pTextStyleTblRcd->close();
	pTextStyleTbl->close();
	return 0;

}



/*
_StrName:文字样式名
_StrTextStyle:文字样式
_HeightText:标注文字高度
_ColorDimText:标注文字颜色
_VarDimMove:标注文字的移动规则
0:尺寸线和标注文字一起移动;
1:在移动标注文字时添加一条引线;
2:允许标注文字自由移动而不用添加引线
__StrTypeArrow:箭头形状
"":实心闭合
"_DOT":点
"_DOTSMALL":小点
"_DOTBLANK":空心点
"_ORIGIN":原点标记
"_ORIGIN2":原点标记 2
"_OPEN":打开
"_OPEN90":直角
"_OPEN30":30 度角
"_CLOSED":闭合
"_SMALL":空心小点
"_NONE":无
"_OBLIQUE":倾斜
"_BOXFILLED":实心框
"_BOXBLANK":框
"_CLOSEDBLANK":空心闭合
"_DATUMFILLED":实心基准三角形
"_DATUMBLANK":基准三角形
"_INTEGRAL":完整标记
"_ARCHTICK":建筑标记
_VarArrow:箭头尺寸(推荐为_HeightText / 2.0)
_BolTextInExtlineUp:设置标注文字始终绘制在尺寸界线之间
_VarTextToDimline:文字在标注线上方的距离(推荐为_HeightText / 10.0)
_VarExtlineOffOriginDist:尺寸界线偏移原点的距离
_VarDimJust:控制标注文字的水平位置
0:将文字置于尺寸线之上,并在尺寸界线之间置中对正;
1:紧邻第一条尺寸界线放置标注文字;
2:紧邻第二条尺寸界线放置标注文字;
3:将标注文字放在第一条尺寸界线以上,并与之对齐;
4:将标注文字放在第二条尺寸界线以上,并与之对齐)
_VarExcDimline:指定尺寸界线超出尺寸线的距离(推荐为_HeightText / 2.0)
_VarScale:比例因子
_ColorDimline:尺寸线颜色(中间的尺寸线)
_ColorExtline:尺寸界线颜色(两边的尺寸线)
_VarDimdec:标注主单位显示的小数位位数,0为随图层
_VarTextOffDimline:文字从尺寸线偏移,当尺寸线分成段以在两段之间放置标注文字时,设置标注文字周围的距离
*/
//创建引线样式
int ObjectArx::CreateDimensionStyle(CString _StrName, CString _StrTextStyle, double _HeightText, int _ColorDimText, int _VarDimMove,
	CString _StrTypeArrow, double _VarArrow, bool _BolTextInExtlineUp, double _VarTextToDimline, double _VarExtlineOffOriginDist,
	int _VarDimJust, double _VarExcDimline, double  _VarScale, int _ColorDimline, int _ColorExtline, int _VarDimdec,
	double _VarTextOffDimline)
{
	//获得当前图形的标注样式表  
	AcDbDimStyleTable* pDimStyleTable;
	acdbHostApplicationServices()->workingDatabase()->getDimStyleTable(pDimStyleTable, AcDb::kForWrite);


	//是否已包含  
	if (pDimStyleTable->has(_StrName))
	{
		pDimStyleTable->close();
		return -1;
	}


	//创建新的标注样式表  
	AcDbDimStyleTableRecord* pDimStyleTableRcd;
	pDimStyleTableRcd = new AcDbDimStyleTableRecord();



	//获得文字样式Id
	AcDbTextStyleTable* pTextStyle = NULL;
	acdbHostApplicationServices()->workingDatabase()->getTextStyleTable(pTextStyle, AcDb::kForRead);
	AcDbObjectId TextStyleId = NULL;
	pTextStyle->getAt(_StrTextStyle, TextStyleId);
	pTextStyle->close();



	AcCmColor Color;



	//设置标注样式的特性  
	pDimStyleTableRcd->setName(_StrName);										//标注样式名称

	pDimStyleTableRcd->setDimasz(_VarArrow);									//箭头长度

	pDimStyleTableRcd->setDimblk(_StrTypeArrow);								//设置箭头的形状

	pDimStyleTableRcd->setDimexe(_VarExcDimline);								//指定尺寸界线超出尺寸线的距离

	pDimStyleTableRcd->setDimlfac(_VarScale);									//比例因子

	Color.setColorIndex(_ColorDimline);
	pDimStyleTableRcd->setDimclrd(Color);										//为尺寸线,箭头和标注引线指定颜色

	Color.setColorIndex(_ColorExtline);
	pDimStyleTableRcd->setDimclre(Color);										//为尺寸界线指定颜色

	Color.setColorIndex(_ColorDimText);
	pDimStyleTableRcd->setDimclrt(Color);										//为标注文字指定颜色

	pDimStyleTableRcd->setDimdec(_VarDimdec);									//设置标注主单位显示的小数位位数,0为随图层

	pDimStyleTableRcd->setDimexo(_VarExtlineOffOriginDist);						//指定尺寸界线偏移原点的距离

	pDimStyleTableRcd->setDimgap(_VarTextOffDimline);							//文字从尺寸线偏移,当尺寸线分成段以在两段之间放置标注文字时,设置标注文字周围的距离

	pDimStyleTableRcd->setDimjust(_VarDimJust);									//控制标注文字的水平位置

	pDimStyleTableRcd->setDimtix(_BolTextInExtlineUp);							//设置标注文字始终绘制在尺寸界线之间

	pDimStyleTableRcd->setDimtmove(_VarDimMove);								//设置标注文字的移动规则

	pDimStyleTableRcd->setDimtxsty(TextStyleId);								//指定标注的文字样式

	pDimStyleTableRcd->setDimtxt(_HeightText);									//指定标注文字的高度,除非当前文字样式具有固定的高度

	pDimStyleTableRcd->setDimtad(_VarTextToDimline);							//文字位于标注线的上方


	//将标注样式表记录添加到标注样式表中  
	pDimStyleTable->add(pDimStyleTableRcd);


	pDimStyleTableRcd->close();
	pDimStyleTable->close();
	return 0;
}



//创建对齐标注
AcDbObjectId ObjectArx::CreateAligDimension(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint, AcGePoint3d _PointPos, CString _StrStyle, CString _StrText)
{
	AcDbAlignedDimension* pDimension = new AcDbAlignedDimension(_StartPoint, _EndPoint, _PointPos, _StrText, GetDimensionStyleId(_StrStyle));
	return AddObjectToModl(pDimension);
}



//创建线性标注
AcDbObjectId ObjectArx::CreateLinearDimension(AcGePoint3d _StartPoint, AcGePoint3d _EndPoint, AcGePoint3d _PointPos, double _Angle, CString _StrStyle, CString _StrText)
{
	AcDbRotatedDimension* pDimension = new AcDbRotatedDimension(3.14159265358 / 180.0 * _Angle, _StartPoint, _EndPoint, _PointPos, _StrText, GetDimensionStyleId(_StrStyle));
	return AddObjectToModl(pDimension);
}


/*-------------------------------------------------------------------------------------------------------------------------------------------*/



















//分析
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/




/*
_ObjectId:求交对象
_ListObjects:要对比的对象
_Mode:求交模式
*/
//获得相交对象
AcDbObjectIdArray ObjectArx::GetInterObject(AcDbObjectId _ObjectId, AcDbObjectIdArray _ListObjects, AcDb::Intersect _Mode)
{
	AcDbEntity* pEnt = NULL, * pEntTemp = NULL;



	//打开第一个对象
	if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
	{
		return -1;
	}



	AcGePoint3dArray ListTempPoints = NULL;
	AcDbObjectIdArray ListInterObjects = NULL;
	for (int i = 0; i < _ListObjects.length(); i++)
	{
		//排除相同对象
		if (_ListObjects[i] == _ObjectId)
		{
			continue;
		}


		//打开对象
		pEntTemp = NULL;
		if (acdbOpenAcDbEntity(pEntTemp, _ListObjects[i], AcDb::kForRead) != 0)
		{
			continue;
		}


		//求交点
		ListTempPoints.removeAll();
		pEnt->intersectWith(pEntTemp, _Mode, ListTempPoints);
		if (ListTempPoints.length() > 0)
		{
			ListInterObjects.append(_ListObjects[i]);
		}


		//关闭对象
		pEntTemp->close();
	}


	pEnt->close();
	return ListInterObjects;
}




/*
_ObjectId:求交对象
_ListObjects:要求交的对象
_Mode:求交模式
*/
//获得相交点
AcGePoint3dArray ObjectArx::GetInterPoint(AcDbObjectId _ObjectId, AcDbObjectIdArray _ListObjects, AcDb::Intersect _Mode)
{
	AcDbEntity* pEnt = NULL, * pEntTemp = NULL;



	//打开第一个对象
	if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
	{
		return -1;
	}



	AcGePoint3dArray ListTempPoints = NULL;
	AcGePoint3dArray ListInterPoints = NULL;
	for (int i = 0; i < _ListObjects.length(); i++)
	{
		//排除相同对象
		if (_ListObjects[i] == _ObjectId)
		{
			continue;
		}


		//打开对象
		pEntTemp = NULL;
		if (acdbOpenAcDbEntity(pEntTemp, _ListObjects[i], AcDb::kForRead) != 0)
		{
			continue;
		}


		//求交点
		ListTempPoints.removeAll();
		pEnt->intersectWith(pEntTemp, _Mode, ListTempPoints);
		ListInterPoints.append(ListTempPoints);


		//关闭对象
		pEntTemp->close();
	}


	pEnt->close();
	return ListInterPoints;
}



//获得相交点
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



	////打开第一个对象
	//if (acdbOpenAcDbEntity(pEnt1, _ObjectAId, AcDb::kForRead) != 0)
	//{
	//	return NULL;
	//}


	////打开第二个对象
	//if (acdbOpenAcDbEntity(pEnt2, _ObjectBId, AcDb::kForRead) != 0)
	//{
	//	pEnt1->close();
	//	return NULL;
	//}



	////获得交点
	//AcGePoint3dArray *pListInterPoints = new AcGePoint3dArray();
	//pEnt1->intersectWith(pEnt2, _Mode, *pListInterPoints);



	//pEnt1->close();
	//pEnt2->close();
	//return *pListInterPoints;
}



/*
_ObjectId:要获得轮廓的对象
_ListObjects::要获得轮廓的对象
_MinPoint:返回的最小点
_MaxPoint:返回的最大点
*/
//获得对象轮廓
int ObjectArx::GetEntityExtents(AcDbObjectId _ObjectId, AcGePoint3d* _MinPoint, AcGePoint3d* _MaxPoint)
{
	int Error = 0;


	do
	{
		//打开实体
		AcDbEntity* pEnt = NULL;
		if (acdbOpenAcDbEntity(pEnt, _ObjectId, AcDb::kForRead) != 0)
		{
			Error = -2;
			break;
		}


		//获取轮廓
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


	//循环获取对象轮廓
	for (int i = 0; i < _ListObjects.length(); i++)
	{
		ObjectId = _ListObjects.at(i);

		//打卡对象
		if (acdbOpenAcDbEntity(pEnt, ObjectId, AcDb::kForRead) != 0)
		{
			continue;
		}

		//获取轮廓
		if (Acad::eInvalidExtents != pEnt->getGeomExtents(Extents))
		{
			MinPoints[NumPoint] = Extents.minPoint();
			MaxPoints[NumPoint] = Extents.maxPoint();
			NumPoint++;
			pEnt->close();
		}
	}

	//比较获得其中的最大和最小轮廓
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
_PointPos:点对象
_CurveId:曲线对象
*/
//获得点到曲线的最小距离
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



//获得点到曲线的最近点
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


//获得两个对象的交点
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
_PointPos:点对象
_CurveId:曲线对象
_Tolerance:公差
*/
//判断点是否在曲线上
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


//添加实体到当前布局
AcDbObjectId ObjectArx::AddObjectToCurSpace(AcDbEntity* _pEnt, AcDbDatabase* _pDatabase)
{
	AcDbObjectId ObjectId = NULL;

	do
	{
		//获得数据库中的块表对象
		AcDbBlockTable* pBlockTable = NULL;
		if (_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead) != NULL)
		{
			break;
		}

		//获得当前布局
		CString LayoutName = ObjectArx::GetCurrentLayout();

		//获得模型空间的块表记录
		AcDbBlockTableRecord* pBlockTableRecord;
		if (pBlockTable->getAt(LayoutName, pBlockTableRecord, AcDb::kForWrite) != NULL)
		{
			pBlockTable->close();
			break;
		}

		//将对象添加到块表记录中
		pBlockTableRecord->appendAcDbEntity(ObjectId, _pEnt);

		//关闭对象
		pBlockTable->close();
		pBlockTableRecord->close();
		_pEnt->close();

	} while (false);

	return ObjectId;
}


/*
pEnt:要添加的指针对象
_pEnts:指针数组
_pDatabase:要添加的数据库对象
*/
//添加实体到模型空间
AcDbObjectId ObjectArx::AddObjectToModl(AcDbEntity* _pEnt, AcDbDatabase* _pDatabase)
{
	AcDbObjectId ObjectId = NULL;


	//获得数据库中的块表对象
	AcDbBlockTable* pBlockTable = NULL;
	_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead);


	//获得模型空间的块表记录
	AcDbBlockTableRecord* pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);

	//将尸体添加到块表记录中
	pBlockTableRecord->appendAcDbEntity(ObjectId, _pEnt);


	//关闭对象
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



//添加块到模型空间
AcDbObjectId ObjectArx::AddBlockToModl(CString _StrBlockName, AcGePoint3d _PointDatum)
{
	//获得当前数据库的块表
	AcDbBlockTable* pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForWrite);


	//查找用户指定的块定义是否存在
	if (pBlockTable->has(_StrBlockName) == 0)
	{
		pBlockTable->close();
		return NULL;
	}


	//获得用户指定的块表记录
	AcDbObjectId BlockDefId;
	if (pBlockTable->getAt(_StrBlockName, BlockDefId) != 0)
	{
		pBlockTable->close();
		return NULL;
	}


	//创建块参照对象
	AcDbBlockReference* pBlockRef = new AcDbBlockReference(_PointDatum, BlockDefId);


	//将块参照添加到模型空间
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


	//关闭数据库的对象
	pBlockRef->close();
	pBlockTableRcd->close();
	pBlockTable->close();


	return ObjectId;
}


//添加块到模型空间
AcDbObjectId ObjectArx::AddBlockToModl(AcDbObjectId _BlockTableRecordId, AcGePoint3d _PointDatum)
{
	AcDbObjectId EntityId = NULL;


	do
	{
		//获得当前数据库的块表
		AcDbBlockTable* pBlockTable;
		acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForWrite);


		//创建块参照对象
		AcDbBlockReference* pBlockReference = new AcDbBlockReference(_PointDatum, _BlockTableRecordId);


		//将块参照添加到模型空间
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
		pBlockTableRecord->appendAcDbEntity(EntityId, pBlockReference);


		//关闭数据库的对象
		pBlockReference->close();
		pBlockTableRecord->close();
		pBlockTable->close();

	} while (false);


	return EntityId;
}



//将指定路径下的块插入到当前数据库
int ObjectArx::ImportBlockTableRecord(CString _StrPath, CString _StrBlockName)
{
	int Err = 0;


	do
	{
		//获得当前数据库的块表
		AcDbBlockTable* pBlockTable = NULL;
		if (acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead) != 0)
		{
			Err = -1;
			break;
		}


		//获得数据库中是否存在指定块表记录
		AcDbObjectId BlockTableRecordId = NULL;
		if (pBlockTable->getAt(_StrBlockName, BlockTableRecordId) == 0)
		{
			pBlockTable->close();
			Err = -2;
			break;
		}


		//关闭块表
		pBlockTable->close();


		//读取DWG文件获得数据库
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


		//导入图块
		Err = ImportBlockTableRecord(pExternal, _StrBlockName);


		//释放内存
		delete pExternal;


	} while (false);


	return Err;
}
int ObjectArx::ImportBlockTableRecord(AcDbDatabase* _pDatabase, CString _StrBlockName)
{
	int Err = 0;


	do
	{
		//打开外部图形数据库的块表
		AcDbBlockTable* pBlockTable = NULL;
		if (_pDatabase->getBlockTable(pBlockTable, AcDb::kForRead) != 0)
		{
			Err = -1;
			break;
		}


		//判断外部图形数据库是否存在指定名称的块
		if (pBlockTable->has(_StrBlockName) != 1)
		{
			Err = -2;
			pBlockTable->close();
			break;
		}


		//获得指定名称的块表记录Id
		AcDbObjectId BlockTableRecordId = NULL;
		if (pBlockTable->getAt(_StrBlockName, BlockTableRecordId) != 0)
		{
			pBlockTable->close();
			Err = -3;
			break;
		}


		//把指定的图块输出到一个临时图形数据库
		AcDbDatabase* pTempDb = NULL;
		_pDatabase->wblock(pTempDb, BlockTableRecordId);


		//把该临时图形数据库作为块插入到当前dwg
		acdbHostApplicationServices()->workingDatabase()->insert(BlockTableRecordId, _StrBlockName, pTempDb);


		//释放内存
		delete pTempDb;
		pBlockTable->close();

	} while (false);


	return Err;
}


AcDbObjectIdArray ObjectArx::splitCurves(AcDbObjectId _entityId, AcDbObjectId _cuttingId) {

	AcDbObjectIdArray segments;

	do {

		//求交点
		AcGePoint3dArray intersection = ObjectArx::GetObjectInter(_entityId, _cuttingId);
		if (intersection.length() <= 0) {
			continue;
		}

		//打开曲线
		AcDbCurve* pCurve = NULL;
		if (::acdbOpenObject(pCurve, _entityId, AcDb::OpenMode::kForWrite) != 0) {
			continue;
		}

		//打断实体
		AcDbVoidPtrArray curveSegments;
		pCurve->getSplitCurves(intersection, curveSegments);

		//添加到模型空间
		for (int i = 0; i < curveSegments.length(); i++) {
			AcDbEntity* pEntity = (AcDbEntity*)curveSegments.at(i);
			if (pEntity == NULL) {
				continue;
			}

			segments.append(ObjectArx::AddObjectToModl(pEntity));
		}

		//关闭对象
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

//导入块参照
AcDbObjectId ObjectArx::ImportBlockReference(CString _StrPath, CString _StrBlockName)
{
	AcDbObjectId EntityId = NULL;


	do
	{
		//获得模型空间最后一个对象
		AcDbObjectId EndEntityId = GetEndEntity();


		//打开数据库模型空间块表记录
		AcDbDatabase* pExternalDatabase = new AcDbDatabase(Adesk::kFalse);
		if (Acad::eOk != pExternalDatabase->readDwgFile(_StrPath))
		{
			break;
		}


		//打开块表
		AcDbBlockTable* pBlockTable = NULL;
		pExternalDatabase->getSymbolTable(pBlockTable, AcDb::kForRead);


		//获得模型空间的块表记录
		AcDbBlockTableRecord* pBlockTableRecord = NULL;
		pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
		pBlockTable->close();


		//创建块表记录遍历器
		AcDbBlockTableRecordIterator* pBlockTableRecordIterator = NULL;
		pBlockTableRecord->newIterator(pBlockTableRecordIterator);


		//遍历块表记录
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


			//判断是否是指定块
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


			//删除实体
			if (IsErase == true)
			{
				pEnt->erase();
			}


			pEnt->close();
		}


		//关闭块表
		pBlockTableRecord->close();
		delete pBlockTableRecordIterator;


		//将数据库中的数据导入一个临时数据库
		AcDbDatabase* pTempDatabase = NULL;
		if (Acad::eOk != pExternalDatabase->wblock(pTempDatabase))
		{
			delete pExternalDatabase;
			break;
		}


		//将临时数据库中的数据导入数据库
		acdbHostApplicationServices()->workingDatabase()->insert(AcGeMatrix3d::kIdentity, pTempDatabase);


		//关闭数据库
		delete pTempDatabase;
		delete pExternalDatabase;


		//获得实体对象
		EntityId = GetEndEntity();
		if (EndEntityId == EntityId)
		{
			EntityId = NULL;
		}

	} while (false);


	return EntityId;
}



/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/







//其他
/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

//获得点与直线的垂点
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