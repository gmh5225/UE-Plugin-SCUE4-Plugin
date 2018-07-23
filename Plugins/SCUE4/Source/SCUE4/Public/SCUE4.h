/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//		Copyright 2016 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if PLATFORM_WINDOWS
 #include "Windows/AllowWindowsPlatformTypes.h"
 #include <Windows.h>
 #include <Winuser.h>
 #include "Windows/HideWindowsPlatformTypes.h"
#endif

#pragma once

#include "SCUE4PrivatePCH.h"
#include "Runtime/Core/Public/Misc/App.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Core/Public/Windows/WindowsPlatformProcess.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformProcess.h"
#include "Runtime/Core/Public/Misc/Paths.h"

#include "SCUE4.generated.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if PLATFORM_WINDOWS
#pragma region SETTINGS
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Default SCUE4 Settings

UCLASS(ClassGroup = Synaptech, Category = "Synaptech", config = Game)
class SCUE4_API USCUE4Settings : public UObject {
	GENERATED_UCLASS_BODY()
public:
	/** Path to the Editor's Key Generator standalone executable. */
	UPROPERTY(Category = "General Settings", config, EditAnywhere, BlueprintReadOnly)
	FString KeyGeneratorPath;
	//
	/** List of illegal process words that internal scanner shall look for. */
	UPROPERTY(Category = "General Settings", config, EditAnywhere, BlueprintReadOnly)
	TArray<FString> IllegalKeywords;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if PLATFORM_WINDOWS
#pragma endregion SETTINGS
#pragma region ENCRYPTION SYSTEM
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Encryption:: Caesar Cipher ^ Byte Mangling

static const FString ASCII_DIC = TEXT("0123456789aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ@~!#$%^&*()_-=+/|[]{}`;:,<>?.\\");
static const FString ASCII_KEY = TEXT(":.,@#}{[=^otTxmY+_!)*rACbfklpKVFWz]%Ju8ZgIi/HXGqj$>2MhPwDd1BS7-(3sEn06c5yRe?9U`L&|OvaNQ;4~<\\");

static FORCEINLINE FString FEncode(FString Input) {
	for (int I = 0; I<Input.Len(); I++) {
		int ID = -1; for (auto CH : ASCII_DIC) {
			ID++; if (Input[I]==CH) {Input[I]=ASCII_KEY[ID]; ID=-1; break;}
	}}
	//
	return *Input;
}

static FORCEINLINE FString FDecode(FString Input) {
	for (int I = 0; I<Input.Len(); I++) {
		int ID = -1; for (auto CH : ASCII_KEY) {
			ID++; if (Input[I]==CH) {Input[I]=ASCII_DIC[ID]; ID=-1; break;}
	}}
	//
	return *Input;
}

static FORCEINLINE FString FEncode(FString Key, FString Input) {
	for (int I = 0; I<Input.Len(); I++) {
		int ID = -1; for (auto CH : ASCII_DIC) {
			ID++; if (Input[I]==CH) {Input[I]=Key[ID]; ID=-1; break;}
	}}
	//
	return *Input;
}

static FORCEINLINE FString FDecode(FString Key, FString Input) {
	for (int I = 0; I<Input.Len(); I++) {
		int ID = -1; for (auto CH : Key) {
			ID++; if (Input[I]==CH) {Input[I]=ASCII_DIC[ID]; ID=-1; break;}
	}}
	//
	return *Input;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if PLATFORM_WINDOWS
#pragma endregion ENCRYPTION SYSTEM
#pragma region INTERNAL SCANNER
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Enumerator:: Internal Process Scanner

#if PLATFORM_WINDOWS
void FSCUE4_Enumerate();
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if PLATFORM_WINDOWS
#pragma endregion INTERNAL SCANNER
#pragma region CUSTOM TYPES
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Structs:: Custom 'Safe Types' for run-time data encryption

/** {S}: Safe Boolean Property;
 Use this data format to store sensible Bool values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeBool {
	GENERATED_USTRUCT_BODY()
private:

	/** Internal Key. Can be replaced each operation if wanted. */
	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Internal;

	/** Default Memory Address. */
	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueValue;

	/** Alternative Memory Address. */
	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Shift;

	/** Flag will take the value from Address, clear it and shift them,
	then copy value to previous Address and set it as default container. */
	UPROPERTY(SaveGame)
	uint8 Flag;

public:

	////////////////////////////////////////////////////////////
	/// Accessors
	
	FString GetRaw() {
		if (Shift.Len()>0) {return Shift;} else {return TrueValue;}
	}

	void SetRaw(FString* Value) {
		Shift = *Value;
		TrueValue = *Value;
	}

	/** Gets value using Global Key. */
	bool GetValue() {
		if (Internal.Len()>0) {return GetValue(&Internal);} else {
			Internal = FString(*ASCII_KEY);
			switch (Flag) {
				case 0:
					Flag = 1; Shift = TrueValue; TrueValue = NULL;
					return FCString::ToBool(*FDecode(Shift));
				case 1:
					Flag = 0; TrueValue = Shift; Shift = NULL;
					return FCString::ToBool(*FDecode(TrueValue));
			default:
				return false;
	}}}

	/** Gets value from Custom Key. */
	bool GetValue(FString* Key) {
		switch (Flag) {
			case 0:
				Flag = 1; Shift = TrueValue; TrueValue = NULL;
				return FCString::ToBool(*FDecode(*Key,Shift));
			case 1:
				Flag = 0; TrueValue = Shift; Shift = NULL;
				return FCString::ToBool(*FDecode(*Key,TrueValue));
		default:
			return false;
	}}

	/** Sets value using Internal or Global Key. */
	void SetValue(const bool Input) {
		if (Internal.Len()>0) {SetValue(&Internal,Input);} else {
			switch (Flag) {
				case 0:
					Shift = FEncode((Input?TEXT("true"):TEXT("false")));
					Flag = 1; TrueValue = NULL;
				case 1:
					TrueValue = FEncode((Input?TEXT("true"):TEXT("false")));
					Flag = 0; Shift = NULL;
	}Internal=FString(*ASCII_KEY);}}

	/** Sets value from Custom Key. */
	void SetValue(FString* Key, const bool Input) {
		switch (Flag) {
			case 0:
				Shift = FEncode(*Key,(Input?TEXT("true"):TEXT("false")));
				Flag = 1; TrueValue = NULL;
			case 1:
				TrueValue = FEncode(*Key,(Input?TEXT("true"):TEXT("false")));
				Flag = 0; Shift = NULL;
	}Internal=FString(*Key);}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeBool() {
		Internal = FString(*ASCII_KEY);
		TrueValue = TEXT("false");
		Shift = TEXT("false");
		Flag = 0;
	}

	FSafeBool(const bool Input) {
		Internal = FString(*ASCII_KEY);
		TrueValue = FEncode((Input?TEXT("true"):TEXT("false")));
		Shift = FEncode((Input?TEXT("true"):TEXT("false")));
		Flag = 0;
	}

	FSafeBool(FString* Key, const bool Input) {
		Internal = FString(*Key);
		TrueValue = FEncode(*Key,(Input?TEXT("true"):TEXT("false")));
		Shift = FEncode(*Key,(Input?TEXT("true"):TEXT("false")));
		Flag = 0;
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeBool &operator = (const FSafeBool &B) {
		Internal = B.Internal;
		TrueValue = B.TrueValue;
		Shift = B.Shift;
		Flag = B.Flag;
		return *this;
	}
	
	FORCEINLINE FSafeBool &operator = (const bool &B) {
		SetValue(B); return *this;
	}

	FORCEINLINE FArchive &operator << (FArchive &Ar) { 
		Ar << Flag;
		Ar << TrueValue;
		Ar << Shift;
		Ar << Internal;
		return Ar;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSafeBool &B) {
		return FCrc::MemCrc32(&B,sizeof(FSafeBool));
	}
};

/** {S}: Safe Byte Property;
 Use this data format to store sensible Byte values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeByte {
	GENERATED_USTRUCT_BODY()
private:

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Internal;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueValue;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Shift;

	UPROPERTY(SaveGame)
	uint8 Flag;

public:

	////////////////////////////////////////////////////////////
	/// Accessors

	FString GetRaw() {
		if (Shift.Len()>0) {return Shift;} else {return TrueValue;}
	}

	void SetRaw(FString* Value) {
		Shift = *Value;
		TrueValue = *Value;
	}

	uint8 GetValue() {
		if (Internal.Len()>0) {return GetValue(&Internal);} else {
			Internal = FString(*ASCII_KEY);
			switch (Flag) {
				case 0:
					Flag = 1; Shift = TrueValue; TrueValue = NULL;
					return FCString::Atoi(*FDecode(Shift));
				case 1:
					Flag = 0; TrueValue = Shift; Shift = NULL;
					return FCString::Atoi(*FDecode(TrueValue));
			default:
				return 0;
	}}}

	uint8 GetValue(FString* Key) {
		switch (Flag) {
			case 0:
				Flag = 1; Shift = TrueValue; TrueValue = NULL;
				return FCString::Atoi(*FDecode(*Key,Shift));
			case 1:
				Flag = 0; TrueValue = Shift; Shift = NULL;
				return FCString::Atoi(*FDecode(*Key,TrueValue));
		default:
			return 0;
	}}

	void SetValue(uint8 Input) {
		if (Internal.Len()>0) {SetValue(&Internal,Input);} else {
			switch (Flag) {
				case 0:
					Shift = FEncode(FString::FromInt(Input));
					Flag = 1; TrueValue = NULL;
				case 1:
					TrueValue = FEncode(FString::FromInt(Input));
					Flag = 0; Shift = NULL;
	}Internal=FString(*ASCII_KEY);}}

	void SetValue(FString* Key, uint8 Input) {
		switch (Flag) {
			case 0:
				Shift = FEncode(*Key,FString::FromInt(Input));
				Flag = 1; TrueValue = NULL;
			case 1:
				TrueValue = FEncode(*Key,FString::FromInt(Input));
				Flag = 0; Shift = NULL;
	}Internal=FString(*Key);}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeByte() {
		Internal = FString(*ASCII_KEY);
		TrueValue = TEXT("0");
		Shift = TEXT("0");
		Flag = 0;
	}

	FSafeByte(const uint8 Input) {
		Internal = FString(*ASCII_KEY);
		TrueValue = FEncode(FString::FromInt(Input));
		Shift = FEncode(FString::FromInt(Input));
		Flag = 0;
	}

	FSafeByte(FString* Key, const uint8 Input) {
		Internal = FString(*Key);
		TrueValue = FEncode(*Key,FString::FromInt(Input));
		Shift = FEncode(*Key,FString::FromInt(Input));
		Flag = 0;
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeByte &operator = (const FSafeByte &B) {
		Internal = B.Internal;
		TrueValue = B.TrueValue;
		Shift = B.Shift;
		Flag = B.Flag;
		return *this;
	}
	
	FORCEINLINE FSafeByte &operator = (const uint8 &B) {
		SetValue(B); return *this;
	}

	FORCEINLINE FArchive &operator << (FArchive &Ar) { 
		Ar << Flag;
		Ar << TrueValue;
		Ar << Shift;
		Ar << Internal;
		return Ar;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSafeByte &B) {
		return FCrc::MemCrc32(&B,sizeof(FSafeByte));
	}

};

/** {S}: Safe Int32 Property;
 Use this data format to store sensible Int values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeInt {
	GENERATED_USTRUCT_BODY()
private:

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Internal;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueValue;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Shift;

	UPROPERTY(SaveGame)
	uint8 Flag;

public:

	////////////////////////////////////////////////////////////
	/// Accessors

	FString GetRaw() {
		if (Shift.Len()>0) {return Shift;} else {return TrueValue;}
	}

	void SetRaw(FString* Value) {
		Shift = *Value;
		TrueValue = *Value;
	}

	int32 GetValue() {
		if (Internal.Len()>0) {return GetValue(&Internal);} else {
			Internal = FString(*ASCII_KEY);
			switch (Flag) {
				case 0:
					Flag = 1; Shift = TrueValue; TrueValue = NULL;
					return FCString::Atoi(*FDecode(Shift));
				case 1:
					Flag = 0; TrueValue = Shift; Shift = NULL;
					return FCString::Atoi(*FDecode(TrueValue));
			default:
				return 0;
	}}}

	int32 GetValue(FString* Key) {
		switch (Flag) {
			case 0:
				Flag = 1; Shift = TrueValue; TrueValue = NULL;
				return FCString::Atoi(*FDecode(*Key,Shift));
			case 1:
				Flag = 0; TrueValue = Shift; Shift = NULL;
				return FCString::Atoi(*FDecode(*Key,TrueValue));
		default:
			return 0;
	}}

	void SetValue(int32 Input) {
		if (Internal.Len()>0) {SetValue(&Internal,Input);} else {
			switch (Flag) {
				case 0:
					Shift = FEncode(FString::FromInt(Input));
					Flag = 1; TrueValue = NULL;
				case 1:
					TrueValue = FEncode(FString::FromInt(Input));
					Flag = 0; Shift = NULL;
	}Internal=FString(*ASCII_KEY);}}

	void SetValue(FString* Key, int32 Input) {
		switch (Flag) {
			case 0:
				Shift = FEncode(*Key,FString::FromInt(Input));
				Flag = 1; TrueValue = NULL;
			case 1:
				TrueValue = FEncode(*Key,FString::FromInt(Input));
				Flag = 0; Shift = NULL;
	}Internal=FString(*Key);}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeInt() {
		Internal = FString(*ASCII_KEY);
		TrueValue = TEXT("0");
		Shift = TEXT("0");
		Flag = 0;
	}

	FSafeInt(const int32 Input) {
		Internal = FString(*ASCII_KEY);
		TrueValue = FEncode(FString::FromInt(Input));
		Shift = FEncode(FString::FromInt(Input));
		Flag = 0;
	}

	FSafeInt(FString* Key, const int32 Input) {
		Internal = FString(*Key);
		TrueValue = FEncode(*Key,FString::FromInt(Input));
		Shift = FEncode(*Key,FString::FromInt(Input));
		Flag = 0;
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeInt &operator = (const FSafeInt &I) {
		Internal = I.Internal;
		TrueValue = I.TrueValue;
		Shift = I.Shift;
		Flag = I.Flag;
		return *this;
	}
	
	FORCEINLINE FSafeInt &operator = (const int32 &I) {
		SetValue(I); return *this;
	}

	FORCEINLINE FArchive &operator << (FArchive &Ar) { 
		Ar << Flag;
		Ar << TrueValue;
		Ar << Shift;
		Ar << Internal;
		return Ar;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSafeInt &I) {
		return FCrc::MemCrc32(&I,sizeof(FSafeInt));
	}

};

/** {S}: Safe Float Property;
 Use this data format to store sensible Float values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeFloat {
	GENERATED_USTRUCT_BODY()
private:

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Internal;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueValue;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Shift;

	UPROPERTY(SaveGame)
	uint8 Flag;

public:

	////////////////////////////////////////////////////////////
	/// Accessors

	FString GetRaw() {
		if (Shift.Len()>0) {return Shift;} else {return TrueValue;}
	}

	void SetRaw(FString* Value) {
		Shift = *Value;
		TrueValue = *Value;
	}

	float GetValue() {
		if (Internal.Len()>0) {return GetValue(&Internal);} else {
			Internal = FString(*ASCII_KEY);
			switch (Flag) {
				case 0:
					Flag = 1; Shift = TrueValue; TrueValue = NULL;
					return FCString::Atof(*FDecode(Shift));
				case 1:
					Flag = 0; TrueValue = Shift; Shift = NULL;
					return FCString::Atof(*FDecode(TrueValue));
			default:
				return 0.f;
	}}}

	float GetValue(FString* Key) {
		switch (Flag) {
			case 0:
				Flag = 1; Shift = TrueValue; TrueValue = NULL;
				return FCString::Atof(*FDecode(*Key,Shift));
			case 1:
				Flag = 0; TrueValue = Shift; Shift = NULL;
				return FCString::Atof(*FDecode(*Key,TrueValue));
		default:
			return 0.f;
	}}

	void SetValue(float Input) {
		if (Internal.Len()>0) {SetValue(&Internal,Input);} else {
			switch (Flag) {
				case 0:
					Shift = FEncode(FString::Printf(TEXT("%f"),Input));
					Flag = 1; TrueValue = NULL;
				case 1:
					TrueValue = FEncode(FString::Printf(TEXT("%f"),Input));
					Flag = 0; Shift = NULL;
	}Internal=FString(*ASCII_KEY);}}

	void SetValue(FString* Key, float Input) {
		switch (Flag) {
			case 0:
				Shift = FEncode(*Key,FString::Printf(TEXT("%f"),Input));
				Flag = 1; TrueValue = NULL;
			case 1:
				TrueValue = FEncode(*Key,FString::Printf(TEXT("%f"),Input));
				Flag = 0; Shift = NULL;
	}Internal=FString(*Key);}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeFloat() {
		Internal = FString(*ASCII_KEY);
		TrueValue = TEXT("0.0");
		Shift = TEXT("0.0");
		Flag = 0;
	}

	FSafeFloat(const float Input) {
		Internal = FString(*ASCII_KEY);
		TrueValue = FEncode(FString::Printf(TEXT("%f"),Input));
		Shift = FEncode(FString::Printf(TEXT("%f"),Input));
		Flag = 0;
	}

	FSafeFloat(FString* Key, const float Input) {
		Internal = FString(*Key);
		TrueValue = FEncode(*Key,FString::Printf(TEXT("%f"),Input));
		Shift = FEncode(*Key,FString::Printf(TEXT("%f"),Input));
		Flag = 0;
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeFloat &operator = (const FSafeFloat &F) {
		Internal = F.Internal;
		TrueValue = F.TrueValue;
		Shift = F.Shift;
		Flag = F.Flag;
		return *this;
	}
	
	FORCEINLINE FSafeFloat &operator = (const float &F) {
		SetValue(F); return *this;
	}

	FORCEINLINE FArchive &operator << (FArchive &Ar) { 
		Ar << Flag;
		Ar << TrueValue;
		Ar << Shift;
		Ar << Internal;
		return Ar;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSafeFloat &F) {
		return FCrc::MemCrc32(&F,sizeof(FSafeFloat));
	}

};

/** {S}: Safe Name Property;
 Use this data format to store sensible Name values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeName {
	GENERATED_USTRUCT_BODY()
private:

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Internal;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueValue;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Shift;

	UPROPERTY(SaveGame)
	uint8 Flag;

public:

	////////////////////////////////////////////////////////////
	/// Accessors

	FString GetRaw() {
		if (Shift.Len()>0) {return Shift;} else {return TrueValue;}
	}

	void SetRaw(FString* Value) {
		Shift = *Value;
		TrueValue = *Value;
	}

	FName GetValue() {
		if (Internal.Len()>0) {return GetValue(&Internal);} else {
			Internal = FString(*ASCII_KEY);
			switch (Flag) {
			case 0:
				Flag = 1; Shift = TrueValue; TrueValue = NULL;
				return FName::FName(*FDecode(Shift));
			case 1:
				Flag = 0; TrueValue = Shift; Shift = NULL;
				return FName::FName(*FDecode(TrueValue));
			default:
				return TEXT("");
	}}}

	FName GetValue(FString* Key) {
		switch (Flag) {
			case 0:
				Flag = 1; Shift = TrueValue; TrueValue = NULL;
				return FName::FName(*FDecode(*Key,Shift));
			case 1:
				Flag = 0; TrueValue = Shift; Shift = NULL;
				return FName::FName(*FDecode(*Key,TrueValue));
		default:
			return TEXT("");
	}}

	void SetValue(FName Input) {
		if (Internal.Len()>0) {SetValue(&Internal,Input);} else {
			switch (Flag) {
				case 0:
					Shift = *FEncode(Input.ToString());
					Flag = 1; TrueValue = NULL;
				case 1:
					TrueValue = *FEncode(Input.ToString());
					Flag = 0; Shift = NULL;
	}Internal=FString(*ASCII_KEY);}}

	void SetValue(FString* Key, FName Input) {
		switch (Flag) {
			case 0:
				Shift = *FEncode(*Key,Input.ToString());
				Flag = 1; TrueValue = NULL;
			case 1:
				TrueValue = *FEncode(*Key,Input.ToString());
				Flag = 0; Shift = NULL;
	}Internal=FString(*Key);}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeName() {
		Internal = FString(*ASCII_KEY);
		TrueValue = TEXT("");
		Shift = TEXT("");
		Flag = 0;
	}

	FSafeName(const FName Input) {
		Internal = FString(*ASCII_KEY);
		TrueValue = *FEncode(Input.ToString());
		Shift = *FEncode(Input.ToString());
		Flag = 0;
	}

	FSafeName(FString* Key, const FName Input) {
		Internal = FString(*Key);
		TrueValue = *FEncode(*Key,Input.ToString());
		Shift = *FEncode(*Key,Input.ToString());
		Flag = 0;
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeName &operator = (const FSafeName &N) {
		Internal = N.Internal;
		TrueValue = N.TrueValue;
		Shift = N.Shift;
		Flag = N.Flag;
		return *this;
	}
	
	FORCEINLINE FSafeName &operator = (const FName &N) {
		SetValue(N); return *this;
	}

	FORCEINLINE FArchive &operator << (FArchive &Ar) { 
		Ar << Flag;
		Ar << TrueValue;
		Ar << Shift;
		Ar << Internal;
		return Ar;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSafeName &N) {
		return FCrc::MemCrc32(&N,sizeof(FSafeName));
	}

};

/** {S}: Safe String Property;
 Use this data format to store sensible String values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeString {
	GENERATED_USTRUCT_BODY()
private:

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Internal;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueValue;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Shift;

	UPROPERTY(SaveGame)
	uint8 Flag;

public:

	////////////////////////////////////////////////////////////
	/// Accessors

	FString GetRaw() {
		if (Shift.Len()>0) {return Shift;} else {return TrueValue;}
	}

	void SetRaw(FString* Value) {
		Shift = *Value;
		TrueValue = *Value;
	}

	FString GetValue() {
		if (Internal.Len()>0) {return GetValue(&Internal);} else {
			Internal = FString(*ASCII_KEY);
			switch (Flag) {
				case 0:
					Flag = 1; Shift = TrueValue; TrueValue = NULL;
					return FString::FString(*FDecode(Shift));
				case 1:
					Flag = 0; TrueValue = Shift; Shift = NULL;
					return FString::FString(*FDecode(TrueValue));
			default:
				return TEXT("");
	}}}

	FString GetValue(FString* Key) {
		switch (Flag) {
			case 0:
				Flag = 1; Shift = TrueValue; TrueValue = NULL;
				return FString::FString(*FDecode(*Key,Shift));
			case 1:
				Flag = 0; TrueValue = Shift; Shift = NULL;
				return FString::FString(*FDecode(*Key,TrueValue));
		default:
			return TEXT("");
	}}

	void SetValue(FString Input) {
		if (Internal.Len()>0) {SetValue(&Internal,Input);} else {
			switch (Flag) {
				case 0:
					Shift = *FEncode(Input);
					Flag = 1; TrueValue = NULL;
				case 1:
					TrueValue = *FEncode(Input);
					Flag = 0; Shift = NULL;
	}Internal=FString(*ASCII_KEY);}}

	void SetValue(FString* Key, FString Input) {
		switch (Flag) {
			case 0:
				Shift = *FEncode(*Key,Input);
				Flag = 1; TrueValue = NULL;
			case 1:
				TrueValue = *FEncode(*Key,Input);
				Flag = 0; Shift = NULL;
	}Internal=FString(*Key);}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeString() {
		Internal = FString(*ASCII_KEY);
		TrueValue = TEXT("");
		Shift = TEXT("");
		Flag = 0;
	}

	FSafeString(const FString Input) {
		Internal = FString(*ASCII_KEY);
		TrueValue = *FEncode(Input);
		Shift = *FEncode(Input);
		Flag = 0;
	}

	FSafeString(FString* Key, const FString Input) {
		Internal = FString(*Key);
		TrueValue = *FEncode(*Key,Input);
		Shift = *FEncode(*Key,Input);
		Flag = 0;
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeString &operator = (const FSafeString &S) {
		Internal = S.Internal;
		TrueValue = S.TrueValue;
		Shift = S.Shift;
		Flag = S.Flag;
		return *this;
	}
	
	FORCEINLINE FSafeString &operator = (const FString &S) {
		SetValue(S); return *this;
	}

	FORCEINLINE FArchive &operator << (FArchive &Ar) { 
		Ar << Flag;
		Ar << TrueValue;
		Ar << Shift;
		Ar << Internal;
		return Ar;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSafeString &S) {
		return FCrc::MemCrc32(&S,sizeof(FSafeString));
	}

};

/** {S}: Safe Text Property;
 Use this data format to store sensible Text values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeText {
	GENERATED_USTRUCT_BODY()
private:

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Internal;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueValue;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Shift;

	UPROPERTY(SaveGame)
	uint8 Flag;

public:

	////////////////////////////////////////////////////////////
	/// Accessors

	FString GetRaw() {
		if (Shift.Len()>0) {return Shift;} else {return TrueValue;}
	}

	void SetRaw(FString* Value) {
		Shift = *Value;
		TrueValue = *Value;
	}

	FText GetValue() {
		if (Internal.Len()>0) {return GetValue(&Internal);} else {
			Internal = FString(*ASCII_KEY);
			switch (Flag) {
				case 0:
					Flag = 1; Shift = TrueValue; TrueValue = NULL;
					return FText::FromString(FDecode(Shift));
				case 1:
					Flag = 0; TrueValue = Shift; Shift = NULL;
					return FText::FromString(FDecode(TrueValue));
			default:
				return FText::GetEmpty();
	}}}

	FText GetValue(FString* Key) {
		switch (Flag) {
			case 0:
				Flag = 1; Shift = TrueValue; TrueValue = NULL;
				return FText::FromString(FDecode(*Key,Shift));
			case 1:
				Flag = 0; TrueValue = Shift; Shift = NULL;
				return FText::FromString(FDecode(*Key,TrueValue));
		default:
			return FText::GetEmpty();
	}}

	void SetValue(FText Input) {
		if (Internal.Len()>0) {SetValue(&Internal,Input);} else {
			switch (Flag) {
				case 0:
					Shift = *FEncode(Input.ToString());
					Flag = 1; TrueValue = NULL;
				case 1:
					TrueValue = *FEncode(Input.ToString());
					Flag = 0; Shift = NULL;
	}Internal=FString(*ASCII_KEY);}}

	void SetValue(FString* Key, FText Input) {
		switch (Flag) {
			case 0:
				Shift = *FEncode(*Key,Input.ToString());
				Flag = 1; TrueValue = NULL;
			case 1:
				TrueValue = *FEncode(*Key,Input.ToString());
				Flag = 0; Shift = NULL;
	}Internal=FString(*Key);}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeText() {
		Internal = FString(*ASCII_KEY);
		TrueValue = TEXT("");
		Shift = TEXT("");
		Flag = 0;
	}

	FSafeText(const FText Input) {
		Internal = FString(*ASCII_KEY);
		TrueValue = *FEncode(Input.ToString());
		Shift = *FEncode(Input.ToString());
		Flag = 0;
	}

	FSafeText(FString* Key, const FText Input) {
		Internal = FString(*Key);
		TrueValue = *FEncode(*Key,Input.ToString());
		Shift = *FEncode(*Key,Input.ToString());
		Flag = 0;
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeText &operator = (const FSafeText &T) {
		Internal = T.Internal;
		TrueValue = T.TrueValue;
		Shift = T.Shift;
		Flag = T.Flag;
		return *this;
	}
	
	FORCEINLINE FSafeText &operator = (const FText &T) {
		SetValue(T); return *this;
	}

	FORCEINLINE FArchive &operator << (FArchive &Ar) { 
		Ar << Flag;
		Ar << TrueValue;
		Ar << Shift;
		Ar << Internal;
		return Ar;
	}

};

/** {S}: Safe Vector2D Property;
 Use this data format to store sensible Vector2D values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeVector2D {
	GENERATED_USTRUCT_BODY()
private:

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Internal;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueX;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueY;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftX;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftY;

	UPROPERTY(SaveGame)
	uint8 Flag;

public:

	////////////////////////////////////////////////////////////
	/// Accessors

	FVector2D GetValue() {
		if (Internal.Len()>0) {return GetValue(&Internal);} else {
			Internal = FString(*ASCII_KEY);
			switch (Flag) {
				case 0:
					Flag = 1;
					ShiftX = TrueX; ShiftY = TrueY;
					TrueX = NULL; TrueY = NULL;
					return FVector2D::FVector2D(FCString::Atof(*FDecode(ShiftX)),FCString::Atof(*FDecode(ShiftY)));
				case 1:
					Flag = 0;
					TrueX = ShiftX; TrueY = ShiftY;
					ShiftX = NULL; ShiftY = NULL;
					return FVector2D::FVector2D(FCString::Atof(*FDecode(TrueX)),FCString::Atof(*FDecode(TrueY)));
			default:
				return FVector2D::ZeroVector;
	}}}

	FVector2D GetValue(FString* Key) {
		switch (Flag) {
			case 0:
				Flag = 1;
				ShiftX = TrueX; ShiftY = TrueY;
				TrueX = NULL; TrueY = NULL;
				return FVector2D::FVector2D(FCString::Atof(*FDecode(*Key,ShiftX)),FCString::Atof(*FDecode(*Key,ShiftY)));
			case 1:
				Flag = 0;
				TrueX = ShiftX; TrueY = ShiftY;
				ShiftX = NULL; ShiftY = NULL;
				return FVector2D::FVector2D(FCString::Atof(*FDecode(*Key,TrueX)),FCString::Atof(*FDecode(*Key,TrueY)));
		default:
			return FVector2D::ZeroVector;
	}}

	void SetValue(FVector2D Input) {
		if (Internal.Len()>0) {SetValue(&Internal,Input);} else {
			switch (Flag) {
				case 0:
					Flag = 1;
					ShiftX = *FEncode(FString::Printf(TEXT("%f"),Input.X));
					ShiftY = *FEncode(FString::Printf(TEXT("%f"),Input.Y));
					TrueX = NULL; TrueY = NULL;
				case 1:
					Flag = 0;
					TrueX = *FEncode(FString::Printf(TEXT("%f"),Input.X));
					TrueY = *FEncode(FString::Printf(TEXT("%f"),Input.Y));
					ShiftX = NULL; ShiftY = NULL;
	}Internal=FString(*ASCII_KEY);}}

	void SetValue(FString* Key, FVector2D Input) {
		switch (Flag) {
			case 0:
				Flag = 1;
				ShiftX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.X));
				ShiftY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Y));
				TrueX = NULL; TrueY = NULL;
			case 1:
				Flag = 0;
				TrueX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.X));
				TrueY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Y));
				ShiftX = NULL; ShiftY = NULL;
	}Internal=FString(*Key);}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeVector2D() {
		Internal = FString(*ASCII_KEY);
		TrueX = TEXT(""); TrueY = TEXT("");
		ShiftX = TEXT(""); ShiftY = TEXT("");
		Flag = 0;
	}

	FSafeVector2D(const float X, const float Y) {
		Internal = FString(*ASCII_KEY);
		TrueX = *FEncode(FString::Printf(TEXT("%f"),X));
		TrueY = *FEncode(FString::Printf(TEXT("%f"),Y));
		ShiftX = *FEncode(FString::Printf(TEXT("%f"),X));
		ShiftY = *FEncode(FString::Printf(TEXT("%f"),Y));
		Flag = 0;
	}

	FSafeVector2D(const FVector2D Input) {
		Internal = FString(*ASCII_KEY);
		TrueX = *FEncode(FString::Printf(TEXT("%f"),Input.X));
		TrueY = *FEncode(FString::Printf(TEXT("%f"),Input.Y));
		ShiftX = *FEncode(FString::Printf(TEXT("%f"),Input.X));
		ShiftY = *FEncode(FString::Printf(TEXT("%f"),Input.Y));
		Flag = 0;
	}

	FSafeVector2D(FString* Key, const FVector2D Input) {
		Internal = FString(*Key);
		TrueX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.X));
		TrueY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Y));
		ShiftX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.X));
		ShiftY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Y));
		Flag = 0;
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeVector2D &operator = (const FSafeVector2D &V) {
		Internal = V.Internal;
		TrueX = V.TrueX; TrueY = V.TrueY;
		ShiftX = V.ShiftX; ShiftY = V.ShiftY;
		Flag = V.Flag;
		return *this;
	}
	
	FORCEINLINE FSafeVector2D &operator = (const FVector2D &V) {
		SetValue(V); return *this;
	}

	FORCEINLINE FArchive &operator << (FArchive &Ar) { 
		Ar << Flag;
		Ar << TrueX;
		Ar << TrueY;
		Ar << ShiftX;
		Ar << ShiftY;
		Ar << Internal;
		return Ar;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSafeVector2D &V) {
		return FCrc::MemCrc32(&V,sizeof(FSafeVector2D));
	}

};

/** {S}: Safe Vector3D Property;
 Use this data format to store sensible Vector3D values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeVector3D {
	GENERATED_USTRUCT_BODY()
private:

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Internal;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueX;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueY;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueZ;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftX;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftY;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftZ;

	UPROPERTY(SaveGame)
	uint8 Flag;

public:

	////////////////////////////////////////////////////////////

	FVector GetValue() {
		if (Internal.Len()>0) {return GetValue(&Internal);} else {
			Internal = FString(*ASCII_KEY);
			switch (Flag) {
				case 0:
					Flag = 1;
					ShiftX = TrueX; ShiftY = TrueY; ShiftZ = TrueZ;
					TrueX = NULL; TrueY = NULL; TrueZ = NULL;
					return FVector::FVector(FCString::Atof(*FDecode(ShiftX)),FCString::Atof(*FDecode(ShiftY)),FCString::Atof(*FDecode(ShiftZ)));
				case 1:
					Flag = 0;
					TrueX = ShiftX; TrueY = ShiftY; TrueZ = ShiftZ;
					ShiftX = NULL; ShiftY = NULL; ShiftZ = NULL;
					return FVector::FVector(FCString::Atof(*FDecode(TrueX)),FCString::Atof(*FDecode(TrueY)),FCString::Atof(*FDecode(TrueZ)));
			default:
				return FVector::ZeroVector;
	}}}

	FVector GetValue(FString* Key) {
		switch (Flag) {
			case 0:
				Flag = 1;
				ShiftX = TrueX; ShiftY = TrueY; ShiftZ = TrueZ;
				TrueX = NULL; TrueY = NULL; TrueZ = NULL;
				return FVector::FVector(FCString::Atof(*FDecode(*Key,ShiftX)),FCString::Atof(*FDecode(*Key,ShiftY)),FCString::Atof(*FDecode(*Key,ShiftZ)));
			case 1:
				Flag = 0;
				TrueX = ShiftX; TrueY = ShiftY; TrueZ = ShiftZ;
				ShiftX = NULL; ShiftY = NULL; ShiftZ = NULL;
				return FVector::FVector(FCString::Atof(*FDecode(*Key,TrueX)),FCString::Atof(*FDecode(*Key,TrueY)),FCString::Atof(*FDecode(*Key,TrueZ)));
		default:
			return FVector::ZeroVector;
	}}

	void SetValue(FVector Input) {
		if (Internal.Len()>0) {SetValue(&Internal,Input);} else {
			switch (Flag) {
				case 0:
					Flag = 1;
					ShiftX = *FEncode(FString::Printf(TEXT("%f"),Input.X));
					ShiftY = *FEncode(FString::Printf(TEXT("%f"),Input.Y));
					ShiftZ = *FEncode(FString::Printf(TEXT("%f"),Input.Z));
					TrueX = NULL; TrueY = NULL; TrueZ = NULL;
				case 1:
					Flag = 0;
					TrueX = *FEncode(FString::Printf(TEXT("%f"),Input.X));
					TrueY = *FEncode(FString::Printf(TEXT("%f"),Input.Y));
					TrueZ = *FEncode(FString::Printf(TEXT("%f"),Input.Z));
					ShiftX = NULL; ShiftY = NULL; ShiftZ = NULL;
	}Internal=FString(*ASCII_KEY);}}

	void SetValue(FString* Key, FVector Input) {
		switch (Flag) {
			case 0:
				Flag = 1;
				ShiftX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.X));
				ShiftY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Y));
				ShiftZ = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Z));
				TrueX = NULL; TrueY = NULL; TrueZ = NULL;
			case 1:
				Flag = 0;
				TrueX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.X));
				TrueY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Y));
				TrueZ = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Z));
				ShiftX = NULL; ShiftY = NULL; ShiftZ = NULL;
	}Internal=FString(*Key);}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeVector3D() {
		Internal = FString(*ASCII_KEY);
		TrueX = TEXT(""); TrueY = TEXT(""); TrueZ = TEXT("");
		ShiftX = TEXT(""); ShiftY = TEXT(""); ShiftZ = TEXT("");
		Flag = 0;
	}

	FSafeVector3D(const float X, const float Y, const float Z) {
		Internal = FString(*ASCII_KEY);
		TrueX = *FEncode(FString::Printf(TEXT("%f"),X));
		TrueY = *FEncode(FString::Printf(TEXT("%f"),Y));
		TrueZ = *FEncode(FString::Printf(TEXT("%f"),Z));
		ShiftX = *FEncode(FString::Printf(TEXT("%f"),X));
		ShiftY = *FEncode(FString::Printf(TEXT("%f"),Y));
		ShiftZ = *FEncode(FString::Printf(TEXT("%f"),Z));
		Flag = 0;
	}

	FSafeVector3D(const FVector Input) {
		Internal = FString(*ASCII_KEY);
		TrueX = *FEncode(FString::Printf(TEXT("%f"),Input.X));
		TrueY = *FEncode(FString::Printf(TEXT("%f"),Input.Y));
		TrueZ = *FEncode(FString::Printf(TEXT("%f"),Input.Z));
		ShiftX = *FEncode(FString::Printf(TEXT("%f"),Input.X));
		ShiftY = *FEncode(FString::Printf(TEXT("%f"),Input.Y));
		ShiftZ = *FEncode(FString::Printf(TEXT("%f"),Input.Z));
		Flag = 0;
	}

	FSafeVector3D(FString* Key, const FVector Input) {
		Internal = FString(*Key);
		TrueX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.X));
		TrueY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Y));
		TrueZ = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Z));
		ShiftX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.X));
		ShiftY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Y));
		ShiftZ = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Z));
		Flag = 0;
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeVector3D &operator = (const FSafeVector3D &V) {
		Internal = V.Internal;
		TrueX = V.TrueX; TrueY = V.TrueY; TrueZ = V.TrueZ;
		ShiftX = V.ShiftX; ShiftY = V.ShiftY; ShiftZ = V.ShiftZ;
		Flag = V.Flag;
		return *this;
	}
	
	FORCEINLINE FSafeVector3D &operator = (const FVector &V) {
		SetValue(V); return *this;
	}

	FORCEINLINE FArchive &operator << (FArchive &Ar) { 
		Ar << Flag;
		Ar << TrueX;
		Ar << TrueY;
		Ar << TrueZ;
		Ar << ShiftX;
		Ar << ShiftY;
		Ar << ShiftZ;
		Ar << Internal;
		return Ar;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSafeVector3D &V) {
		return FCrc::MemCrc32(&V,sizeof(FSafeVector3D));
	}

};

/** {S}: Safe Vector4D Property;
 Use this data format to store sensible Vector4D values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeVector4D {
	GENERATED_USTRUCT_BODY()
private:

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Internal;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueX;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueY;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueZ;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueW;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftX;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftY;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftZ;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftW;

	UPROPERTY(SaveGame)
	uint8 Flag;

public:

	////////////////////////////////////////////////////////////
	/// Accessors

	FVector4 GetValue() {
		if (Internal.Len()>0) {return GetValue(&Internal);} else {
			Internal = FString(*ASCII_KEY);
			switch (Flag) {
				case 0:
					Flag = 1;
					ShiftX = TrueX; ShiftY = TrueY; ShiftZ = TrueZ; ShiftW = TrueW;
					TrueX = NULL; TrueY = NULL; TrueZ = NULL; TrueW = NULL;
					return FVector4::FVector4(FCString::Atof(*FDecode(ShiftX)),FCString::Atof(*FDecode(ShiftY)),FCString::Atof(*FDecode(ShiftZ)),FCString::Atof(*FDecode(ShiftW)));
				case 1:
					Flag = 0;
					TrueX = ShiftX; TrueY = ShiftY; TrueZ = ShiftZ; TrueW = ShiftW;
					ShiftX = NULL; ShiftY = NULL; ShiftZ = NULL; ShiftW = NULL;
					return FVector4::FVector4(FCString::Atof(*FDecode(TrueX)),FCString::Atof(*FDecode(TrueY)),FCString::Atof(*FDecode(TrueZ)),FCString::Atof(*FDecode(TrueW)));
			default:
				return FVector4::FVector4(FVector2D::ZeroVector,FVector2D::ZeroVector);
	}}}

	FVector4 GetValue(FString* Key) {
		switch (Flag) {
			case 0:
				Flag = 1;
				ShiftX = TrueX; ShiftY = TrueY; ShiftZ = TrueZ; ShiftW = TrueW;
				TrueX = NULL; TrueY = NULL; TrueZ = NULL; TrueW = NULL;
				return FVector4::FVector4(FCString::Atof(*FDecode(*Key,ShiftX)),FCString::Atof(*FDecode(*Key,ShiftY)),FCString::Atof(*FDecode(*Key,ShiftZ)),FCString::Atof(*FDecode(*Key,ShiftW)));
			case 1:
				Flag = 0;
				TrueX = ShiftX; TrueY = ShiftY; TrueZ = ShiftZ; TrueW = ShiftW;
				ShiftX = NULL; ShiftY = NULL; ShiftZ = NULL; ShiftW = NULL;
				return FVector4::FVector4(FCString::Atof(*FDecode(*Key,TrueX)),FCString::Atof(*FDecode(*Key,TrueY)),FCString::Atof(*FDecode(*Key,TrueZ)),FCString::Atof(*FDecode(*Key,TrueW)));
		default:
			return FVector4::FVector4(FVector2D::ZeroVector,FVector2D::ZeroVector);
	}}

	void SetValue(FVector4* Input) {
		if (Internal.Len()>0) {SetValue(&Internal,Input);} else {
			switch (Flag) {
				case 0:
					Flag = 1;
					ShiftX = *FEncode(FString::Printf(TEXT("%f"),Input->X));
					ShiftY = *FEncode(FString::Printf(TEXT("%f"),Input->Y));
					ShiftZ = *FEncode(FString::Printf(TEXT("%f"),Input->Z));
					ShiftW = *FEncode(FString::Printf(TEXT("%f"),Input->W));
					TrueX = NULL; TrueY = NULL; TrueZ = NULL; TrueW = NULL;
				case 1:
					Flag = 0;
					TrueX = *FEncode(FString::Printf(TEXT("%f"),Input->X));
					TrueY = *FEncode(FString::Printf(TEXT("%f"),Input->Y));
					TrueZ = *FEncode(FString::Printf(TEXT("%f"),Input->Z));
					TrueW = *FEncode(FString::Printf(TEXT("%f"),Input->W));
					ShiftX = NULL; ShiftY = NULL; ShiftZ = NULL; ShiftW = NULL;
	}Internal=FString(*ASCII_KEY);}}

	void SetValue(FString* Key, FVector4* Input) {
		switch (Flag) {
			case 0:
				Flag = 1;
				ShiftX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->X));
				ShiftY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->Y));
				ShiftZ = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->Z));
				ShiftW = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->W));
				TrueX = NULL; TrueY = NULL; TrueZ = NULL; TrueW = NULL;
			case 1:
				Flag = 0;
				TrueX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->X));
				TrueY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->Y));
				TrueZ = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->Z));
				TrueW = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->W));
				ShiftX = NULL; ShiftY = NULL; ShiftZ = NULL; ShiftW = NULL;
	}Internal=FString(*Key);}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeVector4D() {
		Internal = FString(*ASCII_KEY);
		TrueX = TEXT(""); TrueY = TEXT(""); TrueZ = TEXT(""); TrueW = TEXT("");
		ShiftX = TEXT(""); ShiftY = TEXT(""); ShiftZ = TEXT(""); ShiftW = TEXT("");
		Flag = 0;
	}

	FSafeVector4D(const float X, const float Y, const float Z, const float W) {
		Internal = FString(*ASCII_KEY);
		TrueX = *FEncode(FString::Printf(TEXT("%f"),X));
		TrueY = *FEncode(FString::Printf(TEXT("%f"),Y));
		TrueZ = *FEncode(FString::Printf(TEXT("%f"),Z));
		TrueW = *FEncode(FString::Printf(TEXT("%f"),W));
		ShiftX = *FEncode(FString::Printf(TEXT("%f"),X));
		ShiftY = *FEncode(FString::Printf(TEXT("%f"),Y));
		ShiftZ = *FEncode(FString::Printf(TEXT("%f"),Z));
		ShiftW = *FEncode(FString::Printf(TEXT("%f"),W));
		Flag = 0;
	}

	FSafeVector4D(FVector4* Input) {
		Internal = FString(*ASCII_KEY);
		TrueX = *FEncode(FString::Printf(TEXT("%f"),Input->X));
		TrueY = *FEncode(FString::Printf(TEXT("%f"),Input->Y));
		TrueZ = *FEncode(FString::Printf(TEXT("%f"),Input->Z));
		TrueW = *FEncode(FString::Printf(TEXT("%f"),Input->W));
		ShiftX = *FEncode(FString::Printf(TEXT("%f"),Input->X));
		ShiftY = *FEncode(FString::Printf(TEXT("%f"),Input->Y));
		ShiftZ = *FEncode(FString::Printf(TEXT("%f"),Input->Z));
		ShiftW = *FEncode(FString::Printf(TEXT("%f"),Input->W));
		Flag = 0;
	}

	FSafeVector4D(FString* Key, FVector4* Input) {
		Internal = FString(*Key);
		TrueX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->X));
		TrueY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->Y));
		TrueZ = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->Z));
		TrueW = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->W));
		ShiftX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->X));
		ShiftY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->Y));
		ShiftZ = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->Z));
		ShiftW = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->W));
		Flag = 0;
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeVector4D &operator = (const FSafeVector4D &V) {
		Internal = V.Internal;
		TrueX = V.TrueX; TrueY = V.TrueY; TrueZ = V.TrueZ; TrueW = V.TrueW;
		ShiftX = V.ShiftX; ShiftY = V.ShiftY; ShiftZ = V.ShiftZ; ShiftW = V.ShiftW;
		Flag = V.Flag;
		return *this;
	}
	
	FORCEINLINE FSafeVector4D &operator = (FVector4 &V) {
		SetValue(&V); return *this;
	}

	FORCEINLINE FArchive &operator << (FArchive &Ar) { 
		Ar << Flag;
		Ar << TrueX;
		Ar << TrueY;
		Ar << TrueZ;
		Ar << TrueW;
		Ar << ShiftX;
		Ar << ShiftY;
		Ar << ShiftZ;
		Ar << ShiftW;
		Ar << Internal;
		return Ar;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSafeVector4D &V) {
		return FCrc::MemCrc32(&V,sizeof(FSafeVector4D));
	}

};

/** {S}: Safe Color Property;
 Use this data format to store sensible Linear Color values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeColor {
	GENERATED_USTRUCT_BODY()
private:

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Internal;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueR;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueG;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueB;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueA;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftR;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftG;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftB;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftA;

	UPROPERTY(SaveGame)
	uint8 Flag;

public:

	////////////////////////////////////////////////////////////
	/// Accessors

	FLinearColor GetValue() {
		if (Internal.Len()>0) {return GetValue(&Internal);} else {
			Internal = FString(*ASCII_KEY);
			switch (Flag) {
				case 0:
					Flag = 1;
					ShiftR = TrueR; ShiftG = TrueG; ShiftB = TrueB; ShiftA = TrueA;
					TrueR = NULL; TrueG = NULL; TrueB = NULL; TrueA = NULL;
					return FLinearColor::FLinearColor(FCString::Atof(*FDecode(ShiftR)),FCString::Atof(*FDecode(ShiftG)),FCString::Atof(*FDecode(ShiftB)),FCString::Atof(*FDecode(ShiftA)));
				case 1:
					Flag = 0;
					TrueR = ShiftR; TrueG = ShiftG; TrueB = ShiftB; TrueA = ShiftA;
					ShiftR = NULL; ShiftG = NULL; ShiftB = NULL; ShiftA = NULL;
					return FLinearColor::FLinearColor(FCString::Atof(*FDecode(TrueR)),FCString::Atof(*FDecode(TrueG)),FCString::Atof(*FDecode(TrueB)),FCString::Atof(*FDecode(TrueA)));
			default:
				return FLinearColor::FLinearColor();
	}}}

	FLinearColor GetValue(FString* Key) {
		switch (Flag) {
			case 0:
				Flag = 1;
				ShiftR = TrueR; ShiftG = TrueG; ShiftB = TrueB; ShiftA = TrueA;
				TrueR = NULL; TrueG = NULL; TrueB = NULL; TrueA = NULL;
				return FLinearColor::FLinearColor(FCString::Atof(*FDecode(*Key,ShiftR)),FCString::Atof(*FDecode(*Key,ShiftG)),FCString::Atof(*FDecode(*Key,ShiftB)),FCString::Atof(*FDecode(*Key,ShiftA)));
			case 1:
				Flag = 0;
				TrueR = ShiftR; TrueG = ShiftG; TrueB = ShiftB; TrueA = ShiftA;
				ShiftR = NULL; ShiftG = NULL; ShiftB = NULL; ShiftA = NULL;
				return FLinearColor::FLinearColor(FCString::Atof(*FDecode(*Key,TrueR)),FCString::Atof(*FDecode(*Key,TrueG)),FCString::Atof(*FDecode(*Key,TrueB)),FCString::Atof(*FDecode(*Key,TrueA)));
		default:
			return FLinearColor::FLinearColor();
	}}

	void SetValue(FLinearColor* Input) {
		if (Internal.Len()>0) {SetValue(&Internal,Input);} else {
			switch (Flag) {
				case 0:
					Flag = 1;
					ShiftR = *FEncode(FString::Printf(TEXT("%f"),Input->R));
					ShiftG = *FEncode(FString::Printf(TEXT("%f"),Input->G));
					ShiftB = *FEncode(FString::Printf(TEXT("%f"),Input->B));
					ShiftA = *FEncode(FString::Printf(TEXT("%f"),Input->A));
					TrueR = NULL; TrueG = NULL; TrueB = NULL; TrueA = NULL;
				case 1:
					Flag = 0;
					TrueR = *FEncode(FString::Printf(TEXT("%f"),Input->R));
					TrueG = *FEncode(FString::Printf(TEXT("%f"),Input->G));
					TrueB = *FEncode(FString::Printf(TEXT("%f"),Input->B));
					TrueA = *FEncode(FString::Printf(TEXT("%f"),Input->A));
					ShiftR = NULL; ShiftG = NULL; ShiftB = NULL; ShiftA = NULL;
	}Internal=FString(*ASCII_KEY);}}

	void SetValue(FString* Key, FLinearColor* Input) {
		switch (Flag) {
			case 0:
				Flag = 1;
				ShiftR = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->R));
				ShiftG = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->G));
				ShiftB = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->B));
				ShiftA = *FEncode(FString::Printf(TEXT("%f"),Input->A));
				TrueR = NULL; TrueG = NULL; TrueB = NULL; TrueA = NULL;
			case 1:
				Flag = 0;
				TrueR = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->R));
				TrueG = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->G));
				TrueB = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->B));
				TrueA = *FEncode(*Key,FString::Printf(TEXT("%f"),Input->A));
				ShiftR = NULL; ShiftG = NULL; ShiftB = NULL; ShiftA = NULL;
	}Internal=FString(*Key);}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeColor() {
		Internal = FString(*ASCII_KEY);
		TrueR = TEXT(""); TrueG = TEXT(""); TrueB = TEXT(""); TrueA = TEXT("");
		ShiftR = TEXT(""); ShiftG = TEXT(""); ShiftB = TEXT(""); ShiftA = TEXT("");
		Flag = 0;
	}

	FSafeColor(const FLinearColor Input) {
		Internal = FString(*ASCII_KEY);
		TrueR = *FEncode(FString::Printf(TEXT("%f"),Input.R));
		TrueG = *FEncode(FString::Printf(TEXT("%f"),Input.G));
		TrueB = *FEncode(FString::Printf(TEXT("%f"),Input.B));
		TrueA = *FEncode(FString::Printf(TEXT("%f"),Input.A));
		ShiftR = *FEncode(FString::Printf(TEXT("%f"),Input.R));
		ShiftG = *FEncode(FString::Printf(TEXT("%f"),Input.G));
		ShiftB = *FEncode(FString::Printf(TEXT("%f"),Input.B));
		ShiftA = *FEncode(FString::Printf(TEXT("%f"),Input.A));
		Flag = 0;
	}

	FSafeColor(FString* Key, const FLinearColor Input) {
		Internal = FString(*Key);
		TrueR = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.R));
		TrueG = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.G));
		TrueB = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.B));
		TrueA = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.A));
		ShiftR = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.R));
		ShiftG = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.G));
		ShiftB = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.B));
		ShiftA = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.A));
		Flag = 0;
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeColor &operator = (const FSafeColor &C) {
		Internal = C.Internal;
		TrueR = C.TrueR; TrueG = C.TrueG; TrueB = C.TrueB; TrueA = C.TrueA;
		ShiftR = C.ShiftR; ShiftG = C.ShiftG; ShiftB = C.ShiftB; ShiftA = C.ShiftA;
		Flag = C.Flag;
		return *this;
	}
	
	FORCEINLINE FSafeColor &operator = (FLinearColor &C) {
		SetValue(&C); return *this;
	}

	FORCEINLINE FArchive &operator << (FArchive &Ar) { 
		Ar << Flag;
		Ar << TrueR;
		Ar << TrueG;
		Ar << TrueB;
		Ar << TrueA;
		Ar << ShiftR;
		Ar << ShiftG;
		Ar << ShiftB;
		Ar << ShiftA;
		Ar << Internal;
		return Ar;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSafeColor &C) {
		return FCrc::MemCrc32(&C,sizeof(FSafeColor));
	}

};

/** {S}: Safe Rotator Property;
 Use this data format to store sensible Rotator values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeRotator {
	GENERATED_USTRUCT_BODY()
private:

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString Internal;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueX;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueY;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString TrueZ;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftX;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftY;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FString ShiftZ;

	UPROPERTY(SaveGame)
	uint8 Flag;

public:

	////////////////////////////////////////////////////////////
	/// Accessors

	FRotator GetValue() {
		if (Internal.Len()>0) {return GetValue(&Internal);} else {
			Internal = FString(*ASCII_KEY);
			switch (Flag) {
				case 0:
					Flag = 1;
					ShiftX = TrueX; ShiftY = TrueY; ShiftZ = TrueZ;
					TrueX = NULL; TrueY = NULL; TrueZ = NULL;
					return FRotator::FRotator(FCString::Atof(*FDecode(ShiftX)),FCString::Atof(*FDecode(ShiftY)),FCString::Atof(*FDecode(ShiftZ)));
				case 1:
					Flag = 0;
					TrueX = ShiftX; TrueY = ShiftY; TrueZ = ShiftZ;
					ShiftX = NULL; ShiftY = NULL; ShiftZ = NULL;
					return FRotator::FRotator(FCString::Atof(*FDecode(TrueX)),FCString::Atof(*FDecode(TrueY)),FCString::Atof(*FDecode(TrueZ)));
			default:
				return FRotator::ZeroRotator;
	}}}

	FRotator GetValue(FString* Key) {
		switch (Flag) {
			case 0:
				Flag = 1;
				ShiftX = TrueX; ShiftY = TrueY; ShiftZ = TrueZ;
				TrueX = NULL; TrueY = NULL; TrueZ = NULL;
				return FRotator::FRotator(FCString::Atof(*FDecode(*Key,ShiftX)),FCString::Atof(*FDecode(*Key,ShiftY)),FCString::Atof(*FDecode(*Key,ShiftZ)));
			case 1:
				Flag = 0;
				TrueX = ShiftX; TrueY = ShiftY; TrueZ = ShiftZ;
				ShiftX = NULL; ShiftY = NULL; ShiftZ = NULL;
				return FRotator::FRotator(FCString::Atof(*FDecode(*Key,TrueX)),FCString::Atof(*FDecode(*Key,TrueY)),FCString::Atof(*FDecode(*Key,TrueZ)));
		default:
			return FRotator::ZeroRotator;
	}}

	void SetValue(FRotator Input) {
		if (Internal.Len()>0) {SetValue(&Internal,Input);} else {
			switch (Flag) {
				case 0:
					Flag = 1;
					ShiftX = *FEncode(FString::Printf(TEXT("%f"),Input.Pitch));
					ShiftY = *FEncode(FString::Printf(TEXT("%f"),Input.Yaw));
					ShiftZ = *FEncode(FString::Printf(TEXT("%f"),Input.Roll));
					TrueX = NULL; TrueY = NULL; TrueZ = NULL;
				case 1:
					Flag = 0;
					TrueX = *FEncode(FString::Printf(TEXT("%f"),Input.Pitch));
					TrueY = *FEncode(FString::Printf(TEXT("%f"),Input.Yaw));
					TrueZ = *FEncode(FString::Printf(TEXT("%f"),Input.Roll));
					ShiftX = NULL; ShiftY = NULL; ShiftZ = NULL;
	}Internal=FString(*ASCII_KEY);}}

	void SetValue(FString* Key, FRotator Input) {
		switch (Flag) {
			case 0:
				Flag = 1;
				ShiftX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Pitch));
				ShiftY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Yaw));
				ShiftZ = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Roll));
				TrueX = NULL; TrueY = NULL; TrueZ = NULL;
			case 1:
				Flag = 0;
				TrueX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Pitch));
				TrueY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Yaw));
				TrueZ = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Roll));
				ShiftX = NULL; ShiftY = NULL; ShiftZ = NULL;
	}Internal=FString(*Key);}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeRotator() {
		Internal = FString(*ASCII_KEY);
		TrueX = TEXT(""); TrueY = TEXT(""); TrueZ = TEXT("");
		ShiftX = TEXT(""); ShiftY = TEXT(""); ShiftZ = TEXT("");
		Flag = 0;
	}

	FSafeRotator(const float Pitch, const float Yall, const float Roll) {
		Internal = FString(*ASCII_KEY);
		TrueX = *FEncode(FString::Printf(TEXT("%f"),Pitch));
		TrueY = *FEncode(FString::Printf(TEXT("%f"),Yall));
		TrueZ = *FEncode(FString::Printf(TEXT("%f"),Roll));
		ShiftX = *FEncode(FString::Printf(TEXT("%f"),Pitch));
		ShiftY = *FEncode(FString::Printf(TEXT("%f"),Yall));
		ShiftZ = *FEncode(FString::Printf(TEXT("%f"),Roll));
		Flag = 0;
	}

	FSafeRotator(const FRotator Input) {
		Internal = FString(*ASCII_KEY);
		TrueX = *FEncode(FString::Printf(TEXT("%f"),Input.Pitch));
		TrueY = *FEncode(FString::Printf(TEXT("%f"),Input.Yaw));
		TrueZ = *FEncode(FString::Printf(TEXT("%f"),Input.Roll));
		ShiftX = *FEncode(FString::Printf(TEXT("%f"),Input.Pitch));
		ShiftY = *FEncode(FString::Printf(TEXT("%f"),Input.Yaw));
		ShiftZ = *FEncode(FString::Printf(TEXT("%f"),Input.Roll));
		Flag = 0;
	}

	FSafeRotator(FString* Key, const FRotator Input) {
		Internal = FString(*Key);
		TrueX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Pitch));
		TrueY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Yaw));
		TrueZ = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Roll));
		ShiftX = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Pitch));
		ShiftY = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Yaw));
		ShiftZ = *FEncode(*Key,FString::Printf(TEXT("%f"),Input.Roll));
		Flag = 0;
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeRotator &operator = (const FSafeRotator &R) {
		Internal = R.Internal;
		TrueX = R.TrueX; TrueY = R.TrueY; TrueZ = R.TrueZ;
		ShiftX = R.ShiftX; ShiftY = R.ShiftY; ShiftZ = R.ShiftZ;
		Flag = R.Flag;
		return *this;
	}
	
	FORCEINLINE FSafeRotator &operator = (const FRotator &R) {
		SetValue(R); return *this;
	}

	FORCEINLINE FArchive &operator << (FArchive &Ar) { 
		Ar << Flag;
		Ar << TrueX;
		Ar << TrueY;
		Ar << TrueZ;
		Ar << ShiftX;
		Ar << ShiftY;
		Ar << ShiftZ;
		Ar << Internal;
		return Ar;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSafeRotator &R) {
		return FCrc::MemCrc32(&R,sizeof(FSafeRotator));
	}

};

/** {S}: Safe Transform Property;
 Use this data format to store sensible Transform values you need protected against memory hackers. */
USTRUCT(BlueprintType)
struct SCUE4_API FSafeTransform {
	GENERATED_USTRUCT_BODY()
private:

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FSafeVector3D Scale;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FSafeVector3D Position;

	UPROPERTY(Category = "Security", VisibleAnywhere, SaveGame)
	FSafeRotator Rotation;

public:

	////////////////////////////////////////////////////////////

	FTransform GetValue() {
		return FTransform::FTransform(Rotation.GetValue(),Position.GetValue(),Scale.GetValue());
	}

	FTransform GetValue(FString* Key) {
		return FTransform::FTransform(Rotation.GetValue(*&Key),Position.GetValue(*&Key),Scale.GetValue(*&Key));
	}

	void SetValue(FTransform* Input) {
		Scale.SetValue(Input->GetScale3D());
		Position.SetValue(Input->GetLocation());
		Rotation.SetValue(Input->GetRotation().Rotator());
	}

	void SetValue(FString* Key, FTransform* Input) {
		Scale.SetValue(*&Key,Input->GetScale3D());
		Position.SetValue(*&Key,Input->GetLocation());
		Rotation.SetValue(*&Key,Input->GetRotation().Rotator());
	}

	////////////////////////////////////////////////////////////
	/// Constructors

	FSafeTransform() {
		Scale = FSafeVector3D::FSafeVector3D();
		Position = FSafeVector3D::FSafeVector3D();
		Rotation = FSafeRotator::FSafeRotator();
	}

	FSafeTransform(FTransform* Input) {
		Scale = FSafeVector3D::FSafeVector3D(Input->GetScale3D());
		Position = FSafeVector3D::FSafeVector3D(Input->GetLocation());
		Rotation = FSafeRotator::FSafeRotator(Input->Rotator());
	}

	FSafeTransform(FString* Key, FTransform* Input) {
		Scale = FSafeVector3D::FSafeVector3D(*&Key,Input->GetScale3D());
		Position = FSafeVector3D::FSafeVector3D(*&Key,Input->GetLocation());
		Rotation = FSafeRotator::FSafeRotator(*&Key,Input->Rotator());
	}

	////////////////////////////////////////////////////////////
	/// Operators

	FORCEINLINE FSafeTransform &operator = (const FSafeTransform &T) {
		Scale = T.Scale;
		Position = T.Position;
		Rotation = T.Rotation;
		return *this;
	}
	
	FORCEINLINE FSafeTransform &operator = (const FTransform &T) {
		Scale = T.GetScale3D();
		Position = T.GetLocation();
		Rotation = T.Rotator();
		return *this;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSafeTransform &T) {
		return FCrc::MemCrc32(&T,sizeof(FSafeTransform));
	}

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if PLATFORM_WINDOWS
#pragma endregion CUSTOM TYPES
#pragma region OPERATORS
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Operators:: Adding support for custom 'Safe Type' operations

// FSBool

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeBool &FSB, FSafeBool &B) { // CONST!  CHANGE ALL GET() and SET() to CONST TOO!!
	return  (FSB.GetValue()==B.GetValue());
}

FORCEINLINE bool operator != (FSafeBool &FSB, FSafeBool &B) {
	return (FSB.GetValue()!=B.GetValue());
}

FORCEINLINE bool operator && (FSafeBool &FSB, FSafeBool &B) {
	return (FSB.GetValue()&&B.GetValue());
}

FORCEINLINE bool operator || (FSafeBool &FSB, FSafeBool &B) {
	return (FSB.GetValue()||B.GetValue());
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeBool &FSB, const bool &B) {
	return (FSB.GetValue()==B);
}

FORCEINLINE bool operator != (FSafeBool &FSB, const bool &B) {
	return (FSB.GetValue()!=B);
}

FORCEINLINE bool operator && (FSafeBool &FSB, const bool &B) {
	return (FSB.GetValue()&&B);
}

FORCEINLINE bool operator || (FSafeBool &FSB, const bool &B) {
	return (FSB.GetValue()||B);
}

/* FSafe -> Native */

FORCEINLINE bool operator == (const bool &B, FSafeBool &FSB) {
	return (B==FSB.GetValue());
}

FORCEINLINE bool operator != (const bool &B, FSafeBool &FSB) {
	return (B!=FSB.GetValue());
}

FORCEINLINE bool operator && (const bool &B, FSafeBool &FSB) {
	return (B&&FSB.GetValue());
}

FORCEINLINE bool operator || (const bool &B, FSafeBool &FSB) {
	return (B||FSB.GetValue());
}

// FSByte

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeByte &FSB, FSafeByte &B) {
	return (FSB.GetValue()==B.GetValue());
}

FORCEINLINE bool operator != (FSafeByte &FSB, FSafeByte &B) {
	return (FSB.GetValue()!=B.GetValue());
}

FORCEINLINE bool operator > (FSafeByte &FSB, FSafeByte &B) {
	return (FSB.GetValue()>B.GetValue());
}

FORCEINLINE bool operator < (FSafeByte &FSB, FSafeByte &B) {
	return (FSB.GetValue()<B.GetValue());
}

FORCEINLINE bool operator >= (FSafeByte &FSB, FSafeByte &B) {
	return (FSB.GetValue()>=B.GetValue());
}

FORCEINLINE bool operator <= (FSafeByte &FSB, FSafeByte &B) {
	return (FSB.GetValue()<=B.GetValue());
}

FORCEINLINE FSafeByte operator + (FSafeByte &FSB, FSafeByte &B) {
	return FSafeByte(FSB.GetValue()+B.GetValue());
}

FORCEINLINE FSafeByte operator - (FSafeByte &FSB, FSafeByte &B) {
	return FSafeByte(FSB.GetValue()-B.GetValue());
}

FORCEINLINE FSafeByte operator * (FSafeByte &FSB, FSafeByte &B) {
	return FSafeByte(FSB.GetValue()*B.GetValue());
}

FORCEINLINE FSafeByte operator / (FSafeByte &FSB, FSafeByte &B) {
	return FSafeByte(FSB.GetValue()/B.GetValue());
}

FORCEINLINE FSafeByte operator ++ (FSafeByte &FSB) {
	auto Local = FSB.GetValue(); Local++;
	return FSafeByte(Local);
}

FORCEINLINE FSafeByte operator -- (FSafeByte &FSB) {
	auto Local = FSB.GetValue(); Local--;
	return FSafeByte(Local);
}

FORCEINLINE FSafeByte operator ~ (FSafeByte &FSB) {
	auto Local = FSB.GetValue();
	return FSafeByte(~Local);
}

FORCEINLINE FSafeByte operator += (FSafeByte &FSB, FSafeByte &B) {
	auto Local = FSB.GetValue(); auto Plus = B.GetValue();
	return FSafeByte((Local+=Plus));
}

FORCEINLINE FSafeByte operator -= (FSafeByte &FSB, FSafeByte &B) {
	auto Local = FSB.GetValue(); auto Minus = B.GetValue();
	return FSafeByte((Local-=Minus));
}

FORCEINLINE FSafeByte operator % (FSafeByte &FSB, FSafeByte &B) {
	auto Local = FSB.GetValue(); auto Mod = B.GetValue();
	return FSafeByte((Local%Mod));
}

FORCEINLINE FSafeByte operator & (FSafeByte &FSB, FSafeByte &B) {
	auto Local = FSB.GetValue(); auto An = B.GetValue();
	return FSafeByte((Local&An));
}

FORCEINLINE FSafeByte operator | (FSafeByte &FSB, FSafeByte &B) {
	auto Local = FSB.GetValue(); auto Orr = B.GetValue();
	return FSafeByte((Local|Orr));
}

FORCEINLINE FSafeByte operator ^ (FSafeByte &FSB, FSafeByte &B) {
	auto Local = FSB.GetValue(); auto To = B.GetValue();
	return FSafeByte((Local^To));
}

FORCEINLINE FSafeByte operator >> (FSafeByte &FSB, FSafeByte &B) {
	auto Local = FSB.GetValue(); auto To = B.GetValue();
	return FSafeByte((Local>>To));
}

FORCEINLINE FSafeByte operator << (FSafeByte &FSB, FSafeByte &B) {
	auto Local = FSB.GetValue(); auto To = B.GetValue();
	return FSafeByte((Local<<To));
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeByte &FSB, const uint8 &B) {
	return (FSB.GetValue()==B);
}

FORCEINLINE bool operator != (FSafeByte &FSB, const uint8 &B) {
	return (FSB.GetValue()!=B);
}

FORCEINLINE bool operator > (FSafeByte &FSB, const uint8 &B) {
	return (FSB.GetValue()>B);
}

FORCEINLINE bool operator < (FSafeByte &FSB, const uint8 &B) {
	return (FSB.GetValue()<B);
}

FORCEINLINE bool operator >= (FSafeByte &FSB, const uint8 &B) {
	return (FSB.GetValue()>=B);
}

FORCEINLINE bool operator <= (FSafeByte &FSB, const uint8 &B) {
	return (FSB.GetValue()<=B);
}

FORCEINLINE FSafeByte operator + (FSafeByte &FSB, const uint8 &B) {
	return FSafeByte(FSB.GetValue()+B);
}

FORCEINLINE FSafeByte operator - (FSafeByte &FSB, const uint8 &B) {
	return FSafeByte(FSB.GetValue()-B);
}

FORCEINLINE FSafeByte operator * (FSafeByte &FSB, const uint8 &B) {
	return FSafeByte(FSB.GetValue()*B);
}

FORCEINLINE FSafeByte operator / (FSafeByte &FSB, const uint8 &B) {
	return FSafeByte(FSB.GetValue()/B);
}

FORCEINLINE FSafeByte operator += (FSafeByte &FSB, const uint8 &B) {
	auto Local = FSB.GetValue();
	return FSafeByte((Local+=B));
}

FORCEINLINE FSafeByte operator -= (FSafeByte &FSB, const uint8 &B) {
	auto Local = FSB.GetValue();
	return FSafeByte((Local-=B));
}

FORCEINLINE FSafeByte operator % (FSafeByte &FSB, const uint8 &B) {
	auto Local = FSB.GetValue();
	return FSafeByte((Local%B));
}

FORCEINLINE FSafeByte operator & (FSafeByte &FSB, const uint8 &B) {
	auto Local = FSB.GetValue();
	return FSafeByte((Local&B));
}

FORCEINLINE FSafeByte operator | (FSafeByte &FSB, const uint8 &B) {
	auto Local = FSB.GetValue();
	return FSafeByte((Local|B));
}

FORCEINLINE FSafeByte operator ^ (FSafeByte &FSB, const uint8 &B) {
	auto Local = FSB.GetValue();
	return FSafeByte((Local^B));
}

FORCEINLINE FSafeByte operator >> (FSafeByte &FSB, const uint8 &B) {
	auto Local = FSB.GetValue();
	return FSafeByte((Local>>B));
}

FORCEINLINE FSafeByte operator << (FSafeByte &FSB, const uint8 &B) {
	auto Local = FSB.GetValue();
	return FSafeByte((Local<<B));
}

/* FSafe -> Native */

FORCEINLINE bool operator == (const uint8 &B, FSafeByte &FSB) {
	return (B==FSB.GetValue());
}

FORCEINLINE bool operator != (const uint8 &B, FSafeByte &FSB) {
	return (B!=FSB.GetValue());
}

FORCEINLINE bool operator > (const uint8 &B, FSafeByte &FSB) {
	return (B>FSB.GetValue());
}

FORCEINLINE bool operator < (const uint8 &B, FSafeByte &FSB) {
	return (B<FSB.GetValue());
}

FORCEINLINE bool operator >= (const uint8 &B, FSafeByte &FSB) {
	return (B>=FSB.GetValue());
}

FORCEINLINE bool operator <= (const uint8 &B, FSafeByte &FSB) {
	return (B<=FSB.GetValue());
}

FORCEINLINE uint8 operator + (uint8 &B, FSafeByte &FSB) {
	return (B+FSB.GetValue());
}

FORCEINLINE uint8 operator - (uint8 &B, FSafeByte &FSB) {
	return (B-FSB.GetValue());
}

FORCEINLINE uint8 operator * (uint8 &B, FSafeByte &FSB) {
	return (B*FSB.GetValue());
}

FORCEINLINE uint8 operator / (uint8 &B, FSafeByte &FSB) {
	return (B/FSB.GetValue());
}

FORCEINLINE uint8 operator += (uint8 &B, FSafeByte &FSB) {
	auto Local = FSB.GetValue();
	return ((B+=Local));
}

FORCEINLINE uint8 operator -= (uint8 &B, FSafeByte &FSB) {
	auto Local = FSB.GetValue();
	return ((B-=Local));
}

FORCEINLINE uint8 operator % (uint8 &B, FSafeByte &FSB) {
	auto Mod = FSB.GetValue();
	return ((B%Mod));
}

FORCEINLINE uint8 operator & (uint8 &B, FSafeByte &FSB) {
	auto Local = FSB.GetValue();
	return ((B&Local));
}

FORCEINLINE uint8 operator | (uint8 &B, FSafeByte &FSB) {
	auto Local = FSB.GetValue();
	return ((B|Local));
}

FORCEINLINE uint8 operator ^ (uint8 &B, FSafeByte &FSB) {
	auto Local = FSB.GetValue();
	return ((B^Local));
}

FORCEINLINE uint8 operator >> (uint8 &B, FSafeByte &FSB) {
	auto Local = FSB.GetValue();
	return ((B>>Local));
}

FORCEINLINE uint8 operator << (uint8 &B, FSafeByte &FSB) {
	auto Local = FSB.GetValue();
	return ((B<<Local));
}

// FSInt

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeInt &FSI, FSafeInt &I) {
	return (FSI.GetValue() == I.GetValue());
}

FORCEINLINE bool operator != (FSafeInt &FSI, FSafeInt &I) {
	return (FSI.GetValue()!=I.GetValue());
}

FORCEINLINE bool operator > (FSafeInt &FSI, FSafeInt &I) {
	return (FSI.GetValue()>I.GetValue());
}

FORCEINLINE bool operator < (FSafeInt &FSI, FSafeInt &I) {
	return (FSI.GetValue()<I.GetValue());
}

FORCEINLINE bool operator >= (FSafeInt &FSI, FSafeInt &I) {
	return (FSI.GetValue()>=I.GetValue());
}

FORCEINLINE bool operator <= (FSafeInt &FSI, FSafeInt &I) {
	return (FSI.GetValue()<=I.GetValue());
}

FORCEINLINE FSafeInt operator + (FSafeInt &FSI, FSafeInt &I) {
	return FSafeInt(FSI.GetValue()+I.GetValue());
}

FORCEINLINE FSafeInt operator - (FSafeInt &FSI, FSafeInt &I) {
	return FSafeInt(FSI.GetValue()-I.GetValue());
}

FORCEINLINE FSafeInt operator * (FSafeInt &FSI, FSafeInt &I) {
	return FSafeInt(FSI.GetValue()*I.GetValue());
}

FORCEINLINE FSafeInt operator / (FSafeInt &FSI, FSafeInt &I) {
	return FSafeInt(FSI.GetValue()/I.GetValue());
}

FORCEINLINE FSafeInt operator ++ (FSafeInt &FSI) {
	auto Local = FSI.GetValue(); Local++;
	return FSafeInt(Local);
}

FORCEINLINE FSafeInt operator -- (FSafeInt &FSI) {
	auto Local = FSI.GetValue(); Local--;
	return FSafeInt(Local);
}

FORCEINLINE FSafeInt operator ~ (FSafeInt &FSI) {
	auto Local = FSI.GetValue();
	return FSafeInt(~Local);
}

FORCEINLINE FSafeInt operator += (FSafeInt &FSI, FSafeInt &I) {
	auto Local = FSI.GetValue(); auto Plus = I.GetValue();
	return FSafeInt((Local+=Plus));
}

FORCEINLINE FSafeInt operator -= (FSafeInt &FSI, FSafeInt &I) {
	auto Local = FSI.GetValue(); auto Minus = I.GetValue();
	return FSafeInt((Local-=Minus));
}

FORCEINLINE FSafeInt operator % (FSafeInt &FSI, FSafeInt &I) {
	auto Local = FSI.GetValue(); auto Mod = I.GetValue();
	return FSafeInt((Local%Mod));
}

FORCEINLINE FSafeInt operator & (FSafeInt &FSI, FSafeInt &I) {
	auto Local = FSI.GetValue(); auto An = I.GetValue();
	return FSafeInt((Local&An));
}

FORCEINLINE FSafeInt operator | (FSafeInt &FSI, FSafeInt &I) {
	auto Local = FSI.GetValue(); auto Orr = I.GetValue();
	return FSafeInt((Local|Orr));
}

FORCEINLINE FSafeInt operator ^ (FSafeInt &FSI, FSafeInt &I) {
	auto Local = FSI.GetValue(); auto To = I.GetValue();
	return FSafeInt((Local^To));
}

FORCEINLINE FSafeInt operator >> (FSafeInt &FSI, FSafeInt &I) {
	auto Local = FSI.GetValue(); auto To = I.GetValue();
	return FSafeInt((Local>>To));
}

FORCEINLINE FSafeInt operator << (FSafeInt &FSI, FSafeInt &I) {
	auto Local = FSI.GetValue(); auto To = I.GetValue();
	return FSafeInt((Local<<To));
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeInt &FSI, const int32 &I) {
	return (FSI.GetValue()==I);
}

FORCEINLINE bool operator != (FSafeInt &FSI, const int32 &I) {
	return (FSI.GetValue()!=I);
}

FORCEINLINE bool operator > (FSafeInt &FSI, const int32 &I) {
	return (FSI.GetValue()>I);
}

FORCEINLINE bool operator < (FSafeInt &FSI, const int32 &I) {
	return (FSI.GetValue()<I);
}

FORCEINLINE bool operator >= (FSafeInt &FSI, const int32 &I) {
	return (FSI.GetValue()>=I);
}

FORCEINLINE bool operator <= (FSafeInt &FSI, const int32 &I) {
	return (FSI.GetValue()<=I);
}

FORCEINLINE FSafeInt operator + (FSafeInt &FSI, const int32 &I) {
	return FSafeInt(FSI.GetValue()+I);
}

FORCEINLINE FSafeInt operator - (FSafeInt &FSI, const int32 &I) {
	return FSafeInt(FSI.GetValue()-I);
}

FORCEINLINE FSafeInt operator * (FSafeInt &FSI, const int32 &I) {
	return FSafeInt(FSI.GetValue()*I);
}

FORCEINLINE FSafeInt operator / (FSafeInt &FSI, const int32 &I) {
	return FSafeInt(FSI.GetValue()/I);
}

FORCEINLINE FSafeInt operator += (FSafeInt &FSI, const int32 &I) {
	auto Local = FSI.GetValue();
	return FSafeInt((Local+=I));
}

FORCEINLINE FSafeInt operator -= (FSafeInt &FSI, const int32 &I) {
	auto Local = FSI.GetValue();
	return FSafeInt((Local-=I));
}

FORCEINLINE FSafeInt operator % (FSafeInt &FSI, const int32 &I) {
	auto Local = FSI.GetValue();
	return FSafeInt((Local%I));
}

FORCEINLINE FSafeInt operator & (FSafeInt &FSI, const int32 &I) {
	auto Local = FSI.GetValue();
	return FSafeInt((Local&I));
}

FORCEINLINE FSafeInt operator | (FSafeInt &FSI, const int32 &I) {
	auto Local = FSI.GetValue();
	return FSafeInt((Local|I));
}

FORCEINLINE FSafeInt operator ^ (FSafeInt &FSI, const int32 &I) {
	auto Local = FSI.GetValue();
	return FSafeInt((Local^I));
}

FORCEINLINE FSafeInt operator >> (FSafeInt &FSI, const int32 &I) {
	auto Local = FSI.GetValue();
	return FSafeInt((Local>>I));
}

FORCEINLINE FSafeInt operator << (FSafeInt &FSI, const int32 &I) {
	auto Local = FSI.GetValue();
	return FSafeInt((Local<<I));
}

/* FSafe -> Native */

FORCEINLINE bool operator == (const int32 &I, FSafeInt &FSI) {
	return (I==FSI.GetValue());
}

FORCEINLINE bool operator != (const int32 &I, FSafeInt &FSI) {
	return (I!=FSI.GetValue());
}

FORCEINLINE bool operator > (const int32 &I, FSafeInt &FSI) {
	return (I>FSI.GetValue());
}

FORCEINLINE bool operator < (const int32 &I, FSafeInt &FSI) {
	return (I<FSI.GetValue());
}

FORCEINLINE bool operator >= (const int32 &I, FSafeInt &FSI) {
	return (I>=FSI.GetValue());
}

FORCEINLINE bool operator <= (const int32 &I, FSafeInt &FSI) {
	return (I<=FSI.GetValue());
}

FORCEINLINE int32 operator + (int32 &I, FSafeInt &FSI) {
	return (I+FSI.GetValue());
}

FORCEINLINE int32 operator - (int32 &I, FSafeInt &FSI) {
	return (I-FSI.GetValue());
}

FORCEINLINE int32 operator * (int32 &I, FSafeInt &FSI) {
	return (I*FSI.GetValue());
}

FORCEINLINE int32 operator / (int32 &I, FSafeInt &FSI) {
	return (I/FSI.GetValue());
}

FORCEINLINE int32 operator += (int32 &I, FSafeInt &FSI) {
	auto Local = FSI.GetValue();
	return ((I+=Local));
}

FORCEINLINE int32 operator -= (int32 &I, FSafeInt &FSI) {
	auto Local = FSI.GetValue();
	return ((I-=Local));
}

FORCEINLINE int32 operator % (int32 &I, FSafeInt &FSI) {
	auto Mod = FSI.GetValue();
	return ((I%Mod));
}

FORCEINLINE int32 operator & (int32 &I, FSafeInt &FSI) {
	auto Local = FSI.GetValue();
	return ((I&Local));
}

FORCEINLINE int32 operator | (int32 &I, FSafeInt &FSI) {
	auto Local = FSI.GetValue();
	return ((I|Local));
}

FORCEINLINE int32 operator ^ (int32 &I, FSafeInt &FSI) {
	auto Local = FSI.GetValue();
	return ((I^Local));
}

FORCEINLINE int32 operator >> (int32 &I, FSafeInt &FSI) {
	auto Local = FSI.GetValue();
	return ((I>>Local));
}

FORCEINLINE int32 operator << (int32 &I, FSafeInt &FSI) {
	auto Local = FSI.GetValue();
	return ((I<<Local));
}

// FSFloat

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeFloat &FSF, FSafeFloat &F) {
	return (FSF.GetValue()==F.GetValue());
}

FORCEINLINE bool operator != (FSafeFloat &FSF, FSafeFloat &F) {
	return (FSF.GetValue()!=F.GetValue());
}

FORCEINLINE bool operator > (FSafeFloat &FSF, FSafeFloat &F) {
	return (FSF.GetValue()>F.GetValue());
}

FORCEINLINE bool operator < (FSafeFloat &FSF, FSafeFloat &F) {
	return (FSF.GetValue()<F.GetValue());
}

FORCEINLINE bool operator >= (FSafeFloat &FSF, FSafeFloat &F) {
	return (FSF.GetValue()>=F.GetValue());
}

FORCEINLINE bool operator <= (FSafeFloat &FSF, FSafeFloat &F) {
	return (FSF.GetValue()<=F.GetValue());
}

FORCEINLINE FSafeFloat operator + (FSafeFloat &FSF, FSafeFloat &F) {
	return FSafeFloat(FSF.GetValue()+F.GetValue());
}

FORCEINLINE FSafeFloat operator - (FSafeFloat &FSF, FSafeFloat &F) {
	return FSafeFloat(FSF.GetValue()-F.GetValue());
}

FORCEINLINE FSafeFloat operator * (FSafeFloat &FSF, FSafeFloat &F) {
	return FSafeFloat(FSF.GetValue()*F.GetValue());
}

FORCEINLINE FSafeFloat operator / (FSafeFloat &FSF, FSafeFloat &F) {
	return FSafeFloat(FSF.GetValue()/F.GetValue());
}

FORCEINLINE FSafeFloat operator ++ (FSafeFloat &FSF) {
	auto Local = FSF.GetValue(); Local++;
	return FSafeFloat(Local);
}

FORCEINLINE FSafeFloat operator -- (FSafeFloat &FSF) {
	auto Local = FSF.GetValue(); Local--;
	return FSafeFloat(Local);
}

FORCEINLINE FSafeFloat operator += (FSafeFloat &FSF, FSafeFloat &F) {
	auto Local = FSF.GetValue(); auto Plus = F.GetValue();
	return FSafeFloat((Local+=Plus));
}

FORCEINLINE FSafeFloat operator -= (FSafeFloat &FSF, FSafeFloat &F) {
	auto Local = FSF.GetValue(); auto Minus = F.GetValue();
	return FSafeFloat((Local-=Minus));
}

FORCEINLINE FSafeFloat operator % (FSafeFloat &FSF, FSafeFloat &F) {
	auto Local = FSF.GetValue(); auto Mod = F.GetValue();
	return FSafeFloat(FGenericPlatformMath::Fmod(Local,Mod));
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeFloat &FSF, const float &F) {
	return (FSF.GetValue()==F);
}

FORCEINLINE bool operator != (FSafeFloat &FSF, const float &F) {
	return (FSF.GetValue()!=F);
}

FORCEINLINE bool operator > (FSafeFloat &FSF, const float &F) {
	return (FSF.GetValue()>F);
}

FORCEINLINE bool operator < (FSafeFloat &FSF, const float &F) {
	return (FSF.GetValue()<F);
}

FORCEINLINE bool operator >= (FSafeFloat &FSF, const float &F) {
	return (FSF.GetValue()>=F);
}

FORCEINLINE bool operator <= (FSafeFloat &FSF, const float &F) {
	return (FSF.GetValue()<=F);
}

FORCEINLINE FSafeFloat operator + (FSafeFloat &FSF, const float &F) {
	return FSafeFloat(FSF.GetValue()+F);
}

FORCEINLINE FSafeFloat operator - (FSafeFloat &FSF, const float &F) {
	return FSafeFloat(FSF.GetValue()-F);
}

FORCEINLINE FSafeFloat operator * (FSafeFloat &FSF, const float &F) {
	return FSafeFloat(FSF.GetValue()*F);
}

FORCEINLINE FSafeFloat operator / (FSafeFloat &FSF, const float &F) {
	return FSafeFloat(FSF.GetValue()/F);
}

FORCEINLINE FSafeFloat operator += (FSafeFloat &FSF, const float &F) {
	auto Local = FSF.GetValue();
	return FSafeFloat((Local+=F));
}

FORCEINLINE FSafeFloat operator -= (FSafeFloat &FSF, const float &F) {
	auto Local = FSF.GetValue();
	return FSafeFloat((Local-=F));
}

FORCEINLINE FSafeFloat operator % (FSafeFloat &FSF, const float &F) {
	auto Local = FSF.GetValue();
	return FSafeFloat(FGenericPlatformMath::Fmod(Local,F));
}

/* FSafe -> Native */

FORCEINLINE bool operator == (const float &F, FSafeFloat &FSF) {
	return (F==FSF.GetValue());
}

FORCEINLINE bool operator != (const float &F, FSafeFloat &FSF) {
	return (F!=FSF.GetValue());
}

FORCEINLINE bool operator > (const float &F, FSafeFloat &FSF) {
	return (F>FSF.GetValue());
}

FORCEINLINE bool operator < (const float &F, FSafeFloat &FSF) {
	return (F<FSF.GetValue());
}

FORCEINLINE bool operator >= (const float &F, FSafeFloat &FSF) {
	return (F>=FSF.GetValue());
}

FORCEINLINE bool operator <= (const float &F, FSafeFloat &FSF) {
	return (F<=FSF.GetValue());
}

FORCEINLINE float operator + (float &F, FSafeFloat &FSF) {
	return (F+FSF.GetValue());
}

FORCEINLINE float operator - (float &F, FSafeFloat &FSF) {
	return (F-FSF.GetValue());
}

FORCEINLINE float operator * (float &F, FSafeFloat &FSF) {
	return (F*FSF.GetValue());
}

FORCEINLINE float operator / (float &F, FSafeFloat &FSF) {
	return (F/FSF.GetValue());
}

FORCEINLINE float operator += (float &F, FSafeFloat &FSF) {
	auto Local = FSF.GetValue();
	return ((F+=Local));
}

FORCEINLINE float operator -= (float &F, FSafeFloat &FSF) {
	auto Local = FSF.GetValue();
	return ((F-=Local));
}

FORCEINLINE float operator % (float &F, FSafeFloat &FSF) {
	auto Mod = FSF.GetValue();
	return (FGenericPlatformMath::Fmod(F,Mod));
}

// FSName

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeName &FSN, FSafeName &N) {
	return (FSN.GetValue().IsEqual(N.GetValue(),ENameCase::CaseSensitive,true));
}

FORCEINLINE bool operator != (FSafeName &FSN, FSafeName &N) {
	return (!FSN.GetValue().IsEqual(N.GetValue(),ENameCase::CaseSensitive,true));
}

FORCEINLINE bool operator > (FSafeName &FSN, FSafeName &N) {
	return (FSN.GetValue()>N.GetValue());
}

FORCEINLINE bool operator < (FSafeName &FSN, FSafeName &N) {
	return (FSN.GetValue()<N.GetValue());
}

FORCEINLINE FSafeName operator + (FSafeName &FSN, FSafeName &N) {
	return FSafeName(*(FSN.GetValue().ToString().Append(N.GetValue().ToString())));
}

FORCEINLINE FSafeName operator += (FSafeName &FSN, FSafeName &N) {
	*&FSN = FSafeName(*(FSN.GetValue().ToString().Append(N.GetValue().ToString()))); return *&FSN;
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeName &FSN, FName &N) {
	return (FSN.GetValue().IsEqual(N,ENameCase::CaseSensitive,true));
}

FORCEINLINE bool operator != (FSafeName &FSN, FName &N) {
	return (!FSN.GetValue().IsEqual(N,ENameCase::CaseSensitive,true));
}

FORCEINLINE bool operator > (FSafeName &FSN, FName &N) {
	return (FSN.GetValue()>N);
}

FORCEINLINE bool operator < (FSafeName &FSN, FName &N) {
	return (FSN.GetValue()<N);
}

FORCEINLINE FSafeName operator + (FSafeName &FSN, FName &N) {
	return FSafeName(*(FSN.GetValue().ToString().Append(N.ToString())));
}

FORCEINLINE FSafeName operator += (FSafeName &FSN, FName &N) {
	*&FSN = FSafeName(*(FSN.GetValue().ToString().Append(N.ToString()))); return *&FSN;
}

/* FSafe -> Native */

FORCEINLINE bool operator == (FName &N, FSafeName &FSN) {
	return (N.IsEqual(FSN.GetValue(),ENameCase::CaseSensitive,true));
}

FORCEINLINE bool operator != (FName &N, FSafeName &FSN) {
	return (!N.IsEqual(FSN.GetValue(),ENameCase::CaseSensitive,true));
}

FORCEINLINE bool operator > (FName &N, FSafeName &FSN) {
	return (N>FSN.GetValue());
}

FORCEINLINE bool operator < (FName &N, FSafeName &FSN) {
	return (N<FSN.GetValue());
}

FORCEINLINE FName operator + (FName &N, FSafeName &FSN) {
	return FName(*(N.ToString().Append(FSN.GetValue().ToString())));
}

FORCEINLINE FName operator += (FName &N, FSafeName &FSN) {
	*&N = FName(*(N.ToString().Append(FSN.GetValue().ToString()))); return *&N;
}

// FSText

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeText &FST, FSafeText &T) {
	return (FST.GetValue().EqualTo(T.GetValue()));
}

FORCEINLINE bool operator != (FSafeText &FST, FSafeText &T) {
	return (!FST.GetValue().EqualTo(T.GetValue()));
}

FORCEINLINE bool operator > (FSafeText &FST, FSafeText &T) {
	return (FST.GetValue().ToString()>T.GetValue().ToString());
}

FORCEINLINE bool operator < (FSafeText &FST, FSafeText &T) {
	return (FST.GetValue().ToString()<T.GetValue().ToString());
}

FORCEINLINE FSafeText operator + (FSafeText &FST, FSafeText &T) {
	return FSafeText(FText::Format(FST.GetValue(),T.GetValue()));
}

FORCEINLINE FSafeText operator += (FSafeText &FST, FSafeText &T) {
	*&FST = FSafeText(FText::Format(FST.GetValue(),T.GetValue())); return *&FST;
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeText &FST, FText &T) {
	return (FST.GetValue().EqualTo(T));
}

FORCEINLINE bool operator != (FSafeText &FST, FText &T) {
	return (!FST.GetValue().EqualTo(T));
}

FORCEINLINE bool operator > (FSafeText &FST, FText &T) {
	return (FST.GetValue().ToString()>T.ToString());
}

FORCEINLINE bool operator < (FSafeText &FST, FText &T) {
	return (FST.GetValue().ToString()<T.ToString());
}

FORCEINLINE FSafeText operator + (FSafeText &FST, FText &T) {
	return FSafeText(FText::Format(FST.GetValue(),T));
}

FORCEINLINE FSafeText operator += (FSafeText &FST, FText &T) {
	*&FST = FSafeText(FText::Format(FST.GetValue(),T)); return *&FST;
}

/* FSafe -> Native */

FORCEINLINE bool operator == (FText &T, FSafeText &FST) {
	return (T.EqualTo(FST.GetValue()));
}

FORCEINLINE bool operator != (FText &T, FSafeText &FST) {
	return (!T.EqualTo(FST.GetValue()));
}

FORCEINLINE bool operator > (FText &T, FSafeText &FST) {
	return (T.ToString()>FST.GetValue().ToString());
}

FORCEINLINE bool operator < (FText &T, FSafeText &FST) {
	return (T.ToString()<FST.GetValue().ToString());
}

FORCEINLINE FText operator + (FText &T, FSafeText &FST) {
	return FText::Format(T,FST.GetValue());
}

FORCEINLINE FText operator += (FText &T, FSafeText &FST) {
	*&T = FText::Format(T,FST.GetValue()); return *&T;
}

// FSString

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeString &FSS, FSafeString &S) {
	return (FSS.GetValue().Equals(S.GetValue(),ESearchCase::CaseSensitive));
}

FORCEINLINE bool operator != (FSafeString &FSS, FSafeString &S) {
	return (!FSS.GetValue().Equals(S.GetValue(),ESearchCase::CaseSensitive));
}

FORCEINLINE bool operator > (FSafeString &FSS, FSafeString &S) {
	return (FSS.GetValue()>S.GetValue());
}

FORCEINLINE bool operator < (FSafeString &FSS, FSafeString &S) {
	return (FSS.GetValue()<S.GetValue());
}

FORCEINLINE FSafeString operator + (FSafeString &FSS, FSafeString &S) {
	return FSafeString(*(FSS.GetValue().Append(S.GetValue()))); return *&FSS;
}

FORCEINLINE FSafeString operator += (FSafeString &FSS, FSafeString &S) {
	*&FSS = FSafeString(*(FSS.GetValue().Append(S.GetValue()))); return *&FSS;
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeString &FSS, FString &S) {
	return (FSS.GetValue().Equals(S,ESearchCase::CaseSensitive));
}

FORCEINLINE bool operator != (FSafeString &FSS, FString &S) {
	return (!FSS.GetValue().Equals(S,ESearchCase::CaseSensitive));
}

FORCEINLINE bool operator > (FSafeString &FSS, FString &S) {
	return (FSS.GetValue()>S);
}

FORCEINLINE bool operator < (FSafeString &FSS, FString &S) {
	return (FSS.GetValue()<S);
}

FORCEINLINE FSafeString operator + (FSafeString &FSS, FString &S) {
	return FSafeString(*(FSS.GetValue().Append(S)));
}

FORCEINLINE FSafeString operator += (FSafeString &FSS, FString &S) {
	*&FSS = FSafeString(*(FSS.GetValue().Append(S))); return *&FSS;
}

/* FSafe -> Native */

FORCEINLINE bool operator == (FString &S, FSafeString &FSS) {
	return (S.Equals(FSS.GetValue(),ESearchCase::CaseSensitive));
}

FORCEINLINE bool operator != (FString &S, FSafeString &FSS) {
	return (!S.Equals(FSS.GetValue(),ESearchCase::CaseSensitive));
}

FORCEINLINE bool operator > (FString &S, FSafeString &FSS) {
	return (S>FSS.GetValue());
}

FORCEINLINE bool operator < (FString &S, FSafeString &FSS) {
	return (S<FSS.GetValue());
}

FORCEINLINE FString operator + (FString &S, FSafeString &FSS) {
	return FString(*(S.Append(FSS.GetValue())));
}

FORCEINLINE FString operator += (FString &S, FSafeString &FSS) {
	*&S = FString(*(S.Append(FSS.GetValue()))); return *&S;
}

// FSVector2D

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeVector2D &FSV, FSafeVector2D &V) {
	return (FSV.GetValue()==V.GetValue());
}

FORCEINLINE bool operator != (FSafeVector2D &FSV, FSafeVector2D &V) {
	return (FSV.GetValue()!=V.GetValue());
}

FORCEINLINE bool operator > (FSafeVector2D &FSV, FSafeVector2D &V) {
	return (FSV.GetValue()>V.GetValue());
}

FORCEINLINE bool operator < (FSafeVector2D &FSV, FSafeVector2D &V) {
	return (FSV.GetValue()<V.GetValue());
}

FORCEINLINE bool operator >= (FSafeVector2D &FSV, FSafeVector2D &V) {
	return (FSV.GetValue()>=V.GetValue());
}

FORCEINLINE bool operator <= (FSafeVector2D &FSV, FSafeVector2D &V) {
	return (FSV.GetValue()<=V.GetValue());
}

FORCEINLINE FSafeVector2D operator + (FSafeVector2D &FSV, FSafeVector2D &V) {
	return FSafeVector2D(FSV.GetValue()+V.GetValue());
}

FORCEINLINE FSafeVector2D operator - (FSafeVector2D &FSV, FSafeVector2D &V) {
	return FSafeVector2D(FSV.GetValue()-V.GetValue());
}

FORCEINLINE FSafeVector2D operator * (FSafeVector2D &FSV, FSafeVector2D &V) {
	return FSafeVector2D(FSV.GetValue()*V.GetValue());
}

FORCEINLINE FSafeVector2D operator / (FSafeVector2D &FSV, FSafeVector2D &V) {
	return FSafeVector2D(FSV.GetValue()/V.GetValue());
}

FORCEINLINE FSafeVector2D operator ++ (FSafeVector2D &FSV) {
	auto Local = FSV.GetValue();
	*&FSV = FSafeVector2D(Local.X+1,Local.Y+1); return *&FSV;
}

FORCEINLINE FSafeVector2D operator -- (FSafeVector2D &FSV) {
	auto Local = FSV.GetValue();
	*&FSV = FSafeVector2D(Local.X-1,Local.Y-1); return *&FSV;
}

FORCEINLINE FSafeVector2D operator += (FSafeVector2D &FSV, FSafeVector2D &V) {
	auto Local = FSV.GetValue(); auto Plus = V.GetValue();
	return FSafeVector2D((Local+=Plus));
}

FORCEINLINE FSafeVector2D operator -= (FSafeVector2D &FSV, FSafeVector2D &V) {
	auto Local = FSV.GetValue(); auto Minus = V.GetValue();
	return FSafeVector2D((Local-=Minus));
}

FORCEINLINE FSafeVector2D operator % (FSafeVector2D &FSV, FSafeVector2D &V) {
	auto Local = FSV.GetValue(); auto Mod = V.GetValue();
	auto X = Local.X; auto Y = Local.Y; auto MX = Mod.X; auto MY = Mod.Y; 
	return FSafeVector2D(FVector2D(FGenericPlatformMath::Fmod(X,MX),FGenericPlatformMath::Fmod(Y,MY)));
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeVector2D &FSV, const FVector2D &V) {
	return (FSV.GetValue()==V);
}

FORCEINLINE bool operator != (FSafeVector2D &FSV, const FVector2D &V) {
	return (FSV.GetValue()!=V);
}

FORCEINLINE bool operator > (FSafeVector2D &FSV, const FVector2D &V) {
	return (FSV.GetValue()>V);
}

FORCEINLINE bool operator < (FSafeVector2D &FSV, const FVector2D &V) {
	return (FSV.GetValue()<V);
}

FORCEINLINE bool operator >= (FSafeVector2D &FSV, const FVector2D &V) {
	return (FSV.GetValue()>=V);
}

FORCEINLINE bool operator <= (FSafeVector2D &FSV, const FVector2D &V) {
	return (FSV.GetValue()<=V);
}

FORCEINLINE FSafeVector2D operator + (FSafeVector2D &FSV, const FVector2D &V) {
	return FSafeVector2D(FSV.GetValue()+V);
}

FORCEINLINE FSafeVector2D operator - (FSafeVector2D &FSV, const FVector2D &V) {
	return FSafeVector2D(FSV.GetValue()-V);
}

FORCEINLINE FSafeVector2D operator * (FSafeVector2D &FSV, const FVector2D &V) {
	return FSafeVector2D(FSV.GetValue()*V);
}

FORCEINLINE FSafeVector2D operator / (FSafeVector2D &FSV, const FVector2D &V) {
	return FSafeVector2D(FSV.GetValue()/V);
}

FORCEINLINE FSafeVector2D operator += (FSafeVector2D &FSV, const FVector2D &V) {
	auto Local = FSV.GetValue();
	return FSafeVector2D((Local+=V));
}

FORCEINLINE FSafeVector2D operator -= (FSafeVector2D &FSV, const FVector2D &V) {
	auto Local = FSV.GetValue();
	return FSafeVector2D((Local-=V));
}

FORCEINLINE FSafeVector2D operator % (FSafeVector2D &FSV, const FVector2D &V) {
	auto Local = FSV.GetValue();
	auto X = Local.X; auto Y = Local.Y;
	auto MX = V.X; auto MY = V.Y; 
	return FSafeVector2D(FVector2D(FGenericPlatformMath::Fmod(X,MX),FGenericPlatformMath::Fmod(Y,MY)));
}

/* FSafe -> Native */

FORCEINLINE bool operator == (const FVector2D &V, FSafeVector2D &FSV) {
	return (V==FSV.GetValue());
}

FORCEINLINE bool operator != (const FVector2D &V, FSafeVector2D &FSV) {
	return (V!=FSV.GetValue());
}

FORCEINLINE bool operator > (const FVector2D &V, FSafeVector2D &FSV) {
	return (V>FSV.GetValue());
}

FORCEINLINE bool operator < (const FVector2D &V, FSafeVector2D &FSV) {
	return (V<FSV.GetValue());
}

FORCEINLINE bool operator >= (const FVector2D &V, FSafeVector2D &FSV) {
	return (V>=FSV.GetValue());
}

FORCEINLINE bool operator <= (const FVector2D &V, FSafeVector2D &FSV) {
	return (FSV.GetValue()<=V);
}

FORCEINLINE FVector2D operator + (const FVector2D &V, FSafeVector2D &FSV) {
	return (V+FSV.GetValue());
}

FORCEINLINE FVector2D operator - (const FVector2D &V, FSafeVector2D &FSV) {
	return (V-FSV.GetValue());
}

FORCEINLINE FVector2D operator * (const FVector2D &V, FSafeVector2D &FSV) {
	return (V*FSV.GetValue());
}

FORCEINLINE FVector2D operator / (const FVector2D &V, FSafeVector2D &FSV) {
	return (V/FSV.GetValue());
}

FORCEINLINE FVector2D operator += (FVector2D &V, FSafeVector2D &FSV) {
	auto Local = FSV.GetValue();
	return (V+=Local);
}

FORCEINLINE FVector2D operator -= (FVector2D &V, FSafeVector2D &FSV) {
	auto Local = FSV.GetValue();
	return (V-=Local);
}

FORCEINLINE FVector2D operator % (const FVector2D &V, FSafeVector2D &FSV) {
	auto Local = FSV.GetValue();
	auto X = V.X; auto Y = V.Y; 
	auto MX = Local.X; auto MY = Local.Y;
	return FVector2D(FGenericPlatformMath::Fmod(X,MX),FGenericPlatformMath::Fmod(Y,MY));
}

// FSVector3D

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeVector3D &FSV, FSafeVector3D &V) {
	return (FSV.GetValue()==V.GetValue());
}

FORCEINLINE bool operator != (FSafeVector3D &FSV, FSafeVector3D &V) {
	return (FSV.GetValue()!=V.GetValue());
}

FORCEINLINE bool operator > (FSafeVector3D &FSV, FSafeVector3D &V) {
	return ((FSV.GetValue().X>V.GetValue().X)&&(FSV.GetValue().Y>V.GetValue().Y)&&(FSV.GetValue().Z>V.GetValue().Z));
}

FORCEINLINE bool operator < (FSafeVector3D &FSV, FSafeVector3D &V) {
	return ((FSV.GetValue().X<V.GetValue().X)&&(FSV.GetValue().Y<V.GetValue().Y)&&(FSV.GetValue().Z<V.GetValue().Z));
}

FORCEINLINE bool operator >= (FSafeVector3D &FSV, FSafeVector3D &V) {
	return ((FSV.GetValue().X>=V.GetValue().X)&&(FSV.GetValue().Y>=V.GetValue().Y)&&(FSV.GetValue().Z>=V.GetValue().Z));
}

FORCEINLINE bool operator <= (FSafeVector3D &FSV, FSafeVector3D &V) {
	return ((FSV.GetValue().X<=V.GetValue().X)&&(FSV.GetValue().Y<=V.GetValue().Y)&&(FSV.GetValue().Z<=V.GetValue().Z));
}

FORCEINLINE FSafeVector3D operator + (FSafeVector3D &FSV, FSafeVector3D &V) {
	return FSafeVector3D(FSV.GetValue()+V.GetValue());
}

FORCEINLINE FSafeVector3D operator - (FSafeVector3D &FSV, FSafeVector3D &V) {
	return FSafeVector3D(FSV.GetValue()-V.GetValue());
}

FORCEINLINE FSafeVector3D operator * (FSafeVector3D &FSV, FSafeVector3D &V) {
	return FSafeVector3D(FSV.GetValue()*V.GetValue());
}

FORCEINLINE FSafeVector3D operator / (FSafeVector3D &FSV, FSafeVector3D &V) {
	return FSafeVector3D(FSV.GetValue()/V.GetValue());
}

FORCEINLINE FSafeVector3D operator ++ (FSafeVector3D &FSV) {
	auto Local = FSV.GetValue();
	*&FSV = FSafeVector3D(Local.X+1,Local.Y+1,Local.Z+1); return *&FSV;
}

FORCEINLINE FSafeVector3D operator -- (FSafeVector3D &FSV) {
	auto Local = FSV.GetValue();
	*&FSV = FSafeVector3D(Local.X-1,Local.Y-1,Local.Z-1); return *&FSV;
}

FORCEINLINE FSafeVector3D operator += (FSafeVector3D &FSV, FSafeVector3D &V) {
	auto Local = FSV.GetValue(); auto Plus = V.GetValue();
	return FSafeVector3D((Local+=Plus));
}

FORCEINLINE FSafeVector3D operator -= (FSafeVector3D &FSV, FSafeVector3D &V) {
	auto Local = FSV.GetValue(); auto Minus = V.GetValue();
	return FSafeVector3D((Local-=Minus));
}

FORCEINLINE FSafeVector3D operator % (FSafeVector3D &FSV, FSafeVector3D &V) {
	auto Local = FSV.GetValue(); auto Mod = V.GetValue();
	auto X = Local.X; auto Y = Local.Y; auto Z = Local.Z;
	auto MX = Mod.X; auto MY = Mod.Y;  auto MZ = Mod.Z;
	return FSafeVector3D(FVector(FGenericPlatformMath::Fmod(X,MX),FGenericPlatformMath::Fmod(Y,MY),FGenericPlatformMath::Fmod(Z,MZ)));
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeVector3D &FSV, const FVector &V) {
	return (FSV.GetValue()==V);
}

FORCEINLINE bool operator != (FSafeVector3D &FSV, const FVector &V) {
	return (FSV.GetValue()!=V);
}

FORCEINLINE bool operator > (FSafeVector3D &FSV, const FVector &V) {
	return ((FSV.GetValue().X>V.X)&&(FSV.GetValue().Y>V.Y)&&(FSV.GetValue().Z>V.Z));
}

FORCEINLINE bool operator < (FSafeVector3D &FSV, const FVector &V) {
	return ((FSV.GetValue().X<V.X)&&(FSV.GetValue().Y<V.Y)&&(FSV.GetValue().Z<V.Z));
}

FORCEINLINE bool operator >= (FSafeVector3D &FSV, const FVector &V) {
	return ((FSV.GetValue().X>=V.X)&&(FSV.GetValue().Y>=V.Y)&&(FSV.GetValue().Z>=V.Z));
}

FORCEINLINE bool operator <= (FSafeVector3D &FSV, const FVector &V) {
	return ((FSV.GetValue().X<=V.X)&&(FSV.GetValue().Y<=V.Y)&&(FSV.GetValue().Z<=V.Z));
}

FORCEINLINE FSafeVector3D operator + (FSafeVector3D &FSV, const FVector &V) {
	return FSafeVector3D(FSV.GetValue()+V);
}

FORCEINLINE FSafeVector3D operator - (FSafeVector3D &FSV, const FVector &V) {
	return FSafeVector3D(FSV.GetValue()-V);
}

FORCEINLINE FSafeVector3D operator * (FSafeVector3D &FSV, const FVector &V) {
	return FSafeVector3D(FSV.GetValue()*V);
}

FORCEINLINE FSafeVector3D operator / (FSafeVector3D &FSV, const FVector &V) {
	return FSafeVector3D(FSV.GetValue()/V);
}

FORCEINLINE FSafeVector3D operator += (FSafeVector3D &FSV, const FVector &V) {
	auto Local = FSV.GetValue();
	return FSafeVector3D((Local+=V));
}

FORCEINLINE FSafeVector3D operator -= (FSafeVector3D &FSV, const FVector &V) {
	auto Local = FSV.GetValue();
	return FSafeVector3D((Local-=V));
}

FORCEINLINE FSafeVector3D operator % (FSafeVector3D &FSV, const FVector &V) {
	auto Local = FSV.GetValue();
	auto X = Local.X; auto Y = Local.Y; auto Z = Local.Z;
	auto MX = V.X; auto MY = V.Y; auto MZ = V.Z;
	return FSafeVector3D(FVector(FGenericPlatformMath::Fmod(X,MX),FGenericPlatformMath::Fmod(Y,MY),FGenericPlatformMath::Fmod(Z,MZ)));
}

/* FSafe -> Native */

FORCEINLINE bool operator == (const FVector &V, FSafeVector3D &FSV) {
	return (V==FSV.GetValue());
}

FORCEINLINE bool operator != (const FVector &V, FSafeVector3D &FSV) {
	return (V!=FSV.GetValue());
}

FORCEINLINE bool operator > (const FVector &V, FSafeVector3D &FSV) {
	return ((V.X>FSV.GetValue().X)&&(V.Y>FSV.GetValue().Y)&&(V.Z>FSV.GetValue().Z));
}

FORCEINLINE bool operator < (const FVector &V, FSafeVector3D &FSV) {
	return ((V.X<FSV.GetValue().X)&&(V.Y<FSV.GetValue().Y)&&(V.Z<FSV.GetValue().Z));
}

FORCEINLINE bool operator >= (const FVector &V, FSafeVector3D &FSV) {
	return ((V.X>=FSV.GetValue().X)&&(V.Y>=FSV.GetValue().Y)&&(V.Z>=FSV.GetValue().Z));
}

FORCEINLINE bool operator <= (const FVector &V, FSafeVector3D &FSV) {
	return ((V.X<=FSV.GetValue().X)&&(V.Y<=FSV.GetValue().Y)&&(V.Z<=FSV.GetValue().Z));
}

FORCEINLINE FVector operator + (const FVector &V, FSafeVector3D &FSV) {
	return (V+FSV.GetValue());
}

FORCEINLINE FVector operator - (const FVector &V, FSafeVector3D &FSV) {
	return (V-FSV.GetValue());
}

FORCEINLINE FVector operator * (const FVector &V, FSafeVector3D &FSV) {
	return (V*FSV.GetValue());
}

FORCEINLINE FVector operator / (const FVector &V, FSafeVector3D &FSV) {
	return (V/FSV.GetValue());
}

FORCEINLINE FVector operator += (FVector &V, FSafeVector3D &FSV) {
	auto Local = FSV.GetValue();
	return (V+=Local);
}

FORCEINLINE FVector operator -= (FVector &V, FSafeVector3D &FSV) {
	auto Local = FSV.GetValue();
	return (V-=Local);
}

FORCEINLINE FVector operator % (const FVector &V, FSafeVector3D &FSV) {
	auto Local = FSV.GetValue();
	auto X = V.X; auto Y = V.Y; auto Z = V.Z;
	auto MX = Local.X; auto MY = Local.Y; auto MZ = Local.Z;
	return FVector(FGenericPlatformMath::Fmod(X,MX),FGenericPlatformMath::Fmod(Y,MY),FGenericPlatformMath::Fmod(Z,MZ));
}

// FSVector4D

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeVector4D &FSV, FSafeVector4D &V) {
	return (FSV.GetValue()==V.GetValue());
}

FORCEINLINE bool operator != (FSafeVector4D &FSV, FSafeVector4D &V) {
	return (FSV.GetValue()!=V.GetValue());
}

FORCEINLINE bool operator > (FSafeVector4D &FSV, FSafeVector4D &V) {
	return ((FSV.GetValue().X>V.GetValue().X)&&(FSV.GetValue().Y>V.GetValue().Y)&&(FSV.GetValue().Z>V.GetValue().Z)&&(FSV.GetValue().W>V.GetValue().W));
}

FORCEINLINE bool operator < (FSafeVector4D &FSV, FSafeVector4D &V) {
	return ((FSV.GetValue().X<V.GetValue().X)&&(FSV.GetValue().Y<V.GetValue().Y)&&(FSV.GetValue().Z<V.GetValue().Z)&&(FSV.GetValue().W<V.GetValue().W));
}

FORCEINLINE bool operator >= (FSafeVector4D &FSV, FSafeVector4D &V) {
	return ((FSV.GetValue().X>=V.GetValue().X)&&(FSV.GetValue().Y>=V.GetValue().Y)&&(FSV.GetValue().Z>=V.GetValue().Z)&&(FSV.GetValue().W>=V.GetValue().W));
}

FORCEINLINE bool operator <= (FSafeVector4D &FSV, FSafeVector4D &V) {
	return ((FSV.GetValue().X<=V.GetValue().X)&&(FSV.GetValue().Y<=V.GetValue().Y)&&(FSV.GetValue().Z<=V.GetValue().Z)&&(FSV.GetValue().W<=V.GetValue().W));
}

FORCEINLINE FSafeVector4D operator + (FSafeVector4D &FSV, FSafeVector4D &V) {
	auto Local = (FSV.GetValue()+V.GetValue());
	return FSafeVector4D(Local.X,Local.Y,Local.Z,Local.W);
}

FORCEINLINE FSafeVector4D operator - (FSafeVector4D &FSV, FSafeVector4D &V) {
	auto Local = (FSV.GetValue()-V.GetValue());
	return FSafeVector4D(Local.X,Local.Y,Local.Z,Local.W);
}

FORCEINLINE FSafeVector4D operator * (FSafeVector4D &FSV, FSafeVector4D &V) {
	auto Local = (FSV.GetValue()*V.GetValue());
	return FSafeVector4D(Local.X,Local.Y,Local.Z,Local.W);
}

FORCEINLINE FSafeVector4D operator / (FSafeVector4D &FSV, FSafeVector4D &V) {
	auto Local = (FSV.GetValue()/V.GetValue());
	return FSafeVector4D(Local.X,Local.Y,Local.Z,Local.W);
}

FORCEINLINE FSafeVector4D operator ++ (FSafeVector4D &FSV) {
	auto Local = FSV.GetValue();
	*&FSV = FSafeVector4D(Local.X+1,Local.Y+1,Local.Z+1,Local.W+1); return *&FSV;
}

FORCEINLINE FSafeVector4D operator -- (FSafeVector4D &FSV) {
	auto Local = FSV.GetValue();
	*&FSV = FSafeVector4D(Local.X-1,Local.Y-1,Local.Z-1,Local.W-1); return *&FSV;
}

FORCEINLINE FSafeVector4D operator += (FSafeVector4D &FSV, FSafeVector4D &V) {
	auto L = FSV.GetValue(); auto R = V.GetValue(); auto Local = (L+=R);
	return FSafeVector4D(Local.X,Local.Y,Local.Z,Local.W);
}

FORCEINLINE FSafeVector4D operator -= (FSafeVector4D &FSV, FSafeVector4D &V) {
	auto L = FSV.GetValue(); auto R = V.GetValue();
	auto Local = FVector4((L.X-=R.X),(L.Y-=R.Y),(L.Z-=R.Z),(L.W-=R.W));
	return FSafeVector4D(Local.X,Local.Y,Local.Z,Local.W);
}

FORCEINLINE FSafeVector4D operator % (FSafeVector4D &FSV, FSafeVector4D &V) {
	auto Local = FSV.GetValue(); auto Mod = V.GetValue();
	auto X = Local.X; auto Y = Local.Y; auto Z = Local.Z; auto W = Local.W;
	auto MX = Mod.X; auto MY = Mod.Y;  auto MZ = Mod.Z; auto MW = Mod.W;
	auto F4D = FVector4(FGenericPlatformMath::Fmod(X,MX),FGenericPlatformMath::Fmod(Y,MY),FGenericPlatformMath::Fmod(Z,MZ),FGenericPlatformMath::Fmod(W,MW));
	return FSafeVector4D(F4D.X,F4D.Y,F4D.Z,F4D.W);
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeVector4D &FSV, const FVector4 &V) {
	return (FSV.GetValue()==V);
}

FORCEINLINE bool operator != (FSafeVector4D &FSV, const FVector4 &V) {
	return (FSV.GetValue()!=V);
}

FORCEINLINE bool operator > (FSafeVector4D &FSV, const FVector4 &V) {
	return ((FSV.GetValue().X>V.X)&&(FSV.GetValue().Y>V.Y)&&(FSV.GetValue().Z>V.Z)&&(FSV.GetValue().W>V.W));
}

FORCEINLINE bool operator < (FSafeVector4D &FSV, const FVector4 &V) {
	return ((FSV.GetValue().X<V.X)&&(FSV.GetValue().Y<V.Y)&&(FSV.GetValue().Z<V.Z)&&(FSV.GetValue().W>V.W));
}

FORCEINLINE bool operator >= (FSafeVector4D &FSV, const FVector4 &V) {
	return ((FSV.GetValue().X>=V.X)&&(FSV.GetValue().Y>=V.Y)&&(FSV.GetValue().Z>=V.Z));
}

FORCEINLINE bool operator <= (FSafeVector4D &FSV, const FVector4 &V) {
	return ((FSV.GetValue().X<=V.X)&&(FSV.GetValue().Y<=V.Y)&&(FSV.GetValue().Z<=V.Z));
}

FORCEINLINE FSafeVector4D operator + (FSafeVector4D &FSV, const FVector4 &V) {
	auto Local = (FSV.GetValue()+V);
	return FSafeVector4D(Local.X,Local.Y,Local.Z,Local.W);
}

FORCEINLINE FSafeVector4D operator - (FSafeVector4D &FSV, const FVector4 &V) {
	auto Local = (FSV.GetValue()-V);
	return FSafeVector4D(Local.X,Local.Y,Local.Z,Local.W);
}

FORCEINLINE FSafeVector4D operator * (FSafeVector4D &FSV, const FVector4 &V) {
	auto Local = (FSV.GetValue()*V);
	return FSafeVector4D(Local.X,Local.Y,Local.Z,Local.W);
}

FORCEINLINE FSafeVector4D operator / (FSafeVector4D &FSV, const FVector4 &V) {
	auto Local = (FSV.GetValue()/V);
	return FSafeVector4D(Local.X,Local.Y,Local.Z,Local.W);
}

FORCEINLINE FSafeVector4D operator += (FSafeVector4D &FSV, const FVector4 &V) {
	auto L = FSV.GetValue(); auto Local = (L+=V);
	return FSafeVector4D(Local.X,Local.Y,Local.Z,Local.W);
}

FORCEINLINE FSafeVector4D operator -= (FSafeVector4D &FSV, const FVector4 &V) {
	auto L = FSV.GetValue();
	auto Local = FVector4((L.X-=V.X),(L.Y-=V.Y),(L.Z-=V.Z),(L.W-=V.W));
	return FSafeVector4D(Local.X,Local.Y,Local.Z,Local.W);
}

FORCEINLINE FSafeVector4D operator % (FSafeVector4D &FSV, const FVector4 &V) {
	auto Local = FSV.GetValue();
	auto MX = V.X; auto MY = V.Y;  auto MZ = V.Z; auto MW = V.W;
	auto X = Local.X; auto Y = Local.Y; auto Z = Local.Z; auto W = Local.W;
	auto F4D = FVector4(FGenericPlatformMath::Fmod(X,MX),FGenericPlatformMath::Fmod(Y,MY),FGenericPlatformMath::Fmod(Z,MZ),FGenericPlatformMath::Fmod(W,MW));
	return FSafeVector4D(F4D.X,F4D.Y,F4D.Z,F4D.W);
}

/* FSafe -> Native */

FORCEINLINE bool operator == (const FVector4 &V, FSafeVector4D &FSV) {
	return (V==FSV.GetValue());
}

FORCEINLINE bool operator != (const FVector4 &V, FSafeVector4D &FSV) {
	return (V!=FSV.GetValue());
}

FORCEINLINE bool operator > (const FVector4 &V, FSafeVector4D &FSV) {
	return ((V.X>FSV.GetValue().X)&&(V.Y>FSV.GetValue().Y)&&(V.Z>FSV.GetValue().Z)&&(V.W>FSV.GetValue().W));
}

FORCEINLINE bool operator < (const FVector4 &V, FSafeVector4D &FSV) {
	return ((V.X<FSV.GetValue().X)&&(V.Y<FSV.GetValue().Y)&&(V.Z<FSV.GetValue().Z)&&(V.W<FSV.GetValue().W));
}

FORCEINLINE bool operator >= (const FVector4 &V, FSafeVector4D &FSV) {
	return ((V.X>=FSV.GetValue().X)&&(V.Y>=FSV.GetValue().Y)&&(V.Z>=FSV.GetValue().Z)&&(V.W>=FSV.GetValue().W));
}

FORCEINLINE bool operator <= (const FVector4 &V, FSafeVector4D &FSV) {
	return ((V.X<=FSV.GetValue().X)&&(V.Y<=FSV.GetValue().Y)&&(V.Z<=FSV.GetValue().Z)&&(V.W<=FSV.GetValue().W));
}

FORCEINLINE FVector4 operator + (const FVector4 &V, FSafeVector4D &FSV) {
	return (V+FSV.GetValue());
}

FORCEINLINE FVector4 operator - (const FVector4 &V, FSafeVector4D &FSV) {
	return (V-FSV.GetValue());
}

FORCEINLINE FVector4 operator * (const FVector4 &V, FSafeVector4D &FSV) {
	return (V*FSV.GetValue());
}

FORCEINLINE FVector4 operator / (const FVector4 &V, FSafeVector4D &FSV) {
	return (V/FSV.GetValue());
}

FORCEINLINE FVector4 operator += (FVector4 &V, FSafeVector4D &FSV) {
	auto Local = FSV.GetValue();
	return (V+=Local);
}

FORCEINLINE FVector4 operator -= (FVector4 &V, FSafeVector4D &FSV) {
	auto Local = FSV.GetValue(); V = (V-Local); return Local;
}

FORCEINLINE FVector4 operator % (const FVector4 &V, FSafeVector4D &FSV) {
	auto Local = FSV.GetValue();
	auto X = V.X; auto Y = V.Y; auto Z = V.Z; auto W = V.W;
	auto MX = Local.X; auto MY = Local.Y; auto MZ = Local.Z; auto MW = Local.W;
	return FVector4(FGenericPlatformMath::Fmod(X,MX),FGenericPlatformMath::Fmod(Y,MY),FGenericPlatformMath::Fmod(Z,MZ),FGenericPlatformMath::Fmod(W,MW));
}

// FSColor

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeColor &FSC, FSafeColor &C) {
	return (FSC.GetValue()==C.GetValue());
}

FORCEINLINE bool operator != (FSafeColor &FSC, FSafeColor &C) {
	return (FSC.GetValue()!=C.GetValue());
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeColor &FSC, const FLinearColor &C) {
	return (FSC.GetValue()==C);
}

FORCEINLINE bool operator != (FSafeColor &FSC, const FLinearColor &C) {
	return (FSC.GetValue()!=C);
}

/* FSafe -> Native */

FORCEINLINE bool operator == (const FLinearColor &C, FSafeColor &FSC) {
	return (C==FSC.GetValue());
}

FORCEINLINE bool operator != (const FLinearColor &C, FSafeColor &FSC) {
	return (C!=FSC.GetValue());
}

// FSRotator

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeRotator &FSR, FSafeRotator &R) {
	return (FSR.GetValue()==R.GetValue());
}

FORCEINLINE bool operator != (FSafeRotator &FSR, FSafeRotator &R) {
	return (FSR.GetValue()!=R.GetValue());
}

FORCEINLINE bool operator > (FSafeRotator &FSR, FSafeRotator &R) {
	return ((FSR.GetValue().Pitch>R.GetValue().Pitch)&&(FSR.GetValue().Yaw>R.GetValue().Yaw)&&(FSR.GetValue().Roll>R.GetValue().Roll));
}

FORCEINLINE bool operator < (FSafeRotator &FSR, FSafeRotator &R) {
	return ((FSR.GetValue().Pitch<R.GetValue().Pitch)&&(FSR.GetValue().Yaw<R.GetValue().Yaw)&&(FSR.GetValue().Roll<R.GetValue().Roll));
}

FORCEINLINE bool operator >= (FSafeRotator &FSR, FSafeRotator &R) {
	return ((FSR.GetValue().Pitch>=R.GetValue().Pitch)&&(FSR.GetValue().Yaw>=R.GetValue().Yaw)&&(FSR.GetValue().Roll>=R.GetValue().Roll));
}

FORCEINLINE bool operator <= (FSafeRotator &FSR, FSafeRotator &R) {
	return ((FSR.GetValue().Pitch<=R.GetValue().Pitch)&&(FSR.GetValue().Yaw<=R.GetValue().Yaw)&&(FSR.GetValue().Roll<=R.GetValue().Roll));
}

FORCEINLINE FSafeRotator operator + (FSafeRotator &FSR, FSafeRotator &R) {
	return FSafeRotator(FSR.GetValue()+R.GetValue());
}

FORCEINLINE FSafeRotator operator - (FSafeRotator &FSR, FSafeRotator &R) {
	return FSafeRotator(FSR.GetValue()-R.GetValue());
}

FORCEINLINE FSafeRotator operator * (FSafeRotator &FSR, FSafeRotator &R) {
	auto A = FSR.GetValue(); auto B = R.GetValue();
	auto Rotator = FRotator(A.Pitch*B.Pitch,A.Yaw*B.Yaw,A.Roll*B.Roll);
	return FSafeRotator(Rotator);
}

FORCEINLINE FSafeRotator operator / (FSafeRotator &FSR, FSafeRotator &R) {
	auto A = FSR.GetValue(); auto B = R.GetValue();
	auto Rotator = FRotator(A.Pitch/B.Pitch,A.Yaw/B.Yaw,A.Roll/B.Roll);
	return FSafeRotator(Rotator);
}

FORCEINLINE FSafeRotator operator ++ (FSafeRotator &FSR) {
	auto Local = FSR.GetValue();
	*&FSR = FSafeRotator(Local.Pitch+1,Local.Yaw+1,Local.Roll+1); return *&FSR;
}

FORCEINLINE FSafeRotator operator -- (FSafeRotator &FSR) {
	auto Local = FSR.GetValue();
	*&FSR = FSafeRotator(Local.Pitch-1,Local.Yaw-1,Local.Roll-1); return *&FSR;
}

FORCEINLINE FSafeRotator operator += (FSafeRotator &FSR, FSafeRotator &R) {
	auto Local = FSR.GetValue(); auto Plus = R.GetValue();
	return FSafeRotator((Local+=Plus));
}

FORCEINLINE FSafeRotator operator -= (FSafeRotator &FSR, FSafeRotator &R) {
	auto Local = FSR.GetValue(); auto Minus = R.GetValue();
	return FSafeRotator((Local-=Minus));
}

FORCEINLINE FSafeRotator operator % (FSafeRotator &FSR, FSafeRotator &R) {
	auto Local = FSR.GetValue(); auto Mod = R.GetValue();
	auto X = Local.Pitch; auto Y = Local.Yaw; auto Z = Local.Roll;
	auto MX = Mod.Pitch; auto MY = Mod.Yaw;  auto MZ = Mod.Roll;
	return FSafeRotator(FRotator(FGenericPlatformMath::Fmod(X,MX),FGenericPlatformMath::Fmod(Y,MY),FGenericPlatformMath::Fmod(Z,MZ)));
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeRotator &FSR, const FRotator &R) {
	return (FSR.GetValue()==R);
}

FORCEINLINE bool operator != (FSafeRotator &FSR, const FRotator &R) {
	return (FSR.GetValue()!=R);
}

FORCEINLINE bool operator > (FSafeRotator &FSR, const FRotator &R) {
	return ((FSR.GetValue().Pitch>R.Pitch)&&(FSR.GetValue().Yaw>R.Yaw)&&(FSR.GetValue().Roll>R.Roll));
}

FORCEINLINE bool operator < (FSafeRotator &FSR, const FRotator &R) {
	return ((FSR.GetValue().Pitch<R.Pitch)&&(FSR.GetValue().Yaw<R.Yaw)&&(FSR.GetValue().Roll<R.Roll));
}

FORCEINLINE bool operator >= (FSafeRotator &FSR, const FRotator &R) {
	return ((FSR.GetValue().Pitch>=R.Pitch)&&(FSR.GetValue().Yaw>=R.Yaw)&&(FSR.GetValue().Roll>=R.Roll));
}

FORCEINLINE bool operator <= (FSafeRotator &FSR, const FRotator &R) {
	return ((FSR.GetValue().Pitch<=R.Pitch)&&(FSR.GetValue().Yaw<=R.Yaw)&&(FSR.GetValue().Roll<=R.Roll));
}

FORCEINLINE FSafeRotator operator + (FSafeRotator &FSR, const FRotator &R) {
	return FSafeRotator(FSR.GetValue()+R);
}

FORCEINLINE FSafeRotator operator - (FSafeRotator &FSR, const FRotator &R) {
	return FSafeRotator(FSR.GetValue()-R);
}

FORCEINLINE FSafeRotator operator * (FSafeRotator &FSR, const FRotator &R) {
	auto Local = FSR.GetValue();
	auto Rotator = FRotator(Local.Pitch*R.Pitch,Local.Yaw*R.Yaw,Local.Roll*R.Roll);
	return FSafeRotator(Rotator);
}

FORCEINLINE FSafeRotator operator / (FSafeRotator &FSR, const FRotator &R) {
	auto Local = FSR.GetValue();
	auto Rotator = FRotator(Local.Pitch/R.Pitch,Local.Yaw/R.Yaw,Local.Roll/R.Roll);
	return FSafeRotator(Rotator);
}

FORCEINLINE FSafeRotator operator += (FSafeRotator &FSR, const FRotator &R) {
	auto Local = FSR.GetValue();
	return FSafeRotator((Local+=R));
}

FORCEINLINE FSafeRotator operator -= (FSafeRotator &FSR, const FRotator &R) {
	auto Local = FSR.GetValue();
	return FSafeRotator((Local-=R));
}

FORCEINLINE FSafeRotator operator % (FSafeRotator &FSR, const FRotator &R) {
	auto Local = FSR.GetValue();
	auto X = Local.Pitch; auto Y = Local.Yaw; auto Z = Local.Roll;
	auto MX = R.Pitch; auto MY = R.Yaw; auto MZ = R.Roll;
	return FSafeRotator(FRotator(FGenericPlatformMath::Fmod(X,MX),FGenericPlatformMath::Fmod(Y,MY),FGenericPlatformMath::Fmod(Z,MZ)));
}

/* FSafe -> Native */

FORCEINLINE bool operator == (const FRotator &R, FSafeRotator &FSR) {
	return (R==FSR.GetValue());
}

FORCEINLINE bool operator != (const FRotator &R, FSafeRotator &FSR) {
	return (R!=FSR.GetValue());
}

FORCEINLINE bool operator > (const FRotator &R, FSafeRotator &FSR) {
	return ((R.Pitch>FSR.GetValue().Pitch)&&(R.Yaw>FSR.GetValue().Yaw)&&(R.Roll>FSR.GetValue().Roll));
}

FORCEINLINE bool operator < (const FRotator &R, FSafeRotator &FSR) {
	return ((R.Pitch<FSR.GetValue().Pitch)&&(R.Yaw<FSR.GetValue().Yaw)&&(R.Roll<FSR.GetValue().Roll));
}

FORCEINLINE bool operator >= (const FRotator &R, FSafeRotator &FSR) {
	return ((R.Pitch>=FSR.GetValue().Pitch)&&(R.Yaw>=FSR.GetValue().Yaw)&&(R.Roll>=FSR.GetValue().Roll));
}

FORCEINLINE bool operator <= (const FRotator &R, FSafeRotator &FSR) {
	return ((R.Pitch<=FSR.GetValue().Pitch)&&(R.Yaw<=FSR.GetValue().Yaw)&&(R.Roll<=FSR.GetValue().Roll));
}

FORCEINLINE FRotator operator + (const FRotator &R, FSafeRotator &FSR) {
	return (R+FSR.GetValue());
}

FORCEINLINE FRotator operator - (const FRotator &R, FSafeRotator &FSR) {
	return (R-FSR.GetValue());
}

FORCEINLINE FRotator operator * (const FRotator &R, FSafeRotator &FSR) {
	auto Local = FSR.GetValue();
	return FRotator(R.Pitch*Local.Pitch,R.Yaw*Local.Yaw,R.Roll*Local.Roll);
}

FORCEINLINE FRotator operator / (const FRotator &R, FSafeRotator &FSR) {
	auto Local = FSR.GetValue();
	return FRotator(R.Pitch/Local.Pitch,R.Yaw/Local.Yaw,R.Roll/Local.Roll);
}

FORCEINLINE FRotator operator += (FRotator &R, FSafeRotator &FSR) {
	auto Local = FSR.GetValue();
	return (R+=Local);
}

FORCEINLINE FRotator operator -= (FRotator &R, FSafeRotator &FSR) {
	auto Local = FSR.GetValue();
	return (R-=Local);
}

FORCEINLINE FRotator operator % (const FRotator &R, FSafeRotator &FSR) {
	auto Local = FSR.GetValue();
	auto X = R.Pitch; auto Y = R.Yaw; auto Z = R.Roll;
	auto MX = Local.Pitch; auto MY = Local.Yaw; auto MZ = Local.Roll;
	return FRotator(FGenericPlatformMath::Fmod(X,MX),FGenericPlatformMath::Fmod(Y,MY),FGenericPlatformMath::Fmod(Z,MZ));
}

// FSTransform

/* FSafe -> FSafe */

FORCEINLINE bool operator == (FSafeTransform &FST, FSafeTransform &T) {
	auto Local = FST.GetValue(); auto To = T.GetValue();
	return ((Local.GetLocation()==To.GetLocation())&&(Local.GetRotation()==To.GetRotation())&&(Local.GetScale3D()==To.GetScale3D()));
}

FORCEINLINE bool operator != (FSafeTransform &FST, FSafeTransform &T) {
	auto Local = FST.GetValue(); auto To = T.GetValue();
	return ((Local.GetLocation()!=To.GetLocation())&&(Local.GetRotation()!=To.GetRotation())&&(Local.GetScale3D()!=To.GetScale3D()));
}

/* Native -> FSafe */

FORCEINLINE bool operator == (FSafeTransform &FST, const FTransform &T) {
	auto Local = FST.GetValue();
	return ((Local.GetLocation()==T.GetLocation())&&(Local.GetRotation()==T.GetRotation())&&(Local.GetScale3D()==T.GetScale3D()));
}

FORCEINLINE bool operator != (FSafeTransform &FST, const FTransform &T) {
	auto Local = FST.GetValue();
	return ((Local.GetLocation()!=T.GetLocation())&&(Local.GetRotation()!=T.GetRotation())&&(Local.GetScale3D()!=T.GetScale3D()));
}

/* FSafe -> Native */

FORCEINLINE bool operator == (const FTransform &T, FSafeTransform &FST) {
	auto Local = FST.GetValue();
	return ((T.GetLocation()==Local.GetLocation())&&(T.GetRotation()==Local.GetRotation())&&(T.GetScale3D()==Local.GetScale3D()));
}

FORCEINLINE bool operator != (const FTransform &T, FSafeTransform &FST) {
	auto Local = FST.GetValue();
	return ((T.GetLocation()!=Local.GetLocation())&&(T.GetRotation()!=Local.GetRotation())&&(T.GetScale3D()!=Local.GetScale3D()));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if PLATFORM_WINDOWS
#pragma endregion OPERATORS
#pragma region GAME INSTANCE
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Utility

static FORCEINLINE int32 GetFullScreenMode() {
	static const auto ScreenMode = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FullScreenMode"));
	return ScreenMode->GetValueOnGameThread();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Game Instance:: Runs anti-cheat, anti-debugging methods

/// {S}: Safe Game Instance; Secure-Client is a C++ Plugin designed to provide anti-cheat capability to your games.
/// Other custom Game Instance classes, when used, should have this class as a parent for effective use.
UCLASS(ClassGroup = Security, Category = "Security", hideCategories = (Activation, Socket, Tick, Thumbnail), meta = (BlueprintType, DisplayName = "{S} Safe Game Instance", ShortTooltip = "{S} Safe Game Instance Class. Check documentation for more information."))
class SCUE4_API USafeGameInstance : public UGameInstance {
	GENERATED_BODY()
public:
	/* Sets visibility of Game-Guard console window.
	Console is only visible in Editor Mode, on packaged games it's always hidden. */
	UPROPERTY(Category = "Security", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool HideGameGuard = true;
	//
	/* To increase defenses against memory readers, uncheck this option when you're ready to
	ship final standalone game build. If disabled, option won't allow Debuggers attach to the game process. */
	UPROPERTY(Category = "Security", EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool AllowDebugging = true;
	//
	/** Interval, in seconds, for Internal Process Scanner to be automatically invoked. */
	UPROPERTY(Category = "Security", EditAnywhere, meta = (AllowPrivateAccess = "true", ClampMin = "10", ClampMax = "1000", UIMin = "10", UIMax = "1000"))
	uint32 ScannerInterval = 30.f;
	//
	/* Sets visibility of Game-Guard console window.
	Console is only visible in Editor Mode, on packaged games it's always hidden. */
	UFUNCTION(Category = "Security", BlueprintCallable, meta = (DisplayName = "{S}:: Hide Game-Guard Console", Keywords = "Security Guard"))
	void HideGameGuardConsole(bool Set);
	//
	//
	FProcHandle GuardProcess;
	const TCHAR* Guardx86 = TEXT("SCUE4x86.exe");
	const TCHAR* Guardx64 = TEXT("SCUE4x64.exe");
	const TCHAR* Editor = TEXT("EDITOR");
	const TCHAR* Game = FApp::GetProjectName();
	//
	FTimerHandle THInvokeGuard;
	FTimerHandle THScanner;
	uint32 GuardPID = 0;
	//
	//
	virtual void Init() override {
		#if PLATFORM_WINDOWS
		const FTimerDelegate TimerScanProcesses = FTimerDelegate::CreateUObject(this,&USafeGameInstance::ScanProcesses);
		const FTimerDelegate TimerInvokeGuard = FTimerDelegate::CreateUObject(this,&USafeGameInstance::GameGuard);
		GetTimerManager().SetTimer(THScanner,TimerScanProcesses,ScannerInterval,true);
		GetTimerManager().SetTimer(THInvokeGuard,TimerInvokeGuard,5,true);
		InvokeGuard();
		#endif
		//
		Super::Init();
	}
	//
	virtual void Shutdown() override {
		#if PLATFORM_WINDOWS
		FPlatformProcess::TerminateProc(GuardProcess);
		#endif
		Super::Shutdown();
	}
	//
	//
	/* ~Le Vsauce is real cool.
	This external process scanner is just distraction. */
	void InvokeGuard() {
	#if PLATFORM_WINDOWS
		#if WITH_EDITOR
		GuardProcess = FPlatformProcess::CreateProc(*FPaths::Combine(*FPaths::EnginePluginsDir(),TEXT("Marketplace/SCUE4/Source/ThirdParty/x64"),Guardx64),Editor,false,HideGameGuard,HideGameGuard,&GuardPID,0,nullptr,nullptr);
		#else
			#if !PLATFORM_64BITS
			if (!FPaths::FileExists(FPaths::Combine(*FPaths::EnginePluginsDir(),TEXT("Marketplace/SCUE4/Source/ThirdParty/x86/"),Guardx86))) {FGenericPlatformMisc::RequestExit(false);}
			GuardProcess = FPlatformProcess::CreateProc(*FPaths::Combine(*FPaths::EnginePluginsDir(),TEXT("Marketplace/SCUE4/Source/ThirdParty/x86/"),Guardx86),Game,false,true,true,&GuardPID,0,nullptr,nullptr);
			#else
			if (!FPaths::FileExists(FPaths::Combine(*FPaths::EnginePluginsDir(),TEXT("Marketplace/SCUE4/Source/ThirdParty/x64/"),Guardx64))) {FGenericPlatformMisc::RequestExit(false);}
			GuardProcess = FPlatformProcess::CreateProc(*FPaths::Combine(*FPaths::EnginePluginsDir(),TEXT("Marketplace/SCUE4/Source/ThirdParty/x64/"),Guardx64),Game,false,true,true,&GuardPID,0,nullptr,nullptr);
			#endif
		#endif
	#endif
	}
	//
	/* ~Le Vsauce is real cool. */
	void GameGuard() {
	#if PLATFORM_WINDOWS && !WITH_EDITOR
		if (!GuardProcess.IsValid() || !FPlatformProcess::IsProcRunning(GuardProcess)) {InvokeGuard();}
		if (!AllowDebugging) {if (IsDebuggerPresent() || HasDebugger() || HasThreat()) {FGenericPlatformMisc::RequestExit(false);}}
	#endif
	}
	//
	/* ~Le Vsauce is real cool. */
	bool IsDebuggerPresent() {
	#if PLATFORM_WINDOWS && !WITH_EDITOR
		HINSTANCE Kernel = LoadLibraryEx(TEXT("kernel32.dll"),NULL,0);
		FARPROC IDebuggerPresent = GetProcAddress(Kernel,"IsDebuggerPresent");
		if(IDebuggerPresent && IDebuggerPresent()) {FreeLibrary(Kernel); return true;}
		FreeLibrary(Kernel); return false;
	#else
		return false;
	#endif
	}
	//
	/* ~Le Vsauce is real cool. */
	FORCEINLINE bool HasDebugger() {
	#if PLATFORM_WINDOWS
		#if !WITH_EDITOR && PLATFORM_32BITS
			__try {
				__asm __emit 0xF3
				__asm __emit 0x64
				__asm __emit 0xF1
			} __except(EXCEPTION_EXECUTE_HANDLER) {
				return false;
			} return true;
		#else
			return false;
		#endif
	#else
		return false;
	#endif
	}
	//
	/* ~Le Vsauce is real cool. */
	FORCEINLINE bool HasThreat() {
	#if PLATFORM_WINDOWS
		#if !WITH_EDITOR
		if (OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE,false,L"CEHYPERSCANSETTINGS")!=0) {return true;}
		if (GetFullScreenMode()==0) {return false;}
		auto HND = FindWindowA((LPCSTR)"WinDbgFrameClass",NULL); if (HND) {return true;}
		HND = FindWindowA((LPCSTR)"WinDbgFrameClass",NULL); if (HND) {return true;}
		HND = FindWindowA((LPCSTR)"OLLYDBG",NULL); if (HND) {return true;}
		HND = FindWindowA((LPCSTR)"Window",NULL); if (HND) {return true;}
		return false;
		#endif
		return false;
	#else
		return false;
	#endif
	}
	//
	/* ~Le Vsauce is real cool. */
	void ScanProcesses() {
	#if PLATFORM_WINDOWS
		#if !WITH_EDITOR && !WITH_SERVER_CODE
		if (GetFullScreenMode()>0) {FSCUE4_Enumerate();}
		#endif
	#endif
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if PLATFORM_WINDOWS
#pragma endregion GAME INSTANCE
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////