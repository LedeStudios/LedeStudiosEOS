// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"

void ULS_EOSSubsystem::WriteUserFile(const FString& FileName, TArray<uint8>& FileData,
                                     FLSEOS_WriteUserFileComplete OnDone)
{
	// note: this implementation expects the data to already be serialized before calling it.
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	if (Identity->GetUniquePlayerId(0).IsValid() == false)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	const IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
	if (UserCloud == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	UserCloud->AddOnWriteUserFileCompleteDelegate_Handle(FOnWriteUserFileComplete::FDelegate::CreateUObject(
		this,
		&ULS_EOSSubsystem::HandleWriteUserFileComplete,
		OnDone));

	const int32 TotalFileSize = FileData.Num();
	UserCloud->AddOnWriteUserFileProgressDelegate_Handle(FOnWriteUserFileProgress::FDelegate::CreateUObject(
		this,
		&ULS_EOSSubsystem::HandleWriteUserFileProgress,
		TotalFileSize));

	UserCloud->AddOnWriteUserFileCanceledDelegate_Handle(
		FOnWriteUserFileCanceled::FDelegate::CreateUObject(this, &ULS_EOSSubsystem::HandleWriteUserFileCancelled));

	UserCloud->WriteUserFile(*Identity->GetUniquePlayerId(0), FileName, FileData);
}

void ULS_EOSSubsystem::WriteUserFileFromString(const FString& FileName, const FString& FileData,
	FLSEOS_WriteUserFileComplete OnDone)
{
	if (FileData.IsEmpty())
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	// note: this implementation expects the data to already be in a string format
	const FTCHARToUTF8 Data(*FileData);
	TArray<uint8> FileDataArray((uint8 *)Data.Get(), Data.Length());

	WriteUserFile(FileName, FileDataArray, OnDone);
}

void ULS_EOSSubsystem::WriteUserFileFromSaveGame(const FString& FileName, USaveGame* SaveGame,
	FLSEOS_WriteUserFileComplete OnDone)
{
	TArray<uint8> FileData;
	UGameplayStatics::SaveGameToMemory(SaveGame, FileData);

	WriteUserFile(FileName, FileData, OnDone);
}

void ULS_EOSSubsystem::ReadUserFileAsString(const FString& FileName, FLSEOS_ReadUserFileAsStringComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, FString());
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity == nullptr)
	{
		OnDone.ExecuteIfBound(false, FString());
		return;
	}

	if (Identity->GetUniquePlayerId(0).IsValid() == false)
	{
		OnDone.ExecuteIfBound(false, FString());
		return;
	}

	IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
	if (UserCloud == nullptr)
	{
		OnDone.ExecuteIfBound(false, FString());
		return;
	}

	UserCloud->AddOnReadUserFileCompleteDelegate_Handle(FOnReadUserFileComplete::FDelegate::CreateUObject(
		this,
		&ULS_EOSSubsystem::HandleReadUserFileComplete,
		OnDone));

	UserCloud->ReadUserFile(*Identity->GetUniquePlayerId(0), FileName);
}

void ULS_EOSSubsystem::ReadUserFileAsSaveGame(const FString& FileName, FLSEOS_ReadUserFileAsSaveGameComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, nullptr);
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity == nullptr)
	{
		OnDone.ExecuteIfBound(false, nullptr);
		return;
	}

	if (Identity->GetUniquePlayerId(0).IsValid() == false)
	{
		OnDone.ExecuteIfBound(false, nullptr);
		return;
	}

	const IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
	if (UserCloud == nullptr)
	{
		OnDone.ExecuteIfBound(false, nullptr);
		return;
	}

	UserCloud->AddOnReadUserFileCompleteDelegate_Handle(FOnReadUserFileComplete::FDelegate::CreateUObject(
		this,
		&ULS_EOSSubsystem::HandleReadUserFileComplete,
		OnDone));

	UserCloud->ReadUserFile(*Identity->GetUniquePlayerId(0), FileName);
}

void ULS_EOSSubsystem::EnumerateUserFiles(FLSEOS_EnumerateUserFilesComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_CloudFileData>());
		return;
	}

	const IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (Identity == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_CloudFileData>());
		return;
	}

	if (Identity->GetUniquePlayerId(0).IsValid() == false)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_CloudFileData>());
		return;
	}

	const IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
	if (UserCloud == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_CloudFileData>());
		return;
	}

	UserCloud->AddOnEnumerateUserFilesCompleteDelegate_Handle(FOnEnumerateUserFilesComplete::FDelegate::CreateUObject(
		this,
		&ULS_EOSSubsystem::HandleEnumerateUserFilesComplete,
		OnDone));

	UserCloud->EnumerateUserFiles(*Identity->GetUniquePlayerId(0));
}

void ULS_EOSSubsystem::HandleWriteUserFileComplete(const bool WasSuccessful, const FUniqueNetId& UserId,
	const FString& FileName, FLSEOS_WriteUserFileComplete OnDone)
{
	OnDone.ExecuteIfBound(WasSuccessful);
}

void ULS_EOSSubsystem::HandleWriteUserFileCancelled(bool WasSuccessful, const FUniqueNetId& UserId,
	const FString& FileName) const
{
	ExampleCPPSubsystemWriteUserFileCancelled.Broadcast(WasSuccessful);
}

void ULS_EOSSubsystem::HandleWriteUserFileProgress(const int32 BytesWritten, const FUniqueNetId& UserId,
	const FString& FileName, const int32 TotalFileSize) const
{
	if (TotalFileSize > 0)
	{
		const float PercentageTransferred = BytesWritten / TotalFileSize;
		this->ExampleCPPSubsystemWriteUserFileProgress.Broadcast(PercentageTransferred, FileName);
	}
}

void ULS_EOSSubsystem::HandleReadUserFileComplete(bool WasSuccessful, const FUniqueNetId& UserId,
	const FString& FileName, FLSEOS_ReadUserFileAsStringComplete OnDone) const
{
	if (WasSuccessful == false)
	{
		OnDone.ExecuteIfBound(false, FString());
		return;
	}

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, FString());
		return;
	}

	const IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
	if (UserCloud == nullptr)
	{
		OnDone.ExecuteIfBound(false, FString());
		return;
	}

	TArray<uint8> FileContents;
	if (UserCloud->GetFileContents(UserId, FileName, FileContents))
	{
		const FUTF8ToTCHAR Data(reinterpret_cast<const ANSICHAR*>(FileContents.GetData()), FileContents.Num());
		OnDone.ExecuteIfBound(true, FString(Data.Length(), Data.Get()));
		return;
	}

	OnDone.ExecuteIfBound(false, FString());
}

void ULS_EOSSubsystem::HandleReadUserFileComplete(const bool WasSuccessful, const FUniqueNetId& UserId,
	const FString& FileName, FLSEOS_ReadUserFileAsSaveGameComplete OnDone) const
{
	if (WasSuccessful == false)
	{
		OnDone.ExecuteIfBound(false, nullptr);
		return;
	}

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, nullptr);
		return;
	}

	const IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
	if (UserCloud == nullptr)
	{
		OnDone.ExecuteIfBound(false, nullptr);
		return;
	}

	TArray<uint8> FileContents;
	if (UserCloud->GetFileContents(UserId, FileName, FileContents))
	{
		USaveGame *Data = UGameplayStatics::LoadGameFromMemory(FileContents);
		OnDone.ExecuteIfBound(true, Data);
		return;
	}

	OnDone.ExecuteIfBound(false, nullptr);
}

void ULS_EOSSubsystem::HandleEnumerateUserFilesComplete(const bool WasSuccessful, const FUniqueNetId& UserId,
	FLSEOS_EnumerateUserFilesComplete OnDone) const
{
	if (WasSuccessful == false)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_CloudFileData>());
		return;
	}

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_CloudFileData>());
		return;
	}

	const IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
	if (UserCloud == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FLSEOS_CloudFileData>());
		return;
	}

	TArray<FCloudFileHeader> Files;
	UserCloud->GetUserFileList(UserId, Files);
	TArray<FLSEOS_CloudFileData> FileData;
	FileData.Reserve(Files.Num() - 1);
	for (auto &&It : Files)
	{
		FLSEOS_CloudFileData Data = FLSEOS_CloudFileData::FromNative(It);
		FileData.Add(Data);
	}

	OnDone.ExecuteIfBound(true, FileData);
}


