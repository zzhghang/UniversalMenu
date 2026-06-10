#pragma once


UENUM(BlueprintType)
enum class EConfirmScreenType : uint8
{
	Ok,        
	YesNo,     
	OkCancel,  
	UnKnown UMETA(Hidden) 
};


UENUM(BlueprintType)
enum class EConfirmScreenButtonType : uint8
{
	Confirmed, 
	Cancelled, 
	Closed,    
	UnKnown UMETA(Hidden) 

};


UENUM(BlueprintType)
enum class EOptionsListDataModifyReason : uint8
{
	DirectlyModified,
	DependencyModified,
	ResetToDefault,
	UnKnown UMETA(Hidden) 

};