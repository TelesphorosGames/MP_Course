// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef COLINSMULTIPLAYERSESSIONS_MultiplayerSessionSubsystem_generated_h
#error "MultiplayerSessionSubsystem.generated.h already included, missing '#pragma once' in MultiplayerSessionSubsystem.h"
#endif
#define COLINSMULTIPLAYERSESSIONS_MultiplayerSessionSubsystem_generated_h

#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_15_DELEGATE \
struct _Script_ColinsMultiplayerSessions_eventMultiplayerSubsessionOnCreateSessionComplete_Parms \
{ \
	bool bWasSuccessful; \
}; \
static inline void FMultiplayerSubsessionOnCreateSessionComplete_DelegateWrapper(const FMulticastScriptDelegate& MultiplayerSubsessionOnCreateSessionComplete, bool bWasSuccessful) \
{ \
	_Script_ColinsMultiplayerSessions_eventMultiplayerSubsessionOnCreateSessionComplete_Parms Parms; \
	Parms.bWasSuccessful=bWasSuccessful ? true : false; \
	MultiplayerSubsessionOnCreateSessionComplete.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_18_DELEGATE \
struct _Script_ColinsMultiplayerSessions_eventMultiplayerSubsessionOnDestroySessionComplete_Parms \
{ \
	bool bWasSuccessful; \
}; \
static inline void FMultiplayerSubsessionOnDestroySessionComplete_DelegateWrapper(const FMulticastScriptDelegate& MultiplayerSubsessionOnDestroySessionComplete, bool bWasSuccessful) \
{ \
	_Script_ColinsMultiplayerSessions_eventMultiplayerSubsessionOnDestroySessionComplete_Parms Parms; \
	Parms.bWasSuccessful=bWasSuccessful ? true : false; \
	MultiplayerSubsessionOnDestroySessionComplete.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_19_DELEGATE \
struct _Script_ColinsMultiplayerSessions_eventMultiplayerSubsessionOnStartSessionComplete_Parms \
{ \
	bool bWasSuccessful; \
}; \
static inline void FMultiplayerSubsessionOnStartSessionComplete_DelegateWrapper(const FMulticastScriptDelegate& MultiplayerSubsessionOnStartSessionComplete, bool bWasSuccessful) \
{ \
	_Script_ColinsMultiplayerSessions_eventMultiplayerSubsessionOnStartSessionComplete_Parms Parms; \
	Parms.bWasSuccessful=bWasSuccessful ? true : false; \
	MultiplayerSubsessionOnStartSessionComplete.ProcessMulticastDelegate<UObject>(&Parms); \
}


#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_SPARSE_DATA
#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_RPC_WRAPPERS
#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_RPC_WRAPPERS_NO_PURE_DECLS
#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUMultiplayerSessionSubsystem(); \
	friend struct Z_Construct_UClass_UMultiplayerSessionSubsystem_Statics; \
public: \
	DECLARE_CLASS(UMultiplayerSessionSubsystem, UGameInstanceSubsystem, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/ColinsMultiplayerSessions"), NO_API) \
	DECLARE_SERIALIZER(UMultiplayerSessionSubsystem)


#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_INCLASS \
private: \
	static void StaticRegisterNativesUMultiplayerSessionSubsystem(); \
	friend struct Z_Construct_UClass_UMultiplayerSessionSubsystem_Statics; \
public: \
	DECLARE_CLASS(UMultiplayerSessionSubsystem, UGameInstanceSubsystem, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/ColinsMultiplayerSessions"), NO_API) \
	DECLARE_SERIALIZER(UMultiplayerSessionSubsystem)


#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UMultiplayerSessionSubsystem(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UMultiplayerSessionSubsystem) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UMultiplayerSessionSubsystem); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UMultiplayerSessionSubsystem); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UMultiplayerSessionSubsystem(UMultiplayerSessionSubsystem&&); \
	NO_API UMultiplayerSessionSubsystem(const UMultiplayerSessionSubsystem&); \
public:


#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UMultiplayerSessionSubsystem(UMultiplayerSessionSubsystem&&); \
	NO_API UMultiplayerSessionSubsystem(const UMultiplayerSessionSubsystem&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UMultiplayerSessionSubsystem); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UMultiplayerSessionSubsystem); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UMultiplayerSessionSubsystem)


#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_24_PROLOG
#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_SPARSE_DATA \
	FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_RPC_WRAPPERS \
	FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_INCLASS \
	FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_SPARSE_DATA \
	FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_INCLASS_NO_PURE_DECLS \
	FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h_27_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> COLINSMULTIPLAYERSESSIONS_API UClass* StaticClass<class UMultiplayerSessionSubsystem>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_MP_Course_Plugins_ColinsMultiplayerSessions_Source_ColinsMultiplayerSessions_Public_MultiplayerSessionSubsystem_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
