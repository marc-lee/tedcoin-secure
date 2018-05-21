#ifndef _MUSC_UTILS_EX_H_
#define _MUSC_UTILS_EX_H_

namespace epee
{
namespace reg_utils
{
	//-----------------------------------------------------------------------------------------------------------------------------------
	template<class T>
		bool RegSetPODValue(HKEY hParentKey, const char* pSubKey, const char* pValName, const T& valToSave, bool force_create = true)
	{
		HKEY hRegKey = 0;
		DWORD dw = 0;

		if( ::RegOpenKeyExA(hParentKey, pSubKey, 0, KEY_WRITE, &hRegKey) != ERROR_SUCCESS )
			if(force_create && (::RegCreateKeyExA(hParentKey, pSubKey, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hRegKey, &dw) != ERROR_SUCCESS) )
				return false;


		DWORD val_type = (sizeof(valToSave) == sizeof(DWORD)) ? REG_DWORD:REG_BINARY;

		BOOL res = ::RegSetValueExA( hRegKey, pValName, 0, val_type, (LPBYTE)&valToSave, sizeof(valToSave)) == ERROR_SUCCESS;

		::RegCloseKey(hRegKey);
		return ERROR_SUCCESS==res ? true:false;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	template<class T>
		bool RegGetPODValue(HKEY hParentKey, const char* pSubKey, const char* pValName, T& valToSave)
	{
		HKEY hRegKey = 0;
		LONG res = 0;


		if(::RegOpenKeyExA(hParentKey, pSubKey, 0, KEY_READ, &hRegKey) == ERROR_SUCCESS )
		{
			DWORD dwType, lSize = 0;
			res = ::RegQueryValueExA(hRegKey, pValName, 0, &dwType, NULL, &lSize);        
			if(ERROR_SUCCESS!=res || (sizeof(valToSave) < lSize) ) 
			{
				::RegCloseKey(hRegKey);
				return false;
			}
			res = ::RegQueryValueExA(hRegKey, pValName, 0, &dwType, (LPBYTE)&valToSave, &lSize);			
		}
		return ERROR_SUCCESS==res ? true:false;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	inline
	bool RegSetANSIString(HKEY hParentKey, const char* pSubKey, const char* pValName, const std::string& strToSave)
	{
		HKEY hRegKey = 0;
		DWORD dw = 0;
		DWORD res_ = 0;
		if( (res_ = ::RegCreateKeyExA(hParentKey, pSubKey, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hRegKey, &dw)) != ERROR_SUCCESS )
			if( (res_= ::RegOpenKeyExA(hParentKey, pSubKey, 0, KEY_WRITE, &hRegKey)) != ERROR_SUCCESS )
				return false;

		DWORD valType = REG_SZ;
		const char* pStr = strToSave.c_str();
		DWORD sizeOfStr = (DWORD)strToSave.size()+1;
		LSTATUS res = ::RegSetValueExA(hRegKey, pValName, 0, valType, (LPBYTE)pStr, sizeOfStr);

		::RegCloseKey(hRegKey);
		return ERROR_SUCCESS==res ? true:false;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	inline
	bool RegGetANSIString(HKEY hParentKey, const char* pSubKey, const char* pValName,  std::string& strToSave)
	{
		HKEY hRegKey = 0;
		LONG res = 0;


		if((res = ::RegOpenKeyExA(hParentKey, pSubKey, 0, KEY_READ, &hRegKey)) == ERROR_SUCCESS )
		{
			DWORD dwType, lSize = 0;
			res = ::RegQueryValueExA(hRegKey, pValName, 0, &dwType, NULL, &lSize);        
			if(ERROR_SUCCESS!=res) 
			{
				
				::RegCloseKey(hRegKey);
				return false;
			}
			char* pTmpStr = new char[lSize+2];
			memset(pTmpStr, 0, lSize+2);
			res = ::RegQueryValueExA(hRegKey, pValName, 0, &dwType, (LPBYTE)pTmpStr, &lSize);			
			pTmpStr[lSize+1] = 0; //be happy ;)
			strToSave = pTmpStr;
			delete [] pTmpStr;
			::RegCloseKey(hRegKey);
		}
		return ERROR_SUCCESS==res ? true:false;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	template<class TMemoryObject>
	bool RegSetRAWValue(HKEY hKey, const char* pValName, const TMemoryObject& valToSave, DWORD valType = REG_BINARY)
	{
		LONG res = ::RegSetValueExA( hKey, pValName, 0, valType, (CONST BYTE*)valToSave.get(0),  (DWORD)valToSave.get_size());

		return ERROR_SUCCESS==res ? true:false;
	}
	//----------------------------------------------------------------------------------------------------------------------------------
	bool RegSetRAWValue(HKEY hKey, const char* pValName, const std::string & valToSave, DWORD valType = REG_BINARY)
	{
		LONG res = ::RegSetValueExA( hKey, pValName, 0, valType, (CONST BYTE*)valToSave.data(),  (DWORD)valToSave.size());

		return ERROR_SUCCESS==res ? true:false;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	template<class TMemoryObject>
	bool RegGetRAWValue(HKEY hKey, const char* pValName, TMemoryObject& valToSave, DWORD* pRegType)
	{
		DWORD dwType, lSize = 0;
		LONG res = ::RegQueryValueExA(hKey, pValName, 0, &dwType, NULL, &lSize);        
		if(ERROR_SUCCESS!=res || 0 >= lSize)  
		{
			valToSave.release();
			return false;
		}
		if(valToSave.get_size() < lSize)
			valToSave.alloc_buff(lSize);
		res = ::RegQueryValueExA(hKey, pValName, 0, &dwType, (LPBYTE)valToSave.get(0), &lSize);			
		if(pRegType) *pRegType = dwType;

		return ERROR_SUCCESS==res ? true:false;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	bool RegGetRAWValue(HKEY hKey, const char* pValName, std::string& valToSave, DWORD* pRegType)
	{
		DWORD dwType, lSize = 0;
		LONG res = ::RegQueryValueExA(hKey, pValName, 0, &dwType, NULL, &lSize);        
		if(ERROR_SUCCESS!=res || 0 >= lSize)  
		{
			return false;
		}
		
		valToSave.resize(lSize);
		res = ::RegQueryValueExA(hKey, pValName, 0, &dwType, (LPBYTE)valToSave.data(), &lSize);			
		if(pRegType) *pRegType = dwType;

		return ERROR_SUCCESS==res ? true:false;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	template<class TMemoryObject>
	bool RegSetRAWValue(HKEY hParentKey, const char* pSubKey, const char* pValName, const TMemoryObject& valToSave, DWORD valType = REG_BINARY)
	{
		HKEY hRegKey = 0;
		DWORD dw = 0;
		bool res = false;

		if( ::RegCreateKeyExA(hParentKey, pSubKey, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hRegKey, &dw) != ERROR_SUCCESS )
			if( ::RegOpenKeyExA(hParentKey, pSubKey, 0, KEY_WRITE, &hRegKey) != ERROR_SUCCESS )
				return false;

		res = RegSetRAWValue(hRegKey, pValName, valToSave, valType);

		::RegCloseKey(hRegKey);
		return res;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	bool RegSetRAWValue(HKEY hParentKey, const char* pSubKey, const char* pValName, const std::string& valToSave, DWORD valType = REG_BINARY)
	{
		HKEY hRegKey = 0;
		DWORD dw = 0;
		bool res = false;

		if( ::RegCreateKeyExA(hParentKey, pSubKey, 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hRegKey, &dw) != ERROR_SUCCESS )
			if( ::RegOpenKeyExA(hParentKey, pSubKey, 0, KEY_WRITE, &hRegKey) != ERROR_SUCCESS )
				return false;

		res = RegSetRAWValue(hRegKey, pValName, valToSave, valType);

		::RegCloseKey(hRegKey);
		return res;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	template<class TMemoryObject>
	bool RegGetRAWValue(HKEY hParentKey, const char* pSubKey, const char* pValName, TMemoryObject& valToSave, DWORD* pRegType)
	{
		HKEY hRegKey = 0;
		bool res = false;

		if(::RegOpenKeyExA(hParentKey, pSubKey, 0, KEY_READ, &hRegKey) == ERROR_SUCCESS )
		{
			res = RegGetRAWValue(hRegKey, pValName, valToSave, pRegType);
            ::RegCloseKey(hRegKey);
		}
		return res;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	inline
	bool RegGetRAWValue(HKEY hParentKey, const char* pSubKey, const char* pValName, std::string& valToSave, DWORD* pRegType)
	{
		HKEY hRegKey = 0;
		bool res = false;

		if(::RegOpenKeyExA(hParentKey, pSubKey, 0, KEY_READ, &hRegKey) == ERROR_SUCCESS )
		{
			res = RegGetRAWValue(hRegKey, pValName, valToSave, pRegType);
			::RegCloseKey(hRegKey);
		}
		return res;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	inline
	bool RegRemoveValue(HKEY hParentKey, const char* pValName)
	{
		//CHECK_AND_ASSERT(hParentKey&&pValName, false);
		return ::RegDeleteValueA(hParentKey, pValName)==ERROR_SUCCESS ? true:false;
	}
	//-----------------------------------------------------------------------------------------------------------------------------------
	inline
	bool RegRemoveKey(HKEY hParentKey, const char* pKeyName)
	{
		//CHECK_AND_ASSERT(hParentKey&&pKeyName, false);
		return ::RegDeleteKeyA(hParentKey, pKeyName)==ERROR_SUCCESS ? true:false;
	}

}
}
#endif //_MUSC_UTILS_EX_H_
