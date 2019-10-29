#include "Header.h"
#include "Findfile.h"
#include "tinyxml2.h"
#include <wincrypt.h>
#include <dpapi.h>
#include <wlanapi.h>
#include <atlstr.h>
#pragma comment(lib,"Crypt32.lib")
#pragma comment (lib,"wlanapi.lib")

struct wifidetails{
	std::string name,ssidhex,ssidname,conntype,connmode,
		authentication,encryption,useronex,keytype,protect,keymaterial,
		macreandomization;
	
};
bool compareChar(char& c1, char& c2)
{
	if (c1 == c2)
		return true;
	else if (std::toupper(c1) == std::toupper(c2))
		return true;
	return false;
}
bool caseInSensStringCompare(std::string& str1, std::string& str2)
{
	return ((str1.size() == str2.size()) &&
		std::equal(str1.begin(), str1.end(), str2.begin(), &compareChar));
}
bool dirExists(std::string dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}
	else {

		if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		{
			return true;
		}

		return false;
	}
}
int main() {
	HANDLE handle;
	DWORD dw = 0;
	DWORD error = WlanOpenHandle(1,NULL,&dw,&handle);
	if (error != ERROR_SUCCESS) {
		std::cout << "WlanOpenHandle()" << GetLastError() << std::endl;
		return 0;
	}
	WLAN_INTERFACE_INFO_LIST* wlanadapter = NULL;
	error = WlanEnumInterfaces(handle,NULL,&wlanadapter);
	if (error != ERROR_SUCCESS) {
		std::cout << "WlanEnumInterfaces()" << GetLastError() << std::endl;
		WlanCloseHandle(handle, NULL);
		return 0;
	}
	int rescount = 1;
	WLAN_INTERFACE_INFO* wlanadapterinfo = NULL;
	for (size_t x = 0; x < wlanadapter->dwNumberOfItems; x++) {
		wlanadapterinfo = &wlanadapter->InterfaceInfo[x];
		WLAN_PROFILE_INFO_LIST* wlanprofileinfo;
		error = WlanGetProfileList(handle, &wlanadapterinfo->InterfaceGuid, NULL, &wlanprofileinfo);
		if (error != ERROR_SUCCESS) {
			continue;
		}
		else {
			std::cout << "Interface found :" << wlanprofileinfo->dwNumberOfItems + 1 << std::endl;
			for (size_t j = 0; j < wlanprofileinfo->dwNumberOfItems; j++) {
				WLAN_PROFILE_INFO* wlaninfo = &wlanprofileinfo->ProfileInfo[j];
				LPWSTR xmlprofile;
				DWORD dwflags = WLAN_PROFILE_GET_PLAINTEXT_KEY | WLAN_PROFILE_USER;
				DWORD dwaccess = WLAN_READ_ACCESS;
				error = WlanGetProfile(handle, &wlanadapterinfo->InterfaceGuid, wlaninfo->strProfileName, NULL, &xmlprofile, &dwflags, &dwaccess);
				if (error == ERROR_SUCCESS) {
					CString strxml = xmlprofile;
					WlanFreeMemory(xmlprofile);
					std::cout << strxml << std::endl;
				}
				
			}
		}

	}
		
	/*if (IsWindows7OrGreater()) {
		std::cout << "The system is windows 7 and later" << std::endl;
			std::string p="C:/ProgramData/Microsoft/Wlansvc/Profiles/Interfaces/";
			std::cout<<p<<std::endl;
			if (dirExists(p)) {
				std::cout << "The computer has been connected to WLAN. " << std::endl;
				findfile xmlfiles;
				xmlfiles.findfiles(p, "", ".xml", true);
				for (size_t x = 0; x < xmlfiles.files.size(); x++) {
					std::cout << xmlfiles.files[x] << std::endl;
					tinyxml2::XMLDocument doc;
					doc.LoadFile(xmlfiles.files[x].c_str());
					wifidetails details;
					details.name = doc.FirstChildElement("WLANProfile")->FirstChildElement("name")->GetText();
					details.ssidhex = doc.FirstChildElement("WLANProfile")->FirstChildElement("SSIDConfig")->FirstChildElement("SSID")->FirstChildElement("hex")->GetText();
					details.ssidname = doc.FirstChildElement("WLANProfile")->FirstChildElement("SSIDConfig")->FirstChildElement("SSID")->FirstChildElement("name")->GetText();
					details.conntype = doc.FirstChildElement("WLANProfile")->FirstChildElement("connectionType")->GetText();
					details.connmode = doc.FirstChildElement("WLANProfile")->FirstChildElement("connectionMode")->GetText();
					details.authentication = doc.FirstChildElement("WLANProfile")->FirstChildElement("MSM")->FirstChildElement("security")->FirstChildElement("authEncryption")->FirstChildElement("authentication")->GetText();
					details.encryption = doc.FirstChildElement("WLANProfile")->FirstChildElement("MSM")->FirstChildElement("security")->FirstChildElement("authEncryption")->FirstChildElement("encryption")->GetText();
					details.useronex = doc.FirstChildElement("WLANProfile")->FirstChildElement("MSM")->FirstChildElement("security")->FirstChildElement("authEncryption")->FirstChildElement("useOneX")->GetText();
					details.keytype = doc.FirstChildElement("WLANProfile")->FirstChildElement("MSM")->FirstChildElement("security")->FirstChildElement("sharedKey")->FirstChildElement("keyType")->GetText();
					details.protect = doc.FirstChildElement("WLANProfile")->FirstChildElement("MSM")->FirstChildElement("security")->FirstChildElement("sharedKey")->FirstChildElement("protected")->GetText();
					details.keymaterial = doc.FirstChildElement("WLANProfile")->FirstChildElement("MSM")->FirstChildElement("security")->FirstChildElement("sharedKey")->FirstChildElement("keyMaterial")->GetText();
					details.macreandomization = doc.FirstChildElement("WLANProfile")->FirstChildElement("MacRandomization")->FirstChildElement("enableRandomization")->GetText();
					std::string prot = "true";
					if (caseInSensStringCompare(details.protect ,prot)) {
						std::cout << "protected" << std::endl;
						BYTE bytekey[MAXBYTE] = { 0 };
						DWORD length = MAXBYTE;
						DATA_BLOB dataout, dataverify;
						if (CryptStringToBinaryA(details.keymaterial.c_str(), details.keymaterial.length(), CRYPT_STRING_HEX, bytekey, &length, 0, 0)) {
							dataout.cbData = length;
							dataout.pbData = (BYTE*)bytekey;
							if (CryptUnprotectData(&dataout, NULL, NULL, NULL, NULL, 0, &dataverify)) {
								std::cout << dataverify.pbData;
							}
							else {
								std::cout << "CryptUnprotectData()" << GetLastError() << std::endl;
							}
						}
						else {
							std::cout << "CryptstringtobinaryA()" << GetLastError() << std::endl;
						}
					}
					else {
						std::cout << "Not protected" << std::endl;						
					}
				}

			}
			else {
				std::cout << "The computer has never been connected to WLAN :" << GetLastError() << std::endl;
			}		
	}
	else {
		std::cout << "The system is Windows XP and earlier" << std::endl;
	}*/
	system("pause");
	return 0;
}