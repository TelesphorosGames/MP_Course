// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeColinsMultiplayerSessions_init() {}
	COLINSMULTIPLAYERSESSIONS_API UFunction* Z_Construct_UDelegateFunction_ColinsMultiplayerSessions_MultiplayerSubsessionOnCreateSessionComplete__DelegateSignature();
	COLINSMULTIPLAYERSESSIONS_API UFunction* Z_Construct_UDelegateFunction_ColinsMultiplayerSessions_MultiplayerSubsessionOnDestroySessionComplete__DelegateSignature();
	COLINSMULTIPLAYERSESSIONS_API UFunction* Z_Construct_UDelegateFunction_ColinsMultiplayerSessions_MultiplayerSubsessionOnStartSessionComplete__DelegateSignature();
	static FPackageRegistrationInfo Z_Registration_Info_UPackage__Script_ColinsMultiplayerSessions;
	FORCENOINLINE UPackage* Z_Construct_UPackage__Script_ColinsMultiplayerSessions()
	{
		if (!Z_Registration_Info_UPackage__Script_ColinsMultiplayerSessions.OuterSingleton)
		{
			static UObject* (*const SingletonFuncArray[])() = {
				(UObject* (*)())Z_Construct_UDelegateFunction_ColinsMultiplayerSessions_MultiplayerSubsessionOnCreateSessionComplete__DelegateSignature,
				(UObject* (*)())Z_Construct_UDelegateFunction_ColinsMultiplayerSessions_MultiplayerSubsessionOnDestroySessionComplete__DelegateSignature,
				(UObject* (*)())Z_Construct_UDelegateFunction_ColinsMultiplayerSessions_MultiplayerSubsessionOnStartSessionComplete__DelegateSignature,
			};
			static const UECodeGen_Private::FPackageParams PackageParams = {
				"/Script/ColinsMultiplayerSessions",
				SingletonFuncArray,
				UE_ARRAY_COUNT(SingletonFuncArray),
				PKG_CompiledIn | 0x00000000,
				0x9F53C186,
				0x16C1C4AC,
				METADATA_PARAMS(nullptr, 0)
			};
			UECodeGen_Private::ConstructUPackage(Z_Registration_Info_UPackage__Script_ColinsMultiplayerSessions.OuterSingleton, PackageParams);
		}
		return Z_Registration_Info_UPackage__Script_ColinsMultiplayerSessions.OuterSingleton;
	}
	static FRegisterCompiledInInfo Z_CompiledInDeferPackage_UPackage__Script_ColinsMultiplayerSessions(Z_Construct_UPackage__Script_ColinsMultiplayerSessions, TEXT("/Script/ColinsMultiplayerSessions"), Z_Registration_Info_UPackage__Script_ColinsMultiplayerSessions, CONSTRUCT_RELOAD_VERSION_INFO(FPackageReloadVersionInfo, 0x9F53C186, 0x16C1C4AC));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
