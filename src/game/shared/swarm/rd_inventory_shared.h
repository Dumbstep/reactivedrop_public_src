#pragma once

#include "steam/steam_api.h"
//#include "jsmn.h"

#ifdef CLIENT_DLL
namespace vgui
{
	class IImage;
}
#endif

namespace ReactiveDropInventory
{
	struct ItemDef_t
	{
		SteamItemDef_t ID;
		CUtlString ItemSlot;
		CUtlString Tags;
		CUtlString DisplayType;
		CUtlString Name;
		CUtlString BriefingName;
		CUtlString Description;
		CUtlString BeforeDescription;
		CUtlString AfterDescription;
		bool AfterDescriptionOnlyMultiStack;
#ifdef CLIENT_DLL
		vgui::IImage *Icon;
		vgui::IImage *IconSmall;
#endif
	};

	const ItemDef_t *GetItemDef( SteamItemDef_t id );
	void FormatDescription( wchar_t *wszBuf, size_t sizeOfBufferInBytes, const CUtlString &szDesc, SteamInventoryResult_t hResult, uint32_t index );
	bool DecodeItemData( SteamInventoryResult_t &hResult, const char *szEncodedData );
	bool ValidateItemData( bool &bValid, SteamInventoryResult_t hResult, const char *szRequiredSlot = NULL, CSteamID requiredSteamID = k_steamIDNil, bool bRequireFresh = false );
	SteamItemDetails_t GetItemDetails( SteamInventoryResult_t hResult, uint32_t index );
}

#ifdef CLIENT_DLL
class CRDTranslation
{
public:
	CRDTranslation( char* szText, bool send )
	{
		ISteamHTTP *pHTTP = SteamHTTP();
		Assert( pHTTP );
		if ( pHTTP )
		{
			// The medal images send a Cache-Control header of "public, max-age=315569520" (1 decade).
			// The Steam API will automatically cache stuff for us, so we don't have to manage cache ourselves.
			HTTPRequestHandle hRequest = pHTTP->CreateHTTPRequest( k_EHTTPMethodGET, "http://fanyi.youdao.com/translate" );
			pHTTP->SetHTTPRequestGetOrPostParameter( hRequest, "type", "AUTO" );
			pHTTP->SetHTTPRequestGetOrPostParameter( hRequest, "i", szText );
			pHTTP->SetHTTPRequestGetOrPostParameter( hRequest, "doctype", "json" );
			pHTTP->SetHTTPRequestGetOrPostParameter( hRequest, "xmlVersion", "1.8" );
			//pHTTP->SetHTTPRequestContextValue( hRequest, (uint64)send );
			SteamAPICall_t hAPICall;
			if ( pHTTP->SendHTTPRequest( hRequest, &hAPICall ) )
			{
				m_HTTPRequestCompleted.Set( hAPICall, this, &CRDTranslation::OnTranslationCompleted );
			}
			else
			{
				Warning( "Sending request for translation failed!\n" );
			}
		}
		else
		{
			Warning( "No ISteamHTTP access - cannot translation.\n" );
		}
	}

	/*bool	jsoneq(const char* json, jsmntok_t* tok, const char* s);
	char*	jsgetval(const char* json, jsmntok_t* tok);*/

	void	OnTranslationCompleted( HTTPRequestCompleted_t* pParam, bool bIOFailure );

private:
	CCallResult<CRDTranslation, HTTPRequestCompleted_t> m_HTTPRequestCompleted;
};
#endif
