[EntityEditorProps(category: "GameScripted/UI/Inventory", description: "Inventory Item Info UI class")]

//------------------------------------------------------------------------------------------------
//! UI Script
//! Inventory Slot UI Layout
class SCR_InventoryItemInfoUI : ScriptedWidgetComponent
{

	private Widget							m_infoWidget;
	private TextWidget						m_wTextName;
	private TextWidget						m_wTextDescription;
	private TextWidget						m_wTextWeight;
	private TextWidget						m_wTextWeightUnit;
	private TextWidget						m_wTextInsulation;
	private TextWidget						m_wTextCharName;
	private ImageWidget 					m_wItemIcon;
	protected SCR_SlotUIComponent			m_pFrameSlotUI;
	protected Widget 						m_wWidgetUnderCursor;
	protected bool 							m_bForceShow;
	
					
	//------------------------------------------------------------------------ USER METHODS ------------------------------------------------------------------------							
					
	//------------------------------------------------------------------------------------------------	
			
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	protected void ShowInfoWidget( bool bShow )
	{
		if (m_bForceShow)
		{
			m_infoWidget.SetVisible( true );
			return;
		}

		if ( !m_wWidgetUnderCursor )
			return;
		if ( WidgetManager.GetWidgetUnderCursor() != m_wWidgetUnderCursor )
			return; //the cursor is on different position already
		m_infoWidget.SetVisible( true );
	}
		
	void Show( float fDelay = 0.0, Widget w = null, bool forceShow = false )
	{
		m_bForceShow = forceShow;
		m_wWidgetUnderCursor = w;
		if ( fDelay == 0 )
			ShowInfoWidget( true );
		else
		{
			GetGame().GetCallqueue().Remove( ShowInfoWidget );
			GetGame().GetCallqueue().CallLater( ShowInfoWidget, fDelay*1000, false, true );
		}
	}
	
	void SetIcon(ResourceName iconPath, Color color = null)
	{
		if (iconPath.IsEmpty())
			return;
		
		m_wItemIcon.SetVisible(m_wItemIcon.LoadImageTexture(0, iconPath));
		if (color)
			m_wItemIcon.SetColor(color);
	}
	
	//------------------------------------------------------------------------------------------------
	void Hide( float fDelay = 1.0 )
	{
		m_infoWidget.SetVisible( false );
		m_infoWidget.SetEnabled( false );
		m_wItemIcon.SetVisible(false);
	}
		
	//------------------------------------------------------------------------------------------------
	void Move( float x, float y )
	{
		if ( !m_pFrameSlotUI )
			return;
		m_pFrameSlotUI.SetPosX( x );
		m_pFrameSlotUI.SetPosY( y );
	}
	
	
	//------------------------------------------------------------------------------------------------
	void SetName( string sName )
	{
		m_wTextName.SetText( sName );
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDescription( string sDescr )
	{
		if( sDescr != "" )
		{
			m_wTextDescription.SetEnabled( true );
			m_wTextDescription.SetVisible( true );
			m_wTextDescription.SetText( sDescr );
		}
		else
		{
			m_wTextDescription.SetEnabled( false );
			m_wTextDescription.SetVisible( false );
		}
	}
	//------------------------------------------------------------------------------------------------
	void SetInsulation( string insulation )
	{
		if( insulation != "" )
		{
			m_wTextInsulation.SetEnabled( true );
			m_wTextInsulation.SetVisible( true );
			m_wTextInsulation.SetText( insulation );
		}
		else
		{
			m_wTextInsulation.SetEnabled( false );
			m_wTextInsulation.SetVisible( false );
		}
	}
	//------------------------------------------------------------------------------------------------
	void SetCharName( string name )
	{
		if( name != "" )
		{
			m_wTextCharName.SetEnabled( true );
			m_wTextCharName.SetVisible( true );
			m_wTextCharName.SetText( name );
		}
		else
		{
			m_wTextCharName.SetEnabled( false );
			m_wTextCharName.SetVisible( false );
		}
	}
	//------------------------------------------------------------------------------------------------
	void SetWeight( string sWeight )
	{
		if( sWeight != "" )
		{
			m_wTextWeight.SetEnabled( true );
			m_wTextWeight.SetVisible( true );
			m_wTextWeightUnit.SetEnabled( true );
			m_wTextWeightUnit.SetVisible( true );
			m_wTextWeight.SetText( sWeight );
		}
		else
		{
			m_wTextWeight.SetEnabled( false );
			m_wTextWeight.SetVisible( false );
			m_wTextWeightUnit.SetEnabled( false );
			m_wTextWeightUnit.SetVisible( false );
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SetWeight( float fWeight )
	{
		if( fWeight <= 0.0 )
			SetWeight( "" );						
		else
			SetWeight( fWeight.ToString() );
	}

	//------------------------------------------------------------------------------------------------
	override void HandlerAttached( Widget w )
	{
		if( !w )
			return;
		m_infoWidget		= w;
		m_wTextName 		= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_name" ) );
		m_wTextDescription 	= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_description" ) );
		m_wTextWeight 		= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_weight" ) );
		m_wTextWeightUnit	= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_weightUnit" ) );
		m_wTextInsulation	= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_insulationUnit" ) );
		m_wTextCharName		= TextWidget.Cast( w.FindAnyWidget( "ItemInfo_CharName" ) );
		m_wItemIcon 		= ImageWidget.Cast(w.FindAnyWidget("ItemInfo_icon"));
		Widget wItemInfo	= m_infoWidget.FindAnyWidget( "ItemInfo" );
		if ( !wItemInfo )
			return;
		m_pFrameSlotUI 		= SCR_SlotUIComponent.Cast( wItemInfo.FindHandler( SCR_SlotUIComponent ) );
	}
	
	//------------------------------------------------------------------------------------------------
	void Destroy()
	{
		if ( m_infoWidget )
		{		
			GetGame().GetCallqueue().Remove( ShowInfoWidget );
			m_infoWidget.RemoveHandler( m_pFrameSlotUI );
			m_infoWidget.RemoveHandler( this );
			m_infoWidget.RemoveFromHierarchy();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	Widget GetInfoWidget()
	{
		return m_infoWidget.FindAnyWidget("size");
	}

	//------------------------------------------------------------------------ COMMON METHODS ----------------------------------------------------------------------
	
			
	//------------------------------------------------------------------------------------------------
	
	void SCR_InventoryItemInfoUI()
	{
	}	

	//------------------------------------------------------------------------------------------------
	void ~SCR_InventoryItemInfoUI()
	{
	}
};
