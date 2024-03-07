// Copyright Lede Studios. All Rights Reserved.

// ReSharper disable CppExpressionWithoutSideEffects
// ReSharper disable CppPassValueParameterByConstReference
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppUE4BlueprintCallableFunctionMayBeStatic

#include "LS_EOSSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void ULS_EOSSubsystem::ReadTitleFile(const FString& FileName, FLSEOS_ReadTitleFileComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, FString());
		return;
	}

	const IOnlineTitleFilePtr Title = Subsystem->GetTitleFileInterface();
	if (Title == nullptr)
	{
		OnDone.ExecuteIfBound(false, FString());
		return;
	}

	Title->AddOnReadFileCompleteDelegate_Handle(FOnReadFileComplete::FDelegate::CreateUObject(
		this,
		&ULS_EOSSubsystem::HandleReadTitleFileComplete,
		OnDone));

	Title->ReadFile(FileName);
}

void ULS_EOSSubsystem::HandleReadTitleFileComplete(const bool WasSuccessful, const FString& FileName,
	FLSEOS_ReadTitleFileComplete OnDone)
{
	OnDone.ExecuteIfBound(WasSuccessful, FileName);
}

void ULS_EOSSubsystem::EnumerateTitleFiles(FLSEOS_EnumerateTitleFilesComplete OnDone)
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FString>(), TEXT("Subsystem is null"));
		return;
	}

	const IOnlineTitleFilePtr Title = Subsystem->GetTitleFileInterface();
	if (Title == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FString>(), TEXT("Unable to get title interface from subsystem"));
		return;
	}

	Title->AddOnEnumerateFilesCompleteDelegate_Handle(FOnEnumerateFilesComplete::FDelegate::CreateUObject(
		this,
		&ULS_EOSSubsystem::HandleEnumerateTitleFilesComplete,
		OnDone));

	// EOS does not support anything other than the default value being passed in here (unless you use the EOS
	// specific types for pagination).
	const FPagedQuery PagedQuery = FPagedQuery();
	Title->EnumerateFiles(PagedQuery);
}

void ULS_EOSSubsystem::HandleEnumerateTitleFilesComplete(const bool WasSuccessful, const FString& Error,
	FLSEOS_EnumerateTitleFilesComplete OnDone) const
{
	if (WasSuccessful == false)
	{
		OnDone.ExecuteIfBound(false, TArray<FString>(), Error);
		return;
	}

	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FString>(), TEXT("Subsystem is null"));
		return;
	}

	const IOnlineTitleFilePtr Title = Subsystem->GetTitleFileInterface();
	if (Title == nullptr)
	{
		OnDone.ExecuteIfBound(false, TArray<FString>(), TEXT("Unable to get title interface from subsystem"));
		return;
	}

	TArray<FCloudFileHeader> Files;
	Title->GetFileList(Files);
	TArray<FString> FileNames;
	FileNames.Reserve(Files.Num());

	for (auto &&It : Files)
	{
		FileNames.Add(It.FileName);
	}

	OnDone.ExecuteIfBound(true, FileNames, FString());
}

FString ULS_EOSSubsystem::GetFileContents(const FString& FileName) const
{
	const IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem == nullptr)
	{
		return FString();
	}

	const IOnlineTitleFilePtr Title = Subsystem->GetTitleFileInterface();
	if (Title == nullptr)
	{
		return FString();
	}

	TArray<uint8> FileContents;
	Title->GetFileContents(FileName, FileContents);

	const ULSEOS_FileData *FileData = ULSEOS_FileData::FromNative(FileContents);
	return ReadFileDataAsString(FileData);
}

FString ULS_EOSSubsystem::ReadFileDataAsString(const ULSEOS_FileData* FileData)
{
	if (!IsValid(FileData))
	{
		return TEXT("");
	}
	const FUTF8ToTCHAR Data(reinterpret_cast<const ANSICHAR*>(FileData->GetData().GetData()), FileData->GetData().Num());
	return FString(Data.Length(), Data.Get());
}


