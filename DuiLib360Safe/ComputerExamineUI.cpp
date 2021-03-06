
#include <tchar.h>
#include "./core/UIDlgBuilder.h"
#include "ComputerExamineUI.h"

ComputerExamineUI::ComputerExamineUI()
{
	DuiLib::CDialogBuilder builder;
	CContainerUI* pComputerExamine = static_cast<CContainerUI*>(builder.Create(_T("ComputerExamine.xml"), (UINT)0));
	if (pComputerExamine) {
		this->Add(pComputerExamine);
	}
	else {
		this->RemoveAll();
		return;
	}
}
