#include "pch.h"
#include <vector>
#include <algorithm>
#include "_Func_.h"
#define RKTDEVICE _T("RKTDEVICE")
#define RKTLEGEND _T("RKTLEGEND")



class CDemoApp : public AcRxArxApp
{
public:
	CDemoApp() : AcRxArxApp()
	{}

	virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt)
	{
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);
		
		FRKTEditReactor* pReactor = new FRKTEditReactor();
		
		return (retCode);
	}

	virtual AcRx::AppRetCode On_kQuitMsg(void* pkt)
	{
		AcRx::AppRetCode retCode = AcRxArxApp::On_kQuitMsg(pkt);

		return (retCode);
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt)
	{
		AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);

		return (retCode);
	}

	virtual AcRx::AppRetCode On_kLoadDwgMsg(void* pkt)
	{

		AcRx::AppRetCode retCode = AcRxDbxApp::On_kLoadDwgMsg(pkt);
		RKTInputContextReactor* pInputContextReactor = new RKTInputContextReactor();
		RKTDatabaseReactor* pDatabaseReactor = new RKTDatabaseReactor(acdbHostApplicationServices()->workingDatabase());
		//选择反应器
		curDoc()->inputPointManager()->addInputContextReactor(pInputContextReactor);
		return (retCode);
	}

	virtual AcRx::AppRetCode On_kUnloadDwgMsg(void* pkt)
	{
		AcRx::AppRetCode retCode = AcRxDbxApp::On_kUnloadDwgMsg(pkt);


		return (retCode);
	}


	virtual AcRx::AppRetCode On_kInvkSubrMsg(void* pkt)
	{
		AcRx::AppRetCode retCode = AcRxDbxApp::On_kInvkSubrMsg(pkt);

		return (retCode);
	}

	virtual void RegisterServerComponents()
	{

	}

	static void fun_Move_Demo(void)
	{
		_move_demo::command();
	}
		
};



//-----------------------------------------------------------------------------
	IMPLEMENT_ARX_ENTRYPOINT(CDemoApp)
	ACED_ARXCOMMAND_ENTRY_AUTO(CDemoApp, fun, _Move_Demo, GB_Move, ACRX_CMD_TRANSPARENT, NULL)