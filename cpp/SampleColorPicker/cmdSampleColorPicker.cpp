/////////////////////////////////////////////////////////////////////////////
// cmdSampleColorPicker.cpp : command file
//

#include "StdAfx.h"
#include "SampleColorPickerPlugIn.h"
#include "WindowsColorDialog.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// BEGIN SampleColorPicker command
//

#pragma region SampleColorPicker command

class CWindowsColorPickerDialog;

class CCommandSampleColorPicker : public CRhinoCommand
{
public:
	CCommandSampleColorPicker();
  ~CCommandSampleColorPicker();

	UUID CommandUUID()
	{
		// {1D1DA49F-A707-4365-8255-A0176199519F}
    static const GUID SampleColorPickerCommand_UUID =
    { 0x1D1DA49F, 0xA707, 0x4365, { 0x82, 0x55, 0xA0, 0x17, 0x61, 0x99, 0x51, 0x9F } };
    return SampleColorPickerCommand_UUID;
	}

	const wchar_t* EnglishCommandName() { return L"SampleColorPicker"; }
	const wchar_t* LocalCommandName() const { return L"SampleColorPicker"; }

  void LoadProfile( LPCTSTR lpszSection, CRhinoProfileContext& pc );
  void SaveProfile( LPCTSTR lpszSection, CRhinoProfileContext& pc );

	CRhinoCommand::result RunCommand( const CRhinoCommandContext& );

public:
  bool m_bUseWindows;
  CWindowsColorPickerDialog* m_pDialog;
  COLORREF m_CustomColors[16];

private:
  void ClearColorPicker();
  void SetColorPicker( bool bUseWindowsPicker );
};

// The one and only CCommandSampleColorPicker object.  
// Do NOT create any other instance of a CCommandSampleColorPicker class.
static class CCommandSampleColorPicker theSampleColorPickerCommand;

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

class CWindowsColorPickerDialog : public CRhinoReplaceColorDialog
{
public:
  CWindowsColorPickerDialog();
  bool ColorDialog(
    HWND hWndParent, 
    ON_Color& color, 
    bool bButtonColors, 
    const wchar_t* pszTitle = 0 
    );
};

CWindowsColorPickerDialog::CWindowsColorPickerDialog()
: CRhinoReplaceColorDialog( ::AfxGetStaticModuleState() )
{
}

bool CWindowsColorPickerDialog::ColorDialog( HWND hWndParent, ON_Color& color, bool bButtonColors, const wchar_t* pszTitle )
{
  CWindowsColorDialog dlg( (COLORREF)color, CC_ANYCOLOR|CC_FULLOPEN, CWnd::FromHandle(hWndParent) );
  dlg.m_cc.lpCustColors = theSampleColorPickerCommand.m_CustomColors;
  if( pszTitle && pszTitle[0] )
    dlg.SetTitle( pszTitle );
  if( IDOK != dlg.DoModal() )
    return false;
  color = dlg.GetColor();
  return true;
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

CCommandSampleColorPicker::CCommandSampleColorPicker()
: m_pDialog(0)
{
  m_bUseWindows = false;

  for( int i = 0; i < 16; i++ )
    m_CustomColors[i] = RGB(255,255,255);
}

CCommandSampleColorPicker::~CCommandSampleColorPicker()
{
  ClearColorPicker();
}

void CCommandSampleColorPicker::LoadProfile( LPCTSTR lpszSection, CRhinoProfileContext& pc )
{
  bool bUseWindows = m_bUseWindows;
  if( pc.LoadProfileBool(lpszSection, L"UseWindows", &bUseWindows) )
    SetColorPicker( bUseWindows );

  ON_wString str;
  for( int i = 0; i < 16; i++ )
  {
    COLORREF color = RGB(255,255,255);
    str.Format( L"CustomColor%d", i );
    if( pc.LoadProfileColor( lpszSection, str, &color ) )
      m_CustomColors[i] = color;
  }
}

void CCommandSampleColorPicker::SaveProfile( LPCTSTR lpszSection, CRhinoProfileContext& pc )
{
  pc.SaveProfileBool( lpszSection, L"UseWindows", m_bUseWindows );

  ON_wString str;
  for( int i = 0; i < 16; i++ )
  {
    str.Format( L"CustomColor%d", i );
    pc.SaveProfileColor( lpszSection, str, m_CustomColors[i] );
  }
}

void CCommandSampleColorPicker::ClearColorPicker()
{
  if( m_pDialog )
  {
    delete m_pDialog;
    m_pDialog = 0;
  }

  m_bUseWindows = false;
}

void CCommandSampleColorPicker::SetColorPicker( bool bUseWindowsPicker )
{
  ClearColorPicker();

  if( bUseWindowsPicker )
  {
    m_pDialog = new CWindowsColorPickerDialog();
    if( m_pDialog )
      m_bUseWindows = true;
  }
}

CRhinoCommand::result CCommandSampleColorPicker::RunCommand( const CRhinoCommandContext& context )
{
  if( m_bUseWindows )
    RhinoApp().Print( L"Using the Windows color picker.\n" );
  else
    RhinoApp().Print( L"Using the Rhino color picker.\n" );

  CRhinoGetOption go;
  go.SetCommandPrompt( L"Choose color picker" );
  go.AcceptNothing();

  int rhino_option = go.AddCommandOption( RHCMDOPTNAME(L"Rhino") );
  int windows_option = go.AddCommandOption( RHCMDOPTNAME(L"Windows") );

  CRhinoGet::result res = go.GetOption();
  if( res == CRhinoGet::nothing )
    return CRhinoCommand::nothing;
  else if( res != CRhinoGet::option )
    return CRhinoCommand::cancel;

  const CRhinoCommandOption* option = go.Option();
  if( 0 == option )
    return CRhinoCommand::failure;

  SetColorPicker( windows_option == option->m_option_index );

  if( m_bUseWindows )
    RhinoApp().Print( L"Color picker set to Windows.\n" );
  else
    RhinoApp().Print( L"Color picker set to Rhino.\n" );

  return CRhinoCommand::success;
}

#pragma endregion

//
// END SampleColorPicker command
//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
